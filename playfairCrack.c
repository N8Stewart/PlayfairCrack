/*
 * Written by Nate Stewart
 * Program to crack a playfair cipher using simulated annealing with quadgrams
 * 03/20/16
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>

#include "quadgram.h"
#include "playfairCrack.h"

extern float quadgram[];

int main(int argc, char **argv) {

	char key[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	FILE *fin = stdin;

	/*
	 * Get command line arguments. Setup key and input location
	 */
	switch (argc) {
		case 2:
			fin = fopen(argv[1], "r");
			if (fin == NULL) {
				printf("Invalid file: %s. Unable to open for reading.\n", argv[3]);
				return -1;
			}
		case 1:
			if (!removeLetter(key, 'J')) {
				printf("Could not remove letter J from cipher key.\n");
				return -1;
			}
			break;
		case 4:
			fin = fopen(argv[3], "r");
			if (fin == NULL) {
				printf("Invalid file: %s. Unable to open for reading.\n", argv[3]);
				return -1;
			}
		case 3:
			if (strcmp(argv[1], "-r")) {
				printf("Optional parameter '-r' not found. '%s' found instead.\n", argv[1]);
				return -1;
			}
			if(!removeLetter(key, argv[2][0])) {
				printf("Could not remove letter %c from cipher key.\n", argv[2][0]);
				return -1;
			}
			break;
		default:
			printf("Invalid usage. See below for proper usage.\n");
			printf("\t./%s [ -r <character_to_remove> ] [ ciphertext_filepath ]\n", argv[0]);
			return -1;
	}

	/*
	 * Input cipher and ensure it is valid
	 */
	char *ciphertext, *plaintext;
	int messageLen;
	ciphertext = readCipher(fin, INPUT_STEP_SIZE);
	messageLen = strlen(ciphertext);
	if (validateText(ciphertext, messageLen) != 0)
		printf("Invalid input.\n");
	plaintext = malloc(sizeof(*plaintext) * (messageLen + 1));
	// close the file as long as it is not stdin
	if (fin != stdin)
		fclose(fin);

	// Output relevant information for error checking
	printf("Attempting to crack the following ciphertext with key: %s\n", key);
	printf("%s\n", ciphertext);

	int iter = 0;
	double score;//, max = -DBL_MAX;
	// Run until max iteration met 
	while (iter < MAX_ITERATIONS) {
		iter++;
		// Compute new key
		score = scoreText(ciphertext, messageLen);
		decipher(key, ciphertext, plaintext, messageLen);
		output(iter, score, key, plaintext);
	}
	
	free(plaintext);
	free(ciphertext);
	return 0;
}

void decipher(char *key, char *ciphertext, char *plaintext, int len) {
	int i;
	// index, row and column of the current digram
	char c1, c2;
	int c1_ind, c1_row, c1_col, c2_ind, c2_row, c2_col;

	for (i = 0; i < len; i += 2) {
		c1 = ciphertext[i], c2 = ciphertext[i + 1];
		// strchr returns a pointer to the first index of character in key. subtract key from pointer to get index
		c1_ind = (int)(strchr(key, c1) - key), c2_ind = (int)(strchr(key, c2) - key);
		// Rows have offset 5, columns are mod 5
		c1_row = c1_ind / 5, c2_row = c2_ind / 5;
		c1_col = c1_ind % 5, c2_col = c2_ind % 5;

		if (c1_row == c2_row) { // same row
			// Determine if wrapping occurred
			if (c1_col == 0) {
				plaintext[i] = key[c1_ind + 4];
				plaintext[i+1] = key[c2_ind - 1];
			} else if (c2_col == 0 ) {
				plaintext[i] = key[c1_ind - 1];
				plaintext[i+1] = key[c2_ind + 4];
			} else {
				plaintext[i] = key[c1_ind - 1];
				plaintext[i+1] = key[c2_ind - 1];
			}
		} else if (c1_col == c2_col ) { // same column
			if (c1_row == 0) {
				plaintext[i] = key[c1_ind + 20];
				plaintext[i+1] = key[c2_ind - 5];
			} else if (c2_row == 0) {
				plaintext[i] = key[c1_ind - 5];
				plaintext[i+1] = key[c2_ind + 20];
			} else {
				plaintext[i] = key[c1_ind - 5];
				plaintext[i+1] = key[c2_ind - 5];
			}
		} else { // rectangle rule
			plaintext[i] = key[5 * c1_row + c2_col];
			plaintext[i+1] = key[5 * c2_row + c1_col];
		}
	}

	// Add null terminator since it was skipped
	plaintext[i] = '\0';
}

void output(int iteration, double score, char *key, char *plaintext) {
	printf("Iteration: %8d, \tbest score: %12.4lf, \tCurrent key: %s\n", iteration, score, key);
	printf("%s\n", plaintext);
}

bool removeLetter(char *cipher, char letter) {
	char *src, *dst;
	if (letter >= 'a' && letter <= 'z') {
		letter -= ' '; // make it capital letter 
	}
	bool found; // if the letter was found or not
	for (src = dst = cipher; *src != '\0'; src++) {
		*dst = *src;
		if (*dst != letter) 
			dst++;
		else
			found = true;
	}
	*dst = '\0';
	return found;
}

char *readCipher(FILE *fin, size_t size) {
	char *str;
	int currChar;
	size_t len = 0;
	str = malloc(sizeof(*str) * size); // Initially allocate str with provided size
	if (!str) return str; // If provided size is 0, or unable to allocate memory, return empty string
	
	while (EOF != (currChar = fgetc(fin)) && currChar != '\n') {
		str[len++] = currChar;
		if (len == size) {
			str = realloc(str, sizeof(*str) * (size += INPUT_STEP_SIZE));
			if (!str) return str; // If unable to realloc, return what we have so far
		}
	}

	// Add null terminator onto end of string
	str = realloc(str, sizeof(*str) * (++len));
	str[len - 1] = '\0';
	return str;
}

double scoreQuadgram(char *text) {
	int index[4];
	// Get a number associated with the index of each character
	index[0] = (*(text + 0) - 'A') * 17576;
	index[1] = (*(text + 1) - 'A') * 676;
	index[2] = (*(text + 2) - 'A') * 26;
	index[3] = (*(text + 3) - 'A') * 1;
	return quadgram[index[0] + index[1] + index[2] + index[3]];
}

double scoreText(char *text, int len) {
	int i;
	double score = 0.0;
	// Calculate all quadgrams in the text
	for (i = 0; i < len - 3; i++) {
		score += scoreQuadgram(text + i);
	}

	return score;
}

int validateText(char *text, int len) {
	int i;
	for (i = 0; i < len; i++)
		if (text[i] < 'A' || text[i] > 'Z')
			return -1;

	return 0;
}

