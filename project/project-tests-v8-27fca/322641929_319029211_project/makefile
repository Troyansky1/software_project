CC = gcc
#CFLAGS = -ansi -Wall -Wextra -Werror -pedantic-errors 
CFLAGS = 
LDFLAGS = -lm

# Main target
symnmf: symnmf.o
	$(CC) -o symnmf symnmf.o $(LDFLAGS)

# Compile source file
symnmf.o: symnmf.c symnmf.h
	$(CC) -c symnmf.c $(CFLAGS)

# Clean build artifacts
clean:
	rm -f *.o symnmf

