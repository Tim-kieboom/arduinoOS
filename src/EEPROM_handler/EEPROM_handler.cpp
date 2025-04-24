/*
Tim Kieboom 1025003
*/
#include "EEPROM_handler.h"
#include "../lib/TKardunio/TKarduino.h"
#include "InputBuffer/InputBuffer.h"
inline uint8_t charSpanTo_uint8(ConstSpan<char> str, /*out*/bool& success);

static EEPROM_Data eepromData = EEPROM_Data();

void commandFunc_store(InputBuffer &input) {
  uint16_t newAddress = ++eepromData.lastAddress;
  FAT newFAT = FAT();

  newFAT.address = newAddress;

  ConstSpan<char> nameSpan = getToken(input, 1);
  if(nameSpan.len() > 11) {
    Serial.println("!!error!! fileName is to long can only be 11 chars");
    return;
  }
  newFAT.fileName = (char *)nameSpan.copy_asCstr();
  
  bool parseSuccess = false;
  newFAT.size = charSpanTo_uint8(getToken(input, 2), /*out*/ parseSuccess);

  if (!parseSuccess) {
    Serial.println("!!error!! while parsing file size, uint8 overflow caught (number has to be < 256), store not successfull");
    return;
  }

  Serial.println("successfully stored new file");
}

void commandFunc_clearEEPROM(InputBuffer& input) {
  Serial.flush();
  Serial.print("are you sure by doing this you delete ALL your disk data yes(y)/no(n) >> ");
  while(Serial.available() <= 0) {}
  
  char firstChar = Serial.read();
  Serial.println();
  if(firstChar != 'y') {
    Serial.println("clear cancelled");
    return;
  }

  for(uint16_t i = 0; i < EEPROM.length(); i++)
    EEPROM.write(i, 0);

  Serial.println("disk successfully cleared");
}

bool writeFATEntry(FAT& fat) {
  EEPROM.put(eepromData.lastAddress, fat);
  eepromData.lastAddress += sizeof(FAT);
  return true;
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