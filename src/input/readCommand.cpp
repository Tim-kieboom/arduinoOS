#include "readCommand.hpp"

namespace input {

    void readCommand(MutRef<input::Buffer> inputRef, MutRef<CommandFunctionsPtr> commandRef) {
        static usize i = 0;
        static bool tokensRead = false;
        static StrSlice token = StrSlice();
        
        auto& input = inputRef.ref;
        auto& command = commandRef.ref;

        if(!tokensRead) {
            Task await = input.readTokens();
            if (!await.isDone)
                return;

            auto error = input.getToken(out(token), 0);
            if (error) {
                Serial.println(error);
                Serial.print(F(">> "));
                return;
            }

            tokensRead = true;
        }

        Task await = commandFunctions::find(token, out(i), out(command));
        if (!await.isDone)
            return;

        i = 0;
        tokensRead = false;
        if(command == nullptr) {
            Serial.print(F("!! command: `"));
            token.print();
            Serial.println(F("` not found type 'help' for more info!!"));
            Serial.print(F(">> "));
        }
    }
}