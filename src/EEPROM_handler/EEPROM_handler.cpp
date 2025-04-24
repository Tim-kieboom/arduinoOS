/*
Tim Kieboom 1025003
*/
#include "EEPROM_handler.h"
#include "../lib/TKardunio/TKarduino.h"
#include "InputBuffer/InputBuffer.h"

inline uint8_t charSpanTo_uint8(ConstSpan<char> str, /*out*/bool& success);

static EEPROM_Data eepromData = EEPROM_Data();

void commandFunc_store(InputBuffer& inputBuff) {
  uint16_t newAddress = ++eepromData.lastAddress;
  FAT newFAT = FAT();
  
  newFAT.address = newAddress;
  newFAT.fileName = (char*)getToken(inputBuff, 1).copy_asCstr();
  bool parseSuccess = false;
  newFAT.size = charSpanTo_uint8(getToken(inputBuff, 2), /*out*/parseSuccess);

  if(!parseSuccess) {
    Serial.println("!!error!! while parsing file size, uint8 overflow caught (number has to be < 256), store not successfull");
    return;
  }

  Serial.print("fileName: ");
  Serial.println(newFAT.fileName);

  Serial.print("size: ");
  Serial.println(newFAT.size);
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