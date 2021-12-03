# Intel x64 version
# CC = gcc -g -DSGI32
#CC = cc -O2 -DSGI32
#to add debugging information, include -g below
CC = g++ -std=c++17 -Wall -Wextra -O2 -DSGI32 -nostartfiles -c
AR = ar -r
#ALL = noise.o stack.o octree.o raster.o rays.o timing.o view.o

libepr.a: $(ALL)
	$(AR) libepr.a $(ALL)
	ranlib libepr.a

#malloc.o: malloc.cpp

noise.o:
	$(CC) noise.hpp noise.cpp 

raster.o:
	$(CC) raster.hpp atomics.hpp raster.cpp

rays.o:
	$(CC) rays.hpp rays.cpp

view.o:
	$(CC) rays.hpp view.hpp view.cpp

stack.o:
	$(CC) stack.hpp rays.hpp stack.cpp

octree.o:
	$(CC) octree.hpp octree.cpp

timing.o:
	$(CC) timing.hpp timing.cpp

clean:
	rm -f libepr.a *.o
