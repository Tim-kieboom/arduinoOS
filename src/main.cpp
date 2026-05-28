/*
Tim Kieboom 1025003
*/
#include <TKarduino.hpp> 
#include "commandFunctions/mod.hpp"

constexpr CommandFuncPtr NO_COMMAND = nullptr; 

inline bool hasSerialInput();
inline bool hasCommand(CommandFuncPtr command);
void runCommand(MutRef<input::Buffer> inputRef, MutRef<CommandFuncPtr> commandRef);
void readCommand(MutRef<input::Buffer> inputRef, MutRef<CommandFuncPtr> commandRef);

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
        readCommand(out(input), out(command));

    if(hasCommand(command))
        runCommand(out(input), out(command));
}

/**
 * @brief Reads a line of input from Serial and resolves it to a command.
 * @param inputRef  Mutable reference to the input buffer.
 * @param commandRef Mutable reference where the resolved command pointer is stored.
 * 
 * Reads tokens from Serial, looks up the first token as a command name,
 * and stores the matching CommandFuncPtr in commandRef.
 */
void readCommand(MutRef<input::Buffer> inputRef, MutRef<CommandFuncPtr> commandRef) {
    auto& input = inputRef.ref;

    Task await = input.readTokens();
    if (!await.isDone)
        return;

    auto token = StrSlice();
    auto error = input.getToken(out(token), 0);
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

/**
 * @brief Runs the active command function.
 * @param inputRef  Mutable reference to the input buffer.
 * @param commandRef Mutable reference to the current command pointer.
 * 
 * Clears the command and prints the prompt prefix when the command task is done.
 */
void runCommand(MutRef<input::Buffer> inputRef, MutRef<CommandFuncPtr> commandRef) {
    auto& input = inputRef.ref;
    auto& command = commandRef.ref;
    
    Task await = command(out(input));
    if(await.isDone) {
        command = NO_COMMAND;
        Serial.print(F(">> "));
    }
}

inline bool hasCommand(CommandFuncPtr command) {
    return command != NO_COMMAND;
}

inline bool hasSerialInput() {
    return Serial.available() > 0;
}





