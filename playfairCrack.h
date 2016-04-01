// C89 does not have bool type. Create my own
#define bool int
#define true 1
#define false 0

// Max number of iterations and number of iterations per output checkpoint
#define MAX_ITERATIONS 10000
#define ANNEALING_STEP_SIZE 0.5
#define ANNEALING_TEMP 50
/*
 * Score a quadgram using the quadgrams stored in quadgrams.h
 */
double scoreQuadgram(char *text);

/*
 * Score the text
 */
double scoreText(char *text, int len);

/*
 * Ensure the text is valid. All uppercase letters and no punctuation or spaces
 * Returns 0 if the text is valid, -1 otherwise
 */
int validateText(char *text, int *len);

/*
 * Read the cipher from the file into an array. Return a pointer to that array.
 * Step size determines how often the array must be reallocated 
 */
#define INPUT_STEP_SIZE 32
char *readCipher(FILE *fin, size_t size);

/*
 * Remove the letter, 'letter', from the ciphertext, 'cipher'
 * Return true if successful, false if the letter was not found
 */
bool removeLetter(char *cipher, char letter);

/*
 * Output relevant information to the screen
 */
void outputStats(int iteration, double score, char *key);

/*
 * Output the key in pretty form
 */
void outputKey(char *key);

/*
 * Decipher ciphertext using key and put result in plaintext
 */
void decipher(char *key, char *ciphertext, char *plaintext, int len);

/*
 * Control modification of the key. Randomly choose an operation to complete.
 * Operations include: 
 * shuffle      - 2% chance
 * swap columns - 4% chance
 * swap rows    - 4% chance
 * swap chars   - 90% chance
 */
void alterKey(char *key);

/* 
 * Modify the key by switching two random letters, i1 and i2.
 */
void keySwapChars(char *key, int i1, int i2);

/*
 * Modify the key by switching two entire rows, r1 and r2
 */
void keySwapRows(char *key, int r1, int r2);

/*
 * Modify the key by switching two entire columns, c1 and c2
 */
void keySwapRows(char *key, int r1, int r2);

/*
 * Modify the key by randomly switching num characters
 */
void keyShuffle(char *key, int num);

/*
 * A simulated annealing algorithm to determine the best key
 */
double simulatedAnnealing(char *key, char *ciphertext, char *plaintext, int messageLen);

