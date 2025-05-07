/*
Tim Kieboom 1025003
*/
#ifndef EEPROM_HANDLER_H
#define EEPROM_HANDLER_H
#pragma once

#include "./commandHandler/commandHandler.h"
#include "EEPROM_header.hpp"


struct File {
  char fileName[BUFFER_SIZE];
  uint8_t size;
  char data[BUFFER_SIZE];
};

constexpr size_t SIZEOF_FAT = sizeof(uint16_t) + sizeof(uint8_t) + BUFFER_SIZE;
struct FAT {
  char fileName[BUFFER_SIZE];
  uint16_t address;
  uint8_t size;

  void fillName(ConstSpan<char>& name);
};
Task commandFunc_store(InputBuffer& input);
Task commandFunc_files(InputBuffer& input);
Task commandFunc_sysinfo(InputBuffer& input);
Task commandFunc_retrieve(InputBuffer& input);
Task commandFunc_clearall(InputBuffer& input);
Task commandFunc_freespace(InputBuffer& input);

#endif


