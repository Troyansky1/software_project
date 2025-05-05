CC = gcc
CFLAGS = -ansi -Wall -Wextra -Werror -pedantic-errors 
LDFLAGS = -lm  # link the math library

# Target to build the final executable 'symnmf' from object files
symnmf: symnmf.o lin_alg_utils.o utils.o
# $@ = target name, $^ = dependencies
	$(CC) -o $@ $^ $(LDFLAGS) 

# Compile .c files into .o object files with .h as a dependency for recompilation
%.o: %.c %.h
	$(CC) -c $^ $(CFLAGS)

# Clean build artifacts
clean:
	rm -f *.o symnmf

