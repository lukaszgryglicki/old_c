all: game game.dbg 
game: game.c xmwnd.h mh.h mran.h mgraph.h qsortasm.o
	c++ -I/usr/X11R6/include -L/usr/X11R6/lib -o game game.c qsortasm.o -lX11
	strip game
game.dbg: game.c xmwnd.h mh.h mran.h mgraph.h qsortasm.o
	c++ -Wall -I/usr/X11R6/include -L/usr/X11R6/lib -g3 -DDBG -DDEBUG --save-temps -o game.dbg game.c qsortasm.o -lX11
	-rm *.ii
qsortasm.o: qsortasm.s
	as -o qsortasm.o qsortasm.s


