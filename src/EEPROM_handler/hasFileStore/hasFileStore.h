#ifndef HAS_FILE_STORE_H
#define HAS_FILE_STORE_H
#pragma once

#include "./EEPROM_handler/EEPROM_header.hpp"
namespace FileStore {
    struct FileState {bool value; };
    constexpr FileState HAS_FILE = {true};
    constexpr FileState EMPTY = {false};

    bool setFile(uint8_t index, FileState value);     
    int getFirstEmpty();     
    SmartArray<uint8_t> getAllIndex();
};

#endif


