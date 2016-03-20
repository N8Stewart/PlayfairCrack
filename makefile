COMMON_FILES := playfairCrack.h
CRACK_FILE   := playfairCrack.c
CRACK_BINARY := crack

all: playfair

playfair: $(COMMON_FILES) $(CRACK_FILE) quadgram.o
	gcc -g $(CRACK_FILE) -o $(CRACK_BINARY)

quadgram.o: quadgram.h
	gcc quadgram.h -c -o quadgram.o

clean:
	rm -f *.out $(CRACK_BINARY)
