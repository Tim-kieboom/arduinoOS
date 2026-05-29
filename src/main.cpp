/*
Tim Kieboom 1025003
*/
#include <TKarduino.hpp> 
#include "commandFunctions/mod.hpp"
#include "input/readCommand.hpp"

inline bool hasSerialInput();

void setup() {
    Serial.begin(9600);
    delay(500);
    Serial.println(F("starting TKArduinoOS type 'help' for more information"));
    Serial.println(F("welcom my friend :)"));
    ASSERT_PRINT_RAM;
    delay(500);

    Serial.print(F(">> "));
}

void loop() {
    static input::Buffer input = input::Buffer();
    static CommandFunctionsPtr command = NO_COMMAND;

    if(command != NO_COMMAND) {
        commandFunctions::run(out(input), out(command));
    } 
    else if(hasSerialInput()) {
        input::readCommand(out(input), out(command));
    }
}

inline bool hasSerialInput() {
    return Serial.available() > 0;
}





