/*
Tim Kieboom 1025003
*/
#include <TKarduino.hpp> 
#include "commandFunctions/mod.hpp"
void getCommand(MutRef<input::Buffer> inputRef, MutRef<CommandFuncPtr> commandRef);

void setup() {
    Serial.begin(9600);
    delay(500);
    Serial.println(F("starting TKArduinoOS type 'help' for more information"));
    Serial.println(F("welkom my friend :)"));
    ASSERT_PRINT_RAM;
    delay(500);

    Serial.print(F(">> "));
}

void loop() {
    static auto input = input::Buffer();
    static CommandFuncPtr command = nullptr;

    if (Serial.available() > 0) {
        if(command)
            Serial.flush();

        getCommand(mutRef(input), mutRef(command));
    }

    if(command) {
        Task await = command(mutRef(input));
        if(await.isDone) {
            command = nullptr;
            Serial.print(F(">> "));
        }
    }
}

void getCommand(MutRef<input::Buffer> inputRef, MutRef<CommandFuncPtr> commandRef) {
    auto& input = inputRef.ref;

    Task await = input.readTokens();
    if (!await.isDone)
        return;

    IF_DEBUG(
        Serial.print(F("(debug only) buffer: "));
        Serial.print('"');
        StrSlice(input.buffer, input.bufferLen).print();
        Serial.println('"');
        Serial.print(F("(debug only) tokenEnds: "));
        Slice<u8>(input.tokenEnds, input.tokenEndsLen).println();
        
        StrSlice tokenStr;
        for(u8 i = 0; i < input.tokenEndsLen; i++) {
            auto error = input.getToken(mutRef(tokenStr), i);
            if(error) {
                Serial.println(error);
                break;
            }

            Serial.print(F("(debug only) token: "));
            Serial.print('"');
            tokenStr.print();
            Serial.println('"');
        }
    );

    auto token = StrSlice();
    auto error = input.getToken(mutRef(token), 0);
    if (error) {
        Serial.println(error);
        Serial.print(F(">> "));
        return;
    }

    commandRef.ref = commandFunctions::find(token);
    if(commandRef.ref == nullptr) {
        Serial.print(F("!! command: `"));
        token.print();
        Serial.println(F("` not found type 'help' for more info!!"));
        Serial.print(F(">> "));
    }
}







