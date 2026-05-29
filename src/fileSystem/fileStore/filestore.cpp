#include "mod.hpp"
#include <EEPROM.h>

inline bool indexBit(u8 byte, u8 index); 
inline int getFirstEmptyBit(u8 byte, u8 offset);

namespace fileSystem {
    namespace fileStore {

        const Fstr* set(u8 index, FileFlag value) {
            ASSERT_SMALLER(index, FAT_Header::FILE_STORE_LEN * 8);
            if(index > FAT_Header::FILE_STORE_LEN * 8)
                return F("!!error!! fileIndex is out of bounds");

            const int byteIndex = index / 8;
            const int bitIndex = index % 8;

            int address = byteIndex + FAT_Header::FILE_STORE_INDEX;
            if (address < 0 || address >= (int)EEPROM.length()) {
                return F("!!error!! EEPROM address is out of bounds");
            }

            u8 eepromByte = EEPROM.read(address);
            if(value == FileFlag::HasFile) {
                eepromByte |= (1 << bitIndex);
            } 
            else {
                eepromByte &= ~(1 << bitIndex);
            }

            EEPROM.put(address, eepromByte);
            return nullptr;
        }

        FileFlag at(u8 fileIndex) {
            ASSERT_SMALLER(fileIndex, FAT_Header::FILE_STORE_LEN * 8);

            const int byteIndex = fileIndex / 8;
            const int bitIndex  = fileIndex % 8;

            int address = byteIndex + FAT_Header::FILE_STORE_INDEX;
            if (address < 0 || address >= (int)EEPROM.length()) {
                return FileFlag::Empty;
            }

            u8 eepromByte = EEPROM.read(address);
            if ((eepromByte & (1 << bitIndex)) == 0)
                return FileFlag::Empty;
            
            return FileFlag::HasFile;
        }
        
        int getFirstEmpty() {
            for(usize byteIndex = 0; byteIndex < FAT_Header::FILE_STORE_LEN; byteIndex++) {
                u8 eepromByte = EEPROM.read(byteIndex + FAT_Header::FILE_STORE_INDEX);
                if(eepromByte != UINT8_MAX) {
                    int bitIndex = getFirstEmptyBit(eepromByte, 0);
                    return bitIndex + byteIndex * 8;
                }
            }

            return -1;
        }

        u8 getIndexes(MutRef<u8[MAX_INDEXES]> indexes) {
            u8 numIndexes = EEPROM.read(FAT_Header::NUM_FILES_INDEX);
            u8 top = 0;
            
            for (u8 byteIndex = 0; byteIndex < FAT_Header::FILE_STORE_LEN; byteIndex++) {
                
                u8 byte = EEPROM.read(byteIndex + FAT_Header::FILE_STORE_INDEX);
                for (u8 i = 0; i < 8; i++) {

                    if (indexBit(byte, i) == 0)
                        continue;
                    
                    u8 index = (byteIndex * 8) + i;
                    indexes.ref[top++] = index;
                    if (top == numIndexes)
                        return numIndexes;
                }
            }
            return numIndexes;
        }
    }
}

inline bool indexBit(u8 byte, u8 index) {
    return byte & (1 << index);
}

inline int getFirstEmptyBit(u8 byte, u8 offset) {
    if (offset >= 8 || byte == UINT8_MAX) 
        return -1;

    u8 mask = UINT8_MAX << offset;
    u8 inverted = ~byte & mask;
    return __builtin_ctz(inverted);  // Compiler built-in for trailing zeros
}