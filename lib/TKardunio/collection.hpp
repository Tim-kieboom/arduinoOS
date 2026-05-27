/*
Tim Kieboom 1025003
*/
#pragma once
#include "assert.h"
#include "types.hpp"

template<typename T>
class Slice {
private:
    T const* _ptr = nullptr;
    usize _len = 0;
public:
    constexpr Slice() {}
    constexpr Slice(T const* ptr, usize len): _ptr(ptr), _len(len) {}
    constexpr Slice(T const* ptr, usize len, usize offset): _ptr(ptr+offset), _len(len) {}
    T const* asPtr() const {return _ptr;}
    usize len() const {return _len;}

    T operator[](usize index) const {
        ASSERT_SMALLER(index, _len);
        return _ptr[index];
    }

    bool equal(Slice<T> const& other) const {
        if(this->len() != other.len())
            return false;
        
        for (usize i = 0; i < _len; i++) {
            if (_ptr[i] != other._ptr[i])
                return false;
        }
        return true;
    }

    void println() const {
        print();
        Serial.println();
    }

    void print() const {
        
        Serial.print('[');
        usize lastIndex = _len-1;
        for(usize i = 0; i < _len; i++) {
            Serial.print(_ptr[i]);
            if (i != lastIndex) {
                Serial.print(", ");
            }
        }
        Serial.print(']');
    }
};
