//
// Created by drspaceship on 22/05/16.
//
#include "util.h"

// POST: returns a 32bit int shifted based on shiftType
ShiftResult binaryShift(uint32_t shiftee, shiftType st, uint32_t amount) {
    ShiftResult sr;

    //save some work and make edge cases easier.
    if (amount == 0) {
        sr.carry = 0;
        sr.result = shiftee;
    }

    //don't shift by more than bits than are in the shiftee
    if (amount > INTWIDTH) {
        return binaryShift(shiftee, st, amount % INTWIDTH);
    }

    switch(st) {
        case LSL:
            //gets the first bit of everything shifted off the left side
            sr.carry = (shiftee >> (INTWIDTH - amount)) & 1;
            sr.result = shiftee << amount;
        case LSR:
            sr.carry = rightShiftCarry(shiftee, amount);
            sr.result = shiftee >> amount;
        case ASR:
            sr.carry = rightShiftCarry(shiftee, amount);

            //start with only most significant bit, and repeat it to the left
            uint32_t signBit = shiftee & (1 << (amount - 1));
            for (int i = 0; i < amount; ++i) {
                signBit = signBit | signBit >> 1;
            }
            sr.result = (shiftee >> amount) | signBit;
        case ROR:
            sr.carry = rightShiftCarry(shiftee, amount);
            //least significant bits that rotate around
            sr.result = (shiftee << (INTWIDTH - amount)) |
            //everything else
                        (shiftee >> amount);
        default:
            fprintf(stderr, "Error in binaryShift(): "
                            "unrecognised shift type: %i\n", st);
    }

    return sr;
}

//gets the carry for any right shift by shifting it by one less, making the
//first bit the carry we require.
uint32_t rightShiftCarry(uint32_t shiftee, uint32_t amount) {
    return (shiftee >> (amount - 1)) & 1;
}

// creates a bit mask to extract the bits i to j by &&-ing with this mask
uint32_t createMask(unsigned int i, unsigned int j) {
    uint32_t mask = 0;

    for (unsigned int m = i; m <= j; m++) {
        mask |= 1 << m;
    }

    return mask;
}

// Returns the bits at positions j to i from the given number.
// unsigned -> signed cast of same width integers is guarenteed not to
// change bit pattern but signed -> unsigned may
uint32_t extractBits(uint32_t binaryNumber, int j, int i) {
    // ensure j >= i and both i and j are positive and both less than the number
    // of bits in the binaryNumber
    const int numBits = sizeof(binaryNumber) * CHAR_BIT - 1;
    if (j < i || i < 0 || j > numBits) {
        fprintf(stderr, "Error in util: extractBits(): "
                "invalid indexes from which to extract bits. You attempted "
                "extractBits(0x%08x, %d, %d)\n", binaryNumber, j, i);
        exit(2);
    }

    uint32_t mask = createMask(i, j);
    binaryNumber &= mask;
    binaryNumber >>= i;

    return binaryNumber;
}

uint32_t extractBit(uint32_t binaryNumber, int i) {
    return extractBits(binaryNumber, i, i);
}

// dest: pointer to a 32bit to put the result into
// src:  pointer to a memory address (element of 8 bit array)
// post: Will read four consecutive 8 bit elements from memory starting from
//       address src. --Reads the bytes accounting for that fact that they are
//       ordered according to the wrong endianness-- wrong, no byte order change
void read32Bits(uint32_t *dest, uint8_t *src) {
    // get number of memory adddresses we will have to read to accumulate
    // 32 bits
    const int n = sizeof(uint32_t) / WORD_SIZE;

    // declare array of the bytes we will read and zero out the destination
    uint32_t bytesToRead[n];
    *dest = 0;

    for (int i = n - 1; i >= 0; i--) {
        bytesToRead[i] = (uint32_t) *(src + i);
        // bytesToRead[i] <<= i * CHAR_BIT; dont want to swap endianness now

        *dest |= bytesToRead[i];
    }
}

// swaps the endianness of an instr
uint32_t swapEndianness(uint32_t instr) {
    const int n = INTWIDTH / CHAR_BIT;
    uint32_t instrBytes[n];

    uint32_t swappedEndianInstr = 0;

    // swap the endianness
    for (int i = 0; i < n; i++ ) {
        instrBytes[i] = extractBits(instr, (i + 1) * CHAR_BIT - 1,
                i * CHAR_BIT);
        instrBytes[i] <<= (n - i - 1) * CHAR_BIT;

        swappedEndianInstr |= instrBytes[i];
    }

    return swappedEndianInstr;
}

// Swaps the endianness of instr, converts the indexes and then extracts
// the bits inbetween those indexes
// PRE: upperBit should be less than lowerBit because the endianness of instrBytes
//      will be swapped.
uint32_t extractFragmentedBits(uint32_t instr, int upperBit, int lowerBit) {
    if (lowerBit < 0 || upperBit > INTWIDTH - 1 || lowerBit < upperBit) {
        fprintf(stderr, "Error in extractFragmentedBits: "
                        "Invalid indexes from which to extract bits. You"
                        " attempted extractFragmentedBits(%d, %d, %d)",
                        instr, upperBit, lowerBit);
        exit(2);
    }

    uint32_t swappedEndianInstr = swapEndianness(instr);

    upperBit = SWAP_INDEX_ENDIANNESS(upperBit);
    lowerBit = SWAP_INDEX_ENDIANNESS(lowerBit);

    return extractBits(swappedEndianInstr, upperBit, lowerBit);
}

// sign extend an n-bit number.
// uses division instead of right shift because c standard does not force
// right shift to be arithmetical instead of logical
void signExtend(int32_t *i, int n) {
    if (n > INTWIDTH) {
        fprintf(stderr, "Error in util: Attempting to sign extend a %d bit"
                "number to %d bits", n, INTWIDTH);
    }

    *i <<= (INTWIDTH - n);
    *i /= 2 ^ (INTWIDTH - n);
}
