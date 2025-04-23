/*
Tim Kieboom 1025003
*/
#include <Arduino.h>
#include "InputBuffer/InputBuffer.h"
#include "EEPROM_handler/EEPROM_handler.h"
#include "commandHandler/commandHandler.h"

CommandType _ptr_allCommands[] = {
    {"help", &commandFunc_printAllCommands},
    {"store", &commandFunc_store}
};
size_t _len_allCommands = sizeof(_ptr_allCommands) / sizeof(CommandType);

void setup() {
    Serial.begin(9600);


}

void loop() {
    static ConstSpan<CommandType> allCommands = ConstSpan<CommandType>(_ptr_allCommands, 0, _len_allCommands);
    static InputBuffer input = InputBuffer(SmartArray<char>(new char[50], 50));

    if (Serial.available() > 0) {

        if (readTokens(/*out*/ input)) {
            int i = 0;
            while(input.buffer[i++] != '\0'){}
            Serial.println(input.buffer.constSpan(0, 50).copy_asCstr());

            ConstSpan<char> commandName = getToken(input, 0);
            doCommand(allCommands, commandName, input);
        }
    }
}