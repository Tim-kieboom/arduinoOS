#include "arduinoOSDebugger.h"

void printString(char *string)
{
  uint8_t i = -1;
  while (string[++i] != '\0')
  {
    Serial.print(string[i]);
  }
  Serial.println();
}

void printArray(uint8_t *array, uint8_t size)
{
  for (int i = 0; i < size; i++)
  {
    Serial.print(array[i]);
    Serial.print(", ");
  }
}
