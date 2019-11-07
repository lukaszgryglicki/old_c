/* Written by Morgoth DBMA, morgothdbma@o2.pl
 This is part of PgXexplorer software, Open Source
 on BSD licence, Libraries(interaces) used:
 GNU GCC, AS (all stuff needed to compile C source into executable binary)
 LibPQ-FE from PostgreSQL, GTK (GIMP Toolkit)
 written in VIM editor, ctags used, CVS used
 Currently only one author: MOrgoth DBMA
 FILE: query.h */
#ifndef __MORGOTHDBMA_QUERY_H__
#define __MORGOTHDBMA_QUERY_H__
#define MAX_ERROR_STRING 4096
#define MAX_QUERY_LENGTH 4096
#include "ilibpq.h"
#include <string.h>
#include "consoleout.h"
#include "mmsystem.h"
PGconn* connect_db(CHAR*, CHAR*);
PGconn* gtk_connect_db(CHAR*,CHAR*,CHAR*);
void clear_result(PGresult**);
void disconnect_db(PGconn**);
PGresult* execute_silent_query(PGconn*,CHAR*);
PGresult* execute_query(PGconn*,CHAR*);
void execute_printf_query(PGconn*,PGresult**,CHAR*, CHAR*,...);
void execute_printf(PGconn*,CHAR****, INT*, INT*, CHAR*, INT, CHAR*,...);
void mexecute_printf(PGconn*,CHAR****, INT*, INT*, CHAR*, INT, CHAR*,...);
void mem_execute_printf(struct mem_list**, PGconn*,CHAR****, INT*, INT*, CHAR*, INT, CHAR*,...);
void dmexecute_printf(PGconn*,CHAR****, INT*, INT*, CHAR*, INT, CHAR*,INT,CHAR*,...);
void dmem_execute_printf(struct dmem_list**, PGconn*,CHAR****, INT*, INT*, CHAR*, INT, CHAR*,INT,CHAR*,...);
CHAR*** p3c_execute_printf(PGconn*,INT*, INT*,CHAR*,INT, CHAR*,...);
CHAR*** mp3c_execute_printf(PGconn*,INT*, INT*,CHAR*,INT, CHAR*,...);
CHAR*** mem_p3c_execute_printf(struct mem_list**, PGconn*,INT*, INT*,CHAR*,INT, CHAR*,...);
CHAR*** dmp3c_execute_printf(PGconn*,INT*, INT*,CHAR*,INT, CHAR*,INT,CHAR*,...);
CHAR*** dmem_p3c_execute_printf(struct dmem_list**, PGconn*,INT*, INT*,CHAR*,INT, CHAR*,INT,CHAR*,...);
CHAR*** pgres2pc3(const PGresult*,INT*,INT*, INT);
CHAR*** mpgres2pc3(const PGresult*,INT*,INT*, INT);
CHAR*** mem_pgres2pc3(struct mem_list**,const PGresult*,INT*,INT*, INT);
CHAR*** dmpgres2pc3(const PGresult*,INT*,INT*, INT,CHAR*,INT);
CHAR*** dmem_pgres2pc3(struct dmem_list**,const PGresult*,INT*,INT*, INT,CHAR*,INT);
void free_p3c(CHAR****,INT,INT);
void mfree_p3c(CHAR****,INT,INT);
void dmfree_p3c(CHAR****,INT,INT,CHAR*,INT);
void mem_free_p3c(struct mem_list**,CHAR****,INT,INT);
void dmem_free_p3c(struct dmem_list**,CHAR****,INT,INT,CHAR*,INT);
INT get_db_tables(PGconn*,CHAR***, INT*,INT);
INT mget_db_tables(PGconn*,CHAR***, INT*,INT);
INT mem_get_db_tables(struct mem_list**, PGconn*,CHAR***, INT*,INT);
INT dmget_db_tables(PGconn*,CHAR***, INT*,INT,CHAR*,INT);
INT dmem_get_db_tables(struct dmem_list**, PGconn*,CHAR***, INT*,INT,CHAR*,INT);
#endif
