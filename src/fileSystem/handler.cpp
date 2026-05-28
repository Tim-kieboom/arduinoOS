#include "mod.hpp"
inline bool isErr(String& str);
static const Fstr* checkName(StrSlice& name);

bool strEquals(const char* self, const char* other) {
    return strcmp(self, other) == 0;
}

namespace fileSystem {
    static Task innerStore(MutRef<StoreState> stateRef, FileInfo file);
    static Task doesNameExist(MutRef<u8> iRef, const char* name, MutRef<bool> exists);

    void clearFAT() {
        FAT::clearall();

        Serial.println(F("disk cleared"));
        Serial.flush();
        delay(20);
        RESET_ARDUINO;
    }

    Task store(MutRef<StoreState> state, FileInfo file) {
        Task await = innerStore(state, file);
        if(await.isDone)
            state.ref = StoreState();
        
        return await;
    }

    static Task innerStore(MutRef<StoreState> stateRef, FileInfo file) {
        auto& state = stateRef.ref;

        auto error = checkName(file.name);
        if(error) {
            Serial.print(error);
            return Task::Done();
        }
        
        u8 numFiles = FAT::numFiles();
        if(numFiles >= EEPROM_Header::FAT_MAX_NUM_FILES) {
            Serial.println(F("!!error!! not more space left to store another file"));
            return Task::Done();
        }

        state.fatEntry.setName(file.name);

        bool exists = false;
        Task await = doesNameExist(mutRef(state.i), file.name.asPtr(), mutRef(exists));
        if(!await.isDone) 
            return Task::Pending();

        if (exists) {
            Serial.println(F("!!error!! fileName already exists"));
            return Task::Done();
        }

        bool isSuccess = state.fatEntry.writeToEEPROM();
        if(isSuccess) {
            Serial.print(F("successfully stored file: `"));
            file.name.print();
            Serial.println('`');
        }

        return Task::Done();
    }
    
    static Task doesNameExist(MutRef<u8> iRef, const char* name, MutRef<bool> exists) {
        auto& i = iRef.ref;
        exists.ref = false;
        
        u8 len = FAT::numFiles();
        if(len == 0)
            return Task::Done();
        
        if(i >= len) {
            i = 0;
            return Task::Done();
        }
        
        char nameBuffer[DATA_SIZE];
        if(!FAT::entry_name(i, mutRef(nameBuffer)))
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

inline bool isErr(String const& str) {
    return str.length() > 0;
}

inline bool isNumber(char ch) {
    int num = ch - '0';
    return num > -1 && num < 10;
}

static const Fstr* checkName(StrSlice& name) {
    static const char* illigalChars = "~`!@#$%^&*()-+={}|[]\\;':\",./<>?";
    
    if(name.len() == 0)
        return F("!!error!! name is empty");

    if (name.len() > 11)
        return F("!!error!! name of file is to big, file names can not be bigger then 11 symbools");

    if(isNumber(name[0]))
        return F("!!error!! name of file can not start with a number");

    for(usize i = 0; i < name.len(); i++) {

        if (strchr(illigalChars, name[i]) != NULL)
            return F("!!error!! name of file can not contain any special chars");
    }

    return nullptr;
}