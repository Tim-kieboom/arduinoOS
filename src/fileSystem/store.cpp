#include "mod.hpp"
#include <EEPROM.h>
static const Fstr* checkName(StrSlice const& name);

bool strEquals(const char* self, const char* other) {
    return strcmp(self, other) == 0;
}

namespace fileSystem {
    inline Task store_checkName(MutRef<StoreState> state, FileInfo const& file);
    inline Task store_writeData(MutRef<StoreState> state, FileInfo const& file);
    inline Task store_writeFAT(MutRef<StoreState> stateRef, FileInfo const& file);
    inline Task doesNameExist(MutRef<u16> i, const char* name, MutRef<bool> exists);
    inline Task store_checkForDuplicate(MutRef<StoreState> state, FileInfo const& file);

    Task store(MutRef<StoreState> stateRef, FileInfo const& file) {
        auto& state = stateRef.ref;

        switch(state.taskId) {
            case StoreState::CheckName:
                return store_checkName(out(state), file);

            case StoreState::CheckForDuplicate:
                return store_checkForDuplicate(out(state), file);
            
            case StoreState::WriteFAT:
                return store_writeFAT(out(state), file);

            case StoreState::WriteData:
                return store_writeData(out(state), file);

            case StoreState::End: 
                return Task::Done();
            
            default: DEBUG_UNREACHABLE;
        }

        return Task::Done();
    }

    inline Task store_checkName(MutRef<StoreState> stateRef, FileInfo const& file) {
        auto& state = stateRef.ref;

        auto error = checkName(file.name);
        if(error) {
            Serial.println(error);
            return Task::Done();
        }
        
        u8 numFiles = FAT::numFiles();
        if(numFiles >= FAT_Header::MAX_NUM_FILES) {
            Serial.println(F("!!error!! not more space left to store another file"));
            return Task::Done();
        }

        state.fatEntry.setName(file.name);
        state.taskId = StoreState::CheckForDuplicate;
        return Task::Pending();
    }

    inline Task store_checkForDuplicate(MutRef<StoreState> stateRef, FileInfo const& file) {
        auto& state = stateRef.ref;
        
        bool exists = false;
        Task await = doesNameExist(out(state.i), file.name.asPtr(), out(exists));
        if(!await.isDone) 
            return Task::Pending();

        if(exists) {
            Serial.println(F("!!error!! fileName already exists"));
            return Task::Done();
        }

        state.i = 0;
        state.taskId = StoreState::WriteFAT;
        return Task::Pending();
    }

    inline Task store_writeFAT(MutRef<StoreState> stateRef, FileInfo const& file) {
        auto& state = stateRef.ref;

        int address;
        Task await = freeSpace(out(state.freeSpaceState), out(address), file.data.len());
        if(!await.isDone)
            return Task::Pending();

        if (address == ADDRESS_ERROR) {
            Serial.println(F("!!error!! not enough free space in EEPROM for file data"));
            return Task::Done();
        }

        state.fatEntry.address = u16(address);

        state.fatEntry.size = file.data.len();
        bool isSuccess = state.fatEntry.writeToEEPROM();
        if(!isSuccess)
            return Task::Done();

        state.taskId = StoreState::WriteData;
        return Task::Pending();
    }

    inline Task store_writeData(MutRef<StoreState> stateRef, FileInfo const& file) {
        auto& state = stateRef.ref;

        u16 address = state.fatEntry.address;
        u16 len = file.data.len();

        if(state.i >= len) {
            Serial.print(F("successfully stored file: `"));
            file.name.print();
            Serial.println('`');
            state.taskId = StoreState::End;
            return Task::Done();
        }

        EEPROM.write(state.i + address, file.data[state.i]);
        state.i++;
        return Task::Pending();
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