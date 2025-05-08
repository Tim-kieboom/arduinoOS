#include "hasFileStore.h"
#include <TKarduino.h> 
inline int getFirstEmptyBit(uint8_t byte, uint8_t offset);
inline int getFirstFullBit(uint8_t byte, uint8_t offset);
inline uint8_t countEmptyBits(uint8_t byte);
namespace FileStore {

TempPtr<char> byteToString(uint8_t byte) {
    TempPtr<char> buffer = TempPtr<char>(new char[9]);
    int i = 0;
    for (int j = 7; j >= 0; j--) 
        buffer.ptr[i++] = (bitRead(byte, j) == 1) ? '1' : '0';
    
    buffer.ptr[8] = '\0';
    return buffer;
}

bool setFile(uint8_t index, FileState state) {
    ASSERT_SMALLER(index, (EEPROM_Header::FILE_STORE_LEN * 8)-1);
    if(index > EEPROM_Header::FILE_STORE_LEN * 8)
        return false;

    const int byteIndex = index / 8;
    const int bitIndex = index % 8;
    DEBUG_PRINTM(
        F("(debug only) in setFile(): \n"), 
        F("\tindex: "), index, '\n',
        F("\tbyteIndex: "), byteIndex, '\t',
        F("\tbitIndex: "), bitIndex, '\n'
    );

    uint8_t byte = EEPROM.read(byteIndex + EEPROM_Header::FILE_STORE_INDEX);
    DEBUG_PRINTM(F("\told byte: "), byteToString(byte).ptr, '\n');
    if(state.value) {
        byte |= (1 << bitIndex);
    } 
    else {
        byte &= ~(1 << bitIndex);
    }

    DEBUG_PRINTM(F("\tnew byte: "), byteToString(byte).ptr, '\n');

    EEPROM.put(byteIndex + EEPROM_Header::FILE_STORE_INDEX, byte);
    return true;
}

int getFirstEmpty() {
    for(size_t byteIndex = 0; byteIndex < EEPROM_Header::FILE_STORE_LEN; byteIndex++) {
        uint8_t byte = EEPROM.read(byteIndex + EEPROM_Header::FILE_STORE_INDEX);
        if(byte < UINT8_MAX) {
            int bitIndex = getFirstEmptyBit(byte, 0);
            return bitIndex + byteIndex * 8;
        }
    }

    return -1;
}

SmartArray<uint8_t> getAllIndex() {
    uint8_t numIndexes = EEPROM.read(EEPROM_Header::NUM_FILES_INDEX);
    auto indexes = SmartArray<uint8_t>(numIndexes);
    uint8_t top = 0;
    
    for (uint8_t byteIndex = 0; byteIndex < EEPROM_Header::FILE_STORE_LEN; byteIndex++) {
        uint8_t byte = EEPROM.read(byteIndex + EEPROM_Header::FILE_STORE_INDEX);
        for (int i = 0; i < 8; i++) {

            if (byte & (1 << i)) { 
                uint8_t index = (byteIndex * 8) + i;
                indexes[top++] = index;
                if (top == numIndexes)
                    return indexes;
            }
        }
    }
    return indexes;
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


