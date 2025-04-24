/*
Tim Kieboom 1025003
*/
#ifndef COMMANDHANDLER_H
#define COMMANDHANDLER_H
#pragma once

#include <Arduino.h>
#include "../lib/TKardunio/TKarduino.h"
#include "../InputBuffer/InputBuffer.h"

#define INPUT_BUFFER_SIZE 35

void commandFunc_printAllCommands(InputBuffer& inputBuff);

typedef void (*CommandFunc)(InputBuffer& inputBuff);
struct CommandType {
  char name[BUFFER_SIZE];
  CommandFunc function;
};

/// @brief reads the input text from terminal and gets the indexes of the input tokens
/// @param inputBuffer buffer that stores metadata about the input and the tokens of that input
/// @return true is done reading line, false if not
bool readTokens(/*out*/InputBuffer& inputBuffer);

/// @brief get the n'th token from inputBuffer
/// @param inputBuff 
/// @param tokenIndex 
/// @return a ConstSpan of raw chars of Token
ConstSpan<char> getToken(const InputBuffer& inputBuff, const uint8_t tokenIndex);
bool doCommand(const ConstSpan<CommandType>& commands, ConstSpan<char>& commandName, InputBuffer& input);

#endif