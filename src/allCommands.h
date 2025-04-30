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

    // clear WHOLE EEPROM
    -clear

    -restart
*/

const CommandType _ptr_allCommands[] = {
    {"help", &commandFunc_printAllCommands},
    {"store", &commandFunc_store},
    {"clearall", &commandFunc_clearEEPROM},
    {"restart", &commandFunc_restart},

    {"temp", &commandFunc_temp}
};
const size_t _len_allCommands = sizeof(_ptr_allCommands) / sizeof(CommandType);
const ConstSpan<CommandType> allCommands = ConstSpan<CommandType>(_ptr_allCommands, 0, _len_allCommands);

#endif









