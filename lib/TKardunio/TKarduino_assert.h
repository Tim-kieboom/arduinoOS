/*
Tim Kieboom 1025003
*/
#ifndef TKARDUINO_ASSERT_H
#define TKARDUINO_ASSERT_H
#pragma once

#include <Arduino.h>

// To enable assertions (debug mode), leave the line below uncommented.
// To disable assertions (release mode), comment out the line below.
#define ENABLE_ASSERT

#ifdef ENABLE_ASSERT
/**
 * @brief Asserts that a condition is true in debug builds.
 *
 * If the condition evaluates to false, the programs panics(aka halts execution) and prints the file name
 * and line number.
 *
 * @param condition Expression to evaluate. If false, triggers the assertion failure.
 */
#define ASSERT(condition) if (!(condition)) {                                                   \
    Serial.println();                                                                           \
    Serial.println("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");                                           \
    Serial.print("Assertion '"); Serial.print(#condition); Serial.println("' failed! at:");     \
    Serial.print("file: "); Serial.println(__FILE__);                                           \
    Serial.print("line: "); Serial.println(__LINE__);                                           \
    Serial.println("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");                                           \
    delay(500); /*give micro controller some time to print out assert*/                         \
    exit(1); /*panic!!*/                                                                        \
    while(1); /*while(1) is just for the compiler plz ignore*/                                  \
}
#else
/**
 * @brief In release mode, ASSERT does nothing.
 */ 
#define ASSERT(condition) ((void)0)
#endif

#ifdef ENABLE_ASSERT
/**
 * @brief Assert that a condition is true in debug builds, with a custom message.
 *
 * If the condition evaluates to false, the programs panics(aka halts execution) and prints a message and the file name
 * and line number.
 *
 * @param condition Expression to evaluate. If false, triggers assertion failure.
 * @param message Custom message to print if assertion fails.
 */
#define ASSERT_PRINT(condition, message) if (!(condition)) {                \
    Serial.println();                                                       \
    Serial.println("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");                       \
    Serial.println("Assertion '" +String(#condition)+ "' failed! at:");     \
    Serial.println(__FILE__);                                               \
    Serial.print("line: "); Serial.println(__LINE__);                       \
    Serial.print("msg: "); Serial.println(message);                         \
    Serial.println("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");                       \
    delay(500); /*give micro controller some time to print out assert*/     \
    exit(1); /*panic!!*/                                                    \
    while(1); /*while(1) is just for the compiler plz ignore*/              \
}
#else
/**
 * @brief In release mode, ASSERT_PRINT does nothing.
 */
#define ASSERT_PRINT(condition, message) ((void)0)
#endif

/**
 * @brief Mark code paths that are not yet implemented.
 *
 * In debug mode, always fails with "not yet implemented" message.
 * In release mode, does nothing.
 */
#define ASSERT_TODO ASSERT_PRINT(false, "not yet implemented")

#define _ASSERT_CMP(a, cmp, invcmp, b) ASSERT_PRINT(a cmp b, "(values: '" + String(a) + "' " + String(#invcmp) + " '" + String(b) + "')")

/**
 * @brief Asserts that two values are equal in debug builds.
 *
 * If the values are not equal, prints the file name, line number, and a message
 * showing both values, then halts execution.
 *
 * @param a First value to compare.
 * @param b Second value to compare.
 */
#define ASSERT_EQ(a, b) _ASSERT_CMP(a, ==, !=, b)

/**
 * @brief Asserts that two values are not equal in debug builds.
 *
 * If the values are equal, prints the file name, line number, and a message
 * showing both values, then halts execution.
 *
 * @param a First value to compare.
 * @param b Second value to compare.
 */
#define ASSERT_NQ(a, b) _ASSERT_CMP(a, !=, ==, b)

/**
 * @brief Asserts that two values are smaller in debug builds.
 *
 * If the values are equal, prints the file name, line number, and a message
 * showing both values, then halts execution.
 *
 * @param a First value to compare.
 * @param b Second value to compare.
 */
#define ASSERT_SMALLER(a, b) _ASSERT_CMP(a, <, >=, b)

/**
 * @brief Asserts that two values are bigger in debug builds.
 *
 * If the values are equal, prints the file name, line number, and a message
 * showing both values, then halts execution.
 *
 * @param a First value to compare.
 * @param b Second value to compare.
 */
#define ASSERT_BIGGER(a, b) _ASSERT_CMP(a, >, <=, b)

#endif


