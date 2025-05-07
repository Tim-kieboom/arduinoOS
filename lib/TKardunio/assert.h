/*
Tim Kieboom 1025003
*/
#ifndef TKARDUINO_ASSERT_H
#define TKARDUINO_ASSERT_H
#pragma once

#include <Arduino.h>
#include "debug.h"

inline int getAmountOfFreeRam() {
    extern int __heap_start, *__brkval;
    int v;
    return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

#ifdef ARDUINO_AVR_UNO
#define RESET_ARDUINO asm volatile(" jmp 0 ")
#define PANIC Serial.println(F("!!panic!! restarting arduino")); delay(500); RESET_ARDUINO
#else 
#define RESET_ARDUINO PANIC; Serial.print(F("define 'RESET_ARDUINO' not found for this platfrom"))
#define PANIC Serial.print(F("!!panic!! exiting program")); delay(500); exit(1)
#endif

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
#define ASSERT(condition) IF_DEBUG(                                                                         \
    if (!(condition)) {                                                                                     \
        Serial.println();                                                                                   \
        Serial.println(F("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));                                                \
        Serial.print(F("Assertion '")); Serial.print(F(#condition)); Serial.println(F("' failed! at:"));    \
        Serial.print(F("file: ")); Serial.println(F(__FILE__));                                             \
        Serial.print(F("func: ")); Serial.println(__func__);                                                \
        Serial.print(F("line: ")); Serial.println(__LINE__);                                                \
        Serial.println(F("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));                                                \
        delay(500); /*give micro controller some time to print out assert*/                                 \
        PANIC;                                                                                              \
        while(1); /*while(1) is just for the compiler plz ignore*/                                          \
    }                                                                                                       \
)

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
#define ASSERT_PRINT(condition, message) IF_DEBUG(                                                          \
    if (!(condition)) {                                                                                     \
        Serial.println();                                                                                   \
        Serial.println(F("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));                                                \
        Serial.print(F("Assertion '")); Serial.print(F(#condition)); Serial.println(F("' failed! at:"));    \
        Serial.print(F("file: ")); Serial.println(F(__FILE__));                                             \
        Serial.print(F("func: ")); Serial.println(__func__);                                                \
        Serial.print(F("line: ")); Serial.println(__LINE__);                                                \
        Serial.print(F("msg: ")); Serial.println(message);                                                  \
        Serial.println(F("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));                                                \
        delay(500); /*give micro controller some time to print out assert*/                                 \
        PANIC;                                                                                              \
        while(1); /*while(1) is just for the compiler plz ignore*/                                          \
    }                                                                                                       \
)                                                                                                      

/**
 * @brief Mark code paths that are not yet implemented.
 *
 * In debug mode, always fails with "not yet implemented" message.
 * In release mode, does nothing.
 */
#define ASSERT_TODO ASSERT_PRINT(false, F("not yet implemented"))

/**
 * @brief asserts that alloc was successfull.
 *
 * In debug mode, will check if ptr is nullptr if i is fail.
 * In release mode, does nothing.
 */
#define ASSERT_ALLOC(ptr) ASSERT_PRINT(ptr != NULL, F("alloc failed"))

#define _ASSERT_CMP(a, cmp, b) ASSERT_PRINT(a cmp b, "(left: '" + String(a) + "', right: '" + String(b) + "')")

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
#define ASSERT_RAM ASSERT_PRINT(getAmountOfFreeRam() != 0, "no more RAM left"); DEBUG_PRINTM(F("\nRAM left: "), getAmountOfFreeRam(), F(" KB\n"))

#endif


