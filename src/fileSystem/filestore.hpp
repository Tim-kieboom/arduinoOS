/*
Tim Kieboom 1025003
*/
#pragma once
#include "mod.hpp"

namespace fileStore {
    /// Maximum number of file indexes the bitmap can track.
    constexpr u8 MAX_INDEXES = fileSystem::EEPROM_Header::FAT_MAX_NUM_FILES;
    /// Bit value indicating a file slot is occupied.
    constexpr bool HAS_FILE = true;
    /// Bit value indicating a file slot is free.
    constexpr bool EMPTY = false;

    /// Checks whether a file slot at the given index is occupied.
    bool hasFile(u8 fileIndex);

    /** 
     * @brief Sets the occupation flag for a file slot.
     * @return nullptr on success, or an error message on failure.
     */
    const Fstr* setFile(u8 index, bool value);     
    
    /// Finds the index of the first empty file slot, or -1 if none are free.
    int getFirstEmpty(); 
        
    /// Fills the output array with indices of all occupied file slots. Returns the count.
    u8 getAllIndex(MutRef<u8[MAX_INDEXES]> indexes);
}