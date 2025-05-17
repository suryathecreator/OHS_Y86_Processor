/*
  * @authors Surya Duraivenkatesh, Josh Tittiranonda
  * @date: 05/2025
  * This is the GUI for the OHS Y86-64 (Sequential) Processor! With an input file of object code, you will receive a lively simulation of the object code running on the processor. Welcome to the world of OHS Y86!
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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



char *interface();
char *file_parsing(char *filename);

long registers[15];

int main() {
	// char *hexFile = file_parsing("test.yo");
}

/*
 * GUI/Function for the user to input file name of the file they want to assemble.
 * @return: char* filename: file name, <= 50 characters
 */
char *interface() { // Add more GUI.
  char *str = malloc(50);
  printf("%s\n", "Welcome to the OHS Y86-64 Emulator! This program will help "
                 "convert your assembly instructions into machine code. Please "
                 "enter the file you'd like to assemble: ");
  int successfulScan = scanf(
      "%49s",
      str); // I'll assume the file name won't exceed 50 characters. Note, scanf
            // returns a value indicating the number of successful elements
            // scanned, so I am saving it to silence the warning easily.
  return str;
}

