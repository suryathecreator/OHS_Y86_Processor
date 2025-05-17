

char *interface() {
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
