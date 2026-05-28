/*
Tim Kieboom 1025003
*/
#pragma once 
#include <TKarduino.hpp>

constexpr int roundUp(float value) {
    return (int)(value) + (value > (int)(value) ? 1 : 0);
}

namespace fileSystem {

    constexpr u8 DATA_SIZE = 12;
    struct File {
        char name[DATA_SIZE];
        char data[DATA_SIZE];
        u8 size;
    };

    struct FATEntry {
        static const usize SIZEOF = sizeof(u16) + sizeof(u8) + DATA_SIZE;
        char name[DATA_SIZE];
        u16 address;
        u8 size;

        bool writeToEEPROM();
        void setName(StrSlice& name);
    };

    namespace FAT {
        
        u8 numFiles();
        void clearall();
        bool entry(u8 fileIndex, MutRef<FATEntry> entry);
        bool entry_name(u8 fileIndex, MutRef<char[DATA_SIZE]> name);
    }

    struct FileInfo {
        StrSlice name = StrSlice(); 
        StrSlice data = StrSlice(); 
        u8 size = 0;
    };
    struct StoreState {
        FATEntry fatEntry = FATEntry();
        u8 i = 0;
    };
    
    void clearFAT();
    Task store(MutRef<StoreState> state, FileInfo info);

    namespace EEPROM_Header {
        constexpr u8 FAT_MAX_NUM_FILES = 10;
        constexpr usize NUM_FILES_INDEX = 0;
        constexpr usize FILE_STORE_INDEX = 1;
        constexpr usize FILE_STORE_LEN = static_cast<usize>(roundUp(FAT_MAX_NUM_FILES / 8.0));
        constexpr usize SIZE = FILE_STORE_LEN+1;
    }
}