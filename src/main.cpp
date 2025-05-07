/*
Tim Kieboom 1025003
*/
#include <Arduino.h>

#include "allCommands.h"
inline void getCommand(/*out*/ InputBuffer& input, /*out*/ CommandFunc& currentCommand);

void setup() {
    Serial.begin(9600);
    delay(500);
    Serial.println(F("starting TKArduinoOS type 'help' for more information"));
    Serial.println(F("welkom my friend :)"));
    ASSERT_RAM;
    delay(500);

    Serial.print(F(">> "));
}

void loop() {
    static InputBuffer input = InputBuffer(INPUT_BUFFER_SIZE);
    static CommandFunc currentCommand = nullptr;

    if (Serial.available() > 0) {
        if(currentCommand) 
            Serial.flush();

        getCommand(/*out*/ input, /*out*/ currentCommand);
    }

    if(currentCommand) {
        Task await = currentCommand(input);
        if(await.isDone) {
            currentCommand = nullptr;
            ASSERT_RAM;
            Serial.print(F(">> "));
        }
    }
} 

inline void getCommand(/*out*/ InputBuffer& input, /*out*/ CommandFunc& currentCommand) {
    Task await = readTokens(/*out*/ input);
    if(!await.isDone) 
        return;

    ConstSpan<char> commandName = getToken(input, 0);
    CommandFunc func = getCommandFunction(allCommands, commandName);
    if(func == nullptr) {
        Serial.println(F("!!error!! command not found 'help' to see all commands"));
        Serial.print(F(">> "));
        return;
    }
    currentCommand = func;
}








