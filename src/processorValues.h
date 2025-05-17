/*
 * @authors Surya Duraivenkatesh, Josh Tittiranonda
 * @date: 05/2025
 * Description.
 */

#ifndef PROCESSORVALUES_H
#define PROCESSORVALUES_H

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

#endif