/*
Tim Kieboom 1025003
*/
#include "utils.h"

bool checkArguments(InputBuffer &input, int shouldBe) {
    if(input.tokenEndIndexes_len-1 != shouldBe)
    {
        Serial.print("command should have ");
        Serial.print(shouldBe);
        Serial.print(" arguments has: '");
        Serial.print((input.tokenEndIndexes_len==0) ? 0 : input.tokenEndIndexes_len-1);
        Serial.println("'");
        return false;
    }
    return true;
}