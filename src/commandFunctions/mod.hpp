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
typedef Task (*CommandFuncPtr)(MutRef<input::Buffer> input);

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

    /// Looks up a command by name and returns its function pointer, or nullptr if not found.
    CommandFuncPtr find(StrSlice name);

    #define __CMD(fn) {StrSlice(#fn, sizeof(#fn)-1), fn}

    /// Entry mapping a command name to its handler function.
    struct CommandEntry {
        StrSlice name;              ///< The command name string.
        CommandFuncPtr functionPtr; ///< Pointer to the command handler.
    };

    constexpr usize commandsLen = 13;
    
    /// Table of all available commands.
    constexpr CommandEntry commands[commandsLen] = {
        __CMD(run),
        __CMD(kill),
        __CMD(help),
        __CMD(store),
        __CMD(erase),
        __CMD(files),
        __CMD(resume),
        __CMD(restart),
        __CMD(recieve),
        __CMD(suspend),
        __CMD(sysinfo),
        __CMD(clearall),
        __CMD(freespace),
    };
}