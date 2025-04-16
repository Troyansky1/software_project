CC = gcc
CFLAGS = -ansi -Wall -Wextra -Werror -pedantic-errors 
LDFLAGS = -lm


symnmf: symnmf.o lin_alg_utils.o utils.o
# call gcc, ouput file is symnmf with all sources (ma)
	$(CC) -o $@ $^ $(LDFLAGS)

# Compile and link source file
%.o: %.c %.h
	$(CC) -c $^ $(CFLAGS)



# Clean build artifacts
clean:
	rm -f *.o symnmf

