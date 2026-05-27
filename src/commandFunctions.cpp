/*
Tim Kieboom 1025003
*/
#include "commandFunctions.hpp"

const Fstr* getHelpMessage();
namespace commandFunctions {
    Task help(MutRef<input::Buffer> input) {
        Serial.println(getHelpMessage());
        return Task::Done();
    }

    Task restart(MutRef<input::Buffer> input) {
        Serial.println(F("goodbye :)"));
        RESET_ARDUINO;
    }
    
    Task run(MutRef<input::Buffer> input) {
        TODO;
    }

    Task kill(MutRef<input::Buffer> input) {
        TODO;
    }

    Task store(MutRef<input::Buffer> input) {
        TODO;
    }

    Task erase(MutRef<input::Buffer> input) {
        TODO;
    }

    Task files(MutRef<input::Buffer> input) {
        TODO;
    }

    Task resume(MutRef<input::Buffer> input) {
        TODO;
    }

    Task suspend(MutRef<input::Buffer> input) {
        TODO;
    }

    Task recieve(MutRef<input::Buffer> input) {
        TODO;
    }

    Task sysinfo(MutRef<input::Buffer> input) {
        TODO;
    }

    Task clearall(MutRef<input::Buffer> input) {
        TODO;
    }

    Task freespace(MutRef<input::Buffer> input) {
        TODO;
    }

    CommandFunc find(Slice<char> name) {

        for(usize i = 0; i < commandsLen; i++) {
            auto command = commands[i];
            
            if(command.name.equal(name)) {
                return command.functionPtr;
            }
        }
        
        return nullptr;
    }
}

const Fstr* getHelpMessage() {
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
        "\n\t// ??"
        "\n\t-files"                                       
        "\n"
        "\n\t// ??"
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
