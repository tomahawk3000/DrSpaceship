//
// Created by drspaceship on 31/05/16.
//

#include "functionTable.h"

#define NUM_MNEMONICS 24



void functionTableInit(functionTable *this) {
    ListMapInit(&this->listmap);

    functionTableAdd(this, "add", &handleDataProcessing);
    functionTableAdd(this, "sub", &handleDataProcessing);
    functionTableAdd(this, "rsb", &handleDataProcessing);
    functionTableAdd(this, "and", &handleDataProcessing);
    functionTableAdd(this, "eor", &handleDataProcessing);
    functionTableAdd(this, "orr", &handleDataProcessing);
    functionTableAdd(this, "mov", &handleDataProcessing);
    functionTableAdd(this, "tst", &handleDataProcessing);
    functionTableAdd(this, "teq", &handleDataProcessing);
    functionTableAdd(this, "cmp", &handleDataProcessing);
    functionTableAdd(this, "mul", &handleMultiply);
    functionTableAdd(this, "mla", &handleMultiply);
    functionTableAdd(this, "ldr", &handleSDT);
    functionTableAdd(this, "str", &handleSDT);
    functionTableAdd(this, "beq", &handleBranch);
    functionTableAdd(this, "bne", &handleBranch);
    functionTableAdd(this, "bge", &handleBranch);
    functionTableAdd(this, "blt", &handleBranch);
    functionTableAdd(this, "bgt", &handleBranch);
    functionTableAdd(this, "ble", &handleBranch);
    functionTableAdd(this, "b", &handleBranch);
    functionTableAdd(this, "lsl", &handleLSL);
    functionTableAdd(this, "andeq", &handleHalt);
    functionTableAdd(this, "halt", &handleHalt);
}

void functionTableAdd(
        functionTable *this,
        char *mnen,
        assembleFunctionPointer func
) {
    ListMapAdd(&this->listmap, mnen, (void **) &func);
}

assembleFunctionPointer *functionTableGet(
        functionTable *this,
        char *mnen
) {
    return ListMapGet(&this->listmap, mnen, &strcmpFromVoid);
}

// TODO: name better, maybe incorporate the getting as well
uint32_t functionTableGetAndApply(
        functionTable *this,
        char *mnen,
        Assembler *a,
        char **tokens
) {
//    assembleFunctionPointer *fp = functionTableGet(this, mnen);
//    return (*fp)(a, tokens);
    char* mnems[NUM_MNEMONICS] =
            {"add", "sub", "rsb", "and", "eor", "orr", "mov", "tst", "teq",
             "cmp", "mul", "mla", "ldr", "str", "beq", "bne", "bge", "blt",
             "bgt", "ble", "b", "lsl", "andeq", "halt"};
    assembleFunctionPointer asmFuncs[NUM_MNEMONICS] =
            {&handleDataProcessing, &handleDataProcessing, &handleDataProcessing,
             &handleDataProcessing, &handleDataProcessing, &handleDataProcessing,
             &handleDataProcessing, &handleDataProcessing, &handleDataProcessing,
             &handleDataProcessing, &handleMultiply, &handleMultiply, &handleSDT,
             &handleSDT, &handleBranch, &handleBranch, &handleBranch,
             &handleBranch, &handleBranch, &handleBranch, &handleBranch,
             &handleLSL, &handleHalt, &handleHalt};

    for (int i = 0; i < NUM_MNEMONICS; ++i) {
        if (!strcmp(mnems[i], mnen)) {
            return (asmFuncs[i])(a, tokens);
        }
    }

    fprintf(stderr, "mnemonic not matched to any assemble function");
    exit(EXIT_FAILURE);
}
