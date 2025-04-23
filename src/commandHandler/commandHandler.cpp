/*
Tim Kieboom 1025003
*/
#include "commandHandler.h"
#include "../lib/TKardunio/TKarduino.h"
#include "InputBuffer/InputBuffer.h"

inline bool addEndIndex(/*out*/uint8_t* tokenEndIndexes, int value);
inline void resetInputBuffer(/*out*/InputBuffer& inputBuffer);
inline bool isEndOfLine(const char ch);
inline bool isEndOfToken(const char ch);

static constexpr const char* ALL_COMANDS_STRING =
"all commands:"                                    
  "\n\t-help"                                       
  "\n\t-store \t\t<file_name> <file_size> <data>"   
  "\n\t-retrieve \t<file_name>"                     
  "\n\t-erase \t\t<file_name>"                      
  "\n\t-file"                                       
  "\n\t-freespace"                                  
  "\n\t-run \t\t<file_name>"                        
  "\n\t-suspend \t<process_id>"                     
  "\n\t-resume \t<process_id>"                     
  "\n\t-kill \t\t<process_id>";

void commandFunc_printAllCommands(InputBuffer& inputBuff) {
    Serial.println(ALL_COMANDS_STRING);
}

bool readTokens(/*out*/InputBuffer& input) {
    
    if(input.shouldResetBuffer) {
        resetInputBuffer(/*out*/input);
    }
    
    char ch = Serial.read();

    input.buffer[input.currentIndex++] = ch;

    if (isEndOfLine(ch)) {
        input.buffer[input.currentIndex+1] = '\0';
        input.shouldResetBuffer = true;
        return true;
    }

    if (isEndOfToken(ch))
        addEndIndex(/*out*/input.tokenEndIndexes, input.currentIndex - 2);

    return false;
}

ConstSpan<char> getToken(const InputBuffer& input, const uint8_t tokenIndex) {
    ASSERT_SMALLER(tokenIndex, TOKEN_END_INDEXES_SIZE);
    const uint8_t len = input.tokenEndIndexes[tokenIndex] + 1;
    ASSERT_NQ(tokenIndex, 1);

    uint8_t offset = (tokenIndex > 0) ? input.tokenEndIndexes[tokenIndex - 1] + 2 : 0;

    return input.buffer.constSpan(offset, len);
}

bool doCommand(ConstSpan<CommandType>& commands, ConstSpan<char>& commandName, InputBuffer& input) {
    ASSERT(!commands.isEmpty());
    ASSERT(!commandName.isEmpty());

    for(size_t i = 0; i < commands.len(); i++ )
    {
        const CommandType& command = commands[i];
        if(STR_EQUAL(commandName.copy_asCstr(), command.name))
        {
            command.function(input);
            return true;
        }
    }

    return false;
}

inline bool addEndIndex(/*out*/uint8_t* tokenEndIndexes, int index)
{
    for(int i = 0; i < TOKEN_END_INDEXES_SIZE; i++) {
        if (tokenEndIndexes[i] != 0)
            continue;

        tokenEndIndexes[i] = index;
        return true;
    }

    return false;
}

inline bool isEndOfToken(const char ch) {
    return isSpace(ch) || ch == ' ';
}

inline bool isEndOfLine(const char ch) {
    return ch == '\n' || ch == '\0';
}

inline void resetInputBuffer(/*out*/InputBuffer& inputBuffer) {
    
    // SmartArray<char>& buffer = inputBuffer.buffer;
    // if (buffer.isEmpty()) {
    //     buffer.free();
    // }

    // buffer.resize(Serial.available());

    for(int i = 0; i < TOKEN_END_INDEXES_SIZE; i++) {
        inputBuffer.tokenEndIndexes[i] = 0;
    }

    inputBuffer.shouldResetBuffer = false;
    inputBuffer.currentIndex = 0;
}
