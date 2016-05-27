#ifndef ARM11_39_PRINTSTATE_H
#define ARM11_39_PRINTSTATE_H


#include <string.h>
#include "emulate.h"

#define LAST_GENERAL_REGISTER 12
#define LAST_SINGLE_DIGIT 9
#define HEX_PADDING 8
#define LONGEST_REGISTER_NAME "CPSR"
#define MEM_PRINTING_INTERVAL 4

void printState(void);


#endif //ARM11_39_PRINTSTATE_H
