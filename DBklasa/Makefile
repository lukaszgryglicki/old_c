all: DBklasa DBklasa.exe DBklasa.dbg
DBklasa: DBklasa.c 
	gcc -DUNIX -ansi -Wall -pedantic -Os -I/usr/local/pgsql/include -L/usr/local/pgsql/lib/ -o DBklasa DBklasa.c -lpq
	strip -s DBklasa
DBklasa.exe: DBklasa.c 
	gcc -DWINDOWS -Wall -ansi -O2 -I/usr/local/pgsql/include -L/usr/local/pgsql/lib/ -o DBklasa.exe DBklasa.c -lpq
DBklasa.dbg: DBklasa.c 
	gcc -DDEBUG -DUNIX -Wall -ansi -g3 -I/usr/local/pgsql/include -L/usr/local/pgsql/lib/ -o DBklasa.dbg DBklasa.c -lpq
package:
	tar jcf DBklasa.tar.bz2 *.c *.SQL Makefile 
