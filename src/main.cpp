#include <Arduino.h>

#define BUFFER_SIZE 12
#define COMMAND_MAX_SIZE 9 

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
  char* buffer;
  uint8_t size;
  uint8_t currentIndex;

  uint8_t* tokenEndIndexes = new uint8_t[8];
};

bool isEndOfToken(char ch)
{
  return ch == ' ' || ch == '\n';
}

bool addEndIndex(InputBuffer* inputBuff, int index)
{
  int i = 0;
  while(inputBuff->tokenEndIndexes[i++] != 0)
  {
    if(i > 8)
      return false;
  }

  inputBuff->tokenEndIndexes[i] = index;
  return true;
}

void store()
{

}

char* getCommandName(InputBuffer* inputBuff)
{
  uint8_t tokenSize = inputBuff->tokenEndIndexes[0]+1;
  char* firstToken = new char[tokenSize];

  for(uint8_t i = 0; i < tokenSize; i++)
  {
    firstToken[i] = inputBuff->buffer[i]; 
  }

  return firstToken;
}

void resetInputBuffer(InputBuffer* inputBuff)
{
  if(inputBuff->buffer != nullptr)
    delete[] inputBuff->buffer;

  inputBuff->size = Serial.available();
  inputBuff->buffer = new char[inputBuff->size];
  inputBuff->currentIndex = 0;

  Serial.println(inputBuff->size);
}

bool readTokens(InputBuffer* inputBuff)
{
  static bool bufferInit = true;

  if(bufferInit)
  {
    resetInputBuffer(inputBuff);
    bufferInit = false;
  }

  uint8_t currentIndex = inputBuff->currentIndex;
  char ch = Serial.read();
  
  inputBuff->buffer[currentIndex] = ch;

  if(isEndOfToken(ch))
    addEndIndex(inputBuff, currentIndex-1);

  Serial.print("index: ");
  Serial.println(inputBuff->currentIndex);

  inputBuff->currentIndex++;
  if(inputBuff->currentIndex >= inputBuff->size)
  {
    Serial.println("!!true!!");
    bufferInit = true;
    return true;
  }

  return false;
}

void printBuffer(InputBuffer* inputBuff)
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
  static InputBuffer* input = new InputBuffer();
  static CommandType command[] =
  {
    {"store", &store}
  };

  if(Serial.available() > 0)
  {
    if(readTokens(input))
    {
      // printBuffer(input);
      // char* commandName = getCommandName(input);
      // Serial.println(commandName);
    }
  }
}