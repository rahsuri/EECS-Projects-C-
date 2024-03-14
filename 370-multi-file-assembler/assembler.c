/**
 * Project 1
 * Assembler code fragment for LC-2K
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

//Every LC2K file will contain less than 1000 lines of assembly.
#define MAXLINELENGTH 1000

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int createInstr(int opcode_int, int field0, int field1, int field2);
int twosComp(int posInt);
static void checkForBlankLinesInCode(FILE *inFilePtr);
static inline int isNumber(char *);

struct Symbol {
    char label[MAXLINELENGTH];
    char type;
    int offset;
};

struct Reloc {
    int offset;
    char opcode[MAXLINELENGTH];
    char symbol[MAXLINELENGTH];
};

int
main(int argc, char **argv)
{
    char *inFileString, *outFileString;
    FILE *inFilePtr, *outFilePtr;
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
            arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

    if (argc != 3) {
        printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
            argv[0]);
        exit(1);
    }

    inFileString = argv[1];
    outFileString = argv[2];

    inFilePtr = fopen(inFileString, "r");
    if (inFilePtr == NULL) {
        printf("error in opening %s\n", inFileString);
        exit(1);
    }

    // Check for blank lines in the middle of the code.
    checkForBlankLinesInCode(inFilePtr);

    outFilePtr = fopen(outFileString, "w");
    if (outFilePtr == NULL) {
        printf("error in opening %s\n", outFileString);
        exit(1);
    }

    //CHECK
    /* here is an example for how to use readAndParse to read a line from
        inFilePtr */
    
    //checking for duplicate label definition
    char curr_label[MAXLINELENGTH];
    int line_counter = 1;
    
    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2) ) {
        if (!isNumber(label) && label[0] != '\0') {
            FILE* inFilePtr2 = fopen(inFileString, "r");
            char buffer[MAXLINELENGTH];
            for (int i = 0; i < line_counter; i++) {
                fgets(buffer, MAXLINELENGTH, inFilePtr2);
            }
            strcpy(curr_label, label);
            while(readAndParse(inFilePtr2, label, opcode, arg0, arg1, arg2)) {
                if (!strcmp(curr_label, label)) {
                    printf("duplicate labels\n");
                    exit(1);
                }
            }
            fclose(inFilePtr2);
        }
        line_counter++;
    }
    
    line_counter--;
    
    //rewinding
    rewind(inFilePtr);
    
    //P2A Data Structures
    int assembly_lines[line_counter];
    int data_lines = 0; //number of .fills
    struct Symbol * symbols[MAXLINELENGTH];
    int num_symbols = 0;
    struct Reloc * relocs[MAXLINELENGTH];
    int num_relocs = 0;
    
    //now converting assembly to machine.
    bool fill_found = false;
    int curr_address = 0;
    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
        int opcode_int;
        int field0;
        int field1;
        int field2 = 0;
        bool label_found = false;
        
        //for lw and sw
        if (!isNumber(arg2) && arg2[0] != ' ' && strcmp(opcode, "halt") && strcmp(opcode, "noop") && strcmp(opcode, ".fill")) {
            if ((!strcmp(opcode, "lw") || !strcmp(opcode, "sw"))) {
                char find[MAXLINELENGTH];
                char junk[MAXLINELENGTH];
                char search[MAXLINELENGTH];
                strcpy(find, arg2);
                int address = 0;
                bool address_found = false;
                FILE* inFilePtr2 = fopen(inFileString, "r");
                while (readAndParse(inFilePtr2, search, junk, junk, junk, junk)) {
                    if (!strcmp(find, search)) {
                        address_found = true;
                        break;
                    }
                    address++;
                }
                fclose(inFilePtr2);
                
                if (address_found == true) { //handling when label is found (global and local)
                    label_found = true;
                    field2 = address;
                }
                else if (address_found == false && isupper(arg2[0])){ //handling when label is not found, but it is a global
                    label_found = true;
                    field2 = 0;
                }
                else { //any other case (label is local and not defined) then we exit(1)
                    printf("undefined label: %s\n", label);
                    exit(1);
                }
                

            }
            else if (!strcmp(opcode, "beq")){
                char find[MAXLINELENGTH];
                char junk[MAXLINELENGTH];
                char search_def[MAXLINELENGTH];
                strcpy(find, arg2);
                int address = 0;
                bool address_found = false;
                FILE* inFilePtr2 = fopen(inFileString, "r");
                while (readAndParse(inFilePtr2, search_def, junk, junk, junk, junk)) {
                    if (!strcmp(find, search_def)) {
                        address_found = true;
                        break;
                    }
                    address++;
                }
                fclose(inFilePtr2);
                
                if (address_found == false) {
                    printf("undefined label: %s\n", label);
                    exit(1);
                }
                label_found = true;
                field2 = address - curr_address - 1;
                if (field2 > 32767 || field2 < -327678) {
                    printf("offset field larger than 16 bits: %d", field2);
                    exit(1);
                }
            }
               
        }
        
        
        
        //field0 initialization
//        printf("arg0: %s", arg0);
        
        if (!strcmp(opcode, ".fill")) {
            fill_found = true;
            data_lines++;
            if (!isNumber(arg0)) {
                char find[MAXLINELENGTH];
                char junk[MAXLINELENGTH];
                char search[MAXLINELENGTH];
                strcpy(find, arg0);
                int address = 0;
                bool address_found = false;
                FILE* inFilePtr2 = fopen(inFileString, "r");
                while (readAndParse(inFilePtr2, search, junk, junk, junk, junk)) {
                    if (!strcmp(find, search)) {
                        address_found = true;
                        break;
                    }
                    address++;
                }
                fclose(inFilePtr2);
                
                
                if (address_found == true) {
                    label_found = true;
                    field0 = address;
                }
                else if (address_found == false && isupper(arg0[0])) {
                    label_found = true;
                    field0 = 0;
                }
                else {
                    printf("undefined label: %s\n", label);
                    exit(1);
                }
            }
            else {
                field0 = atoi(arg0);
            }
            
            if (field0 < 0) {
                field0 *= -1;
                field0 = twosComp(field0);
            }
            
//            printf("instruction: %d\n", field0);
//            fprintf(outFilePtr, "%d", field0);
//            fprintf(outFilePtr, "\n");
            assembly_lines[curr_address] = field0;
        }
        else {
            if (fill_found == true) {
                printf("intermixed .fill\n");
                exit(1);
            }
            field0 = atoi(arg0);
            
            //if opcode is not fill, checking that register1 is 0-7
            if (field0 > 7 || field0 < 0) {
                printf("invalid register\n");
                exit(1);
            }
            
//            printf("arg1: %s", arg1);
            field1 = atoi(arg1);
            
            
            //checking that register1 is 0-7
            if (field1 > 7 || field1 < 0) {
                printf("invalid register\n");
                exit(1);
            }
            if (label_found == false) {
                field2 = atoi(arg2);
            }
            
            //converting opcode
            if (!strcmp(opcode, "add")) {
                /* do whatever you need to do for opcode "add" */
                if (!isNumber(arg0) || !isNumber(arg1) || !isNumber(arg2)) {
                    printf("Non-integer register!\n");
                    exit(1);
                }
                opcode_int = 0;
                
            }
            else if (!strcmp(opcode, "nor")) {
                /* do whatever you need to do for opcode "add" */
                if (!isNumber(arg0) || !isNumber(arg1) || !isNumber(arg2)) {
                    printf("Non-integer register!\n");
                    exit(1);
                }
                opcode_int = 1;
                
            }
            else if (!strcmp(opcode, "lw")) {
                /* do whatever you need to do for opcode "add" */
                if (!isNumber(arg0) || !isNumber(arg1)) {
                    printf("Non-integer register!\n");
                    exit(1);
                }
                opcode_int = 2;
                
            }
            else if (!strcmp(opcode, "sw")) {
                /* do whatever you need to do for opcode "add" */
                if (!isNumber(arg0) || !isNumber(arg1)) {
                    printf("Non-integer register!\n");
                    exit(1);
                }
                opcode_int = 3;
            
            }
            else if (!strcmp(opcode, "beq")) {
                /* do whatever you need to do for opcode "add" */
                if (!isNumber(arg0) || !isNumber(arg1)) {
                    printf("Non-integer register!\n");
                    exit(1);
                }
                opcode_int = 4;
                
            }
            else if (!strcmp(opcode, "jalr")) {
                /* do whatever you need to do for opcode "add" */
                if (!isNumber(arg0) || !isNumber(arg1)) {
                    printf("Non-integer register!\n");
                    exit(1);
                }
                opcode_int = 5;
                
            }
            else if (!strcmp(opcode, "halt")) {
                /* do whatever you need to do for opcode "add" */
                opcode_int = 6;
                
            }
            else if (!strcmp(opcode, "noop")) {
                /* do whatever you need to do for opcode "add" */
                opcode_int = 7;
                
            }
            //invalid opcode
            else {
                printf("invalid opcode: %s\n", opcode);
                exit(1);
            }
            
            //finally, creating and printing out instruction
            int instr = createInstr(opcode_int, field0, field1, field2);
            
//            printf("instruction: %d\n", instr);
//            fprintf(outFilePtr, "%d", instr);
//            fprintf(outFilePtr, "\n");
            assembly_lines[curr_address] = instr;
        }
        curr_address++;
    } //end of assembly conversion
    
    //start of symbolic and relocation table creation
    rewind(inFilePtr);
    
    //
    //SYMBOL TABLE THINGS
    //
    
    //handing global symbolic labels
    curr_address = 0;
    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2) ) {
        if (!isNumber(label) && label[0] != '\0' && isupper(label[0])) {
            char type;
            int offset;
            if (curr_address < (line_counter - data_lines)) {
                type = 'T';
                offset = curr_address;
            }
            else {
                type = 'D';
                offset = curr_address - (line_counter - data_lines);
            }
            struct Symbol * ptr = (struct Symbol*)malloc(sizeof(struct Symbol));
            
            strcpy(ptr->label, label);
            ptr->type = type;
            ptr->offset = offset;
            
            symbols[num_symbols] = ptr;
            num_symbols++;
        }
        curr_address++;
    }
    
    rewind(inFilePtr);
    
    //handing global symbolic variables
    curr_address = 0;
    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2) ) {
        if (!strcmp(opcode, "lw") || !strcmp(opcode, "sw")) {
            if (!isNumber(arg2) && arg2[0] != '\0' && isupper(arg2[0])) { //assuming the symbolic address is for a lw or sw.
                bool defined = false;
                for (int i = 0; i < num_symbols; i++) {
                    if (!strcmp(arg2, symbols[i]->label)) {
                        defined = true;
                    }
                }
                if (defined == false) {
                    struct Symbol * ptr = (struct Symbol*)malloc(sizeof(struct Symbol));
                    
                    strcpy(ptr->label, arg2);
                    ptr->type = 'U';
                    ptr->offset = 0;
                    
                    symbols[num_symbols] = ptr;
                    num_symbols++;
                }
            }
        }
        else if (!strcmp(opcode, ".fill")) {
            if (!isNumber(arg0) && arg0[0] != '\0' && isupper(arg0[0])) { //assuming the symbolic address is for a lw or sw.
                bool defined = false;
                for (int i = 0; i < num_symbols; i++) {
                    if (!strcmp(arg0, symbols[i]->label)) {
                        defined = true;
                    }
                }
                if (defined == false) {
                    struct Symbol * ptr = (struct Symbol*)malloc(sizeof(struct Symbol));
                    
                    strcpy(ptr->label, arg0);
                    ptr->type = 'U';
                    ptr->offset = 0;
                    
                    symbols[num_symbols] = ptr;
                    num_symbols++;
                }
            }
        }
        curr_address++;
    }
    
    //
    //RELOCATION TABLE TINGS
    //
    
    rewind(inFilePtr);
    
    curr_address = 0;
    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2) ) {
        if (!strcmp(opcode, "lw") || !strcmp(opcode, "sw")) {
            if (!isNumber(arg2) && arg2[0] != '\0') {
                struct Reloc * ptr = (struct Reloc*)malloc(sizeof(struct Reloc));
                
                ptr->offset = curr_address;
                strcpy(ptr->opcode, opcode);
                strcpy(ptr->symbol, arg2);
                
                relocs[num_relocs] = ptr;
                num_relocs++;
            }
        }
        else if (!strcmp(opcode, ".fill")) {
            if (!isNumber(arg0) && arg0[0] != '\0') {
                int offset = curr_address - (line_counter - data_lines);
                
                struct Reloc * ptr = (struct Reloc*)malloc(sizeof(struct Reloc));
                
                ptr->offset = offset;
                strcpy(ptr->opcode, opcode);
                strcpy(ptr->symbol, arg0);
                
                relocs[num_relocs] = ptr;
                num_relocs++;
            }
        }
        curr_address++;
    }
    
    //
    //WRITING TO FILE
    //
    
    //Header
    fprintf(outFilePtr, "%d %d %d %d\n", (line_counter - data_lines), data_lines, num_symbols, num_relocs);
    
    //Text & Data
    for (int i = 0; i < line_counter; i++) {
        fprintf(outFilePtr, "%d\n", assembly_lines[i]);
    }
    
    //Symbol Table
    for (int i = 0; i < num_symbols; i++) {
        fprintf(outFilePtr, "%s %c %d\n", symbols[i]->label, symbols[i]->type, symbols[i]->offset);
    }
    
    //Reloc Table
    for (int i = 0; i < num_relocs; i++) {
        fprintf(outFilePtr, "%d %s %s\n", relocs[i]->offset, relocs[i]->opcode, relocs[i]->symbol);
    }
    
    fclose(outFilePtr);

    return(0);
}

// Returns non-zero if the line contains only whitespace.
static int lineIsBlank(char *line) {
    char whitespace[4] = {'\t', '\n', '\r', ' '};
    int nonempty_line = 0;
    for(int line_idx=0; line_idx < strlen(line); ++line_idx) {
        int line_char_is_whitespace = 0;
        for(int whitespace_idx = 0; whitespace_idx < 4; ++ whitespace_idx) {
            if(line[line_idx] == whitespace[whitespace_idx]) {
                line_char_is_whitespace = 1;
                break;
            }
        }
        if(!line_char_is_whitespace) {
            nonempty_line = 1;
            break;
        }
    }
    return !nonempty_line;
}

// Exits 2 if file contains an empty line anywhere other than at the end of the file.
// Note calling this function rewinds inFilePtr.
static void checkForBlankLinesInCode(FILE *inFilePtr) {
    char line[MAXLINELENGTH];
    int blank_line_encountered = 0;
    int address_of_blank_line = 0;
    rewind(inFilePtr);

    for(int address = 0; fgets(line, MAXLINELENGTH, inFilePtr) != NULL; ++address) {
        // Check for line too long
        if (strlen(line) >= MAXLINELENGTH-1) {
            printf("error: line too long\n");
            exit(1);
        }

        // Check for blank line.
        if(lineIsBlank(line)) {
            if(!blank_line_encountered) {
                blank_line_encountered = 1;
                address_of_blank_line = address;
            }
        } else {
            if(blank_line_encountered) {
                printf("Invalid Assembly: Empty line at address %d\n", address_of_blank_line);
                exit(2);
            }
        }
    }
    rewind(inFilePtr);
}


/*
* NOTE: The code defined below is not to be modifed as it is implimented correctly.
*/

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int
readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
    char *arg1, char *arg2)
{
    char line[MAXLINELENGTH];
    char *ptr = line;

    /* delete prior values */
    label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

    /* read the line from the assembly-language file */
    if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
	/* reached end of file */
        return(0);
    }

    /* check for line too long */
    if (strlen(line) == MAXLINELENGTH-1) {
	printf("error: line too long\n");
	exit(1);
    }

    // Ignore blank lines at the end of the file.
    if(lineIsBlank(line)) {
        return 0;
    }

    /* is there a label? */
    ptr = line;
    if (sscanf(ptr, "%[^\t\n ]", label)) {
	/* successfully read label; advance pointer over the label */
        ptr += strlen(label);
    }

    /*
     * Parse the rest of the line.  Would be nice to have real regular
     * expressions, but scanf will suffice.
     */
    sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]",
        opcode, arg0, arg1, arg2);

    return(1);
}

int twosComp(int posInt) {
    return (~posInt) + 1;
}

int createInstr(int opcode, int arg0, int arg1, int arg2) {
    int instruction = 0;
    
    //R-Type instructions(add, nor); opcode, field0, field1, field2
    if (opcode <= 1) {
        instruction = instruction | opcode;
        instruction = instruction<<3;
        instruction = instruction | arg0;
        instruction = instruction<<3;
        instruction = instruction | arg1;
        instruction = instruction<<16;
        instruction = instruction | arg2;
    }
    
    //I-Type instructions(lw, sw, beq); opcode, field0, field1, field2
    else if (opcode <= 4) {
        instruction = instruction | opcode;
        instruction = instruction<<3;
        instruction = instruction | arg0;
        instruction = instruction<<3;
        instruction = instruction | arg1;
        instruction = instruction<<16;
        
        if (arg2 < -32768 || arg2 > 32767) {
            printf("offset larger than 16 bits\n");
            exit(1);
        }
        
        if (arg2 < 0) {
            arg2 *= -1;
            arg2 = twosComp(arg2);
            int dummy = 0b1111111111111111;
            arg2 = arg2 & dummy;
        }
        instruction = instruction | arg2;
    }
    
    //J-Type instructions(jalr); opcode, field0, field1
    else if (opcode <= 5) {
        instruction = instruction | opcode;
        instruction = instruction<<3;
        instruction = instruction | arg0;
        instruction = instruction<<3;
        instruction = instruction | arg1;
        instruction = instruction<<16;
    }
    
    //O-Type instructions(halt, noop); opcode
    else if (opcode <= 7) {
        instruction = instruction | opcode;
        instruction = instruction<<22;
    }
        
    //.fill function
    else {
        return arg0;
    }
    
    return instruction;
}

static inline int
isNumber(char *string)
{
    int num;
    char c;
    return((sscanf(string, "%d%c",&num, &c)) == 1);
}

