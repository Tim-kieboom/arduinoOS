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

static constexpr const char* ALL_COMANDS_STRING =
"all commands:"                                    
  "\n\t-help"                                       
  "\n\t-store \t\t<file_name> <file_size> <data>"   
  "\n\t-write \t\t<file_name>"   
  "\n\t-dir"   
  "\n\t-retrieve \t<file_name>"                     
  "\n\t-erase \t\t<file_name>"                      
  "\n\t-file"                                       
  "\n\t-freespace"                                  
  "\n\t-run \t\t<file_name>"                        
  "\n\t-suspend \t<process_id>"                     
  "\n\t-resume \t<process_id>"                     
  "\n\t-kill \t\t<process_id>";

void commandFunc_printAllCommands(InputBuffer& input) {
    if(input.tokenEndIndexes_top == 1)
    {
        Serial.print("command only should have 0 arguments you have: '");
        Serial.print(input.tokenEndIndexes_top);
        Serial.println("' arguments");
        return;
    }

    Serial.println(ALL_COMANDS_STRING);
}

bool readTokens(/*out*/InputBuffer& input) {
    if(input.shouldResetBuffer) {
        if(!resetInputBuffer(/*out*/input))
            return false;
    }

    char ch = Serial.read();

    if(checkBufferOverflow(input, input.currentIndex))
        return false;

    input.buffer[input.currentIndex++] = ch;

    if (isEndOfLine(ch)) {       
        if(checkBufferOverflow(input, input.currentIndex+1))
            return false;

        input.buffer[input.currentIndex+1] = '\0';
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

bool doCommand(const ConstSpan<CommandType>& commands, ConstSpan<char>& commandName, InputBuffer& input) {
    ASSERT(!commands.isEmpty());
    if(commandName.isEmpty())
        return false;

    for(size_t i = 0; i < commands.len(); i++ )
    {
        const CommandType& command = commands[i];
        if(commandName.equal(command.name))
        {
            command.function(input);
            return true;
        }
    }

    return false;
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






