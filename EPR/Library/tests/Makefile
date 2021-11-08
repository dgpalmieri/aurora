# Intel x64 version

#https://stackoverflow.com/a/28236905
#https://gcc.gnu.org/onlinedocs/gcc/Invoking-G_002b_002b.html
CC = g++ -std=c++17 -Wall -Wextra -Werror -O2 -DSGI32
LIB = ../../Library

test_atomics: $(LIB)/atomics.hpp test_atomics.cpp
	$(CC) $(LFLAGS) -I$(LIB) -L$(LIB) \
	test_atomics.cpp -o test_atomics -lm

test_raster: $(LIB)/raster.hpp test_raster.cpp
	$(CC) $(LFLAGS) -I$(LIB) -L$(LIB) \
	test_raster.cpp -o test_raster -lm

clean:
	rm -f test_atomics test_raster
