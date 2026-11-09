all: DBclass 
CFLAGS = -ansi -Wall -pedantic -O3 -I/usr/local/pgsql/include -I/usr/local/include
CLINK = -L/usr/local/pgsql/lib -L/usr/local/lib -lpq
CC = gcc
DBclass: DBclass.c
	$(CC) $(CFLAGS) -o DBclass DBclass.c $(CLINK)
	strip -s DBclass
package:
	tar jcf DBclass.tar.bz2 *.c *.SQL Makefile 
install: DBclass
	-pg_ctl start
	-createdb morgoth
	psql -f ./Morgoth.SQL
	cp DBclass /usr/local/bin
	echo "Instalation completed."
clean:
	-rm  DBclass
