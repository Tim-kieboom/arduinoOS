#include <Arduino.h>

#define BUFFER_SIZE 12
#define COMMAND_MAX_SIZE 9
#define TOKEN_END_INDEXES_SIZE 8

typedef struct File
{
  char fileName[BUFFER_SIZE];
  uint8_t size;
  char data[BUFFER_SIZE];
};

typedef struct CommandType
{
  char name[BUFFER_SIZE];
  void (*func)();
};

typedef struct InputBuffer
{
  char *buffer;
  uint8_t size;

  uint8_t *tokenEndIndexes = new uint8_t[TOKEN_END_INDEXES_SIZE];
};

bool isEndOfToken(char ch)
{
  return ch == ' ' || ch == '\n';
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

void store()
{
}

char *getCommandName(InputBuffer *inputBuff)
{
  uint8_t tokenSize = inputBuff->tokenEndIndexes[0] + 1;

  char *firstToken = new char[tokenSize];

  for (uint8_t i = 0; i < tokenSize; i++)
  {
    firstToken[i] = inputBuff->buffer[i];
  }

  return firstToken;
}

void resetInputBuffer(InputBuffer *inputBuff)
{
  inputBuff->size = Serial.available();

  if (inputBuff->buffer != nullptr)
    delete[] inputBuff->buffer;

  inputBuff->buffer = new char[Serial.available()];

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
  }

  if (currentIndex >= inputBuff->size)
  {
    bufferInit = true;
    return true;
  }

  return false;
}

void printBuffer(InputBuffer *inputBuff)
{
  for (int i = 0; i < inputBuff->size; i++)
  {
    Serial.print(inputBuff->buffer[i]);
  }
}

void setup()
{
  Serial.begin(9600);
  Serial.println("Arduino started");
}

void loop()
{
  static InputBuffer *input = new InputBuffer();
  static CommandType command[] = {
      {"store", &store}};

  if (Serial.available() > 0)
  {
    if (readTokens(input))
    {
      printBuffer(input);
      // char *commandName = getCommandName(input);
      // for (int i = 0; i < 3; i++)
      // {
      //   char ch = commandName[i];
      //   Serial.println(ch);
      // }
    }
  }
}