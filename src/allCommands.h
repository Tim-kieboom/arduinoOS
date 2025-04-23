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



#endif