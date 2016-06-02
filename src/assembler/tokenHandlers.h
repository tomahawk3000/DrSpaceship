//
// Created by drspaceship on 31/05/16.
//

#include <stdint.h>
#include "assembler.h"
#include <util/util.h>

#ifndef ARM11_39_TOKENHANDLERS_H
#define ARM11_39_TOKENHANDLERS_H



uint32_t handleDataProcessing(Assembler assembler, char **tokens);
uint32_t handleMultiply(Assembler assembler, char **tokens);
uint32_t handleSDT(Assembler assembler, char **tokens);
uint32_t handleBranch(Assembler assembler, char **tokens);
uint32_t handleHalt(Assembler assembler, char **tokens);
uint32_t handleLSL(Assembler assembler, char **tokens);





#endif //ARM11_39_TOKENHANDLERS_H
