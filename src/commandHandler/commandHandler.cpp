#include "commandHandler.h"

bool isEndOfToken(char ch)
{
  return isSpace(ch) || ch == ' ';
}

bool addEndIndex(InputBuffer *inputBuff, int value)
{
  for(int i = 0; i < TOKEN_END_INDEXES_SIZE; i++)
  {
    if (inputBuff->tokenEndIndexes[i] == 0)
    {
      inputBuff->tokenEndIndexes[i] = value;
      return true;
    }
  }

  return false;
}

void printAllCommands(InputBuffer *inputBuff)
{
  Serial.println(STRING_OF_COMMAND_NAMES);
}

void resetInputBuffer(InputBuffer *inputBuff)
{
  if (inputBuff->buffer != nullptr)
    delete[] inputBuff->buffer;

  inputBuff->buffer = new char[Serial.available() + 1];
  inputBuff->buffer[Serial.available()] = '\0';

  for (uint8_t i = 0; i < TOKEN_END_INDEXES_SIZE; i++)
    inputBuff->tokenEndIndexes[i] = 0;
}

bool readTokens(InputBuffer *inputBuff)
{
  static bool bufferInit = true;
  static uint8_t currentIndex = 0;

  if (bufferInit)
  {
    resetInputBuffer(inputBuff);
    bufferInit = false;
    currentIndex = 0;
  }

  char ch = Serial.read();

  inputBuff->buffer[currentIndex++] = ch;

  if (ch == '\n' || ch == '\0')
  {
    bufferInit = true;
    return true;
  }

  if (isEndOfToken(ch))
    addEndIndex(inputBuff, currentIndex - 2);

  return false;
}

char *getToken(InputBuffer *inputBuff, uint8_t tokenIndex)
{
  uint8_t tokenSize = inputBuff->tokenEndIndexes[tokenIndex] + 1;

  if (inputBuff->tokenEndIndexes[tokenIndex] == 0)
    return new char[1]{'\0'};

  uint8_t beginIndex;

  if (tokenIndex > 0)
  {
    beginIndex = inputBuff->tokenEndIndexes[tokenIndex - 1] + 2;
  }
  else
  {
    beginIndex = 0;
  }

  char *firstToken = new char[tokenSize];

  uint8_t j = 0;
  for (uint8_t i = beginIndex; i < tokenSize; i++)
  {
    firstToken[j++] = inputBuff->buffer[i];
  }
  firstToken[j] = '\0';

  return firstToken;
}

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