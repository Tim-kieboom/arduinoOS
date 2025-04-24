/*
Tim Kieboom 1025003
*/
#ifndef ALLCOMMANDS
#define ALLCOMMANDS
#pragma once

#include "EEPROM_handler/EEPROM_handler.h"
/*
all commands: 
    // prints all cammands                               
    -help

    // creates a file and stores it in the FAT                  
    -store <file_name> <file_size> <data>

    // creates a file and able to write lines
    -write <file_name>

    // print all files in FAT
    -dir

    // get file
    -retrieve <file_name>  

    // remove file
    -erase <file_name>  

    // ??
    -file

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
*/

const CommandType _ptr_allCommands[] = {
    {"help", &commandFunc_printAllCommands},
    {"store", &commandFunc_store}
};
const size_t _len_allCommands = sizeof(_ptr_allCommands) / sizeof(CommandType);
const ConstSpan<CommandType> allCommands = ConstSpan<CommandType>(_ptr_allCommands, 0, _len_allCommands);

#endif