/*
Tim Kieboom 1025003
*/
#ifndef EEPROM_HANDLER_H
#define EEPROM_HANDLER_H
#pragma once

#include <Arduino.h>
#include <EEPROM.h>
#include "./commandHandler/commandHandler.h"

constexpr size_t EEPROM_HEADER_FAT_LEN_INDEX = 0;
constexpr size_t EEPROM_HEADER_SIZE = 1;

struct File {
  char fileName[BUFFER_SIZE];
  uint8_t size;
  char data[BUFFER_SIZE];
};

struct EEPROM_Data {
  uint16_t lastAddress;

  EEPROM_Data(uint16_t lastAddress)
    : lastAddress(lastAddress)
  {

  }
};

struct FAT {
  char* fileName;
  uint16_t address;
  uint8_t size;
};

bool commandFunc_store(InputBuffer &input);
bool commandFunc_clearEEPROM(InputBuffer& inputBuff);

#endif