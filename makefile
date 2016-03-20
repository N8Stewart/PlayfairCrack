COMMON_FILES     := playfairCrack.h quadgram.h
COMMON_ARGUMENTS := -Wall -Werror -lm -g
OPTIMIZATION     := -O3
CRACK_FILE       := playfairCrack.c
CRACK_BINARY     := crack

all: playfair

playfair: $(COMMON_FILES) $(CRACK_FILE)
	gcc $(COMMON_ARGUMENTS) $(OPTIMIZATION) $(CRACK_FILE) -o $(CRACK_BINARY)

clean:
	rm -f *.out *.o $(CRACK_BINARY)
