/*
Tim Kieboom 1025003
*/
#include "mod.hpp"
#include "common.hpp"
#include "asyncStates.hpp"

inline const Fstr* getHelpMessage();
inline bool hasArgumentCount(input::Buffer const& input, int shouldBe);

namespace commandFunctions {
    static Task innerStore(MutRef<input::Buffer> inputRef, MutRef<StoreState> stateRef);
    
    Task help(MutRef<input::Buffer> input) {
        if(!hasArgumentCount(input.ref, 0))
            return Task::Done();

        Serial.println(getHelpMessage());
        return Task::Done();
    }

    Task restart(MutRef<input::Buffer> input) {
        if(!hasArgumentCount(input.ref, 0))
            return Task::Done();

        delay(50);
        RESET_ARDUINO;
    }
    
    Task run(MutRef<input::Buffer> input) {
        if(!hasArgumentCount(input.ref, 1))
            return Task::Done();

        TODO;
    }

    Task kill(MutRef<input::Buffer> input) {
        if(!hasArgumentCount(input.ref, 1))
            return Task::Done();

        TODO;
    }

    Task store(MutRef<input::Buffer> input) {
        static auto state = StoreState();

        Task await = innerStore(input, out(state));
        if(await.isDone)
            state = StoreState();
        
        return await;
    }

    Task erase(MutRef<input::Buffer> input) {
        if(!hasArgumentCount(input.ref, 1))
            return Task::Done();

        TODO;
    }

    Task files(MutRef<input::Buffer> input) {
        if(!hasArgumentCount(input.ref, 0))
            return Task::Done();

        TODO;
    }

    Task resume(MutRef<input::Buffer> input) {
        if(!hasArgumentCount(input.ref, 1))
            return Task::Done();

        TODO;
    }

    Task suspend(MutRef<input::Buffer> input) {
        if(!hasArgumentCount(input.ref, 1))
            return Task::Done();

        TODO;
    }

    Task recieve(MutRef<input::Buffer> input) {
        if(!hasArgumentCount(input.ref, 1))
            return Task::Done();

        TODO;
    }

    Task sysinfo(MutRef<input::Buffer> input) {
        if(!hasArgumentCount(input.ref, 0))
            return Task::Done();

        auto kb = F("\tBytes|\n");
        auto indent F("      "); 
        auto files = F("\t\tFiles|\n");
        auto seperator = F("------------------------\n");
        
        auto freeRam = getAmountOfFreeRam();
        auto filesLen = fileSystem::FAT::numFiles();
        auto filesMax = fileSystem::EEPROM_Header::FAT_MAX_NUM_FILES;
        printM(
            seperator,
            F("version: "), ARDUINO_OS_VERSION_MAJOR, '.', ARDUINO_OS_VERSION_MINOR, '\n',
            F("platform: "), ARDUINO_OS_PLATFORM, '\n',
            F("[FAT] space max:    |"), filesMax, files, 
                indent, F("space left:   |"), filesMax - filesLen, files,
                indent, F("amount files: |"), filesLen, files,
            F("[RAM] total memory: |"), RAM_AMOUNT, kb, 
                indent, F("free memory:  |"), freeRam, kb,
                indent, F("used memory:  |"), RAM_AMOUNT - freeRam, kb,
            seperator
        );

        return Task::Done();
    }

    Task clearall(MutRef<input::Buffer> input) {
        if(!hasArgumentCount(input.ref, 0))
            return Task::Done();

        fileSystem::clearFAT();
        return Task::Done();
    }

    Task freespace(MutRef<input::Buffer> input) {
        if(!hasArgumentCount(input.ref, 0))
            return Task::Done();

        TODO;
    }

    CommandFuncPtr find(StrSlice name) {

        for(usize i = 0; i < commandsLen; i++) {
            auto command = commands[i];
            
            if(command.name.equal(name)) {
                return command.functionPtr;
            }
        }
        
        return nullptr;
    }

    static Task innerStore(MutRef<input::Buffer> inputRef, MutRef<StoreState> stateRef) {
        auto& input = inputRef.ref;
        auto& state = stateRef.ref;

        if(state.first) {
            if(!hasArgumentCount(input, 3)) {
                return Task::Done();
            }

            auto error = input.getToken(out(state.file.name), 1);
            if(error) {
                printM(F("!!error!! tried to get first argument\n"), error, '\n');
                return Task::Done();
            }
            
            StrSlice sizeStr;
            error = input.getToken(out(sizeStr), 2);
            if(error) {
                printM(F("!!error!! tried to get second argument\n"), error, '\n');
                return Task::Done();
            }

            error = input.getToken(out(state.file.data), 3);
            if(error) {
                printM(F("!!error!! tried to get third argument\n"), error, '\n');
                return Task::Done();
            }

            error = parseU8(sizeStr, out(state.file.size));
            if(error) {
                Serial.print(F("!!error!! tried to parse "));
                sizeStr.println();
                Serial.println(error);
                return Task::Done();
            } 

            state.first = false;
        }

        Task await = fileSystem::store(out(state.storeState), state.file);
        if(await.isDone) {
            return Task::Done();
        }

        return Task::Pending();
    }
}

/** Validates that the input has the expected number of arguments (excluding the command name). */
inline bool hasArgumentCount(input::Buffer const& input, int shouldBe) {
    if(input.tokenEndsLen-1 != shouldBe){
        
        int numArgs = (input.tokenEndsLen==0) ? 0 : input.tokenEndsLen-1;
        printM(F("!!error!! command should have "), shouldBe, F(" arguments has: '"), numArgs, F("'\n"));
        return false;
    }
    return true;
}

inline const Fstr* getHelpMessage() {
    return F(
    "all commands:"
        "\n\t// prints all commands"
        "\n\t-help"                                       
        "\n"
        "\n\t// creates a file and stores it in the FAT"
        "\n\t-store \t\t<file_name> <file_size> <data>"      
        "\n"
        "\n\t// get file"
        "\n\t-retrieve \t<file_name>"                     
        "\n"
        "\n\t// remove file"
        "\n\t-erase \t\t<file_name>"                      
        "\n"
        "\n\t// show all files"
        "\n\t-files"                                       
        "\n"
        "\n\t// show free space in fileSystem"
        "\n\t-freespace"                                  
        "\n"
        "\n\t// run program in file"
        "\n\t-run \t\t<file_name>"                        
        "\n"
        "\n\t// pause program"
        "\n\t-suspend \t<process_id>"                     
        "\n"
        "\n\t// resume program"
        "\n\t-resume \t<process_id>"                     
        "\n"
        "\n\t// stop program"
        "\n\t-kill \t\t<process_id>"
        "\n"
        "\n\t// fills WHOLE EEPROM with 0"
        "\n\t-clearall"
        "\n"
        "\n\t// restart OS"
        "\n\t-restart"
        "\n"
        "\n\t// show info of current state of system (RAM left, files left ect..)"
        "\n\t-sysinfo"
    );
}
