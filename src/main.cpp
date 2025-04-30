/*
Tim Kieboom 1025003
*/
#include <Arduino.h>
#include "allCommands.h"
inline void readAndRunLine(/*out*/ InputBuffer& input, /*out*/CommandFunc& currentCommand);

void setup() {
    Serial.begin(9600);
    delay(500);
    Serial.println(F("starting TKArduinoOS type 'help' for more information"));
    Serial.println(F("welkom my friend :)"));

    IF_DEBUG(
        uint8_t len = 0;
        EEPROM.get(EEPROM_HEADER_FAT_LEN_INDEX, /*out*/len);
        Serial.print(F("(debug only) current FAT.len: ")); 
        Serial.println(len);
    )
    delay(500);

    Serial.print(F(">> "));
}

void loop() {
    static InputBuffer input = InputBuffer(INPUT_BUFFER_SIZE);
    static CommandFunc currentCommand = nullptr;

    if (Serial.available() > 0) {
        if(currentCommand) 
            Serial.flush();

        readAndRunLine(/*out*/ input, /*out*/currentCommand);
    }

    if(currentCommand) {
        bool isDone = currentCommand(input);
        if(isDone) {
            currentCommand = nullptr;
            Serial.print(F(">> "));
        }
    }
} 

inline void readAndRunLine(/*out*/ InputBuffer& input, /*out*/CommandFunc& currentCommand) {
    bool isSuccess = readTokens(/*out*/ input);
    if(!isSuccess) 
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








