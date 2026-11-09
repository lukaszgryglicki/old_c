#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <mysql.h>
#include <string.h>
#include <termios.h>
#include <stdlib.h>
#define MAXPATH 1024
static MYSQL *connection=NULL;
static MYSQL mysql;

typedef struct List
{
 char* data;
 List* next;
 List* prev;
};
typedef List* pList;

pList add_to_last(pList& h, char* e)
{
 if (!h)
   {
    h = new List;
    if (!h) return NULL;
    h->next = h->prev = NULL;
    h->data = new char[strlen(e)+1];
    strcpy(h->data, e);
    return h;
   }
 pList tmp = new List;
 if (!tmp) return NULL;
 tmp->next = NULL;
 tmp->prev = h;
 h->next = tmp;
 tmp->data = new char[strlen(e)+1];
 strcpy(tmp->data, e);
 return tmp;
}

pList head=NULL, current=NULL, new_cmds=NULL;

int getch()
{
 struct termios st,end;
 tcgetattr(fileno(stdin),&st);
 end = st;
 end.c_lflag &= ~ICANON;
 end.c_lflag &= ~ECHO;
 end.c_cc[VMIN] = 1;
 end.c_cc[VTIME] = 0;
 if (tcsetattr(fileno(stdin),TCSAFLUSH,&end)) return 0;
 st:
 int ret = 0;
 ret = getchar();
 if (ret == 10) goto st;
 tcsetattr(fileno(stdin),TCSAFLUSH,&st);
 putchar(ret);
 return ret;
}

#define _IMPL_ printf("Not yet programmed!\n");

inline void downcase(int& str)
{
  if ((str >= 'A') && (str <= 'Z')) str += (int)0x20;
}


void takestring(char*& par)
{
 struct termios st,end;
 tcgetattr(fileno(stdin),&st);
 end = st;
 end.c_lflag &= ~ICANON;
 end.c_lflag &= ~ECHO;
 end.c_cc[VMIN] = 1;
 end.c_cc[VTIME] = 0;
 if (tcsetattr(fileno(stdin),TCSAFLUSH,&end) != 0) return;
 int i=0;
 int zn;
 pList tmp=current;
 st:
 par[0] = 0;
 while (i<511)
	{
	 par[i] = getchar();
	 if ((i==0) && (par[i] == 10)) goto cont;
	 if (par[i] == 10) {par[i] = '\0';goto lab;}
	 if (par[i] == 8) {if (i>0)printf("\b \b");i--;if (i<0) {i=0;}goto cont;}
	 if (par[i] == 27)
	 {
	  zn=getchar();
	  zn=getchar();
	  if (zn == 0x42)
	   {
	    if (!tmp)
	      {
	       printf("\n(null)\n");
	       i=0;
	       goto st;
	      }
	    i=0;
	    strcpy(par, tmp->data);
	    printf("\n(old) %s",par);
	    i=strlen(par);
	    if (tmp->next) tmp=tmp->next;
	    goto cont;
	   }
	  else if (zn == 0x41)
	   {
	    if (!tmp)
	      {
	       printf("\n(null)\n");
	       i=0;
	       goto st;
	      }
	    i=0;
	    strcpy(par, tmp->data);
	    printf("\n(old) %s",par);
	    i=strlen(par);
	    if (tmp->prev) tmp=tmp->prev;
	    goto cont;
	   }
	  else goto st;
	 }
	 //printf(">> 0x%x << ", par[i]);
	 putchar(par[i]);
	 i++;
	 cont:;
	}
 lab:
 if (i==0) {strcpy(par,"(null)");printf("(null)\n"); }
 putchar('\n');
 tcsetattr(fileno(stdin),TCSAFLUSH,&st);
}


void downcase(char* str)
{
 if (!str) return;
 for (int i=0;i<(int)strlen(str);i++)
	{
    if ((str[i] >= 'A') && (str[i] <= 'Z')) str[i] += (char)0x20;
   }
}


int connect()
{
 mysql_init(&mysql);
 connection = mysql_real_connect(&mysql, "localhost", "morgoth", "", "KLASA", 0, "/tmp/mysql.sock", 0);
 if (!connection) { printf(mysql_error(&mysql)); return 1; }
 return 0;
}


void Free2(char**& t, int co, int ro)
{
 if (!t) return;
 if (co<=0 || ro<=0) return;
 for (int i=0;i<ro;i++) if (t[i]) { delete [] t[i]; t[i] = NULL; }
 if (t) delete [] t;
 t=NULL;
}


void Free3(char***& t, int co, int ro)
{
 if (!t) return;
 if (co<=0 || ro<=0) return ;
 for (int i=0;i<ro;i++)
 {
  for (int j=0;j<co;j++) if (t[i][j]) { delete [] t[i][j]; t[i][j] = NULL; }
  if (t[i]) { delete [] t[i]; t[i] = NULL; }
 }
 if (t) delete [] t;
 t=NULL;
}


inline void Perr(int er)
{
 if (er) printf("\nMySQL nie akceptuje zapytania.\n");
}


char** P3C2P2C(char*** in, int cols, int rows)
{
 if (!in) return NULL;
 if (cols<=0 || rows<=0) return NULL;
 char** out = new char*[rows];
 int len = 1;
 for (int i=0;i<rows;i++)
   {
    len = 1;
    for (int j=0;j<cols;j++) len += (strlen(in[i][j])+3);
	out[i] = new char[len];
	strcpy(out[i],"");
    for (int j=0;j<cols;j++)
	  {
       strcat(out[i], in[i][j]);
	   strcat(out[i], " ");
	  }
   }
 return out;
}


char*** Q(char* stat, int& cols, int& rows, int& err)
{
 int state = mysql_query(connection, stat);
 cols=rows=err=0;
 if (state) { printf(mysql_error(connection)); err=1; return NULL; }
 MYSQL_RES* result = mysql_store_result(connection);
 MYSQL_ROW row;
 if (!result) {rows=cols=0; return NULL; }
 rows=mysql_num_rows(result);
 cols=mysql_num_fields(result);
 if (!rows || !cols) {rows=cols=0; return NULL; }
 char*** Unit = new char**[rows];
 for (int i=0;i<rows;i++) Unit[i] = new char*[cols];
 int x=0;
 while ((row=mysql_fetch_row(result)))
  {
   for (int i=0;i<cols;i++)
     {
      Unit[x][i] = new char[strlen(row[i]+1)];
      strcpy(Unit[x][i], row[i]);
     }
   x++;
  }
 mysql_free_result(result);
 return Unit;
}


void help()
{
 putchar('\n');
 FILE* helpf=fopen("./HELP", "r");
 if (!helpf) { printf("Brak pliku pomocy, ale na pewno dzialaja zapytania sql\nJesli jestes administratorem SQl to wpisz sql zapytanie np:\n\tsql select * from przedmioty\nAby uruchomic baze wpisz po prostu mysql\nPamietaj, ze w poleceniach sa rozrozniane litery np sql to co innego niz Sql\na w kwerendach nie wiec sql select * from Bla to to samo co:\nsql SELECT * from bLA\n"); return; }
 int zn;
 while ((zn=fgetc(helpf))!=EOF) putchar(zn);
 fclose(helpf);
}


void getinput(char*& cmd)
{
 printf("KLASA@MySQL> ");
 takestring(cmd);
 //fgets(cmd, 2047, stdin);
 //printf(">> %s\n", cmd);
}


int white(char zn)
{
 if (zn==' ' || zn=='\t') return 1;
 else return 0;
}


char** parser(char* cmd, int& lbw)
{
 lbw=0;
 if (!cmd) return NULL;
 lbw++;
 int wh=0;
 if (white(cmd[0])) wh=1;
 int cnt=1;
 int maxcnt=cnt;
 for (int i=0;i<(int)strlen(cmd);i++)
   {
    if (white(cmd[i]))
      {
       if (!wh) lbw++;
       wh=1;
       cnt=1;
      }
    else
      {
       wh=0;
       cnt++;
      }
    if (cnt>maxcnt)maxcnt=cnt;
   }
 if (wh) lbw--;
 char** res = new char*[lbw];
 if (!res) return NULL;
 for (int i=0;i<lbw;i++) res[i] = new char[maxcnt+1];
 int cur=0;
 int index=0;
 wh=1;
 for (int i=0;i<(int)strlen(cmd);i++)
   {
    if (white(cmd[i]))
      {
       if (!wh) { res[cur][index]=0; cur++; index=0; }
       wh=1;
      }
    else
      {
       wh=0;
       res[cur][index]=cmd[i];
       index++;
      }
   }
 res[cur][index]=0;
 return res;
}

#define prepare_cmdline prepare_SQL_stmt

char* prepare_SQL_stmt(char** words, int from, int max)
{
 if (!words) return NULL;
 if (from>=max) return NULL;
 int len=1;
 for (int i=from;i<max;i++) len += strlen(words[i])+1;
 char* ret = new char[len+1];
 strcpy(ret,"");
 for (int i=from;i<max;i++) { strcat(ret, words[i]); strcat(ret," "); }
 return ret;
}

char* insert_class_query(char** words, int from, int max)
{
 _IMPL_
}

char* insert_student_query(char** words, int from, int max)
{
 if (!words) return NULL;
 char* ret;
 int len=1;
 if (from==max)
   {
    printf("<Dodaj ucznia> wpisz polecenie: klasa, nazwisko, imie, numer_w_dzienniku\n==> ");
    ret = new char[512];
    takestring(ret);
    goto manually_set;
   }
 if (from>=max) return NULL;
 for (int i=from;i<max;i++) len += strlen(words[i])+1;
 ret = new char[len+256];
 strcpy(ret,"");
 for (int i=from;i<max;i++) { strcat(ret, words[i]); strcat(ret," "); }
 //printf("RET=%s\n", ret);
 manually_set:
 char* imie		= new char[len+1];
 char* nazwisko		= new char[len+1];
 char* klasa		= new char[len+1];
 int numer=-1;
 strcpy(imie,"");
 strcpy(nazwisko,"");
 strcpy(klasa,"");
 int ile_rd = sscanf(ret,"%s %s %s %d", klasa, nazwisko, imie, &numer);
 if (ile_rd<4)
   {
    printf("Za malo danych dla 'dodaj ucznia', przeczytaj pomoc\n");
    delete [] imie;
    delete [] nazwisko;
    delete [] klasa;
    return NULL;
   }
 //printf("imie=%s, nazw=%s, klasa=%s, numer=%s, ile_rd=%d\n", imie, nazwisko, klasa, numer, ile_rd);
 char query[512];
 sprintf(query,"select idklasy from klasa where nazwa='%s'", klasa);
 int co=0,ro=0,err=0;
 char*** r = Q(query,co,ro,err);
 Perr(err);
 if (!r || ro>1 || err)
   {
    printf("Nie istnieje klasa '%s', uzyj 'dodaj klase'\nAlbo identyfikator '%s' jest niejednoznaczny\n", klasa, klasa);
    delete [] imie;
    delete [] nazwisko;
    delete [] klasa;
    Free3(r,co,ro);
    return NULL;
   }
 int idkl;
 sscanf(r[0][0], "%d", &idkl);
 sprintf(query,"select iducznia from uczen where numer=%d", numer);
 Free3(r,co,ro);
 r = Q(query,co,ro,err);
 Perr(err);
 if (r)
   {
    Free3(r,co,ro);
    printf("Numer (%d) jez juz w bazie SQL, kontynuowac? (t/n) ", numer);
    int zn=getch();
    downcase(zn);
    putchar('\n');
    if (zn=='n')
      {
       delete [] imie;
       delete [] nazwisko;
       delete [] klasa;
       return NULL;
      }
    printf("OK, ale wieloznaczne numery moga utrudnic niektore obliczenia...\n");
   }
 sprintf(ret,"insert into uczen(Imie,Nazwisko,Numer,IDklasy) values('%s','%s',%d,%d)", imie, nazwisko, numer,idkl);
 delete [] imie;
 delete [] nazwisko;
 delete [] klasa;
 return ret;
}


char* prepare_insert_query(char**& words, int from, int& max)
{
 if (!words) return NULL;
 if (from==max)
   {
    printf("<Dodaj> wpisz polecenie: ucznia, przedmiot, klase, ocene, frekwencje\n==> ");
    char** tab;
    tab = new char*[2];
    tab[0] = new char[strlen(words[0])+2];
    strcpy(tab[0],words[0]);
    tab[1] = new char[512];
    takestring(tab[1]);
    for (int i=0;i<=from;i++) if (words[i]) { delete [] words[i]; words[i] = NULL; }
    if (words) delete [] words;
    words = tab;
    max++;
   }
 if (from>=max) return NULL;
 char* ret=NULL;
 if (!strcmp(words[from], "ucznia")) ret=insert_student_query(words, from+1,max);
 if (!strcmp(words[from], "klase")) ret=insert_class_query(words, from+1,max);
 return ret;
}

void free_list(void);

inline void clear_history()
{
 printf("Usuwam plik historii\n");
 unlink("HISTORY");
 free_list();
 head=current=new_cmds=NULL;
}


void execute(char* cmd)
{
 int lbw;
 char** words = parser(cmd, lbw);
 char* exec=NULL;
 int co=0, ro=0,err=0;
 char*** r=NULL;
 if (lbw<=0 || !words) return;
 //printf("lbw=%d\n", lbw);
 //for (int i=0;i<lbw;i++) printf("%s\n", words[i]);
 printf("Wykonam polecenie: %s\n", cmd);
 if (!strcmp(words[0],"pomoc")) help();
 else if (!strcmp(words[0],"help")) help();
 else if (!strcmp(words[0],"p")) help();
 else if (!strcmp(words[0],"h")) help();
 else if (!strcmp(words[0],"?")) help();
 else if (lbw==2 && !strcmp(words[0],"wyczysc") && !strcmp(words[1],"historie")) clear_history();
 else if (!strcmp(words[0],"historia")) clear_history();
 else if (!strcmp(words[0],"clh")) clear_history();
 else if (!strcmp(words[0],"SQL")) system("mysql KLASA");
 else if (!strcmp(words[0],"XSQL")) system("xmysql KLASA");
 else if (!strcmp(words[0],"XSQLA")) system("xmysqladmin");
 else if (!strcmp(words[0],"k") || !strcmp(words[0],"koniec") || !strcmp(words[0],"quit") || !strcmp(words[0],"q")) return;
 else if (!strcmp(words[0],"dodaj"))
   {
    exec=prepare_insert_query(words, 1, lbw);
    if (!exec) { printf("Blad przy przygotowywaniu kolejki SQL\n"); return; }
    printf("Zapytanie SQL: %s\n", exec);
    r=Q(exec,co,ro,err);
    Perr(err);
    if (err) return;
    Free3(r,co,ro);
    printf("Kolejka SQL zostala wykonana poprawnie.\n");
   }
 else if (!strcmp(words[0],"sys"))
   {
    exec=prepare_cmdline(words, 1, lbw);
    if (!exec) return;
    printf("Wywolanie Systemu UNIX: %s\n", exec);
    system(exec);
    printf("Powrot z systemu UNIX do SQl-Klasa\n");
   }
 else if (!strcmp(words[0],"sql"))
   {
    exec=prepare_SQL_stmt(words, 1, lbw);
    downcase(exec);
    if (!exec) return;
    printf("Zapytanie SQL: %s\n", exec);
    r=Q(exec,co,ro,err);
    Perr(err);
    if (err) return;
    if (!r) return;
    for (int i=0;i<ro;i++)
      {
       for (int j=0;j<co;j++) printf("%-18s\t", r[i][j]);
       putchar('\n');
      }
    Free3(r,co,ro);
    printf("Kolejka SQL zostala wykonana poprawnie.\n");
   }
 else printf("Nie znam tego polecenia, aby zobaczyc spis polecen wpisz pomoc\nAby zakonczyc program wpisz koniec\n");
 for (int i=0;i<lbw;i++) if (words[i]) { delete [] words[i]; words[i] = NULL; }
 if (words) delete [] words;
 if (exec) delete [] exec;
 words=NULL;
}


int readline(FILE* f, char*& buf)
{
 int zn;
 int pos = ftell(f);
 int i=0;
 while ((zn=fgetc(f))!=EOF) { if (zn=='\n') break; i++; }
 //printf("pos=%d,i=%d\n",pos,i);
 buf = new char[i+2];
 fseek(f,pos,SEEK_SET);
 for (int x=0;x<i;x++) buf[x] = fgetc(f);
 buf[i]=0;
 fgetc(f);
 //printf("buf=%s,pos=%d,i=%d\n",buf,pos,i);
 return i;
}


void load_history()
{
 FILE* his=fopen("HISTORY","r");
 if (!his) { printf("Nie moge otworzyc pliku historii polecen, kontynuuje bez...\n"); return; }
 char* cmd;
 while (readline(his, cmd))
   {
    current=add_to_last(current, cmd);
    if (!head) head=current;
   }
 new_cmds=current;
}


void save_history()
{
 FILE* his;
 his=fopen("HISTORY", "a");
 if (!his) { printf("Nie moge zapisac historii polecen do pliku!\n"); return; }
 pList tmp=new_cmds->next;
 while (tmp)
   {
    fprintf(his,"%s\n", tmp->data);
    tmp=tmp->next;
   }
 fclose(his);
}


void callback(char* cmd)
{
 current=add_to_last(current, cmd);
 if (!head) head=current;
 if (!new_cmds) new_cmds=head;
}


void free_list()
{
 while (head->next)
   {
    head = head->next;
    if (head->prev->data) delete [] head->prev->data;
    delete head->prev;
   }
}


int main(int lb, char** par)
{
 printf("Kompilacja: plik: [%s]; linia: [%d]; data: [%s %s]\n", __FILE__, __LINE__, __DATE__, __TIME__);
 printf("Program do zarzadzania klasa w szkole\nUzywa MySQl, mysqlclient.so, oraz C++\nNapisany przez Morgoth DBMA \n\ttel: 693-582-014, mail: morgothdbma@o2.pl\nWpisz: pomoc - aby zobaczyc liste polecen\n");
 int ret = connect();
 if (ret)
  {
   printf("\n\tPANIC:\nNie moge polaczyc sie z MySQL: parametry uzyte: \nlocalhost,morgoth,KLASA,<ENCRYPTED_PSWD>,/tmp/mysql.sock\n");
   return ret;
  }
 load_history();
 char* cmd = new char[2048];
 strcpy(cmd,"");
 while (strcmp(cmd,"koniec") && strcmp(cmd,"quit") && strcmp(cmd,"k") && strcmp(cmd,"q"))
  {
   getinput(cmd);
   execute(cmd);
   callback(cmd);
  }
 delete [] cmd;
 save_history();
 free_list();
 mysql_close(connection);
 printf("Do zobaczenia nastepnym razem!\n");
 return 0;
}

