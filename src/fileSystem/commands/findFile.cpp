#include "../mod.hpp"
static bool strEquals(const char* self, const char* other) {
    return strcmp(self, other) == 0;
}

namespace fileSystem {
    Task findFileIndex(MutRef<int> iRef, const char* name) {
        auto& i = iRef.ref;
        
        u16 len = FAT::numFiles();
        if(len == 0) {
            i = -1;
            return Task::Done();
        }
        
        if((u16)i >= len) {
            i = -1;
            return Task::Done();
        }
        
        char nameBuffer[BUFFER_SIZE];
        if(!FAT::entry_name(i, out(nameBuffer)))
            return Task::Pending();
        
        if(strEquals(name, nameBuffer)) {
            return Task::Done();
        }

        i++;
        return Task::Pending();
    }
}