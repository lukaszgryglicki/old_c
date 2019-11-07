# Written by Morgoth DBMA, morgothdbma@o2.pl
# This is part of PgXexplorer software, Open Source
# on BSD licence, Libraries(interaces) used:
# GNU GCC, AS (all stuff needed to compile C source into executable binary)
# LibPQ-FE from PostgreSQL, GTK (GIMP Toolkit)
# written in VIM editor, ctags used, CVS used
# Currently only one author: MOrgoth DBMA
# FILE: Makefile (automatically generated)
all: pgxtest pgxexplorer pbar
CFLAGS = -Wall -std=c99 -pedantic -O3 -I/usr/X11R6/include/gtk12 -I/usr/local/include/glib12 -I/usr/local/include -I/usr/X11R6/include -I/usr/local/pgsql/include
CFLESS = -Wall -std=c99 -pedantic -O3
CLINK  = -O3 -L/usr/X11R6/lib -L/usr/X11R6/lib -lgtk12 -lgdk12 -L/usr/local/lib -Wl,-E -lgmodule12 -lglib12 -lintl -lXi -lXext -lX11 -lm -L/usr/local/pgsql/lib -lpq 
GCC = gcc
main.o: main.c main.h gtkincl.h consoleout.h gtkdialogs.h pgxexplorer.h signals.h common.h entireDB.c mmsystem.h sqlparser.c sqlparser.h 
	$(GCC) $(CFLAGS) -c main.c -o main.o
pgxexplorer: main.o consoleout.o gtkdialogs.o query.o signals.o toolbar.o entireDB.o mmsystem.o sqlparser.o wizards.o pbar
	$(GCC) main.o consoleout.o gtkdialogs.o query.o signals.o toolbar.o entireDB.o mmsystem.o sqlparser.o wizards.o -o pgxexplorer $(CLINK)
pgxtest: pgxtest.o consoleout.o query.o gtktest.o signals.o gtkdialogs.o mmsystem.o
	$(GCC) -o pgxtest pgxtest.o consoleout.o query.o gtktest.o signals.o gtkdialogs.o mmsystem.o $(CLINK)
pbar: pbar.c pbar.h common.h 
	$(GCC) $(CFLAGS) -o pbar pbar.c $(CLINK)
pgxtest.o: pgxtest.c consoleout.h query.h ilibpq.h common.h 
	$(GCC) $(CFLAGS) -c pgxtest.c
consoleout.o: consoleout.c consoleout.h common.h mmsystem.h
	$(GCC) $(CFLAGS) -c consoleout.c
query.o: query.c consoleout.h query.h ilibpq.h signals.h signals.c common.h mmsystem.h 
	$(GCC) $(CFLAGS) -c query.c
toolbar.o: toolbar.h toolbar.c main.h common.h 
	$(GCC) $(CFLAGS) -c toolbar.c
wizards.o: wizards.c wizards.h main.h common.h 
	$(GCC) $(CFLAGS) -c wizards.c
entireDB.o: entireDB.c entireDB.h main.h common.h ilibpq.h query.h gtkdialogs.h pgxexplorer.h mmsystem.h 
	$(GCC) $(CFLAGS) -c entireDB.c
signals.o: signals.h signals.c gtkdialogs.h gtkdialogs.c common.h 
	$(GCC) $(CFLAGS) -c signals.c
gtktest.o: gtktest.c ilibpq.h query.h gtktest.h gtkincl.h common.h 
	$(GCC) $(CFLAGS) -c gtktest.c
gtkdialogs.o: gtkdialogs.c gtkdialogs.h gtkincl.h ilibpq.h query.h consoleout.h common.h mmsystem.h 
	$(GCC) $(CFLAGS) -c gtkdialogs.c
mmsystem.o: mmsystem.c mmsystem.h common.h gtkdialogs.h consoleout.h 
	$(GCC) $(CFLAGS) -c mmsystem.c
sqlparser.o: mmsystem.c mmsystem.h common.h consoleout.h consoleout.c
	$(GCC) $(CFLAGS) -c sqlparser.c
clean:
	-rm -f *.o pgxexplorer pgxtest pbar
install: pgxexplorer pbar
	cp ./pgxexplorer /usr/local/pgsql/bin/
	cp ./pgxtest /usr/local/pgsql/bin/
	cp ./pbar /usr/local/pgsql/bin/
package:
	mkdir PgXexplorer-0.32
	mkdir PgXexplorer-0.32/extra
	cp *.C *.c *.h *.s mkfile_parser PARSER.DAT tags configure INSTALL README IOTODO.sxw *.SQL *.QUE PgXexplorer-0.32/
	cp extra/Makefile.* extra/README extra/*.c extra/*.h extra/configure extra/*.SQL PgXexplorer-0.32/extra
	tar jcvf PgXexplorer-0.32.tar.bz2 PgXexplorer-0.32/
	rm -rf PgXexplorer-0.32/
