#pragma once
#include "types.hpp"
#include "assert.h"
#include <new>

class IAsyncState {
public:
    virtual void reset() = 0;
};