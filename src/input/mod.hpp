#pragma once
#include <TKarduino.hpp>

namespace input {
    /// Maximum number of characters the input buffer can hold.
    constexpr u8 BUFFER_SIZE = 35;
    /// Maximum number of tokens that can be extracted from one input line.
    constexpr u8 TOKENS_SIZE = 4;
    
    /// Line-based input buffer that reads from Serial and tokenises the input.
    struct Buffer {
        char buffer[BUFFER_SIZE];       ///< Raw character buffer for the input line.
        u8 bufferLen = 0;               ///< Current number of characters in the buffer.

        u8 tokenEnds[TOKENS_SIZE];      ///< End indices of each token in the buffer.
        u8 tokenEndsLen = 0;            ///< Number of tokens parsed so far.

        bool shouldResetBuffer = true;  ///< Whether the buffer should be reset on the next read.

        /** @brief Default constructor. */
        Buffer() {}

        /** @brief Resets the buffer and token state. */
        void reset();

        /** 
         * @brief Reads characters from Serial and builds tokens. 
         * @returns Done once a full line is read. 
         */
        Task readTokens();

        /** 
         * @brief Appends a character to the buffer.
         * @returns false if the buffer is full. 
         */
        bool tryPush(const char ch);

        /** @brief Records the end index of the current token. */
        void addEndIndex(const u8 index);
        
        /** 
         * @brief Retrieves a token by index. 
         * @returns nullptr on success, or an error message. 
         */
        const Fstr* getToken(MutRef<StrSlice> slice, const u8 index);
    };
}