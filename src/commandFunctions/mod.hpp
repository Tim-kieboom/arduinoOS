/*
Tim Kieboom 1025003
*/
#pragma once
#include <Tkarduino.hpp>
#include "input/mod.hpp"
/*
all commands: 
    // prints all cammands                               
    -help

    // creates a file and stores it in the FAT                  
    -store <file_name> <file_size> <data>

    // get file
    -retrieve <file_name>  

    // remove file
    -erase <file_name>  

    // show all files
    -files

    // ??
    -freespace  

    // run program in file
    -run <file_name>    

    // pause program
    -suspend <process_id> 

    // resume prgram
    -resume <process_id>

    // stop program
    -kill <process_id>

    // fills WHOLE EEPROM with 0
    -clear

    -restart

    // show info of current state of system (RAM left, files left ect..)
    -sysinfo
*/

/// Function pointer type for all shell command implementations.
typedef Task (*CommandFunctionsPtr)(MutRef<input::Buffer> input);
constexpr CommandFunctionsPtr NO_COMMAND = nullptr; 

namespace commandFunctions {

    /** @brief Runs a program stored in a file. */
    Task run(MutRef<input::Buffer> input);

    /** @brief Terminates a running process by ID. */
    Task kill(MutRef<input::Buffer> input);

    /** @brief Prints the help message listing all commands. */
    Task help(MutRef<input::Buffer> input);

    /** @brief Stores a new file in the file system. */
    Task store(MutRef<input::Buffer> input);

    /** @brief Removes a file by name. */
    Task erase(MutRef<input::Buffer> input);

    /** @brief Lists all stored files. */
    Task files(MutRef<input::Buffer> input);

    /** @brief Resumes a suspended process by ID. */
    Task resume(MutRef<input::Buffer> input);

    /** @brief Restarts the OS. */
    Task restart(MutRef<input::Buffer> input);

    /** @brief Suspends a running process by ID. */
    Task suspend(MutRef<input::Buffer> input);

    /** @brief Receives/retrieves a file by name. */
    Task recieve(MutRef<input::Buffer> input);

    /** @brief Displays system information (RAM, files, version, etc.). */
    Task sysinfo(MutRef<input::Buffer> input);

    /** @brief Clears all data from the file system. */
    Task clearall(MutRef<input::Buffer> input);

    /** @brief Shows available free space on the system. */
    Task freespace(MutRef<input::Buffer> input);

    /// Looks up a command by name and returns its function pointer in ouput, or nullptr if not found.
    Task find(StrSlice name, MutRef<usize> i, MutRef<CommandFunctionsPtr> output);

    /**
     * @brief Runs the active command function.
     * @param inputRef  Mutable reference to the input buffer.
     * @param commandRef Mutable reference to the current command pointer.
     * 
     * Clears the command and prints the prompt prefix when the command task is done.
     */
    void run(MutRef<input::Buffer> inputRef, MutRef<CommandFunctionsPtr> commandRef);

    #define __NEW_CMD(fn) {StrSlice(#fn, sizeof(#fn)-1), fn}
    /// Entry mapping a command name to its handler function.
    struct CommandEntry {
        StrSlice name;                      ///< The command name string.
        CommandFunctionsPtr functionPtr;    ///< Pointer to the command handler.
    };

    constexpr usize commandsLen = 13;

    /// Table of all available commands.
    constexpr CommandEntry commands[commandsLen] = {
        __NEW_CMD(run),
        __NEW_CMD(kill),
        __NEW_CMD(help),
        __NEW_CMD(store),
        __NEW_CMD(erase),
        __NEW_CMD(files),
        __NEW_CMD(resume),
        __NEW_CMD(restart),
        __NEW_CMD(recieve),
        __NEW_CMD(suspend),
        __NEW_CMD(sysinfo),
        __NEW_CMD(clearall),
        __NEW_CMD(freespace),
    };
}