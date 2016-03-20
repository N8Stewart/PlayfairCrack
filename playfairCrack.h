// C89 does not have bool type. Create my own
#define bool int
#define true 1
#define false 0

// Max number of iterations and number of iterations per output checkpoint
#define MAX_ITERATIONS 100
#define ITERATION_CHECKPOINT 10

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
int validateText(char *text, int len);

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
void output(int iteration, double score, char *key, char *plaintext);

/*
 * Decipher ciphertext using key and put result in plaintext
 */
void decipher(char *key, char *ciphertext, char *plaintext, int len);

