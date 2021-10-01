
CC = gcc -O2
LIB = /System/Library/Frameworks/OpenGL.framework/Libraries

blde2ppm: blde2ppm.c
	$(CC) $(LFLAGS) -L$(LIB) blde2ppm.c -o blde2ppm \
	-lGL -lGLU /System/Library/Frameworks/GLUT.framework/GLUT -lobjc -lm

turbulence: turbulence.c noise.o
	$(CC) $(LFLAGS) -L$(LIB) turbulence.c -o turbulence noise.o \
	-lGL -lGLU /System/Library/Frameworks/GLUT.framework/GLUT -lobjc

turb: turb.c
	$(CC) $(LFLAGS) -L$(LIB) turb.c -o turb \
	-lGL -lGLU /System/Library/Frameworks/GLUT.framework/GLUT -lobjc

builddep: builddep.c
	$(CC) $(LFLAGS) -L$(LIB) builddep.c -o builddep \
	-lGL -lGLU /System/Library/Frameworks/GLUT.framework/GLUT -lobjc

buildcurl: buildcurl.c
	$(CC) $(LFLAGS) -L$(LIB) buildcurl.c -o buildcurl \
	-lGL -lGLU /System/Library/Frameworks/GLUT.framework/GLUT -lobjc

dumpview: dumpview.c
	$(CC) $(LFLAGS) -L$(LIB) dumpview.c -o dumpview \
	-lGL -lGLU /System/Library/Frameworks/GLUT.framework/GLUT -lobjc

viewvol: viewvol.c
	$(CC) $(LFLAGS) -L$(LIB) viewvol.c -o viewvol \
	-lGL -lGLU /System/Library/Frameworks/GLUT.framework/GLUT -lobjc

storm: storm.c noise.o
	$(CC) $(LFLAGS) -L$(LIB) storm.c -o storm noise.o \
	-lGL -lGLU /System/Library/Frameworks/GLUT.framework/GLUT -lobjc

noise.o: noise.h noise.c
	$(CC) -c noise.c

