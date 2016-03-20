COMMON_FILES     := playfairCrack.h
COMMON_ARGUMENTS := -Wall -Werror -g
OPTIMIZATION     := -O0
CRACK_FILE       := playfairCrack.c
CRACK_BINARY     := crack

all: playfair

playfair: $(COMMON_FILES) $(CRACK_FILE) quadgram.o
	gcc $(COMMON_ARGUMENTS) $(OPTIMIZATION) $(CRACK_FILE) -o $(CRACK_BINARY)

quadgram.o: quadgram.h
	gcc quadgram.h -c -o quadgram.o

clean:
	rm -f *.out *.o $(CRACK_BINARY)
