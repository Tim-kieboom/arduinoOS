#include "commandHandler.h"

bool isEndOfToken(char ch)
{
  return isSpace(ch) || ch == ' ';
}

bool addEndIndex(InputBuffer *inputBuff, int index)
{
  int i = 0;
  while (i < TOKEN_END_INDEXES_SIZE)
  {
    if (inputBuff->tokenEndIndexes[i] == 0)
    {
      inputBuff->tokenEndIndexes[i] = index;
      return true;
    }
    i++;
  }

  return false;
}

void printAllCommands(InputBuffer *inputBuff)
{
  Serial.println(STRING_OF_COMMAND_NAMES);
}

char *getCommandName(InputBuffer *inputBuff)
{
  uint8_t tokenSize = inputBuff->tokenEndIndexes[0] + 2;

  char *firstToken = new char[tokenSize];

  for (uint8_t i = 0; i < tokenSize - 1; i++)
  {
    firstToken[i] = inputBuff->buffer[i];
  }
  firstToken[tokenSize - 1] = '\0';

  return firstToken;
}

void resetInputBuffer(InputBuffer *inputBuff)
{
  if (inputBuff->buffer != nullptr)
    delete[] inputBuff->buffer;

  inputBuff->buffer = new char[Serial.available() + 1];

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

  if (isEndOfToken(ch))
  {
    addEndIndex(inputBuff, currentIndex - 2);
    currentIndex++;
  }

  if (ch == '\n')
  {
    inputBuff->buffer[currentIndex] = '\0';
    bufferInit = true;
    return true;
  }

  return false;
}

char *getToken(InputBuffer *inputBuff, uint8_t tokenIndex)
{
  uint8_t tokenSize = inputBuff->tokenEndIndexes[tokenIndex] + 2;

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

  for (uint8_t i = beginIndex; i < tokenSize - 1; i++)
  {
    firstToken[i] = inputBuff->buffer[i];
  }
  firstToken[tokenSize - 1] = '\0';

  return firstToken;
}