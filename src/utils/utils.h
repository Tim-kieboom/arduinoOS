/*
Tim Kieboom 1025003
*/
#ifndef UTILS_H
#define UTILS_H
#pragma once

#include "InputBuffer/InputBuffer.h"

bool checkArguments(InputBuffer& input, int shouldBe);

constexpr int roundUp(float value) {
    return (int)(value) + (value > (int)(value) ? 1 : 0);
}



#endif


