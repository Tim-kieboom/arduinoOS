#include "mod.hpp"

inline bool isEndOfLine(const char ch);
inline bool isEndOfToken(const char ch);

namespace input {
    
    Task Buffer::readTokens() {
        if(shouldResetBuffer)
            reset();

        const int raw = Serial.read();
        if (raw < 0)
            return Task::Pending();

        const char ch = (char)raw;
        if(!tryPush(isEndOfToken(ch) ? '\0' : ch))
            return Task::Pending();

        if(isEndOfLine(ch)) {
            
            if(bufferLen < 2) {
                shouldResetBuffer = true;
                return Task::Pending();
            }

            addEndIndex(bufferLen);
            shouldResetBuffer = true;
            return Task::Done();
        }
    
        if(isEndOfToken(ch))
            addEndIndex(bufferLen);

        return Task::Pending();
    }

    bool Buffer::tryPush(const char ch) {
        if(bufferLen >= input::BUFFER_SIZE) {
            printM(F("\n!!error!! input to large\n"), F(">> "));
            Serial.flush();
            reset();
            return false;
        }
        
        buffer[bufferLen++] = ch;
        return true;
    }

    const Fstr* Buffer::getToken(MutRef<StrSlice> slice, const u8 index) {
        
        if(index >= this->tokenEndsLen) {
            auto error = F("!!error!! token not found");
            ASSERT_PRINT(false, error);
            return error;
        }
        
        u8 offset = (index > 0) ? this->tokenEnds[index-1] : 0;
        u8 len = this->tokenEnds[index] - offset - 1;

        slice.ref = StrSlice(this->buffer, len, offset);
        return nullptr;
    }

    void Buffer::addEndIndex(const u8 index) {
        if(tokenEndsLen >= input::TOKENS_SIZE)
            return;

        tokenEnds[tokenEndsLen++] = index;
        return;
    }

    void Buffer::reset() {
        bufferLen = 0;
        tokenEndsLen = 0;
        shouldResetBuffer = false;
    }
}

inline bool isEndOfToken(const char ch) {
    return isSpace(ch);
}

inline bool isEndOfLine(const char ch) {
    return ch == '\n' || ch == '\r' || ch == '\0';
}