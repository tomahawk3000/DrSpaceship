//
// Created by drspaceship on 22/05/16.
//

#include "dataProcess.h"

// PRE: instr is a data process instruction
// assume: PC does not feature in the instruction
// behavior: checks opcode and performs instruction, possibly writes result to register and updates flags

void dataProcess(uint32_t instr) {
    bool I = (bool) extractBit(instr, IMM_BIT);
    uint32_t opcode = extractBits(instr, Opcode_Start, Opcode_End);
    bool S = (bool) extractBit(instr, S_BIT);
    uint32_t RnVal = REGFILE[extractBits(instr, Rn_UPPER, Rn_LOWER)];
    uint32_t Rd = extractBits(instr, Rd_UPPER, Rd_LOWER);
    ShiftResult operand2WC = getOperand2(instr, I);
    uint32_t operand2 = operand2WC.result;
    bool C = operand2WC.carry;
    bool writeResult = true;
    uint32_t result = 0;

    switch (opcode) {
        case 1000:
            writeResult = false;
        case 0000:
            result = RnVal & operand2;
            break;
        case 1001:
            writeResult = false;
        case 0001:
            result = RnVal ^ operand2;
            break;
        case 1010:
            writeResult = false;
        case 0010:
            result = RnVal - operand2;
            C = RnVal >= operand2;
            break;
        case 0011:
            result = operand2 - RnVal;
            C = operand2 >= RnVal;
            break;
        case 0100:
            result = RnVal + operand2;
            C = extractBit(RnVal, 31) && extractBit(operand2, 31);
            break;
        case 1100:
            result = RnVal | operand2;
            break;
        case 1101:
            result = operand2;
    }


    if (S) {
        updateCPSR(C, Cbit);
        updateCPSR(!result, Zbit);
        updateCPSR((bool) extractBit(result, 31), Nbit);
    }

    if (writeResult) {
        REGFILE[Rd] = result;
    }

}