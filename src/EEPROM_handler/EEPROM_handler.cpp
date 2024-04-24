#include "EEPROM_handler.h"

static EEPROM_Data *eepromData = new EEPROM_Data();

uint8_t stringTo_uint8(char * string)
{
  uint8_t prevNumbers = 0;

  uint8_t i = -1;
  while(string[++i] != '\0')
  {
    prevNumbers *= 10;
    uint8_t currentNumber = (uint8_t)(string[i] - '0');
    prevNumbers += currentNumber;
  }

  return prevNumbers;
}

void store(InputBuffer *inputBuff)
{
  uint16_t newAddress = eepromData->lastAddress + 1;

  FAT newFAT = FAT();
  
  newFAT.address = newAddress;
  newFAT.fileName = getToken(inputBuff, 1);
  newFAT.size = stringTo_uint8(getToken(inputBuff, 2));

  Serial.print("fileName: ");
  printString(newFAT.fileName);

  Serial.print("size: ");
  Serial.println(newFAT.size);
}