/*
 * Written by Nate Stewart
 * Program to crack a playfair cipher using simulated annealing with quadgrams
 * 03/20/16
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "quadgram.h"
#include "playfairCrack.h"

extern float quadgram[];

int main() {
	char *m;
	printf("Input String: ");
	m = readCipher(stdin, INPUT_STEP_SIZE);

	if (validateText(m, strlen(m)) == 0)
		printf("%s\n", m);
	else
		printf("Invalid input.\n");
	free(m);
	return 0;
}

char * readCipher(FILE *fin, size_t size) {
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


int validateText(char *text, int len) {
	int i;
	for (i = 0; i < len; i++)
		if (text[i] < 'A' || text[i] > 'Z')
			return -1;

	return 0;
}

