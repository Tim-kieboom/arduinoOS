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

    // ??
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

typedef Task (*CommandFunc)(MutRef<input::Buffer> input);
namespace commandFunctions {

    Task run(MutRef<input::Buffer> input);
    Task kill(MutRef<input::Buffer> input);
    Task help(MutRef<input::Buffer> input);
    Task store(MutRef<input::Buffer> input);
    Task erase(MutRef<input::Buffer> input);
    Task files(MutRef<input::Buffer> input);
    Task resume(MutRef<input::Buffer> input);
    Task restart(MutRef<input::Buffer> input);
    Task suspend(MutRef<input::Buffer> input);
    Task recieve(MutRef<input::Buffer> input);
    Task sysinfo(MutRef<input::Buffer> input);
    Task clearall(MutRef<input::Buffer> input);
    Task freespace(MutRef<input::Buffer> input);

    CommandFunc find(Slice<char> name);

    #define __CMD(fn) {Slice<char>(#fn, sizeof(#fn)-1), fn}
    struct CommandEntry {
        Slice<char> name; 
        CommandFunc functionPtr;
    };

    constexpr usize commandsLen = 13;
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