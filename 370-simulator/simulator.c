/*
 * Project 1
 * EECS 370 LC-2K Instruction-level simulator
 *
 * Make sure to NOT modify printState or any of the associated functions
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

//DO NOT CHANGE THE FOLLOWING DEFINITIONS 

// Machine Definitions
#define MEMORYSIZE 65536 /* maximum number of words in memory (maximum number of lines in a given file)*/
#define NUMREGS 8 /*total number of machine registers [0,7]*/

// File Definitions
#define MAXLINELENGTH 1000 /* MAXLINELENGTH is the max number of characters we read */

typedef struct 
stateStruct {
    int pc;
    int mem[MEMORYSIZE];
    int reg[NUMREGS];
    int numMemory;
} stateType;

void printState(stateType *);

static inline int convertNum(int32_t);

int 
main(int argc, char **argv)
{
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;

    if (argc != 2) {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
        printf("error: can't open file %s , please ensure you are providing the correct path", argv[1]);
        perror("fopen");
        exit(1);
    }

    /* read the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL; ++state.numMemory) {
        if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
            printf("error in reading address  %d\n. Please ensure you are providing a machine code file.", state.numMemory);
            perror("sscanf");
            exit(1);
        }
        printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }

    //Your code starts here!
    //
    //
    

    //initialize the starting state
    for (int i = 0; i < NUMREGS; i++) {
        state.reg[i] = 0;
    }
    state.pc = 0;
    // end intitialization
    
    //start reading lines of memory
    
    //instruction counter
    int instructions = 0;
    
    for (; state.pc < state.numMemory; state.pc++) {
        instructions++;
        
        //first, print the current state.
        printState(&state);
        // end of print state
        
        //first, we will extract the opcode value.
        int opcode = 0b111;
        opcode = opcode<<22;
        opcode = state.mem[state.pc] & opcode;
        opcode = opcode>>22;
        
        //now performing actions according to the opcode..
        
        //add
        if (opcode == 0) {
            
            int regA = 0b111;
            regA = regA<<19;
            regA = regA & state.mem[state.pc];
            regA = regA>>19;
            
            int regB = 0b111;
            regB = regB<<16;
            regB = regB & state.mem[state.pc];
            regB = regB>>16;
            
            int DestReg = 0b111;
            DestReg = DestReg & state.mem[state.pc];
            
            state.reg[DestReg] = state.reg[regA] + state.reg[regB];
            
        }
        
        //nor
        else if (opcode == 1) {
            
            int regA = 0b111;
            regA = regA<<19;
            regA = regA & state.mem[state.pc];
            regA = regA>>19;
            
            int regB = 0b111;
            regB = regB<<16;
            regB = regB & state.mem[state.pc];
            regB = regB>>16;
            
            int DestReg = 0b111;
            DestReg = DestReg & state.mem[state.pc];
            
            state.reg[DestReg] =  ~(state.reg[regA] | state.reg[regB]);
            
        }
        
        //lw
        else if (opcode == 2) {
            
            int regA = 0b111;
            regA = regA<<19;
            regA = regA & state.mem[state.pc];
            regA = regA>>19;
            
            int regB = 0b111;
            regB = regB<<16;
            regB = regB & state.mem[state.pc];
            regB = regB>>16;
            
            int offset = 0b1111111111111111; // 16-bit twos-comp
            offset = offset & state.mem[state.pc];
            offset = convertNum(offset);
            
            offset += state.reg[regA];
            
            int load_val = state.mem[offset];
            
            state.reg[regB] = load_val;
        }
        
        //sw
        else if (opcode == 3) {
            
            int regA = 0b111;
            regA = regA<<19;
            regA = regA & state.mem[state.pc];
            regA = regA>>19;
            
            int regB = 0b111;
            regB = regB<<16;
            regB = regB & state.mem[state.pc];
            regB = regB>>16;
            
            int offset = 0b1111111111111111; // 16-bit twos-comp
            offset = offset & state.mem[state.pc];
            offset = convertNum(offset);
            
            offset += state.reg[regA];
            
            state.mem[offset] = state.reg[regB];
        }
        
        //beq
        else if (opcode == 4) {
            
            int regA = 0b111;
            regA = regA<<19;
            regA = regA & state.mem[state.pc];
            regA = regA>>19;
            
            int regB = 0b111;
            regB = regB<<16;
            regB = regB & state.mem[state.pc];
            regB = regB>>16;
            
            int offset = 0b1111111111111111; // 16-bit twos-comp
            offset = offset & state.mem[state.pc];
            offset = convertNum(offset);
            
            if (state.reg[regA] == state.reg[regB]) {
                state.pc += offset;
            }
            
        }
        
        //jalr
        else if (opcode == 5) {
            
            int regA = 0b111;
            regA = regA<<19;
            regA = regA & state.mem[state.pc];
            regA = regA>>19;
            
            int regB = 0b111;
            regB = regB<<16;
            regB = regB & state.mem[state.pc];
            regB = regB>>16;
            
            //link
            state.reg[regB] = state.pc + 1;
            
            //jump
            state.pc = state.reg[regA] - 1;
        }
        
        //halt
        else if (opcode == 6) {
            
            //ending the loop
            printf("machine halted\n");
            printf("total of %d instructions executed\n", instructions);
            printf("final state of machine:\n");
            state.pc++;
            printState(&state);
            state.pc = state.numMemory;
        }
        
        //noop
        else if (opcode == 7) {
            //do nothing lol
        }
        // end of else-if opcode
    }
    // end of reading
    
    

    //
    //
    //Your code ends here!

    return(0);
}

/*
* DO NOT MODIFY ANY OF THE CODE BELOW. 
*/

void 
printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
    for (i=0; i<statePtr->numMemory; i++) 
              printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
    printf("\tregisters:\n");
    for (i=0; i<NUMREGS; i++) 
              printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    printf("end state\n");
}

// convert a 16-bit number into a 32-bit Linux integer
static inline int convertNum(int num)
{
    return num - ( (num & (1<<15)) ? 1<<16 : 0 );
}

/*
* Write any helper functions that you wish down here. 
*/

