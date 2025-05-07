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

/// @brief struct that is used for async functions if Task.isDone is true function is done
struct Task {bool isDone; };
constexpr Task Done = {true};
constexpr Task NotDone = {false};

Task commandFunc_printAllCommands(InputBuffer& inputBuff);
Task commandFunc_restart(InputBuffer& inputBuff);

/// @brief abtract function ptr for all commands
typedef Task (*CommandFunc)(InputBuffer& inputBuff);
struct CommandType {
  char name[BUFFER_SIZE];
  CommandFunc function;
};

/// @brief reads the input text from terminal and gets the indexes of the input tokens
/// @param inputBuffer buffer that stores metadata about the input and the tokens of that input
Task readTokens(/*out*/InputBuffer& inputBuffer);

/// @brief get the n'th token from inputBuffer
/// @param inputBuff 
/// @param tokenIndex 
ConstSpan<char> getToken(const InputBuffer& inputBuff, const uint8_t tokenIndex);
CommandFunc getCommandFunction(const ConstSpan<CommandType>& commands, ConstSpan<char>& commandName);

#endif