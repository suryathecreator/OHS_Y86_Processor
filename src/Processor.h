/*
 * @authors Surya Duraivenkatesh, Josh Tittiranonda
 * @date: 05/2025
 * Description.
 */

#ifndef PROCESSOR_H
#define PROCESSOR_H
#include "processorValues.h"

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
		long registers[15];
		int PC;
		int currState; // location within hexFile
		bool ZF, SF, OF, cond; 
		long memory[65536]; // 64k memory
		char *hexFile; // File in a character array of hex values
		processorValues values;
} Processor;

#endif