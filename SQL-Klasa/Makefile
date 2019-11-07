all: sqlklasa.dbg sqlklasa
sqlklasa.dbg: sqlkl.c
	c++ -L/usr/local/lib/mysql -I/usr/local/include/mysql -DREENTRANT -g3 -Wall -ansi -o sqlklasa.dbg sqlkl.c -lmysqlclient 
sqlklasa: sqlkl.c
	c++ -L/usr/local/lib/mysql -I/usr/local/include/mysql -DREENTRANT -Wall -ansi -O3 -o sqlklasa sqlkl.c -lmysqlclient 
	-strip -s sqlklasa
clean:
	-rm -f sqlklasa sqlklasa.dbg

