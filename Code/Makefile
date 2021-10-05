
# Apple G4 uniprocessor version
#CC = gcc -g
CC = cc -O2
AR = ar -r

LIB = Library
GLIB = /System/Library/Frameworks/OpenGL.framework/Libraries

bench_dep: bench_dep.c $(LIB)/libaurora.a
	$(CC) $(LFLAGS) -I$(LIB) -L$(LIB) bench_dep.c -o bench_dep -laurora -lm
	cp bench_dep /Users/genetti/bin/bench_dep

bench_trilerp: bench_trilerp.c $(LIB)/libaurora.a
	$(CC) $(LFLAGS) -I$(LIB) -L$(LIB) bench_trilerp.c -o bench_trilerp \
	-laurora -lm
	cp bench_trilerp /Users/genetti/bin/bench_trilerp

ppmcrop: ppmcrop.c
	$(CC) ppmcrop.c -o ppmcrop
	cp ppmcrop /Users/genetti/bin/ppmcrop

gamma: gamma.c
	$(CC) gamma.c -o gamma -lm
	cp gamma /Users/genetti/bin/gamma

oldfraw2ppm: oldfraw2ppm.c
	$(CC) oldfraw2ppm.c -o oldfraw2ppm -lm
	cp oldfraw2ppm /Users/genetti/bin/oldfraw2ppm

sp2ppm: sp2ppm.c
	$(CC) sp2ppm.c -o sp2ppm -lm

e2ppm: e2ppm.c
	$(CC) e2ppm.c -o e2ppm -lm
	cp e2ppm /Users/genetti/bin/e2ppm

e2ppmB: e2ppm.c
	$(CC) -DSHOW_EARTH_AS_BLUE e2ppm.c -o e2ppmB -lm
	cp e2ppmB /Users/genetti/bin/e2ppmB

e2ppmS: e2ppm.c
	$(CC) -DSWAP_BYTES e2ppm.c -o e2ppmS -lm
	cp e2ppmS /Users/genetti/bin/e2ppmS

e2ppmBS: e2ppm.c
	$(CC) -DSWAP_BYTES -DSHOW_EARTH_AS_BLUE e2ppm.c -o e2ppmBS -lm
	cp e2ppmBS /Users/genetti/bin/e2ppmBS

energy2ppm: energy2ppm.c
	$(CC) energy2ppm.c -o energy2ppm -lm
	cp energy2ppm /Users/genetti/bin/energy2ppm

fraw2ppm: fraw2ppm.c
	$(CC) fraw2ppm.c -o fraw2ppm -lm
	cp fraw2ppm /Users/genetti/bin/fraw2ppm

expose: expose.c
	$(CC) -L$(LIB) expose.c -o expose -laurora -lm
	cp expose /Users/genetti/bin/expose

slicer: slicer.c $(LIB)/libaurora.a
	$(CC) $(LFLAGS) -I$(LIB) -L$(LIB) slicer.c -o slicer -laurora -lm
	cp slicer /Users/genetti/bin/slicer

pot_interp: pot_interp.c $(LIB)/libaurora.a
	$(CC) $(LFLAGS) -I$(LIB) -L$(LIB) pot_interp.c -o pot_interp -laurora -lm
	cp pot_interp /Users/genetti/bin/pot_interp

pot2energy: pot2energy.c $(LIB)/libaurora.a
	$(CC) $(LFLAGS) -I$(LIB) -L$(LIB) pot2energy.c -o pot2energy -laurora -lm
	cp pot2energy /Users/genetti/bin/pot2energy

pot2alpha: pot2alpha.c $(LIB)/libaurora.a
	$(CC) $(LFLAGS) -I$(LIB) -L$(LIB) pot2alpha.c -o pot2alpha -laurora -lm
	cp pot2alpha /Users/genetti/bin/pot2alpha

pot2ppm: pot2ppm.c $(LIB)/libaurora.a
	$(CC) $(LFLAGS) -I$(LIB) -L$(LIB) pot2ppm.c -o pot2ppm -laurora -lm
	cp pot2ppm /Users/genetti/bin/pot2ppm

pot2volc: pot2volc.c $(LIB)/libaurora.a
	$(CC) $(LFLAGS) -I$(LIB) -L$(LIB) pot2volc.c -o pot2volc -laurora -lm
	cp pot2volc /Users/genetti/bin/pot2volc

eview: eview.c
	$(CC) $(LFLAGS) -L$(LIB) eview.c -o eview \
	-lGL -lGLU /System/Library/Frameworks/GLUT.framework/GLUT -lobjc

