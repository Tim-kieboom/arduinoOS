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

    // fills WHOLE EEPROM with 0
    -clear

    -restart

    // show info of current state of system (RAM left, files left ect..)
    -sysinfo
*/
#include "EEPROM_handler/hasFileStore/hasFileStore.h"
Task temp(InputBuffer& input) {
    uint8_t len = EEPROM.read(EEPROM_Header::NUM_FILES_INDEX);
    printM(F("number of files: "), len, '\n');

    for(size_t i = 0; i < EEPROM_Header::FILE_STORE_LEN; i++) {
      
        uint8_t byte = EEPROM.read(i + EEPROM_Header::FILE_STORE_INDEX);
        Serial.print(byte, BIN);
        Serial.print(' ');
    }

    printM(
        '\n',
        F("firstEmpty: "), FileStore::getFirstEmpty(), '\n'
    );
    return Done;
}

const CommandType _ptr_allCommands[] = {
    {"help", &commandFunc_printAllCommands},
    {"store", &commandFunc_store},
    {"retrieve", &commandFunc_retrieve},
    {"erase", &commandFunc_erase},
    {"files", &commandFunc_files},
    {"freespace", &commandFunc_freespace}, //todo
    {"clearall", &commandFunc_clearall},
    {"restart", &commandFunc_restart},
    {"sysinfo", &commandFunc_sysinfo},

    {"temp", &temp},
};
const size_t _len_allCommands = sizeof(_ptr_allCommands) / sizeof(CommandType);
const ConstSpan<CommandType> allCommands = ConstSpan<CommandType>(_ptr_allCommands, 0, _len_allCommands);

#endif









