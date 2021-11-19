# Intel x64 version
# this is the EPR/Library/tests Makefile
#https://stackoverflow.com/a/28236905
#https://gcc.gnu.org/onlinedocs/gcc/Invoking-G_002b_002b.html

#for debugging information, add -g to this next line or in a recipe, below
CC = g++ -std=c++17 -Wall -Wextra -O2 -DSGI32
LIB = ../../Library
INCQ = -iquote ../


test_atomics:
	$(CC) $(INCQ) -L$(LIB) \
	$(LIB)/atomics.cpp test_atomics.cpp -o test_atomics -lm

test_noise: 
	$(CC) -g $(INCQ) -L$(LIB) \
	test_noise.cpp $(LIB)/noise.cpp -o test_noise -lm

test_octree: #TODO change libtest.cpp to test_octree.cpp
	$(CC) -g $(INCQ) -L$(LIB) \
  $(LIB)/err_msg.cpp $(LIB)/octree.cpp libtest.cpp -o test_octree
  
test_raster:
	$(CC) $(INCQ) -L$(LIB) \
	test_raster.cpp $(LIB)/raster.cpp -o test_raster -lm

test_rays: 
	$(CC) $(INCQ) -L$(LIB) \
	test_rays.cpp $(LIB)/rays.cpp -o test_rays -lm
  
 test_stack:
	$(CC) -g $(INCQ) -L$(LIB) \
	$(LIB)/octree.c $(LIB)/stack.cpp test_stack.cpp -o test_stack -lm
  
clean:
	find . -maxdepth 1 -name "test_*" -not -name "*\.cpp" -print -delete
