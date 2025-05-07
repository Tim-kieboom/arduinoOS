#ifndef HAS_FILE_STORE_H
#define HAS_FILE_STORE_H
#pragma once

#include "./EEPROM_handler/EEPROM_header.hpp"

namespace FileStore {
    constexpr bool HAS_FILE = true;
    constexpr bool EMPTY = false;

    bool setFile(uint8_t index, bool value);     
    int getFirstEmpty();     
    SmartArray<uint8_t> getAllIndex();
};

#endif


