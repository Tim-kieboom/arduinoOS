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

constexpr uint8_t FAT_MAX_NUM_FILES = 2;

struct File {
  char fileName[BUFFER_SIZE];
  uint8_t size;
  char data[BUFFER_SIZE];
};

struct EEPROM_Data {
  int16_t lastAddress = -1;

  EEPROM_Data(int16_t lastAddress)
    : lastAddress(lastAddress) { }
};

struct FAT {
  char* fileName;
  uint16_t address;
  uint8_t size;
};

bool commandFunc_temp(InputBuffer &input);
bool commandFunc_store(InputBuffer& input);
bool commandFunc_clearEEPROM(InputBuffer &input);
#endif