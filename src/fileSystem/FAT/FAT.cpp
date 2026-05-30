#include "../mod.hpp"
#include "../filestore/mod.hpp"
#include <EEPROM.h>
#include "mod.hpp"

namespace fileSystem {

    namespace FAT {
        inline void getEntry_size(uint fileIndex, MutRef<u8> size);
        inline void getEntry_name(uint fileIndex, MutRef<char[BUFFER_SIZE]> name);
        inline void getEntry_address(uint fileIndex, MutRef<u16> address);

        u8 numFiles() {
            return EEPROM.read(FAT_Header::NUM_FILES_INDEX);
        }

        bool removeEntry(u8 fileIndex) {
            u8 numFiles = FAT::numFiles();
            if(numFiles == 0 || fileStore::at(fileIndex) == FileFlag::Empty)
                return false;
            
            auto error = fileStore::set(fileIndex, FileFlag::Empty);
            if(error) {
                Serial.println(error);
                return false;
            }

            EEPROM.update(FAT_Header::NUM_FILES_INDEX, numFiles-1);
            return true;
        }

        void clearall() {
            for(u16 i = 0; i < EEPROM.length(); i++)
                EEPROM.update(i, 0);
        }

        bool entry(u8 fileIndex, MutRef<FATEntry> entry) {
            if(fileStore::at(fileIndex) == FileFlag::Empty)
                return false;
            
            auto& fatEntry = entry.ref;
            getEntry_name(fileIndex, out(fatEntry.name));
            getEntry_size(fileIndex, out(fatEntry.size));
            getEntry_address(fileIndex, out(fatEntry.address));
            return true;
        }

        bool entry_name(u8 fileIndex, MutRef<char[BUFFER_SIZE]> name) {
            if(fileStore::at(fileIndex) == FileFlag::Empty)
                return false;
            
            getEntry_name(fileIndex, name);
            return true;
        }
        
        bool entry_address(u8 fileIndex, MutRef<u16> address) {
            if(fileStore::at(fileIndex) == FileFlag::Empty)
                return false;
            
            getEntry_address(fileIndex, address);
            return true;
        }

        bool entry_size(u8 fileIndex, MutRef<u8> size) {
            if(fileStore::at(fileIndex) == FileFlag::Empty)
                return false;
            
            getEntry_size(fileIndex, size);
            return true;
        }

        void readData(u16 address, u8 size, MutRef<char[BUFFER_SIZE]> data) {
            for(u8 i = 0; i < size; i++)
                data.ref[i] = EEPROM.read(address++);
        }

        inline uint entry_memoryIndex(uint fileIndex)
        {
            constexpr int firstFATIndex = FAT_Header::SIZE;
            return firstFATIndex + (FATEntry::SIZEOF * fileIndex);
        }

        inline void getEntry_name(uint fileIndex, MutRef<char[BUFFER_SIZE]> name) {
            uint address = entry_memoryIndex(fileIndex);
            for(u8 i = 0; i < BUFFER_SIZE; i++)
                name.ref[i] = EEPROM.read(address++);
        }

        inline void getEntry_size(uint fileIndex, MutRef<u8> size) {
            const uint i = entry_memoryIndex(fileIndex) + BUFFER_SIZE + sizeof(u16);
            EEPROM.get(i, size.ref);
        }

        inline void getEntry_address(uint fileIndex, MutRef<u16> address) {
            const uint i = entry_memoryIndex(fileIndex) + BUFFER_SIZE;
            EEPROM.get(i, address.ref);
        }
    }

    bool FATEntry::writeToEEPROM() {
        u8 numFiles = FAT::numFiles();
        int fileIndex = fileStore::getFirstEmpty();

        if(fileIndex == -1) {
            Serial.println(F("!!error!! can not add a new file to system because system is already full"));
            return false;
        }

        uint memoryIndex = FAT::entry_memoryIndex(fileIndex);
        for(u8 i = 0; i < BUFFER_SIZE; i++)
            EEPROM.put(memoryIndex++, this->name[i]);

        EEPROM.put(memoryIndex, this->address);
        memoryIndex += sizeof(u16);
        EEPROM.put(memoryIndex, this->size);

        auto error = fileStore::set(fileIndex, FileFlag::HasFile);
        if(error) {
            Serial.println(error);
            return false;
        }
        
        EEPROM.update(FAT_Header::NUM_FILES_INDEX, numFiles+1);
        ASSERT_EQ(numFiles+1, FAT::numFiles());
        return true;
    }

    void FATEntry::setName(StrSlice const& name) {
        u8 len = min(name.len(), BUFFER_SIZE-1);
        for(u8 i = 0; i < len; i++)
            this->name[i] = name[i];
        
        for(u8 i = len; i < BUFFER_SIZE; i++)
            this->name[i] = '\0';
    }
}

