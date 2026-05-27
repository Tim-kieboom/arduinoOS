#pragma once
#include <TKarduino.hpp>

constexpr u8 BUFFER_SIZE = 12;

namespace input {
    constexpr u8 BUFFER_SIZE = 35;
    constexpr u8 TOKENS_SIZE = 4;
    
    struct Buffer {
        char buffer[input::BUFFER_SIZE];
        u8 bufferLen = 0;

        u8 tokens[TOKENS_SIZE];
        u8 tokensLen = 0;

        bool shouldResetBuffer = true;

        Buffer() {}
        void reset();
        bool tryPush(const char ch);
        void addEndIndex(const u8 index);
        bool getToken(MutRef<Slice<char>> slice, const u8 index);
    };

    Task readTokens(MutRef<Buffer> input);
}