# Makefile for quadtree programs

all: printquad showquad

showquad:	showquad.c
	gcc -o showquad showquad.c -L/usr/local/X11/lib64 -lX11 -lm

printquad:	printquad.c
	gcc -o printquad printquad.c

clean:
	rm -f printquad showquad
