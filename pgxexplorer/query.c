/* Written by Morgoth DBMA, morgothdbma@o2.pl
 This is part of PgXexplorer software, Open Source
 on BSD licence, Libraries(interaces) used:
 GNU GCC, AS (all stuff needed to compile C source into executable binary)
 LibPQ-FE from PostgreSQL, GTK (GIMP Toolkit)
 written in VIM editor, ctags used, CVS used
 Currently only one author: MOrgoth DBMA
 FILE: query.c */
#include "common.h"
#include "consoleout.h"
#include "query.h"
#include "signals.h"
/*FIXME dodac obsluge debug, error ==> DlgWindow*/
/* THERE WOULDN'T BE ANY DEBUG LOG TO FILE ETC. */
INT ping_conn;
extern INT timeout;

INT connAlive(PGconn* c)
{
 PGresult* r;
 INT stat;
 debug("%s:%d connAlive(%p)", __FILE__,__LINE__,c);
 ping_conn=1;
 alarm_clock(timeout);
 r= PQexec(c, "SELECT datname FROM pg_database");
 alarm_stop();
 if (!r)
   {
    debug("PQresult is NULL %s\n", PQerrorMessage(c));
    return 0;
   }
 stat = PQresultStatus(r);
 if (stat!=PGRES_COMMAND_OK && stat!=PGRES_TUPLES_OK)
   {
    debug("PQexec error: %s %s",PQcmdStatus(r),PQresultErrorMessage(r));
    return 0;
   }
 return ping_conn;
}


void disconnect_db(PGconn** c)
{
 debug("%s:%d disconnect_db(%p)", __FILE__,__LINE__,*c);
 if (*c) PQfinish(*c);
 else warn("connect is null: %s:%d", __FILE__,__LINE__);
 *c = NULL;
}


PGconn* connect_db(CHAR* arg, CHAR* pass)
{
 PGconn* conn;
 CHAR* strarg;
 conn=0;
 debug("%s:%d connect_db(%s,***)", __FILE__,__LINE__,arg?arg:"null");
 strarg = (CHAR*)malloc(strlen(arg)+64+(pass?strlen(pass):1));
 debug("\nconnect to database %s\n", arg);
 if (!pass) sprintf(strarg,"dbname=template1");
 else	     sprintf(strarg,"dbname=template1 password='%s'", pass);
 alarm_clock(timeout);
 conn = PQconnectdb(strarg);
 alarm_stop();
 if (!conn) { error("cannot connect to template1 DB"); free(strarg); return NULL; }
 if (PQstatus(conn)==CONNECTION_BAD) {  error("cannot connect to template1 DB"); free(strarg); return NULL; }
 disconnect_db(&conn);
 if (!pass) sprintf(strarg,"dbname=%s", arg);
 else sprintf(strarg,"dbname=%s password='%s'", arg,pass);
 alarm_clock(timeout);
 conn = PQconnectdb(strarg);
 alarm_stop();
 if (!conn) { free(strarg); error("connection is NULL %s:%d", __FILE__,__LINE__); return NULL; }
 if (PQstatus(conn) == CONNECTION_BAD)
   {
    error("\nCannot connect to DB:%s\n", arg);
    ERR
    free(strarg);
    return NULL;
   }
 free(strarg);
 return conn;
}


PGconn* gtk_connect_db(CHAR* arg, CHAR* pass, CHAR* errstr)
{
 PGconn* conn;
 CHAR* strarg;
 conn=0;
 debug("%s:%d gtk_connect_db(%s,***)", __FILE__,__LINE__,arg?arg:"null");
 strarg = (CHAR*)malloc(strlen(arg)+64+(pass?strlen(pass):1));
 if (!pass) sprintf(strarg,"dbname=template1");
 else       sprintf(strarg,"dbname=template1 password='%s'", pass);
 alarm_clock(timeout);
 conn = PQconnectdb(strarg);
 alarm_stop();
 if (!conn) { sprintf(errstr,"cannot connect to template1 DB"); free(strarg); return NULL; }
 if (PQstatus(conn)==CONNECTION_BAD) {  sprintf(errstr,"cannot connect to template1 DB"); free(strarg); return NULL; }
 disconnect_db(&conn);
 if (!pass) sprintf(strarg,"dbname=%s",arg);
 else       sprintf(strarg,"dbname=%s password='%s'", arg, pass);
 alarm_clock(timeout);
 conn = PQconnectdb(strarg);
 alarm_stop();
 if (!conn) { error("connection is NULL %s:%d", __FILE__,__LINE__); free(strarg); return NULL; }
 if (PQstatus(conn) == CONNECTION_BAD)
  {
   sprintf(errstr, "Connection string:\ndbname=%s failed.", arg);
   free(strarg);
   return NULL;
  }
 free(strarg);
 return conn;
}


void clear_result(PGresult** r)
{
 debug("%s:%d clear_result()", __FILE__,__LINE__);
 if (*r) PQclear(*r);
 *r=0;
}


PGresult* execute_silent_query(PGconn* c,  CHAR* query)
{
 PGresult* r;
 INT stat;
 debug("%s:%d execute_silent_query(%s)", __FILE__,__LINE__,query);
 debug("SQLquery: \"%s\"\n", query);
 if (!connAlive(c)) { debug("connection dead!\n"); return NULL; }
 if (!query)
   {
    debug("NULL query passed\n");
    return NULL;
   }
 if (!c) { error("no connection"); return NULL; }
 r= PQexec(c, query);
 if (!r)
   {
    debug("PQresult is NULL %s\n", PQerrorMessage(c));
    return NULL;
   }
 stat = PQresultStatus(r);
 if (stat!=PGRES_COMMAND_OK && stat!=PGRES_TUPLES_OK)
   {
    debug("PQexec error: %s %s",PQcmdStatus(r),PQresultErrorMessage(r));
    debug("Failed query was: \"%s\"\n", query);
    return NULL;
   }
 return r;
}


CHAR*** p3c_execute_printf(PGconn* c,INT* row, INT* col, CHAR* errstr, INT want_meta, CHAR* fmt,...)
{
 va_list ap;
 CHAR*** ret;
 CHAR msg[MAX_QUERY_LENGTH*2];
 debug("%s:%d p3c_execute_printf(%s, %d)", __FILE__,__LINE__,fmt,want_meta);
 if (!c) { error("no connection"); return NULL; }
 va_start(ap, fmt);
 vsprintf(msg,fmt,ap);
 va_end(ap);
 execute_printf(c, &ret, row, col, errstr, want_meta, msg);
 return ret;
}


CHAR*** dmp3c_execute_printf(PGconn* c,INT* row, INT* col, CHAR* errstr, INT want_meta, CHAR* file, INT line, CHAR* fmt,...)
{
 va_list ap;
 CHAR*** ret;
 CHAR msg[MAX_QUERY_LENGTH*2];
 debug("%s:%d dmp3c_execute_printf(%s,%d) called from %s:%d", __FILE__,__LINE__,fmt,want_meta,file,line);
 if (!c) { error("no connection"); return NULL; }
 va_start(ap, fmt);
 vsprintf(msg,fmt,ap);
 va_end(ap);
 dmexecute_printf(c, &ret, row, col, errstr, want_meta, HERE, msg);
 return ret;
}


CHAR*** dmem_p3c_execute_printf(struct dmem_list** mem, PGconn* c,INT* row, INT* col, CHAR* errstr, INT want_meta, CHAR* file, INT line, CHAR* fmt,...)
{
 va_list ap;
 CHAR*** ret;
 CHAR msg[MAX_QUERY_LENGTH*2];
 debug("%s:%d dmem_p3c_execute_printf(%s,%d) called from %s:%d", __FILE__,__LINE__,fmt,want_meta,file,line);
 if (!c) { error("no connection"); return NULL; }
 va_start(ap, fmt);
 vsprintf(msg,fmt,ap);
 va_end(ap);
 dmem_execute_printf(mem, c, &ret, row, col, errstr, want_meta, HERE, msg);
 return ret;
}


CHAR*** mp3c_execute_printf(PGconn* c,INT* row, INT* col, CHAR* errstr, INT want_meta, CHAR* fmt,...)
{
 va_list ap;
 CHAR*** ret;
 CHAR msg[MAX_QUERY_LENGTH*2];
 debug("%s:%d mp3c_execute_printf(%s,%d)", __FILE__,__LINE__,fmt,want_meta);
 if (!c) { error("no connection"); return NULL; }
 va_start(ap, fmt);
 vsprintf(msg,fmt,ap);
 va_end(ap);
 mexecute_printf(c, &ret, row, col, errstr, want_meta, msg);
 return ret;
}


CHAR*** mem_p3c_execute_printf(struct mem_list** mem, PGconn* c,INT* row, INT* col, CHAR* errstr, INT want_meta, CHAR* fmt,...)
{
 va_list ap;
 CHAR*** ret;
 CHAR msg[MAX_QUERY_LENGTH*2];
 debug("%s:%d mem_p3c_execute_printf(%s,%d)", __FILE__,__LINE__,fmt,want_meta);
 if (!c) { error("no connection"); return NULL; }
 va_start(ap, fmt);
 vsprintf(msg,fmt,ap);
 va_end(ap);
 mem_execute_printf(mem, c, &ret, row, col, errstr, want_meta, msg);
 return ret;
}


void dmem_execute_printf(struct dmem_list** mem, PGconn* c, CHAR**** res, INT* row, INT* col, CHAR* errstr, INT want_meta, CHAR* file, INT line, CHAR* fmt,...)
{
 INT stat;
 va_list ap;
 CHAR query[MAX_QUERY_LENGTH*2];
 PGresult* r;
 debug("%s:%d mem_execute_printf(%s,%d) called from %s:%d", __FILE__,__LINE__,fmt,want_meta,file,line);
 va_start(ap, fmt);
 vsprintf(query,fmt,ap);
 va_end(ap);
 r=NULL;
 if (!c)
   {
    *res = NULL;
    *row = *col = -1;
    strcpy(errstr, "Not connected to DB");
    return;
   }
 if (!query)
   {
    *res = NULL;
    *row = *col = -1;
    strcpy(errstr, "NULL query passed to DB");
    return;
   }
 if (!connAlive(c)) { sprintf(errstr,"connection dead!\n"); return ; }
 r= PQexec(c, query);
 if (!r)
   {
    sprintf(errstr,"PQresult is NULL %s\nQuery was:\n%s\n", PQerrorMessage(c), query);
    *res = NULL;
    *row = *col = -1;
    return ;
   }
 stat = PQresultStatus(r);
 if (stat!=PGRES_COMMAND_OK && stat!=PGRES_TUPLES_OK)
   {
    sprintf(errstr,"PQexec:\n%s\n%s\nQuery was:\n%s",PQcmdStatus(r),PQresultErrorMessage(r), query);
    *res = NULL;
    *row = *col = -1;
    clear_result(&r);
    return;
   }
 *res = dmem_pgres2pc3(mem, r, row, col, want_meta, HERE);
 clear_result(&r);
}


void mem_execute_printf(struct mem_list** mem, PGconn* c, CHAR**** res, INT* row, INT* col, CHAR* errstr, INT want_meta, CHAR* fmt,...)
{
 INT stat;
 va_list ap;
 CHAR query[MAX_QUERY_LENGTH*2];
 PGresult* r;
 debug("%s:%d mem_execute_printf(%s,%d)", __FILE__,__LINE__,fmt,want_meta);
 va_start(ap, fmt);
 vsprintf(query,fmt,ap);
 va_end(ap);
 r=NULL;
 if (!c)
   {
    *res = NULL;
    *row = *col = -1;
    strcpy(errstr, "Not connected to DB");
    return;
   }
 if (!query)
   {
    *res = NULL;
    *row = *col = -1;
    strcpy(errstr, "NULL query passed to DB");
    return;
   }
 if (!connAlive(c)) { sprintf(errstr,"connection dead!\n"); return ; }
 r= PQexec(c, query);
 if (!r)
   {
    sprintf(errstr,"PQresult is NULL %s\nQuery was:\n%s\n", PQerrorMessage(c), query);
    *res = NULL;
    *row = *col = -1;
    return ;
   }
 stat = PQresultStatus(r);
 if (stat!=PGRES_COMMAND_OK && stat!=PGRES_TUPLES_OK)
   {
    sprintf(errstr,"PQexec:\n%s\n%s\nQuery was:\n%s",PQcmdStatus(r),PQresultErrorMessage(r), query);
    *res = NULL;
    *row = *col = -1;
    clear_result(&r);
    return;
   }
 *res = mem_pgres2pc3(mem, r, row, col, want_meta);
 clear_result(&r);
}


void dmexecute_printf(PGconn* c, CHAR**** res, INT* row, INT* col, CHAR* errstr, INT want_meta, CHAR* file, INT line, CHAR* fmt,...)
{
 INT stat;
 va_list ap;
 CHAR query[MAX_QUERY_LENGTH*2];
 PGresult* r;
 debug("%s:%d dmexecute_printf(%s,%d) called from %s:%d", __FILE__,__LINE__,fmt,want_meta,file,line);
 va_start(ap, fmt);
 vsprintf(query,fmt,ap);
 va_end(ap);
 r=NULL;
 if (!c)
   {
    *res = NULL;
    *row = *col = -1;
    strcpy(errstr, "Not connected to DB");
    return;
   }
 if (!query)
   {
    *res = NULL;
    *row = *col = -1;
    strcpy(errstr, "NULL query passed to DB");
    return;
   }
 if (!connAlive(c)) { sprintf(errstr,"connection dead!\n"); return ; }
 r= PQexec(c, query);
 if (!r)
   {
    sprintf(errstr,"PQresult is NULL %s\nQuery was:\n%s\n", PQerrorMessage(c), query);
    *res = NULL;
    *row = *col = -1;
    return ;
   }
 stat = PQresultStatus(r);
 if (stat!=PGRES_COMMAND_OK && stat!=PGRES_TUPLES_OK)
   {
    sprintf(errstr,"PQexec:\n%s\n%s\nQuery was:\n%s",PQcmdStatus(r),PQresultErrorMessage(r), query);
    *res = NULL;
    *row = *col = -1;
    clear_result(&r);
    return;
   }
 *res = dmpgres2pc3(r, row, col, want_meta, HERE);
 clear_result(&r);
}


void mexecute_printf(PGconn* c, CHAR**** res, INT* row, INT* col, CHAR* errstr, INT want_meta, CHAR* fmt,...)
{
 INT stat;
 va_list ap;
 CHAR query[MAX_QUERY_LENGTH*2];
 PGresult* r;
 debug("%s:%d mexecute_printf(%s,%d)", __FILE__,__LINE__,fmt,want_meta);
 va_start(ap, fmt);
 vsprintf(query,fmt,ap);
 va_end(ap);
 r=NULL;
 if (!c)
   {
    *res = NULL;
    *row = *col = -1;
    strcpy(errstr, "Not connected to DB");
    return;
   }
 if (!query)
   {
    *res = NULL;
    *row = *col = -1;
    strcpy(errstr, "NULL query passed to DB");
    return;
   }
 if (!connAlive(c)) { sprintf(errstr,"connection dead!\n"); return ; }
 r= PQexec(c, query);
 if (!r)
   {
    sprintf(errstr,"PQresult is NULL %s\nQuery was:\n%s\n", PQerrorMessage(c), query);
    *res = NULL;
    *row = *col = -1;
    return ;
   }
 stat = PQresultStatus(r);
 if (stat!=PGRES_COMMAND_OK && stat!=PGRES_TUPLES_OK)
   {
    sprintf(errstr,"PQexec:\n%s\n%s\nQuery was:\n%s",PQcmdStatus(r),PQresultErrorMessage(r), query);
    *res = NULL;
    *row = *col = -1;
    clear_result(&r);
    return;
   }
 *res = mpgres2pc3(r, row, col, want_meta);
 clear_result(&r);
}


void execute_printf(PGconn* c, CHAR**** res, INT* row, INT* col, CHAR* errstr, INT want_meta, CHAR* fmt,...)
{
 INT stat;
 va_list ap;
 CHAR query[MAX_QUERY_LENGTH*2];
 PGresult* r;
 debug("%s:%d execute_printf(%s,%d)", __FILE__,__LINE__,fmt,want_meta);
 va_start(ap, fmt);
 vsprintf(query,fmt,ap);
 va_end(ap);
 r=NULL;
 if (!c)
   {
    *res = NULL;
    *row = *col = -1;
    strcpy(errstr, "Not connected to DB");
    return;
   }
 if (!query)
   {
    *res = NULL;
    *row = *col = -1;
    strcpy(errstr, "NULL query passed to DB");
    return;
   }
 if (!connAlive(c)) { sprintf(errstr,"connection dead!\n"); return ; }
 r= PQexec(c, query);
 if (!r)
   {
    sprintf(errstr,"PQresult is NULL %s\nQuery was:\n%s\n", PQerrorMessage(c), query);
    *res = NULL;
    *row = *col = -1;
    return ;
   }
 stat = PQresultStatus(r);
 if (stat!=PGRES_COMMAND_OK && stat!=PGRES_TUPLES_OK)
   {
    sprintf(errstr,"PQexec:\n%s\n%s\nQuery was:\n%s",PQcmdStatus(r),PQresultErrorMessage(r), query);
    *res = NULL;
    *row = *col = -1;
    clear_result(&r);
    return;
   }
 *res = pgres2pc3(r, row, col, want_meta);
 clear_result(&r);
}


void execute_printf_query(PGconn* c,PGresult** r, CHAR* errstr, CHAR* fmt,...)
{
 INT stat;
 va_list ap;
 CHAR query[MAX_QUERY_LENGTH*2];
 debug("%s:%d execute_printf_query(%s)", __FILE__,__LINE__,fmt);
 va_start(ap, fmt);
 vsprintf(query,fmt,ap);
 va_end(ap);
 if (!c)
   {
    *r = NULL;
    strcpy(errstr, "Not connected to DB");
    return;
   }
 if (!query)
   {
    *r = NULL;
    strcpy(errstr, "NULL query passed to DB");
    return;
   }
 if (!connAlive(c)) { sprintf(errstr,"connection dead!\n"); return ; }
 *r= PQexec(c, query);
 if (!*r)
   {
    sprintf(errstr,"PQresult is NULL %s\nQuery was:\n%s\n", PQerrorMessage(c), query);
    return ;
   }
 stat = PQresultStatus(*r);
 if (stat!=PGRES_COMMAND_OK && stat!=PGRES_TUPLES_OK)
   {
    sprintf(errstr,"PQexec:\n%s\n%s\nQuery was:\n%s",PQcmdStatus(*r),PQresultErrorMessage(*r), query);
    clear_result(r);
    *r = NULL;
    return;
   }
}


INT get_db_tables(PGconn* c, CHAR*** dat, INT* ntabs, INT want_all_elems)
{
 CHAR*** res;
 CHAR** data;
 INT row,col,i;
 CHAR errstr[MAX_QUERY_LENGTH+100];
 data = *dat;
 debug("%s:%d get_db_tables(%d)", __FILE__,__LINE__,want_all_elems);
 if (!want_all_elems)
 execute_printf(c, &res, &row, &col, errstr, 0,
		 "SELECT tablename from pg_tables WHERE tablename NOT LIKE 'pg_%%' AND tablename NOT LIKE 'sql_%%' ORDER BY tablename");
 else
 execute_printf(c, &res, &row, &col, errstr, 0,
		 "SELECT tablename from pg_tables ORDER BY tablename");
 if (!res) { *dat=NULL; ntabs=0; warn("empty result get_db_tables: %s:%d", __FILE__,__LINE__); return 0; }
 if (col<0 || row<0) { printf("errstr=%s\n", errstr); (*dat) = NULL; *ntabs=-1; return 0; }
 *ntabs = row;
 data = (CHAR**)malloc(row*sizeof(CHAR*));
 for (i=0;i<row;i++)
   {
    data[i] = malloc(strlen(res[i][0])+1);
    strcpy(data[i], res[i][0]);
   }
 free_p3c(&res, row, col);		/* FIXME CHECK IT */
 *dat = data;
 return 1;
}


INT dmget_db_tables(PGconn* c, CHAR*** dat, INT* ntabs, INT want_all_elems, CHAR* file, INT line)
{
 CHAR*** res;
 CHAR** data;
 INT row,col,i;
 CHAR errstr[MAX_QUERY_LENGTH+100];
 data = *dat;
 debug("%s:%d mget_db_tables(%d) called from %s:%d", __FILE__,__LINE__,want_all_elems,file,line);
 if (!want_all_elems)
 dmexecute_printf(c, &res, &row, &col, errstr, 0,HERE,
		 "SELECT tablename from pg_tables WHERE tablename NOT LIKE 'pg_%%' AND tablename NOT LIKE 'sql_%%' ORDER BY tablename");
 else
 dmexecute_printf(c, &res, &row, &col, errstr, 0, HERE,
		 "SELECT tablename from pg_tables ORDER BY tablename");
 if (!res) { *dat=NULL; ntabs=0; warn("empty result get_db_tables: %s:%d", __FILE__,__LINE__); return 0; }
 if (col<0 || row<0) { printf("errstr=%s\n", errstr); (*dat) = NULL; *ntabs=-1; return 0; }
 *ntabs = row;
 data = (CHAR**)dmmalloc(row*sizeof(CHAR*), HERE);
 for (i=0;i<row;i++)
   {
    data[i] = dmmalloc(strlen(res[i][0])+1, HERE);
    strcpy(data[i], res[i][0]);
   }
 dmfree_p3c(&res, row, col, HERE);		/* FIXME CHECK IT */
 *dat = data;
 return 1;
}


INT mget_db_tables(PGconn* c, CHAR*** dat, INT* ntabs, INT want_all_elems)
{
 CHAR*** res;
 CHAR** data;
 INT row,col,i;
 CHAR errstr[MAX_QUERY_LENGTH+100];
 data = *dat;
 debug("%s:%d mget_db_tables(%d)", __FILE__,__LINE__,want_all_elems);
 if (!want_all_elems)
 execute_printf(c, &res, &row, &col, errstr, 0,
		 "SELECT tablename from pg_tables WHERE tablename NOT LIKE 'pg_%%' AND tablename NOT LIKE 'sql_%%' ORDER BY tablename");
 else
 execute_printf(c, &res, &row, &col, errstr, 0,
		 "SELECT tablename from pg_tables ORDER BY tablename");
 if (!res) { *dat=NULL; ntabs=0; warn("empty result get_db_tables: %s:%d", __FILE__,__LINE__); return 0; }
 if (col<0 || row<0) { printf("errstr=%s\n", errstr); (*dat) = NULL; *ntabs=-1; return 0; }
 *ntabs = row;
 data = (CHAR**)mmalloc(row*sizeof(CHAR*));
 for (i=0;i<row;i++)
   {
    data[i] = mmalloc(strlen(res[i][0])+1);
    strcpy(data[i], res[i][0]);
   }
 mfree_p3c(&res, row, col);		/* FIXME CHECK IT */
 *dat = data;
 return 1;
}


INT mem_get_db_tables(struct mem_list** mem, PGconn* c, CHAR*** dat, INT* ntabs, INT want_all_elems)
{
 CHAR*** res;
 CHAR** data;
 INT row,col,i;
 CHAR errstr[MAX_QUERY_LENGTH+100];
 data = *dat;
 debug("%s:%d mem_get_db_tables(%d)", __FILE__,__LINE__,want_all_elems);
 if (!want_all_elems)
 mem_execute_printf(mem,c, &res, &row, &col, errstr, 0,
		 "SELECT tablename from pg_tables WHERE tablename NOT LIKE 'pg_%%' AND tablename NOT LIKE 'sql_%%' ORDER BY tablename");
 else
 mem_execute_printf(mem,c, &res, &row, &col, errstr, 0,
		 "SELECT tablename from pg_tables ORDER BY tablename");
 if (!res) { *dat=NULL; ntabs=0; warn("empty result get_db_tables: %s:%d", __FILE__,__LINE__); return 0; }
 if (col<0 || row<0) { debug("errstr=%s\n", errstr); (*dat) = NULL; *ntabs=-1; return 0; }
 *ntabs = row;
 data = (CHAR**)smalloc(mem, row*sizeof(CHAR*));
 for (i=0;i<row;i++)
   {
    data[i] = smalloc(mem, strlen(res[i][0])+1);
    strcpy(data[i], res[i][0]);
   }
 mem_free_p3c(mem, &res, row, col);
 *dat = data;
 return 1;
}


INT dmem_get_db_tables(struct dmem_list** mem, PGconn* c, CHAR*** dat, INT* ntabs, INT want_all_elems,CHAR* file, INT line)
{
 CHAR*** res;
 CHAR** data;
 INT row,col,i;
 CHAR errstr[MAX_QUERY_LENGTH+100];
 data = *dat;
 debug("%s:%d mem_get_db_tables(%d) called from %s:%d", __FILE__,__LINE__,want_all_elems,file,line);
 if (!want_all_elems)
 dmem_execute_printf(mem,c, &res, &row, &col, errstr, 0,HERE,
		 "SELECT tablename from pg_tables WHERE tablename NOT LIKE 'pg_%%' AND tablename NOT LIKE 'sql_%%' ORDER BY tablename");
 else
 dmem_execute_printf(mem,c, &res, &row, &col, errstr, 0,HERE,
		 "SELECT tablename from pg_tables ORDER BY tablename");
 if (!res) { *dat=NULL; ntabs=0; warn("empty result get_db_tables: %s:%d", __FILE__,__LINE__); return 0; }
 if (col<0 || row<0) { debug("errstr=%s\n", errstr); (*dat) = NULL; *ntabs=-1; return 0; }
 *ntabs = row;
 data = (CHAR**)dsmalloc(mem, row*sizeof(CHAR*), HERE);
 for (i=0;i<row;i++)
   {
    data[i] = dsmalloc(mem, strlen(res[i][0])+1, HERE);
    strcpy(data[i], res[i][0]);
   }
 dmem_free_p3c(mem, &res, row, col, HERE);
 *dat = data;
 return 1;
}


PGresult* execute_query(PGconn* c,  CHAR* query)
{
 PGresult* r;
 INT stat;
 debug("%s:%d execute_query(%s)", __FILE__,__LINE__,query);
 debug("SQLquery: \"%s\"\n", query);
 if (!query)
   {
    ERR error("NULL query passed\n");
    return NULL;
   }
 if (!c) return NULL;
 if (!connAlive(c)) { debug("connection dead!\n"); return NULL; }
 r= PQexec(c, query);
 if (!r)
   {
    ERR error("PQresult is NULL %s\n", PQerrorMessage(c));
    return NULL;
   }
 stat = PQresultStatus(r);
 if (stat!=PGRES_COMMAND_OK && stat!=PGRES_TUPLES_OK)
   {
    ERR error("PQexec: %s %s",PQcmdStatus(r),PQresultErrorMessage(r));
    error("Failed query was: \"%s\"\n", query);
    return NULL;
   }
 return r;
}


CHAR*** dmem_pgres2pc3(struct dmem_list** mem, const PGresult* r, INT* a, INT* b, INT want_meta, CHAR* file, INT line)
{
 CHAR*** result;
 CHAR* tmp;
 INT i,j;
 debug("%s:%d dmem_pgres2pc3() called from %s:%d", __FILE__,__LINE__,file,line);
 debug("mem_pgres2pc3: %p\n", (void*)r);
 if (!r) return 0;
 i=j=0;
 *a = PQntuples(r);
 *b = PQnfields(r);
 if ((!(*a) || !(*b)) && !want_meta)
 //if (!(*a) || !(*b))
   {
    warn("no cols or no rows (%s:%d)", __FILE__,__LINE__);
    return NULL;
   }
 /*printf("ntuples=%d,%d\n", *a,*b);*/
 /*if (!(*a) || !(*b)) { warn("no cols or no rows (%s:%d)", __FILE__,__LINE__); return NULL; }*/
 if (!want_meta)
 {
 result = (CHAR***)dsmalloc(mem, (*a)*sizeof(CHAR**), HERE);
 if (!result) { ERR error("smalloc error\n"); return NULL; }
 for (i=0;i<*a;i++)
   {
    result[i] = (CHAR**)dsmalloc(mem, (*b)*sizeof(CHAR*), HERE);
    if (!result[i])
      {
       for (j=0;j<i;j++) dsfree(mem, (void**)(&result[i]), HERE);
       ERR error("smalloc_i error!\n");
       return NULL;
      }
   }
 for (i=0;i<*a;i++)
 for (j=0;j<*b;j++)
   {
    result[i][j] = (CHAR*)dsmalloc(mem, PQgetlength(r,i,j)+1, HERE);
    if (!result[i][j])
      {
       dmem_free_p3c(mem, &result,*a,*b, HERE);
       ERR error("malloc_ij error\n");
       return NULL;
      }
    strcpy(result[i][j], (CHAR*)PQgetvalue(r,i,j));
   }
/*#ifdef DEBUG
  debug("CHAR*** result is (%d:%d):\n",i,j);
  for (i=0;i<*a;i++)
    {
     for (j=0;j<*b;j++) debug("%-15s ", result[i][j]);
     ln();
    }
#endif*/
 return result;
 }
 else
 {
  if ((*b)<=0) return NULL;		/*FIXME is it OK? */
 (*a)++;
 result = (CHAR***)dsmalloc(mem, (*a)*sizeof(CHAR**), HERE);
 if (!result) { ERR error("smalloc error\n"); return NULL; }
 for (i=0;i<*a;i++)
   {
    result[i] = (CHAR**)dsmalloc(mem, (*b)*sizeof(CHAR*), HERE);
    if (!result[i])
      {
       for (j=0;j<i;j++) dsfree(mem, (void**)(&result[i]), HERE);
       ERR error("smalloc_i error!\n");
       return NULL;
      }
   }
 for (i=0;i<*b;i++)
   {
    tmp = PQfname(r,i);
    result[0][i] = (CHAR*)dsmalloc(mem, strlen(tmp)+1, HERE);
    if (!result[0][i]) { ERR error("smalloc_namej error"); return NULL; }
    strcpy(result[0][i], tmp);
   }
 for (i=1;i<*a;i++)
 for (j=0;j<*b;j++)
   {
    result[i][j] = (CHAR*)dsmalloc(mem, PQgetlength(r,i-1,j)+1, HERE);
    if (!result[i][j])
      {
       dmem_free_p3c(mem, &result,*a,*b, HERE);
       ERR error("malloc_ij error\n");
       return NULL;
      }
    strcpy(result[i][j], (CHAR*)PQgetvalue(r,i-1,j));
   }
  return result;
 }
}


CHAR*** mem_pgres2pc3(struct mem_list** mem, const PGresult* r, INT* a, INT* b, INT want_meta)
{
 CHAR*** result;
 CHAR* tmp;
 INT i,j;
 debug("%s:%d mem_pgres2pc3()", __FILE__,__LINE__);
 debug("mem_pgres2pc3: %p\n", (void*)r);
 if (!r) return 0;
 i=j=0;
 *a = PQntuples(r);
 *b = PQnfields(r);
 if ((!(*a) || !(*b)) && !want_meta)
 //if (!(*a) || !(*b))
   {
    warn("no cols or no rows (%s:%d)", __FILE__,__LINE__);
    return NULL;
   }
 /*printf("ntuples=%d,%d\n", *a,*b);*/
 /*if (!(*a) || !(*b)) { warn("no cols or no rows (%s:%d)", __FILE__,__LINE__); return NULL; }*/
 if (!want_meta)
 {
 result = (CHAR***)smalloc(mem, (*a)*sizeof(CHAR**));
 if (!result) { ERR error("smalloc error\n"); return NULL; }
 for (i=0;i<*a;i++)
   {
    result[i] = (CHAR**)smalloc(mem, (*b)*sizeof(CHAR*));
    if (!result[i])
      {
       for (j=0;j<i;j++) sfree(mem, (void**)(&result[i]));
       ERR error("smalloc_i error!\n");
       return NULL;
      }
   }
 for (i=0;i<*a;i++)
 for (j=0;j<*b;j++)
   {
    result[i][j] = (CHAR*)smalloc(mem, PQgetlength(r,i,j)+1);
    if (!result[i][j])
      {
       mem_free_p3c(mem, &result,*a,*b);
       ERR error("malloc_ij error\n");
       return NULL;
      }
    strcpy(result[i][j], (CHAR*)PQgetvalue(r,i,j));
   }
/*#ifdef DEBUG
  debug("CHAR*** result is (%d:%d):\n",i,j);
  for (i=0;i<*a;i++)
    {
     for (j=0;j<*b;j++) debug("%-15s ", result[i][j]);
     ln();
    }
#endif*/
 return result;
 }
 else
 {
  if ((*b)<=0) return NULL;		/*FIXME is it OK? */
 (*a)++;
 result = (CHAR***)smalloc(mem, (*a)*sizeof(CHAR**));
 if (!result) { ERR error("smalloc error\n"); return NULL; }
 for (i=0;i<*a;i++)
   {
    result[i] = (CHAR**)smalloc(mem, (*b)*sizeof(CHAR*));
    if (!result[i])
      {
       for (j=0;j<i;j++) sfree(mem, (void**)(&result[i]));
       ERR error("smalloc_i error!\n");
       return NULL;
      }
   }
 for (i=0;i<*b;i++)
   {
    tmp = PQfname(r,i);
    result[0][i] = (CHAR*)smalloc(mem, strlen(tmp)+1);
    if (!result[0][i]) { ERR error("smalloc_namej error"); return NULL; }
    strcpy(result[0][i], tmp);
   }
 for (i=1;i<*a;i++)
 for (j=0;j<*b;j++)
   {
    result[i][j] = (CHAR*)smalloc(mem, PQgetlength(r,i-1,j)+1);
    if (!result[i][j])
      {
       mem_free_p3c(mem, &result,*a,*b);
       ERR error("malloc_ij error\n");
       return NULL;
      }
    strcpy(result[i][j], (CHAR*)PQgetvalue(r,i-1,j));
   }
  return result;
 }
}


CHAR*** dmpgres2pc3(const PGresult* r, INT* a, INT* b, INT want_meta, CHAR* file, INT line)
{
 CHAR*** result;
 CHAR* tmp;
 INT i,j;
 debug("%s:%d dmpgres2pc3() called from %s:%d", __FILE__,__LINE__,file,line);
 debug("dmpgres2pc3: %p\n", (void*)r);
 if (!r) return 0;
 i=j=0;
 *a = PQntuples(r);
 *b = PQnfields(r);
 /*if (!(*a) || !(*b)) { warn("no cols or no rows (%s:%d)", __FILE__,__LINE__); return NULL; }*/
 if ((!(*a) || !(*b)) && !want_meta)
 //if (!(*a) || !(*b))
   {
    warn("no cols or no rows (%s:%d)", __FILE__,__LINE__);
    return NULL;
   }
 /*printf("ntuples=%d,%d\n", *a,*b);*/
 if (!want_meta)
 {
 result = (CHAR***)dmmalloc((*a)*sizeof(CHAR**), HERE);
 if (!result) { ERR error("mmalloc error\n"); return NULL; }
 for (i=0;i<*a;i++)
  {
    result[i] = (CHAR**)dmmalloc((*b)*sizeof(CHAR*), HERE);
    if (!result[i])
      {
       for (j=0;j<i;j++)
       {
	 dmfree((void**)(&result[i]), HERE);
       }
       ERR error("mmalloc_i error!\n");
       return NULL;
      }
   }
 for (i=0;i<*a;i++)
 for (j=0;j<*b;j++)
   {
    result[i][j] = (CHAR*)dmmalloc(PQgetlength(r,i,j)+1, HERE);
    if (!result[i][j])
      {
       dmfree_p3c(&result,*a,*b, HERE);
       ERR error("mmalloc_ij error\n");
       return NULL;
      }
    strcpy(result[i][j], (CHAR*)PQgetvalue(r,i,j));
   }
/*#ifdef DEBUG
  debug("CHAR*** result is (%d:%d):\n",i,j);
  for (i=0;i<*a;i++)
    {
     for (j=0;j<*b;j++) debug("%-15s ", result[i][j]);
     ln();
    }
#endif*/
 return result;
 }
 else
 {
  if ((*b)<0) return NULL;		/*FIXME is it OK? */
 (*a)++;
 result = (CHAR***)dmmalloc((*a)*sizeof(CHAR**), HERE);
 if (!result) { ERR error("mmalloc error\n"); return NULL; }
 for (i=0;i<*a;i++)
   {
    result[i] = (CHAR**)dmmalloc((*b)*sizeof(CHAR*), HERE);
    if (!result[i])
      {
       for (j=0;j<i;j++)
         {
	  dmfree((void**)(&result[i]), HERE);
	 }
       ERR error("mmalloc_i error!\n");
       return NULL;
      }
   }
 for (i=0;i<*b;i++)
   {
    tmp = PQfname(r,i);
    result[0][i] = (CHAR*)dmmalloc(strlen(tmp)+1, HERE);
    if (!result[0][i]) { ERR error("mmalloc_namej error"); return NULL; }
    strcpy(result[0][i], tmp);
   }
 for (i=1;i<*a;i++)
 for (j=0;j<*b;j++)
   {
    result[i][j] = (CHAR*)dmmalloc(PQgetlength(r,i-1,j)+1, HERE);
    if (!result[i][j])
      {
       dmfree_p3c(&result,*a,*b, HERE);
       ERR error("mmalloc_ij error\n");
       return NULL;
      }
    strcpy(result[i][j], (CHAR*)PQgetvalue(r,i-1,j));
   }
  return result;
 }
}


CHAR*** mpgres2pc3(const PGresult* r, INT* a, INT* b, INT want_meta)
{
 CHAR*** result;
 CHAR* tmp;
 INT i,j;
 debug("%s:%d mpgres2pc3()", __FILE__,__LINE__);
 debug("mpgres2pc3: %p\n", (void*)r);
 if (!r) return 0;
 i=j=0;
 *a = PQntuples(r);
 *b = PQnfields(r);
 /*if (!(*a) || !(*b)) { warn("no cols or no rows (%s:%d)", __FILE__,__LINE__); return NULL; }*/
 if ((!(*a) || !(*b)) && !want_meta)
 //if (!(*a) || !(*b))
   {
    warn("no cols or no rows (%s:%d)", __FILE__,__LINE__);
    return NULL;
   }
 /*printf("ntuples=%d,%d\n", *a,*b);*/
 if (!want_meta)
 {
 result = (CHAR***)mmalloc((*a)*sizeof(CHAR**));
 if (!result) { ERR error("mmalloc error\n"); return NULL; }
 for (i=0;i<*a;i++)
  {
    result[i] = (CHAR**)mmalloc((*b)*sizeof(CHAR*));
    if (!result[i])
      {
       for (j=0;j<i;j++)
       {
	 mfree((void**)(&result[i]));
       }
       ERR error("mmalloc_i error!\n");
       return NULL;
      }
   }
 for (i=0;i<*a;i++)
 for (j=0;j<*b;j++)
   {
    result[i][j] = (CHAR*)mmalloc(PQgetlength(r,i,j)+1);
    if (!result[i][j])
      {
       mfree_p3c(&result,*a,*b);
       ERR error("mmalloc_ij error\n");
       return NULL;
      }
    strcpy(result[i][j], (CHAR*)PQgetvalue(r,i,j));
   }
/*#ifdef DEBUG
  debug("CHAR*** result is (%d:%d):\n",i,j);
  for (i=0;i<*a;i++)
    {
     for (j=0;j<*b;j++) debug("%-15s ", result[i][j]);
     ln();
    }
#endif*/
 return result;
 }
 else
 {
  if ((*b)<=0) return NULL;		/*FIXME is it OK? */
 (*a)++;
 result = (CHAR***)mmalloc((*a)*sizeof(CHAR**));
 if (!result) { ERR error("mmalloc error\n"); return NULL; }
 for (i=0;i<*a;i++)
   {
    result[i] = (CHAR**)mmalloc((*b)*sizeof(CHAR*));
    if (!result[i])
      {
       for (j=0;j<i;j++)
         {
	  mfree((void**)(&result[i]));
	 }
       ERR error("mmalloc_i error!\n");
       return NULL;
      }
   }
 for (i=0;i<*b;i++)
   {
    tmp = PQfname(r,i);
    result[0][i] = (CHAR*)mmalloc(strlen(tmp)+1);
    if (!result[0][i]) { ERR error("mmalloc_namej error"); return NULL; }
    strcpy(result[0][i], tmp);
   }
 for (i=1;i<*a;i++)
 for (j=0;j<*b;j++)
   {
    result[i][j] = (CHAR*)mmalloc(PQgetlength(r,i-1,j)+1);
    if (!result[i][j])
      {
       mfree_p3c(&result,*a,*b);
       ERR error("mmalloc_ij error\n");
       return NULL;
      }
    strcpy(result[i][j], (CHAR*)PQgetvalue(r,i-1,j));
   }
  return result;
 }
}


CHAR*** pgres2pc3(const PGresult* r, INT* a, INT* b, INT want_meta)
{
 CHAR*** result;
 CHAR* tmp;
 INT i,j;
 debug("%s:%d pgres2pc3()", __FILE__,__LINE__);
 debug("pgres2pc3: %p\n", (void*)r);
 if (!r) return 0;
 i=j=0;
 *a = PQntuples(r);
 *b = PQnfields(r);
 if ((!(*a) || !(*b)) && !want_meta)
 //if (!(*a) || !(*b))
   {
    warn("no cols or no rows (%s:%d)", __FILE__,__LINE__);
    return NULL;
   }
 /*printf("ntuples=%d,%d\n", *a,*b);*/
 if (!want_meta)
 {
 result = (CHAR***)malloc((*a)*sizeof(CHAR**));
 if (!result) { ERR error("malloc error\n"); return NULL; }
 for (i=0;i<*a;i++)
   {
    result[i] = (CHAR**)malloc((*b)*sizeof(CHAR*));
    if (!result[i])
      {
       for (j=0;j<i;j++) free(result[i]);
       ERR error("malloc_i error!\n");
       return NULL;
      }
   }
 for (i=0;i<*a;i++)
 for (j=0;j<*b;j++)
   {
    result[i][j] = (CHAR*)malloc(PQgetlength(r,i,j)+1);
    if (!result[i][j])
      {
       free_p3c(&result,*a,*b);
       ERR error("malloc_ij error\n");
       return NULL;
      }
    strcpy(result[i][j], (CHAR*)PQgetvalue(r,i,j));
   }
/*#ifdef DEBUG
  debug("CHAR*** result is (%d:%d):\n",i,j);
  for (i=0;i<*a;i++)
    {
     for (j=0;j<*b;j++) debug("%-15s ", result[i][j]);
     ln();
    }
#endif*/
 return result;
 }
 else
 {
  if ((*b)<=0) return NULL;		/*FIXME is it OK? */
 (*a)++;
 result = (CHAR***)malloc((*a)*sizeof(CHAR**));
 if (!result) { ERR error("malloc error\n"); return NULL; }
 for (i=0;i<*a;i++)
   {
    result[i] = (CHAR**)malloc((*b)*sizeof(CHAR*));
    if (!result[i])
      {
       for (j=0;j<i;j++) free(result[i]);
       ERR error("malloc_i error!\n");
       return NULL;
      }
   }
 for (i=0;i<*b;i++)
   {
    tmp = PQfname(r,i);
    result[0][i] = (CHAR*)malloc(strlen(tmp)+1);
    if (!result[0][i]) { ERR error("malloc_namej error"); return NULL; }
    strcpy(result[0][i], tmp);
   }
 for (i=1;i<*a;i++)
 for (j=0;j<*b;j++)
   {
    result[i][j] = (CHAR*)malloc(PQgetlength(r,i-1,j)+1);
    if (!result[i][j])
      {
       free_p3c(&result,*a,*b);
       ERR error("malloc_ij error\n");
       return NULL;
      }
    strcpy(result[i][j], (CHAR*)PQgetvalue(r,i-1,j));
   }
  return result;
 }
}


void mem_free_p3c(struct mem_list** mem, CHAR**** p, INT a, INT b)
{
 INT i,j;
 debug("%s:%d mem_free_p3c()", __FILE__,__LINE__);
 debug("mem_free_p3c: ****p=%p, a=%d, b=%d\n",(void*)p,a,b);
 if (!(*p)) return ;
 for (i=0;i<a;i++) if (!(*p)[i]) return ;
 for (i=0;i<a;i++)
 for (j=0;j<b;j++) if ((*p)[i][j]) sfree(mem, (void**)(&(*p)[i][j]));
 for (i=0;i<a;i++) if ((*p)[i])    sfree(mem, (void**)(&(*p)[i]));
 sfree(mem, (void**)(&(*p)));
}


void dmem_free_p3c(struct dmem_list** mem, CHAR**** p, INT a, INT b, CHAR* file, INT line)
{
 INT i,j;
 debug("%s:%d dmem_free_p3c() called from %s:%d", __FILE__,__LINE__,file,line);
 debug("dmem_free_p3c: ****p=%p, a=%d, b=%d\n",(void*)p,a,b);
 if (!(*p)) return ;
 for (i=0;i<a;i++) if (!(*p)[i]) return ;
 for (i=0;i<a;i++)
 for (j=0;j<b;j++) if ((*p)[i][j]) dsfree(mem, (void**)(&(*p)[i][j]), HERE);
 for (i=0;i<a;i++) if ((*p)[i])    dsfree(mem, (void**)(&(*p)[i]), HERE);
 dsfree(mem, (void**)(&(*p)), HERE);
}


void mfree_p3c(CHAR**** p, INT a, INT b)
{
 INT i,j;
 debug("%s:%d mfree_p3c()", __FILE__,__LINE__);
 debug("mfree_p3c: ****p=%p, a=%d, b=%d\n",(void*)p,a,b);
 if (!(*p)) return ;
 for (i=0;i<a;i++) if (!(*p)[i]) return ;
 for (i=0;i<a;i++)
 for (j=0;j<b;j++) if ((*p)[i][j]) mfree((void**)(&(*p)[i][j]));
 for (i=0;i<a;i++) if ((*p)[i])    mfree((void**)(&(*p)[i]));
 mfree((void**)(&(*p)));
}


void dmfree_p3c(CHAR**** p, INT a, INT b, CHAR* file, INT line)
{
 INT i,j;
 debug("%s:%d dmfree_p3c() called from: %s:%d", __FILE__,__LINE__,file,line);
 debug("dmfree_p3c: ****p=%p, a=%d, b=%d\n",(void*)p,a,b);
 if (!(*p)) return ;
 for (i=0;i<a;i++) if (!(*p)[i]) return ;
 for (i=0;i<a;i++)
 for (j=0;j<b;j++) if ((*p)[i][j]) dmfree((void**)(&(*p)[i][j]), HERE);
 for (i=0;i<a;i++) if ((*p)[i])    dmfree((void**)(&(*p)[i]), HERE);
 dmfree((void**)(&(*p)), HERE);
}


void free_p3c(CHAR**** p, INT a, INT b)
{
 INT i,j;
 debug("%s:%d free_p3c()", __FILE__,__LINE__);
 debug("free_p3c: ****p=%p, a=%d, b=%d\n",(void*)p,a,b);
 if (!(*p)) return ;
 for (i=0;i<a;i++) if (!(*p)[i]) return ;
 for (i=0;i<a;i++)
 for (j=0;j<b;j++) if ((*p)[i][j]) free((*p)[i][j]);
 for (i=0;i<a;i++) if ((*p)[i])    free((*p)[i]);
 free(*p);
}

