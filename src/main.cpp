/*
Tim Kieboom 1025003
*/
#include <Arduino.h>
#include "allCommands.h"
inline void readAndRunLine(/*out*/ InputBuffer& input, /*out*/CommandFunc& currentCommand);

void setup() {
    Serial.begin(9600);
    delay(500);
    Serial.println("starting TKArduinoOS type 'help' for more information");
    Serial.print(">> ");
}

void loop() {
    static InputBuffer input = InputBuffer(SmartArray<char>(30));
    static CommandFunc currentCommand = nullptr;

    if (Serial.available() > 0) {
        if(currentCommand) 
            Serial.flush();

        readAndRunLine(/*out*/ input, /*out*/currentCommand);
    }

    if(currentCommand) {
        bool isDone = currentCommand(input);
        if(isDone)
        {
            currentCommand = nullptr;
            Serial.print(">> ");
        }
    }
} 

inline void readAndRunLine(/*out*/ InputBuffer& input, /*out*/CommandFunc& currentCommand) {
    bool isSuccess = readTokens(/*out*/ input);
    if(!isSuccess) 
        return;

    ConstSpan<char> commandName = getToken(input, 0);
    CommandFunc func = getCommandFunction(allCommands, commandName);
    if(!func) {
        Serial.println("!!error!! command is not found type 'help' to see a list of all commands");
        Serial.print(">> ");
        return;
    }
    currentCommand = func;
    Serial.print(">> ");
}








