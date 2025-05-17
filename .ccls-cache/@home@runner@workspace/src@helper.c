/*
* @authors Surya Duraivenkatesh, Josh Tittiranonda
* @date: 05/2025
* Description.
*/

// Add more things that help clean the code up, then uncomment helper line.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "Processor.h"
#include "processorValues.h"

void printProcessor(Processor ohsy86);
void printProcessorValues(processorValues values);

void printProcessor(Processor ohsy86) {
	printf("Registers: \n");
	for (int i = 0; i < 15; i++) {
		printf("Register %d: %ld\n", i, ohsy86.registers[i]);
	}
	printf("PC: %d\n", ohsy86.PC);
	printf("currState: %d\n", ohsy86.currState);
	printf("ZF: %d\n", ohsy86.ZF);
	printf("SF: %d\n", ohsy86.SF);
	printf("OF: %d\n", ohsy86.OF);
	printf("cond: %d\n", ohsy86.cond);
	printf("memory: \n");
	for (int i = 0; i < 100; i++) {
		printf("(->100)Memory %d: %#lx\n", i, ohsy86.memory[i]);
	}
	printf("hexFile: %s\n", ohsy86.hexFile);
	printProcessorValues(ohsy86.values);
}

void printProcessorValues(processorValues values) {
	printf("values: \n");
	printf("icode: %d\n", values.icode);
	printf("ifun: %d\n", values.ifun);
	printf("rA: %d\n", values.rA);
	printf("rB: %d\n", values.rB);
	printf("valC: %#lx\n", values.valC);
	printf("valP: %d\n", values.valP);
	printf("valA: %ld\n", values.valA);
	printf("valB: %ld\n", values.valB);
	printf("valE: %ld\n", values.valE);
	printf("valM: %ld\n", values.valM);
}