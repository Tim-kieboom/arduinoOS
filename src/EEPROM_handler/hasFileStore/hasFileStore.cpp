#include "hasFileStore.h"
#include <TKarduino.h> 
inline int getFirstEmptyBit(uint8_t byte, uint8_t offset);
inline int getFirstFullBit(uint8_t byte, uint8_t offset);
inline uint8_t countEmptyBits(uint8_t byte);
namespace FileStore {

bool setFile(uint8_t index, bool value) {
    ASSERT_SMALLER(index, (EEPROM_Header::FILE_STORE_LEN * 8)-1);
    if(index > EEPROM_Header::FILE_STORE_LEN * 8)
        return false;

    const int byteIndex = index / 8;
    const int bitIndex = index % 8;

    uint8_t byte = EEPROM.read(byteIndex);
    if(value) {
        byte |= (1 << bitIndex);
    } else {
        byte &= ~(1 << bitIndex);
    }
    EEPROM.update(byteIndex, byte);
    return true;
}

int getFirstEmpty(uint8_t offset) {
    uint8_t byteOffset = offset / 8;
    uint8_t bitOffset = offset % 8;

    for(size_t byteIndex = byteOffset; byteIndex < EEPROM_Header::FILE_STORE_LEN; byteIndex++) {
        uint8_t byte = EEPROM.read(byteIndex);
        if(byte < UINT8_MAX) {
            int bitIndex = getFirstEmptyBit(byte, bitOffset);
            return bitIndex + byteIndex * 8;
        }
        bitOffset = 0;
    }

    return -1;
}

int getFirstIndex(uint8_t offset) {
    uint8_t byteOffset = offset / 8;
    uint8_t bitOffset = offset % 8;

    for (size_t byteIndex = byteOffset; byteIndex < EEPROM_Header::FILE_STORE_LEN; byteIndex++) {
        uint8_t byte = EEPROM.read(byteIndex);
        if (byte > 0) {
            int bitIndex = getFirstFullBit(byte, bitOffset);
            if (bitIndex >= 0)
                return bitIndex + byteIndex * 8;
        }
        bitOffset = 0;
    }

    return -1;
}

int amountOfFree() {
    uint8_t numEmptys = 0;

    for(size_t byteIndex = 0; byteIndex < EEPROM_Header::FILE_STORE_LEN; byteIndex++) {
        uint8_t byte = EEPROM.read(byteIndex);
        numEmptys += countEmptyBits(byte);
    }

    return numEmptys;
}

}

inline uint8_t countEmptyBits(uint8_t byte) {
    return (sizeof(uint8_t) * 8) - __builtin_popcount(byte); // Built-in for counting 1s
}

inline int getFirstEmptyBit(uint8_t byte, uint8_t offset) {
    if (offset >= 8 || byte == UINT8_MAX) 
        return -1;

    uint8_t mask = UINT8_MAX << offset;
    uint8_t inverted = ~byte & mask;
    return __builtin_ctz(inverted);  // Compiler built-in for trailing zeros
}

inline int getFirstFullBit(uint8_t byte, uint8_t offset) {
    if (offset >= 8 || byte == 0)
        return -1;

    uint8_t mask = UINT8_MAX << offset;
    uint8_t masked = byte & mask;
    return (masked) ? __builtin_ctz(masked) : -1;
}


