
# Apple G4 uniprocessor version
# CC = gcc -g -DSGI32
CC = cc -O2 -DSGI32

LIB = Library

epr: render.o epr.o $(LIB)/libepr.a
	$(CC) $(LFLAGS) -I$(LIB) -L$(LIB) render.o epr.o -o epr \
	-lepr -lm
#	cp epr /Users/genetti/bin/epr

# defaults to only rendering green
epr.o: $(LIB)/atomics.h render.h epr.c
	$(CC) -DDO_GREEN $(CFLAGS) -I$(LIB) -c epr.c

render.o: $(LIB)/atomics.h render.h render.c
	$(CC) $(CFLAGS) -I$(LIB) -c render.c

epr.gre: render.o epr_gre.o $(LIB)/libepr.a
	$(CC) $(LFLAGS) -I$(LIB) -L$(LIB) render.o epr_gre.o -o epr.gre \
	-lepr -lm

epr.red: render.o epr_red.o $(LIB)/libepr.a
	$(CC) $(LFLAGS) -I$(LIB) -L$(LIB) render.o epr_red.o -o epr.red \
	-lepr -lm

epr.mag: render.o epr_mag.o $(LIB)/libepr.a
	$(CC) $(LFLAGS) -I$(LIB) -L$(LIB) render.o epr_mag.o -o epr.mag \
	-lepr -lm

epr_gre.o: $(LIB)/atomics.h render.h epr.c
	$(CC) -DDO_GREEN $(CFLAGS) -I$(LIB) -c epr.c -o epr_gre.o

epr_red.o: $(LIB)/atomics.h render.h epr.c
	$(CC) -DDO_RED $(CFLAGS) -I$(LIB) -c epr.c -o epr_red.o

epr_mag.o: $(LIB)/atomics.h render.h epr.c
	$(CC) -DDO_MAGENTA $(CFLAGS) -I$(LIB) -c epr.c -o epr_mag.o

clean:
	rm -f epr epr.gre epr.mag epr.red *.o

tar:
	tar -cvf epr.tar epr.c render.h render.c Makefile \
	$(LIB)/Makefile $(LIB)/*.h $(LIB)/*.c
