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
    size_t _len;

public:
    const T* buffer;

    ConstSpan() 
        : _len(0), buffer(nullptr)
    {

    } 
    ConstSpan(const T* buffer, size_t offset, size_t len) 
        :  _len(len - offset), buffer(&buffer[offset])
    {

    }

    size_t len() const {
        return _len;
    }

    const T* ptr() const {
        return buffer;
    }

    bool isEmpty() const  {
        return _len == 0;
    }

    const T& operator[](int index) const  {
        ASSERT(buffer != nullptr);
        ASSERT_SMALLER(index, _len);
        return buffer[index];
    }

    const T* tryAt(int index, bool& ok) const {
        ASSERT(buffer != nullptr);
        if(index >= _len) {
            ok = false;
            return nullptr;
        }

        ok = true;
        return &buffer[index];
    }

    T* copy() const {
        ASSERT(buffer != nullptr);

        T* buff = new T[_len];
        for(int i = 0; i < _len; i++)
            buff[i] = buffer[i];
            
        return buff;
    }

    bool equal(const ConstSpan<T>& other) const {
        if(_len != other.len())
            return false;
        
        return equal(other.ptr());
    }

    bool equal(const T* other) const { 
        if(buffer == nullptr || other == nullptr)
            return buffer == nullptr && other == nullptr;
        
        for(size_t i = 0; i < _len; i++)
        {
            if(buffer[i] != other[i])
                return false;
        }

        return true;
    }

    /// @brief puts span in a char array and adds '\0' at the end only for [T=char]
    /// @return c_str of span
    const char* copy_asCstr() const {
        static_assert(is_same<T, char>::value, "copy_asCstr() only available for ConstSpan<char>");

        ASSERT(buffer != nullptr);

        char* str = new char[_len + 1];
        for(size_t i = 0; i < _len; i++)
            str[i] = buffer[i];
            
        str[_len] = '\0';
        return str;
    }
};

/// @brief an array with a type thats it (THIS OBJECT DOES NOT DELTE THE PTR)
/// @tparam T any type
template<typename T>
class SmartArray {
private:
    size_t _len;
    T* buffer;
public:

    SmartArray()
        : _len(0), buffer(nullptr) 
    {

    } 

    SmartArray(size_t len) 
        : _len(len), buffer(new T[len])
    {

    }

    SmartArray(T* buffer, size_t len) 
        : _len(len), buffer(buffer)
    {

    }

    size_t len() const {
        return _len;
    }

    void free() {
        if(buffer != nullptr)
            delete[] buffer;

        buffer = nullptr;
    }

    bool isEmpty() const {
        return buffer == nullptr || _len <= 0;
    }

    ConstSpan<T> constSpan(int start, int end) const {
        ASSERT(buffer != nullptr);
        ASSERT_BIGGER(start, -1);
        ASSERT_SMALLER((unsigned long)end, _len+1);
        return ConstSpan<T>(buffer, start, end);
    }

    ConstSpan<T> asConstSpan() const {
        ASSERT(buffer != nullptr);
        return ConstSpan<T>(buffer, 0, _len);
    }

    T* copy() const {
        ASSERT(buffer != nullptr);

        T* buff = new T[_len];
        for(int i = 0; i < _len; i++)
            buff[i] = buffer[i];
            
        return buff;
    }

    /// @brief puts span in a char array and adds '\0' at the end only for [T=char]
    /// @return c_str of span
    const char* copy_asCstr() const {
        static_assert(is_same<T, char>::value, "copy_asCstr() only available for ConstSpan<char>");

        ASSERT(buffer != nullptr);

        char* str = new char[_len + 1];
        for(size_t i = 0; i < _len; i++)
            str[i] = buffer[i];
            
        str[_len] = '\0';
        return str;
    }

    void resize(int newLen) {
        ASSERT_BIGGER(newLen, 0);
        T* buff = new T[newLen];
        for(size_t i = 0; i < min(newLen, _len); i++)
            buff[i] = buffer[i];
        
        if(buffer != nullptr)    
            delete[] buffer;
        
        buffer = buff; 
        _len = newLen;
    }

    T& operator[](int index) {
        ASSERT(buffer != nullptr);
        ASSERT_SMALLER((unsigned long)index, _len);
        return buffer[index];
    }

    T* tryAt(int index, bool& ok) {
        ASSERT(buffer != nullptr);
        if(index >= _len) {
            ok = false;
            return nullptr;
        }

        ok = true;
        return &buffer[index];
    }
};

#endif





