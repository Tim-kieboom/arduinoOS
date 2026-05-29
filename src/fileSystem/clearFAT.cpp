#include "mod.hpp"

namespace fileSystem {
    
    void clearFAT() {
        FAT::clearall();

        Serial.println(F("disk cleared"));
        Serial.flush();
        delay(20);
        RESET_ARDUINO;
    }
}