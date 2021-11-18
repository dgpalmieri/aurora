# Intel x64 version

#https://stackoverflow.com/a/28236905
#https://gcc.gnu.org/onlinedocs/gcc/Invoking-G_002b_002b.html
CC = g++ -Wall -Wextra -Werror -O2 -DSGI32
LIB = ../../Library

test_atomics: $(LIB)/atomics.hpp test_atomics.cpp
	$(CC) $(LFLAGS) -I$(LIB) -L$(LIB) \
	test_atomics.cpp -o test_atomics -lm

test_stack: $(LIB)/stack.hpp test_stack.cpp
	$(CC) $(LFLAGS) -I$(LIB) -L$(LIB) \
	test_stack.cpp -o test_stack -lm

#render.o: $(LIB)/atomics.h render.h render.c
#	$(CC) $(CFLAGS) -I$(LIB) -c render.c

clean:
	find . -maxdepth 1 -name "test_*" -not -name "*\.cpp"
	find . -maxdepth 1 -name "test_*" -not -name "*\.cpp" -delete
