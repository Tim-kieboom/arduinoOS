#include <Arduino.h>
#include "common.h"
#include "commandHandler/commandHandler.h"
#include "EEPROM_handler/EEPROM_handler.h"

#define COMMAND_MAX_SIZE 9
#define AMOUNT_OF_COMMANDS 2

#define WHILE_NOT_END_OF_COMMANDS(commands, i) while (strcmp(commands[i++].name, "\0") != 0)

bool doCommand(CommandType *commands, char *commandName, InputBuffer *input)
{
  uint8_t i = -1;

  WHILE_NOT_END_OF_COMMANDS(commands, i)
  {
    CommandType command = commands[i];
    if (strcmp(commandName, command.name) == 0)
    {
      command.func(input);
      return true;
    }
  }

  Serial.println("!!<error> command not found (type \'help\' to see options)!!");
  return false;
}

void setup()
{
  Serial.begin(9600);
  Serial.println("Arduino started");
}

void loop()
{
  static bool afterRead = false;
  static InputBuffer *input = new InputBuffer();
  static CommandType commands[] =
  {
    {"help", &printAllCommands},
    {"store", &store},
    {"\0", nullptr}
  };

  if (Serial.available() > 0)
  {
    if (readTokens(input))
      afterRead = true;
  }

  if (afterRead)
  {
    printArray(input->tokenEndIndexes, TOKEN_END_INDEXES_SIZE);
    char *commandName = getToken(input, 0);
    doCommand(commands, commandName, input);

    Serial.println();
    printString(getToken(input, 1));
    afterRead = false;
  }
}