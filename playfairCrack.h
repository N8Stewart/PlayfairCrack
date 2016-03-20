

/*
 * Read the cipher from the file into an array. Return a pointer to that array.
 * Step size determines how often the array must be reallocated 
 */
#define INPUT_STEP_SIZE 32
char *readCipher(FILE *fin, size_t size);

