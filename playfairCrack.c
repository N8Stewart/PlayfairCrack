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
#include <time.h>

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
	if (validateText(ciphertext, &messageLen) != 0) {
		free(ciphertext);
		return -1;
	}
	ciphertext = realloc(ciphertext, sizeof(*ciphertext) * (messageLen + 1));
	ciphertext[messageLen] = '\0';
	plaintext = calloc(messageLen + 1, sizeof(*plaintext));
	strcpy(plaintext, ciphertext);	
	
	// close the file as long as it is not stdin
	if (fin != stdin)
		fclose(fin);

	// Output relevant information for error checking
	printf("Attempting to crack the following ciphertext with key: %s\n", key);
	printf("%s\n", ciphertext);

	int iter = 0;
	double score = -DBL_MAX, maxScore = -DBL_MAX;
	srand(time(NULL)); // randomize seed
	// Run until max iteration met 
	while (iter < MAX_ITERATIONS) {
		iter++;
		score = simulatedAnnealing(key, ciphertext, plaintext, messageLen);
		if (score > maxScore) {
			maxScore = score;
			decipher(key, ciphertext, plaintext, messageLen);
			printf("\nPossible Plaintext found using key:\n");
			outputKey(key);
			printf("%s\n\n", plaintext);
		}
	}
	
	free(plaintext);
	free(ciphertext);
	return 0;
}

double simulatedAnnealing(char *key, char *ciphertext, char *plaintext, int messageLen) {
	int count, iter;
	float annealStep;
	char newKey[26], oldKey[26];
	double prob, delta, maxScore, score, bestScore;
	
	// Copy over key so we don't screw up our master copy. Decipher ciphertext using key and score it
	strcpy(oldKey,key);
	decipher(oldKey, ciphertext, plaintext, messageLen);
	maxScore = scoreText(plaintext,messageLen);
	bestScore = maxScore;
	iter = 0;

	// For each step, find our best key
	for (annealStep = ANNEALING_TEMP; annealStep >= 0; annealStep -= ANNEALING_STEP_SIZE) {
		for (count = 0; count < MAX_ITERATIONS; count++) { 
			strcpy(newKey, oldKey);
			alterKey(newKey);
			decipher(newKey, ciphertext, plaintext, messageLen);
			score = scoreText(plaintext, messageLen);
			// our difference between our current maxScore and step score
			delta = score - maxScore;
			// We did work in the positive direction (hopefully...)
			if (delta >= 0) {
				maxScore = score;
				strcpy(oldKey, newKey);
			} else if (annealStep > 0) {
				// the work we did is a side-grade 
				prob = exp(delta / annealStep);
				if (prob > 1.0 * rand() / RAND_MAX) {
					maxScore = score;
					strcpy(oldKey, newKey);				
				}
			}
			// This is our best score so far
			if (maxScore > bestScore){
				bestScore = maxScore;
				strcpy(key, oldKey);
				outputStats(iter, bestScore, key);
			} 
			iter++;
		}
	}
	
	return bestScore;
}

void keySwapRows(char *key, int r1, int r2) {
	int i;
	char temp;
	for (i = 0; i < 5; i++) {
		temp = key[r1 * 5 + i];
		key[r1 * 5 + i] = key[r2 * 5 + i];
		key[r2 * 5 + i] = temp;
	}
}

void keySwapCols(char *key, int c1, int c2) {
	int i;
	char temp;
	for (i = 0; i < 5; i++) {
		temp = key[i * 5 + c1];
		key[i * 5 + c1] = key[i * 5 + c2];
		key[i * 5 + c2] = temp;
	}
}

void keySwapChars(char *key, int i1, int i2) {	
	char temp = key[i1];
	key[i1] = key[i2];
	key[i2] = temp;
}

void keyShuffle(char *key, int num) {
	int i;
	for (i = 0; i < num; i++) {
		keySwapChars(key, rand() % 25, rand() % 25);
	}
}

void alterKey(char *key) {
	switch(rand() % 100) {
		case 1:
		case 2:
			keyShuffle(key, rand() % 26);
			break;
		case 3:
		case 4:
		case 5:
		case 6:
			keySwapRows(key, rand() % 5, rand() % 5);
			break;
		case 7:
		case 8:
		case 9:
		case 10:
			keySwapCols(key, rand() % 5, rand() % 5);
		default:
			keySwapChars(key, rand() % 25, rand() % 25);
			break;
	}
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

		if (c1_row == c2_row && c1_col == c2_col) { // Same character
			int row = c1_row - 1, col = c1_col - 1;
			if (row < 0)
				row += 5;
			if (col < 0)
				col += 5;
			int ind = 5 * row + col;
			plaintext[i] = ind, plaintext[i + 1] = ind;
		} else if (c1_row == c2_row) { // same row
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
}

void outputKey(char *key) {
	int i;
	for (i = 0; i < 25; i += 5) { 
		printf("%c %c %c %c %c\n", key[i + 0], key[i + 1], key[i + 2], key[i + 3], key[i + 4]);
	}
}

void outputStats(int iteration, double score, char *key) {
	printf("Iteration: %8d, \tbest score: %12.4lf, \tCurrent key: %s\n", iteration, score, key);
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
	char output[5];
	memset(output, '\0', sizeof(output));
	strncpy(output, text, 4);
	// Get a number associated with the index of each character
	index[0] = (*(text + 0) - 'A') * 17576;
	index[1] = (*(text + 1) - 'A') * 676;
	index[2] = (*(text + 2) - 'A') * 26;
	index[3] = (*(text + 3) - 'A') * 1;
	//int totalIndex = index[0] + index[1] + index[2] + index[3];
	//printf("Text: %s | ", output);
	//printf("total Index:%d | 0:%d | 1:%d | 2:%d | 3:%d\n",totalIndex, index[0], index[1], index[2], index[3]);
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

int validateText(char *input, int *len) {
	int i;
	// Declare an output array and initialize it to all 0's
	char *output = calloc(*len, sizeof(*output));
	int offset = 0;
	// Eliminate spaces and reformat case of text
	for (i = 0; i < *len; i++) {
		// Convert lower case to upper case where applicable
		if (input[i] >= 'a' && input[i] <= 'z') {
			output[i - offset] = input[i] - ' ';
		} else if (input[i] == ' ') {
			offset += 1;
		} else if (input[i] >= 'A' && input[i] <= 'Z') {
			output[i - offset] = input[i];
		} else {
			// Invalid character 
			printf("Invalid character: %c\n", input[i]);
			free(output);
			return -1;
		}
	}
	strcpy(input, output);
	free(output);
	*len -= offset; // Shrink length according to how many spaces were filtered out
	return 0;
}

