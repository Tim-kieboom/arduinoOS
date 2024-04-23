#include "EEPROM_handler.h"

static EEPROM_Data *eepromData = new EEPROM_Data();

void store(InputBuffer *inputBuff)
{
  Serial.println("storeTest");

  uint16_t newAddress = eepromData->lastAddress + 1;
  FAT newFAT = FAT();
  newFAT.address = newAddress;
  strcpy(newFAT.fileName, getToken(inputBuff, 1));
  newFAT.size = (uint8_t)(getToken(inputBuff, 2)[0] - '0');

  FAT fat;

  EEPROM.put(newAddress, newFAT);
  EEPROM.get(newAddress, fat);

  Serial.print("fileName: ");
  printString(newFAT.fileName);
}