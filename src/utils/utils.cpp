/*
Tim Kieboom 1025003
*/
#include "utils.h"
#include <TKarduino.h>

bool checkArguments(InputBuffer &input, int shouldBe) {
    if(input.tokenEndIndexes_len-1 != shouldBe){
        
        int numArgs =- (input.tokenEndIndexes_len==0) ? 0 : input.tokenEndIndexes_len-1;
        printM(F("command should have "), shouldBe, F(" arguments has: '"), numArgs, F("'\n"));
        return false;
    }
    return true;
}