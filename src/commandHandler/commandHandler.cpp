/*
Tim Kieboom 1025003
*/
#include "commandHandler.h"
#include "../lib/TKardunio/TKarduino.h"
#include "InputBuffer/InputBuffer.h"

inline bool checkBufferOverflow(InputBuffer& input, const int index);
inline bool addEndIndex(/*out*/InputBuffer& input, int index);
inline bool resetInputBuffer(/*out*/InputBuffer& inputBuffer);
inline bool isEndOfLine(const char ch);
inline bool isEndOfToken(const char ch);
inline bool isUpArrow(InputBuffer& input);

static constexpr const char* ALL_COMANDS_STRING =
"all commands:"                                    
  "\n\t-help"                                       
  "\n\t-store \t\t<file_name> <file_size> <data>"   
  "\n\t-write \t\t<file_name>"   
  "\n\t-retrieve \t<file_name>"                     
  "\n\t-erase \t\t<file_name>"                      
  "\n\t-files"                                       
  "\n\t-freespace"                                  
  "\n\t-run \t\t<file_name>"                        
  "\n\t-suspend \t<process_id>"                     
  "\n\t-resume \t<process_id>"                     
  "\n\t-kill \t\t<process_id>"
  "\n\t-clearall";

bool commandFunc_printAllCommands(InputBuffer& input) {
    if(input.tokenEndIndexes_top != 1)
    {
        Serial.print("command only should have 0 arguments you have: '");
        Serial.print(min((int)input.tokenEndIndexes_top-1, 1));
        Serial.println("' arguments");
        return true;
    }

    Serial.println(ALL_COMANDS_STRING);
    return true;
}

bool readTokens(/*out*/InputBuffer& input) {
    static SmartArray<char> prev = SmartArray<char>(INPUT_BUFFER_SIZE);
    static uint8_t prevEndTokensIndexes[TOKEN_END_INDEXES_SIZE];
    static uint8_t prevEndTokensIndexes_top = 0;

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
            input.tokenEndIndexes_top = prevEndTokensIndexes_top;
        }
        else {
            if(checkBufferOverflow(input, input.currentIndex+1))
                return false;

            input.buffer[input.currentIndex+1] = '\0';

            prev.setRange(input.buffer);
            memcpy(prevEndTokensIndexes, input.tokenEndIndexes, TOKEN_END_INDEXES_SIZE);
            prevEndTokensIndexes_top = input.tokenEndIndexes_top;
        }

        input.shouldResetBuffer = true;
        return true;
    }

    if (isEndOfToken(ch))
        addEndIndex(/*out*/input, input.currentIndex - 2);

    return false;
}

inline bool checkBufferOverflow(InputBuffer& input, const int index)  {
    if(input.currentIndex >= input.buffer.len()) {
        Serial.println();
        Serial.println("!!error!! input is bigger then buffer try inputting again");
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

CommandFunc getCommandFunction(const ConstSpan<CommandType>& commands, ConstSpan<char>& commandName) {
    ASSERT(!commands.isEmpty());
    if(commandName.isEmpty())
        return nullptr;

    for(size_t i = 0; i < commands.len(); i++ )
    {
        const CommandType& command = commands[i];
        if(commandName.equal(command.name))
            return command.function;
    }

    return nullptr;
}

inline bool isUpArrow(InputBuffer& input) {
    constexpr char UpArrow[] = {27, 91, 'A'}; 
    return input.buffer.len() >= 3 && input.buffer.constSpan(0,2).equal(UpArrow);
}

inline bool addEndIndex(/*out*/InputBuffer& input, int index)
{
    if(input.tokenEndIndexes_top >= sizeof(input.tokenEndIndexes))
        return false;

    input.tokenEndIndexes[input.tokenEndIndexes_top++] = index;

    return true;
}

inline bool isEndOfToken(const char ch) {
    return isSpace(ch) || ch == ' ';
}

inline bool isEndOfLine(const char ch) {
    return ch == '\n' || ch == '\0';
}

inline bool resetInputBuffer(/*out*/InputBuffer& input) {
    for(int i = 0; i < TOKEN_END_INDEXES_SIZE; i++) {
        input.tokenEndIndexes[i] = 0;
    }

    input.shouldResetBuffer = false;
    input.tokenEndIndexes_top = 0;
    input.currentIndex = 0;
    return true;
}






