#include <Arduino.h>
#include "common.h"
#include "commandHandler/commandHandler.h"
#include "EEPROM_handler/EEPROM_handler.h"

#define COMMAND_MAX_SIZE 9
#define AMOUNT_OF_COMMANDS 2

void setup()
{
  Serial.begin(9600);
  Serial.println("Arduino started");
}

void loop()
{
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
    {
      char *commandName = getToken(input, 0);
      doCommand(commands, commandName, input);
    }
  }
}