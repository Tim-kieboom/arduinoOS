/*
Tim Kieboom 1025003
*/
#ifndef INPUTBUFFER_H
#define INPUTBUFFER_H
#pragma once

#include <Arduino.h>
#include "../TKardunio/TKarduino.h"

constexpr uint8_t TOKEN_END_INDEXES_SIZE = 4;
constexpr uint8_t BUFFER_SIZE = 12;

struct InputBuffer {
    SmartArray<char> buffer = SmartArray<char>();
    uint8_t tokenEndIndexes[TOKEN_END_INDEXES_SIZE];
    uint8_t tokenEndIndexes_top = 0;

    bool shouldResetBuffer = true;
    uint8_t currentIndex = 0;

    InputBuffer() {}
    InputBuffer(SmartArray<char> buffer): buffer(buffer) {}
};



#endif







