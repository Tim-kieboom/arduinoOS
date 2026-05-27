#include "mod.hpp"

namespace input {
    
    bool Buffer::tryPush(const char ch) {
        if(bufferLen >= input::BUFFER_SIZE) {
            printM('\n', F("!!error!! input to large"), '\n', F(">> "));
            Serial.flush();
            reset();
            return false;
        }
        
        buffer[bufferLen++] = ch;
        return true;
    }

    bool Buffer::getToken(MutRef<Slice<char>> slice, const u8 index) {
        
        if(index >= this->tokensLen)
            return false;
        
        u8 tokenStart = (index == 0) ? 0 : this->tokens[index-1];
        u8 tokenEnd = this->tokens[index];
        
        slice.ref = Slice<char>(this->buffer, tokenEnd-1, tokenStart);
        return true;
    }

    void Buffer::addEndIndex(const u8 index) {
        if(tokensLen >= input::TOKENS_SIZE)
            return;

        tokens[tokensLen++] = index;
        return;
    }

    void Buffer::reset() {
        bufferLen = 0;
        tokensLen = 0;
        shouldResetBuffer = false;
    }
}