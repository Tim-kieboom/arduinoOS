#ifndef COMMANDHANDLER_H
#define COMMANDHANDLER_H
#pragma once

#include <Arduino.h>
#include "common.h"

#define WHILE_NOT_END_OF_COMMANDS(commands, i) while(strcmp(commands[i++].name, "\0") != 0)

#define STRING_OF_COMMAND_NAMES                   \
"all commands:                                    \
  \n\t-help                                       \
  \n\t-store \t\t<file_name> <file_size> <data>   \
  \n\t-retrieve \t<file_name>                     \
  \n\t-erase \t\t<file_name>                      \
  \n\t-file                                       \
  \n\t-freespace                                  \
  \n\t-run \t\t<file_name>                        \
  \n\t-suspend \t<process_id>                     \
  \n\t-resume \t<process_id>                      \
  \n\t-kill \t\t<process_id>"

struct CommandType
{
  char name[BUFFER_SIZE];
  void (*func)(InputBuffer *inputBuff);
};

bool readTokens(InputBuffer *inputBuff);
void printAllCommands(InputBuffer *inputBuff);
char *getToken(InputBuffer *inputBuff, uint8_t tokenIndex);
bool doCommand(CommandType *commands, char *commandName, InputBuffer *input);

#endif