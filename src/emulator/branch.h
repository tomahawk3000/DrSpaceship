#ifndef ARM11_39_BRANCH_H
#define ARM11_39_BRANCH_H

#include <stdint.h>
#include <stdio.h>
#include <util/util.h>
#include <emulate.h>

#define OFFSET_SIZE 24
#define SHIFT_TO_OFFSET 2

void branch(uint32_t instr);

#endif //ARM11_39_BRANCH_H
