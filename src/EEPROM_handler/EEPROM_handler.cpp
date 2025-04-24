/*
Tim Kieboom 1025003
*/
#include "EEPROM_handler.h"
#include "../lib/TKardunio/TKarduino.h"
#include "InputBuffer/InputBuffer.h"
inline uint8_t charSpanTo_uint8(ConstSpan<char> str, /*out*/bool& success);
bool doesNameExist(const char* name, /*out*/bool& exist);
inline int getFATEntry_index(int index);
bool readFATEntry(int index, FAT& fat);
bool writeFATEntry(const FAT& fat);
inline uint8_t getFATLength();

static EEPROM_Data eepromData = EEPROM_Data(
  getFATEntry_index(getFATLength()-1)
);

bool commandFunc_store(InputBuffer &input) {
  static int processId = 0;
  static FAT fat = FAT();

  if(processId == 0) {

    fat.address = eepromData.lastAddress+1;

    ConstSpan<char> nameSpan = getToken(input, 1);
    if(nameSpan.len() > 11) {
      Serial.println("!!error!! fileName is to long can only be 11 chars");
      processId = 0;
      fat = FAT();
      return true;
    }
    fat.fileName = (char *)nameSpan.copy_asCstr();
    
    bool parseSuccess = false;
    fat.size = charSpanTo_uint8(getToken(input, 2), /*out*/ parseSuccess);

    if (!parseSuccess) {
      Serial.println("!!error!! while parsing file size, uint8 overflow caught (number has to be < 256), store not successfull");
      processId = 0;
      fat = FAT();
      return true;
    }

    processId++;
    return false;
  }

  bool exists = false;
  bool isDone = doesNameExist(fat.fileName, /*out*/exists);

  if(isDone) {
    if(!exists) {
      writeFATEntry(fat);
      processId = 0;
      fat = FAT();
      Serial.println("successfully stored new file");
      return true;
    }

    Serial.println("!!error!! fileName already exists try another name");
    processId = 0;
    fat = FAT();
    return true;
  }

  return false;
}

bool commandFunc_clearEEPROM(InputBuffer& input) {
  Serial.flush();
  Serial.print("are you sure by doing this you delete ALL your disk data yes(y)/no(n) >> ");
  while(Serial.available() <= 0) {}
  
  char firstChar = Serial.read();
  Serial.println();
  if(firstChar != 'y') {
    Serial.println("clear cancelled");
    return true;
  }

  for(uint16_t i = 0; i < EEPROM.length(); i++)
    EEPROM.write(i, 0);

  Serial.println("disk successfully cleared");
  eepromData.lastAddress = getFATEntry_index(0);
  return true;
}

bool doesNameExist(const char* name, /*out*/bool& exist) {
  static uint8_t i = 0;
  exist = false;
  
  uint8_t len = getFATLength();
  if(len == 0)
  {
    return true;
  }

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
  eepromData.lastAddress;
  EEPROM.put(eepromData.lastAddress, fat);
  return true;
}

bool readFATEntry(int index, FAT& fat) {
  if(index >= getFATLength())
    return false;

  


  return true;
}

inline int getFATEntry_index(int index) {
  constexpr int firstFAT = EEPROM_HEADER_SIZE;
  return firstFAT * (sizeof(FAT) * index);
}

inline uint8_t getFATLength() {
  uint8_t len = 0;
  EEPROM.get(EEPROM_HEADER_FAT_LEN_INDEX, /*out*/len);
  return len;
}

inline uint8_t charSpanTo_uint8(ConstSpan<char> str, /*out*/bool& success) {
  uint16_t number = 0;

  for(size_t i = 0; i < str.len(); i++) {
    number *= 10;
    uint8_t currentDecimal = (uint8_t)(str[i] - '0');
    ASSERT_PRINT((currentDecimal < 10), str.copy_asCstr());
    number += currentDecimal;
  }

  if(number < UINT8_MAX+1)
    success = true;

  return number;
}