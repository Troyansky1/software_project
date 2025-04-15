CC = gcc
CFLAGS = -ansi -Wall -Wextra -Werror -pedantic-errors 
LDFLAGS = -lm

# Compile ad link source file
symnmf.o: symnmf.c symnmf.h
	$(CC) -o symnmf symnmf.c $(CFLAGS) $(LDFLAGS)

# Clean build artifacts
clean:
	rm -f *.o symnmf

