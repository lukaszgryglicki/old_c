/* Written by Morgoth DBMA, morgothdbma@o2.pl
 This is part of PgXexplorer software, Open Source
 on BSD licence, Libraries(interaces) used:
 GNU GCC, AS (all stuff needed to compile C source into executable binary)
 LibPQ-FE from PostgreSQL, GTK (GIMP Toolkit)
 written in VIM editor, ctags used, CVS used
 Currently only one author: MOrgoth DBMA
 FILE: pgxtest.c */
#include "common.h"
#include "consoleout.h"
#include "gtktest.h"
#include "query.h"
int timeout=10;
int cont=1;
PGconn* connection;

int main(int lb, char** par)
{
 PGresult* pgres;
 char*** out;
 int a,b;
 debug("%s:%d pgxtest(%d)",__FILE__,__LINE__,lb);
 connection=0;
 pgres=0;
 out=0;
 a=b=0;
 printf("Testing PostgreSQL\nTesting ability to integrate with LibPQ...\n");
 if (lb>=2) connection = connect_db("template1", par[1]);
 else      connection = connect_db("template1", NULL);
 if (!connection) { error("connect to template1\n"); return 1; }
 pgres=execute_query(connection, "CREATE DATABASE pgxtest");
 if (!pgres) { error("create DB\n"); return 1; }
 printf("CREATE DATABASE OK.\n");
 disconnect_db(&connection);
 if (lb>=2) connection = connect_db("pgxtest", par[1]);
 else      connection = connect_db("pgxtest", NULL);
 if (!connection) { error("connect to pgxtest\n"); return 1; }
 pgres=execute_query(connection, "CREATE TABLE test(a INT, b text)");
 if (!pgres)  error("create table\n"); else printf("CREATE TABLE OK.\n");
 execute_query(connection, "INSERT INTO test(a,b) VALUES (1,'one')");
 execute_query(connection, "INSERT INTO test(a,b) VALUES (2,'two')");
 pgres = execute_query(connection, "INSERT INTO test(a,b) VALUES (3,'three')");
 if (!pgres)  error("insert\n"); else printf("INSERT OK.\n");
 pgres = execute_query(connection, "SELECT * FROM test");
 if (!pgres)  error("select\n");
 else
  {
   printf("INSERT OK.\n");
   out = pgres2pc3(pgres, &a, &b, 0);
   clear_result(&pgres);
   print_results(out,a,b);
   free_p3c(&out,a,b);
   printf("FREE RESULTS OK.\n");
  }
 execute_query(connection, "INSERT INTO test VALUES(10, 'Button From DB')");
 execute_query(connection, "INSERT INTO test VALUES(11, 'Another such')");
 printf("Testing GTK.\n");
 if (lb>=2) test_gtk(connection, par[1]);
 else test_gtk(connection, NULL);
 return 0;
}

