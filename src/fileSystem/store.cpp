#include "mod.hpp"
#include <EEPROM.h>
static const Fstr* checkName(StrSlice const& name);

bool strEquals(const char* self, const char* other) {
    return strcmp(self, other) == 0;
}

namespace fileSystem {
    inline Task doesNameExist(MutRef<u16> i, const char* name, MutRef<bool> exists);
    
    inline void store_checkName(MutRef<StoreState> state, FileInfo const& file);
    inline void store_writeData(MutRef<StoreState> state, FileInfo const& file);
    inline void store_writeFAT(MutRef<StoreState> stateRef, FileInfo const& file);
    inline void store_checkForDuplicate(MutRef<StoreState> state, FileInfo const& file);

    Task store(MutRef<StoreState> stateRef, FileInfo const& file) {
        auto& state = stateRef.ref;

        switch(state.taskId) {
            case StoreState::CheckName:
                store_checkName(out(state), file);
                return Task::Pending();

            case StoreState::CheckForDuplicate:
                store_checkForDuplicate(out(state), file);
                return Task::Pending();
            
            case StoreState::WriteFAT:
                store_writeFAT(out(state), file);
                return Task::Pending();

            case StoreState::WriteData:
                store_writeData(out(state), file);
                return Task::Pending();

            case StoreState::End: 
                return Task::Done();
            
            default: DEBUG_UNREACHABLE;
        }

        return Task::Done();
    }

    inline void store_checkName(MutRef<StoreState> stateRef, FileInfo const& file) {
        auto& state = stateRef.ref;

        auto error = checkName(file.name);
        if(error) {
            Serial.println(error);
            state.taskId = StoreState::End;
            return;
        }
        
        u8 numFiles = FAT::numFiles();
        if(numFiles >= FAT_Header::MAX_NUM_FILES) {
            Serial.println(F("!!error!! not more space left to store another file"));
            state.taskId = StoreState::End;
            return;
        }

        state.fatEntry.setName(file.name);
        if(file.data.len() > BUFFER_SIZE) {
            Serial.println(F("!!error!! files data is to big"));
            state.taskId = StoreState::End;
            return;
        }

        state.taskId = StoreState::CheckForDuplicate;
    }

    inline void store_checkForDuplicate(MutRef<StoreState> stateRef, FileInfo const& file) {
        auto& state = stateRef.ref;
        
        bool exists = false;
        Task await = doesNameExist(out(state.i), file.name.asPtr(), out(exists));
        if(!await.isDone) 
            return;

        if(exists) {
            Serial.println(F("!!error!! fileName already exists"));
            state.taskId = StoreState::End;
            return;
        }

        state.i = 0;
        state.taskId = StoreState::WriteFAT;
    }

    inline void store_writeFAT(MutRef<StoreState> stateRef, FileInfo const& file) {
        auto& state = stateRef.ref;

        Task await = findFreeSpace(out(state.freeSpaceState), out(state.writeAddress), file.data.len());
        if(!await.isDone)
            return;

        if (state.writeAddress == FREESPACE_ERROR) {
            Serial.println(F("!!error!! not enough free space in EEPROM for file data"));
            state.taskId = StoreState::End;
            return;
        }

        state.fatEntry.address = state.writeAddress;
        state.fatEntry.size = file.data.len();
        bool isSuccess = state.fatEntry.writeToEEPROM();
        if(!isSuccess) {
            state.taskId = StoreState::End;
            return;
        }

        state.taskId = StoreState::WriteData;
    }

    inline void store_writeData(MutRef<StoreState> stateRef, FileInfo const& file) {
        auto& state = stateRef.ref;

        u16 address = state.fatEntry.address;

        if(state.i >= file.data.len()) {
            Serial.print(F("successfully stored file: `"));
            file.name.print();
            Serial.println('`');
            state.taskId = StoreState::End;
            return;
        }

        EEPROM.write(state.i + address, file.data[state.i]);
        state.i++;
    }

    inline Task doesNameExist(MutRef<u16> iRef, const char* name, MutRef<bool> exists) {
        auto& i = iRef.ref;
        
        u16 len = FAT::numFiles();
        if(len == 0) {
            i = 0;
            exists.ref = false;
            return Task::Done();
        }
        
        if(i >= len) {
            i = 0;
            exists.ref = false;
            return Task::Done();
        }
        
        char nameBuffer[BUFFER_SIZE];
        if(!FAT::entry_name(i, out(nameBuffer)))
            return Task::Pending();
        
        if(strEquals(name, nameBuffer)) {
            i = 0;
            exists.ref = true;
            return Task::Done();
        }

        i++;
        return Task::Pending();
    }
}

inline bool isNumber(char ch) {
    return ch >= '0' && ch < '9';
}

static const Fstr* checkName(StrSlice const& name) {
    const char* illigalChars = "~`!@#$%^&*()-+={}|[]\\;':\",./<>?";
    
    if(name.len() == 0)
        return F("!!error!! name is empty");

    if (name.len() > fileSystem::BUFFER_SIZE-1)
        return F("!!error!! name of file is to big, file names can not be bigger then 11 symbools");

    if(isNumber(name[0]))
        return F("!!error!! name of file can not start with a number");

    for(usize i = 0; i < name.len(); i++) {

        if (strchr(illigalChars, name[i]) != nullptr)
            return F("!!error!! name of file can not contain any special chars");
    }

    return nullptr;
}