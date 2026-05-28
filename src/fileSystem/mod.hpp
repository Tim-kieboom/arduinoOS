/*
Tim Kieboom 1025003
*/
#pragma once 
#include <TKarduino.hpp>

/** Rounds a float up to the nearest integer. */
constexpr int roundUp(float value) {
    return (int)(value) + (value > (int)(value) ? 1 : 0);
}

namespace fileSystem {

    /// Size of the name and data character arrays in FATEntry.
    constexpr u8 DATA_SIZE = 12;

    /// Represents a file with name, data, and size.
    struct File {
        char name[DATA_SIZE]; ///< File name buffer.
        char data[DATA_SIZE]; ///< File data buffer.
        u8 size;              ///< Size of the file data.
    };

    /// A File Allocation Table entry stored in EEPROM.
    struct FATEntry {

        /** Total byte size of a serialised FATEntry (address + size + name). */
        static const usize SIZEOF = sizeof(u16) + sizeof(u8) + DATA_SIZE;

        char name[DATA_SIZE]; ///< File name buffer.
        u16 address;          ///< EEPROM address of the file data.
        u8 size;              ///< File size in bytes.

        /// Writes this entry to the next free slot in EEPROM. Returns true on success.
        bool writeToEEPROM();

        /// Copies the given name into the internal name buffer (null-terminated).
        void setName(StrSlice& name);
    };

    /// FAT-level operations on the file allocation table.
    namespace FAT {
        /// @returns the number of files currently stored.
        u8 numFiles();

        /// Clears the entire FAT (all entries).
        void clearall();

        /**
         * @brief Reads the FAT entry at fileIndex. 
         * @returns false if the slot is empty.
         */
        bool entry(u8 fileIndex, MutRef<FATEntry> entry);

        /**
         * @brief Reads only the name of the FAT entry at fileIndex. 
         * @returns false if empty.
         */
         bool entry_name(u8 fileIndex, MutRef<char[DATA_SIZE]> name);
    }

    /// Describes a file to be stored (name, data, size).
    struct FileInfo {
        StrSlice name = StrSlice(); ///< File name.
        StrSlice data = StrSlice(); ///< File data.
        u8 size = 0;                ///< Data size in bytes.
    };

    /// Async state holder for the file-store operation.
    struct StoreState {
        FATEntry fatEntry = FATEntry(); ///< The FAT entry being written.
        u8 nameExistIndex = 0;                       ///< Loop counter for name-existence check.
    };
    
    /// Erases all files from the FAT and resets the system.
    void clearFAT();
    
    /// Stores a file asynchronously. Call repeatedly until the returned Task is done.
    Task store(MutRef<StoreState> state, FileInfo info);

    /// EEPROM layout constants.
    namespace EEPROM_Header {
        /// Maximum number of files the FAT can contain.
        constexpr u8 FAT_MAX_NUM_FILES = 10;
        /// EEPROM address storing the current file count.
        constexpr usize NUM_FILES_INDEX = 0;
        /// EEPROM address where the file-store bitmap starts.
        constexpr usize FILE_STORE_INDEX = 1;
        /// Number of bytes used for the file-store bitmap.
        constexpr usize FILE_STORE_LEN = static_cast<usize>(roundUp(FAT_MAX_NUM_FILES / 8.0));
        /// Total EEPROM header size in bytes.
        constexpr usize SIZE = FILE_STORE_LEN+1;
    }
}