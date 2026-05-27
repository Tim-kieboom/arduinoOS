#include <TKarduino.hpp>
#include "mod.hpp"

inline bool isEndOfLine(const char ch);
inline bool isEndOfToken(const char ch);

namespace input {

    Task readTokens(MutRef<Buffer> inputRef) {
        auto& input = inputRef.ref;
        if(input.shouldResetBuffer)
            input.reset();

        const int raw = Serial.read();
        if (raw < 0)
            return Task::Pending();

        const char ch = (char)raw;
        if(!input.tryPush(ch))
            return Task::Pending();

        if(isEndOfLine(ch)) {
            
            if(input.bufferLen < 2) {
                input.shouldResetBuffer = true;
                return Task::Pending();
            }

            input.addEndIndex(input.bufferLen);
            input.shouldResetBuffer = true;
            return Task::Done();
        }
    
        if(isEndOfToken(ch))
            input.addEndIndex(input.bufferLen);

        return Task::Pending();
    }
}

inline bool isEndOfToken(const char ch) {
    return isSpace(ch);
}

inline bool isEndOfLine(const char ch) {
    return ch == '\n' || ch == '\r' || ch == '\0';
}