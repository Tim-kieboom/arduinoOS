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

#include "./assert.h"

inline bool strEquals(const char* str1, const char* str2) {
    return strcmp(str1, str2) == 0;
}

template<typename T>
class ConstSpan;
template<typename T>
class SmartArray;

template<typename T>
class BaseSmartArray {
protected:
    T* buffer;
    size_t _len;

public:
    constexpr size_t len() const {
        return _len;
    }

    constexpr T* ptr() const {
        return buffer;
    }

    constexpr bool isEmpty() const {
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
        if(buffer == nullptr)
            return nullptr;

        T* buff = new T[_len];
        ASSERT_ALLOC(buff);
        for(int i = 0; i < _len; i++)
            buff[i] = buffer[i];
            
        return buff;
    }

    /// @brief puts span in a char array and adds '\0' at the end only for [T=char]
    /// @return c_str of span
    const char* copy_asCstr() const {
        static_assert(is_same<T, char>::value, "copy_asCstr() only available for ConstSpan<char>");

        if(buffer == nullptr)
            return nullptr;

        char* str = new char[_len + 1];
        ASSERT_ALLOC(str);
        for(size_t i = 0; i < _len; i++)
            str[i] = buffer[i];
            
        str[_len] = '\0';
        return str;
    }

    bool equals(const SmartArray<T>& other) const {
        if(_len != other.len())
            return false;
        
        return equals(other.ptr(), other.len());
    }

    bool equals(const ConstSpan<T>& other) const {
        if(_len != other.len())
            return false;
        
        return equals(other.ptr(), other.len());
    }

    bool equals(const T* other, size_t len) const { 
        if(buffer == nullptr || other == nullptr)
            return buffer == nullptr && other == nullptr;
        
        if(len != _len)
            return false;

        for(size_t i = 0; i < _len; i++)
        {
            if(buffer[i] != other[i])
                return false;
        }

        return true;
    }

    IF_DEBUG_ELSE(const, constexpr) T& operator[](int index) const {
        ASSERT(buffer != nullptr);
        ASSERT_SMALLER((unsigned long)index, _len);
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
};

/// @brief an imutable span of an array with length (THIS OBJECT DOES NOT DELETE THE PTR)
/// @tparam T any type
template<typename T>
class ConstSpan : public BaseSmartArray<T> {
public:

    ConstSpan() {
        this->buffer = nullptr;
        this->_len = 0;
    } 

    ConstSpan(const T* buffer, size_t offset, size_t length) {
        this->buffer = const_cast<T*>(buffer) + offset;
        this->_len = length - offset;
    }
};

/// @brief an array and length (THIS OBJECT DOES NOT DELETE THE PTR)
/// @tparam T any type
template<typename T>
class SmartArray : public BaseSmartArray<T> {
public:

    SmartArray() {
        this->buffer = nullptr;
        this->_len = 0;
    } 

    SmartArray(size_t len) {
        this->buffer = new T[len];
        ASSERT_ALLOC(this->buffer);
        this->_len = len;
    }

    SmartArray(T* buffer, size_t len) {
        this->buffer = buffer;
        this->_len = len;
    }

    T* mutPtr() {
        return this->buffer;
    }

    void free() {
        if(this->buffer != nullptr)
            delete[] this->buffer;

        this->buffer = nullptr;
        this->_len = 0;
    }

    void setRange(SmartArray<T>& from) {
        ASSERT(this->buffer != nullptr);

        for(size_t i = 0; i < min(from.len(), this->_len); i++)
            this->buffer[i] = from[i];
    }

    T& operator[](int index) {
        ASSERT(this->buffer != nullptr);
        ASSERT_SMALLER((unsigned long)index, this->_len);
        return this->buffer[index];
    }
};

#endif





