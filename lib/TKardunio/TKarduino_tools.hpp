/*
Tim Kieboom 1025003
*/
#ifndef TKARDUINO_TOOLS_H
#define TKARDUINO_TOOLS_H
#pragma once

template<typename A, typename B>
struct is_same { static const bool value = false; };

template<typename A>
struct is_same<A, A> { static const bool value = true; };

#include "./TKarduino_assert.h"

inline bool STR_EQUAL(const char* str1, const char* str2) {
    return strcmp(str1, str2) == 0;
}

/// @brief an imutable span of an array (THIS OBJECT DOES NOT DELTE THE PTR)
/// @tparam T any type
template<typename T>
class ConstSpan {
private:
    size_t offset;
    size_t _len;

public:
    const T* buffer;

    ConstSpan() 
        : offset(0), _len(0), buffer(nullptr)
    {

    } 
    ConstSpan(const T* buffer, size_t offset, size_t len) 
        : offset(offset), _len(len), buffer(buffer)
    {

    }

    int start() const {
        return offset;
    } 

    int end() const  {
        return _len-1;
    }

    size_t len() const {
        return _len - offset;
    }

    bool isEmpty() const  {
        return buffer == nullptr || _len - offset <= 0;
    }

    const T& operator[](int index) const  {
        ASSERT(buffer != nullptr);
        ASSERT_SMALLER((unsigned long)(index + offset), _len);
        return buffer[index + offset];
    }

    T* copy() const {
        ASSERT(buffer != nullptr);
        ASSERT_SMALLER(offset, _len);
        const size_t spanLength = _len - offset;

        T* buff = new T[spanLength];
        for(int i = 0; i < spanLength; i++)
            buff[i] = buffer[i + offset];
            
        return buff;
    }

    /// @brief puts span in a char array and adds '\0' at the end only for [T=char]
    /// @return c_str of span
    const char* copy_asCstr() const {
        static_assert(is_same<T, char>::value, "copy_asCstr() only available for ConstSpan<char>");

        ASSERT(buffer != nullptr);
        ASSERT_SMALLER(offset, _len);
        const size_t spanLength = _len - offset;

        char* str = new char[spanLength + 1];
        for(size_t i = 0; i < spanLength; i++)
            str[i] = buffer[i + offset];
            
        str[spanLength] = '\0';
        return str;
    }
};

/// @brief an array with a type thats it (THIS OBJECT DOES NOT DELTE THE PTR)
/// @tparam T any type
template<typename T>
class SmartArray {
private:
    size_t len;
    T* buffer;
public:

    SmartArray()
        : len(0), buffer(nullptr) 
    {

    } 

    SmartArray(T* buffer, size_t len) 
        : len(len), buffer(buffer)
    {

    }

    void free() {
        if(buffer != nullptr)
            delete[] buffer;

        buffer = nullptr;
    }

    bool isEmpty() const {
        return buffer == nullptr || len <= 0;
    }

    ConstSpan<T> constSpan(int start, int end) const {
        ASSERT(buffer != nullptr);
        ASSERT_BIGGER(start, -1);
        ASSERT_SMALLER((unsigned long)end, len+1);
        return ConstSpan<T>(buffer, start, end);
    }

    void resize(int newLen) {
        ASSERT_BIGGER(newLen, 0);
        T* buff = new T[newLen];
        for(size_t i = 0; i < min(newLen, len); i++)
            buff[i] = buffer[i];
        
        if(buffer != nullptr)    
            delete[] buffer;
        
        buffer = buff; 
        len = newLen;
    }

    T& operator[](int index) {
        ASSERT(buffer != nullptr);
        ASSERT_SMALLER((unsigned long)index, len);
        return buffer[index];
    }
};

#endif





