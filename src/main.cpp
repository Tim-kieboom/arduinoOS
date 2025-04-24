/*
Tim Kieboom 1025003
*/
#include <Arduino.h>
#include "allCommands.h"
void readAndRunLine(/*out*/ InputBuffer& input);

void setup() {
    Serial.begin(9600);
    delay(500);
    Serial.println("starting TKArduinoOS type 'help' for more information");
    Serial.print(">> ");
}

void loop() {
    static InputBuffer input = InputBuffer(SmartArray<char>(25));

    if (Serial.available() > 0)
        readAndRunLine(/*out*/ input);
}

void readAndRunLine(/*out*/ InputBuffer& input) {

    bool isSuccess = readTokens(/*out*/ input);
    if(!isSuccess) 
        return;

    ConstSpan<char> commandName = getToken(input, 0);
    if(!doCommand(allCommands, commandName, input))
        Serial.println("command: is not found type 'help' to see a list of all commands");

    Serial.print(">> ");
}






