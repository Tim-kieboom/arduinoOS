#include <Arduino.h>

#define BUFFER_SIZE 12
#define COMMAND_MAX_SIZE 9
#define AMOUNT_OF_COMMANDS 1
#define TOKEN_END_INDEXES_SIZE 8

#define STRING_OF_COMMAND_NAMES "all commands:\n\t-store\n\t"

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
  uint8_t *tokenEndIndexes = new uint8_t[TOKEN_END_INDEXES_SIZE];
};

bool isEndOfToken(char ch)
{
  return isSpace(ch) || ch == '\0';
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
  Serial.println("storeTest");
}

char *getCommandName(InputBuffer *inputBuff)
{
  uint8_t tokenSize = inputBuff->tokenEndIndexes[0] + 2;

  char *firstToken = new char[tokenSize];

  for (uint8_t i = 0; i < tokenSize-1; i++)
  {
    firstToken[i] = inputBuff->buffer[i];
  }
  firstToken[tokenSize-1] = '\0';

  return firstToken;
}

void resetInputBuffer(InputBuffer *inputBuff)
{
  if (inputBuff->buffer != nullptr)
    delete[] inputBuff->buffer;

  inputBuff->buffer = new char[Serial.available()+1];

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
    addEndIndex(inputBuff, currentIndex - 2);

  if (ch == '\n')
  {
    inputBuff->buffer[currentIndex] = '\0';
    bufferInit = true;
    return true;
  }

  return false;
}

void printBuffer(InputBuffer *inputBuff)
{
  char* buffer = inputBuff->buffer;
  uint8_t i = -1;
  while(buffer[++i] != '\0')
  {
    Serial.print(buffer[i]);
  }
  Serial.println();
}

void printArray(uint8_t* array, uint8_t size)
{
  for (int i = 0; i < size; i++)
  {
    Serial.print(array[i]);
    Serial.print(", ");
  }
}

bool doCommand(CommandType* commands, char* commandName)
{
  for(int i = 0; i < AMOUNT_OF_COMMANDS; i++)
  {
    CommandType command = commands[i];
    if(strcmp(commandName, command.name) == 0)
    {
      command.func();
      return true;
    }
  }

  Serial.println("!!<error> command not found!!");
  Serial.println(STRING_OF_COMMAND_NAMES);
  return false;
}

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
    {"store", &store}
  };

  if (Serial.available() > 0)
  {
    if (readTokens(input))
    {
      char *commandName = getCommandName(input);
      doCommand(commands, commandName);
    }
  }

}