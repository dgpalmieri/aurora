

CC = gcc -O2
LIB = /System/Library/Frameworks/OpenGL.framework/Libraries

gen_dmsp: gen_dmsp.c rays.o
	cc gen_dmsp.c -o gen_dmsp rays.o -lm

turb: turb.c noise.o
	cc turb.c noise.o -o turb -lm

movieplayer: movieplayer.c
	$(CC) $(LFLAGS) -L$(LIB) movieplayer.c -o movieplayer \
	-lGL -lGLU /System/Library/Frameworks/GLUT.framework/GLUT -lobjc -lm

