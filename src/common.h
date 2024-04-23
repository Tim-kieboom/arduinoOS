#ifndef COMMON_H
#define COMMON_H
#pragma once

#include <Arduino.h>
#include "arduinoOSDebugger/arduinoOSDebugger.h"
#define TOKEN_END_INDEXES_SIZE 4
#define BUFFER_SIZE 12

struct InputBuffer
{
  char *buffer;
  uint8_t *tokenEndIndexes = new uint8_t[TOKEN_END_INDEXES_SIZE];
};



#endif
