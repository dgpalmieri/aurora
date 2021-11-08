# Intel x64 processor version
C++17 = g++ -std=c++17 -Wall -Wextra -Werror -O2 -DSGI32
CC = cc -O2 -DSGI32
AR = ar -r
ALL = noise.o stack.o octree.o raster.o rays.o timing.o view.o

libepr.a: $(ALL)
	$(AR) libepr.a $(ALL)
	ranlib libepr.a

malloc.o: malloc.c

noise.o: noise.h noise.c

raster.o: raster.h atomics.h raster.c
#comment out the line above
#this is ready for testing once we have C++ to use the .o files
#uncomment the two lines below
#raster.o: raster.hpp atomics.hpp raster.cpp
#	$(C++17) raster.cpp -c -o raster.o

rays.o: rays.h rays.c

view.o: rays.h view.h view.c

stack.o: stack.h rays.h stack.c

octree.o: octree.h octree.c

timing.o: timing.h timing.c

clean:
	rm -f libepr.a *.o

tar:
	tar -cvf libepr.tar Makefile *.c *.h *.cpp *.hpp
