#include "mod.hpp"

namespace fileSystem {

    Task printFiles(MutRef<PrintFilesState> stateRef) {
        auto& state = stateRef.ref;

        if(state.taskId == PrintFilesState::CheckName) {
            
            state.i = 0;
            state.indexesLen = fileStore::getIndexes(out(state.indexes));
            if(state.indexesLen == 0)  {
                Serial.println(F("no files found"));
                return Task::Done();
            }

            state.taskId = PrintFilesState::IndexesLoaded;            
        }
        
        u8 index = state.indexes[state.i];
        
        FATEntry entry;
        if(!FAT::entry(index, out(entry)))
            return Task::Pending();

        Serial.print(F("name: \""));
        for(u8 i = 0; i < BUFFER_SIZE; i++) {
            const char ch = entry.name[i];
            if(ch == '\0')
                break;
            
            Serial.print(ch);
        }
        printM(F("\" size: "), entry.size, F(" address: "), entry.address, '\n');

        if(++state.i >= state.indexesLen)
            return Task::Done();
        
        return Task::Pending();
    }
}