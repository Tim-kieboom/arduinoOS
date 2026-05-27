/*
Tim Kieboom 1025003
*/
#pragma once

// To enable (debug mode), leave the line below uncommented.
// To disable (release mode), comment out the line below.
#define ENABLE_TK_DEBUGGING

#ifdef ENABLE_TK_DEBUGGING
    ///runs code in debug mode, but in release mode does nothing
    #define IF_DEBUG(body) body
    ///runs code in release mode, but in debug mode does nothing
    #define IF_RELEASE(body) 
    ///runs debugBody in debug mode, and releaseBody in release mode
    #define IF_DEBUG_ELSE(debugBody, releaseBody) debugBody
#else
    ///runs code in debug mode, but in release mode does nothing
    #define IF_DEBUG(body) 
    ///runs code in release mode, but in debug mode does nothing
    #define IF_RELEASE(body) body
    ///runs debugBody in debug mode, and releaseBody in release mode
    #define IF_DEBUG_ELSE(debugBody, releaseBody) releaseBody
#endif

///'Serial.print' each arguments
template <typename... Args>
inline void printM(Args&&... args) {
    using expander = int[];
    (void)expander{0, (Serial.print(args), 0)...};
}

#define DEBUG_PRINT(msg) IF_DEBUG(Serial.print(msg))
#define DEBUG_PRINTLN(msg) IF_DEBUG(Serial.println(msg))
#define DEBUG_PRINTM(...) IF_DEBUG(printM(__VA_ARGS__))