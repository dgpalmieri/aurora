# Intel x64 version
# this is the EPR/Library/tests Makefile
#https://stackoverflow.com/a/28236905
#https://gcc.gnu.org/onlinedocs/gcc/Invoking-G_002b_002b.html
#for debugging information, add -g to this next line or in a recipe, below
CC = g++ -std=c++17 -Wall -Wextra -O2 -DSGI32
LIB = ../../Library
INCQ = -iquote ../

libtest:
	$(CC) -g $(INCQ) libtest.cpp ../err_msg.cpp ../octree.cpp -o test

test_atomics: $(LIB)/atomics.hpp test_atomics.cpp
	$(CC) $(INCQ) -L$(LIB) \
	test_atomics.cpp -o test_atomics -lm

test_rays: 
	$(CC) $(INCQ) -L$(LIB) \
	test_rays.cpp ../rays.cpp -o test_rays -lm

test_noise: 
	$(CC) -g $(INCQ) -L$(LIB) \
	test_noise.cpp ../noise.cpp -o test_noise -lm

test_raster:
	$(CC) $(INCQ) -L$(LIB) \
	test_raster.cpp ../raster.cpp -o test_raster -lm

test_view:
	$(CC) $(INCQ) -L$(LIB) \
	$(LIB)/rays.cpp $(LIB)/view.cpp test_view.cpp  -o test_view -lm

clean:
	rm -f test_atomics test_rays test_noise test test_raster
