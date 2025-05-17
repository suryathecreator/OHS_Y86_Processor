/*
  * @authors Surya Duraivenkatesh, Josh Tittiranonda
  * @date: 05/2025
  * This is the processor code for the OHS Y86-64 (Sequential) Processor! With an input file of object code, you will receive a lively simulation of the object code running on the processor. Welcome to the world of OHS Y86!
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
// #include "helper.h" To clean up code.

#define RAX 0
#define RCX 1
#define RDX 2
#define RBX 3
#define RSP 4
#define RBP 5
#define RSI 6
#define RDI 7
#define R8  8
#define R9  9
#define R10 10
#define R11 11
#define R12 12
#define R13 13
#define R14 14
#define NOREG 15

typedef struct {
		int icode; // instruction name
		int ifun; //  type of instruction (e.g. add, sub, etc.)
		int rA; // reg a / one input field
		int rB; // reg b / one input field
		unsigned long valC; // never signed
		int valP; // pc with offset due to inst.

		long valA; // can be signed (reg contents)
		long valB; // can be signed (reg contents)
		long valE; // result of arithmetic, can be signed
		unsigned long valM; // memory, cannot be signed
} processorValues;

typedef struct {
		long registers[15];
		int PC;
		int currState; // location within hexFile
		bool ZF, SF, OF, cond; 
		unsigned long memory[65536]; // 64k memory
		char *hexFile; // File in a character array of hex values
		processorValues values;
} Processor;

char *file_parsing(char *);
int readNextHex(Processor *);
int hexToInt(char);
unsigned long readNextEightHex(Processor *);
processorValues fetch(Processor *);
processorValues decode(Processor *);
processorValues execute(Processor *);
processorValues memory(Processor *);
void writeback(Processor *);
void write(Processor *, long, long);
long read(Processor *, long);

int main() {
	int instructionCount = 1;
	
	// char *hexFile = file_parsing("test.yo"); // Loading into the file array. **UNCOMMENT AFTER TEST**
	char *hexFile = strdup("30f40001000030f500010000");  // To test // from tester file in canvas

	
	Processor ohsy86 = {0};
	ohsy86.hexFile = hexFile;
	ohsy86.PC = 0;
	ohsy86.currState = 0;

	// Testing fetch
	processorValues val1 = fetch(&ohsy86);
	printf("icode: %d\n", val1.icode);
	printf("ifun : %d\n", val1.ifun);
	printf("rA   : %d\n", val1.rA);
	printf("rB   : %d\n", val1.rB);
	printf("valC : %#lx\n", val1.valC);
	printf("valP : %d\n", val1.valP);

	processorValues val2 = fetch(&ohsy86);
	printf("icode: %d\n", val2.icode);
	printf("ifun : %d\n", val2.ifun);
	printf("rA   : %d\n", val2.rA);
	printf("rB   : %d\n", val2.rB);
	printf("valC : %#lx\n", val2.valC);
	printf("valP : %d\n", val2.valP);

	/*
	while (ohsy86.currState < strlen(hexFile)) {
	printf("Instruction %d\n", instructionCount++);
	processorValues ohsy86Values = fetch(&ohsy86);
	ohsy86.values = ohsy86Values;
		
	ohsy86.values = decode(&ohsy86);
	ohsy86.values = execute(&ohsy86);
	ohsy86.values = memory(&ohsy86);
	writeback(&ohsy86);
	ohsy86.PC += 1;
	}
	*/ // UNCOMMENT AFTER TEST

	free(hexFile);
	return 0;
}

/*
 * Function to parse the file into an array of lines.
 * @param: char* filename: file name, <= 50 characters
 */
char *file_parsing(char *filename) {
FILE *file = fopen(filename, "r");
	if (file == NULL) {
			printf("File open failed. Please contact the developer to fix a logical "
						 "error within the program."); // Debugging, adding error handling later
	}
	char *hexFile = malloc(1024*1024); // up to 1 mb file, in an array of characters

	hexFile[0] = '\0';  // empty string to start
	char line[1024];

	while (fgets(line, sizeof(line), file)) {
		line[strcspn(line, "\n")] = '\0';
		strcat(hexFile, line);
	}

	fclose(file);
	return hexFile;
}

int readNextHex(Processor *ohsy86) {
	char c = ohsy86->hexFile[ohsy86->currState++];

	// Error handling to be added
	return hexToInt(c);
}

unsigned long readNextEightHex(Processor *ohsy86) {
	unsigned long l;
	
	char temp[17] = {0}; // Temp. storage for 16 hex chars and null terminator.
		char reversed[17] = {0}; // Temp. storage for the reversed hex chars.

		strncpy(temp, ohsy86->hexFile + ohsy86->currState, 16);
		ohsy86->currState += 16;

		for (int i = 0; i < 16; i += 2) {
				reversed[14 - i] = temp[i];
				reversed[15 - i] = temp[i + 1];
		}

		l = strtoul(reversed, NULL, 16);
		return l;
}

/* memory stuffs */
void write(Processor *ohsy86, long index, long value) {
	ohsy86->memory[index] = value;
}

long read(Processor *ohsy86, long index) {
	return ohsy86->memory[index];
}

int hexToInt(char hex) {
	int i;
	if (hex >= '0' && hex <= '9') i = hex - '0';
	if (hex >= 'a' && hex <= 'f') i = hex - 'a' + 10;
	if (hex >= 'A' && hex <= 'F') i = hex - 'A' + 10;
	return i;
}

/*
Overall plan:

** Processor **
- Fetch
- Decode
- Execute
- Memory
- Writeback
- PC

** Commands to-do per thing ^ **
- halt
- nop
- rrmovl
- irmovl
- rmmovl
- mrmovl
- OPq
- jXX
- call
- ret
- pushq

** Computed Values (from slides) **
Fetch:
icode
ifun
rA
rB
valC
valP
--

Decode:
srcA
srcB
dstE
dstM
valA
valB

Execute:
valE
Cnd

Memory:
valM
*/



processorValues fetch(Processor *ohsy86) {
	int icode = readNextHex(ohsy86);
	int ifun = readNextHex(ohsy86);
	int rA, rB, valP;
	long valC;

	// initialized to these default values for cases like halt and nop
	rA = NOREG;
	rB = NOREG;
	valP = 0;
	valC = 0;

	switch (icode) {
		case 0: // Halt.
			printf("OHS Y86-64 Processor halted, as you desired! Thank you for using the OHS Y86-64 Processor!"); // Add more GUI?
			exit(0);
		case 1: // instruction nop
			break;
		case 2: // instruction rrmovl
			rA = readNextHex(ohsy86);
			rB = readNextHex(ohsy86);
			valP = ohsy86->PC + 2;
			ohsy86->currState += 4;
			break;
		case 3: // instruction irmovl
			rA = readNextHex(ohsy86); // Stores immediate value
			rB = readNextHex(ohsy86);
			valC = readNextEightHex(ohsy86);
			valP = ohsy86->PC + 10;
			break;
		case 4: // instruction rmmovl
			rA = readNextHex(ohsy86);
			rB = readNextHex(ohsy86);
			valC = readNextEightHex(ohsy86);
			valP = ohsy86->PC + 10;
			break;
		case 5: // instruction mrmovl
			rA = readNextHex(ohsy86);
			rB = readNextHex(ohsy86);
			valC = readNextEightHex(ohsy86);
			valP = ohsy86->PC + 10;
			break;
		case 6: // instruction OPq
			rA = readNextHex(ohsy86);
			rB = readNextHex(ohsy86);
			valP = ohsy86->PC + 2;
			break;
		case 7: // instruction jXX
			valC = readNextEightHex(ohsy86);
			valP = ohsy86->PC + 9;
			break;
		case 8: // instruction call	
			valC = readNextEightHex(ohsy86);
			valP = ohsy86->PC + 9;
			break;
		case 9: // instruction ret
			valP = ohsy86->PC + 1;
			break;
		case 10: // instruction pushq
			rA = readNextHex(ohsy86);
			rB = readNextHex(ohsy86);
			valP = ohsy86->PC + 2;
			break;
		case 11: // instruction popq
			rA = readNextHex(ohsy86);
			rB = readNextHex(ohsy86);
			valP = ohsy86->PC + 2;
			break;
		default:
			printf("Error: Invalid instruction code. Please contact the developer to fix a logical error within the program."); // Debugging, adding error handling later
	}
	processorValues values = {
			.icode = icode,
			.ifun = ifun,
			.rA = rA,
			.rB = rB,
			.valC = valC,
			.valP = valP
	};
	return values;
}

processorValues decode(Processor *ohsy86) {
	processorValues values = ohsy86->values;

	// copying values into local vars
	int icode = values.icode;
	int ifun = values.ifun;
	int rA = values.rA;
	int rB = values.rB;
	unsigned long valC = values.valC;
	int valP = values.valP;

	long valA = 0;
	long valB = 0;
	long valE = 0; // will be calculated in execute

	switch (icode) {
		case 0:
			break; // halt
		case 1:
			break; // nop
		case 2: // rrmovl
			valA = ohsy86->registers[rA];
			break; // 
// Finish up w/ the canvas guide
		
	}

	// updating ohsy86
	values.valA = valA;
	values.valB = valB;
	values.valE = valE; // zero for now, calculated in execute

	return values;
}



