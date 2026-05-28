/*
Tim Kieboom 1025003
*/
#pragma once
#include "assert.h"
#include "types.hpp"


/**
 * @brief A non-owning view over a contiguous array of elements.
 * 
 * Provides read-only access to an array, with bound checking in debug mode.
 */
template<typename T>
class Slice {
private:
    T const* _ptr = nullptr;
    usize _len = 0;
    
public:
    /** @brief Construct an empty slice. */
    constexpr Slice() {}

    /** @brief Construct a slice from a pointer and length. */
    constexpr Slice(T const* ptr, usize len): _ptr(ptr), _len(len) {}

    /** @brief Construct a slice from a pointer, length, and starting offset. */
    constexpr Slice(T const* ptr, usize len, usize offset): _ptr(ptr+offset), _len(len) {}

    /** @brief Returns a pointer to the underlying data. */
    T const* asPtr() const {return _ptr;}

    /** @brief Returns the number of elements in the slice. */
    usize len() const {return _len;}
    
    /** @brief Accesses the element at the given index (debug-bounds-checked). */
    T operator[](usize index) const {
        ASSERT_SMALLER(index, _len);
        return _ptr[index];
    }
    
    /** @brief Returns true if both slices have the same length and contents. */
    bool equal(Slice<T> const& other) const {
        if(this->len() != other.len())
        return false;
        
        for (usize i = 0; i < _len; i++) {
            if (_ptr[i] != other._ptr[i])
                return false;
        }
        return true;
    }
    
    /** @brief Prints the slice followed by a newline to Serial. */
    void println() const {
        print();
        Serial.print('\n');
    }
    
    /** @brief Prints the slice to Serial. */
    void print() const;
};

template<>
inline void Slice<char>::print() const {
    for (usize i = 0; i < _len; i++) {
        Serial.print(_ptr[i]);
    }
}

template<typename T>
void Slice<T>::print() const {
    Serial.print('[');

    for (usize i = 0; i < _len; i++) {
        Serial.print(_ptr[i]);

        if (i + 1 < _len)
            Serial.print(F(", "));
    }

    Serial.print(']');
}


/// Convenience alias for a Slice of characters (a string view).
typedef Slice<char> StrSlice;

/**
 * @brief Parses a StrSlice into an unsigned 8-bit integer.
 * @param str  The string to parse.
 * @param value Output parameter for the parsed value.
 * @return nullptr on success, or an error message on failure.
 */
const Fstr* parseU8(StrSlice str, MutRef<u8> value);