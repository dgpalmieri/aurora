# Intel x64 version
# this is the EPR/Library/tests Makefile

#https://stackoverflow.com/a/28236905
#https://gcc.gnu.org/onlinedocs/gcc/Invoking-G_002b_002b.html
#if you want debugging information, add -g to this next line
CC = g++ -std=c++17 -Wall -Wextra -Werror -O2 -DSGI32
LIB = ../../Library
INCQ = ../

test_atomics: $(LIB)/atomics.hpp test_atomics.cpp
	$(CC) $(LFLAGS) -iquote $(INCQ) -L$(LIB) \
	test_atomics.cpp -o test_atomics -lm

test_rays: 
	$(CC) $(LFLAGS) -iquote $(INCQ) -L$(LIB) \
	test_rays.cpp ../rays.cpp -o test_rays -lm

#render.o: $(LIB)/atomics.h render.h render.c
#	$(CC) $(CFLAGS) -I$(LIB) -c render.c

clean:
	rm -f test_atomics test_rays
