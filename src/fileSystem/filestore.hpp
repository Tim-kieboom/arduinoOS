/*
Tim Kieboom 1025003
*/
#pragma once
#include "mod.hpp"

namespace fileStore {
    constexpr u8 MAX_INDEXES = fileSystem::EEPROM_Header::FAT_MAX_NUM_FILES;
    constexpr bool HAS_FILE = true;
    constexpr bool EMPTY = false;

    bool hasFile(u8 fileIndex);
    const Fstr* setFile(u8 index, bool value);     
    
    int getFirstEmpty();     
    u8 getAllIndex(MutRef<u8[MAX_INDEXES]> indexes);
}