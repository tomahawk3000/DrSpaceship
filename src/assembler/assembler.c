#include "assembler.h"

static void parseInstructions(Assembler *this);
static void writeToBinaryFile(Assembler *this);
static void initSourceLines(Assembler *assembler);

// constructs a new assembler and returns a pointer to it
Assembler *newAssembler(char *sourcePath, char *binaryPath) {
    Assembler *a = malloc(sizeof(Assembler));
    assemblerInit(a, sourcePath, binaryPath);

    return a;
}

// initialises an assembler
void assemblerInit(Assembler *this, char *sourcePath, char *binaryPath) {
    this->sourcePath = sourcePath;
    this->binaryPath = binaryPath;
    this->numLines = 0;
    this->numInstrs = 0;
    this->firstEmptyAddr = 0;
    this->currInstrAddr = 0;
    this->symbolTable = malloc(sizeof(ListMap));
    ListMapInit(this->symbolTable);
    initSourceLines(this);
}

//read all the lines (delimited by \n) in file to an array of strings
static void initSourceLines(Assembler *this) {
    //open the source file in read text mode.
    FILE *sourceFile = openFile(this->sourcePath, "rt");

    // get number of lines and allocate space for lines array.
    int numLines = countLines(sourceFile);
    this->numLines = numLines;
    this->sourceLines = malloc(sizeof(char *) * numLines);

    // allocate space for and read in each line
    for (int i = 0; i < numLines; ++i) {
        char *str = malloc(sizeof(char) * MAX_LINE_LENGTH);

        if (!fgets(str, MAX_LINE_LENGTH, sourceFile)) {
            fprintf(stderr, "Error reading line %d\n", i);
            exit(EXIT_FAILURE);
        }

        //strip leading space on line, and if it was empty, don't count it.
        str = skipSpace(str);
        if (*str != '\0') {
            this->sourceLines[i] = str;
        } else {
            --i;
        }
    }

    closeFile(sourceFile);
}

// assembles the source file
void assemble(Assembler *this) {
    // do first pass
    createSymbolTableAndCountInstrs(this);

    // do second pass
    parseInstructions(this);

    // write to binary file
    writeToBinaryFile(this);
}

// deinitialises an assembler
void assemblerDeInit(Assembler *this) {
    // free sourcelines
    for (int i = 0, n = this->numLines; i < n; i++) {
        free(this->sourceLines[i]);
    }
    free(this->sourceLines);

    // free binary program
    free(this->binaryProgram);

    // free symbolTable
    ListMapDeinit(this->symbolTable, &symbolTableDeleter);
    free(this->symbolTable);
}


void assemblerDeconstruct(Assembler *this) {
    assemblerDeInit(this);
    free(this);
}

// parses sourceLines into instructions stored in binaryProgram
static void parseInstructions(Assembler *this) {
    // initialise ListMap<mneumonic, functions>
    functionTable ft;
    functionTableInit(&ft);

    //allocate twice as many words as there are instructions (space for consts)
    this->binaryProgram = malloc(sizeof(uint32_t) * 2 * this->numInstrs);

    // initialise tokens, alloc'd in getTokens
    char *tokens[NUM_TOKENS];

    // for each line, get the tokens and generate the instruction
    for (int i = 0, n = this->numLines; i < n; i++) {
        getTokens(tokens, NUM_TOKENS, this->sourceLines[i]);
        char *mnem = tokens[0];

        // if mnem is null, we have a blank line from stripped label
        if (mnem != NULL) {
            this->binaryProgram[this->currInstrAddr / INSTR_LENGTH] =
                    functionTableGetAndApply(&ft, mnem, this, tokens);
            this->currInstrAddr += INSTR_LENGTH;
        }
    }

    functionTableDeinit(&ft);
}

// writes binaryProgram to the file with name binaryPath
static void writeToBinaryFile(Assembler *this) {
    // write to the first empty address
    int numToWrite = this->firstEmptyAddr / INSTR_LENGTH;
    assert(numToWrite > 0 && this->binaryProgram != NULL);

    FILE *outfile = openFile(this->binaryPath, "wb");

    // write to the binaryProgram array, checking for errors
    int numWritten =
            (int) fwrite(this->binaryProgram, sizeof(uint32_t),
            numToWrite, outfile);
    if (numWritten != numToWrite) {
        fputs("Assembler: Error When writing instructions to binary file.\n",
                stderr);
    }

    // close the outfile
    if (fclose(outfile) == EOF) {
        fputs("Assembler: Error closing binary file", stderr);
    }
}
