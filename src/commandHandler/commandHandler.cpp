/*
Tim Kieboom 1025003
*/
#include "commandHandler.h"
#include "../lib/TKardunio/TKarduino.h"
#include "InputBuffer/InputBuffer.h"
#include "utils/utils.h"

inline bool wouldBufferOverflow(InputBuffer& input, const int index);
inline void addEndIndex(/*out*/InputBuffer& input, int index);
inline void resetInputBuffer(/*out*/InputBuffer& inputBuffer);
inline bool isEndOfLine(const char ch);
inline bool isEndOfToken(const char ch);

Task commandFunc_printAllCommands(InputBuffer& input) {
    static const __FlashStringHelper* ALL_COMANDS_STRING =
    F("all commands:"
        "\n\t-help"                                       
        "\n\t-store \t\t<file_name> <file_size> <data>"      
        "\n\t-retrieve \t<file_name>"                     
        "\n\t-erase \t\t<file_name>"                      
        "\n\t-files"                                       
        "\n\t-freespace"                                  
        "\n\t-run \t\t<file_name>"                        
        "\n\t-suspend \t<process_id>"                     
        "\n\t-resume \t<process_id>"                     
        "\n\t-kill \t\t<process_id>"
        "\n\t-clearall"
        "\n\t-restart"
    );
    
    if(!checkArguments(input, 0))
        return Done;

    Serial.println(ALL_COMANDS_STRING);
    return Done;
}

Task commandFunc_restart(InputBuffer &input) {
    if(!checkArguments(input, 0))
        return Done;
    
    RESET_ARDUINO;
    return Done; //return is for compiler DOES NOTHING
}

Task readTokens(/*out*/InputBuffer& input) {
    if(input.shouldResetBuffer) 
        resetInputBuffer(/*out*/input);

    const char ch = Serial.read();
    if(wouldBufferOverflow(input, input.currentIndex))
        return NotDone;

    input.buffer[input.currentIndex++] = ch;

    if (isEndOfLine(ch)) {
        if(input.currentIndex == 2) {
            input.shouldResetBuffer = true;
            Serial.flush();
            Serial.print(">> ");
            return NotDone;
        }

        if(wouldBufferOverflow(input, input.currentIndex+1))
            return NotDone;

        input.buffer[input.currentIndex+1] = '\0';

        input.shouldResetBuffer = true;
        return Done;
    }

    if (isEndOfToken(ch)) {
        addEndIndex(/*out*/input, input.currentIndex - 2);
    }
    return NotDone;
}

inline bool wouldBufferOverflow(InputBuffer& input, const int index)  {
    if(input.currentIndex >= input.buffer.len()) {
        printM('\n', F("!!error!! input to large"), '\n', F(">> "));
        Serial.flush();
        resetInputBuffer(input);
        return true;
    }

    return false;
}

ConstSpan<char> getToken(const InputBuffer& input, const uint8_t tokenIndex) {
    ASSERT_SMALLER(tokenIndex, TOKEN_END_INDEXES_SIZE);
    const uint8_t len = input.tokenEndIndexes[tokenIndex] + 1;
    ASSERT_PRINT(len > 1, "token is empty");
    if(len <= 1)
        return ConstSpan<char>();

    uint8_t offset = (tokenIndex > 0) ? input.tokenEndIndexes[tokenIndex - 1] + 2 : 0;
    
    return input.buffer.constSpan(offset, len);
}

CommandFunc getCommandFunction(const ConstSpan<CommandType>& commands, ConstSpan<char>& name) {
    ASSERT(!commands.isEmpty());
    if(name.isEmpty())
        return nullptr;

    for(size_t i = 0; i < commands.len(); i++) {
        const CommandType& command = commands[i];
        if(name.equals(command.name, strlen(command.name)))
            return command.function;
    }

    return nullptr;
}

inline void addEndIndex(/*out*/InputBuffer& input, int index) {
    if(input.tokenEndIndexes_len >= TOKEN_END_INDEXES_SIZE)
        return;

    input.tokenEndIndexes[input.tokenEndIndexes_len++] = index;
    return;
}

inline bool isEndOfToken(const char ch) {
    return isSpace(ch);
}

inline bool isEndOfLine(const char ch) {
    return ch == '\n' || ch == '\0';
}

inline void resetInputBuffer(/*out*/InputBuffer& input) {
    for(int i = 0; i < TOKEN_END_INDEXES_SIZE; i++) {
        input.tokenEndIndexes[i] = 0;
    }

    input.shouldResetBuffer = false;
    input.tokenEndIndexes_len = 0;
    input.currentIndex = 0;
}






