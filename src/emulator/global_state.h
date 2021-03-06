#ifndef ARM11_39_GLOBAL_STATE_H
#define ARM11_39_GLOBAL_STATE_H

#include <stdint.h>

#define MEM_SIZE 65536
#define NUM_REGISTERS 17

// memory and registers are little endian whilst GPIO pins are big endian
typedef struct ARMstate {
    uint8_t *memory;
    uint32_t *registers;
    uint32_t controlBitsGPIO0To9;
    uint32_t controlBitsGPIO10To19;
    uint32_t controlBitsGPIO20To29;
    uint32_t clearPins;
    uint32_t onPins;
} ARMstate;

extern ARMstate state;

#endif //ARM11_39_GLOBAL_STATE_H
