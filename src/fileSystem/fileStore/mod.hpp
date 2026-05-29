/*
Tim Kieboom 1025003
*/
#pragma once
#include "../FAT/mod.hpp"

namespace fileSystem {

    enum class FileFlag {
        Empty = false,
        HasFile = true,
    };

    /// the EEPROM based file-store bitmap
    namespace fileStore {
        /// Maximum number of file indexes the bitmap can track.
        constexpr u8 MAX_INDEXES = fileSystem::FAT_Header::MAX_NUM_FILES;

        /// Checks whether a file slot at the given index is occupied.
        FileFlag at(u8 fileIndex);

        /** 
         * @brief Sets the occupation flag for a file slot.
         * @return nullptr on success, or an error message on failure.
         */
        const Fstr* set(u8 index, FileFlag value);     
        
        /// Finds the index of the first empty file slot, or -1 if none are free.
        int getFirstEmpty(); 

        /** 
         * @brief Fills the output array with indices of all occupied file slots. 
         * @returns the count.
         */
        u8 getIndexes(MutRef<u8[MAX_INDEXES]> indexes);
    }
}