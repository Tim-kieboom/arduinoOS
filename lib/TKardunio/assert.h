/*
Tim Kieboom 1025003
*/
#pragma once

#include <Arduino.h>
#include "debug.h"

/** Returns the amount of free RAM in bytes by measuring stack distance from the heap. */
inline int getAmountOfFreeRam() {
    extern int __heap_start, *__brkval;
    int ref;
    return (int)&ref - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

/** Marks a code path as unreachable (compiler hint). */
#define UNREACHABLE __builtin_unreachable()

#ifdef ARDUINO_AVR_UNO
/// Total RAM available on the target platform (2048 bytes for ATmega328P).
#define RAM_AMOUNT 2048
/** Resets the Arduino by jumping to the reset vector. */
#define RESET_ARDUINO asm volatile(" jmp 0 "); UNREACHABLE
/** Prints a panic message and resets the Arduino. */
#define PANIC Serial.println(F("!!panic!! restarting arduino")); delay(500); RESET_ARDUINO
#else 
/** Platform fallback: reset behaviour must be defined per platform. */
#define RESET_ARDUINO PANIC; Serial.print(F("define 'RESET_ARDUINO' not found for this platfrom"))
/** Prints a panic message and exits the program. */
#define PANIC Serial.print(F("!!panic!! exiting program")); delay(500); exit(1)
/** Placeholder: RAM_AMOUNT must be defined for the target platform. */
#define RAM_AMOUNT Serial.println("RAM_AMOUNT not supported in platform"); PANIC;
#endif

#define _ASSERT_MSG(condition, extra)                                       \
        printM(                                                             \
            '\n',                                                           \
            F("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"), '\n',                      \
            F("Assertion '"), F(#condition), F("' failed! at:\n"),          \
            F("file: "), F(__FILE__), '\n',                                 \
            F("func: "), __func__, '\n',                                    \
            F("line: "), __LINE__, '\n'                                     \
        );                                                                  \
        extra;                                                              \
        Serial.println(F("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));                \
        delay(500); /*give micro controller some time to print out assert*/ \
        PANIC;                                                              \
        while(1); /*while(1) is just for the compiler plz ignore*/           

#define _ASSERT_CUSTOM(condition, extra) IF_DEBUG(  \
    if (!(condition)) {                             \
        _ASSERT_MSG(condition, extra)               \
    }                                               \
)                                                   \

#define _ASSERT_CMP(a, cmp, b) _ASSERT_CUSTOM(a cmp b, printM(F("(left: '"), a, F("', right: '"), b, F("')\n")))

/**
 * @brief Asserts that a condition is true in debug builds.
 *
 * In debug mode, If the condition evaluates to false, the programs panics(aka halts execution) and prints the file name
 * and line number.
 *
 * In release mode, does nothing.
 * 
 * @param condition Expression to evaluate. If false, triggers the assertion failure.
 */
#define ASSERT(condition) _ASSERT_CUSTOM(condition, ;)

/**
 * @brief Assert that a condition is true in debug builds, with a custom message.
 *
 * In debug mode, If the condition evaluates to false, the programs panics(aka halts execution) and prints a message and the file name
 * and line number.
 *
 * In release mode, does nothing.
 * 
 * @param condition Expression to evaluate. If false, triggers assertion failure.
 * @param message Custom message to print if assertion fails.
 */
#define ASSERT_PRINT(condition, message) _ASSERT_CUSTOM(condition, printM(F("msg: "), message, '\n');)                                                                                               

/**
 * @brief Mark code paths that are not yet implemented.
 *
 * always fails with "not yet implemented" message.
 */
#define TODO _ASSERT_MSG(false, Serial.println(F("not yet implemented")))

/**
 * @brief asserts that alloc was successfull.
 *
 * In debug mode, will check if ptr is nullptr if i is fail.
 * In release mode, does nothing.
 * 
 * @param ptr the pointer you just newed.
 */
#define ASSERT_ALLOC(ptr) ASSERT_PRINT(ptr != NULL, F("alloc failed"))

/**
 * @brief Asserts that two values are equal in debug builds.
 *
 * In debug mode, If the values are not equal, prints the file name, line number, and a message
 * showing both values, then halts execution.
 * 
 * In release mode, does nothing.
 * 
 * @param a First value to compare.
 * @param b Second value to compare.
 */
#define ASSERT_EQ(a, b) _ASSERT_CMP(a, ==, b)

/**
 * @brief Asserts that two values are not equal in debug builds.
 *
 * In debug mode, If the values are equal, prints the file name, line number, and a message
 * showing both values, then halts execution.
 * 
 * In release mode, does nothing.
 * 
 * @param a First value to compare.
 * @param b Second value to compare.
 */
#define ASSERT_NQ(a, b) _ASSERT_CMP(a, != b)

/**
 * @brief Asserts that two values are smaller in debug builds.
 *
 * In debug mode, If the values are equal, prints the file name, line number, and a message
 * showing both values, then halts execution.
 * 
 * In release mode, does nothing.
 * 
 * @param a First value to compare.
 * @param b Second value to compare.
 */
#define ASSERT_SMALLER(a, b) _ASSERT_CMP(a, <, b)

/**
 * @brief Asserts that two values are bigger in debug builds.
 *
 * debug mode, If the values are equal, prints the file name, line number, and a message
 * showing both values, then halts execution.
 * 
 * In release mode, does nothing.
 * 
 * @param a First value to compare.
 * @param b Second value to compare.
 */
#define ASSERT_BIGGER(a, b) _ASSERT_CMP(a, >, b)

/**
 * @brief Asserts that RAM left is not 0 prints ram left.
 */
#define ASSERT_PRINT_RAM ASSERT_PRINT(getAmountOfFreeRam() != 0, F("no more RAM left")); DEBUG_PRINTM(F("\nRAM left: "), getAmountOfFreeRam(), F(" BYTES\n"))

/**
 * @brief Panics if called in debug builds.
 */
#define DEBUG_UNREACHABLE IF_DEBUG(_ASSERT_MSG(false, Serial.println("reached unreachable")))
