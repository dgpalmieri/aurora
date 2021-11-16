CFLAGS = -Wall -g

libtest:
	$(CC) $(CFLAGS) libtest.cpp doctest.h ../err_msg.hpp ../err_msg.cpp ../octree.hpp ../octree.cpp ../atomics.h -o test
# Intel x64 version

#https://stackoverflow.com/a/28236905
#https://gcc.gnu.org/onlinedocs/gcc/Invoking-G_002b_002b.html
CC = g++ -Wall -Wextra -Werror -O2 -DSGI32
LIB = ../../Library

test_atomics: $(LIB)/atomics.hpp test_atomics.cpp
	$(CC) $(LFLAGS) -I$(LIB) -L$(LIB) \
	test_atomics.cpp -o test_atomics -lm

#render.o: $(LIB)/atomics.h render.h render.c
#	$(CC) $(CFLAGS) -I$(LIB) -c render.c

clean:
	rm -f test_atomics