/*
Tim Kieboom 1025003
*/
#ifndef EEPROM_HANDLER_H
#define EEPROM_HANDLER_H
#pragma once

#include <Arduino.h>
#include <EEPROM.h>
#include "./commandHandler/commandHandler.h"

struct File {
  char fileName[BUFFER_SIZE];
  uint8_t size;
  char data[BUFFER_SIZE];
};

struct EEPROM_Data {
  uint16_t lastAddress = 0;
};

struct FAT {
  char* fileName;
  uint16_t address;
  uint8_t size;
};

void commandFunc_store(InputBuffer& inputBuff);


#endif