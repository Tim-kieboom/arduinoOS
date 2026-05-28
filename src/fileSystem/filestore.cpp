#include "filestore.hpp"
#include <EEPROM.h>

inline bool indexBit(u8 byte, u8 index); 
inline int getFirstEmptyBit(u8 byte, u8 offset);

using namespace fileSystem;
namespace fileStore {

    const Fstr* setFile(u8 index, bool value) {
        ASSERT_SMALLER(index, EEPROM_Header::FILE_STORE_LEN * 8);
        if(index > EEPROM_Header::FILE_STORE_LEN * 8)
            return F("!!error!! fileIndex is out of bounds");

        const int byteIndex = index / 8;
        const int bitIndex = index % 8;
        DEBUG_PRINTM(
            F("(debug only) in setFile(): "), 
            F("\n\tindex: "), index,
            F("\n\tbyteIndex: "), byteIndex,
            F("\n\tbitIndex: "), bitIndex, '\n'
        );

        int address = byteIndex + EEPROM_Header::FILE_STORE_INDEX;
        if (address < 0 || address >= (int)EEPROM.length()) {
            return F("!!error!! EEPROM address is out of bounds");
        }

        u8 eepromByte = EEPROM.read(address);
        IF_DEBUG(
            Serial.print(F("\told byte: 0b"));
            Serial.print(eepromByte, BIN);
            Serial.print('\n');
        );
        if(value) {
            eepromByte |= (1 << bitIndex);
        } 
        else {
            eepromByte &= ~(1 << bitIndex);
        }

        IF_DEBUG(
            Serial.print(F("\tnew byte: 0b"));
            Serial.print(eepromByte, BIN);
            Serial.print('\n');
        );
        EEPROM.put(address, eepromByte);
        return nullptr;
    }

    bool hasFile(u8 fileIndex) {
        ASSERT_SMALLER(fileIndex, EEPROM_Header::FILE_STORE_LEN * 8);

        const int byteIndex = fileIndex / 8;
        const int bitIndex  = fileIndex % 8;

        int address = byteIndex + EEPROM_Header::FILE_STORE_INDEX;
        if (address < 0 || address >= (int)EEPROM.length()) {
            return false;
        }

        u8 eepromByte = EEPROM.read(address);
        return (eepromByte & (1 << bitIndex)) != 0;
    }
    
    int getFirstEmpty() {
        for(usize byteIndex = 0; byteIndex < EEPROM_Header::FILE_STORE_LEN; byteIndex++) {
            u8 eepromByte = EEPROM.read(byteIndex + EEPROM_Header::FILE_STORE_INDEX);
            if(eepromByte != UINT8_MAX) {
                int bitIndex = getFirstEmptyBit(eepromByte, 0);
                return bitIndex + byteIndex * 8;
            }
        }

        return -1;
    }

    u8 getAllIndex(MutRef<u8[MAX_INDEXES]> indexes) {
        u8 numIndexes = EEPROM.read(EEPROM_Header::NUM_FILES_INDEX);
        u8 top = 0;
        
        for (u8 byteIndex = 0; byteIndex < EEPROM_Header::FILE_STORE_LEN; byteIndex++) {
            
            u8 byte = EEPROM.read(byteIndex + EEPROM_Header::FILE_STORE_INDEX);
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