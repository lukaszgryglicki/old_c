#include <libpq-fe.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <stdarg.h>
#include "mach.h"
#define ln() putchar('\n')
#define ERR  printf("Error occured at line: %d, file %s,  compiled: %s %s\n", __LINE__,__FILE__,__DATE__,__TIME__);
#define M_BEGIN		77
#define M_END		78
#define M_MIDDLE	79
#define M_NONE		80
#define DBNAME "morgoth"
int displayClasses(PGconn*);
int displayStudents(PGconn*);
int displaySubjects(PGconn*);
/* extern int fileno(FILE*); */
static int m_mode = M_BEGIN;

int mgetch()
{
 struct termios st,end;
 int ret;
 tcgetattr(fileno(stdin),&st);
 end = st;
 end.c_lflag &= ~ICANON;
 end.c_lflag &= ~ECHO;
 end.c_cc[VMIN] = 1;
 end.c_cc[VTIME] = 0;
 if (tcsetattr(fileno(stdin),TCSAFLUSH,&end)) return 0;
 st:
 ret = 0;
 ret = getchar();
 if (ret == 10) goto st;
 tcsetattr(fileno(stdin),TCSAFLUSH,&st);
 printf("%c\n",ret);
 return ret;
}


int debug(char* fmt, ...)
{
#ifdef DEBUG
 va_list ap;
 int err;
 va_start(ap,fmt);
 err = vprintf(fmt,ap);
 va_end(ap);
 return err;
#endif
#ifndef DEBUG
 return 0;
#endif
}


void takestring(char* par)
{
 struct termios st,end;
 int i;
 tcgetattr(fileno(stdin),&st);
 end = st;
 end.c_lflag &= ~ICANON;
 end.c_lflag &= ~ECHO;
 end.c_cc[VMIN] = 1;
 end.c_cc[VTIME] = 0;
 if (tcsetattr(fileno(stdin),TCSAFLUSH,&end) != 0) return;
 st:
 i = 0;
 par[0] = 0;
 while (i<1024)
	{
	 par[i] = getchar();
	 if ((i ==0) && (par[i] == 10)) continue;
	 if (par[i] == 10) {par[i] = '\0';goto lab;}
	 if (par[i] == 8)
	   {if (i>0)printf("\b \b"); i--; if (i<0) i=0; continue;}
	 if (par[i] == 27) { printf("Bad character!, retype>\n"); goto st; }
	 printf("%c",par[i]);
	 i++;
	}
 lab:
 if (i == 0) strcpy(par,"");
 putchar('\n');
 tcsetattr(fileno(stdin),TCSAFLUSH,&st);
 debug("debug: got string: %s\n", par);
}


void shortstring(char* par, int LEN)
{
 struct termios st,end;
 int i;
 tcgetattr(fileno(stdin),&st);
 end = st;
 end.c_lflag &= ~ICANON;
 end.c_lflag &= ~ECHO;
 end.c_cc[VMIN] = 1;
 end.c_cc[VTIME] = 0;
 if (tcsetattr(fileno(stdin),TCSAFLUSH,&end) != 0) return;
 st:
 i = 0;
 par[0] = 0;
 while (i<LEN)
	{
	 par[i] = getchar();
	 if ((i ==0) && (par[i] == 10)) continue;
	 if (par[i] == 10) {par[i] = '\0';goto lab;}
	 if (par[i] == 8)
	   {if (i>0)printf("\b \b"); i--; if (i<0) i=0; continue;}
	 if (par[i] == 27) { printf("Bad character!, retype>\n"); goto st; }
	 printf("%c",par[i]);
	 i++;
	}
 lab:
 if (i == 0) strcpy(par,"");
 putchar('\n');
 tcsetattr(fileno(stdin),TCSAFLUSH,&st);
 debug("debug: got string: %s\n", par);
}


int lsprintf(char* str, char* fmt, ...)
{
 va_list ap;
 int err;
 char* nfmt;
 int i,j;
 debug("debug: lsprintf query:\n\"%s\"\n", fmt);
 nfmt = (char*)malloc(strlen(fmt)+200);
 j=0;
 for (i=0;i<strlen(fmt);i++)
   {
    if (fmt[i]=='|')
      {
       if (m_mode==M_BEGIN)
         {
          nfmt[j]   = '%';
	  nfmt[j+1] = 's';
	  nfmt[j+2] = '%';
	  nfmt[j+3] = '%';
	  j += 4;
         }
       else if (m_mode==M_END)
         {
          nfmt[j]   = '%';
	  nfmt[j+1] = '%';
	  nfmt[j+2] = '%';
	  nfmt[j+3] = 's';
	  j += 4;
         }
       else if (m_mode==M_MIDDLE)
         {
          nfmt[j]   = '%';
	  nfmt[j+1] = '%';
	  nfmt[j+2] = '%';
	  nfmt[j+3] = 's';
          nfmt[j+4] = '%';
	  nfmt[j+5] = '%';
	  j += 6;
         }
       else if (m_mode==M_NONE)
         {
	  nfmt[j]   = '%';
	  nfmt[j+1] = 's';
	  j += 2;
         }
      }
     else
       {
	nfmt[j] = fmt[i];
	j++;
       }
   }
 nfmt[j] = 0;
 va_start(ap,nfmt);
 err = vsprintf(str,nfmt,ap);
 va_end(ap);
 free(nfmt);
 return err;
}


PGconn* connect_db(char* arg)
{
 PGconn* conn;
 char* strarg;
 conn=0;
 strarg = (char*)malloc(strlen(arg)+32);
 debug("\ndebug: connect to database %s\n", arg);
 sprintf(strarg,"dbname=%s", arg);
 conn = PQconnectdb(strarg);
 if (PQstatus(conn) == CONNECTION_BAD)
   {
    printf("\nFATAL >>> Cannot connect to DB:%s\n", arg);
    ERR
    return NULL;
   }
 free(strarg);
 return conn;
}


void clear_result(PGresult** r)
{
 if (*r) PQclear(*r);
 debug("debug: clear result\n");
 *r=0;
}


void disconnect_db(PGconn** c)
{
 debug("debug: disconnect DB\n");
 if (*c) PQfinish(*c);
 *c = NULL;
}


PGresult* execute_silent_query(PGconn* c,  char* query)
{
 PGresult* r;
 int stat;
 debug("debug: SQLquery: \"%s\"\n", query);
 if (!query)
   {
    debug("NULL query passed\n");
    return NULL;
   }
 r= PQexec(c, query);
 if (!r)
   {
    debug("FATAL error: PQresult is NULL %s\n", PQerrorMessage(c));
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


PGresult* execute_query(PGconn* c,  char* query)
{
 PGresult* r;
 int stat;
 debug("debug: SQLquery: \"%s\"\n", query);
 r = NULL;
 if (!query)
   {
    ERR printf("NULL query passed\n");
    return NULL;
   }
 r= PQexec(c, query);
 if (!r)
   {
    ERR printf("FATAL error: PQresult is NULL %s\n", PQerrorMessage(c));
    return NULL;
   }
 stat = PQresultStatus(r);
 if (stat!=PGRES_COMMAND_OK && stat!=PGRES_TUPLES_OK)
   {
    ERR printf("PQexec error: %s %s",PQcmdStatus(r),PQresultErrorMessage(r));
    printf("Failed query was: \"%s\"\n", query);
    return NULL;
   }
 return r;
}

void free_p3c(char****,int,int);

char*** pgres2pc3(const PGresult* r, int* a, int* b)
{
 char*** result;
 int i,j;
 if (!r) return 0;
 i=j=0;
 debug("debug: pgres2pc3: %p\n", (void*)r);
 *a = PQntuples(r);
 *b = PQnfields(r);
 /*printf("ntuples=%d,%d\n", *a,*b);*/
 result = (char***)malloc((*a)*sizeof(char**));
 if (!result) { ERR printf("malloc error\n"); return NULL; }
 for (i=0;i<*a;i++)
   {
    result[i] = (char**)malloc((*b)*sizeof(char*));
    if (!result[i])
      {
       for (j=0;j<i;j++) free(result[i]);
       ERR printf("malloc_i error!\n");
       return NULL;
      }
   }
 for (i=0;i<*a;i++)
 for (j=0;j<*b;j++)
   {
    result[i][j] = (char*)malloc(PQgetlength(r,i,j)+1);
    if (!result[i][j])
      {
       free_p3c(&result,*a,*b);
       ERR printf("malloc_ij error\n");
       return NULL;
      }
    strcpy(result[i][j], (char*)PQgetvalue(r,i,j));
   }
#ifdef DEBUG
	    /* TOO MANY THRASHES ONSCREEN */
  debug("debug: char*** result is (%d:%d):\n",i,j);
  /* for (i=0;i<*a;i++)
    {
      for (j=0;j<*b;j++) debug("%-15s ", result[i][j]);
      ln();
    } */
#endif
 return result;
}


void free_p3c(char**** p, int a, int b)
{
 int i,j;
 debug("free_p3c: ****p=%p, a=%d, b=%d\n",(void*)p,a,b);
 if (!(*p)) return ;
 for (i=0;i<a;i++) if (!(*p)[i]) return ;
 for (i=0;i<a;i++)
 for (j=0;j<b;j++) if ((*p)[i][j]) free((*p)[i][j]);
 for (i=0;i<a;i++) if ((*p)[i])    free((*p)[i]);
 free(*p);
}


void print_results(char*** res, int a, int b)
{
 int i,j;
 ln();
 if (!res) return;
 for (i=0;i<a;i++)
   {
    for (j=0;j<b;j++) printf("%-16s ", res[i][j]);
    ln();
   }
 ln();
}


int getDistinctStudent(int* id, PGconn* connection)
{
 char imie[1025];
 char nazwisko[1025];
 char klasa[16];
 int a,b,i,idklasy;
 char*** out;
 char* check;
 PGresult* pgres;
 pgres=0;
 out=0;
 check=0;
 idklasy=-1;
 printf("Available classes:\n");
 i=displayClasses(connection);
 if (i<0) { printf("No classes - no students.\n"); return 1; }
 pgres = execute_query(connection, "SELECT imie,nazwisko,klasa.idklasy,nazwa FROM klasa,uczen WHERE uczen.idklasy=klasa.idklasy ORDER BY nazwisko");
 if (!pgres)
   {
    printf("SQL error, table 'uczen' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No students\n");
    return 1;
   }
 printf("Available students:\n");
 for (i=0;i<a;i++) printf("%-30s\t %-25s\t %s\n", out[i][1],out[i][0],out[i][3]);
 ln();
 free_p3c(&out,a,b);
 printf("\nStudent's surname (or part): ");
 takestring(nazwisko);
 check = (char*)malloc(270);
 if (!check) { ERR printf("malloc failed.\n"); return 1; }
 lsprintf(check,"SELECT imie,nazwisko,klasa.idklasy,nazwa from uczen,klasa WHERE nazwisko LIKE '|' AND klasa.idklasy=uczen.idklasy ORDER BY nazwisko", nazwisko);
 pgres = execute_query(connection, check);
 free(check);
 if (!pgres)
   {
    printf("SQL Error, table 'uczen' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No such surname: %s!\n",nazwisko);
    return 1;
   }
 if (a>1)
   {
    free_p3c(&out,a,b);
    printf("Selection is ambigiuous, possibilities are:\n");
    for (i=0;i<a;i++) printf("%-30s\t %-25s\t %s\n", out[i][1],out[i][0],out[i][3]);
    ln();
    printf("Student's (or part): ");
    takestring(imie);
    check = (char*)malloc(strlen(imie)+200);
    if (!check) { ERR printf("malloc failed.\n"); return 1; }
    lsprintf(check,"SELECT imie,nazwisko,klasa.idklasy,nazwa FROM uczen,klasa WHERE nazwisko LIKE '|' AND imie LIKE '|' AND klasa.idklasy=uczen.idklasy ORDER BY nazwisko", nazwisko,imie);
    pgres = execute_query(connection, check);
    free(check);
    if (!pgres)
      {
       printf("SQL Error, table 'uczen' not exists?\n");
    printf("%d\n", __LINE__);
       return 1;
      }
    out = pgres2pc3(pgres, &a, &b);
    clear_result(&pgres);
    if (!a)
      {
       free_p3c(&out,a,b);
       printf("No such student: %s %s!\n",nazwisko,imie);
       return 1;
      }
    if (a>1)
      {
       free_p3c(&out,a,b);
       printf("Selection ambiguous, possibilities are:\n");
       for (i=0;i<a;i++) printf("%-30s\t %-25s\t %s\n", out[i][1],out[i][0],out[i][3]);
       ln();
       printf("Now You have to give class name, because name & surname are ambiguous...\n");
       displayClasses(connection);
       printf("Class name (or part): ");
       shortstring(klasa, 8);
       check = (char*)malloc(strlen(imie)+strlen(nazwisko)+strlen(klasa)+200);
       if (!check) { ERR printf("malloc failed!\n"); return 1; }
       lsprintf(check,"SELECT nazwa,idklasy from klasa WHERE nazwa LIKE '|'", klasa);
       pgres = execute_query(connection, check);
       free(check);
       if (!pgres)
         {
          printf("SQL Error, table 'klasa' not exist?\n");
    printf("%d\n", __LINE__);
          return 1;
         }
       out = pgres2pc3(pgres, &a, &b);
       clear_result(&pgres);
       if (!a)
         {
          free_p3c(&out,a,b);
          printf("No such class!\n");
          return 1;
         }
      if (a>1)
         {
          free_p3c(&out,a,b);
          printf("Selection is ambiguous, %d classes match!\n",a);
          for (i=0;i<a;i++) printf("%s, ", out[i][0]);
          ln();
          return 1;
         }
       idklasy = atoi(out[0][1]);
       if (idklasy<0) { free_p3c(&out,a,b) ; printf("Error computing 'idklasy'!\n"); return 1; }
       printf("Selected class: %s\n", out[0][0]);
       free_p3c(&out,a,b);
       check = (char*)malloc(strlen(nazwisko)+strlen(imie)+300);
       if (!check) { ERR printf("malloc failed.\n"); return 1; }
       lsprintf(check,"SELECT iducznia FROM uczen WHERE nazwisko LIKE '|' AND imie LIKE '|' AND idklasy=%d",nazwisko,imie,idklasy);
       pgres = execute_query(connection, check);
       free(check);
       if (!pgres)
         {
          printf("SQL Error, tablle 'uczen' not exists?\n");
    printf("%d\n", __LINE__);
          return 1;
         }
       out = pgres2pc3(pgres, &a, &b);
       clear_result(&pgres);
       if (!a)
         {
          free_p3c(&out,a,b);
          printf("No such student!\n");
          return 1;
         }
       sscanf(out[0][0], "%d", id);
       free_p3c(&out,a,b);
       return 0;
      }
    free_p3c(&out,a,b);
    check = (char*)malloc(strlen(nazwisko)+strlen(imie)+300);
    if (!check) { ERR printf("malloc failed.\n"); return 1; }
    lsprintf(check,"SELECT iducznia FROM uczen WHERE nazwisko LIKE '|' AND imie LIKE '|'",nazwisko,imie);
    pgres = execute_query(connection, check);
    free(check);
    if (!pgres)
      {
       printf("SQL Error, table 'uczen' not exists?\n");
    printf("%d\n", __LINE__);
       return 1;
      }
    out = pgres2pc3(pgres, &a, &b);
    clear_result(&pgres);
    if (!a)
      {
       free_p3c(&out,a,b);
       printf("No such student!\n");
       return 1;
      }
    sscanf(out[0][0], "%d", id);
    free_p3c(&out,a,b);
    return 0;
   }
 free_p3c(&out,a,b);
 check = (char*)malloc(strlen(nazwisko)+strlen(imie)+280);
 if (!check) { ERR ERR printf("malloc failed.\n"); return 1; }
 lsprintf(check,"SELECT iducznia FROM uczen WHERE nazwisko LIKE '|'",nazwisko);
 pgres = execute_query(connection, check);
 free(check);
 if (!pgres)
   {
     printf("SQL Error, table 'uczen' not exists?\n");
    printf("%d\n", __LINE__);
     return 1;
    }
  out = pgres2pc3(pgres, &a, &b);
  clear_result(&pgres);
  if (!a)
    {
     free_p3c(&out,a,b);
     printf("No such student!\n");
     return 1;
    }
 sscanf(out[0][0], "%d", id);
 free_p3c(&out,a,b);
 return 0;
}


int prepareUser(char** userCmd)
{
 char tmp[1025];
 int len;
 printf("SQL statement: ");
 takestring(tmp);
 len = strlen(tmp);
 *userCmd = (char*)malloc(len+2);
 if (!*userCmd) { ERR printf("malloc failed!\n"); return 1; }
 strcpy(*userCmd, tmp);
 return 0;
}


int addStudentsToClass(PGconn* connection)
{
 char imie[1025];
 char nazwisko[1025];
 char klasa[16];
 int a,b,i,idklasy;
 char*** out;
 char* check;
 char* userCmd;
 PGresult* pgres;
 pgres=0;
 out=0;
 check=0;
 idklasy=-1;
 userCmd=0;
 pgres = execute_query(connection, "SELECT imie,nazwisko,klasa.idklasy,nazwa FROM klasa,uczen WHERE uczen.idklasy=klasa.idklasy ORDER BY nazwisko");
 if (!pgres)
   {
    printf("SQL Error, table 'uczen' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    printf("Adding first student...\n");
   }
 printf("Available students:\n");
 for (i=0;i<a;i++) printf("%-30s\t %-25s\t %s\n", out[i][1],out[i][0],out[i][3]);
 ln();
 free_p3c(&out,a,b);
 printf("Choose class for student:");
 i=displayClasses(connection);
 if (i<=0) { printf("\nNo defined classes!\n"); return 1; }
 printf("Class name (or part): ");
 shortstring(klasa, 8);
 check = (char*)malloc(400);
 if (!check) { ERR printf("malloc failed!\n"); return 1; }
 lsprintf(check,"SELECT nazwa,idklasy from klasa WHERE nazwa LIKE '|'", klasa);
 pgres = execute_query(connection, check);
 free(check);
 if (!pgres)
   {
    printf("SQL Error, table 'klasa' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No such class!\n");
    return 1;
   }
 if (a>1)
   {
    free_p3c(&out,a,b);
    printf("Selection is ambiguous, %d classes match!\n",a);
    for (i=0;i<a;i++) printf("%s, ", out[i][0]);
    ln();
    return 1;
   }
 idklasy = atoi(out[0][1]);
 if (idklasy<0) { free_p3c(&out,a,b) ; printf("Error computing idklasy!\n"); return 1; }
 printf("Selected class: %s\n", out[0][0]);
 free_p3c(&out,a,b);
 strcpy(imie,"(dummy)");
 while (1)
  {
   printf("Name (type 'stop' to finish): ");
   takestring(imie);
   if (!strcmp(imie,"stop")) break;
   printf("Surname: ");
   takestring(nazwisko);
   userCmd = (char*)malloc(strlen(imie)+strlen(nazwisko)+strlen(klasa)+200);
   sprintf(userCmd,"INSERT INTO uczen VALUES ('%s','%s',%d)", imie,nazwisko,idklasy);
   pgres=0;
   out=0;
   check = (char*)malloc(512);
   if (!check) { printf("check malloc failed!\n"); return 1; }
   sprintf(check,"SELECT imie,nazwisko,klasa.idklasy,nazwa FROM uczen,klasa "
	       "WHERE imie='%s' AND nazwisko='%s' AND klasa.idklasy=%d AND klasa.idklasy=uczen.idklasy", imie,nazwisko,idklasy);
   pgres = execute_query(connection, check);
   free(check);
   if (!pgres)
     {
      printf("SQL Error, table 'uczen' not exists?\n");
    printf("%d\n", __LINE__);
      free(userCmd);
      return 1;
     }
   out = pgres2pc3(pgres, &a, &b);
   clear_result(&pgres);
   if (a)
     {
      free_p3c(&out,a,b);
      printf("Such student: %s,%s already exists in this class!\n",imie,nazwisko);
      free(userCmd);
      return 1;
     }
   free_p3c(&out,a,b);
   pgres = execute_query(connection, userCmd);
   if (!pgres)
     {
      printf("Error adding student.\n");
    printf("%d\n", __LINE__);
      free(userCmd);
      userCmd = 0;
      return 1;
     }
   clear_result(&pgres);
   free(userCmd);
   userCmd = 0;
  }
 return 0;
}


int getDistinctClass(int* idklasy, PGconn* connection)
{
 int i;
 char* check;
 PGresult* pgres;
 char*** out;
 char klasa[16];
 int a,b;
 i=displayClasses(connection);
 if (i<=0) { printf("\nNo defined classes!\n"); return 1; }
 printf("Class name (or part): ");
 shortstring(klasa,15);
 check = (char*)malloc(strlen(klasa)+200);
 if (!check) { ERR printf("malloc failed!\n"); return 1; }
 lsprintf(check,"SELECT nazwa,idklasy from klasa WHERE nazwa LIKE '|'", klasa);
 pgres = execute_query(connection, check);
 free(check);
 if (!pgres)
   {
    printf("SQL Error, table 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No such class!\n");
    return 1;
   }
 if (a>1)
   {
    free_p3c(&out,a,b);
    printf("Selection is ambiguous, %d classes match!\n",a);
    for (i=0;i<a;i++) printf("%s, ", out[i][0]);
    ln();
    return 1;
   }
 *idklasy = atoi(out[0][1]);
 free_p3c(&out,a,b);
 if (*idklasy<0) { printf("Error computing idklasy!\n"); return 1; }
 return 0;
}


int getDistinctSubject(int* idprzedmiotu, PGconn* connection)
{
 int i;
 char* check;
 PGresult* pgres;
 char*** out;
 char przedmiot[1025];
 int a,b;
 i=displaySubjects(connection);
 if (i<=0) { printf("\nNo defined subjects!\n"); return 1; }
 printf("Subject name (or part): ");
 takestring(przedmiot);
 check = (char*)malloc(strlen(przedmiot)+200);
 if (!check) { ERR printf("malloc failed!\n"); return 1; }
 lsprintf(check,"SELECT nazwa,idprzedmiotu from przedmiot WHERE nazwa LIKE '|'", przedmiot);
 pgres = execute_query(connection, check);
 free(check);
 if (!pgres)
   {
    printf("SQL Error, table 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No such subject!\n");
    return 1;
   }
 if (a>1)
   {
    free_p3c(&out,a,b);
    printf("Selection is ambiguous, %d subjects match!\n",a);
    for (i=0;i<a;i++) printf("%s, ", out[i][0]);
    ln();
    return 1;
   }
 *idprzedmiotu = atoi(out[0][1]);
 free_p3c(&out,a,b);
 if (*idprzedmiotu<0) { printf("Error computing idprzedmiotu!\n"); return 1; }
 return 0;
}


int removeMark(char** userCmd, PGconn* connection)
{
 int a,b,i;
 char*** out;
 int iducznia,idprzedmiotu;
 PGresult* pgres;
 pgres=0;
 out=0;
 iducznia=idprzedmiotu=-1;
 pgres = execute_query(connection, "SELECT ocena FROM ocena ORDER BY ocena");
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    printf("No marks.\n");
    return 1;
   }
 free_p3c(&out,a,b);
 printf("Select student (type in nonexisting to interrupt):\n");
 i=getDistinctStudent(&iducznia,connection);
 if (i) { printf("Error selectiong student.\n"); return 1; }
 printf("Select subject:\n");
 i=getDistinctSubject(&idprzedmiotu,connection);
 if (i) { printf("Error selecting subject.\n"); return 1; }
 *userCmd = (char*)malloc(400);
 sprintf(*userCmd,"SELECT ocena,czyliczona FROM ocena "
	       "WHERE iducznia=%d AND idprzedmiotu=%d", iducznia,idprzedmiotu);
 pgres = execute_query(connection, *userCmd);
 free(*userCmd);
 *userCmd = 0;
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    free(*userCmd);
    *userCmd = 0;
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    printf("This student doesnt have mark from such subject\n");
    return 1;
   }
 if (a>1)
   {
    free_p3c(&out,a,b);
    printf("This student has more than one mark from this subject, MEANS SERIOUS ERROR!\n");
    ERR
    free(*userCmd);
    *userCmd = 0;
    return 1;
   }
 *userCmd = (char*)malloc(400);
 if (!*userCmd) { ERR printf("malloc failled!\n"); return 1; }
 sprintf(*userCmd,"DELETE FROM ocena WHERE iducznia=%d AND idprzedmiotu=%d",iducznia,idprzedmiotu);
 printf("Executing query:\n\"%s\"\n", *userCmd);
 return 0;
}


int getClassByID(PGconn* connection, int idklasy, char* nazwa)
{
 int a,b;
 char*** out;
 char* check;
 PGresult* pgres;
 pgres=0;
 out=0;
 check = (char*)malloc(220);
 sprintf(check,"SELECT nazwa FROM klasa WHERE idklasy=%d", idklasy);
 pgres = execute_query(connection, check);
 if (!pgres)
   {
    printf("SQL Error, table 'klasa' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    printf("No such class\n");
    return 1;
   }
 strcpy(nazwa,out[0][0]);
 free_p3c(&out,a,b);
 return 0;
}


int getSubjectByID(PGconn* connection, int idprzedmiotu, char* nazwa)
{
 int a,b;
 char*** out;
 char* check;
 PGresult* pgres;
 pgres=0;
 out=0;
 check = (char*)malloc(220);
 sprintf(check,"SELECT nazwa FROM przedmiot WHERE idprzedmiotu=%d", idprzedmiotu);
 pgres = execute_query(connection, check);
 if (!pgres)
   {
    printf("SQL Error, table 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    printf("No such subject\n");
    return 1;
   }
 strcpy(nazwa,out[0][0]);
 free_p3c(&out,a,b);
 return 0;
}


int getStudentByID(PGconn* connection, int iducznia, char* imie, char* nazwisko, char* klasa)
{
 int a,b;
 char*** out;
 char* check;
 PGresult* pgres;
 pgres=0;
 out=0;
 check = (char*)malloc(400);
 sprintf(check,"SELECT imie,nazwisko,klasa.nazwa FROM uczen,klasa WHERE iducznia=%d AND uczen.idklasy=klasa.idklasy", iducznia);
 pgres = execute_query(connection, check);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    printf("No such student\n");
    return 1;
   }
 strcpy(imie,out[0][0]);
 strcpy(nazwisko,out[0][1]);
 strcpy(klasa,out[0][2]);
 free_p3c(&out,a,b);
 return 0;
}


int seeMarksAll(char** userCmd, PGconn* connection)
{
 int a,b,i,err;
 char*** out;
 int iducznia,idprzedmiotu;
 char ocena[16];
 char licz[16];
 char przedmiot[256];
 char imie[256],nazwisko[256],klasa[16];
 PGresult* pgres;
 pgres=0;
 out=0;
 iducznia=idprzedmiotu=-1;
 pgres = execute_query(connection, "SELECT ocena FROM ocena ORDER BY ocena");
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    printf("No marks.\n");
    return 1;
   }
 free_p3c(&out,a,b);
 *userCmd = (char*)malloc(512);
 sprintf(*userCmd,"SELECT ocena,czyliczona,uczen.iducznia,idprzedmiotu,nazwisko FROM ocena,uczen WHERE ocena.iducznia=uczen.iducznia ORDER by nazwisko");
 pgres = execute_query(connection, *userCmd);
 free(*userCmd);
 *userCmd = 0;
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    free(*userCmd);
    *userCmd = 0;
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    printf("No marks!\n");
    return 1;
   }
 for (i=0;i<a;i++)
 {
  strcpy(ocena,out[i][0]);
  strcpy(licz,out[i][1]);
  err=getStudentByID(connection,atoi(out[i][2]),imie,nazwisko,klasa);
  if (err) { printf("Error computing iducznia\n"); return 1; }
  err=getSubjectByID(connection,atoi(out[i][3]),przedmiot);
  if (err) { printf("Error computing idprzedmiotu\n"); return 1; }
  if (!strcmp(licz,"f")) strcpy(licz,"nie");
  else strcpy(licz,"tak");
  printf("%-22s %-14s,%-8s,%-20s:%-5s(%-3s)\n",nazwisko,imie,klasa,przedmiot,ocena,licz);
 }
 free_p3c(&out,a,b);
 return 0;
}


int seeMarks(char** userCmd, PGconn* connection)
{
 int a,b,i;
 char*** out;
 int iducznia,idprzedmiotu;
 char ocena[16];
 char licz[16];
 char przedmiot[256];
 char imie[256],nazwisko[256],klasa[16];
 PGresult* pgres;
 pgres=0;
 out=0;
 iducznia=idprzedmiotu=-1;
 pgres = execute_query(connection, "SELECT ocena FROM ocena ORDER BY ocena");
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    printf("No marks.\n");
    return 1;
   }
 free_p3c(&out,a,b);
 printf("Select student (typein nonexisting to interrupt):\n");
 i=getDistinctStudent(&iducznia,connection);
 if (i) { printf("Error selecting student.\n"); return 1; }
 printf("Select subject:\n");
 i=getDistinctSubject(&idprzedmiotu,connection);
 if (i) { printf("Error selecting subject.\n"); return 1; }
 *userCmd = (char*)malloc(400);
 sprintf(*userCmd,"SELECT ocena,czyliczona FROM ocena "
	       "WHERE iducznia=%d AND idprzedmiotu=%d", iducznia,idprzedmiotu);
 pgres = execute_query(connection, *userCmd);
 free(*userCmd);
 *userCmd = 0;
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    free(*userCmd);
    *userCmd = 0;
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    printf("This student doesn't have mark from this subject.\n");
    return 1;
   }
 if (a>1)
   {
    free_p3c(&out,a,b);
    printf("This student has more than one mark from this subject, MEANS SERIOUS ERROR\n");
    ERR
    free(*userCmd);
    *userCmd = 0;
    return 1;
   }
 strcpy(licz,out[0][1]);
 strcpy(ocena,out[0][0]);
 free_p3c(&out,a,b);
 i=getStudentByID(connection,iducznia,imie,nazwisko,klasa);
 if (i) { printf("Error computing iducznia\n"); return 1; }
 i=getSubjectByID(connection,idprzedmiotu,przedmiot);
 if (i) { printf("Error computing idprzedmiotu\n"); return 1; }
 if (!strcmp(licz,"f")) strcpy(licz,"nie");
 else strcpy(licz,"tak");
 printf("Mark of %s %s from %s from %s is %s, taken to avg: %s\n",nazwisko,imie,klasa,przedmiot,ocena,licz);
 return 0;
}


int seeMarksByStudents(char** userCmd, PGconn* connection)
{
 int a,b,i,err;
 char*** out;
 int iducznia,idprzedmiotu;
 char ocena[16];
 char licz[16];
 char przedmiot[256];
 char imie[256],nazwisko[256],klasa[16];
 PGresult* pgres;
 pgres=0;
 out=0;
 iducznia=idprzedmiotu=-1;
 pgres = execute_query(connection, "SELECT ocena FROM ocena ORDER BY ocena");
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    printf("No marks.\n");
    return 1;
   }
 free_p3c(&out,a,b);
 *userCmd = (char*)malloc(512);
 printf("Select student (typein nonexisting to interrupt):\n");
 i=getDistinctStudent(&iducznia,connection);
 if (i) { printf("Error selecting student.\n"); return 1; }
 sprintf(*userCmd,"SELECT ocena,czyliczona,iducznia,idprzedmiotu FROM ocena WHERE iducznia=%d ORDER by ocena",iducznia);
 pgres = execute_query(connection, *userCmd);
 free(*userCmd);
 *userCmd = 0;
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    free(*userCmd);
    *userCmd = 0;
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    printf("No marks!\n");
    return 1;
   }
 for (i=0;i<a;i++)
 {
  strcpy(ocena,out[i][0]);
  strcpy(licz,out[i][1]);
  err=getStudentByID(connection,atoi(out[i][2]),imie,nazwisko,klasa);
  if (err) { printf("Error computing iducznia\n"); return 1; }
  err=getSubjectByID(connection,atoi(out[i][3]),przedmiot);
  if (err) { printf("Error computing idprzedmiotu\n"); return 1; }
  if (!strcmp(licz,"f")) strcpy(licz,"nie");
  else strcpy(licz,"tak");
  printf("%-22s %-14s,%-8s,%-20s:%-5s(%-3s)\n",nazwisko,imie,klasa,przedmiot,ocena,licz);
 }
 free_p3c(&out,a,b);
 return 0;
}


int seeMarksBySubject(char** userCmd, PGconn* connection)
{
 int a,b,i,err;
 char*** out;
 int iducznia,idprzedmiotu;
 char ocena[16];
 char licz[16];
 char przedmiot[256];
 char imie[256],nazwisko[256],klasa[16];
 PGresult* pgres;
 pgres=0;
 out=0;
 iducznia=idprzedmiotu=-1;
 pgres = execute_query(connection, "SELECT ocena FROM ocena ORDER BY ocena");
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    printf("No marks.\n");
    return 1;
   }
 free_p3c(&out,a,b);
 *userCmd = (char*)malloc(512);
 printf("Select subject (nonexisting to interrupt)\n");
 i=getDistinctSubject(&idprzedmiotu,connection);
 if (i) { printf("Error selecting subject.\n"); return 1; }
 sprintf(*userCmd,"SELECT ocena,czyliczona,iducznia,idprzedmiotu FROM ocena WHERE idprzedmiotu=%d ORDER by ocena",idprzedmiotu);
 pgres = execute_query(connection, *userCmd);
 free(*userCmd);
 *userCmd = 0;
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    free(*userCmd);
    *userCmd = 0;
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    printf("No marks!\n");
    return 1;
   }
 for (i=0;i<a;i++)
 {
  strcpy(ocena,out[i][0]);
  strcpy(licz,out[i][1]);
  err=getStudentByID(connection,atoi(out[i][2]),imie,nazwisko,klasa);
  if (err) { printf("Error computing iducznia\n"); return 1; }
  err=getSubjectByID(connection,atoi(out[i][3]),przedmiot);
  if (err) { printf("Error computing idprzedmiotu\n"); return 1; }
  if (!strcmp(licz,"f")) strcpy(licz,"nie");
  else strcpy(licz,"tak");
  printf("%-22s %-14s,%-8s,%-20s:%-5s(%-3s)\n",nazwisko,imie,klasa,przedmiot,ocena,licz);
 }
 free_p3c(&out,a,b);
 return 0;
}


int modifyMark(char** userCmd, PGconn* connection)
{
 int a,b,i;
 char czl[5];
 char tmp[16];
 char*** out;
 char* check;
 int iducznia,idprzedmiotu;
 int czyliczona;
 float wartosc,stara;
 PGresult* pgres;
 pgres=0;
 out=0;
 check=0;
 czyliczona='t';
 iducznia=idprzedmiotu=-1;
 pgres = execute_query(connection, "SELECT ocena FROM ocena ORDER BY ocena");
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    printf("No marks to modify.\n");
    return 1;
   }
 free_p3c(&out,a,b);
 printf("Select student (typein nonexisting to interrupt):\n");
 i=getDistinctStudent(&iducznia,connection);
 if (i) { printf("Error selecting student.\n"); return 1; }
 printf("Select subject:\n");
 i=getDistinctSubject(&idprzedmiotu,connection);
 if (i) { printf("Error selecting subject.\n"); return 1; }
 pgres=0;
 out=0;
 check = (char*)malloc(512);
 if (!check) { printf("check malloc failed!\n"); return 1; }
 sprintf(check,"SELECT ocena,czyliczona FROM ocena "
	       "WHERE iducznia=%d AND idprzedmiotu=%d", iducznia,idprzedmiotu);
 pgres = execute_query(connection, check);
 free(check);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    free(*userCmd);
    *userCmd = 0;
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    printf("This student doesn't have mark from this subject.\n");
    return 1;
   }
 if (a>1)
   {
    free_p3c(&out,a,b);
    printf("This student has more than one mark from this subject, MEANS SERIOUS ERROR\n");
    ERR
    free(*userCmd);
    *userCmd = 0;
   }
 stara = -1.0;
 sscanf(out[0][0],"%f",&stara);
 strcpy(czl,out[0][1]);
 if (!strcmp(czl,"f")) strcpy(czl,"n");
 free_p3c(&out,a,b);
 if (stara<0.0) { printf("Error choosing mark to modification.\n"); return 1; }
 printf("Typein new mark instead of %f,in avg:%s (0 to cancel): ",stara,czl);
 shortstring(tmp, 15);
 wartosc = -1.0;
 sscanf(tmp, "%f", &wartosc);
 if (wartosc<1.0 || wartosc>6.0) { printf("This is not a correct mark.\n"); return 1; }
 czlicz:
 printf("\nShould mark be used to compute avg (t=YES, n=NO)? (t/n): ");
 czyliczona = mgetch();
 ln();
 if (czyliczona>='A' && czyliczona<='Z') czyliczona+=0x20;
 if (czyliczona!='n' && czyliczona!='t') { printf("Bad answer!\n"); goto czlicz; }
 *userCmd = (char*)malloc(400);
 if (!*userCmd) { ERR printf("malloc failed!\n"); return 1; }
 sprintf(*userCmd,"UPDATE ocena SET ocena=%f, czyliczona='%c' WHERE iducznia=%d AND idprzedmiotu=%d", wartosc,czyliczona,iducznia,idprzedmiotu);
 return 0;
}


int addMarkByClass(char** userCmd, PGconn* connection)
{
 int a,b,i,err,ia,ib,j,iia,iib;
 char przedmiot[1025];
 char imie[125];
 char nazwisko[125];
 char klasa[16];
 char tmp[32];
 char*** out, ***iout, ***iiout;
 char* check;
 char* icheck;
 int iducznia,idprzedmiotu,idklasy;
 int czyliczona;
 float wartosc;
 PGresult* pgres;
 pgres=0;
 out=0;
 check=0;
 czyliczona='t';
 iducznia=idprzedmiotu=-1;
 pgres = execute_query(connection, "SELECT ocena FROM ocena ORDER BY ocena");
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a) printf("Adding first mark...\n");
 free_p3c(&out,a,b);
 i=getDistinctClass(&idklasy,connection);
 if (i) { printf("Error selecting class.\n"); return 1; }
 i=getClassByID(connection, idklasy, klasa);
 if (i) { printf("Error selecting class.\n"); return 1; }
 printf("Adding to class %s\n", klasa);
 check = (char*)malloc(strlen(klasa)+200);
 if (!check) { ERR printf("malloc failed!\n"); return 1; }
 sprintf(check,"SELECT idprzedmiotu,nazwa FROM przedmiot ORDER BY nazwa");
 pgres = execute_query(connection, check);
 free(check);
 if (!pgres)
   {
    printf("SQL Error, table 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No subjects!\n");
    return 1;
   }
 for (i=0;i<a;i++)
   {
    again:
    idprzedmiotu = atoi(out[i][0]);
    err=getSubjectByID(connection, idprzedmiotu, przedmiot);
    if (err) { printf("Error choosing subject name!\n"); free_p3c(&out,a,b); return 1; }
    printf("Subject: %s, in avg(t), skipped in avg(n), no marks(b), quit(k): (t/n/b/k) ",przedmiot);
    err = mgetch();
    ln();
    if (err>='A' && err<='Z') err+=0x20;
    if (err=='k') { free_p3c(&out,a,b); return 0; }
    else if (err=='b') continue;
    czyliczona=err;
    if (czyliczona!='t' && czyliczona!='n') goto again;
    if (czyliczona=='n') czyliczona='f';
    icheck = (char*)malloc(400);
    if (!icheck) { ERR printf("imalloc failed!\n"); return 1; }
    sprintf(icheck,"SELECT iducznia,nazwisko FROM uczen WHERE idklasy=%d ORDER BY nazwisko", idklasy);
    pgres = execute_query(connection, icheck);
    free(icheck);
    if (!pgres)
       {
        printf("SQL Error, table 'uczen' not exists?\n");
        printf("%d\n", __LINE__);
	free_p3c(&out,a,b);
        return 1;
       }
    iout = pgres2pc3(pgres, &ia, &ib);
    clear_result(&pgres);
    if (!ia)
      {
       free_p3c(&iout,ia,ib);
       free_p3c(&out,a,b);
       printf("No students in this class!\n");
       return 1;
      }
    for (j=0;j<ia;j++)
      {
       iducznia=atoi(iout[j][0]);
       err=getStudentByID(connection, iducznia, imie,nazwisko,klasa);
       if (err) { printf("Error computing student data!\n"); free_p3c(&iout,ia,ib); free_p3c(&out,a,b); return 1; }
       icheck = (char*)malloc(512);
       if (!icheck) { printf("icheck malloc failed!\n"); return 1; }
       sprintf(icheck,"SELECT ocena FROM ocena "
	       "WHERE iducznia=%d AND idprzedmiotu=%d", iducznia,idprzedmiotu);
       pgres = execute_query(connection, icheck);
       free(icheck);
       if (!pgres)
          {
           printf("SQL Error, table 'ocena' not exists?\n");
           printf("%d\n", __LINE__);
           return 1;
          }
       iiout = pgres2pc3(pgres, &iia, &iib);
       clear_result(&pgres);
       if (iia)
         {
          free_p3c(&iiout,iia,iib);
          printf("Mark: %s %s %s from %s is:\n", nazwisko,imie,klasa,przedmiot);
          printf("Student already have mark from this subject!\n");
	  printf("Press k to terminate\n");
	  printf("Press p to go to next subject\n");
	  printf("Press t to insert new mark (replace old)\n");
	  printf("Selection? (t/n/k/p) ");
          err = mgetch();
          ln();
          if (err>='A' && err<='Z') err+=0x20;
	  if (err=='k')
            {
             free_p3c(&iout,ia,ib);
             free_p3c(&out,a,b);
             return 0;
            }
	  if (err=='p') break;
	  if (err!='t') continue;
          icheck = (char*)malloc(512);
          if (!icheck) { printf("icheck malloc failed!\n"); return 1; }
           sprintf(icheck,"DELETE FROM ocena "
	       "WHERE iducznia=%d AND idprzedmiotu=%d", iducznia,idprzedmiotu);
           pgres = execute_query(connection, icheck);
           free(icheck);
	   if (!pgres)
	     {
              printf("Error deleting mark!\nTry (m)odyfy mark!\n");
	      continue;
	     }
         }
       ocenan:
       printf("Mark %s %s %s from %s is:\n", nazwisko,imie,klasa,przedmiot);
       printf("1.0-6.0\tnormal mark\n");
       printf("0\tdo not insert mark\n");
       printf("7\tnext subject\n");
       printf("8\tterminate inserting marks\n");
       printf(">>>\t");
       scanf("%s", tmp);
       wartosc=-1.0;
       sscanf(tmp,"%f", &wartosc);
       if (wartosc==0.0) continue;
       if (wartosc==7.0) break;
       if (wartosc==8.0)
         {
          free_p3c(&iout,ia,ib);
          free_p3c(&out,a,b);
          return 0;
         }
       if (wartosc<1.0 || wartosc>6.0) { printf("Invalid mark!\n"); goto ocenan; }
       *userCmd = (char*)malloc(400);
       if (!*userCmd) { ERR printf("malloc failed!\n"); return 1; }
       sprintf(*userCmd,"INSERT INTO ocena VALUES (%4.3f,'%c',%d,%d)", wartosc,czyliczona,iducznia,idprzedmiotu);
       pgres = execute_query(connection, *userCmd);
       if (!pgres)
         {
          printf("Error adding mark.\n");
          printf("%d\n", __LINE__);
          free(*userCmd);
          *userCmd = 0;
          return 1;
         }
       clear_result(&pgres);
       free(*userCmd);
       *userCmd = 0;
      }
    free_p3c(&iout,ia,ib);
   }
 free_p3c(&out,a,b);
 return 0;
}


int addMarkByStudent(char** userCmd, PGconn* connection)
{
 int a,b,i;
 char przedmiot[1025];
 char tmp[16];
 char*** out;
 char* check;
 int iducznia,idprzedmiotu;
 int czyliczona;
 float wartosc;
 PGresult* pgres;
 pgres=0;
 out=0;
 check=0;
 czyliczona='t';
 iducznia=idprzedmiotu=-1;
 pgres = execute_query(connection, "SELECT ocena FROM ocena ORDER BY ocena");
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    printf("Adding first mark...\n");
   }
 free_p3c(&out,a,b);
 printf("Select student (typein nonexisting to interrupt):\n");
 i=getDistinctStudent(&iducznia,connection);
 if (i) { printf("Error selecting student.\n"); return 1; }
 while (1)
 {
  wprzz:
 printf("Select subject:");
 i=displaySubjects(connection);
 if (i<=0) { printf("\nNo defined subjects!\n"); return 1; }
 selprz:
 printf("Subject name (or part): ");
 takestring(przedmiot);
 check = (char*)malloc(strlen(przedmiot)+200);
 if (!check) { ERR printf("malloc failed!\n"); return 1; }
 lsprintf(check,"SELECT nazwa,idprzedmiotu from przedmiot WHERE nazwa LIKE '|'", przedmiot);
 pgres = execute_query(connection, check);
 free(check);
 if (!pgres)
   {
    printf("SQL Error, table 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No such subject!\n");
    return 1;
   }
 if (a>1)
   {
    free_p3c(&out,a,b);
    printf("Selection is ambiguous, %d subjects match!\n",a);
    for (i=0;i<a;i++) printf("%s, ", out[i][0]);
    ln();
    goto wprzz;
    return 1;
   }
 idprzedmiotu = atoi(out[0][1]);
 free_p3c(&out,a,b);
 if (idprzedmiotu<0) { printf("Error computing idprzedmiotu!\n"); return 1; }
 pgres=0;
 out=0;
 check = (char*)malloc(512);
 if (!check) { printf("check malloc failed!\n"); return 1; }
 sprintf(check,"SELECT ocena FROM ocena "
	       "WHERE iducznia=%d AND idprzedmiotu=%d", iducznia,idprzedmiotu);
 pgres = execute_query(connection, check);
 free(check);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    free(*userCmd);
    *userCmd = 0;
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (a)
   {
    free_p3c(&out,a,b);
    printf("Already have got mark from this subject!\n");
    free(*userCmd);
    *userCmd = 0;
    goto selprz;
   }
 free_p3c(&out,a,b);
 printf("Typein mark (0 to cancel): ");
 shortstring(tmp, 15);
 wartosc = -1.0;
 sscanf(tmp, "%f", &wartosc);
 if (wartosc<1.0 || wartosc>6.0) { printf("Invalid mark.\n"); return 1; }
 czlicz:
 printf("\nMark is used to compute avg (t=YES, n=NO)? (t/n): ");
 czyliczona = mgetch();
 ln();
 if (czyliczona>='A' && czyliczona<='Z') czyliczona+=0x20;
 if (czyliczona!='n' && czyliczona!='t') { printf("Bad answer!\n"); goto czlicz; }
 *userCmd = (char*)malloc(400);
 if (!*userCmd) { ERR printf("malloc failed!\n"); return 1; }
 sprintf(*userCmd,"INSERT INTO ocena VALUES (%4.3f,'%c',%d,%d)", wartosc,czyliczona,iducznia,idprzedmiotu);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
     {
      printf("Error adding mark.\n");
    printf("%d\n", __LINE__);
      free(*userCmd);
      *userCmd = 0;
      return 1;
     }
   clear_result(&pgres);
   free(*userCmd);
   *userCmd = 0;
 }
 return 0;
}


int addMarkBySubject(char** userCmd, PGconn* connection)
{
 int a,b,i;
 char przedmiot[1025];
 char tmp[16];
 char*** out;
 char* check;
 int iducznia,idprzedmiotu;
 int czyliczona;
 float wartosc;
 PGresult* pgres;
 pgres=0;
 out=0;
 check=0;
 czyliczona='t';
 iducznia=idprzedmiotu=-1;
 pgres = execute_query(connection, "SELECT ocena FROM ocena ORDER BY ocena");
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    printf("Adding first mark...\n");
   }
 free_p3c(&out,a,b);
 printf("Select subject:");
 i=displaySubjects(connection);
 if (i<=0) { printf("\nNo defined subjects!\n"); return 1; }
 printf("Subject name (or part): ");
 takestring(przedmiot);
 check = (char*)malloc(strlen(przedmiot)+200);
 if (!check) { ERR printf("malloc failed!\n"); return 1; }
 lsprintf(check,"SELECT nazwa,idprzedmiotu from przedmiot WHERE nazwa LIKE '|'", przedmiot);
 pgres = execute_query(connection, check);
 free(check);
 if (!pgres)
   {
    printf("SQL Error, table 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No such subject!\n");
    return 1;
   }
 if (a>1)
   {
    free_p3c(&out,a,b);
    printf("Selection is ambiguous, %d subjects match!\n",a);
    for (i=0;i<a;i++) printf("%s, ", out[i][0]);
    ln();
    return 1;
   }
 idprzedmiotu = atoi(out[0][1]);
 free_p3c(&out,a,b);
 if (idprzedmiotu<0) { printf("Error computing idprzedmiotu!\n"); return 1; }
 while (1)
 {
 i=getDistinctStudent(&iducznia,connection);
 if (i) { printf("Error selecting student.\n"); return 1; }
 pgres=0;
 out=0;
 check = (char*)malloc(512);
 if (!check) { printf("check malloc failed!\n"); free(*userCmd); return 1; }
 sprintf(check,"SELECT ocena FROM ocena "
	       "WHERE iducznia=%d AND idprzedmiotu=%d",iducznia,idprzedmiotu);
 pgres = execute_query(connection, check);
 free(check);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    free(*userCmd);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (a)
   {
    free_p3c(&out,a,b);
    printf("Already have got mark from this subject!\n");
    free(*userCmd);
    *userCmd = 0;
    continue;
   }
 free_p3c(&out,a,b);
 printf("Typein mark (0 to cancel): ");
 shortstring(tmp, 15);
 wartosc = -1.0;
 sscanf(tmp, "%f", &wartosc);
 if (wartosc<1.0 || wartosc>6.0) { printf("Invalid mark.\n"); return 1; }
 czlicz:
 printf("\nIs mark used to compute avg (t=YES, n=NO)? (t/n): ");
 czyliczona = mgetch();
 ln();
 if (czyliczona>='A' && czyliczona<='Z') czyliczona+=0x20;
 if (czyliczona!='n' && czyliczona!='t')
   { printf("Bad answer!\n"); goto czlicz; }
 *userCmd = (char*)malloc(400);
 if (!*userCmd) { ERR printf("malloc failed!\n"); return 1; }
 sprintf(*userCmd,"INSERT INTO ocena VALUES (%4.3f,'%c',%d,%d)", wartosc,czyliczona,iducznia,idprzedmiotu);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
     {
      printf("Error adding mark.\n");
    printf("%d\n", __LINE__);
      free(*userCmd);
      *userCmd = 0;
      return 1;
     }
   clear_result(&pgres);
   free(*userCmd);
   *userCmd = 0;
 }
 return 0;
}


int addMark(char** userCmd, PGconn* connection)
{
 int a,b,i;
 char przedmiot[1025];
 char tmp[16];
 char*** out;
 char* check;
 int iducznia,idprzedmiotu;
 int czyliczona;
 float wartosc;
 PGresult* pgres;
 pgres=0;
 out=0;
 check=0;
 czyliczona='t';
 iducznia=idprzedmiotu=-1;
 pgres = execute_query(connection, "SELECT ocena FROM ocena ORDER BY ocena");
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a) printf("Adding first mark...\n");
 free_p3c(&out,a,b);
 printf("Select subject:");
 i=displaySubjects(connection);
 if (i<=0) { printf("\nNo defined subjects!\n"); return 1; }
 printf("Subject name (or part): ");
 takestring(przedmiot);
 check = (char*)malloc(strlen(przedmiot)+200);
 if (!check) { ERR printf("malloc failed!\n"); return 1; }
 lsprintf(check,"SELECT nazwa,idprzedmiotu from przedmiot WHERE nazwa LIKE '|'", przedmiot);
 pgres = execute_query(connection, check);
 free(check);
 if (!pgres)
   {
    printf("SQL Error, table 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No such subject!\n");
    return 1;
   }
 if (a>1)
   {
    free_p3c(&out,a,b);
    printf("Selection is ambiguous, %d subjects match!\n",a);
    for (i=0;i<a;i++) printf("%s, ", out[i][0]);
    ln();
    return 1;
   }
 idprzedmiotu = atoi(out[0][1]);
 free_p3c(&out,a,b);
 if (idprzedmiotu<0) { printf("Error computing idklasy!\n"); return 1; }
 i=getDistinctStudent(&iducznia,connection);
 if (i) { printf("Error selecting student.\n"); return 1; }
 pgres=0;
 out=0;
 check = (char*)malloc(512);
 if (!check) { printf("check malloc failed!\n"); return 1; }
 sprintf(check,"SELECT ocena FROM ocena "
	       "WHERE iducznia=%d AND idprzedmiotu=%d",iducznia,idprzedmiotu);
 pgres = execute_query(connection, check);
 free(check);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    free(*userCmd);
    *userCmd = 0;
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (a)
   {
    free_p3c(&out,a,b);
    printf("Already have got mark from this subject!\n");
    free(*userCmd);
    *userCmd = 0;
    return 1;
   }
 free_p3c(&out,a,b);
 printf("Typein mark: ");
 shortstring(tmp, 15);
 wartosc = -1.0;
 sscanf(tmp, "%f", &wartosc);
 if (wartosc<1.0 || wartosc>6.0) { printf("invalid mark.\n"); return 1; }
 printf("\nIs mark used to compute avg (t=YES, n=NO)? (t/n): ");
 czyliczona = mgetch();
 ln();
 if (czyliczona>='A' && czyliczona<='Z') czyliczona+=0x20;
 if (czyliczona!='n' && czyliczona!='t') { printf("Bad answer!\n"); return 1; }
 *userCmd = (char*)malloc(400);
 if (!*userCmd) { ERR printf("malloc failed!\n"); free(*userCmd); return 1; }
 sprintf(*userCmd,"INSERT INTO ocena VALUES (%4.3f,'%c',%d,%d)", wartosc,czyliczona,iducznia,idprzedmiotu);
 return 0;
}


int addStudent(char** userCmd, PGconn* connection)
{
 char imie[1025];
 char nazwisko[1025];
 char klasa[16];
 int a,b,i,idklasy;
 char*** out;
 char* check;
 PGresult* pgres;
 pgres=0;
 out=0;
 check=0;
 idklasy=-1;
 pgres = execute_query(connection, "SELECT imie,nazwisko,klasa.idklasy,nazwa FROM uczen,klasa WHERE klasa.idklasy=uczen.idklasy ORDER BY nazwisko");
 if (!pgres)
   {
    printf("SQL Error, table 'uczen' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    printf("Adding first student...\n");
   }
 printf("Available students:\n");
 for (i=0;i<a;i++) printf("%-30s\t %-25s\t %s\n", out[i][1],out[i][0],out[i][3]);
 ln();
 free_p3c(&out,a,b);
 printf("Select students class:");
 i=displayClasses(connection);
 if (i<=0) { printf("\nNo defined classes!\n"); return 1; }
 printf("Class name (or part): ");
 shortstring(klasa, 8);
 printf("Name: ");
 takestring(imie);
 printf("Surname: ");
 takestring(nazwisko);
 *userCmd = (char*)malloc(strlen(imie)+strlen(nazwisko)+strlen(klasa)+200);
 check = (char*)malloc(strlen(imie)+strlen(nazwisko)+strlen(klasa)+200);
 if (!*userCmd || !check) { ERR printf("malloc failed!\n"); return 1; }
 lsprintf(check,"SELECT nazwa,idklasy from klasa WHERE nazwa LIKE '|'", klasa);
 pgres = execute_query(connection, check);
 free(check);
 if (!pgres)
   {
    printf("SQL Error, table 'klasa' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No such class!\n");
    return 1;
   }
 if (a>1)
   {
    free_p3c(&out,a,b);
    printf("Selection is ambiguous, %d classes match!\n",a);
    for (i=0;i<a;i++) printf("%s, ", out[i][0]);
    ln();
    return 1;
   }
 idklasy = atoi(out[0][1]);
 free_p3c(&out,a,b);
 if (idklasy<0) { printf("Error computing idklasy!\n"); return 1; }
 sprintf(*userCmd,"INSERT INTO uczen VALUES ('%s','%s',%d)", imie,nazwisko,idklasy);
 pgres=0;
 out=0;
 check = (char*)malloc(512);
 if (!check) { printf("check malloc failed!\n"); return 1; }
 sprintf(check,"SELECT imie,nazwisko,klasa.idklasy,nazwa FROM uczen,klasa "
	       "WHERE imie='%s' AND nazwisko='%s' AND klasa.idklasy=%d AND klasa.idklasy=uczen.idklasy", imie,nazwisko,idklasy);
 pgres = execute_query(connection, check);
 free(check);
 if (!pgres)
   {
    printf("SQL Error, table 'uczen' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (a)
   {
    free_p3c(&out,a,b);
    printf("Such student: %s,%s qalready exists in this class!\n",imie,nazwisko);
    return 1;
   }
 free_p3c(&out,a,b);
 return 0;
}


int addSubject(char** userCmd, PGconn* connection)
{
 char tmp[1025];
 int len,a,b,i;
 char*** out;
 char* check;
 PGresult* pgres;
 pgres=0;
 out=0;
 check=0;
 pgres = execute_query(connection, "SELECT nazwa FROM przedmiot ORDER BY nazwa");
 if (!pgres)
   {
    printf("SQL Error, table 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    printf("Adding first subject...\n");
   }
 printf("Adding new subject do not use names of other subjects:\n");
 for (i=0;i<a;i++) printf("%s, ",out[i][0]);
 printf("\n");
 free_p3c(&out,a,b);
 printf("Subject name (max 30 characters): ");
 takestring(tmp);
 len = strlen(tmp);
 if (len>30) { printf("Name longer than 30 chars!\n"); return 1; }
 *userCmd = (char*)malloc(len+200);
 if (!*userCmd) { ERR printf("malloc failed!\n"); return 1; }
 sprintf(*userCmd,"INSERT INTO przedmiot VALUES ('%s')", tmp);
 pgres=0;
 out=0;
 check = (char*)malloc(220);
 if (!check) { printf("check malloc failed!\n"); return 1; }
 sprintf(check,"SELECT * FROM przedmiot WHERE nazwa='%s'", tmp);
 pgres = execute_query(connection, check);
 free(check);
 if (!pgres)
   {
    printf("SQL Error, table 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (a)
   {
    free_p3c(&out,a,b);
    printf("Already exists!\n");
    return 1;
   }
 free_p3c(&out,a,b);
 return 0;
}


int addClass(char** userCmd, PGconn* connection)
{
 char tmp[1025];
 int len,a,b,i;
 char*** out;
 char* check;
 PGresult* pgres;
 pgres=0;
 out=0;
 check=0;
 pgres = execute_query(connection, "SELECT nazwa FROM klasa ORDER BY nazwa");
 if (!pgres)
   {
    printf("SQL Error, table 'klasa' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a) printf("Adding first class.\n");
 printf("Adding new class do not use names of existing ones:\n");
 for (i=0;i<a;i++) printf("%s, ",out[i][0]);
 printf("\n");
 free_p3c(&out,a,b);
 printf("Class name (max 7 chars): ");
 takestring(tmp);
 len = strlen(tmp);
 if (len>7) { printf("Name longer than 7 chars!\n"); return 1; }
 *userCmd = (char*)malloc(len+200);
 if (!*userCmd) { ERR printf("malloc failed!\n"); return 1; }
 sprintf(*userCmd,"INSERT INTO klasa VALUES ('%s')", tmp);
 pgres=0;
 out=0;
 check = (char*)malloc(250);
 if (!check) { printf("check malloc failed!\n"); return 1; }
 sprintf(check,"SELECT * FROM klasa WHERE nazwa='%s'", tmp);
 pgres = execute_query(connection, check);
 free(check);
 if (!pgres)
   {
    printf("SQL Error, table 'klasa' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (a)
   {
    free_p3c(&out,a,b);
    printf("Such class already exists!\n");
    return 1;
   }
 free_p3c(&out,a,b);
 return 0;
}


int seeStudents(char** userCmd, PGconn* connection)
{
 int a,b,i;
 char*** out;
 int zn;
 PGresult* pgres;
 char klasa[16];
 char* check;
 pgres=0;
 out=0;
 check=0;
 pgres = execute_query(connection, "SELECT * FROM uczen ORDER BY nazwisko");
 if (!pgres)
   {
    printf("SQL Error, table 'uczen' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("Brak zdefiniowanych uczniow!\n");
    return 1;
   }
 printf("\n\nw\tall students\n");
 printf("k\tfrom given class\n");
 printf("\n>>>\t");
 zn = mgetch();
 if (zn>='A' && zn<='Z') zn+=0x20;
 ln();
 if (zn=='k')
   {
    printf("Available classes:\n");
    i=displayClasses(connection);
    if (i<=0) { printf("\nNo defined classes!\n"); return 1; }
    printf("Class name (or part): ");
    shortstring(klasa, 8);
    check = (char*)malloc(400);
    if (!check) { ERR printf("malloc failed!\n"); return 1; }
    lsprintf(check,"SELECT nazwa,idklasy from klasa WHERE nazwa LIKE '|'", klasa);
    pgres = execute_query(connection, check);
    free(check);
    if (!pgres)
      {
       printf("SQL Error, table 'klasa' not exists?\n");
    printf("%d\n", __LINE__);
       return 1;
      }
    out = pgres2pc3(pgres, &a, &b);
    clear_result(&pgres);
    if (!a)
      {
       free_p3c(&out,a,b);
       printf("No such class!\n");
       return 1;
      }
   }
 else strcpy(klasa,"%");
 check = (char*)malloc(450);
 if (!check) { ERR printf("malloc failed!\n"); return 1; }
 lsprintf(check,"SELECT uczen.nazwisko,uczen.imie,klasa.nazwa from uczen,klasa "
	       "WHERE klasa.nazwa LIKE '|' AND klasa.idklasy=uczen.idklasy ORDER BY nazwisko", klasa);
 pgres = execute_query(connection, check);
 free(check);
 if (!pgres)
    {
     printf("SQL Error, table 'klasa' lub uczen not exists?\n");
    printf("%d\n", __LINE__);
     return 1;
    }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No such students!\n");
    return 1;
   }
 for (i=0;i<a;i++) printf("%-30s\t %-25s\t %-9s\n", out[i][0],out[i][1],out[i][2]);
 free_p3c(&out,a,b);
 return 0;
}


void seeSubjects(char** userCmd, PGconn* connection)
{
 int a,b,i;
 char*** out;
 PGresult* pgres;
 pgres=0;
 out=0;
 pgres = execute_query(connection, "SELECT * FROM przedmiot ORDER BY nazwa");
 if (!pgres)
   {
    printf("SQL Error, table 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    return;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No defined subjects!\n");
    return;
   }
 printf("Defined subjects:\n");
 for (i=0;i<a;i++) printf("subject %-30s\t sysID %s\n",out[i][0],out[i][1]);
 printf("\n");
 free_p3c(&out,a,b);
}


void seeClasses(char** userCmd, PGconn* connection)
{
 int a,b,i;
 char*** out;
 PGresult* pgres;
 pgres=0;
 out=0;
 pgres = execute_query(connection, "SELECT * FROM klasa ORDER BY nazwa");
 if (!pgres)
   {
    printf("SQL Error, table 'klasa' not exists?\n");
    printf("%d\n", __LINE__);
    return;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("\nNo defined classes!\n");
    return;
   }
 printf("Defined classes:\n");
 for (i=0;i<a;i++) printf("class %-10s\t sysID %s\n",out[i][0],out[i][1]);
 printf("\n");
 free_p3c(&out,a,b);
}


int displayStudents(PGconn* connection)
{
 int a,b,i;
 char*** out;
 PGresult* pgres;
 pgres=0;
 out=0;
 pgres = execute_query(connection, "SELECT uczen.nazwisko,uczen.imie,klasa.nazwa FROM uczen,klasa WHERE uczen.idklasy=klasa.idklasy ORDER BY uczen.nazwisko");
 if (!pgres)
   {
    printf("SQL Error, table 'klasa' lub uczen not exists?\n");
    printf("%d\n", __LINE__);
    return 0;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("\nNo students!\n");
    return 0;
   }
 printf("\n");
 for (i=0;i<a;i++) printf("%-30s\t %-25s\t %-9s\n",out[i][0],out[i][1],out[i][2]);
 printf("\n");
 free_p3c(&out,a,b);
 return a;
}


int displaySubjects(PGconn* connection)
{
 int a,b,i;
 char*** out;
 PGresult* pgres;
 pgres=0;
 out=0;
 pgres = execute_query(connection, "SELECT nazwa FROM przedmiot ORDER BY nazwa");
 if (!pgres)
   {
    printf("SQL Error, table 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    return 0;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("\nNo defined subjects!\n");
    return 0;
   }
 printf("\n");
 for (i=0;i<a;i++) printf("%s, ",out[i][0]);
 printf("\n");
 free_p3c(&out,a,b);
 return a;
}


int displayClasses(PGconn* connection)
{
 int a,b,i;
 char*** out;
 PGresult* pgres;
 pgres=0;
 out=0;
 pgres = execute_query(connection, "SELECT nazwa FROM klasa ORDER BY nazwa");
 if (!pgres)
   {
    printf("SQL Error, table 'klasa' not exists?\n");
    printf("%d\n", __LINE__);
    return 0;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("\nNo defined classes!\n");
    return 0;
   }
 printf("\n");
 for (i=0;i<a;i++) printf("%s, ",out[i][0]);
 printf("\n");
 free_p3c(&out,a,b);
 return a;
}


int modifyStudentIndirect(PGconn* connection, char* userCmd,char* nazwisko, char* imie, int idklasy)
{
 int n,i,k,z,a,b,j;
 char nimie[1025];
 char nnazwisko[1025];
 char tmp[16];
 int nidklasy;
 char* check;
 char*** out;
 PGresult* pgres;
 i=n=k=z=a=b=j=0;
 check=0;
 out=0;
 nidklasy=-1;
 strcpy(nnazwisko,nazwisko);
 strcpy(nimie,imie);
 nidklasy=idklasy;
 printf("Modify surname? (y/n): ");
 z = mgetch();
 ln();
 if (z>='A' && z<='Z') z+=0x20;
 if (z=='y') n=1;
 printf("Modify name? (y/n): ");
 z = mgetch();
 ln();
 if (z>='A' && z<='Z') z+=0x20;
 if (z=='y') i=1;
 printf("Modify class? (y/n): ");
 z = mgetch();
 ln();
 if (z>='A' && z<='Z') z+=0x20;
 if (z=='y') k=1;
 if (n)
   {
    printf("Surname: ");
    takestring(nnazwisko);
   }
 if (i)
   {
    printf("Name: ");
    takestring(nimie);
   }
 if (k)
   {
    printf("Defined classes:\n");
    z=displayClasses(connection);
    if (z<0) { printf("No defined classes!\n"); return 1; }
    printf("Class name (or part): ");
    shortstring(tmp,8);
    check = (char*)malloc(350);
    if (!check) { ERR printf("malloc failed!\n"); return 1; }
    lsprintf(check,"SELECT nazwa,idklasy from klasa WHERE nazwa LIKE '|'", tmp);
    pgres = execute_query(connection, check);
    free(check);
    if (!pgres)
      {
       printf("SQL Error, table 'klasa' not exists?\n");
    printf("%d\n", __LINE__);
       return 1;
      }
    out = pgres2pc3(pgres, &a, &b);
    clear_result(&pgres);
    if (!a)
      {
       free_p3c(&out,a,b);
       printf("No such class!\n");
       return 1;
      }
    if (a>1)
      {
       free_p3c(&out,a,b);
       printf("Selection is ambiguous, %d classes match!\n",a);
       for (j=0;j<a;j++) printf("%s, ", out[j][0]);
       ln();
       return 1;
      }
    nidklasy = atoi(out[0][1]);
    if (nidklasy<0) { free_p3c(&out,a,b) ; printf("Error computing idklasy!\n"); return 1; }
    printf("Selected class: %s\n", out[0][0]);
    free_p3c(&out,a,b);
   }
 check = (char*)malloc(350);
 if (!check) { ERR printf("malloc failed!\n"); return 1; }
 sprintf(check,"SELECT nazwisko FROM uczen WHERE imie='%s' AND nazwisko='%s' AND idklasy=%d", nimie, nnazwisko, nidklasy);
 pgres = execute_query(connection, check);
 free(check);
 if (!pgres)
      {
       printf("SQL Error, table 'uczen' not exists?\n");
       printf("%d\n", __LINE__);
       return 1;
      }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (a)
      {
       free_p3c(&out,a,b);
       printf("Such student already exists!\n");
       strcpy(userCmd,"");
       return 1;
      }
 free_p3c(&out,a,b);
 lsprintf(userCmd,"UPDATE uczen SET imie='%s', nazwisko='%s', idklasy=%d WHERE nazwisko LIKE '|' AND imie LIKE '|' AND idklasy=%d", nimie,nnazwisko,nidklasy,nazwisko, imie, idklasy);
 return 0;
}


int modifyIndexStudent(char** userCmd, PGconn* connection)
{
 int a,b,i,iducznia,err,idklasy;
 char*** out;
 char* check;
 char imie[1025],nazwisko[1025];
 PGresult* pgres;
 pgres=0;
 out=0;
 check=0;
 idklasy=-1;
 printf("Defined classes:\n");
 i=displayClasses(connection);
 if (i<0) { printf("No classes, no students.\n"); return 1; }
 pgres = execute_query(connection, "SELECT nazwisko,imie,klasa.nazwa,iducznia FROM uczen,klasa WHERE uczen.idklasy=klasa.idklasy ORDER BY nazwisko");
 if (!pgres)
   {
    printf("SQL Error, table 'uczen' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No students to modify!\n");
    return 1;
   }
 printf("Available students:\n");
 for (i=0;i<a;i++) printf("%-28s\t %-20s\t %-8s ID: %s\n", out[i][0],out[i][1],out[i][2],out[i][3]);
 ln();
 free_p3c(&out,a,b);
 printf("\nStudent's ID: ");
 takestring(imie);
 iducznia=-1;
 sscanf(imie,"%d",&iducznia);
 if (iducznia<0) { printf("Invalid ID!\n"); return 1; }
 check = (char*)malloc(370);
 *userCmd = (char*)malloc(400);
 if (!*userCmd || !check) { ERR printf("malloc failed!\n"); return 1; }
 sprintf(check,"SELECT imie,nazwisko,idklasy from uczen WHERE iducznia=%d ORDER BY nazwisko", iducznia);
 pgres = execute_query(connection, check);
 free(check);
 if (!pgres)
   {
    printf("SQL Error, table 'uczen' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No such ID: %d!\n",iducznia);
    return 1;
   }
 strcpy(imie,out[0][0]);
 strcpy(nazwisko,out[0][1]);
 idklasy = atoi(out[0][2]);
 free_p3c(&out,a,b);
 err=modifyStudentIndirect(connection,*userCmd,nazwisko,imie,idklasy);
 if (err) { printf("Error modifying student!\n"); return 1; }
 return 0;
}


int modifyStudent(char** userCmd, PGconn* connection)
{
 char imie[1025];
 char nazwisko[1025];
 char klasa[16];
 int a,b,i,idklasy,err;
 char*** out;
 char* check;
 PGresult* pgres;
 pgres=0;
 out=0;
 check=0;
 idklasy=-1;
 printf("Available classes:\n");
 i=displayClasses(connection);
 if (i<0) { printf("No classes, no students.\n"); return 1; }
 pgres = execute_query(connection, "SELECT imie,nazwisko,klasa.idklasy,nazwa FROM uczen,klasa WHERE uczen.idklasy=klasa.idklasy ORDER BY nazwisko");
 if (!pgres)
   {
    printf("SQL Error, table 'uczen' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No students to modify!\n");
    return 1;
   }
 printf("Available students:\n");
 for (i=0;i<a;i++) printf("%-30s\t %-25s\t %s\n", out[i][1],out[i][0],out[i][3]);
 ln();
 free_p3c(&out,a,b);
 printf("\nStudent's surname (or part): ");
 takestring(nazwisko);
 check = (char*)malloc(370);
 *userCmd = (char*)malloc(400);
 if (!*userCmd || !check) { ERR printf("malloc failed!\n"); return 1; }
 lsprintf(check,"SELECT imie,nazwisko,klasa.idklasy,nazwa from uczen,klasa WHERE nazwisko LIKE '|' AND klasa.idklasy=uczen.idklasy ORDER BY nazwisko", nazwisko);
 pgres = execute_query(connection, check);
 free(check);
 if (!pgres)
   {
    printf("SQL Error, table 'uczen' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No such surname: %s!\n",nazwisko);
    return 1;
   }
 if (a>1)
   {
    free_p3c(&out,a,b);
    printf("Selection ambiguous, listed students match:\n");
    for (i=0;i<a;i++) printf("%-30s\t %-25s\t %s\n", out[i][1],out[i][0],out[i][3]);
    ln();
    printf("Student's name (or part): ");
    takestring(imie);
    check = (char*)malloc(strlen(imie)+200);
    lsprintf(check,"SELECT imie,nazwisko,klasa.idklasy,nazwa from uczen,klasa WHERE nazwisko LIKE '|' AND imie LIKE '|' AND klasa.idklasy=uczen.idklasy ORDER BY nazwisko", nazwisko,imie);
    pgres = execute_query(connection, check);
    free(check);
    if (!pgres)
      {
       printf("SQL Error, table 'uczen' not exists?\n");
    printf("%d\n", __LINE__);
       return 1;
      }
    out = pgres2pc3(pgres, &a, &b);
    clear_result(&pgres);
    if (!a)
      {
       free_p3c(&out,a,b);
       printf("No such student: %s %s!\n",nazwisko,imie);
       return 1;
      }
    if (a>1)
      {
       strcpy(imie,out[0][0]);
       strcpy(nazwisko,out[0][1]);
       free_p3c(&out,a,b);
       printf("Selection ambiguous, listed students match:\n");
       for (i=0;i<a;i++) printf("%-30s\t %-25s\t %s\n", out[i][1],out[i][0],out[i][3]);
       ln();
       printf("You have to typein class, because name & surname are still ambiguous...\n");
       displayClasses(connection);
       printf("Class name (or part): ");
       shortstring(klasa, 8);
       check = (char*)malloc(strlen(imie)+strlen(nazwisko)+strlen(klasa)+200);
       if (!check) { ERR printf("malloc failed!\n"); return 1; }
       lsprintf(check,"SELECT nazwa,idklasy from klasa WHERE nazwa LIKE '|'", klasa);
       pgres = execute_query(connection, check);
       free(check);
       if (!pgres)
         {
          printf("SQL Error, table 'klasa' not exists?\n");
    printf("%d\n", __LINE__);
          return 1;
         }
       out = pgres2pc3(pgres, &a, &b);
       clear_result(&pgres);
       if (!a)
         {
          free_p3c(&out,a,b);
          printf("No such class!\n");
          return 1;
         }
      if (a>1)
         {
          free_p3c(&out,a,b);
          printf("Selection is ambiguous, %d classes match!\n",a);
          for (i=0;i<a;i++) printf("%s, ", out[i][0]);
          ln();
          return 1;
         }
       idklasy = atoi(out[0][1]);
       if (idklasy<0) { free_p3c(&out,a,b) ; printf("Error computing idklasy!\n"); return 1; }
       printf("Selected class: %s\n", out[0][0]);
       free_p3c(&out,a,b);
       err=modifyStudentIndirect(connection,*userCmd,nazwisko,imie,idklasy);
       if (err) { printf("Error mdyfing student!\n"); return 1; }
       return 0;
      }
    strcpy(imie,out[0][0]);
    strcpy(nazwisko,out[0][1]);
    idklasy = atoi(out[0][2]);
    free_p3c(&out,a,b);
    err=modifyStudentIndirect(connection,*userCmd,nazwisko,imie,idklasy);
    if (err) { printf("Error modyfying student!\n"); return 1; }
    return 0;
   }
 strcpy(imie,out[0][0]);
 strcpy(nazwisko,out[0][1]);
 idklasy = atoi(out[0][2]);
 free_p3c(&out,a,b);
 err=modifyStudentIndirect(connection,*userCmd,nazwisko,imie,idklasy);
 if (err) { printf("Error modifying student!\n"); return 1; }
 return 0;
}


int removeStudent(char** userCmd, PGconn* connection)
{
 char imie[1025];
 char nazwisko[1025];
 char klasa[16];
 int a,b,i,idklasy;
 char*** out;
 char* check;
 PGresult* pgres;
 pgres=0;
 out=0;
 check=0;
 idklasy=-1;
 printf("Available classes:\n");
 i=displayClasses(connection);
 if (i<0) { printf("No classes, no students.\n"); return 1; }
 pgres = execute_query(connection, "SELECT imie,nazwisko,uczen.idklasy,nazwa FROM uczen,klasa WHERE uczen.idklasy=klasa.idklasy ORDER BY nazwisko");
 if (!pgres)
   {
    printf("SQL Error, table 'uczen' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No students to remove!\n");
    return 1;
   }
 printf("Available students:\n");
 for (i=0;i<a;i++) printf("%-30s\t %-25s\t %s\n", out[i][1],out[i][0],out[i][3]);
 ln();
 free_p3c(&out,a,b);
 printf("\nTypein student's surname (or part): ");
 takestring(nazwisko);
 check = (char*)malloc(370);
 *userCmd = (char*)malloc(400);
 if (!*userCmd || !check) { ERR printf("malloc failed!\n"); return 1; }
 lsprintf(check,"SELECT imie,nazwisko,klasa.idklasy,nazwa from uczen,klasa WHERE nazwisko LIKE '|' AND uczen.idklasy=klasa.idklasy ORDER BY nazwisko", nazwisko);
 pgres = execute_query(connection, check);
 free(check);
 if (!pgres)
   {
    printf("SQL Error, table 'uczen' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No such surname: %s!\n",nazwisko);
    return 1;
   }
 if (a>1)
   {
    free_p3c(&out,a,b);
    printf("Selection ambiguous, listed students match:\n");
    for (i=0;i<a;i++) printf("%-30s\t %-25s\t %s\n", out[i][1],out[i][0],out[i][3]);
    ln();
    printf("Typein student's name (or part): ");
    takestring(imie);
    check = (char*)malloc(strlen(imie)+200);
    lsprintf(check,"SELECT imie,nazwisko,klasa.idklasy,nazwa from uczen,klasa WHERE nazwisko LIKE '|' AND imie LIKE '|' AND klasa.idklasy=uczen.idklasy ORDER BY nazwisko", nazwisko,imie);
    pgres = execute_query(connection, check);
    free(check);
    if (!pgres)
      {
       printf("SQL Error, table 'uczen' not exists?\n");
    printf("%d\n", __LINE__);
       return 1;
      }
    out = pgres2pc3(pgres, &a, &b);
    clear_result(&pgres);
    if (!a)
      {
       free_p3c(&out,a,b);
       printf("No such student: %s %s!\n",nazwisko,imie);
       return 1;
      }
    if (a>1)
      {
       free_p3c(&out,a,b);
       printf("Selection ambiguous, listed students match:\n");
       for (i=0;i<a;i++) printf("%-30s\t %-25s\t %s\n", out[i][1],out[i][0],out[i][3]);
       ln();
       printf("You have to typein class name, because name & surname is ambiguous\n");
       displayClasses(connection);
       printf("Class name (or part): ");
       shortstring(klasa, 8);
       check = (char*)malloc(strlen(imie)+strlen(nazwisko)+strlen(klasa)+200);
       if (!check) { ERR printf("malloc failed!\n"); return 1; }
       lsprintf(check,"SELECT nazwa,idklasy from klasa WHERE nazwa LIKE '|'", klasa);
       pgres = execute_query(connection, check);
       free(check);
       if (!pgres)
         {
          printf("SQL Error, table 'klasa' not exists?\n");
    printf("%d\n", __LINE__);
          return 1;
         }
       out = pgres2pc3(pgres, &a, &b);
       clear_result(&pgres);
       if (!a)
         {
          free_p3c(&out,a,b);
          printf("No such class!\n");
          return 1;
         }
      if (a>1)
         {
          free_p3c(&out,a,b);
          printf("Selection is ambiguous, %d classes match!\n",a);
          for (i=0;i<a;i++) printf("%s, ", out[i][0]);
          ln();
          return 1;
         }
       idklasy = atoi(out[0][1]);
       if (idklasy<0) { free_p3c(&out,a,b) ; printf("Error computing idklasy!\n"); return 1; }
       printf("Selected class: %s\n", out[0][0]);
       free_p3c(&out,a,b);
       lsprintf(*userCmd,"DELETE FROM uczen WHERE nazwisko LIKE '|' AND imie LIKE '|' AND idklasy=%d", nazwisko, imie, idklasy);
       return 0;
      }
    free_p3c(&out,a,b);
    lsprintf(*userCmd,"DELETE FROM uczen WHERE nazwisko LIKE '|' AND imie LIKE '|'", nazwisko, imie);
    return 0;
   }
 free_p3c(&out,a,b);
 lsprintf(*userCmd,"DELETE FROM uczen WHERE nazwisko LIKE '|'", nazwisko);
 return 0;
}


int countStudentsInClass(char** userCmd, PGconn* connection)
{
 int a,b,i;
 char*** out;
 PGresult* pgres;
 pgres=0;
 out=0;
 pgres = execute_query(connection, "SELECT imie FROM uczen ORDER BY imie");
 if (!pgres)
   {
    printf("SQL Error, table 'uczen' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No students.\n");
    return 1;
   }
 free_p3c(&out,a,b);
 *userCmd = (char*)malloc(256);
 sprintf(*userCmd,
	"SELECT klasa.idklasy,klasa.nazwa,count(uczen.imie) "
	"FROM klasa,uczen "
	"WHERE klasa.idklasy=uczen.idklasy "
	"GROUP BY klasa.idklasy,klasa.nazwa "
	"ORDER BY klasa.nazwa"
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'uczen' lub klasa not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No students or classes.\n");
    return 1;
   }
 for (i=0;i<a;i++) printf("class:%-10s\t students count: %s\n", out[i][1],out[i][2]);
 free_p3c(&out,a,b);
 free(*userCmd);
 *userCmd=0;
 return 0;
}


int averageSchoolByMark(char** userCmd, PGconn* connection)
{
 int a,b;
 char*** out;
 PGresult* pgres;
 pgres=0;
 out=0;
 pgres = execute_query(connection, "SELECT ocena FROM ocena ORDER BY ocena");
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No marks.\n");
    return 1;
   }
 free_p3c(&out,a,b);
 *userCmd = (char*)malloc(256);
 sprintf(*userCmd,
	"SELECT avg(ocena) "
	"FROM ocena"
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No marks.\n");
    return 1;
   }
 printf("Average below is an average of all marks\n");
 printf("school avg: %s\n", out[0][0]);
 free_p3c(&out,a,b);
 free(*userCmd);
 *userCmd=0;
 *userCmd = (char*)malloc(256);
 sprintf(*userCmd,
	"SELECT avg(ocena) "
	"FROM ocena "
	"WHERE ocena.czyliczona='t'"
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No marks.\n");
    return 1;
   }
 printf("Average below is average of computed only marks\n");
 printf("school avg: %s\n", out[0][0]);
 free_p3c(&out,a,b);
 free(*userCmd);
 *userCmd=0;
 return 0;
}


int countStudentsInSchool(char** userCmd, PGconn* connection)
{
 int a,b;
 char*** out;
 PGresult* pgres;
 pgres=0;
 out=0;
 pgres = execute_query(connection, "SELECT imie FROM uczen ORDER BY imie");
 if (!pgres)
   {
    printf("SQL Error, table 'uczen' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No students.\n");
    return 1;
   }
 free_p3c(&out,a,b);
 *userCmd = (char*)malloc(256);
 sprintf(*userCmd,
	"SELECT count(*) "
	"FROM uczen "
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'uczen' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No students.\n");
    return 1;
   }
 printf("count students in school: %s\n", out[0][0]);
 free_p3c(&out,a,b);
 free(*userCmd);
 *userCmd=0;
 return 0;
}


int countMarksSchool(char** userCmd, PGconn* connection)
{
 int a,b;
 char*** out;
 PGresult* pgres;
 pgres=0;
 out=0;
 pgres = execute_query(connection, "SELECT ocena FROM ocena ORDER BY ocena");
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No marks.\n");
    return 1;
   }
 free_p3c(&out,a,b);
 *userCmd = (char*)malloc(256);
 sprintf(*userCmd,
	"SELECT count(ocena) "
	"FROM ocena "
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No marks.\n");
    return 1;
   }
 printf("all marks in school: %s\n", out[0][0]);
 free_p3c(&out,a,b);
 free(*userCmd);
 *userCmd=0;
 *userCmd = (char*)malloc(256);
 sprintf(*userCmd,
	"SELECT count(ocena) "
	"FROM ocena "
	"WHERE ocena.czyliczona='t'"
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No marks.\n");
    return 1;
   }
 printf("all marks computed in avg in school: %s\n", out[0][0]);
 free_p3c(&out,a,b);
 free(*userCmd);
 *userCmd=0;
 return 0;
}


int averageSchool(char** userCmd, PGconn* connection)
{
 int a,b;
 char*** out;
 PGresult* pgres;
 pgres=0;
 out=0;
 pgres = execute_query(connection, "SELECT ocena FROM ocena ORDER BY ocena");
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No marks cannot compute avg.\n");
    return 1;
   }
 free_p3c(&out,a,b);
 *userCmd = (char*)malloc(1024);
 sprintf(*userCmd,
	"CREATE VIEW srednie_uczniow(iducznia,idklasy,srednia) AS "
	"SELECT uczen.iducznia,klasa.idklasy,avg(ocena) "
	"FROM uczen,ocena,klasa "
	"WHERE uczen.idklasy=klasa.idklasy "
	"AND ocena.iducznia=uczen.iducznia "
	"AND ocena.czyliczona='t' "
	"GROUP BY uczen.iducznia,klasa.idklasy "
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' or 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 clear_result(&pgres);
 free(*userCmd);
 *userCmd=0;
 *userCmd = (char*)malloc(512);
 sprintf(*userCmd,
	"CREATE VIEW srednie_klas(idklasy,klasa,srednia) AS "
	"SELECT klasa.idklasy,klasa.nazwa,avg(srednia) "
	"FROM srednie_uczniow,klasa "
	"WHERE srednie_uczniow.idklasy=klasa.idklasy "
	"GROUP BY klasa.idklasy,klasa.nazwa "
	"ORDER BY klasa.nazwa"
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' or 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    execute_query(connection,"DROP VIEW srednie_uczniow");
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 free_p3c(&out,a,b);
 free(*userCmd);
 *userCmd=0;
 *userCmd = (char*)malloc(256);
 sprintf(*userCmd,
	"SELECT avg(srednia) "
	"FROM srednie_klas "
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, view 'srednie_klas' not exists?\n");
    printf("%d\n", __LINE__);
    execute_query(connection,"DROP VIEW srednie_klas");
    execute_query(connection,"DROP VIEW srednie_uczniow");
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No averages, cannot compute school avg.\n");
    execute_query(connection,"DROP VIEW srednie_klas");
    execute_query(connection,"DROP VIEW srednie_uczniow");
    return 1;
   }
 printf("Avg below is an average of averages of classes.\n");
 printf("computed as averages of averages of subjects.\n");
 printf("only marks computed in avg.\n");
 printf("school avg: %s\n", out[0][0]);
 free_p3c(&out,a,b);
 free(*userCmd);
 *userCmd=0;
 execute_query(connection,"DROP VIEW srednie_klas");
 execute_query(connection,"DROP VIEW srednie_uczniow");
 *userCmd = (char*)malloc(1024);
 sprintf(*userCmd,
	"CREATE VIEW srednie_uczniow(iducznia,idklasy,srednia) AS "
	"SELECT uczen.iducznia,klasa.idklasy,avg(ocena) "
	"FROM uczen,ocena,klasa "
	"WHERE uczen.idklasy=klasa.idklasy "
	"AND ocena.iducznia=uczen.iducznia "
	"GROUP BY uczen.iducznia,klasa.idklasy "
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' or 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 clear_result(&pgres);
 free(*userCmd);
 *userCmd=0;
 *userCmd = (char*)malloc(512);
 sprintf(*userCmd,
	"CREATE VIEW srednie_klas(idklasy,klasa,srednia) AS "
	"SELECT klasa.idklasy,klasa.nazwa,avg(srednia) "
	"FROM srednie_uczniow,klasa "
	"WHERE srednie_uczniow.idklasy=klasa.idklasy "
	"GROUP BY klasa.idklasy,klasa.nazwa "
	"ORDER BY klasa.nazwa"
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' or 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    execute_query(connection,"DROP VIEW srednie_uczniow");
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 free_p3c(&out,a,b);
 free(*userCmd);
 *userCmd=0;
 *userCmd = (char*)malloc(256);
 sprintf(*userCmd,
	"SELECT avg(srednia) "
	"FROM srednie_klas "
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, view 'srednie_klas' not exists?\n");
    printf("%d\n", __LINE__);
    execute_query(connection,"DROP VIEW srednie_klas");
    execute_query(connection,"DROP VIEW srednie_uczniow");
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No averages - cannot compute school avg.\n");
    execute_query(connection,"DROP VIEW srednie_klas");
    execute_query(connection,"DROP VIEW srednie_uczniow");
    return 1;
   }
 printf("Avg below is an average of averages of classes.\n");
 printf("Computed as averages of averages of subjects.\n");
 printf("Computed are all marks.\n");
 printf("school average: %s\n", out[0][0]);
 free_p3c(&out,a,b);
 free(*userCmd);
 *userCmd=0;
 execute_query(connection,"DROP VIEW srednie_klas");
 execute_query(connection,"DROP VIEW srednie_uczniow");
 return 0;
}


int countMarksByClassOne(char** userCmd, PGconn* connection)
{
 int a,b,i;
 char*** out;
 int idklasy;
 PGresult* pgres;
 pgres=0;
 out=0;
 pgres = execute_query(connection, "SELECT ocena FROM ocena ORDER BY ocena");
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No marks.\n");
    return 1;
   }
 free_p3c(&out,a,b);
 i=getDistinctClass(&idklasy,connection);
 if (i) { printf("Error selecting class!\n"); return 1; }
 *userCmd = (char*)malloc(1024);
 sprintf(*userCmd,
		"SELECT klasa.idklasy,klasa.nazwa,count(ocena) "
		"FROM ocena,uczen,klasa "
		"WHERE ocena.iducznia=uczen.iducznia "
		"AND uczen.idklasy=klasa.idklasy "
		"AND klasa.idklasy=%d "
		"GROUP BY klasa.idklasy,klasa.nazwa "
		"ORDER BY klasa.nazwa",idklasy
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' or 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    execute_query(connection,"DROP VIEW srednie_uczniow");
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No classes or marks..\n");
    return 1;
   }
 printf("Computed all marks.\n");
 for (i=0;i<a;i++) printf("class: %-8s, marks count: %s\n", out[i][1],out[i][2]);
 free_p3c(&out,a,b);
 free(*userCmd);
 *userCmd=0;
 *userCmd = (char*)malloc(1024);
 sprintf(*userCmd,
		"SELECT klasa.idklasy,klasa.nazwa,count(ocena) "
		"FROM ocena,uczen,klasa "
		"WHERE ocena.iducznia=uczen.iducznia "
		"AND uczen.idklasy=klasa.idklasy "
		"AND klasa.idklasy=%d "
		"AND ocena.czyliczona='t' "
		"GROUP BY klasa.idklasy,klasa.nazwa "
		"ORDER BY klasa.nazwa",idklasy
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' or 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    execute_query(connection,"DROP VIEW srednie_uczniow");
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No classes or marks..\n");
    return 1;
   }
 printf("only marks computed in avg.\n");
 for (i=0;i<a;i++) printf("class: %-8s, marks count: %s\n", out[i][1],out[i][2]);
 free_p3c(&out,a,b);
 free(*userCmd);
 *userCmd=0;
 return 0;
}


int countMarksByClass(char** userCmd, PGconn* connection)
{
 int a,b,i;
 char*** out;
 PGresult* pgres;
 pgres=0;
 out=0;
 pgres = execute_query(connection, "SELECT ocena FROM ocena ORDER BY ocena");
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No marks.\n");
    return 1;
   }
 free_p3c(&out,a,b);
 *userCmd = (char*)malloc(1024);
 sprintf(*userCmd,
		"SELECT klasa.idklasy,klasa.nazwa,count(ocena) "
		"FROM ocena,uczen,klasa "
		"WHERE ocena.iducznia=uczen.iducznia "
		"AND uczen.idklasy=klasa.idklasy "
		"GROUP BY klasa.idklasy,klasa.nazwa "
		"ORDER BY klasa.nazwa"
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' or 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    execute_query(connection,"DROP VIEW srednie_uczniow");
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No classes or marks..\n");
    return 1;
   }
 printf("Computed are all marks.\n");
 for (i=0;i<a;i++) printf("class: %-8s, marks count: %s\n", out[i][1],out[i][2]);
 free_p3c(&out,a,b);
 free(*userCmd);
 *userCmd=0;
 *userCmd = (char*)malloc(1024);
 sprintf(*userCmd,
		"SELECT klasa.idklasy,klasa.nazwa,count(ocena) "
		"FROM ocena,uczen,klasa "
		"WHERE ocena.iducznia=uczen.iducznia "
		"AND uczen.idklasy=klasa.idklasy "
		"AND ocena.czyliczona='t' "
		"GROUP BY klasa.idklasy,klasa.nazwa "
		"ORDER BY klasa.nazwa"
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' or 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    execute_query(connection,"DROP VIEW srednie_uczniow");
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No classes or marks..\n");
    return 1;
   }
 printf("Only marks computed in avg.\n");
 for (i=0;i<a;i++) printf("class: %-8s, marks count: %s\n", out[i][1],out[i][2]);
 free_p3c(&out,a,b);
 free(*userCmd);
 *userCmd=0;
 return 0;
}


int averageClassByMarkOne(char** userCmd, PGconn* connection)
{
 int a,b,i;
 char*** out;
 PGresult* pgres;
 int idklasy;
 pgres=0;
 out=0;
 pgres = execute_query(connection, "SELECT ocena FROM ocena ORDER BY ocena");
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No marks cannot compute avg.\n");
    return 1;
   }
 free_p3c(&out,a,b);
 i=getDistinctClass(&idklasy,connection);
 if (i) { printf("Error selecting class!\n"); return 1; }
 *userCmd = (char*)malloc(1024);
 sprintf(*userCmd,
		"SELECT klasa.idklasy,klasa.nazwa,avg(ocena) "
		"FROM ocena,uczen,klasa "
		"WHERE ocena.iducznia=uczen.iducznia "
		"AND uczen.idklasy=klasa.idklasy "
		"AND klasa.idklasy=%d "
		"GROUP BY klasa.idklasy,klasa.nazwa "
		"ORDER BY klasa.nazwa",idklasy
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' or 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    /*execute_query(connection,"DROP VIEW srednie_uczniow");*/
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No classes or marks cannot compute avg.\n");
    return 1;
   }
 printf("Avg below is an avg of all marks of students of given class.\n");
 printf("All marks are computed.\n");
 for (i=0;i<a;i++) printf("class: %-8s, avg: %s\n", out[i][1],out[i][2]);
 free_p3c(&out,a,b);
 free(*userCmd);
 *userCmd=0;
 *userCmd = (char*)malloc(1024);
 sprintf(*userCmd,
		"SELECT klasa.idklasy,klasa.nazwa,avg(ocena) "
		"FROM ocena,uczen,klasa "
		"WHERE ocena.iducznia=uczen.iducznia "
		"AND uczen.idklasy=klasa.idklasy "
		"AND klasa.idklasy=%d "
		"AND ocena.czyliczona='t' "
		"GROUP BY klasa.idklasy,klasa.nazwa "
		"ORDER BY klasa.nazwa",idklasy
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' or 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    /*execute_query(connection,"DROP VIEW srednie_uczniow");*/
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No classes or marks cannot compute avg.\n");
    return 1;
   }
 printf("Avg below is an avg of all marks of students of given class.\n");
 printf("Only mark computed in avg are used.\n");
 for (i=0;i<a;i++) printf("class: %-8s, avg: %s\n", out[i][1],out[i][2]);
 free_p3c(&out,a,b);
 free(*userCmd);
 *userCmd=0;
 return 0;
}


int averageClassOne(char** userCmd, PGconn* connection)
{
 int a,b,i;
 char*** out;
 PGresult* pgres;
 int idklasy;
 pgres=0;
 out=0;
 pgres = execute_query(connection, "SELECT ocena FROM ocena ORDER BY ocena");
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No marks cannot compute avg.\n");
    return 1;
   }
 free_p3c(&out,a,b);
 i=getDistinctClass(&idklasy,connection);
 if (i) { printf("Error selecting class!\n"); return 1; }
 *userCmd = (char*)malloc(1024);
 sprintf(*userCmd,
	"CREATE VIEW srednie_uczniow(iducznia,idklasy,srednia) AS "
	"SELECT uczen.iducznia,klasa.idklasy,avg(ocena) "
	"FROM uczen,ocena,klasa "
	"WHERE uczen.idklasy=klasa.idklasy "
	"AND ocena.iducznia=uczen.iducznia "
	"AND klasa.idklasy=%d "
	"GROUP BY uczen.iducznia,klasa.idklasy ",idklasy
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' or 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 clear_result(&pgres);
 free(*userCmd);
 *userCmd=0;
 *userCmd = (char*)malloc(512);
 sprintf(*userCmd,
	"SELECT klasa.idklasy,klasa.nazwa,avg(srednia) "
	"FROM srednie_uczniow,klasa "
	"WHERE srednie_uczniow.idklasy=klasa.idklasy "
	"GROUP BY klasa.idklasy,klasa.nazwa "
	"ORDER BY klasa.nazwa"
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' or 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    execute_query(connection,"DROP VIEW srednie_uczniow");
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No averages cannot compute avg.\n");
    execute_query(connection,"DROP VIEW srednie_uczniow");
    return 1;
   }
 printf("Avg below in an average of averages of subjects from which students have got marks.\n");
 printf("All subjects are computed\n");
 for (i=0;i<a;i++) printf("class: %-8s, avg: %s\n", out[i][1],out[i][2]);
 free_p3c(&out,a,b);
 free(*userCmd);
 *userCmd=0;
 execute_query(connection,"DROP VIEW srednie_uczniow");
 *userCmd = (char*)malloc(1024);
 sprintf(*userCmd,
	"CREATE VIEW srednie_uczniow(iducznia,idklasy,srednia) AS "
	"SELECT uczen.iducznia,klasa.idklasy,avg(ocena) "
	"FROM uczen,ocena,klasa "
	"WHERE uczen.idklasy=klasa.idklasy "
	"AND ocena.iducznia=uczen.iducznia "
	"AND klasa.idklasy=%d "
	"AND ocena.czyliczona='t' "
	"GROUP BY uczen.iducznia,klasa.idklasy ",idklasy
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' or 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 clear_result(&pgres);
 free(*userCmd);
 *userCmd=0;
 *userCmd = (char*)malloc(512);
 sprintf(*userCmd,
	"SELECT klasa.idklasy,klasa.nazwa,avg(srednia) "
	"FROM srednie_uczniow,klasa "
	"WHERE srednie_uczniow.idklasy=klasa.idklasy "
	"GROUP BY klasa.idklasy,klasa.nazwa "
	"ORDER BY klasa.nazwa"
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' or 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    execute_query(connection,"DROP VIEW srednie_uczniow");
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No averages cannot compute avg.\n");
    execute_query(connection,"DROP VIEW srednie_uczniow");
    return 1;
   }
 printf("Only marks computed in average are used.\n");
 printf("Avg below is an average of averages of subjects from which students have got marks.\n");
 for (i=0;i<a;i++) printf("class: %-8s, avg: %s\n", out[i][1],out[i][2]);
 free_p3c(&out,a,b);
 free(*userCmd);
 *userCmd=0;
 execute_query(connection,"DROP VIEW srednie_uczniow");
 return 0;
}


int averageSchoolByMark2(char** userCmd, PGconn* connection)
{
 int a,b;
 char*** out;
 PGresult* pgres;
 pgres=0;
 out=0;
 pgres = execute_query(connection, "SELECT ocena FROM ocena ORDER BY ocena");
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No marks cannot compute avg.\n");
    return 1;
   }
 free_p3c(&out,a,b);
 *userCmd = (char*)malloc(1024);
 sprintf(*userCmd,
		"CREATE VIEW sredniel(id,nazwa,srednia) AS "
		"SELECT klasa.idklasy,klasa.nazwa,avg(ocena) "
		"FROM ocena,uczen,klasa "
		"WHERE ocena.iducznia=uczen.iducznia "
		"AND uczen.idklasy=klasa.idklasy "
		"GROUP BY klasa.idklasy,klasa.nazwa "
		"ORDER BY klasa.nazwa"
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' or 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 clear_result(&pgres);
 free(*userCmd);
 *userCmd=0;
 *userCmd = (char*)malloc(1024);
 sprintf(*userCmd,
		"SELECT avg(srednia) FROM sredniel"
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' or 'przedmiot' not exists?\n");
    execute_query(connection,"DROP VIEW sredniel");
    printf("%d\n", __LINE__);
    return 1;
   }
 out=pgres2pc3(pgres,&a,&b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No classes or marks, cannot compute avg.\n");
    execute_query(connection, "DROP VIEW sredniel");
    return 1;
   }
 free(*userCmd);
 *userCmd=0;
 execute_query(connection,"DROP VIEW sredniel");
 printf("Avg below is an average of averages of classes computed as\n");
 printf("average of all marks of students of given class.\n");
 printf("computed are all marks.\n");
 printf("school avg: %s\n", out[0][0]);
 free_p3c(&out,a,b);
 *userCmd = (char*)malloc(1024);
 sprintf(*userCmd,
		"CREATE VIEW sredniel(id,nazwa,srednia) AS "
		"SELECT klasa.idklasy,klasa.nazwa,avg(ocena) "
		"FROM ocena,uczen,klasa "
		"WHERE ocena.iducznia=uczen.iducznia "
		"AND uczen.idklasy=klasa.idklasy "
		"AND ocena.czyliczona='t' "
		"GROUP BY klasa.idklasy,klasa.nazwa "
		"ORDER BY klasa.nazwa"
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' or 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 clear_result(&pgres);
 free(*userCmd);
 *userCmd=0;
 *userCmd = (char*)malloc(1024);
 sprintf(*userCmd,
		"SELECT avg(srednia) FROM sredniel"
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' or 'przedmiot' not exists?\n");
    execute_query(connection,"DROP VIEW sredniel");
    printf("%d\n", __LINE__);
    return 1;
   }
 out=pgres2pc3(pgres,&a,&b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No classes or marks, cannot compute avg.\n");
    execute_query(connection, "DROP VIEW sredniel");
    return 1;
   }
 free(*userCmd);
 *userCmd=0;
 execute_query(connection,"DROP VIEW sredniel");
 printf("Avg below is an average of averages of classes computed as\n");
 printf("average of all marks of students of given class.\n");
 printf("Only marks computed in avg are used.\n");
 printf("school avg: %s\n", out[0][0]);
 free_p3c(&out,a,b);
 return 0;
}


int averageClassByMark(char** userCmd, PGconn* connection)
{
 int a,b,i;
 char*** out;
 PGresult* pgres;
 pgres=0;
 out=0;
 pgres = execute_query(connection, "SELECT ocena FROM ocena ORDER BY ocena");
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No marks cannot compute avg.\n");
    return 1;
   }
 free_p3c(&out,a,b);
 *userCmd = (char*)malloc(1024);
 sprintf(*userCmd,
		"SELECT klasa.idklasy,klasa.nazwa,avg(ocena) "
		"FROM ocena,uczen,klasa "
		"WHERE ocena.iducznia=uczen.iducznia "
		"AND uczen.idklasy=klasa.idklasy "
		"GROUP BY klasa.idklasy,klasa.nazwa "
		"ORDER BY klasa.nazwa"
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' or 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    /*execute_query(connection,"DROP VIEW srednie_uczniow");*/
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No classes or marks, cannot compute avg.\n");
    return 1;
   }
 printf("Avg below is an avg of all marks of students of given class.\n");
 printf("computed are all marks.\n");
 for (i=0;i<a;i++) printf("class: %-8s, avg: %s\n", out[i][1],out[i][2]);
 free_p3c(&out,a,b);
 free(*userCmd);
 *userCmd=0;
 *userCmd = (char*)malloc(1024);
 sprintf(*userCmd,
		"SELECT klasa.idklasy,klasa.nazwa,avg(ocena) "
		"FROM ocena,uczen,klasa "
		"WHERE ocena.iducznia=uczen.iducznia "
		"AND uczen.idklasy=klasa.idklasy "
		"AND ocena.czyliczona='t' "
		"GROUP BY klasa.idklasy,klasa.nazwa "
		"ORDER BY klasa.nazwa"
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' or 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    /*execute_query(connection,"DROP VIEW srednie_uczniow");*/
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No classes or marks, cannot compute avg.\n");
    return 1;
   }
 printf("Avg below is an avg of all marks of students of given class.\n");
 printf("Only marks computed in avg are used.\n");
 for (i=0;i<a;i++) printf("class: %-8s, avg: %s\n", out[i][1],out[i][2]);
 free_p3c(&out,a,b);
 free(*userCmd);
 *userCmd=0;
 return 0;
}


int averageClass(char** userCmd, PGconn* connection)
{
 int a,b,i;
 char*** out;
 PGresult* pgres;
 pgres=0;
 out=0;
 pgres = execute_query(connection, "SELECT ocena FROM ocena ORDER BY ocena");
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No marks cannot compute avg.\n");
    return 1;
   }
 free_p3c(&out,a,b);
 *userCmd = (char*)malloc(1024);
 sprintf(*userCmd,
	"CREATE VIEW srednie_uczniow(iducznia,idklasy,srednia) AS "
	"SELECT uczen.iducznia,klasa.idklasy,avg(ocena) "
	"FROM uczen,ocena,klasa "
	"WHERE uczen.idklasy=klasa.idklasy "
	"AND ocena.iducznia=uczen.iducznia "
	"GROUP BY uczen.iducznia,klasa.idklasy "
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' or 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 clear_result(&pgres);
 free(*userCmd);
 *userCmd=0;
 *userCmd = (char*)malloc(512);
 sprintf(*userCmd,
	"SELECT klasa.idklasy,klasa.nazwa,avg(srednia) "
	"FROM srednie_uczniow,klasa "
	"WHERE srednie_uczniow.idklasy=klasa.idklasy "
	"GROUP BY klasa.idklasy,klasa.nazwa "
	"ORDER BY klasa.nazwa"
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' or 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    execute_query(connection,"DROP VIEW srednie_uczniow");
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No averages cannot compute avg.\n");
    execute_query(connection,"DROP VIEW srednie_uczniow");
    return 1;
   }
 printf("Avg below is an average of averages of all subjects from which students of this class have got marks.\n");
 printf("Computed are all marks\n");
 for (i=0;i<a;i++) printf("class: %-8s, avg: %s\n", out[i][1],out[i][2]);
 free_p3c(&out,a,b);
 free(*userCmd);
 *userCmd=0;
 execute_query(connection,"DROP VIEW srednie_uczniow");
 *userCmd = (char*)malloc(1024);
 sprintf(*userCmd,
	"CREATE VIEW srednie_uczniow(iducznia,idklasy,srednia) AS "
	"SELECT uczen.iducznia,klasa.idklasy,avg(ocena) "
	"FROM uczen,ocena,klasa "
	"WHERE uczen.idklasy=klasa.idklasy "
	"AND ocena.iducznia=uczen.iducznia "
	"AND ocena.czyliczona='t' "
	"GROUP BY uczen.iducznia,klasa.idklasy "
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' or 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 clear_result(&pgres);
 free(*userCmd);
 *userCmd=0;
 *userCmd = (char*)malloc(512);
 sprintf(*userCmd,
	"SELECT klasa.idklasy,klasa.nazwa,avg(srednia) "
	"FROM srednie_uczniow,klasa "
	"WHERE srednie_uczniow.idklasy=klasa.idklasy "
	"GROUP BY klasa.idklasy,klasa.nazwa "
	"ORDER BY klasa.nazwa"
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' or 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    execute_query(connection,"DROP VIEW srednie_uczniow");
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No averages cannot compute avg.\n");
    execute_query(connection,"DROP VIEW srednie_uczniow");
    return 1;
   }
 printf("Avg below is an average of averages of all subjects from which students of this class have got marks.\n");
 printf("Only marks computed in avg are used\n");
 for (i=0;i<a;i++) printf("class: %-8s, avg: %s\n", out[i][1],out[i][2]);
 free_p3c(&out,a,b);
 free(*userCmd);
 *userCmd=0;
 execute_query(connection,"DROP VIEW srednie_uczniow");
 return 0;
}


int countMarksStudentInClass(char** userCmd, PGconn* connection)
{
 int a,b,i;
 char*** out;
 int idklasy;
 PGresult* pgres;
 pgres=0;
 out=0;
 pgres = execute_query(connection, "SELECT ocena FROM ocena ORDER BY ocena");
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No marks.\n");
    return 1;
   }
 free_p3c(&out,a,b);
 i=getDistinctClass(&idklasy,connection);
 if (i) { printf("Error selecting class!\n"); return 1; }
 *userCmd = (char*)malloc(1024);
 sprintf(*userCmd,
	"SELECT uczen.iducznia,uczen.nazwisko,uczen.imie,klasa.nazwa,count(ocena) "
	"FROM uczen,ocena,klasa "
	"WHERE uczen.idklasy=klasa.idklasy "
	"AND ocena.iducznia=uczen.iducznia "
	"AND klasa.idklasy=%d "
	"GROUP BY uczen.iducznia,uczen.nazwisko,uczen.imie,klasa.nazwa "
	"ORDER BY uczen.nazwisko",idklasy
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' or 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No marks.\n");
    return 1;
   }
 printf("All marks used.\n");
 for (i=0;i<a;i++) printf("%-18s %-15s %-8s, marks: %s\n", out[i][1],out[i][2],out[i][3],out[i][4]);
 free_p3c(&out,a,b);
 free(*userCmd);
 *userCmd=0;
 *userCmd = (char*)malloc(1024);
 sprintf(*userCmd,
	"SELECT uczen.iducznia,uczen.nazwisko,uczen.imie,klasa.nazwa,count(ocena) "
	"FROM uczen,ocena,klasa "
	"WHERE uczen.idklasy=klasa.idklasy "
	"AND ocena.iducznia=uczen.iducznia "
	"AND ocena.czyliczona='t' "
	"AND klasa.idklasy=%d "
	"GROUP BY uczen.iducznia,uczen.nazwisko,uczen.imie,klasa.nazwa "
	"ORDER BY uczen.nazwisko",idklasy
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' or 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No marks.\n");
    return 1;
   }
 printf("Only marks computed in avg.\n");
 for (i=0;i<a;i++) printf("%-18s %-15s %-8s, marks: %s\n", out[i][1],out[i][2],out[i][3],out[i][4]);
 free_p3c(&out,a,b);
 free(*userCmd);
 *userCmd=0;
 return 0;
}


int countMarksStudent(char** userCmd, PGconn* connection)
{
 int a,b,i;
 char*** out;
 PGresult* pgres;
 pgres=0;
 out=0;
 pgres = execute_query(connection, "SELECT ocena FROM ocena ORDER BY ocena");
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No marks.\n");
    return 1;
   }
 free_p3c(&out,a,b);
 *userCmd = (char*)malloc(1024);
 sprintf(*userCmd,
	"SELECT uczen.iducznia,uczen.nazwisko,uczen.imie,klasa.nazwa,count(ocena) "
	"FROM uczen,ocena,klasa "
	"WHERE uczen.idklasy=klasa.idklasy "
	"AND ocena.iducznia=uczen.iducznia "
	"GROUP BY uczen.iducznia,uczen.nazwisko,uczen.imie,klasa.nazwa "
	"ORDER BY uczen.nazwisko"
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' or 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No marks.\n");
    return 1;
   }
 printf("All marks used.\n");
 for (i=0;i<a;i++) printf("%-18s %-15s %-8s, marks: %s\n", out[i][1],out[i][2],out[i][3],out[i][4]);
 free_p3c(&out,a,b);
 free(*userCmd);
 *userCmd=0;
 *userCmd = (char*)malloc(1024);
 sprintf(*userCmd,
	"SELECT uczen.iducznia,uczen.nazwisko,uczen.imie,klasa.nazwa,count(ocena) "
	"FROM uczen,ocena,klasa "
	"WHERE uczen.idklasy=klasa.idklasy "
	"AND ocena.iducznia=uczen.iducznia "
	"AND ocena.czyliczona='t' "
	"GROUP BY uczen.iducznia,uczen.nazwisko,uczen.imie,klasa.nazwa "
	"ORDER BY uczen.nazwisko"
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' or 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No marks.\n");
    return 1;
   }
 printf("Only marks computed in avg.\n");
 for (i=0;i<a;i++) printf("%-18s %-15s %-8s, marks: %s\n", out[i][1],out[i][2],out[i][3],out[i][4]);
 free_p3c(&out,a,b);
 free(*userCmd);
 *userCmd=0;
 return 0;
}


int averageStudentInClass(char** userCmd, PGconn* connection)
{
 int a,b,i;
 char*** out;
 int idklasy;
 PGresult* pgres;
 pgres=0;
 out=0;
 pgres = execute_query(connection, "SELECT ocena FROM ocena ORDER BY ocena");
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No marks cannot compute avg.\n");
    return 1;
   }
 free_p3c(&out,a,b);
 i=getDistinctClass(&idklasy,connection);
 if (i) { printf("Error selecting class!\n"); return 1; }
 *userCmd = (char*)malloc(1024);
 sprintf(*userCmd,
	"SELECT uczen.iducznia,uczen.nazwisko,uczen.imie,klasa.nazwa,avg(ocena) "
	"FROM uczen,ocena,klasa "
	"WHERE uczen.idklasy=klasa.idklasy "
	"AND ocena.iducznia=uczen.iducznia "
	"AND ocena.czyliczona='t' "
	"AND klasa.idklasy=%d "
	"GROUP BY uczen.iducznia,uczen.nazwisko,uczen.imie,klasa.nazwa "
	"ORDER BY uczen.nazwisko",idklasy
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' or 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No marks cannot compute avg.\n");
    return 1;
   }
 for (i=0;i<a;i++) printf("%-18s %-15s %-8s, avg: %s\n", out[i][1],out[i][2],out[i][3],out[i][4]);
 free_p3c(&out,a,b);
 free(*userCmd);
 *userCmd=0;
 return 0;
}


int averageStudent(char** userCmd, PGconn* connection)
{
 int a,b,i;
 char*** out;
 PGresult* pgres;
 pgres=0;
 out=0;
 pgres = execute_query(connection, "SELECT ocena FROM ocena ORDER BY ocena");
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No marks cannot compute avg.\n");
    return 1;
   }
 free_p3c(&out,a,b);
 *userCmd = (char*)malloc(1024);
 sprintf(*userCmd,
	"SELECT uczen.iducznia,uczen.nazwisko,uczen.imie,klasa.nazwa,avg(ocena) "
	"FROM uczen,ocena,klasa "
	"WHERE uczen.idklasy=klasa.idklasy "
	"AND ocena.iducznia=uczen.iducznia "
	"AND ocena.czyliczona='t' "
	"GROUP BY uczen.iducznia,uczen.nazwisko,uczen.imie,klasa.nazwa "
	"ORDER BY uczen.nazwisko"
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' or 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No marks cannot compute avg.\n");
    return 1;
   }
 for (i=0;i<a;i++) printf("%-18s %-15s %-8s, avg: %s\n", out[i][1],out[i][2],out[i][3],out[i][4]);
 free_p3c(&out,a,b);
 free(*userCmd);
 *userCmd=0;
 return 0;
}


int countSubjectInClass(char** userCmd, PGconn* connection)
{
 int a,b,i,idklasy;
 char*** out;
 PGresult* pgres;
 pgres=0;
 out=0;
 pgres = execute_query(connection, "SELECT ocena FROM ocena ORDER BY ocena");
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No marks.\n");
    return 1;
   }
 free_p3c(&out,a,b);
 i=getDistinctClass(&idklasy,connection);
 if (i) { printf("Error selecting class!\n"); return 1; }
 *userCmd = (char*)malloc(512);
 sprintf(*userCmd,
	"SELECT przedmiot.nazwa,count(ocena) "
	"FROM ocena,przedmiot,uczen,klasa "
	"WHERE ocena.idprzedmiotu=przedmiot.idprzedmiotu "
	"AND ocena.iducznia=uczen.iducznia "
	"AND uczen.idklasy=klasa.idklasy "
	"AND klasa.idklasy=%d "
	"GROUP BY przedmiot.nazwa "
	"ORDER BY przedmiot.nazwa"
	,idklasy);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' or 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No marks.\n");
    return 1;
   }
 for (i=0;i<a;i++) printf("%-32s, marks count: %s\n", out[i][0],out[i][1]);
 free_p3c(&out,a,b);
 free(*userCmd);
 *userCmd=0;
 return 0;
}


int countSubject(char** userCmd, PGconn* connection)
{
 int a,b,i;
 char*** out;
 PGresult* pgres;
 pgres=0;
 out=0;
 pgres = execute_query(connection, "SELECT ocena FROM ocena ORDER BY ocena");
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No marks.\n");
    return 1;
   }
 free_p3c(&out,a,b);
 *userCmd = (char*)malloc(512);
 sprintf(*userCmd,
	"SELECT przedmiot.nazwa,count(ocena) "
	"FROM ocena,przedmiot "
	"WHERE ocena.idprzedmiotu=przedmiot.idprzedmiotu "
	"GROUP BY przedmiot.nazwa ORDER BY przedmiot.nazwa"
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' or 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No marks.\n");
    return 1;
   }
 for (i=0;i<a;i++) printf("%-32s, marks count: %s\n", out[i][0],out[i][1]);
 free_p3c(&out,a,b);
 free(*userCmd);
 *userCmd=0;
 return 0;
}


int averageSubjectInClass(char** userCmd, PGconn* connection)
{
 int a,b,i,idklasy;
 char*** out;
 PGresult* pgres;
 pgres=0;
 out=0;
 pgres = execute_query(connection, "SELECT ocena FROM ocena ORDER BY ocena");
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No marks cannot compute avg.\n");
    return 1;
   }
 free_p3c(&out,a,b);
 i=getDistinctClass(&idklasy,connection);
 if (i) { printf("Error selecting class!\n"); return 1; }
 *userCmd = (char*)malloc(512);
 sprintf(*userCmd,
	"SELECT przedmiot.nazwa,avg(ocena) "
	"FROM ocena,przedmiot,uczen,klasa "
	"WHERE ocena.idprzedmiotu=przedmiot.idprzedmiotu "
	"AND ocena.iducznia=uczen.iducznia "
	"AND uczen.idklasy=klasa.idklasy "
	"AND klasa.idklasy=%d "
	"GROUP BY przedmiot.nazwa "
	"ORDER BY przedmiot.nazwa"
	,idklasy);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' or 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No marks cannot compute avg.\n");
    return 1;
   }
 for (i=0;i<a;i++) printf("%-32s, avg: %s\n", out[i][0],out[i][1]);
 free_p3c(&out,a,b);
 free(*userCmd);
 *userCmd=0;
 return 0;
}


int averageSubject(char** userCmd, PGconn* connection)
{
 int a,b,i;
 char*** out;
 PGresult* pgres;
 pgres=0;
 out=0;
 pgres = execute_query(connection, "SELECT ocena FROM ocena ORDER BY ocena");
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No marks cannot compute avg.\n");
    return 1;
   }
 free_p3c(&out,a,b);
 *userCmd = (char*)malloc(512);
 sprintf(*userCmd,
	"SELECT przedmiot.nazwa,avg(ocena) "
	"FROM ocena,przedmiot "
	"WHERE ocena.idprzedmiotu=przedmiot.idprzedmiotu "
	"GROUP BY przedmiot.nazwa ORDER BY przedmiot.nazwa"
	);
 pgres = execute_query(connection, *userCmd);
 if (!pgres)
   {
    printf("SQL Error, table 'ocena' or 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No marks cannot compute avg.\n");
    return 1;
   }
 for (i=0;i<a;i++) printf("%-32s, avg: %s\n", out[i][0],out[i][1]);
 free_p3c(&out,a,b);
 free(*userCmd);
 *userCmd=0;
 return 0;
}


int removeSubject(char** userCmd, PGconn* connection)
{
 char tmp[1025];
 int len,a,b,i;
 char*** out;
 char* check;
 PGresult* pgres;
 pgres=0;
 out=0;
 check=0;
 pgres = execute_query(connection, "SELECT nazwa FROM przedmiot ORDER BY nazwa");
 if (!pgres)
   {
    printf("SQL Error, table 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No subjects to remove!\n");
    return 1;
   }
 printf("Defined subjects:\n");
 for (i=0;i<a;i++) printf("%s, ",out[i][0]);
 printf("\n");
 free_p3c(&out,a,b);
 printf("Subject name (or part): ");
 takestring(tmp);
 len = strlen(tmp);
 if (len>30) { printf("Name longer than 30!\n"); return 1; }
 check = (char*)malloc(len+200);
 *userCmd = (char*)malloc(len+200);
 lsprintf(check,"SELECT nazwa from przedmiot WHERE nazwa LIKE '|'", tmp);
 pgres = execute_query(connection, check);
 free(check);
 if (!pgres)
   {
    printf("SQL Error, table 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No such subject to remove!\n");
    return 1;
   }
 if (a>1)
   {
    free_p3c(&out,a,b);
    printf("Selection ambiguous, listed subjects match:\n");
    for (i=0;i<a;i++) printf("%s, ", out[i][0]);
    ln();
    return 1;
   }
 free_p3c(&out,a,b);
 if (!*userCmd) { ERR printf("malloc failed!\n"); return 1; }
 lsprintf(*userCmd,"DELETE FROM przedmiot WHERE nazwa LIKE '|'", tmp);
 return 0;
}


int modifySubject(char** userCmd, PGconn* connection)
{
 char tmp[1025],tmp2[1025];
 char* check;
 int len,a,b,i,len2;
 char*** out;
 PGresult* pgres;
 pgres=0;
 check=0;
 out=0;
 pgres = execute_query(connection, "SELECT nazwa FROM przedmiot ORDER BY nazwa");
 if (!pgres)
   {
    printf("SQL Error, table 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No subjects to modify!\n");
    return 1;
   }
 printf("Defined subjects:\n");
 for (i=0;i<a;i++) printf("%s, ",out[i][0]);
 printf("\n");
 free_p3c(&out,a,b);
 printf("Subject name to modify (or part): ");
 takestring(tmp);
 len = strlen(tmp);
 if (len>30) { printf("Name longer than 30!\n"); return 1; }
 *userCmd = (char*)malloc(len+200);
 check   = (char*)malloc(len+200);
 if (!*userCmd || !check) { ERR printf("malloc failed!\n"); return 1; }
 lsprintf(check,"SELECT nazwa from przedmiot WHERE nazwa LIKE '|'", tmp);
 pgres = execute_query(connection, check);
 free(check);
 if (!pgres)
   {
    printf("SQL Error, table 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No such subject!\n");
    return 1;
   }
 if (a>1)
   {
    free_p3c(&out,a,b);
    printf("Selection is ambiguous, %d subjects match!\n",a);
    for (i=0;i<a;i++) printf("%s, ", out[i][0]);
    ln();
    return 1;
   }
 free_p3c(&out,a,b);
 strcpy(tmp,out[0][0]);
 printf("New name for subject '%s': ", tmp);
 takestring(tmp2);
 len2 = strlen(tmp2);
 if (len2>30) { printf("New name longer than 30!\n"); return 1; }
 check = (char*)malloc(350);
 if (!check) { ERR printf("malloc failed!\n"); return 1; }
 sprintf(check,"SELECT nazwa FROM przedmiot WHERE nazwa='%s'", tmp2);
 pgres = execute_query(connection, check);
 free(check);
 if (!pgres)
      {
       printf("SQL Error, table 'przedmiot' not exists?\n");
       printf("%d\n", __LINE__);
       return 1;
      }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (a)
      {
       free_p3c(&out,a,b);
       printf("Already exists!\n");
       strcpy(*userCmd,"");
       return 1;
      }
 free_p3c(&out,a,b);
 sprintf(*userCmd,"UPDATE przedmiot SET nazwa='%s' WHERE nazwa='%s'", tmp2,tmp);
 return 0;
}


int modifyIndexSubject(char** userCmd, PGconn* connection)
{
 char tmp[1025],tmp2[1025];
 char* check;
 int a,b,i,len2;
 char*** out;
 int idprzedmiotu;
 PGresult* pgres;
 pgres=0;
 check=0;
 out=0;
 pgres = execute_query(connection, "SELECT nazwa,idprzedmiotu FROM przedmiot ORDER BY nazwa");
 if (!pgres)
   {
    printf("SQL Error, table 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No subjects to modify!\n");
    return 1;
   }
 printf("Defined subjects:\n");
 for (i=0;i<a;i++) printf("%-10s\t ID=%s\n",out[i][0],out[i][1]);
 printf("\n");
 free_p3c(&out,a,b);
 printf("Subject's ID: ");
 takestring(tmp);
 idprzedmiotu=-1;
 sscanf(tmp,"%d", &idprzedmiotu);
 if (idprzedmiotu<0) { printf("Invalid ID\n"); return 1; }
 *userCmd = (char*)malloc(220);
 check   = (char*)malloc(200);
 if (!*userCmd || !check) { ERR printf("malloc failed!\n"); return 1; }
 sprintf(check,"SELECT nazwa FROM przedmiot WHERE idprzedmiotu=%d", idprzedmiotu);
 pgres = execute_query(connection, check);
 if (!pgres)
   {
    printf("SQL Error, table 'przedmiot' not exists?\n");
    printf("%d\n", __LINE__);
    free(check);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 free(check);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No such subject!\n");
    return 1;
   }
 free_p3c(&out,a,b);
 strcpy(tmp,out[0][0]);
 printf("New name for subject '%s': ", tmp);
 takestring(tmp2);
 len2 = strlen(tmp2);
 if (len2>30) { printf("New name longer than 30!\n"); return 1; }
 check = (char*)malloc(350);
 if (!check) { ERR printf("malloc failed!\n"); return 1; }
 sprintf(check,"SELECT nazwa FROM przedmiot WHERE nazwa='%s'", tmp2);
 pgres = execute_query(connection, check);
 free(check);
 if (!pgres)
      {
       printf("SQL Error, table 'przedmiot' not exists?\n");
       printf("%d\n", __LINE__);
       return 1;
      }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (a)
      {
       free_p3c(&out,a,b);
       printf("Already exists!\n");
       strcpy(*userCmd,"");
       return 1;
      }
 free_p3c(&out,a,b);
 sprintf(*userCmd,"UPDATE przedmiot SET nazwa='%s' WHERE idprzedmiotu=%d", tmp2,idprzedmiotu);
 return 0;
}


int modifyIndexClass(char** userCmd, PGconn* connection)
{
 char tmp[1025],tmp2[1025];
 char* check;
 int a,b,i,len2;
 char*** out;
 int idklasy;
 PGresult* pgres;
 pgres=0;
 check=0;
 out=0;
 pgres = execute_query(connection, "SELECT nazwa,idklasy FROM klasa ORDER BY nazwa");
 if (!pgres)
   {
    printf("SQL Error, table 'klasa' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No classes to modify!\n");
    return 1;
   }
 printf("Oto zdefiniowane klasy:\n");
 for (i=0;i<a;i++) printf("%-10s\t ID=%s\n",out[i][0],out[i][1]);
 printf("\n");
 free_p3c(&out,a,b);
 printf("class ID: ");
 takestring(tmp);
 idklasy=-1;
 sscanf(tmp,"%d", &idklasy);
 if (idklasy<0) { printf("invalid ID\n"); return 1; }
 *userCmd = (char*)malloc(220);
 check   = (char*)malloc(200);
 if (!*userCmd || !check) { ERR printf("malloc failed!\n"); return 1; }
 sprintf(check,"SELECT nazwa from klasa WHERE idklasy=%d", idklasy);
 pgres = execute_query(connection, check);
 if (!pgres)
   {
    printf("SQL Error, table 'klasa' not exists?\n");
    printf("%d\n", __LINE__);
    free(check);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 free(check);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No such class!\n");
    return 1;
   }
 free_p3c(&out,a,b);
 strcpy(tmp,out[0][0]);
 printf("New name for class '%s': ", tmp);
 takestring(tmp2);
 len2 = strlen(tmp2);
 if (len2>7) { printf("New name longer than 7!\n"); return 1; }
 sprintf(*userCmd,"UPDATE klasa SET nazwa='%s' WHERE idklasy=%d", tmp2,idklasy);
 check = (char*)malloc(350);
 if (!check) { ERR printf("malloc failed!\n"); return 1; }
 sprintf(check,"SELECT nazwa FROM klasa WHERE nazwa='%s'", tmp2);
 pgres = execute_query(connection, check);
 free(check);
 if (!pgres)
      {
       printf("SQL Error, table 'klasa' not exists?\n");
       printf("%d\n", __LINE__);
       return 1;
      }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (a)
      {
       free_p3c(&out,a,b);
       printf("Such class already exists!\n");
       strcpy(*userCmd,"");
       return 1;
      }
 free_p3c(&out,a,b);
 return 0;
}


int modifyClass(char** userCmd, PGconn* connection)
{
 char tmp[1025],tmp2[1025];
 char* check;
 int len,a,b,i,len2;
 char*** out;
 PGresult* pgres;
 pgres=0;
 check=0;
 out=0;
 pgres = execute_query(connection, "SELECT nazwa FROM klasa ORDER BY nazwa");
 if (!pgres)
   {
    printf("SQL Error, table 'klasa' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No classes to modify!\n");
    return 1;
   }
 printf("Defined classes:\n");
 for (i=0;i<a;i++) printf("%s, ",out[i][0]);
 printf("\n");
 free_p3c(&out,a,b);
 printf("Class name to modify (or part): ");
 takestring(tmp);
 len = strlen(tmp);
 if (len>7) { printf("Name longer than 7!\n"); return 1; }
 *userCmd = (char*)malloc(len+200);
 check   = (char*)malloc(len+200);
 if (!*userCmd || !check) { ERR printf("malloc failed!\n"); return 1; }
 lsprintf(check,"SELECT nazwa from klasa WHERE nazwa LIKE '|'", tmp);
 pgres = execute_query(connection, check);
 if (!pgres)
   {
    printf("SQL Error, table 'klasa' not exists?\n");
    printf("%d\n", __LINE__);
    free(check);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 free(check);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No such class!\n");
    return 1;
   }
 if (a>1)
   {
    free_p3c(&out,a,b);
    printf("Selection is ambiguous, %d classes listed below match\n",a);
    for (i=0;i<a;i++) printf("%s, ", out[i][0]);
    ln();
    return 1;
   }
 free_p3c(&out,a,b);
 strcpy(tmp,out[0][0]);
 printf("New name for class '%s': ", tmp);
 takestring(tmp2);
 len2 = strlen(tmp2);
 if (len2>7) { printf("New name longer than 7!\n"); return 1; }
 check = (char*)malloc(350);
 if (!check) { ERR printf("malloc failed!\n"); return 1; }
 sprintf(check,"SELECT nazwa FROM klasa WHERE nazwa='%s'", tmp2);
 pgres = execute_query(connection, check);
 free(check);
 if (!pgres)
      {
       printf("SQL Error, table 'klasa' not exists?\n");
       printf("%d\n", __LINE__);
       return 1;
      }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (a)
      {
       free_p3c(&out,a,b);
       printf("Such class already exists!\n");
       strcpy(*userCmd,"");
       return 1;
      }
 free_p3c(&out,a,b);
 sprintf(*userCmd,"UPDATE klasa SET nazwa='%s' WHERE nazwa='%s'", tmp2,tmp);
 return 0;
}


int removeClass(char** userCmd, PGconn* connection)
{
 char tmp[1025];
 int len,a,b,i;
 char*** out;
 char* check;
 PGresult* pgres;
 pgres=0;
 out=0;
 check=0;
 pgres = execute_query(connection, "SELECT nazwa FROM klasa ORDER BY nazwa");
 if (!pgres)
   {
    printf("SQL Error, table 'klasa' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("no classes to remove!\n");
    return 1;
   }
 printf("Defined classes:\n");
 for (i=0;i<a;i++) printf("%s, ",out[i][0]);
 printf("\n");
 free_p3c(&out,a,b);
 printf("Class name to remove (or part): ");
 takestring(tmp);
 len = strlen(tmp);
 if (len>7) { printf("Name longer than 7!\n"); return 1; }
 *userCmd = (char*)malloc(len+200);
 check = (char*)malloc(len+200);
 if (!*userCmd || !check) { ERR printf("malloc failed!\n"); return 1; }
 lsprintf(check,"SELECT nazwa from klasa WHERE nazwa LIKE '|'", tmp);
 pgres = execute_query(connection, check);
 free(check);
 if (!pgres)
   {
    printf("SQL Error, table 'klasa' not exists?\n");
    printf("%d\n", __LINE__);
    return 1;
   }
 out = pgres2pc3(pgres, &a, &b);
 clear_result(&pgres);
 if (!a)
   {
    free_p3c(&out,a,b);
    printf("No such class to remove!\n");
    return 1;
   }
 if (a>1)
   {
    free_p3c(&out,a,b);
    printf("Selection is ambiguous, %d classes listed below match\n",a);
    for (i=0;i<a;i++) printf("%s, ", out[i][0]);
    ln();
    return 1;
   }
 free_p3c(&out,a,b);
 lsprintf(*userCmd,"DELETE FROM klasa WHERE nazwa LIKE '|'", tmp);
 return 0;
}


void displayHelp()
{
 printf("DBclass is a program to manage classes,students and school\n");
 printf("It uses PostgreSQL version >= 7.4 (7.2,7.3 might work)\n");
 printf("It is a part of OpenSource project PgXexplorer starting from v0.29\n");
 printf("Program uses DB:morgoth, instructions about installing in README\n");
 printf("NOTE: if You delete class, You'll delete it's students too\n");
 printf("The same with subject. Removing student, dletes his/her marks.\n");
 printf("Version is just translated from polish *AND* there can be mistranslations\n");
 printf("Or accidently untranslated strings left, send bugs to: morgothdbma@o2.pl\n");
 printf("And to PgXexplorer project on gborg.postgresql.org, tel: 693582014\n");
 printf("Version: alpha 0.29\n");
 printf("Version: beta 0.31 (with amd64 support):\n\t%s %s\n",__DATE__,__TIME__);
}


void deleteViews (PGconn* connection)
{
 execute_silent_query(connection,"DROP VIEW srednie_klas");
 execute_silent_query(connection,"DROP VIEW srednie_uczniow");
 execute_silent_query(connection,"DROP VIEW sredniel");
}


void saveDBToFile()
{
 char fname[1024];
 char syscmd[1500];
 printf("Enter filename (without .SQL extension): ");
 takestring(fname);
 sprintf(syscmd,"pg_dump %s > %s.SQL", DBNAME, fname);
 system(syscmd);
}


int main(int lb, char** par)
{
 PGconn* connection;
 PGresult* pgres;
 char*** out;
 char* preparedStatement;
 int a,b,done,zn,err,z2;
 connection=0;
 pgres=0;
 out=0;
 a=b=0;
 done=0;
 preparedStatement=0;
 connection = connect_db(DBNAME);
 displayHelp();
 if (!connection) { printf("Error connect to %s!\n", DBNAME); return 2; }
 clear_result(&pgres);
 while (!done)
 {
  if (preparedStatement) { free(preparedStatement); preparedStatement=0; }
  printf("\n>>>\t");
  zn = mgetch();
  if (zn>='A' && zn<='Z') zn+=0x20;
  putchar('\n');
  if (zn=='s')
    {
     err=prepareUser(&preparedStatement);
     if (err) { printf("error occured!\n"); continue; }
    }
  else if (zn=='b')
    {
     printf("\nu\tdrop temporary tables ...\n");
     printf("z\tsave DB to file ...\n");
     printf("\n\n>>>\t");
     z2 = mgetch();
     if (z2>='A' && z2<='Z') z2+=0x20;
     putchar('\n');
     if (z2=='u')      deleteViews(connection);
     else if (z2=='z') saveDBToFile();
     continue;
    }
  else if (zn=='p' || zn=='h' || zn=='?')
    {
     displayHelp();
     printf("\n\n");
     printf("q\tquit program\n");
     printf("s\ttypein SQL query by hand\n");
     printf("d\tadd ...\n");
     printf("u\tremove ...\n");
     printf("z\tsee ...\n");
     printf("m\tmodify ...\n");
     printf("i\tmodify using sysID ...\n");
     printf("r\treports ...\n");
     printf("w\tsearch methods ...\n");
     printf("b\tdatabase ...\n");
     printf("p,h,?\thelp\n");
     continue;
    }
  else if (zn=='w')
    {
     printf("\np\tmatch from begin EXP* ...\n");
     printf("k\tmatch to end *EXP ...\n");
     printf("s\tmatch inside *EXP* ...\n");
     printf("c\tmatch exactly EXP ...\n");
     printf("\n\n>>>\t");
     z2 = mgetch();
     if (z2>='A' && z2<='Z') z2+=0x20;
     putchar('\n');
     if (z2=='p')      m_mode = M_BEGIN;
     else if (z2=='k') m_mode = M_END;
     else if (z2=='s') m_mode = M_MIDDLE;
     else if (z2=='c') m_mode = M_NONE;
     continue;
    }
  else if (zn=='i')
    {
     printf("\nk\tclass ...\n");
     printf("p\tsubject ...\n");
     printf("u\tstudent ...\n");
     printf("\n\n>>>\t");
     z2 = mgetch();
     if (z2>='A' && z2<='Z') z2+=0x20;
     putchar('\n');
     if (z2=='k')
      {
       err=modifyIndexClass(&preparedStatement,connection);
       if (err) { printf("Error changing classID.\n"); continue; }
      }
     if (z2=='p')
      {
       err=modifyIndexSubject(&preparedStatement,connection);
       if (err) { printf("Error changing subjectID.\n"); continue; }
      }
     if (z2=='u')
      {
       err=modifyIndexStudent(&preparedStatement,connection);
       if (err) { printf("Error changing studentID.\n"); continue; }
      }
    }
  else if (zn=='d')
    {
     printf("\nk\tclass ...\n");
     printf("p\tsubject ...\n");
     printf("u\tstudent ...\n");
     printf("c\tstudent to class ...\n");
     printf("o\tmark ...\n");
     printf("r\tmark by subject ...\n");
     printf("z\tmark by student ...\n");
     printf("l\tmarks to class ...\n");
     printf("\n\n>>>\t");
     z2 = mgetch();
     if (z2>='A' && z2<='Z') z2+=0x20;
     putchar('\n');
     if (z2=='k')
       {
        err=addClass(&preparedStatement,connection);
        if (err)
	  { printf("Error adding class!\n"); continue; }
       }
     if (z2=='p')
       {
        err=addSubject(&preparedStatement,connection);
        if (err) { printf("Error adding subject\n"); continue; }
       }
     if (z2=='u')
       {
        err=addStudent(&preparedStatement,connection);
        if (err) { printf("Error adding student!\n"); continue; }
       }
     if (z2=='c')
       {
        err=addStudentsToClass(connection);
	if (!err) printf("Added all students.\n");
	else printf("Error adding students.\n");
	continue;
       }
     if (z2=='o')
       {
        err=addMark(&preparedStatement,connection);
        if (err) { printf("Error adding mark.\n"); continue; }
       }
     if (z2=='r')
       {
        err=addMarkBySubject(&preparedStatement,connection);
	if (!err) printf("Added all marks.\n");
	else printf("Error adding marks.\n");
        continue;
       }
     if (z2=='z')
       {
        err=addMarkByStudent(&preparedStatement,connection);
	if (!err) printf("Added all marks.\n");
	else printf("Error adding marks.\n");
        continue;
       }
     if (z2=='l')
       {
        err=addMarkByClass(&preparedStatement,connection);
	if (!err) printf("Added all marks.\n");
	else printf("Error adding marks.\n");
        continue;
       }
    }
  else if (zn=='m')
    {
     printf("\nk\tclass ...\n");
     printf("p\tsubject ...\n");
     printf("u\tstudent ...\n");
     printf("o\tmark ...\n");
     printf("\n\n>>>\t");
     z2 = mgetch();
     if (z2>='A' && z2<='Z') z2+=0x20;
     putchar('\n');
     if (z2=='k')
       {
        err=modifyClass(&preparedStatement,connection);
        if (err) { printf("Error modyfying class!\n"); continue; }
       }
     if (z2=='p')
       {
        err=modifySubject(&preparedStatement,connection);
        if (err) { printf("Error modifying subject!\n"); continue; }
       }
     if (z2=='u')
       {
        err=modifyStudent(&preparedStatement,connection);
        if (err) { printf("Error modifying student!\n"); continue; }
       }
     if (z2=='o')
       {
        err=modifyMark(&preparedStatement,connection);
        if (err) { printf("Error modifying mark!\n"); continue; }
       }
    }
  else if (zn=='u')
    {
     printf("\nk\tclass ...\n");
     printf("p\tsubject ...\n");
     printf("u\tstudent ...\n");
     printf("o\tmark ...\n");
     printf("\n\n>>>\t");
     z2 = mgetch();
     if (z2>='A' && z2<='Z') z2+=0x20;
     putchar('\n');
     if (z2=='k')
       {
        err=removeClass(&preparedStatement,connection);
        if (err) { printf("Error removing class!\n"); continue; }
       }
     if (z2=='p')
       {
        err=removeSubject(&preparedStatement,connection);
        if (err) { printf("Error removing subject!\n"); continue; }
       }
     if (z2=='u')
       {
        err=removeStudent(&preparedStatement,connection);
        if (err) { printf("Error removing student!\n"); continue; }
       }
     if (z2=='o')
       {
        err=removeMark(&preparedStatement,connection);
        if (err) { printf("Error removing mark!\n"); continue; }
       }
    }
  else if (zn=='z')
    {
     printf("\nk\tclass ...\n");
     printf("p\tsubject ...\n");
     printf("u\tstudent ...\n");
     printf("o\tmark ...\n");
     printf("r\tmark from subject ...\n");
     printf("z\tstudent's mark ...\n");
     printf("w\tmarks (all) ...\n");
     printf("\n\n>>>\t");
     z2 = mgetch();
     if (z2>='A' && z2<='Z') z2+=0x20;
     putchar('\n');
     if (z2=='k')
       {
        seeClasses(&preparedStatement,connection);
        continue;
       }
     if (z2=='p')
       {
        seeSubjects(&preparedStatement,connection);
        continue;
       }
     if (z2=='u')
       {
        seeStudents(&preparedStatement,connection);
        continue;
       }
     if (z2=='o')
       {
        seeMarks(&preparedStatement,connection);
        continue;
       }
     if (z2=='r')
       {
        seeMarksBySubject(&preparedStatement,connection);
        continue;
       }
     if (z2=='z')
       {
        seeMarksByStudents(&preparedStatement,connection);
        continue;
       }
     if (z2=='w')
       {
        seeMarksAll(&preparedStatement,connection);
        continue;
       }
    }
  else if (zn=='r')
    {
     printf("\nk\taverages of all classes (average of class subjects)...\n");
     printf("l\taverages of all classes (average of marks)...\n");
     printf("m\taverage of selected class (average of class subjects)...\n");
     printf("n\taverage of selected class (average of marks)...\n");
     printf("w\tsubjects averages (from all classes) ...\n");
     printf("p\tsubjects averages (in given class) ...\n");
     printf("u\tstudents averages (from all classes)...\n");
     printf("z\tstudents averages (in given class)...\n");
     printf("s\tschool average (average of averages of classes 'k')...\n");
     printf("x\tschool average (average of averages of classes 'l')...\n");
     printf("v\tschool average (average of all marks)...\n");
     printf("o\tmarks count in all classes ...\n");
     printf("q\tmarks count in given class ...\n");
     printf("r\tmarks count from subjects in all classes ...\n");
     printf("t\tmarks count from subjects in all classes...\n");
     printf("d\tstudents count in classes\n");
     printf("e\tstudents count in school\n");
     printf("a\tstudents marks count in all classes\n");
     printf("b\tstudents marks count in given class...\n");
     printf("c\tmarks count in the school ...\n");
     printf("\t'k' means average of averages of subjects in given class\n");
     printf("\t'l' means average of all marks of students in given class\n");
     printf("\n\n>>>\t");
     z2 = mgetch();
     if (z2>='A' && z2<='Z') z2+=0x20;
     putchar('\n');
     if (z2=='k')
       {
        err=averageClass(&preparedStatement,connection);
        if (err) printf("Error computing avg!\n");
	continue;
       }
     if (z2=='l')
       {
        err=averageClassByMark(&preparedStatement,connection);
        if (err) printf("Error computing avg!\n");
	continue;
       }
     if (z2=='x')
       {
        err=averageSchoolByMark2(&preparedStatement,connection);
        if (err) printf("Error computing avg!\n");
	continue;
       }
     if (z2=='q')
       {
        err=countMarksByClassOne(&preparedStatement,connection);
        if (err) printf("Error computing!\n");
	continue;
       }
     if (z2=='o')
       {
        err=countMarksByClass(&preparedStatement,connection);
        if (err) printf("Error computing!\n");
	continue;
       }
     if (z2=='m')
       {
        err=averageClassOne(&preparedStatement,connection);
        if (err) printf("Error computing avg!\n");
	continue;
       }
     if (z2=='n')
       {
        err=averageClassByMarkOne(&preparedStatement,connection);
        if (err) printf("Error computing avg!\n");
	continue;
       }
     if (z2=='s')
       {
        err=averageSchool(&preparedStatement,connection);
        if (err) printf("Error computing avg!\n");
	continue;
       }
     if (z2=='v')
       {
        err=averageSchoolByMark(&preparedStatement,connection);
        if (err) printf("Error computing avg!\n");
	continue;
       }
     if (z2=='c')
       {
        err=countMarksSchool(&preparedStatement,connection);
        if (err) printf("Error computing!\n");
	continue;
       }
     if (z2=='d')
       {
        err=countStudentsInClass(&preparedStatement,connection);
        if (err) printf("Error computing!\n");
	continue;
       }
     if (z2=='e')
       {
        err=countStudentsInSchool(&preparedStatement,connection);
        if (err) printf("Error computing!\n");
	continue;
       }
     if (z2=='r')
       {
        err=countSubject(&preparedStatement,connection);
        if (err) printf("Error computing!\n");
	continue;
       }
     if (z2=='t')
       {
        err=countSubjectInClass(&preparedStatement,connection);
        if (err) printf("Error computing!\n");
	continue;
       }
     if (z2=='w')
       {
        err=averageSubject(&preparedStatement,connection);
        if (err) printf("Error computing avg!\n");
	continue;
       }
     if (z2=='p')
       {
        err=averageSubjectInClass(&preparedStatement,connection);
        if (err) printf("Error computing avg!\n");
	continue;
       }
     if (z2=='u')
       {
        err=averageStudent(&preparedStatement,connection);
        if (err) printf("Error computing avg!\n");
	continue;
       }
     if (z2=='z')
       {
        err=averageStudentInClass(&preparedStatement,connection);
        if (err) printf("Error computing avg!\n");
	continue;
       }
     if (z2=='a')
       {
        err=countMarksStudent(&preparedStatement,connection);
        if (err) printf("Error computing!\n");
	continue;
       }
     if (z2=='b')
       {
        err=countMarksStudentInClass(&preparedStatement,connection);
        if (err) printf("Error computing!\n");
	continue;
       }
    }
  else if (zn=='q') { done=1; continue; }
  else { printf("Unknown command...\n"); continue; }
  pgres = execute_query(connection, preparedStatement);
  if (!pgres)
   {
    printf("SQL Error\n");
    printf("%d\n", __LINE__);
   }
  out = pgres2pc3(pgres, &a, &b);
  if (zn=='s') print_results(out,a,b);
  free_p3c(&out,a,b);
  if (preparedStatement) { free(preparedStatement); preparedStatement=0; }
  clear_result(&pgres);
 }
 disconnect_db(&connection);
 printf("Written by MorgothDBMA: morgothdbma@o2.pl, tel: 693582014\n");
 printf("Version: alpha 0.29\n");
 printf("Version: beta 0.31 (with amd64 support):\n\t%s %s\n",__DATE__,__TIME__);
 return 0;
}

