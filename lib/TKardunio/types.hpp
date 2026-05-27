/*
Tim Kieboom 1025003
*/
#pragma once
#include <Arduino.h>
#include <stddef.h>
#include <stdint.h>

typedef uint8_t u8;
// u16 is already defined
typedef uint32_t u32;
typedef uint64_t u64;
typedef size_t usize;
typedef unsigned int uint;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef __FlashStringHelper Fstr;

struct Task{
    bool isDone;
    static Task Done() {return Task{true};};
    static Task Pending() {return Task{false};};
    Task() = delete;
};

template<typename T>
struct MutRef{
    T& ref;
    constexpr MutRef(T& ref): ref(ref) {}
};

template<typename T>
inline MutRef<T> mutRef(T& ref) {
    return MutRef<T>(ref);
}