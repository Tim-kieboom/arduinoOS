#include "../mod.hpp"

namespace fileSystem {
    bool erase(int fileIndex) {
        if(fileIndex < 0)
            return false;
            
        return FAT::removeEntry((u8)fileIndex);
    }
}