CC = gcc
CFLAGS = -ansi -Wall -Wextra -Werror -pedantic-errors -lm

symnmf: symnmf.o aux.o symnmf.h 
    $(CC) -o symnmf symnmf.o aux.o $(CFLAGS)

symnmf.o: symnmf.c
    $(CC) -c symnmf.c  $(CFLAGS)

aux.o: aux.c
    $(CC) -c aux.c $(CFLAGS)

clean:
    rm -f *.o

