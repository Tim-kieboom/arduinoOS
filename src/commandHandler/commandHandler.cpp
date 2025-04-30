/*
Tim Kieboom 1025003
*/
#include "commandHandler.h"
#include "../lib/TKardunio/TKarduino.h"
#include "InputBuffer/InputBuffer.h"
#include "utils/utils.h"

inline bool checkBufferOverflow(InputBuffer& input, const int index);
inline bool addEndIndex(/*out*/InputBuffer& input, int index);
inline bool resetInputBuffer(/*out*/InputBuffer& inputBuffer);
inline bool isEndOfLine(const char ch);
inline bool isEndOfToken(const char ch);
inline bool isUpArrow(InputBuffer& input);

bool commandFunc_printAllCommands(InputBuffer& input) {
    const __FlashStringHelper* ALL_COMANDS_STRING =
    F("all commands:\n\t-help"                                       
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
    "\n\t-restart");
    
    if(!checkArguments(input, 0))
        return true;

    Serial.println(ALL_COMANDS_STRING);
    return true;
}

bool commandFunc_restart(InputBuffer &input) {
    if(!checkArguments(input, 0))
        return true;
    
    RESET_ARDUINO;
    return true; //return is for compiler DOES NOTHING
}

bool readTokens(/*out*/InputBuffer& input) {
    static SmartArray<char> prev = SmartArray<char>(INPUT_BUFFER_SIZE);
    static uint8_t prevEndTokensIndexes[TOKEN_END_INDEXES_SIZE];
    static uint8_t prevEndTokensIndexes_len = 0;

    if(input.shouldResetBuffer) {
        if(!resetInputBuffer(/*out*/input))
            return false;
    }

    char ch = Serial.read();

    if(checkBufferOverflow(input, input.currentIndex))
        return false;

    input.buffer[input.currentIndex++] = ch;

    if (isEndOfLine(ch)) {
        if(input.currentIndex == 2) {
            input.shouldResetBuffer = true;
            Serial.flush();
            Serial.print(">> ");
            return false;
        }

        if(isUpArrow(input)) {
            input.buffer.setRange(prev);
            memcpy(input.tokenEndIndexes, prevEndTokensIndexes, TOKEN_END_INDEXES_SIZE);
            input.tokenEndIndexes_len = prevEndTokensIndexes_len;
        }
        else {
            if(checkBufferOverflow(input, input.currentIndex+1))
                return false;

            input.buffer[input.currentIndex+1] = '\0';

            prev.setRange(input.buffer);
            memcpy(prevEndTokensIndexes, input.tokenEndIndexes, TOKEN_END_INDEXES_SIZE);
            prevEndTokensIndexes_len = input.tokenEndIndexes_len;
        }

        input.shouldResetBuffer = true;
        return true;
    }

    if (isEndOfToken(ch)) {
        bool isSuccess = addEndIndex(/*out*/input, input.currentIndex - 2);
        ASSERT_PRINT(isSuccess, "to many tokens");
    }
    return false;
}

inline bool checkBufferOverflow(InputBuffer& input, const int index)  {
    if(input.currentIndex >= input.buffer.len()) {
        Serial.println();
        Serial.println(F("!!error!! input to large"));
        Serial.print(">> ");
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

inline bool isUpArrow(InputBuffer& input) {
    constexpr char UpArrow[] = {27, 91, 'A'}; 
    return input.buffer.len() >= 3 && input.buffer.constSpan(0,2).equals(UpArrow, 3);
}

inline bool addEndIndex(/*out*/InputBuffer& input, int index)
{
    if(input.tokenEndIndexes_len >= sizeof(input.tokenEndIndexes))
        return false;

    input.tokenEndIndexes[input.tokenEndIndexes_len++] = index;
    return true;
}

inline bool isEndOfToken(const char ch) {
    return isSpace(ch);
}

inline bool isEndOfLine(const char ch) {
    return ch == '\n' || ch == '\0';
}

inline bool resetInputBuffer(/*out*/InputBuffer& input) {
    for(int i = 0; i < TOKEN_END_INDEXES_SIZE; i++) {
        input.tokenEndIndexes[i] = 0;
    }

    input.shouldResetBuffer = false;
    input.tokenEndIndexes_len = 0;
    input.currentIndex = 0;
    return true;
}






