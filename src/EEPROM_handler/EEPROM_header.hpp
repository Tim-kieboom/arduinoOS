#ifndef EEPROM_HEADER_H
#define EEPROM_HEADER_H
#pragma once
#include <Arduino.h>
#include <EEPROM.h>
#include "../utils/utils.h"

constexpr uint8_t FAT_MAX_NUM_FILES = 10;

namespace EEPROM_Header{

  constexpr size_t NUM_FILES_INDEX = 0;
  constexpr size_t FILE_STORE_INDEX = 1;
  static const size_t FILE_STORE_LEN = (size_t)roundUp(FAT_MAX_NUM_FILES / 8.0);
  static const size_t SIZE = FILE_STORE_LEN+1;
}

#endif