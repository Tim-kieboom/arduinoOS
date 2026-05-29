/*
Tim Kieboom 1025003
*/
#pragma once
#include <Arduino.h>
#include <stddef.h>
#include <stdint.h>

/// Unsigned 8-bit integer.
typedef uint8_t u8;
// NOTE: u16 is already defined elsewhere
/// Unsigned 32-bit integer.
typedef uint32_t u32;
/// Unsigned 64-bit integer.
typedef uint64_t u64;
/// Platform-dependent unsigned size type.
typedef size_t usize;
/// Platform-dependent unsigned integer.
typedef unsigned int uint;

/// Signed 8-bit integer.
typedef int8_t i8;
/// Signed 16-bit integer.
typedef int16_t i16;
/// Signed 32-bit integer.
typedef int32_t i32;
/// Signed 64-bit integer.
typedef int64_t i64;

/// Alias for Arduino's __FlashStringHelper (progmem string).
typedef __FlashStringHelper Fstr;

/**
 * @brief Represents the completion state of an async task.
 */
struct Task{
    bool isDone; ///< true if the task has finished, false if still pending.

    /** @brief Returns a completed (done) task. */
    static Task Done() {return Task{true};};

    /** @brief Returns a pending (incomplete) task. */
    static Task Pending() {return Task{false};};
    
    Task() = delete; ///< Tasks must be created via Done() or Pending().
};

/**
 * @brief A mutable reference wrapper used to pass output parameters.
 */
template<typename T>
struct MutRef{
    T& ref; ///< The underlying reference.
    constexpr MutRef(T& ref): ref(ref) {}
};

/**
 * @brief Explicitly marks a function argument as an output parameter
 *        that may mutate.
 *
 * This is intended as a zero-cost abstraction due to the inline function
 * and constexpr constructor.
 */
template<typename T>
inline MutRef<T> out(T& ref) {
    return MutRef<T>(ref);
}

class IAsyncState {
public:
    virtual void reset() = 0;
};