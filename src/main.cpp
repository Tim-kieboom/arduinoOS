/*
Tim Kieboom 1025003
*/
#include <TKarduino.hpp> 
#include "commandFunctions/mod.hpp"
#include "input/readCommand.hpp"


inline bool hasSerialInput();
inline bool hasCommand(CommandFunctionsPtr command);

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
    static auto input = input::Buffer();
    static auto command = NO_COMMAND;

    if(hasSerialInput())
        input::readCommand(out(input), out(command));

    if(hasCommand(command))
        commandFunctions::run(out(input), out(command));
}

inline bool hasCommand(CommandFunctionsPtr command) {
    return command != NO_COMMAND;
}

inline bool hasSerialInput() {
    return Serial.available() > 0;
}





