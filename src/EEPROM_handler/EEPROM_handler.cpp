/*
Tim Kieboom 1025003
*/
#include "EEPROM_handler.h"
#include "../lib/TKardunio/TKarduino.h"
#include "InputBuffer/InputBuffer.h"
#include <utils/utils.h>
inline uint8_t charSpanTo_uint8(ConstSpan<char> str, /*out*/bool& success);
bool doesNameExist(const char* name, /*out*/bool& exist);
inline int getFATEntry_index(int index);
bool readFATEntry(int index, FAT& fat);
bool writeFATEntry(const FAT& fat);
inline uint8_t getFATLength();

static EEPROM_Data eepromData = EEPROM_Data(
  getFATEntry_index(getFATLength()-1)
);

bool commandFunc_temp(InputBuffer &input) {
  uint8_t len = 0;
  EEPROM.get(EEPROM_HEADER_FAT_LEN_INDEX, /*out*/len);
  Serial.print(F("(debug only) current FAT.len: ")); 
  Serial.println(len);
  
  return true;
}

bool commandFunc_store(InputBuffer &input)
{
  static int processId = 0;
  static FAT fat = FAT();

  if (processId == 0)
  {
    if (!checkArguments(input, 3))
      return true;

    fat.address = eepromData.lastAddress + 1;

    ConstSpan<char> nameSpan = getToken(input, 1);
    if (nameSpan.len() > 11) {
      Serial.println(F("!!error!! name of file is to big file names can not be bigger then 11 symbools"));
      processId = 0;
      fat = FAT();
      return true;
    }
    fat.fileName = (char *)nameSpan.copy_asCstr();

    bool parseSuccess = false;
    fat.size = charSpanTo_uint8(getToken(input, 2), /*out*/ parseSuccess);

    if (!parseSuccess) {
      Serial.println(F("!!error!! while trying to parse number in store command: uint8 overflow caught (number has an invalid symbools or is to big)"));
      processId = 0;
      fat = FAT();
      return true;
    }

    processId++;
    return false;
  }

  bool exists = false;
  bool isDone = doesNameExist(fat.fileName, /*out*/ exists);

  if (isDone)
  {
    if (!exists)
    {
      bool isSuccess = writeFATEntry(fat);
      processId = 0;
      fat = FAT();
      if(isSuccess)
        Serial.println(F("successfully stored new file"));
      return true;
    }

    Serial.println(F("!!error!! fileName already exists"));
    processId = 0;
    fat = FAT();
    return true;
  }

  return false;
}

bool commandFunc_clearEEPROM(InputBuffer& input) {

  if(!checkArguments(input, 0))
    return true;

  Serial.flush();
  Serial.print(F("this will DELETE ALL YOUR FILES are you sure (y)/(n) >> "));

  while(Serial.available() <= 0) {}
  
  char firstChar = Serial.read();
  Serial.println();
  if(firstChar != 'y') {
    Serial.println("clear cancelled");
    return true;
  }

  for(uint16_t i = 0; i < EEPROM.length(); i++)
    EEPROM.write(i, 0);

  Serial.println("disk cleared");
  eepromData.lastAddress = getFATEntry_index(0);
  Serial.flush();
  return true;
}

bool doesNameExist(const char* name, /*out*/bool& exist) {
  static uint8_t i = 0;
  exist = false;
  
  uint8_t len = getFATLength();
  if(len == 0)
    return true;
  

  if(i >= len) {
    i = 0;
    return true;
  }

  FAT fat;
  EEPROM.get(getFATEntry_index(i), /*out*/fat);
  if(strEquals(name, fat.fileName)) {
    i = 0;
    exist = true;
    return true;
  }

  i++;
  return false;
}

bool writeFATEntry(const FAT& fat) {
  uint8_t FATlen = getFATLength();

  if(FATlen+1 > FAT_MAX_NUM_FILES) {
    Serial.println(F("!!error!! can not add a new file to system because system is already full"));
    return false;
  }

  int memIndex = getFATEntry_index(++eepromData.lastAddress);
  EEPROM.put(memIndex, fat);
  EEPROM.put(EEPROM_HEADER_FAT_LEN_INDEX, FATlen+1);

  IF_DEBUG(
    Serial.print(F("(debug only) FAT size: ")); Serial.println(getFATLength());
  )
  return true;
}

bool readFATEntry(int index, FAT& fat) {
  if(index >= getFATLength())
    return false;

  


  return true;
}

inline int getFATEntry_index(int index) {
  constexpr int firstFAT = EEPROM_HEADER_SIZE;
  return firstFAT + (sizeof(FAT) * index);
}

inline uint8_t getFATLength() {
  uint8_t len = 0;
  EEPROM.get(EEPROM_HEADER_FAT_LEN_INDEX, /*out*/len);
  return len;
}

inline bool isNumber(int8_t currentDecimal) {
  return currentDecimal > -1 && currentDecimal < 10;
}

inline uint8_t charSpanTo_uint8(ConstSpan<char> str, /*out*/bool& success) {
  uint16_t number = 0;
  success = false;

  for(size_t i = 0; i < str.len(); i++) {
    number *= 10;
    int8_t currentDecimal = (int8_t)(str[i] - '0');
    ASSERT_PRINT(isNumber(currentDecimal), str.copy_asCstr());
    if(!isNumber(currentDecimal))
      return 0;

    number += currentDecimal;
  }

  if(number < UINT8_MAX+1)
    success = true;

  return (uint8_t)number;
}