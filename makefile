CC=g++
CFLAGS=-g -Wall
IFLAGS=-I/usr/X11R6/include
LFLAGS=-L/usr/X11R6/lib -lX11
all: circle_pack

circle_pack: circle_pack.cc combinatorics.cc trigonometry.cc layout.cc graphics.cc output.cc input_routine.cc 
	$(CC) $(CFLAGS) $(IFLAGS) -o circle_pack circle_pack.cc $(LFLAGS) -lm

clean: rm circle_pack
