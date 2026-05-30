#include "../mod.hpp"

namespace fileSystem {
    
    bool recieve(MutRef<int> iRef, MutRef<char[BUFFER_SIZE]> dataRef, MutRef<u8> dataSizeRef) {
        auto& i = iRef.ref;
        auto& data = dataRef.ref;
        auto& dataSize = dataSizeRef.ref;

        u16 address;
        if(!FAT::entry_address((u8)i, out(address))) {
            Serial.println(F("!!error!! file not found"));
            return false; 
        }

        FAT::entry_size((u8)i, out(dataSize));
        FAT::readData(address, dataSize, out(data));
        return true;
    }
}
