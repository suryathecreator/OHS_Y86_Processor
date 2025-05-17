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
bool cond(Processor *, int);

int main() {
	int instructionCount = 1;
	
	// char *hexFile = file_parsing("test.yo"); // Loading into the file array. **UNCOMMENT AFTER TEST**
	char *hexFile = strdup("30f40001000030f500010000");  // To test // from tester file in canvas
	printf("%s\n", hexFile);
	
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
		
	ohsy86.values = decode(&ohsy86); // For debugging, can get rid of assignment later (as it modifies the real thing)
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
	
		char temp[9] = {0}; // Temp. storage for 16 hex chars and null terminator.
		char reversed[9] = {0}; // Temp. storage for the reversed hex chars.

		strncpy(temp, ohsy86->hexFile + ohsy86->currState, 8);
		ohsy86->currState += 8;

		for (int i = 0; i < 8; i += 2) {
				reversed[6 - i] = temp[i];
				reversed[7 - i] = temp[i + 1];
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
- Helper fn: TESTED AND WORKS
- File: TESTED AND WORKS (note, input is line-by-line "y86" / regular object code)
30f400010000
30f500010000
8024000000
etc.
after parsing, becomes 30f40001000030f5000100008024000000
(because instruction bit tells how many need to be read, this simplifies a lot)

- Fetch: TESTED AND WORKS
- Decode: Started
- Execute: To-do
- Memory: To-do
- Writeback: To-do
- PC: To-do
optional -- gui

** Commands to-do per thing ^ **
halt - nop - rrmovl - irmovl - rmmovl - mrmovl - OPq - jXX - call - ret - pushq

** Computed Values (from slides) **
Fetch:
icode - ifun - rA - rB - valC - valP
--

Decode:
srcA - srcB - dstE - dstM - valA - valB

Execute:
valE - Cnd

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
			valB = 0;
			break; // 
		case 3: // irmovl
			break;
		case 4: // rmmovl
			valA = ohsy86->registers[rA];
			valB = ohsy86->registers[rB];
			break;
		case 5: // mrmovl
			valB = ohsy86->registers[rB];
			break;
		case 6: // OPq
			valA = ohsy86->registers[rA];
			valB = ohsy86->registers[rB];
			break;
		case 7: // jXX
			break;
		case 8: // call
			valB = ohsy86->registers[RSP];
			break;
		case 9: // ret
			valB = ohsy86->registers[RSP];
			break;
		case 10: // pushq
			valA = ohsy86->registers[rA];
			valB = ohsy86->registers[RSP];
			break;
		case 11: // popq
			valA = ohsy86->registers[RSP];
			valB = ohsy86->registers[RSP];
			break;
		default:
			printf("Error: Invalid instruction code. Please contact the developer to fix a logical error within the program."); // Debugging, adding error handling later

	// updating ohsy86
	values.valA = valA;
	values.valB = valB;
	values.valE = valE; // zero for now, calculated in execute

	return values;
	}
}

processorValues execute(Processor *ohsy86) {
	processorValues values = ohsy86->values;
	int icode = values.icode;
	int ifun = values.ifun;
	int rA = values.rA;
	int rB = values.rB;
	unsigned long valC = values.valC;
	int valP = values.valP;
	long valA = values.valA;
	long valB = values.valB;
	
	long valE = 0;
	long valEBefore;

	switch (icode) {
		case 0: // halt
			break;
		case 1: // nop
			break;
		case 2: // rrmovl
			valE = valB + valC;
			ohsy86->cond = cond(ohsy86, ifun);
			if (!ohsy86->cond) {
				rB = NOREG;
			}
			break;
		case 3: // irmovl
			valE = valC;
			break;
		case 4: // rmmovl
			valE = valB + valC;
			break;
		case 5: // mrmovl
			valE = valB + valC;
			break;
		case 6: // OPq
			valEBefore = valA;
			switch (ifun) { // Setting CC
				case 0: // addq
					valE = valA + valB;
					if ((valA > 0) && (valB > 0) && (valE < 0)) 
						ohsy86->OF = true;
					else
						ohsy86->OF = false;
					if ((valA < 0) && (valB < 0) && (valE > 0)) {
						ohsy86->OF = true;
					}
					break;
				case 1: // subq
					valE = valA - valB;
					if ((valA > 0) && (valB < 0) && (valE < 0))
						ohsy86->OF = true;
					else
						ohsy86->OF = false;
					break;
				case 2: // andq
					valE = valA & valB;
					break;
				case 3: // xorq
					valE = valA ^ valB;
					break;
			}
			if (valE == 0) {
				ohsy86->ZF = true;
			}
			else {
				ohsy86->ZF = false;
			}
			if ((valEBefore < 0) && (valE > 0)) {
				ohsy86->SF = true;
			}
			if ((valEBefore > 0) && (valE < 0)) {
				ohsy86->SF = true;
			}
			else {
				ohsy86->SF = false;
			}
			break;
			case 7: // jXX
				ohsy86->cond = cond(ohsy86, ifun);
				break;
			case 8: // call
				valE = valB - 8;
				break;
			case 9: // ret
				valE = valB + 8;
				break;
			case 10: // pushq
				valE = valB - 8;
				break;
			case 11: // popq
				valE = valB + 8;
				break;
			default:
				printf("Error: Invalid instruction code. Please contact the developer to fix a logical error within the program."); // Debugging, adding error handling later
	}	
	// updating ohsy86
	values.valE = valE;
	return values;
	
}

processorValues memory(Processor *ohsy86) {
	processorValues values = ohsy86->values;
	int icode = values.icode;
	int ifun = values.ifun;
	int rA = values.rA;
	int rB = values.rB;
	unsigned long valC = values.valC;
	int valP = values.valP;
	long valA = values.valA;
	long valB = values.valB;
	long valE = values.valE;
	
	long valM = 0;

	switch (icode) {
		case 0: // halt
			break;
		case 1: // nop
			break;
		case 2: // rrmovl
			break;
		case 3: // irmovl
			break;
		case 4: // rmmovl
			valM = read(ohsy86, valE);
			break;
		case 5: // mrmovl
			valM = read(ohsy86, valE);
			break;
		case 6: // OPq
			break;
		case 7: // jXX
			break;
		case 8: // call
			write(ohsy86, valE, valP);
		case 9: // ret
			valM = read(ohsy86, valA);
			break;
		case 10: // pushq
			write(ohsy86, valE, valA);
			break;
		case 11: // popq
			valM = read(ohsy86, valA);
			break;
		default:
			printf("Error: Invalid instruction code. Please contact the developer to fix a logical error within the program."); // Debugging, adding error handling later
	}
		values.valM = valM;
		return values;
}

processorValues writeBack(Processor *ohsy86) {
	processorValues values = ohsy86->values;
	int icode = values.icode;
	int ifun = values.ifun;
	int rA = values.rA;
	int rB = values.rB;
	unsigned long valC = values.valC;
	int valP = values.valP;
	long valA = values.valA;
	long valB = values.valB;
	long valE = values.valE;
	long valM = values.valM;

	switch (icode) {
		case 0: // halt
			break;
		case 1: // nop
			break;
		case 2: // rrmovl
			ohsy86->registers[rB] = valE;
			break;
		case 3: // irmovl
			ohsy86->registers[rB] = valE;
			break;
		case 4: // rmmovl
			break;
		case 5: // mrmovl
			ohsy86->registers[rA] = valM;
			break;
		case 6: // OPq
			ohsy86->registers[rB] = valE;
			break;
		case 7: // jXX
			break;
		case 8: // call
			ohsy86->registers[RSP] = valE;
			break;
		case 9: // ret
			ohsy86->registers[RSP] = valE;
			break;
		case 10: // pushq
			ohsy86->registers[RSP] = valE;
			break;
		case 11: // popq
			ohsy86->registers[RSP] = valE;
			ohsy86->registers[rA] = valM;
			break;
		default:
			printf("Error: Invalid instruction code. Please contact the developer to fix a logical error within the program.");
			break;
	}
	return values;
}

processorValues PC(Processor *ohsy86) {
	processorValues values = ohsy86->values;
	int icode = values.icode;
	int ifun = values.ifun;
	int rA = values.rA;
	int rB = values.rB;
	unsigned long valC = values.valC;
	int valP = values.valP;
	long valA = values.valA;
	long valB = values.valB;
	long valE = values.valE;
	long valM = values.valM;

	switch (icode) {
		case 0: // halt
			break;
		case 1: // nop
			break;
		case 2: // rrmovl
			ohsy86->PC = valP;
			break;
		case 3: // irmovl
			ohsy86->PC = valP;
			break;
		case 4: // rmmovl
			ohsy86->PC = valP;
			break;
		case 5: // mrmovl
			ohsy86->PC = valP;
			break;
		case 6: // OPq
			ohsy86->PC = valP;
			break;
		case 7: // jXX
			if (ohsy86->cond == true) {
				ohsy86->PC = valC;
			}
			else {
				ohsy86->PC = valP;
			}
			break;
		case 8: // call
			ohsy86->PC = valC;
			break;
		case 9: // ret
			ohsy86->PC = valM;
			break;
		case 10: // pushq
			ohsy86->PC = valP;
			break;
		case 11: // popq
			ohsy86->PC = valP;
			break;
		default:
			printf("Error: Invalid instruction code. Please contact the developer to fix a logical error within the program.");
			break;
	}
	return values;
}

bool cond(Processor *ohsy86, int ifun) {
	switch (ifun) {
		case 0: // jmp
			return true;
		case 1: // jle
			if (ohsy86->SF != ohsy86->OF) {
				return true;
			}
			if (ohsy86->ZF == true) {
				return true;
			}
			else {
				return false;
			}
		case 2: // jl
			if (ohsy86->SF != ohsy86->OF) {
				return true;
			}
			else {
				return false;
			}
		case 3: // je
			if (ohsy86->ZF == true) {
				return true;
			}
			else {
				return false;
			}
		case 4: // jne
			if (ohsy86->ZF == false) {
				return true;
			}
			else {
				return false;
			}
		case 5: // jge
			if (ohsy86->SF == ohsy86->OF) {
				return true;
			}
			if (ohsy86->ZF == true) {
				return true;
			}
			else {
				return false;
			}
		case 6: // jg
			if (ohsy86->SF == ohsy86->OF) {
				return true;
			}
		default:
			printf("Error: Invalid instruction code. Please contact the developer to fix a logical error within the program.");
			return false; // to silence warning
	}
}	