
# Apple G4 uniprocessor version
# CC = gcc -g -DSGI32
CC = cc -O2 -DSGI32
AR = ar -r
ALL = noise.o stack.o octree.o raster.o rays.o timing.o view.o

libepr.a: $(ALL)
	$(AR) libepr.a $(ALL)
	ranlib libepr.a

malloc.o: malloc.c

noise.o: noise.h noise.c

raster.o: raster.h atomics.h raster.c

rays.o: rays.h rays.c

view.o: rays.h view.h view.c

stack.o: stack.h rays.h stack.c

octree.o: octree.h octree.c

timing.o: timing.h timing.c

clean:
	rm -f libepr.a *.o

tar:
	tar -cvf libepr.tar Makefile *.c *.h
