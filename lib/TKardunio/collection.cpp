#include "collection.hpp"

inline bool isNumber(int8_t currentDecimal) {
  return currentDecimal > -1 && currentDecimal < 10;
}

const Fstr* parseU8(StrSlice str, MutRef<u8> value) {
    if(str.len() == 0)
        return F("!!error!! string is empty");

    if(str[0] == '-')
        return F("!!error!! string is negative number");
    
    u16 number = 0;
    for(usize i = 0; i < str.len(); i++) {
        number *= 10;
        u8 currentDecimal = (u8)(str[i] - '0');
        if(!isNumber(currentDecimal))
            return F("!!error!! string is not valid number");

        number += currentDecimal;
    }

    if(number > UINT8_MAX)
        return F("!!error!! string number is bigger then 255");
        
    value.ref = (u8)number;
    return nullptr;
}
