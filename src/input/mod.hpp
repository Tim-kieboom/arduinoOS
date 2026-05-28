#pragma once
#include <TKarduino.hpp>

namespace input {
    constexpr u8 BUFFER_SIZE = 35;
    constexpr u8 TOKENS_SIZE = 4;
    
    struct Buffer {
        char buffer[BUFFER_SIZE];
        u8 bufferLen = 0;

        u8 tokenEnds[TOKENS_SIZE];
        u8 tokenEndsLen = 0;

        bool shouldResetBuffer = true;

        Buffer() {}
        void reset();
        Task readTokens();
        bool tryPush(const char ch);
        void addEndIndex(const u8 index);
        const Fstr* getToken(MutRef<StrSlice> slice, const u8 index);
    };
}