/*
  RPG.C plik zrodlowy programu rpg wymaga biblioteki Mrandom.h w tym samym katalogu
  oraz standardowych bibliotek Unix'a Aby program skomplilowac pod Windows'em
  zaznacz #include "Mrandom.h" jako komentarz // oraz niektore komplilatory
  maja juz zdefiniowane FALSE i TRUE wiec te linie takzezaznacz jako komentarz
  ostrzegam ze program dziala dziwnie pod Windowsem wiekszosc komunikatow nie
  miesci sie w linni oraz na ekranie Windows NT target Win32 bit Console Application
  pozwala przewijac ale wyglad dalej jest dziwny poza tym program wymaga do uruchomienia
  parametrow wiec trzeba go wlaczac z konsoli lub MS-DOS'a Jego uzycie w Unix / Linux
  jest o wiele prostsze np rpg a b gdzie a oznacza baze danych (szukaj DataBase/a)
  a b oznacza utworzony w trakcie dzialania plik postaci (Characters/b) analogicznie
  pojawi sie plik Characters/b.html (no comment) mozna tez wpisac rpg a wtedy program
  nie tworzy postaci a pozwala tylko na edycje bazy danych. zalecam tworzenie wlasnych
  baz danych do swoich systemow.. program wymaga okolo 400k RAM'u oraz okolo 200k na
  dysku. Kompilacja pod Unix'em / Linux'em uruchom konsole przejdz do folderu z programem
  wpisz c++ -o rpg rpg.c poczekaj okolo 5 sek... i gotowe. By uzywac zalecam powiekszyc
  konsole kiliknij drugim i Fullscreen nastepnie ustaw czcionke na normal lub (lepiej) small
  Milego uzywania. Morgoth DBMA .Najnowsza wersja 3.0 potrafi konwertowac bazy danych
  z Windowsowej wersji Applicat.cpp (32bit RPG) wpisz rpg name convert, gdzie name oznacza
  baze danych innej aplikacji np. rpg triada.bm convert lub rpg galthar.bm convert
  zamiast convert mozesz napisac -c. program przekompilowujebaze do nowego typu ale nie potrafi
  zapisywac starej wersji !. Na odpowiednim typie Unix'a (Linux'a) ta aplikacja jest 64 bit.
*/
#include <stdio.h>
#include <stdlib.h>      //include segment
#include <fstream.h>
#include <math.h>
#include "oswindows"    //system na ktorym kompilujesz ma byc bez komentarza!
//#include "oslinux"
#if OSTYPE == LINUX
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include "Mrandom.h"  //my own random library
#include <termios.h>
#endif
#if OSTYPE == WINDOWS
#include <string.h>
#include <conio.h>
#endif
#define OK 0;   //precompliced constances segment
#define ATTRIB        10   //10      //6
#define S_ATTRIB      10  //10     //5
#define MAX_ATT       25  //100         //constances segment  //25
#define STRING        80   //75      //50
#define COMMON        2      //2    //2
#define MAX_TYPE      250  //1000   //100
#define TRUE          1
#define FALSE         0
#define MAX_SKI       2500      //2000
#define MULTIPLE      0
#define ATTRIBUTE     1
#define MOD           2
#define DIFF          3
#define CATEG         4
#define MAGIC         5
#define MAX_CATEG     20   //40  //20
#define MAX_LEVELS    75   //100  //30
#define DANE_POSTACI  12   //8
#define IMIE          0
#define NAZWISKO      1
#define RASA          2            //obszar danych predefiniowanych
#define PROFESJA      3
#define CHARAKTER     4
#define OCZY          5
#define WLOSY         6
#define CERA          7
#define WZROST        8
#define WAGA          9
#define PLEC          10
#define MAX_UM        255
#define ALL           2
#define MAX_EKW       50
#define DIR_EXEC      0777

//typedef unsigned int UINT;  //type segment
static int HIDE = 0;
static FILE* LOG = 0;
#if OSTYPE == LINUX
typedef struct DATA;
typedef struct RPG;

inline void ln();
void lns(int);
void DecompressData(char*);         //forwards
void EndWin();
void last_op(DATA*,RPG*);
void MemShot(DATA*,RPG*);
void MemShot(DATA*);
void MemShot(RPG*);
void takeint(int*);
void takechar(char*);
void takestring(char*,int);
void CompressData(char*);
void OpenLog();
void Trap();
void EDIT(char*);
void Protection(int,DATA*,FILE*);
void Load(DATA*,FILE*);
void Uncompress(DATA*);
void Save(DATA*,FILE*);
void ShowAttrib(DATA*);
void QuickSave(DATA*);
void SetAttrib(DATA*);
void ShowTrial(DATA*,short);
void AddTrial(DATA*,short,short);
void DelTrial(DATA*,short,short);
void SetTrial(DATA*,short);
void ShowCateg(DATA*);
void ShowSkill(DATA*,short);
void DelSkill(DATA*,short);
void AddSkill(DATA*,short,short);
void SortSkill(DATA*);
void FindSkill(DATA*,int);
void SortConv(DATA*,int);
void SetSkill(DATA*);
void SetCateg(DATA*);
void ShowLevels(DATA*);
void ShowLevels2(DATA*);
void SetLevels(DATA*);
void SetLevels2(DATA*);
void CheckPassword(DATA*);
void ShowWoman(DATA*);
void SetGender(DATA*);
void Operations(DATA*);
void BaseToHTML(DATA*,FILE*,char*);
void BaseHTML(DATA*,char*);
void DataBase(char*,DATA*);
void RealMM(RPG*);
void Protection(int,RPG*);
void Draw();
void ShowAll(RPG*);
void LoadChar(RPG*,FILE*);
void Uncompress(RPG*);
void SaveChar(RPG*,FILE*,char*);
void lnf(FILE*);
void lnfs(int,FILE*);
void Alternative(RPG*,FILE*);
void WriteHTML(RPG*,FILE*,char*);
void AboutHTML(RPG*,FILE*,char*,char*);
void Call(RPG*);
void CheckPassChar(RPG*);
void MiniShow(RPG*,int);
void ShowSkill(RPG*);
void AddAtt(RPG*,int*);
void AddToSkill(RPG*,DATA*,int*);
void NewSkill(RPG*,DATA*,int*,int);
void ShowAttChar(RPG*);
void ShowEkw(RPG*);
void DelEkw(RPG*);
void AddEkw(RPG*);
void EditEkw(RPG*);
void Jump(int*);
void DealPD(RPG*,DATA*,int*);
void AddPD(RPG*,DATA*);
void AboutSave(RPG*,FILE*,char*,char*);
void Experience(DATA*,RPG*,FILE*,char*,char*);
void Woman(RPG*,DATA*);
void MainData(RPG*,DATA*);
void ChooseRace(RPG*,DATA*);
void ChooseProf(RPG*,DATA*);
void DefaultValues(RPG*,DATA*);
void RandomAtt(RPG*,DATA*);
void OwnAtt(RPG*);
void AddSkill(RPG*,DATA*,int*,short);
void DelSkill(RPG*,DATA*,int*,short);
void SeeLevCost(DATA*);
void SkillOperator(RPG*,DATA*,int*,FILE*,char*);
void SetSkill(RPG*,DATA*,FILE*,char*);
void SetEkw(RPG*);
void CreateChar(DATA*,RPG*,FILE*,char*,char*);
void RolePlayConstructor(DATA*,RPG*,char*,char*);
void AddLine(char*,char*,int);
void LoadAndConvertC(RPG*,FILE*);
void LoadAndConvert(RPG*,FILE*);
void ConversionC(char*);
void Conversion(char*);
void SayHello();
void Process(char*,char*);
void Info();
int  Register();
void HELP();
void TRIADA(char*);
void DataProtector();
void Translate(char);
void ConsoleInfo();
void KonsoleType();
void CheckFolders();
void CreateFile(FILE*,char*,char*,bool);
void BigEdit(RPG*);
void ExecuteCommand(char*, RPG*);
void AboutLog();
#endif
struct DATA
{
 DATA();              //constructor
 ~DATA();             //destructor
 int ilosc[ATTRIB];                     //object segment
 int mod[COMMON][MAX_TYPE][MAX_ATT];
 char name[COMMON][MAX_TYPE][STRING];  //look at this array !
 int value_att[MAX_ATT];
 char name_att[MAX_ATT][STRING];
 char s_name[MAX_SKI][STRING];
 int s_mod[MAX_SKI][S_ATTRIB];
 char c_name[MAX_CATEG][STRING];
 int cost[MAX_LEVELS];
 int pd_cost[MAX_LEVELS];
 int sv;
 int st;
 int p_mod[MAX_ATT];
 int lr;
 int mm;
};



DATA :: DATA()
{
 fprintf(LOG,"Running DATA :: DATA(void); setting up offset 0x%x getting memory %d bytes\n",this,sizeof(*this));
 ilosc[0] = 13;       //attributes
 ilosc[1] = 10;       //races
 ilosc[2] = 20;       //proffessies
 ilosc[3] = 150;      //skills
 ilosc[4] = 6;        //categories
 ilosc[5] = 15;       //costs
 ilosc[6] = 15;       //PD costs
 sv = 0;
 st = 1;
 lr = 0;
 mm = 0;
 for (int i=0;i<MAX_ATT;i++)
        {
         value_att[i] = 10;                        //method & constructor segment
         p_mod[i]     = 0;
         strcpy(name_att[i],"DEFAULT_ATTR");
        }
 for (int i=0;i<MAX_TYPE;i++)
        {
         strcpy(name[0][i],"DEFAULT_RACE");
         strcpy(name[1][i],"DEFAULT_PROF");
         for (int j=0;j<MAX_ATT;j++)
                {
                 mod[0][i][j] = 0;
                 mod[1][i][j] = 0;
                }
        }
  fprintf(LOG,"In progress creating tables...\n");
 for (int i=0;i<MAX_SKI;i++)
        {
         strcpy(s_name[i],"DEFAULT_SKILL");
         s_mod[i][MULTIPLE] = 3;
         s_mod[i][ATTRIBUTE] = 0;
         s_mod[i][MOD] = 0;
         s_mod[i][DIFF] = 1;
         s_mod[i][CATEG] = 0;
         s_mod[i][MAGIC] = 0;
        }
 for (int i=0;i<MAX_CATEG;i++)
        {
         strcpy(c_name[i],"DEFAULT_CATEG");
        }
 for (int i=0;i<MAX_LEVELS;i++) {cost[i] = 2*i+1;pd_cost[i] = cost[i];}
 fprintf(LOG,"LD: Initialized all class fields\n");
 fprintf(LOG,"LD: Returned 0 errors\n");
}
void DecompressData(char* data)
{
 fprintf(LOG,".");
 for (int i=0;i<STRING;i++) if (data[i] == '_') data[i] = ' '; //function segment
}
void MemShot(DATA* db)
{
 char temp[STRING];
 strcpy(temp,"DataBase/memshot");
 int i = 0;
 FILE* plik;
 char name[STRING];
 char lbs[STRING];
 int opened = 0;
 while  (!opened)
        {
          sprintf(lbs,"%d",i);
          strcpy(name,temp);
          strcat(name,lbs);
          if  ((plik = fopen(name,"r+")) != NULL) {cout<<name<<" already exists...\n";opened = 0;fclose(plik);}
          else {cout<<"I will write to "<<name<<endl;opened = 1;}
          i++;
        }
  if ((plik = fopen(name,"w+")) == NULL)
        {
         cout<<"Cannot write to "<<name<<endl;
         return;
        }
 cout<<"Save to "<<name<<endl;
 //will save
 fprintf(plik,"%d\n",db->sv);
 fprintf(plik,"%d\n",db->st);
 fprintf(plik,"%d\n",db->lr);
 fprintf(plik,"%d\n",db->mm);
 for (int i=0;i<ATTRIB;i++)
        {
         fprintf(plik,"%d\n",db->ilosc[i]);
        }
 for (int i=0;i<COMMON;i++)
        {
         for (int j=0;j<MAX_TYPE;j++)
                {
                 fprintf(plik,"%s\n",db->name[i][j]);
                 for (int k=0;k<MAX_ATT;k++)
                        {
                         fprintf(plik,"%d\n",db->mod[i][j][k]);
                        }
                }
        }
  for (int i=0;i<MAX_ATT;i++)
        {
         fprintf(plik,"%d\n",db->value_att[i]);
         fprintf(plik,"%s\n",db->name_att[i]);
         fprintf(plik,"%d\n",db->p_mod[i]);
        }
  for (int i=0;i<MAX_SKI;i++)
        {
         fprintf(plik,"%s\n",db->s_name[i]);
         for (int j=0;j<S_ATTRIB;j++)
                {
                 fprintf(plik,"%d\n",db->s_mod[i][j]);
                }
        }
  for (int i=0;i<MAX_CATEG;i++)
        {
         fprintf(plik,"%s\n",db->c_name[i]);
        }
  for (int i=0;i<MAX_LEVELS;i++)
        {
         fprintf(plik,"%d\n",db->pd_cost[i]);
         fprintf(plik,"%d\n",db->cost[i]);
        }
 //already saved
 cout<<"Done Memshot.\n";
}
void OpenLog()
{
 cout<<"Opening Log file\n";
 if ((LOG = fopen("DataBase/spy_log","a+")) == NULL)
        {
         cout<<"Unknown Internal Error\n";
         if ((LOG = fopen("/dev/null","w+")) == NULL)
                {
                 #if OSTYPE == LINUX
                 last_op(NULL,NULL);
                 #endif
                 exit( -1 );
                }
         cout<<"Fixed?\n";
        }
 fprintf(LOG,"\n\nOpened new Log\nSpying all processes...\n");
 fprintf(LOG,"LD: return from OpenLog(void);\n");
}
void Translate(char zn)
{
 fprintf(LOG,"Running Translate(char), with parameter %c\n",zn);
 zn = char(zn+100);
 cout<<zn;
 fprintf(LOG,"LD: return %c, on local stact orginal parameter unchanged\n",zn);
}
DATA :: ~DATA(){fprintf(LOG,"Running DATA :: ~DATA(), deleted offset 0x%x, freed %d bytes\n",this,sizeof(*this));}                                     //destructor segment
void takechar(char* zn)
{
 fprintf(LOG,"Runnig Konsole :: takechar(char*) memory offset 0x%x\n",zn);
 if (HIDE != 3)
 {
 #if OSTYPE == LINUX
 char znak[STRING];
 scanf("%s",znak);
 sscanf(znak,"%c",zn);
 if (strlen(znak)>1) {cout<<"You've entered too long name\n";fprintf(LOG,"LD: Error detected, user typed too long name, fixed\n");}
 #elif OSTYPE == WINDOWS
 *zn = char(getch());
 if (HIDE == 0) {cout<<*zn;}
 if (HIDE == 1) {cout<<"*";}
 if (HIDE == -1) Translate(*zn);
 cout<<endl;
 #endif
 }
 else cin>>*zn;
 fprintf(LOG,"Taken char %c into memory offset 0x%x\n",*zn,zn);
}
void Uncompress(DATA* db)
{
 cout<<"Uncompressing data...\n";
 fprintf(LOG,"Running Uncompress(DATA*)\n\tAll fields of 0x%x will be decompressed\n",db);
 for (int i=0;i<db->ilosc[0];i++)
        {
         DecompressData(db->name_att[i]);
        }
 for (int i=0;i<db->ilosc[1];i++)
        {
         DecompressData(db->name[0][i]);
        }
 for (int i=0;i<db->ilosc[2];i++)
        {
         DecompressData(db->name[1][i]);
        }
 for (int i=0;i<db->ilosc[3];i++)
        {
         DecompressData(db->s_name[i]);
        }
 for (int i=0;i<db->ilosc[4];i++)
        {
         DecompressData(db->c_name[i]);
        }
 fprintf(LOG,"Already decompressed\n");
}
void takestring(char* par)
{
fprintf(LOG,"Running Konsole :: takestring(char*), memory offset 0x%x\n",par);
 #if OSTYPE == LINUX
 fprintf(LOG,"OS type Linux\n");
 fprintf(LOG,"Console detected %d\n",HIDE);
 if (HIDE == 3)
        {
         scanf("%s",par);
         return;
        }
 struct termios st,end;
 tcgetattr(fileno(stdin),&st);
 end = st;
 end.c_lflag &= ~ICANON;
 end.c_lflag &= ~ECHO;
 end.c_cc[VMIN] = 1;
 end.c_cc[VTIME] = 0;
 if (tcsetattr(fileno(stdin),TCSAFLUSH,&end) != 0)
        {
         cout<<"Error init linux console type\n";
         cout<<"Suggest change mode to 3\n";
         fprintf(LOG,"LD: Error unable to initialize console %d, it's medium error\n.Trying to fix...\n",HIDE);
         KonsoleType();
         fprintf(LOG,"LD: Possibly Error fixed\n");
        }
 fprintf(LOG,"LD: Termios correctly initialized\n");
 int i;
 st:
 i = 0;
 par[0] = 0;
 while (i<STRING-1)
        {
         par[i] = getchar();
         if ((i ==0) && (par[i] == 10)) {fprintf(LOG,"Memory Buffer Flushed, (null) string\n");goto cont;}
         if (par[i] == ' ') par[i] = '_';
         if (par[i] == 10) {par[i] = '\0';goto lab;}
         if (par[i] == 127) {printf("\b \b");fprintf(LOG,"User used DELETE key sign number %d to %d\n",i,i-1);i-=1;if (i<0) {i=0;fprintf(LOG,"Cannot stat array[-1] member, change index to 0\n");}goto cont;}
         if (par[i] == 27) {par[i] = '\0';cout<<" Canceled <"<<par<<">\n";fprintf(LOG,"LD: User cancelled String: %s\n",par);goto st;}
         if (HIDE == 0) {cout<<par[i];}
         if (HIDE == 1) {cout<<"*";}
         if (HIDE == -1) Translate(par[i]);
         if (HIDE == 2) cout<<char(7);
         i++;
         cont:;
        }
 lab:
 if (i == 0) {strcpy(par,"-");cout<<"-";}
 ln();
 tcsetattr(fileno(stdin),TCSAFLUSH,&st);
 fprintf(LOG,"LD: Return to previous console\n");
 #elif OSTYPE == WINDOWS
 fprintf(LOG,"Windows detected\n");
 int i;
 st:
 i = 0;
 par[0] = 0;
 while (i<STRING-1)
        {
         par[i] = (char)getch();
         if (par[i] == ' ') par[i] = '_';
         if (par[i] == 13) {par[i] = '\0';goto lab;}
         if (par[i] == 27) {par[i] = '\0';cout<<" Canceled <"<<par<<">\n";fprintf(LOG,"User cancelled String: %s\n",par);goto st;}
         if (par[i] == 8) {printf("\b \b");fprintf(LOG,"User used DELETE key sign number %d to %d\n",i,i-1);i-=1;if (i<0) {i=0;fprintf(LOG,"Cannot stat array[-1] member, change index to 0\n");}goto cont;}
         if (HIDE == 0) {cout<<par[i];}
         if (HIDE == 1) {cout<<"*";}
         if (HIDE == -1) Translate(par[i]);
         i++;
         cont:
        }
 lab:
 if (i == 0) {strcpy(par,"-");cout<<"-";}
 cout<<endl;
 #endif
 fprintf(LOG,"LD: Taken string %s to memory offset 0x%x, bytes %d\n",par,par,strlen(par));
}
inline void ln() {fprintf(LOG,"->");cout<<endl;}
void lns(int par)
{
 fprintf(LOG,"->%d\n",par);
 for (short i=0;i<par;i++) ln();
 fprintf(LOG,"\n");
}
void EndWin()
{
 fprintf(LOG,"Running EndWin(void)\n");
 #if OSTYPE == WINDOWS
 fprintf(LOG,"Detected Windows, wait for any key. . .\n");
 cout<<"Press any key to end Windows NT version. . . ";
 getch();
 cout<<endl;
 #endif
 fprintf(LOG,"LD: Returned void\n");
}
void takeint(int* par)
{
 fprintf(LOG,"Running takeint(int*) memory offset 0x%x\n",par);
 if (HIDE != 3)
 {
 char string[STRING];
 scanf("%s",string);
 if (!sscanf(string,"%d",par)) {*par = 0;cout<<"You've typed character, change to digit 0\n";fprintf(LOG,"LD: Error detected, user typed character instead digit, change to 0, fixed\n");}
 }
 else cin>>*par;
 fprintf(LOG,"LD: Taken %d into memory offset 0x%x\n",*par,par);
}
void CompressData(char* data)
{
 fprintf(LOG,".");
 for (int i=0;i<STRING;i++) if (data[i] == ' ') data[i] = '_';
}
void Trap()
{
 fprintf(LOG,"System halted, checking access violation\n");
 cout<<"Halted!\n";
 int a;
 takeint(&a);
}
void Protection(int check, DATA* db, FILE* plik)
{
 fprintf(LOG,"Running Protection(int,DATA*,FILE*)\n");
 fprintf(LOG,"Parameters\n\tDATA offset 0x%x\n\tFILE offset 0x%x\n\tIs Correct %d\n",db,plik,check);
 if (!check)
        {
         fprintf(LOG,"LD: Fatal Error, base not cleanly loaded, suggest terminate\n");
         lns(6);
         cout<<"Base doesn't look to be corect written...\n";
         cout<<"Suggest terminate application\n";
         cout<<"To convert base type rpg name -c, name means\n";
         cout<<"Base name, you can continue but it may be cause for\n";
         cout<<"Bad write base output, continue only if you have backup copy\n";
         cout<<"If conversion didn't help, it's mean that base is bad written\n";
         cout<<"Or file is corrupted or no memory for load base to RAM\n";
         lns(4);
         char zn;
         cout<<"Proceed? ";
         takechar(&zn);
         if (zn != 'y') {delete db;fprintf(LOG,"Application halted!\n");fclose(plik); fclose(LOG);
         #if OSTYPE == LINUX
         last_op(db,NULL);
         #endif
         exit( -1 );}
        }
}
void Save(DATA* db, FILE* plik)
{
 fprintf(LOG,"Running Save(DATA*,FILE*)\nDATA offset 0x%x\n\tFILE offset 0x%x\n",db,plik);
 cout<<"Saving record.\n";
 fprintf(plik,"%d\n",db->ilosc[0]);
 for (int i=0;i<db->ilosc[0];i++)
        {
         CompressData(db->name_att[i]);
         fprintf(plik,"%d %s\n",db->value_att[i],db->name_att[i]);
        }
 fprintf(plik,"%d\n",db->ilosc[1]);
 fprintf(plik,"%d\n",db->ilosc[2]);
 for (int i=0;i<db->ilosc[1];i++)
        {
         CompressData(db->name[0][i]);
         fprintf(plik,"%s ",db->name[0][i]);
         for (int j=0;j<db->ilosc[0];j++)
                {
                 fprintf(plik,"%d ",db->mod[0][i][j]);
                }
         fprintf(plik,"\n");
        }
 for (int i=0;i<db->ilosc[2];i++)
        {
         CompressData(db->name[1][i]);
         fprintf(plik,"%s ",db->name[1][i]);
         for (int j=0;j<db->ilosc[0];j++)
                {
                 fprintf(plik,"%d ",db->mod[1][i][j]);
                }
         fprintf(plik,"\n");
        }
 fprintf(plik,"%d\n",db->ilosc[3]);
 for (int i=0;i<db->ilosc[3];i++)
        {
         CompressData(db->s_name[i]);
         fprintf(plik,"%s ",db->s_name[i]);
         fprintf(plik,"%d %d %d %d %d\n",db->s_mod[i][0],db->s_mod[i][1],db->s_mod[i][2],db->s_mod[i][3],db->s_mod[i][4]);
        }
 fprintf(plik,"%d\n",db->ilosc[4]);
 for (int i=0;i<db->ilosc[4];i++)
        {
         CompressData(db->c_name[i]);
         fprintf(plik,"%s\n",db->c_name[i]);
        }
 fprintf(plik,"%d\n",db->ilosc[5]);
 for (int i=0;i<db->ilosc[5];i++)
        {
         fprintf(plik,"%d\n",db->cost[i]);
        }
 fprintf(plik,"%d\n",db->sv);
 fprintf(plik,"%d\n",db->st);
fprintf(plik,"%d\n",db->ilosc[6]);
 for (int i=0;i<db->ilosc[6];i++)
        {
         fprintf(plik,"%d\n",db->pd_cost[i]);
        }
 for (int i=0;i<db->ilosc[0];i++)
        {
         fprintf(plik,"%d\n",db->p_mod[i]);
        }
 fprintf(plik,"%d\n",db->lr);
  for (int i=0;i<db->ilosc[3];i++)
        {
         fprintf(plik,"%d\n",db->s_mod[i][MAGIC]);
        }
 fprintf(plik,"%d\n",db->mm);
 Uncompress(db);
 fprintf(LOG,"All DATA fields were succesfully written\n");
}
void Load(DATA* db,FILE* plik)
{
 fprintf(LOG,"Running Load(DATA*,.FILE*)\nParameters\n\tDATA offset 0x%x\n\tFILE offset 0x%x\n",db,plik);
 cout<<"Loading record.\n";
 int check;
 check = fscanf(plik,"%d\n",&db->ilosc[0]);
 Protection(check,db,plik);
 for (int i=0;i<db->ilosc[0];i++)
        {
         fscanf(plik,"%d %s\n",&db->value_att[i],db->name_att[i]);
         DecompressData(db->name_att[i]);
        }                  //with it
 fscanf(plik,"%d\n",&db->ilosc[1]);
 fscanf(plik,"%d\n",&db->ilosc[2]);
 for (int i=0;i<db->ilosc[1];i++)
        {
         fscanf(plik,"%s ",db->name[0][i]);
         DecompressData(db->name[0][i]);
         for (int j=0;j<db->ilosc[0];j++)
                {
                 fscanf(plik,"%d ",&db->mod[0][i][j]);
                }
         fscanf(plik,"\n");
        }
 for (int i=0;i<db->ilosc[2];i++)
        {
         fscanf(plik,"%s ",db->name[1][i]);
         DecompressData(db->name[1][i]);
         for (int j=0;j<db->ilosc[0];j++)
                {
                 fscanf(plik,"%d ",&db->mod[1][i][j]);
                }
         fscanf(plik,"\n");
        }
 fscanf(plik,"%d\n",&db->ilosc[3]);
 for (int i=0;i<db->ilosc[3];i++)
        {
         fscanf(plik,"%s ",db->s_name[i]);
         DecompressData(db->s_name[i]);
         fscanf(plik,"%d %d %d %d %d\n",&db->s_mod[i][0],&db->s_mod[i][1],&db->s_mod[i][2],&db->s_mod[i][3],&db->s_mod[i][4]);
        }
 fscanf(plik,"%d\n",&db->ilosc[4]);
 for (int i=0;i<db->ilosc[4];i++)
        {
         fscanf(plik,"%s\n",db->c_name[i]);
         DecompressData(db->c_name[i]);
        }
 fscanf(plik,"%d\n",&db->ilosc[5]);
 for (int i=0;i<db->ilosc[5];i++)
        {
         fscanf(plik,"%d\n",&db->cost[i]);
        }
 fscanf(plik,"%d\n",&db->sv);
 fscanf(plik,"%d\n",&db->st);
 fscanf(plik,"%d\n",&db->ilosc[6]);
 for (int i=0;i<db->ilosc[6];i++)
        {
         fscanf(plik,"%d\n",&db->pd_cost[i]);
        }
 for (int i=0;i<db->ilosc[0];i++)
        {
         fscanf(plik,"%d\n",&db->p_mod[i]);
        }
 fscanf(plik,"%d\n",&db->lr);
 for (int i=0;i<db->ilosc[3];i++)
        {
         fscanf(plik,"%d\n",&db->s_mod[i][MAGIC]);
        }
 fscanf(plik,"%d\n",&db->mm);
 fprintf(LOG,"LD: Read all DATA fields, return void\n");
}

#if OSTYPE == LINUX
void MemShot(DATA* db, RPG* post)
{
  if (db == NULL)
        {
         cout<<"Pointer to DATA is NULL, cannot MemShot it\n";
        }
  else MemShot(db);
  if (post == NULL)
        {
         cout<<"Pointer to RPG is NULL, cannot MemShot it\n";
        }
  else MemShot(post);
}
#endif
void ShowAttrib(DATA* db)
{
 fprintf(LOG,"Running ShowAttrib(DATA*) parameter offset 0x%x\n",db);
 lns(2);
 cout<<"Plus value: "<<db->st<<endl;
 cout<<"Magic power attribute is "<<db->name_att[db->mm]<<".\n";
 cout<<"\t\tNumber of attributes: "<<db->ilosc[0]<<endl;
 for (int i=0;i<db->ilosc[0];i++)
        {
         #if OSTYPE == LINUX
         printf("Atrib: (%-3d)%-30s Start Value: %-3d\n",i,db->name_att[i],db->value_att[i]);
         #elif OSTYPE == WINDOWS
         printf("Atrib: (%-2d)%-25s Start Value: %-3d\n",i,db->name_att[i],db->value_att[i]);
         #endif
        }
 lns(2);
}
void QuickSave(DATA* db)
{
 fprintf(LOG,"Running QuickSave(DATA*), memory offset 0x%x\n",db);
 FILE* plik;
 char fn[STRING+20];
 strcpy(fn,"DataBase/");
 char temp[STRING];
 cout<<"Type base name (without directory): ";
 takestring(temp);
 strcat(fn,temp);
 cout<<"Saving in "<<fn<<endl;
 if ((plik = fopen(fn,"w+")) == NULL)
        {
         cout<<"Fatal Reflection, unable to write to "<<fn<<endl;
         fprintf(LOG,"LD: Hard error, cannot write to %s, not fixed but continue possible\n",fn);
         return;
        }
 Save(db,plik);
 fclose(plik);
}
void SetAttrib(DATA* db)
{
 fprintf(LOG,"Running SetAttrib(DATA*), offset 0x%x\n",db);
 cout<<"Max attrib: "<<MAX_ATT<<endl;
 cout<<"Found "<<db->ilosc[0]<<" defined attributes.\n";
 ShowAttrib(db);
 cout<<"Change attrib record? ";
 char zn;takechar(&zn);
 if (zn != 'y') {cout<<"No changes made...\n";return;}
 ShowAttrib(db);
 cout<<"Type number of attrib responsible for Magic Power: ";
 int mmm;
 takeint(&mmm);
 if (mmm<0) mmm = 0;
 if (mmm>db->ilosc[0]-1) mmm = db->ilosc[0] -1;
 db->mm = mmm;
 cout<<"Plus value: ";
 takeint(&db->st);
 cout<<"Continue changing? ";
 takechar(&zn);
 if (zn != 'y') return;
 cout<<"Type number of attributes: ";
 int temp;takeint(&temp);
 if (temp>MAX_ATT) temp = MAX_ATT;
 if (temp<1)       temp = 1;
 cout<<"Taken "<<temp<<" as number of attributes.\n";
 db->ilosc[0] = temp;
 for (int i=0;i<db->ilosc[0];i++)
        {
         cout<<"Type name of attrib number "<<i<<": ";
         takestring(db->name_att[i]);
         cout<<"Start value of "<<db->name_att[i]<<": ";
         takeint(&temp);
         if (temp < 0) temp *= -1;
         db->value_att[i] = temp;
        }
  fprintf(LOG,"LG: DATA fields modified\n");
}
void ShowTrial(DATA* db,short trial)
{
 fprintf(LOG,"Running ShowTrial(DATA*,short) DATA offset is 0x%x\n\ttrial = %d\n",db,trial);
 lns(2);
 fprintf(LOG,"Choosing version for detected OS..\n");
 cout<<"\t\tNumber: "<<db->ilosc[trial+1]<<endl;
 for (int i=0;i<db->ilosc[trial+1];i++)
        {
         #if OSTYPE == LINUX
         printf("(%-3d)%-25s ",i,db->name[trial][i]);
         for (int j=0;j<db->ilosc[0];j++)
                {
                 printf("%-3d ",db->mod[trial][i][j]);
                }
         #elif OSTYPE == WINDOWS
         printf("(%-2d)%-20s ",i,db->name[trial][i]);
         for (int j=0;j<db->ilosc[0];j++)
                {
                 printf("%-2d ",db->mod[trial][i][j]);
                }
         #endif
         ln();
        }
 lns(2);
}
void AddTrial(DATA* db, short trial,short add = TRUE)
{
 fprintf(LOG,"Running AddTrial(DATA* db, short, short = 0)\n");
 fprintf(LOG,"DATA offset is 0x%x\n\ttrial = %d\n\tadd = %d\n",db,trial,add);
 if ((db->ilosc[trial+1] == MAX_TYPE) && (add != TRUE)) {cout<<"Record full\n";fprintf(LOG,"LD: error, record full\n");return;}
 int temp = 0;
 if (add == TRUE) {temp = db->ilosc[trial+1];}
 else
        {
         ShowTrial(db,trial);
         printf("Type number (0 - %d): ",db->ilosc[trial+1]-1);
         takeint(&temp);
         if (temp<0) return;
         if (temp>db->ilosc[trial+1]-1) return;
        }
 cout<<"Type name: ";
 takestring(db->name[trial][temp]);
 for (int i=0;i<db->ilosc[0];i++)
        {
         cout<<"How "<<db->name[trial][temp]<<" modifies "<<db->name_att[i]<<": ";
         takeint(&db->mod[trial][temp][i]);
        }
 if (add == TRUE) db->ilosc[trial+1]++;
}
void DelTrial(DATA* db, short trial,short last = TRUE)
{
 fprintf(LOG,"Running DelTrial(DATA* db, short, short = 0)\n");
 fprintf(LOG,"DATA offset is 0x%x\n\ttrial = %d\n\tlast = %d",db,trial,last);
 if (db->ilosc[trial+1] == 0) {cout<<"Empty record. All data already killed!\n";fprintf(LOG,"LD: warning, record empty\n");return;}
 if (last == TRUE) db->ilosc[trial+1]--;
 else
        {
         int temp;
         ln();
         ShowTrial(db,trial);
         printf("Choose nuber (0 - %d): ",db->ilosc[trial+1]-1);
         takeint(&temp);
         if (temp<0) return;
         if (temp>db->ilosc[trial+1]-1) return;
         db->ilosc[trial+1]--;
         for (int i=temp;i<db->ilosc[trial+1];i++)
                {
                 strcpy(db->name[trial][i],db->name[trial][i+1]);
                 for (int j=0;j<db->ilosc[0];j++)
                        {
                         db->mod[trial][i][j] = db->mod[trial][i+1][j];
                        }
                }
         ln();
        }
}
void SetTrial(DATA* db, short trial)
{
 fprintf(LOG,"Running SetTrial(DATA*,short)\n\tDATA offset 0x%x\n\ttrial = %d\n",db,trial);
 if (trial) {cout<<"Maximum professies: "<<MAX_TYPE<<endl;}
 else {cout<<"Maximum races "<<MAX_TYPE<<endl;}
 cout<<"Found ";
 if (trial == 0) {cout<<db->ilosc[1]<<" races\n";}
 if (trial == 1) {cout<<db->ilosc[2]<<" professies\n";}
 ShowTrial(db,trial);
 cout<<"Change record? ";
 char zn;takechar(&zn);
 if (zn != 'y') {cout<<"Left unmodified...\n";return;}
 char z = '%';
 while (z != 'k')
   {
 lns(3);
 cout<<"\t\tNumber: "<<db->ilosc[trial+1]<<endl;
 cout<<"a\t-Add\n";
 cout<<"b\t-Delete\n";
 cout<<"c\t-See\n";
 cout<<"d\t-Modify\n";
 cout<<"e\t-Delete choosen\n";
 cout<<"f\t-Kill'em all\n";
 cout<<"g\t-Save to file\n";
 cout<<"k\t-End\n";
 cout<<"z\t-Kill program\n";
 cout<<"\n\nYou choose? ";
 takechar(&z);
 switch(z)
        {
         case 'a': AddTrial(db,trial);break;
         case 'b': DelTrial(db,trial);break;
         case 'c': ShowTrial(db,trial);break;
         case 'd': AddTrial(db,trial,FALSE);break;
         case 'e': DelTrial(db,trial,FALSE);break;
         case 'f': db->ilosc[trial+1] = 0;break;
         case 'g': QuickSave(db);break;
         case 'z': fclose(LOG);
         #if OSTYPE == LINUX
         last_op(db,NULL);
         #endif
         exit( -1 );break;
         default: {cout<<"Unknown command.\n";}
        }
   }
 fprintf(LOG,"DATA fields modified\n");
}
void ShowCateg(DATA* db)
{
 fprintf(LOG,"Running ShowCateg(DATA*) offset 0x%x\n",db);
 lns(2);
 cout<<"\t\tNumber of categories: "<<db->ilosc[4]<<endl;
 for (int i=0;i<db->ilosc[4];i++)
        {
         printf("Category: (%-3d)%-30s\n",i,db->c_name[i]);
        }
 lns(2);
}
void ShowSkill(DATA* db,short details = 1)
{
 fprintf(LOG,"Running ShowSkill(DATA*,short = 0) offset 0x%x\n\tdetails is %d\n",db,details);
 cout<<"\t\tNumber of skills: "<<db->ilosc[3]<<endl;
 ln();
 fprintf(LOG,"Choosing version for current OS...\n");
 if (details == 1)
 {
 for (int i=0;i<db->ilosc[3];i++)
        {
         #if OSTYPE == LINUX
         printf("(%-3d)%-25s Attrib: (%-3d)%-20s test modifier %-3d multiple %-3d difficult %-3d category (%-3d)%-20s magic %d\n",i,db->s_name[i],db->s_mod[i][ATTRIBUTE],db->name_att[db->s_mod[i][ATTRIBUTE]],db->s_mod[i][MOD],db->s_mod[i][MULTIPLE],db->s_mod[i][DIFF],db->s_mod[i][CATEG],db->c_name[db->s_mod[i][CATEG]],db->s_mod[i][MAGIC]);
         #elif OSTYPE == WINDOWS
         printf("%-3d.%-18s %-15s Md %-2d Ml %-2d D %-2d %-14s M %d\n",i,db->s_name[i],db->name_att[db->s_mod[i][ATTRIBUTE]],db->s_mod[i][MOD],db->s_mod[i][MULTIPLE],db->s_mod[i][DIFF],db->c_name[db->s_mod[i][CATEG]],db->s_mod[i][MAGIC]);
         #endif
        }
 }
 if (details == 0)
        {
         for (int i=0;i<db->ilosc[3];i++)
                {
                 #if OSTYPE == LINUX
                 printf("(%-3d)%-25s ",i,db->s_name[i]);
                 #elif OSTYPE == WINDOWS
                 printf("(%-3d)%-18s ",i,db->s_name[i]);
                 #endif
                 #if OSTYPE == LINUX
                 if (!(i % 5)) ln();
                 #elif OSTYPE == WINDOWS
                 if (!(i % 3)) ln();
                 #endif
                }
        }
 if (details == 2)
        {
         ShowCateg(db);
         lns(2);
         int temp;
         cout<<"Type category number: ";
         takeint(&temp);
         if ((temp>db->ilosc[4]-1) || (temp<0)) return;
         cout<<"Category:\t"<< db->c_name[temp]<<endl;
         for (int i=0;i<db->ilosc[3];i++)
             {
              if (db->s_mod[i][CATEG] == temp)
              #if OSTYPE == LINUX
              {
              printf("(%-3d)%-25s Attrib: (%-3d)%-20s test modifier %-3d Multiple %-3d Difficult %-3d category (%-3d)%-20s Magic %d\n",i,db->s_name[i],db->s_mod[i][ATTRIBUTE],db->name_att[db->s_mod[i][ATTRIBUTE]],db->s_mod[i][MOD],db->s_mod[i][MULTIPLE],db->s_mod[i][DIFF],db->s_mod[i][CATEG],db->c_name[db->s_mod[i][CATEG]],db->s_mod[i][MAGIC]);
              }
              #elif OSTYPE == WINDOWS
              {
              printf("%-3d.%-18s %-15s Md %-2d Ml %-2d D %-2d %-14s M %d\n",i,db->s_name[i],db->name_att[db->s_mod[i][ATTRIBUTE]],db->s_mod[i][MOD],db->s_mod[i][MULTIPLE],db->s_mod[i][DIFF],db->c_name[db->s_mod[i][CATEG]],db->s_mod[i][MAGIC]);
              }
              #endif

             }
        }
 if (details == 3)
        {
         ShowCateg(db);
         lns(2);
         int temp;
         cout<<"Type category numer: ";
         takeint(&temp);
         if ((temp>db->ilosc[4]-1) || (temp<0)) return;
         cout<<"Category:\t"<< db->c_name[temp]<<endl;
         int j = 0;
         for (int i=0;i<db->ilosc[3];i++)
             {
              if (db->s_mod[i][CATEG] == temp)
                {
                 j++;
                 #if OSTYPE == LINUX
                 printf("(%-3d)%-25s ",i,db->s_name[i]);
                 if (!(j % 5)) ln();
                 #elif OSTYPE == WINDOWS
                 printf("(%-3d)%-18s ",i,db->s_name[i]);
                 if (!(j % 3)) ln();
                 #endif
                }
             }
        }
 lns(2);
}
void DelSkill(DATA* db, short middle = 0)
{
fprintf(LOG,"Running DelSkill(DATA*,short = 0) offset 0x%x\n\tmiddle is %d\n",db,middle);
 if ((db->ilosc[3] == 0) && (middle != 0)){cout<<"There is nothing to erase!\n";fprintf(LOG,"LD: warning, nothing to erase\n");return;}
 if (middle == 0) {db->ilosc[3]--;return;}
 ShowSkill(db,0);
 lns(2);int temp;
 printf("Skill number you want to delete (0 - %d): ",db->ilosc[3]-1);
 takeint(&temp);
 if ((temp<0) || (temp>db->ilosc[3]-1)) return;
 db->ilosc[3]--;
 for (int i=temp;i<db->ilosc[3];i++)
        {
         strcpy(db->s_name[i],db->s_name[i+1]);
         for (int j=0;j<5;j++) db->s_mod[i][j] = db->s_mod[i+1][j];
        }
}
void FindSkill(DATA* db, int diff = 0)
{
 fprintf(LOG,"Running FindSkill(DATA*) offset is 0x%x\n",db);
 cout<<"Type part of skill name, I will find all like it: ";
 char part[STRING];
 int correct;
 int find = 0;
 takestring(part);
 cout<<"I've found skills:\n\n";
 for (int i=0;i<db->ilosc[3];i++)
        {
         correct = 0;
         for (int j=0;j<(int)strlen(part);j++)
                {
                 if (part[j] == db->s_name[i][j]) correct++;
                }
         if (correct == (int)strlen(part))
         	{
             if (!diff) printf("(%-3d) %s\n",i,db->s_name[i]);
             else printf("(%-3d) %-30s\t difficulty is %d\n",i,db->s_name[i],db->s_mod[i][DIFF]);
             find++;
            }
        }
 if (find == 0) cout<<"I couldn't find any skill like "<<part<<endl;
 fprintf(LOG,"LD: Find %d skills\n",find);
}
void AddSkill(DATA* db, short middle = 0,short search = 0)
{
 fprintf(LOG,"Running AddSkill(DATA*,short = 0) offset 0x%x\n\tmiddle is %d\nsearch = %d",db,middle,search);
 if ((db->ilosc[3] == MAX_SKI) && (middle != 0)) {cout<<"Skill record full!\n";fprintf(LOG,"LD: Error, record full\n");return;}
 int temp;
 if (middle == 0)
        {
         temp = db->ilosc[3];
        }
 else
        {
         if (!search) ShowSkill(db,0);
         else FindSkill(db);
         printf("Skill number you want to edit (0 - %d): ",db->ilosc[3]-1);
         takeint(&temp);
         if ((temp<0) || (temp>db->ilosc[3]-1)) return;
        }
 cout<<"Name of "<<temp<<" skill: ";
 takestring(db->s_name[temp]);
 ShowAttrib(db);lns(2);
 cout<<"Number of testing attrib: ";
 int data;takeint(&data);
 if (data<0) data = 0;
 if (data>db->ilosc[0]-1) data = db->ilosc[0]-1;
 db->s_mod[temp][ATTRIBUTE] = data;
 cout<<"Test modifier: ";
 takeint(&db->s_mod[temp][MOD]);
 cout<<"Multiply of skill: ";
 takeint(&db->s_mod[temp][MULTIPLE]);
 cout<<"Magic level: ";
 takeint(&db->s_mod[temp][MAGIC]);
 cout<<"Difficulty: ";
 takeint(&data);
 if (data<1) data = 1;
 db->s_mod[temp][DIFF] = data;
 lns(2);
 ShowCateg(db);
 lns(2);
 cout<<"Category number: ";
 takeint(&data);
 if (data<0) data = 0;
 if (data>db->ilosc[4]-1) data = db->ilosc[4]-1;
 db->s_mod[temp][CATEG] = data;
 if (middle == 0) db->ilosc[3]++;
 fprintf(LOG,"DATA fields modified\n");
}
void SortSkill(DATA* db)
{
 fprintf(LOG,"Running SortSkill(DATA*) offset 0x%x\n",db);
 ShowSkill(db,FALSE);
 cout<<"\n\nStart from: ";
 int z1,z2;takeint(&z1);
 cout<<"End on: ";
 takeint(&z2);
 if ((z1<0) || (z2<0)) return;
 if ((z1>db->ilosc[3]-1) || (z2>db->ilosc[3]-1)) return;
 if (z2<z1) z2 = z1;
 cout<<"Modify <"<<z1<<" - "<<z2<<">\n";
 if ((z2-z1)>30)
        {
         cout<<"There is more than 30 to modify, continue? ";
         fprintf(LOG,"LD: info,try to modify more than 30 skills\n");
         char zn;takechar(&zn);
         if (zn != 'y') return;
        }
 cout<<"OK\n";
 for (int i=z1;i<=z2;i++)
        {
         lns(3);
         printf("%d from <%d - %d>\n",i,z1,z2);
         cout<<"Skill "<<db->s_name[i]<<endl;
         ShowCateg(db);
         cout<<"Number of category or out of range for cancel: ";
         int num;takeint(&num);
         if ((num<0) || (num>db->ilosc[4]-1)) {cout<<"Canceled\n";return;}
         db->s_mod[i][CATEG] = num;
        }
}
void SortConv(DATA* db, int level)
{
 fprintf(LOG,"Running SortConv(DATA*,int) offset 0x%x\n\tlevel is %d\n",db,level);
 ShowSkill(db,FALSE);
 cout<<"\n\nStart from: ";
 int z1,z2;takeint(&z1);
 cout<<"End on: ";
 takeint(&z2);
 if ((z1<0) || (z2<0)) return;
 if ((z1>db->ilosc[3]-1) || (z2>db->ilosc[3]-1)) return;
 if (z2<z1) z2 = z1;
 cout<<"Modify <"<<z1<<" - "<<z2<<">\n";
 if ((z2-z1)>30)
        {
         cout<<"There is more than 30 to modify, continue? ";
         fprintf(LOG,"LD: info, try to modify more than 30 skills\n");
         char zn;takechar(&zn);
         if (zn != 'y') return;
        }
 cout<<"OK\n";
 if (level == 1)
        {
         for (int i=z1;i<=z2;i++)
                {
                 lns(3);
                 printf("%d from <%d - %d>\n",i,z1,z2);
                 cout<<"Skill "<<db->s_name[i]<<endl;
                 cout<<"Magic level or -1 for cancel: ";
                 int num;takeint(&num);
                 if (num == -1) {cout<<"Canceled\n";return;}
                 db->s_mod[i][MAGIC] = num;
                }
        }
 if (level == 2)
        {
         for (int i=z1;i<=z2;i++)
                {
                 lns(3);
                 printf("%d from <%d - %d>\n",i,z1,z2);
                 cout<<"Skill "<<db->s_name[i]<<endl;
                 cout<<"Difficulty or -1 for cancel: ";
                 int num;takeint(&num);
                 if (num == -1) {cout<<"Canceled\n";return;}
                 db->s_mod[i][DIFF] = num;
                }
        }
}
void SetSkill(DATA* db)
{
 fprintf(LOG,"Running SetSkill(DATA*), offset 0x%x\n",db);
 lns(2);
 ShowSkill(db);
 cout<<"\nMaximum skill "<<MAX_SKI<<endl;
 lns(2);
 cout<<"Change record? ";
 char zn;takechar(&zn);
 if (zn != 'y') {cout<<"Left unmodified...\n";return;}
 char z = '%';
 while (z != 'k')
   {
 lns(3);
 cout<<"\t\tNumber: "<<db->ilosc[3]<<endl;
 cout<<"a\t-Add skill\n";
 cout<<"b\t-Delete skill\n";
 cout<<"c\t-See with propeties\n";
 cout<<"d\t-See only names\n";
 cout<<"e\t-See categories with propeties\n";
 cout<<"f\t-See categories, only names\n";
 cout<<"g\t-Edit skill\n";
 cout<<"h\t-Kill choosen\n";
 cout<<"i\t-Sort with categories\n";
 cout<<"j\t-Kill'em all\n";
 cout<<"s\t-Save to file\n";
 cout<<"k\t-End\n";
 cout<<"l\t-Edit magic levels\n";
 cout<<"m\t-Edit skill difficulty\n";
 cout<<"v\t-Search & Edit skill\n";
 cout<<"z\t-Kill program\n";
 cout<<"\n\nChoose? ";
 takechar(&z);
 switch(z)
        {
         case 'a': AddSkill(db);break;
         case 'b': DelSkill(db);break;
         case 'c': ShowSkill(db);break;
         case 'd': ShowSkill(db,0);break;
         case 'e': ShowSkill(db,2);break;
         case 'f': ShowSkill(db,3);break;
         case 'g': AddSkill(db,TRUE);break;
         case 'h': DelSkill(db,TRUE);break;
         case 'i': SortSkill(db);break;
         case 'j': db->ilosc[3] = 0;break;
         case 'l': SortConv(db,1);break;
         case 'm': SortConv(db,2);break;
         case 's': QuickSave(db);break;
         case 'v': AddSkill(db,2,1);break;
         case 'z': fclose(LOG);
         #if OSTYPE == LINUX
         last_op(db,NULL);
         #endif
         exit( -1 );break;
         default: cout<<"Unknown command.\n";
        }
   }
 fprintf(LOG,"Skill already set\n");
}
void SetCateg(DATA* db)
{
 fprintf(LOG,"Running SetCateg(DATA) offset 0x%x\n",db);
 cout<<"Maximum categories "<<MAX_CATEG<<endl;
 cout<<"Found "<<db->ilosc[4]<<" defined categories.\n";
 ShowCateg(db);
 cout<<"Change record? ";
 char zn;takechar(&zn);
 if (zn != 'y') {cout<<"left unmodified...\n";return;}
 cout<<"Type number of categories: ";
 int temp;takeint(&temp);
 if (temp>MAX_CATEG) temp = MAX_CATEG;
 if (temp<1)       temp = 1;
 cout<<"Taken "<<temp<<" as number of categories.\n";
 db->ilosc[4] = temp;
 for (int i=0;i<db->ilosc[4];i++)
        {
         cout<<"Type name of "<<i<<"category: ";
         takestring(db->c_name[i]);
        }
}
void ShowLevels(DATA* db)
{
 fprintf(LOG,"Running ShowLevels(DATA) offset 0x%x\n",db);
 cout<<"\nNuber of levels is "<<db->ilosc[5]<<endl;
 for (int i=0;i<db->ilosc[5];i++)
        {
         cout<<"Level "<<i<<" Points cost: "<<db->cost[i]<<endl;
        }
 ln();
}
void ShowLevels2(DATA* db)
{
 fprintf(LOG,"Running ShowLevels2(DATA) offset 0x%x\n",db);
 cout<<"\nNumber of levels is "<<db->ilosc[6]<<endl;
 for (int i=0;i<db->ilosc[6];i++)
        {
         cout<<"Level "<<i<<" XP cost: "<<db->pd_cost[i]<<endl;
        }
 ln();
}
void SetLevels(DATA* db)
{
fprintf(LOG,"Running SetLevels(DATA*) offset 0x%x\n",db);
 cout<<"Maximum levels "<<MAX_LEVELS<<endl;
 ShowLevels(db);
 cout<<"Change levels? ";
 char zn;takechar(&zn);
 if (zn != 'y') {cout<<"Left unmodified...\n";return;}
 cout<<"Number of levels: ";
 int temp;
 takeint(&temp);
 if ((temp<0) || (temp>MAX_LEVELS)) return;
 db->ilosc[5] = temp;
 for (int i=0;i<temp;i++)
        {
         cout<<"Cost of "<<i<<" level: ";
         takeint(&db->cost[i]);
        }
}
void SetLevels2(DATA* db)
{
 fprintf(LOG,"Running SetLevels2(DATA*) offset 0x%x\n",db);
 cout<<"Maximum levels "<<MAX_LEVELS<<endl;
 ShowLevels2(db);
 cout<<"Change XP costs? ";
 char zn;takechar(&zn);
 if (zn != 'y') {cout<<"Left unmodified...\n";return;}
 cout<<"Number of levels: ";
 int temp;
 takeint(&temp);
 if ((temp<0) || (temp>MAX_LEVELS)) return;
 db->ilosc[6] = temp;
 for (int i=0;i<temp;i++)
        {
         cout<<"Cost of "<<i<<" level: ";
         takeint(&db->pd_cost[i]);
        }
}
void CheckPassword(DATA* db)
{
 fprintf(LOG,"Running CheckPassword(DATA*) offset 0x%x\n",db);
 int a;
 if (db->sv == -1) {cout<<"Access Denied!!!\n";fprintf(LOG,"LD: Error Author didn't set any permission to base\n");takeint(&a);fclose(LOG);
 #if OSTYPE == LINUX
 last_op(db,NULL);
 #endif
 exit( -1 );}
 if (db->sv != 0)
        {
         fprintf(LOG,"LD: Password Checking\n");
         cout<<"Enter current base password: ";
         takeint(&a);
         if (a != db->sv)
                {
                 fprintf(LOG,"User typed wrong password\n");
                 cout<<"Access Denied!\n";
                 takeint(&a);
                 fclose(LOG);
                 #if OSTYPE == LINUX
                 last_op(db,NULL);
                 #endif
                 exit( -1 );
                }
         else {cout<<"OK\n";fprintf(LOG,"LD: user correctly typed password\n");}
        }
 else fprintf(LOG,"LD: No passwword set, you should do it!\n");
 cout<<"Change password? ";
 char zn;takechar(&zn);
 if (zn == 'y') {fprintf(LOG,"Password modified to , it is no so easy to get password :)\n");cout<<"Type new or -1 Access Denied: ";takeint(&db->sv);}
}
void ShowWoman(DATA* db)
{
 fprintf(LOG,"Running ShowWoman(DATA*) offset 0x%x\n",db);
 fprintf(LOG,"Yet body!..\n");
 cout<<"Number of attributes: "<<db->ilosc[0]<<endl<<endl;
 for (int i=0;i<db->ilosc[0];i++)
        {
         printf("(%-3d)%-25s\t%-3d\n",i,db->name_att[i],db->p_mod[i]);
        }
 cout<<"This was woman modifiers\n";
}
void SetGender(DATA* db)
{
 fprintf(LOG,"Running SetGender(DATA*) offset 0x%x ( * )( * )\n",db);
 ShowWoman(db);
 ln();
 cout<<"Change gender modifiers ( * )( * )? ";
 char zn;takechar(&zn);
 if (zn != 'y') return;
 for (int i=0;i<db->ilosc[0];i++)
        {
         printf("What is %-25s modifier for woman, current %-3d: ",db->name_att[i],db->p_mod[i]);
         takeint(&db->p_mod[i]);
        }
}
void Operations(DATA* base)
{
 fprintf(LOG,"Running Operations(DATA*) offset 0x%x\n",base);
 CheckPassword(base);
 SetAttrib(base);
 SetTrial(base,0);
 SetTrial(base,1);
 SetCateg(base);
 SetSkill(base);
 SetLevels(base);
 SetLevels2(base);
 SetGender(base);
 cout<<"\n\nYou have configured data base.\n";
 fprintf(LOG,"Base configured\n");
}
void BaseToHTML(DATA* db, FILE* plik, char* name)
{
 fprintf(LOG,"Running BaseToHTML(DATA*,FILE*,char*)\n");
 fprintf(LOG,"\tDATA offset 0x%x\n",db);
 fprintf(LOG,"\tFILE offset 0x%x\n",plik);
 fprintf(LOG,"\tFile name is %s\n",name);
 fprintf(plik,"<html>\n");
 fprintf(plik,"<head>\n");
 fprintf(plik,"<title>\n");
 fprintf(plik,"Data Base %s\n",name);
 fprintf(plik,"</title>\n");
 fprintf(plik,"</head>\n");
 fprintf(plik,"<body>\n");
 fprintf(plik,"<center><big>Data Base %s</center></big>\n",name);
 fprintf(plik,"<br>\n");
 fprintf(plik,"<center>Number of attributes %d</center>\n",db->ilosc[0]);
 fprintf(plik,"<center><table border =\"3\">\n");
 fprintf(plik,"<tr>\n");
 fprintf(plik,"<td>Number\n");
 fprintf(plik,"<td>Attribute\n");
 fprintf(plik,"<td>Start Value\n");
 for (int i=0;i<db->ilosc[0];i++)
        {
         fprintf(plik,"<tr>\n");
         fprintf(plik,"<td>%d\n",i+1);
         fprintf(plik,"<td>%s\n",db->name_att[i]);
         fprintf(plik,"<td>%d\n",db->value_att[i]);
        }
 fprintf(plik,"</table></center>\n");
 fprintf(plik,"<br>\n");
 fprintf(plik,"<center>Number of races %d</center>\n",db->ilosc[1]);
 fprintf(plik,"<center><table border =\"3\">\n");
 fprintf(plik,"<tr>\n");
 fprintf(plik,"<td>Number\n");
 fprintf(plik,"<td>Race\n");
 for (int i=0;i<db->ilosc[0];i++)
        {
         fprintf(plik,"<td>%c%c%c\n",db->name_att[i][0],db->name_att[i][1],db->name_att[i][2]);
        }
 for (int i=0;i<db->ilosc[1];i++)
        {
         fprintf(plik,"<tr>\n");
         fprintf(plik,"<td>%d\n",i+1);
         fprintf(plik,"<td>%s\n",db->name[0][i]);
         for (int j=0;j<db->ilosc[0];j++)
                {
                 fprintf(plik,"<td>%d\n",db->mod[0][i][j]);
                }
        }
 fprintf(plik,"</table></center>\n");
 fprintf(plik,"<br>\n");
 fprintf(plik,"<center>Number of professies %d</center>\n",db->ilosc[2]);
 fprintf(plik,"<center><table border =\"3\">\n");
 fprintf(plik,"<tr>\n");
 fprintf(plik,"<td>Number\n");
 fprintf(plik,"<td>Profession\n");
 for (int i=0;i<db->ilosc[0];i++)
        {
         fprintf(plik,"<td>%c%c%c\n",db->name_att[i][0],db->name_att[i][1],db->name_att[i][2]);
        }
 for (int i=0;i<db->ilosc[2];i++)
        {
         fprintf(plik,"<tr>\n");
         fprintf(plik,"<td>%d\n",i+1);
         fprintf(plik,"<td>%s\n",db->name[1][i]);
         for (int j=0;j<db->ilosc[0];j++)
                {
                 fprintf(plik,"<td>%d\n",db->mod[1][i][j]);
                }
        }
 fprintf(plik,"</table></center>\n");
 fprintf(plik,"<br>\n");
 fprintf(plik,"<center>Number of skills %d</center>\n",db->ilosc[3]);
 fprintf(plik,"<center><table border =\"3\">\n");
 fprintf(plik,"<tr>\n");
 fprintf(plik,"<td>Number\n");
 fprintf(plik,"<td>Skil\n");
 fprintf(plik,"<td>Multiple\n");
 fprintf(plik,"<td>Num.Attrib\n");
 fprintf(plik,"<td>Attrib\n");
 fprintf(plik,"<td>Mod\n");
 fprintf(plik,"<td>Diff\n");
 fprintf(plik,"<td>Num.Categ\n");
 fprintf(plik,"<td>Category\n");
 fprintf(plik,"<td>Magic\n");
 for (int i=0;i<db->ilosc[3];i++)
        {
         fprintf(plik,"<tr>\n");
         fprintf(plik,"<td>%d\n",i+1);
         fprintf(plik,"<td>%s\n",db->s_name[i]);
         fprintf(plik,"<td>%d\n",db->s_mod[i][MULTIPLE]);
         fprintf(plik,"<td>%d\n",db->s_mod[i][ATTRIBUTE]);
         fprintf(plik,"<td>%s\n",db->name_att[db->s_mod[i][ATTRIBUTE]]);
         fprintf(plik,"<td>%d\n",db->s_mod[i][MOD]);
         fprintf(plik,"<td>%d\n",db->s_mod[i][DIFF]);
         fprintf(plik,"<td>%d\n",db->s_mod[i][CATEG]);
         fprintf(plik,"<td>%s\n",db->c_name[db->s_mod[i][CATEG]]);
         fprintf(plik,"<td>%d\n",db->s_mod[i][MAGIC]);
        }
 fprintf(plik,"</table></center>\n");
 fprintf(plik,"<br>\n");
 fprintf(plik,"<center>Number of categories %d</center>\n",db->ilosc[4]);
 fprintf(plik,"<center><table border =\"3\">\n");
 fprintf(plik,"<tr>\n");
 fprintf(plik,"<td>Number\n");
 fprintf(plik,"<td>category\n");
 for (int i=0;i<db->ilosc[4];i++)
        {
         fprintf(plik,"<tr>\n");
         fprintf(plik,"<td>%d\n",i+1);
         fprintf(plik,"<td>%s\n",db->c_name[i]);
        }
 fprintf(plik,"</table></center>\n");
 fprintf(plik,"<br>\n");
 fprintf(plik,"<center>Number of skill levels %d</center>\n",db->ilosc[5]);
 fprintf(plik,"<center><table border =\"3\">\n");
 fprintf(plik,"<tr>\n");
 fprintf(plik,"<td>Level\n");
 fprintf(plik,"<td>Cost in SP\n");
 for (int i=0;i<db->ilosc[5];i++)
        {
         fprintf(plik,"<tr>\n");
         fprintf(plik,"<td>%d\n",i);
         fprintf(plik,"<td>%d\n",db->cost[i]);
        }
 fprintf(plik,"</table></center>\n");
 fprintf(plik,"<br>\n");
 fprintf(plik,"<center>Number of Experience levels %d</center>\n",db->ilosc[6]);
 fprintf(plik,"<center><table border =\"3\">\n");
 fprintf(plik,"<tr>\n");
 fprintf(plik,"<td>Level\n");
 fprintf(plik,"<td>Cost in XP\n");
 for (int i=0;i<db->ilosc[6];i++)
        {
         fprintf(plik,"<tr>\n");
         fprintf(plik,"<td>%d\n",i);
         fprintf(plik,"<td>%d\n",db->pd_cost[i]);
        }
 fprintf(plik,"</table></center>\n");
 fprintf(plik,"<br>\n");
 fprintf(plik,"<center>Gender modifier(woman)</center>\n");
 fprintf(plik,"<center><table border =\"3\">\n");
 fprintf(plik,"<tr>\n");
 fprintf(plik,"<td>Attrib\n");
 fprintf(plik,"<td>Modifier\n");
 for (int i=0;i<db->ilosc[0];i++)
        {
         fprintf(plik,"<tr>\n");
         fprintf(plik,"<td>%s\n",db->name_att[i]);
         fprintf(plik,"<td>%d\n",db->p_mod[i]);
        }
 fprintf(plik,"</table></center>\n");
 fprintf(plik,"<br>\n");
 fprintf(plik,"<center>Data Base generated by rpg by morgothdbma copyleft@</center>\n",name);
 fprintf(plik,"</body>\n");
 fprintf(plik,"</html>\n");
 fprintf(LOG,"LD: File %s sucessfully writen\n",name);
}
void BaseHTML(DATA* db, char* file_name)
{
 fprintf(LOG,"Running BaseHTML(DATA*,char*)\n");
 fprintf(LOG,"\tDATA offset 0x%x\n",db);
 fprintf(LOG,"\tTemp file name is %s\n",file_name);
 char name[100];
 strcpy(name,file_name);
 strcat(name,".html");
 fprintf(LOG,"Name morphed to %s\n",name);
 cout<<"Generate HTML file "<<name<<"? ";
 char zn;
 takechar(&zn);
 if (zn != 'y') return;
 FILE* html;
 if ((html = fopen(name,"r+")) != NULL)
        {
         fprintf(LOG,"LD: warning, such HTML file already exists\n");
         cout<<"Overwrite file "<<name<<"? ";
         takechar(&zn);
         if (zn != 'y') {fclose(html);fprintf(LOG,"Overwritting...\n");return;}
        }
 if ((html = fopen(name,"w+")) == NULL)
        {
         cout<<"Fatal Reflection: Cannot write to "<<name<<endl;
         fprintf(LOG,"LD: Fatal error, cannot write to %s, unfixed, continue without HTML\n");
         return;
        }
 cout<<"Saving HTML format...\n";
 BaseToHTML(db,html,name);
 fclose(html);
 fprintf(LOG,"HTML file closed\n");
}
void DataBase(char* db, DATA* base)
{
 fprintf(LOG,"Running DataBase(char*,DATA*)\n");
 fprintf(LOG,"\tName is %s\n\tOffset is 0x%x\n",db,base);
 FILE* plik;
 fprintf(LOG,"Trying to read base from file\n");
 if ((plik = fopen(db,"r+")) == NULL)
        {
         cout<<"Fatal Reflection: \tNo such file: "<<db<<endl;
         fprintf(LOG,"LD: Fatal Error file %s doesn't exists\n",db);
         fprintf(LOG,"LD: Create this file with default base\n");
         if ((plik = fopen(db,"w+")) == NULL)
                {
                 fprintf(LOG,"File %s cannot be created, read-only fs or you have no permission\n",db);
                 cout<<"Fatal reflection:\tCannot write to "<<db<<"\n";
                 fclose(LOG);
                 #if OSTYPE == LINUX
                 last_op(base,NULL);
                 #endif
                 exit( -1 );
                }
         else fclose(plik);
         cout<<"File "<<db<<" created, restart application\n\n";
         fprintf(LOG,"File %s created\nLD: sending RESTART signal\n");
         fclose(LOG);
         #if OSTYPE == LINUX
         last_op(base,NULL);
         #endif
         exit( -1 );
        }
 Load(base,plik);
 fprintf(LOG,"Improving RunTimes from %d to %d\n",base->lr,base->lr+1);
 base->lr++;
 if (base->lr == 1000) base->lr = 1000;
 fclose(plik);
 fprintf(LOG,"Base read, now saving and closing file\n");
 if ((plik = fopen(db,"w+"))==NULL)
        {
         fprintf(LOG,"LD: Cannot save data base in %s\n",db);
         cout<<"Fatal Reflection: Cannot write to "<<db<<endl;
         fclose(LOG);
         #if OSTYPE == LINUX
         last_op(base,NULL);
         #endif
         exit( -1 );
        }
  fprintf(LOG,"File opened now saving...\n");
 Save(base,plik);
 fprintf(LOG,"Saved.\n");
 fclose(plik);
 fprintf(LOG,"File %s closed\n",db);
 cout<<"Edit base "<<db<<"? ";
 char zn;takechar(&zn);
 if (zn != 'y') return;
 fprintf(LOG,"Operations on base 0x%x\n",base);
 Operations(base);
 fprintf(LOG,"Gonna save your changes\n");
 if ((plik = fopen(db,"w+"))==NULL)
        {
         fprintf(LOG,"LD: Very Fatal Error (VFE) Cannot save data-base in %s\n",db);
         cout<<"Fatal Reflection: Cannot write to "<<db<<endl;
         fclose(LOG);
         #if OSTYPE == LINUX
         last_op(base,NULL);
         #endif
         exit( -1 );
        }
 Save(base,plik);
 fprintf(LOG,"Base saved\n");
 fclose(plik);
 fprintf(LOG,"About HTML file\n");
 BaseHTML(base,db);
}
// end of base area;







//start RPG area      //this is your main structure to create character

struct RPG
{
 RPG();
 ~RPG();
 int ilosc_danych;
 int ilosc_cech;
 int sv;
 int st;
 int cecha[MAX_ATT];
 char n_cechy[MAX_ATT][STRING];
 char dane[DANE_POSTACI][STRING];
 int il_um;
 char um[MAX_UM][STRING];
 int pozum[MAX_UM];
 int tesum[MAX_UM];
 int trum[MAX_UM];
 int ceum[MAX_UM];
 int mulum[MAX_UM];
 int magum[MAX_UM];
 int il_ekw;
 int PD;
 int p_c[MAX_ATT];
 int p_u[MAX_UM];
 char ekw[MAX_EKW][STRING];
 int rl;
 int stMM;
 int numMM;
 int IsDead;
};
RPG :: ~RPG(){fprintf(LOG,"Running RPG :: ~RPG(), deleting offset 0x%x, memory freed %d\n",this,sizeof(*this));}

RPG :: RPG()
{
 fprintf(LOG,"Running RPG :: RPG(void) setting up offset 0x%x, geting memory %d bytes\n",this,sizeof(*this));
 ilosc_danych = 11;
 ilosc_cech = 13;
 sv = 0;
 st = 1;
 il_um = 0;
 il_ekw = 0;
 PD = 0;
 rl = 0;
 IsDead = 0;
 for (int i=0;i<DANE_POSTACI;i++)
        {
         strcpy(dane[i],"SOME_DATA");
        }
 for (int i=0;i<MAX_ATT;i++)
        {
         cecha[i] = 10;
         p_c[i] = 0;
         strcpy(n_cechy[i],"SOME_DATA");
        }
 stMM = 10;
 for (int i=0;i<MAX_UM;i++)
        {
         strcpy(um[i],"SOME_SKILL");
         tesum[i] = 0;
         pozum[i] = 0;
         trum[i]  = 1;
         mulum[i] = 3;
         ceum[i]  = 0;
         p_u[i]   = 0;
         magum[i] = 0;
        }
 for (int i=0;i<MAX_EKW;i++)
        {
         strcpy(ekw[i],"SOME_EQUIP");
        }
fprintf(LOG,"LD: Class RPG succesfully created\n");
}
#if OSTYPE == LINUX
void last_op(DATA* db, RPG* post)
{
 cout<<"You're about terminate application\nMake Memshot? ";
 char zn;
 cin>>zn;
 if (zn == 'y') MemShot(db,post);
}
#endif
#if OSTYPE == LINUX
void MemShot(RPG* post)
{
 char temp[STRING];
 strcpy(temp,"Characters/memshot");
 int i = 0;
 FILE* plik;
 char name[STRING];
 char lbs[STRING];
 int opened = 0;
 while  (!opened)
        {
          sprintf(lbs,"%d",i);
          strcpy(name,temp);
          strcat(name,lbs);
          if  ((plik = fopen(name,"r+")) != NULL) {cout<<name<<" already exists...\n";opened = 0;fclose(plik);}
          else {cout<<"I will write to "<<name<<endl;opened = 1;}
          i++;
        }
  if ((plik = fopen(name,"w+")) == NULL)
        {
         cout<<"Cannot write to "<<name<<endl;
         return;
        }
 cout<<"Save to "<<name<<endl;
 //will save

 fprintf(plik,"%d\n",post->ilosc_danych);
 fprintf(plik,"%d\n",post->ilosc_cech);
 fprintf(plik,"%d\n",post->sv);
 fprintf(plik,"%d\n",post->st);
 fprintf(plik,"%d\n",post->il_um);
 fprintf(plik,"%d\n",post->il_ekw);
 fprintf(plik,"%d\n",post->PD);
 fprintf(plik,"%d\n",post->rl);
 fprintf(plik,"%d\n",post->stMM);
 fprintf(plik,"%d\n",post->numMM);
 for (int i=0;i<DANE_POSTACI;i++)
        {
         fprintf(plik,"%s\n",post->dane[i]);
        }
 for (int i=0;i<MAX_EKW;i++)
        {
         fprintf(plik,"%s\n",post->ekw[i]);
        }
  for (int i=0;i<MAX_UM;i++)
        {
         fprintf(plik,"%s\n",post->dane[i]);
         fprintf(plik,"%s\n",post->um[i]);
         fprintf(plik,"%d\n",post->pozum[i]);
         fprintf(plik,"%d\n",post->tesum[i]);
         fprintf(plik,"%d\n",post->trum[i]);
         fprintf(plik,"%d\n",post->ceum[i]);
         fprintf(plik,"%d\n",post->mulum[i]);
         fprintf(plik,"%d\n",post->magum[i]);
         fprintf(plik,"%d\n",post->p_u[i]);
        }
   for (int i=0;i<MAX_ATT;i++)
        {
         fprintf(plik,"%s\n",post->n_cechy[i]);
         fprintf(plik,"%d\n",post->cecha[i]);
         fprintf(plik,"%d\n",post->p_c[i]);
        }
 fprintf(plik,"%d\n",IsDead);
 //already saved
 cout<<"Done Memshot.\n";
}
#endif
void RealMM(RPG* post)
{
 fprintf(LOG,"Running RealMM(RPG*) offset 0x%x\n",post);
 int MM = post->numMM;
 post->cecha[MM] = post->stMM;
 for (int i=0;i<post->il_um;i++)
        {
         post->cecha[MM]+= post->magum[i]*(post->pozum[i]-1);
        }
  fprintf(LOG,"MM computed is %d\n",post->cecha[MM]);
}
void Protection(int check, RPG* post)
{
 fprintf(LOG,"Running Protection(int,RPG*) offset 0x%x\n\tIs Correct = %d\n",post,check);
 if (!check)
        {
         fprintf(LOG,"LD: error character not cleanly loaded, suggest terminate\n");
         lns(6);
         cout<<"Character doesn't look to be correct written...\n";
         cout<<"Recommended close program and character converting\n";
         cout<<"To convert character type rpg name -cc, name means\n";
         cout<<"Character name, you can continue but it may be cause for\n";
         cout<<"Bad character save, do not do it without backup of character file\n";
         cout<<"If convert doesn't help it mean that file was not cleanly saved\n";
         cout<<"Is corupted or no memory for load data into RAM\n";
         lns(4);
         char zn;
         cout<<"Proceed? ";
         takechar(&zn);
         if (zn != 'y') {delete post; fprintf(LOG,"Application halted!\n");fclose(LOG);
         #if OSTYPE == LINUX
         last_op(NULL,post);
         #endif
         exit( -1 );}
        }
}
void Draw()
{
 fprintf(LOG,"Running Draw(void) drawing frames\n");
 #if OSTYPE == LINUX
 cout<<" *";
 for (int i=0;i<100;i++) cout<<"-";
 cout<<"*\n";
 #elif OSTYPE == WINDOWS
 cout<<" *";
 for (int i=0;i<72;i++) cout<<"-";
 cout<<"*\n";
 #endif
}
void ShowAll(RPG* post)
{
 fprintf(LOG,"Running ShowAll(RPG*) offset is 0x%x\n",post);
 RealMM(post);
 fprintf(LOG,"Choosing function for current OS...\n");
 #if OSTYPE == LINUX
 lns(10);
 Draw();
 printf("\n\t\tCharacter     %s %s \tXP %d Gend %s\n",post->dane[IMIE],post->dane[NAZWISKO],post->PD,post->dane[PLEC]);
 Draw();
 printf(" |Name:     \t%-25s|\tSurname :\t%-25s             |\n",post->dane[IMIE],post->dane[NAZWISKO]);
 printf(" |Race:     \t%-25s|\tProf    :\t%-25s             |\n",post->dane[RASA],post->dane[PROFESJA]);
 printf(" |Eyes:     \t%-25s|\tHair :   \t%-25s             |\n",post->dane[OCZY],post->dane[WLOSY]);
 printf(" |Height:   \t%-25s|\tWeight:  \t%-25s             |\n",post->dane[WZROST],post->dane[WAGA]);
 printf(" |Character:\t%-45s \t         \t%-5s                 |\n",post->dane[CHARAKTER],"");
 Draw();
 int max = (post->ilosc_cech > post->il_um) ? post->ilosc_cech : post->il_um;
 for (int i=0;i<max;i++)
        {
         if (i<post->ilosc_cech)
         printf(" |  [%-3d]%-29s\t %-3d",post->p_c[i],post->n_cechy[i],post->cecha[i]);
         else
         printf(" | %-3s %-31s\t %-3s","","","");
         if (i<post->il_um)
         printf("|\t[%-3d](%-3d)%-25s %-3d %-3d\t      |",post->p_u[i],post->trum[i],post->um[i],post->pozum[i],post->tesum[i]);
         else
         printf("|\t  %-3s %-3s %-25s %-3s %-3s\t      |","","","","","");
         ln();
        }
 Draw();
 for (int i=0;i<post->il_ekw;i++)
        {
         printf(" | %-99s|\n",post->ekw[i]);
        }
 Draw();
 lns(3);
 #elif OSTYPE == WINDOWS
 lns(2);
 Draw();
 printf("\n\tCharacter     %s %s  XP %d PLEC %s\n",post->dane[IMIE],post->dane[NAZWISKO],post->PD,post->dane[PLEC]);
 Draw();
 printf(" |Name:     \t%-18s |\tSurnameO:\t%-18s|\n",post->dane[IMIE],post->dane[NAZWISKO]);
 printf(" |Race:     \t%-18s |\tProf    :\t%-18s|\n",post->dane[RASA],post->dane[PROFESJA]);
 printf(" |Eyes:     \t%-18s |\tHair :   \t%-18s|\n",post->dane[OCZY],post->dane[WLOSY]);
 printf(" |Height:   \t%-18s |\tWeight:  \t%-18s|\n",post->dane[WZROST],post->dane[WAGA]);
 printf(" |Character:\t%-25s  \t         \t%-5s     |\n",post->dane[CHARAKTER],"");
 Draw();
 int max = (post->ilosc_cech > post->il_um) ? post->ilosc_cech : post->il_um;
 for (int i=0;i<max;i++)
        {
         if (i<post->ilosc_cech)
         printf(" | [%-2d]%-18s  %-3d",post->p_c[i],post->n_cechy[i],post->cecha[i]);
         else
         printf(" |%-2s %-18s  %-3s  ","","","");
         if (i<post->il_um)
         printf("|  [%-2d](%-2d)%-18s %-2d %-2d\t  |",post->p_u[i],post->trum[i],post->um[i],post->pozum[i],post->tesum[i]);
         else
         printf("|    %-2s %-2s %-18s %-2s %-2s\t  |","","","","","");
         ln();
        }
 Draw();
 for (int i=0;i<post->il_ekw;i++)
        {
         printf(" | %-70s |\n",post->ekw[i]);
        }
 Draw();
 lns(3);
 #endif
}
void LoadChar(RPG* post, FILE* plik)
{
 fprintf(LOG,"Running LoadChar(RPG*,FILE*) RPG offset 0x%x\n\tFILE offset 0x%x\n",post,plik);
 cout<<"Loading character definition.\n";
 int check;
 check = fscanf(plik,"%d\n",&post->ilosc_danych);
 Protection(check,post);
 for (int i=0;i<post->ilosc_danych;i++)
        {
         fscanf(plik,"%s\n",post->dane[i]);
         DecompressData(post->dane[i]);
        }
 fscanf(plik,"%d\n",&post->ilosc_cech);
 for (int i=0;i<post->ilosc_cech;i++)
        {
         fscanf(plik,"%d\n",&post->cecha[i]);
         fscanf(plik,"%s\n",post->n_cechy[i]);
         DecompressData(post->n_cechy[i]);
        }
 fscanf(plik,"%x\n",&post->sv);
 fscanf(plik,"%d\n",&post->st);
 fscanf(plik,"%d\n",&post->il_um);
 for (int i=0;i<post->il_um;i++)
        {
         fscanf(plik,"%s %d %d %d %d %d\n",post->um[i],&post->tesum[i],&post->pozum[i],&post->trum[i],&post->ceum[i],&post->mulum[i]);
         DecompressData(post->um[i]);
        }
 fscanf(plik,"%d\n",&post->il_ekw);
 for (int i=0;i<post->il_ekw;i++)
        {
         fscanf(plik,"%s\n",post->ekw[i]);
         DecompressData(post->ekw[i]);
        }
 fscanf(plik,"%d\n",&post->PD);
 for (int i=0;i<post->ilosc_cech;i++)
        {
         fscanf(plik,"%d\n",&post->p_c[i]);
        }
 for (int i=0;i<post->il_um;i++)
        {
         fscanf(plik,"%d\n",&post->p_u[i]);
        }
 check = fscanf(plik,"%d\n",&post->rl);
 Protection(check,post);
 for (int i=0;i<post->il_um;i++)
        {
         fscanf(plik,"%d\n",&post->magum[i]);
        }
 fscanf(plik,"%d\n",&post->stMM);
 fscanf(plik,"%d\n",&post->numMM);
 fscanf(plik,"%d\n",&post->IsDead);
 if (post->IsDead)
 	{
    lns(5);
    cout<<"This character is DEAD.\n";
    cout<<"There is no way to edit corpse, you can see it: type view char_name\n";
    fprintf(LOG,"LD:\tWarning LOADED CHARACTER IS DEAD, TERMINATE!\n");
    fclose(LOG);
    #if OSTYPE == LINUX
    last_op(NULL,post);
    #endif
    EndWin();
    exit( -1 );
   }
 fprintf(LOG,"Charcter loaded\n");
}
void Uncompress(RPG* post)
{
 fprintf(LOG,"Running Uncompress(RPG*) offset 0x%x\n",post);
 cout<<"Uncompressing data\n";
 for (int i=0;i<post->ilosc_danych;i++)
        {
         DecompressData(post->dane[i]);
        }
 for (int i=0;i<post->ilosc_cech;i++)
        {
         DecompressData(post->n_cechy[i]);
        }
 for (int i=0;i<post->il_um;i++)
        {
         DecompressData(post->um[i]);
        }
 for (int i=0;i<post->il_ekw;i++)
        {
         DecompressData(post->ekw[i]);
        }
}
void SaveChar(RPG* post, FILE* plik,char* fn)
{
 fprintf(LOG,"Running SaveChar(RPG*,FILE*,char*)\n");
 fprintf(LOG,"\tRPG offset 0x%x\n",post);
 fprintf(LOG,"\tFILE offset 0x%x\n",plik);
 fprintf(LOG,"\tFile name is %s\n",fn);
 if ((plik = fopen(fn,"w+")) == NULL)
        {
         fprintf(LOG,"LD: Error File %s cannot be open\n",fn);
         cout<<"Fatal Reflection: Cannot write to "<<fn<<endl;
         fclose(LOG);
         #if OSTYPE == LINUX
         last_op(NULL,post);
         #endif
         exit( -1 );
        }
 else cout<<"Opening file "<<fn<<endl;
 fprintf(LOG,"Now saving RPG class fields\n");
 cout<<"Saving character definition.\n";
 fprintf(plik,"%d\n",post->ilosc_danych);
 for (int i=0;i<post->ilosc_danych;i++)
        {
         CompressData(post->dane[i]);
         fprintf(plik,"%s\n",post->dane[i]);
        }
 fprintf(plik,"%d\n",post->ilosc_cech);
 for (int i=0;i<post->ilosc_cech;i++)
        {
         fprintf(plik,"%d\n",post->cecha[i]);
         CompressData(post->n_cechy[i]);
         fprintf(plik,"%s\n",post->n_cechy[i]);
        }
 fprintf(plik,"%x\n",post->sv);
 fprintf(plik,"%d\n",post->st);
 fprintf(plik,"%d\n",post->il_um);
 for (int i=0;i<post->il_um;i++)
        {
         CompressData(post->um[i]);
         fprintf(plik,"%s %d %d %d %d %d\n",post->um[i],post->tesum[i],post->pozum[i],post->trum[i],post->ceum[i],post->mulum[i]);
        }
 fprintf(plik,"%d\n",post->il_ekw);
 for (int i=0;i<post->il_ekw;i++)
        {
         CompressData(post->ekw[i]);
         fprintf(plik,"%s\n",post->ekw[i]);
        }
 fprintf(plik,"%d\n",post->PD);
 for (int i=0;i<post->ilosc_cech;i++)
        {
         fprintf(plik,"%d\n",post->p_c[i]);
        }
 for (int i=0;i<post->il_um;i++)
        {
         fprintf(plik,"%d\n",post->p_u[i]);
        }
 fprintf(plik,"%d\n",post->rl);
  for (int i=0;i<post->il_um;i++)
        {
         fprintf(plik,"%d\n",post->magum[i]);
        }
 fprintf(plik,"%d\n",post->stMM);
 fprintf(plik,"%d\n",post->numMM);
 fprintf(plik,"%d\n",post->IsDead);
 fclose(plik);
 Uncompress(post);
}
void lnf(FILE* plik)
{
 fprintf(LOG,"-<\n");
 fprintf(plik,"\n");
}
void lnfs(int num, FILE* p)
{
 fprintf(LOG,"%d Lines to file offset 0x%x\n",num,p);
 for (int i=0;i<num;i++) lnf(p);
 fprintf(LOG,"\n");
}
void Alternative(RPG* p, FILE* f)
{
 fprintf(LOG,"Running Alternative(RPG*, FILE*) RPG offset is 0x%x\n\tFILE offset is 0x%x\n",p,f);
 fprintf(f,"<center>\n");
 fprintf(f,"<table border = \"3\">\n");
 fprintf(f,"<tr>\n");
 for (int i=0;i<10;i++) DecompressData(p->dane[i]);
 fprintf(f,"<td>Name: %s\n",p->dane[IMIE]);
 fprintf(f,"<td>Surnmae: %s\n",p->dane[NAZWISKO]);
 fprintf(f,"<td>Race: %s\n",p->dane[RASA]);
 fprintf(f,"<td>Profession: %s\n",p->dane[PROFESJA]);
 fprintf(f,"<tr>\n");
 fprintf(f,"<td>Height: %s\n",p->dane[WZROST]);
 fprintf(f,"<td>Weight: %s\n",p->dane[WAGA]);
 fprintf(f,"<td>Hair: %s\n",p->dane[WLOSY]);
 fprintf(f,"<td>Eyes: %s\n",p->dane[OCZY]);
 fprintf(f,"</table>\n");
 fprintf(f,"<table border = \"3\">\n");
 fprintf(f,"<tr>\n");
 fprintf(f,"<td>Character: %s\n",p->dane[CHARAKTER]);
 fprintf(f,"<td>Skin: %s\n",p->dane[CERA]);
 fprintf(f,"</table>\n");
 int max = (p->ilosc_cech > p->il_um) ? p->ilosc_cech : p->il_um;
 fprintf(f,"<table border =\"3\">\n");
 fprintf(f,"<tr>\n");
 fprintf(f,"<td>Pl\n");
 fprintf(f,"<td>Attrib\n");
 fprintf(f,"<td>value\n");
 fprintf(f,"<td>Pl\n");
 fprintf(f,"<td>Diff\n");
 fprintf(f,"<td>Skill\n");
 fprintf(f,"<td>Level\n");
 fprintf(f,"<td>Test\n");
 for (int i=0;i<max;i++)
        {
         fprintf(f,"<tr>\n");
         if (i<p->ilosc_cech)
                {
                 DecompressData(p->n_cechy[i]);
                 fprintf(f,"<td>[%d]\n",p->p_c[i]);
                 fprintf(f,"<td>%s\n",p->n_cechy[i]);
                 fprintf(f,"<td>%d\n",p->cecha[i]);
                }
         else
                {
                 fprintf(f,"<td>-\n");
                 fprintf(f,"<td>-\n");
                 fprintf(f,"<td>-\n");
                }
         if (i<p->il_um)
                {
                 DecompressData(p->um[i]);
                 fprintf(f,"<td>[%d]\n",p->p_u[i]);
                 fprintf(f,"<td>(%d)\n",p->trum[i]);
                 fprintf(f,"<td>%s\n",p->um[i]);
                 fprintf(f,"<td>%d\n",p->pozum[i]);
                 fprintf(f,"<td>%d\n",p->tesum[i]);

                }
         else
                {
                 fprintf(f,"<td>-\n");
                 fprintf(f,"<td>-\n");
                 fprintf(f,"<td>-\n");
                 fprintf(f,"<td>-\n");
                 fprintf(f,"<td>-\n");
                }
        }
 fprintf(f,"</table>\n");
 fprintf(f,"</center>\n");
 fprintf(f,"<center><big<big><u>Equip:</u></big></big></center>\n");
 fprintf(f,"<pre>\n");
 if (p->il_ekw == 0) fprintf(f,"No equip\n");
 for (int i=0;i<p->il_ekw;i++)
        {
         DecompressData(p->ekw[i]);
         fprintf(f,"%s\n",p->ekw[i]);
        }
 fprintf(f,"</pre>\n");
 fprintf(LOG,"Kernel of HTML saved\n");
}
void WriteHTML(RPG* post, FILE* plik,char* fn)
{
 fprintf(LOG,"Running WriteHTML(RPG*,FILE*,char*)\n");
 fprintf(LOG,"RPG offset is 0x%x\n",post);
 fprintf(LOG,"FILE offset is 0x%x\n",plik);
 fprintf(LOG,"File name is %s\n",fn);
 fprintf(plik,"<html>\n");
 fprintf(plik,"<title>\n");
 fprintf(plik,"Charater Card %s %s  XP %d Gender: %s\n",post->dane[IMIE],post->dane[NAZWISKO],post->PD, post->dane[PLEC]);
 fprintf(plik,"</title>\n");
 fprintf(plik,"<head>\n");
 fprintf(plik,"</head>\n");
 fprintf(plik,"<body>\n");
 fprintf(plik,"<center><big>Character %s %s  XP %d Gender: %s</big></center>\n",post->dane[IMIE],post->dane[NAZWISKO],post->PD, post->dane[PLEC]);
 fprintf(plik,"</body>\n");
 Alternative(post,plik);
 fprintf(plik,"<pre>\n\nUsing Data Base %s</pre>\n",fn);
 fprintf(plik,"</html>\n");
 fprintf(LOG,"HTML file saved\n");
}
void AboutHTML(RPG* post, FILE* plik, char* fn,char* db)
{
 fprintf(LOG,"Running AboutHTML(RPG*,FILE*char*,char*)\n");
 fprintf(LOG,"RPG offset is 0x%x\n",post);
 fprintf(LOG,"FILE offset is 0x%x\n",plik);
 fprintf(LOG,"fn is %s\n",fn);
 fprintf(LOG,"db is %s\n",db);
 char fn2[80];
 strcpy(fn2,fn);
 strcat(fn2,".html");
 cout<<"I will write HTML in "<<fn2<<endl;
 fprintf(LOG,"File name morphed %s\nGonna save\n",fn2);
 if ((plik = fopen(fn2,"w+"))==NULL)
        {
         fprintf(LOG,"LD: error, unable to write to %s\n",fn2);
         cout<<"Fatal reflection.\tCannot write to "<<fn2<<endl;
        }
 else
        {
         WriteHTML(post,plik,db);
         fprintf(LOG,"File %s closed\n",fn2);
         fclose(plik);
        }
}
void Call(RPG* post)
{
 fprintf(LOG,"Running Call(RPG*) offset 0x%x\n",post);
 printf("%s %s %s %s\n",post->dane[IMIE],post->dane[NAZWISKO],post->dane[RASA],post->dane[PROFESJA]);
}
void ShowAttChar(RPG* post)
{
 fprintf(LOG,"Running ShowAttrChar(RPG*) offset is 0x%x\n",post);
 Call(post);
 cout<<"\n\nNumber of attributes: "<<post->ilosc_cech<<endl;
 for (int i=0;i<post->ilosc_cech;i++)
        {
         printf("(%-3d)%-25s\t%d\n",i,post->n_cechy[i],post->cecha[i]);
        }
}
void CheckPassChar(RPG* db)
{
 fprintf(LOG,"Running CheckPassChar(RPG*) offset 0x%x\n",db);
 int a;
 if (db->sv == -1) {cout<<"Access Denied!\n";fprintf(LOG,"LD: error, constructor of this character didn/t give any permissions\n");takeint(&a);fclose(LOG);
 #if OSTYPE == LINUX
 last_op(NULL,db);
 #endif
 exit( -1 );}
 if (db->sv != 0)
        {
         fprintf(LOG,"Password exists\n");
         cout<<"Enter current character password: ";
         takeint(&a);
         if (a != db->sv)
                {
                 fprintf(LOG,"LD: Error User typed wrong password\nApplication halted!\n");
                 cout<<"Access denied!\n";
                 takeint(&a);
                 fclose(LOG);
                 #if OSTYPE == LINUX
                 last_op(NULL,db);
                 #endif
                 exit( -1 );
                }
         else {cout<<"OK\n";fprintf(LOG,"Password doesn't exists; do you really want toleft  character not protected?\n");}
        }
 cout<<"Change character password? ";
 char zn;takechar(&zn);
 if (zn == 'y') {cout<<"Type new digital password or -1 for Access Denied: ";fprintf(LOG,"New password set, but I won't show it :)\n");takeint(&db->sv);}
}
void MiniShow(RPG* p, int i)
{
 fprintf(LOG,"Running MiniShow(RPG*,int) offset is 0x%x\n\t parameter is %d , choosing correct function for detected OS\n",p,i);
 #if OSTYPE == LINUX
 printf("%-3d.  (%-2d)%-25s %-2d %-3d Attrib:(%-3d)%-25s Multiple: %-3d\n",i,p->trum[i],p->um[i],p->pozum[i],p->tesum[i],p->ceum[i],p->n_cechy[p->ceum[i]],p->mulum[i]);
 #elif OSTYPE == WINDOWS
  printf("%-3d.(%-2d)%-18s %-2d %-3d |A:%-25s Ml: %-3d\n",i,p->trum[i],p->um[i],p->pozum[i],p->tesum[i],p->n_cechy[p->ceum[i]],p->mulum[i]);
 #endif
}
void ShowSkill(RPG* post)
{
 fprintf(LOG,"Running ShowSkill(RPG*) offset 0x%x\n",post);
 ln();
 for (int i=0;i<post->il_um;i++)
        {
         MiniShow(post,i);
        }
 ln();
}
void AddAtt(RPG* post, int* pd)
{
 fprintf(LOG,"Running AddAtt(RPG*,int*) offset of RPG is 0x%x\n",post);
 fprintf(LOG,"Pointer to XP offset 0x%x value %d\n",pd,*pd);
 ShowAttChar(post);
 ln();
 cout<<"Number of attribute to add plus : ";
 int num;takeint(&num);
 if ((num<0) || (num>post->ilosc_cech-1)) return;
 cout<<"Attribute "<<post->n_cechy[num]<<" currently "<<post->p_c[num]<<" pluses\n";
 cout<<"To develop needed 13\n";
 int left = 13 - post->p_c[num];
 int plusy;
 cout<<"how much pluses? ";
 takeint(&plusy);
 if (plusy<1) return;
 if (plusy>*pd) return;
 if (plusy>left){cout<<"To many plusses\n";return;}
 post->PD +=plusy;
 *pd = *pd - plusy;
 post->p_c[num] += plusy;
 if (post->p_c[num] >= 13)
        {
         fprintf(LOG,"Attribute improved\n");
         cout<<"Attribute "<<post->n_cechy[num]<<" improved\n";
         post->p_c[num] = 0;
         post->cecha[num]++;
        }
  fprintf(LOG,"XP changet to %d\n",*pd);
}
void AddToSkill(RPG* post, DATA* db, int* pd)
{
 fprintf(LOG,"Running AddToSkill(RPG*,DATA*,int*)\n");
 fprintf(LOG,"RPG offset is 0x%x\n",post);
 fprintf(LOG,"DATA offset is 0x%x\n",db);
 fprintf(LOG,"Pointer to XP, offset is 0x%x value %d\n",pd,*pd);
 ShowSkill(post);
 ln();
 cout<<"Skill number for plus: ";
 int num;takeint(&num);
 if ((num<0) || (num>post->il_um-1)) return;
 int dat = post->trum[num] + post->pozum[num];
 int req = db->pd_cost[dat];
 int left = req - post->p_u[num];
 cout<<"To improve this skill needed "<<req<<" pluses.\nCurrentle has "<<post->p_u[num]<<" pluses\n";
 int plusy;
 cout<<"How much pluses? ";
 takeint(&plusy);
 if (plusy<1) return;
 if (plusy>*pd) return;
 if (plusy>left) {cout<<"Too many\n";return;}
 post->PD += plusy;
 *pd = *pd - plusy;
 post->p_u[num] += plusy;
 if (post->p_u[num] >= req)
        {
         cout<<"Skill "<<post->um[num]<<" improved\n";
         fprintf(LOG,"Skill improved\n");
         post->p_u[num] = 0;
         post->pozum[num]++;
         post->tesum[num] += post->mulum[num];
        }
 fprintf(LOG,"New value for XP is %d\n",*pd);
}
void NewSkill(RPG* post, DATA* db, int* pd, int find = 0)
{
 fprintf(LOG,"Running NewSkill(RPG*,DATA*,int*)\n");
 fprintf(LOG,"RPG offset is 0x%x\n",post);
 fprintf(LOG,"DATA offset is 0x%x\n",db);
 fprintf(LOG,"Pointer to XP, offset is 0x%x value %d\n",pd,*pd);
 if (post->il_um == MAX_UM) {cout<<"You have already maximum skills  "<<MAX_UM<<endl;return;}
 if (find) FindSkill(db,TRUE);
 else ShowSkill(db);
 ln();
 cout<<"Type difficulty of new skill: ";
 int trd;takeint(&trd);
 int cost = db->pd_cost[trd];
 if (*pd<cost) {cout<<"You have not enough pluses for such difficult skill, needed "<<cost<<" pluses.\n";return;}
 cout<<"Type skill number or -1 own: ";
 int num;takeint(&num);
 if ((num<-1) || (num>db->ilosc[3]-1)) return;
 if (num != -1) if (db->s_mod[num][DIFF] != trd) {cout<<"This skill isn't difficulty level "<<trd<<endl;return;}
 post->PD += cost;
 * pd = *pd - cost;
 int current = post->il_um;
 if (num != -1) strcpy(post->um[current],db->s_name[num]);
 else
        {
         cout<<"Skill name: ";
         takestring(post->um[current]);
        }
 post->pozum[current] = 1;
 post->trum[current] = trd;
 if (num != -1)post->ceum[current] = db->s_mod[num][ATTRIBUTE];
 else
        {
         ShowAttChar(post);
         ln();
         int n;
         cout<<"Number of testing attribute: ";
         takeint(&n);
         if (n<0) n = 0;
         if (n>post->ilosc_cech-1) n = post->ilosc_cech-1;
         post->ceum[current] = n;
        }
 if (num != -1) post->mulum[current] = db->s_mod[num][MULTIPLE];
 else
        {
         cout<<"Skill multiplier: ";
         takeint(&post->mulum[current]);
        }
 if (num != -1) post->magum[current] = db->s_mod[num][MAGIC];
  else
        {
         cout<<"Magic level: ";
         takeint(&post->magum[current]);
        }
 int mod = 0;
 if (num != -1)mod = db->s_mod[num][MOD];
 else
        {
         cout<<"Test modifier: ";
         takeint(&mod);
        }
 post->tesum[current] = post->cecha[post->ceum[current]]/2 + post->mulum[current]+ mod;
 fprintf(LOG,"New skill taken\nnew value for XP is %d\n",*pd);
 post->il_um++;
}
void ShowEkw(RPG* post)
{
 fprintf(LOG,"Running ShowEkw(RPG*) offset is 0x%x\n",post);
 cout<<"\nNumber of equip "<<post->il_ekw<<endl;
 for (int i=0;i<post->il_ekw;i++)
        {
         #if OSTYPE == LINUX
         printf("(%-3d)%-80s\n",i,post->ekw[i]);
         #elif OSTYPE == WINDOWS
         printf("(%-2d)%-60s\n",i,post->ekw[i]);
         #endif
        }
 ln();
}
void DelEkw(RPG* post)
{
 fprintf(LOG,"Running DelEkw(RPG*) offset is 0x%x\n",post);
 if (post->il_ekw == 0) {cout<<"There is nothing to erase\n";fprintf(LOG,"LD: warning, nothing to delete\n");return;}
 ShowEkw(post);
 ln();
 cout<<"Number of equip to erase: ";
 int num;takeint(&num);
 if ((num<0) || (num>post->il_ekw-1)) return;
 post->il_ekw--;
 for (int i=num;i<post->il_ekw;i++)
        {
         strcpy(post->ekw[i],post->ekw[i+1]);
        }
}
void AddEkw(RPG* post)
{
 fprintf(LOG,"Running AddEkw(RPG*) offset is 0x%x\n",post);
 if (post->il_ekw == MAX_EKW) {cout<<"You can't carry any more\n";return;}
 cout<<"Name of "<<post->il_ekw<<" equip: ";
 takestring(post->ekw[post->il_ekw]);
 post->il_ekw ++;
}
void EditEkw(RPG* post)
{
 fprintf(LOG,"Running EditEkw(RPG*) offset is 0x%x\n",post);
 ShowEkw(post);
 char zn ='l';
 while (zn != 'k')
 {
 cout<<"a\t-Add equip\n";
 cout<<"b\t-Delete equip\n";
 cout<<"c\t-Delete all\n";
 cout<<"d\t-See equip\n";
 cout<<"k\t-End\n";
 cout<<"z\t-Kill program\n";
 cout<<"\nChoose? ";
 takechar(&zn);
 switch(zn)
        {
         case 'a': DelEkw(post);break;
         case 'b': AddEkw(post);break;
         case 'c': post->il_ekw = 0;break;
         case 'd': ShowEkw(post);break;
         case 'z': fclose(LOG);
         #if OSTYPE == LINUX
         last_op(NULL,post);
         #endif
         exit( -1 );break;
         default : cout<<"Unknown command\n";
        }

 }
}
void Jump(int* pd)
{
 fprintf(LOG,"Running Jump(int*)\n");
 fprintf(LOG,"Pointer to XP offset is 0x%x value is %d\n",pd,*pd);
 cout<<"XP NUMBER: ";
 int il;takeint(&il);
 *pd = *pd + il;
 fprintf(LOG,"New value for XP is %d\n",*pd);
}
void DealPD(RPG* post, DATA* db, int* pd)
{
 fprintf(LOG,"Running DealPD(RPG*,DATA*,int*)\n");
 fprintf(LOG,"RPG offset is 0x%x\n",post);
 fprintf(LOG,"DATA offset is 0x%x\n",db);
 fprintf(LOG,"Pointer to XP offset 0x%x value %d\n",pd,*pd);
 cout<<"XP\t"<<*pd<<endl<<endl;
 cout<<"a\t-Add plus to attribute\n";
 cout<<"b\t-Add plus to skill\n";
 cout<<"c\t-See character\n";
 cout<<"d\t-Take new skill\n";
 cout<<"e\t-Edit equipment\n";
 cout<<"v\t-Znajdz umiejetnosc\n";
 cout<<"k\t-End\n";
 cout<<"z\t-Kill program\n";
 cout<<"!\t-Kill current character\n";
 cout<<"\nChoose? ";
 char zn;takechar(&zn);
 switch(zn)
        {
         case 'k': *pd = 0;break;
         case 'a': AddAtt(post,pd);break;
         case 'b': AddToSkill(post,db,pd);break;
         case 'c': ShowAll(post);break;
         case 'd': NewSkill(post,db,pd);break;
         case 'e': EditEkw(post);break;
         case 'v': NewSkill(post,db,pd,1);break;
         case 'x': Jump(pd);break;
         case '!': post->IsDead = 1;break;
         case 'z': fclose(LOG);
         #if OSTYPE == LINUX
         last_op(db,post);
         #endif
         exit( -1 );break;
         default: cout<<"Unknown command\n";
        }
}
void AddPD(RPG* post, DATA* db)
{
 fprintf(LOG,"Running AddPD(RPG*,DATA*)\n");
 fprintf(LOG,"RPG offset is 0x%x\n",post);
 fprintf(LOG,"DATA offset is 0x%x\n",db);
 cout<<"MG type number of XP for "<<post->dane[IMIE]<<": ";
 int pd;
 takeint(&pd);
 if (pd<0) pd *= -1;
 while (pd>0)
        {
         DealPD(post,db,&pd);
        }
 RealMM(post);
}
void AboutSave(RPG* post, FILE* plik, char* fn,char* db)
{
 fprintf(LOG,"Running AboutSave(RPG*,FILE*,char*,char*)\n");
 fprintf(LOG,"RPG offset is 0x%x\n",post);
 fprintf(LOG,"FILE offset is 0x%x\n",plik);
 fprintf(LOG,"fn is %s offset 0x%x\n",fn,fn);
 fprintf(LOG,"db is %s offset 0x%x\n",fn,fn);
 cout<<"Save changes to "<<fn<<": ";
 char zn;takechar(&zn);
 ShowAll(post);
 if (zn != 'y') return;
 if ((plik=fopen(fn,"w+"))==NULL)
        {
         fprintf(LOG,"LD: Error unable to write to %s\n",fn);
         cout<<"Fatal Reflection:\tCannot write to "<<fn<<endl;
        }
 else
        {
         SaveChar(post,plik,fn);
         fprintf(LOG,"Saved file\n",fn);
        }
 fprintf(LOG,"Now HTML session\n");
 AboutHTML(post,plik,fn,db);
}
void Experience(DATA* db, RPG* post, FILE* plik, char* fn,char* db_n)
{
 fprintf(LOG,"Running Experience(DATA*,RPG*,FILE*,char*,char*)\n");
 fprintf(LOG,"DATA offset is 0x%x\n",db);
 fprintf(LOG,"RPG offset is 0x%x\n",post);
 fprintf(LOG,"FILE offset is 0x%x\n",plik);
 fprintf(LOG,"fn is %s offset 0x%x\n",fn,fn);
 fprintf(LOG,"db_n is %s offset 0x%x\n",db_n,db_n);
 LoadChar(post,plik);
 fclose(plik);
 CheckPassChar(post);
 RealMM(post);
 ShowAll(post);
 AddPD(post,db);
 AboutSave(post,plik,fn,db_n);
}
void Woman(RPG* post, DATA* db)
{
 fprintf(LOG,"Running Woman(RPG*,DATA*)\n");
 fprintf(LOG,"RPG offset is 0x%x\n",post);
 fprintf(LOG,"DATA offset is 0x%x\n",db);
 strcpy(post->dane[PLEC],"kobieta");
 for (int i=0;i<post->ilosc_cech;i++) post->cecha[i] += db->p_mod[i];
}
void MainData(RPG* post, DATA* db)
{
 post->PD = 0;
 fprintf(LOG,"Running MainData(RPG*,DATA*)\n");
 fprintf(LOG,"RPG offset is 0x%x\n",post);
 fprintf(LOG,"DATA offset is 0x%x\n",db);
 cout<<"\n\nINSTEAD SPACE KEY ALWAYS USE _\n\n\n";
 cout<<"Name: ";          takestring(post->dane[IMIE]);
 cout<<"Surname: ";      takestring(post->dane[NAZWISKO]);
 cout<<"Character: ";takestring(post->dane[CHARAKTER]);
 cout<<"Eyes color: ";    takestring(post->dane[OCZY]);
 cout<<"Hair color: ";  takestring(post->dane[WLOSY]);
 cout<<"Skin: ";          takestring(post->dane[CERA]);
 cout<<"Height: ";        takestring(post->dane[WZROST]);
 cout<<"Weight: ";          takestring(post->dane[WAGA]);
 char zn = 'a';
 while ((zn != 'm') && (zn != 'k'))
 {
 cout<<"Your Gender (m*)(k*): ";
 takechar(&zn);
 }
 if (zn == 'w') Woman(post,db);
 else strcpy(post->dane[PLEC],"mezczyzna");
}
void ChooseRace(RPG* post,DATA* db)
{
 fprintf(LOG,"Running ChooseRace(RPG*,DATA*)\n");
 fprintf(LOG,"RPG offset is 0x%x\n",post);
 fprintf(LOG,"DATA offset is 0x%x\n",db);
 strcpy(post->dane[RASA],".");
 strcpy(post->dane[PROFESJA],".");
 Call(post);
 ShowTrial(db,0);ln();
 cout<<"Race number or -1 for create own: ";
 int num;takeint(&num);
 if (num == -1)
        {
         cout<<"race name: ";
         takestring(post->dane[RASA]);
         int mod;
         for (int i=0;i<post->ilosc_cech;i++)
              {
               cout<<"Modifier of "<<post->n_cechy[i]<<": ";
               takeint(&mod);
               post->cecha[i] += mod;
              }
        }
 if ((num<0) || (num>db->ilosc[1])) {cout<<"Out of range or own\n";return;}
 strcpy(post->dane[RASA],db->name[0][num]);
 for (int i=0;i<post->ilosc_cech;i++)
        {
         post->cecha[i] += db->mod[0][num][i];
        }
 ShowAttChar(post);
}
void ChooseProf(RPG* post,DATA* db)
{
 fprintf(LOG,"Running ChooseProf(RPG*,DATA*)\n");
 fprintf(LOG,"RPG offset is 0x%x\n",post);
 fprintf(LOG,"DATA offset is 0x%x\n",db);
 Call(post);
 ShowTrial(db,1);ln();
 cout<<"Profession number or -1 own: ";
 int num;takeint(&num);
 if (num == -1)
        {
         cout<<"Profession name: ";
         takestring(post->dane[PROFESJA]);
         int mod;
         for (int i=0;i<post->ilosc_cech;i++)
              {
               cout<<"Modifier of "<<post->n_cechy[i]<<": ";
               takeint(&mod);
               post->cecha[i] += mod;
              }
        }
 if ((num<0) || (num>db->ilosc[2])) {cout<<"Out of range or own\n";return;}
 strcpy(post->dane[PROFESJA],db->name[1][num]);
 for (int i=0;i<post->ilosc_cech;i++)
        {
         post->cecha[i] += db->mod[1][num][i];
        }
 ShowAttChar(post);
}
void DefaultValues(RPG* post, DATA* db)
{
 fprintf(LOG,"Running DefaultValues(RPG*,DATA*)\n");
 fprintf(LOG,"RPG offset is 0x%x\n",post);
 fprintf(LOG,"DATA offset is 0x%x\n",db);
 post->ilosc_cech = db->ilosc[0];
 post->st = db->st;
 for (int i=0;i<db->ilosc[0];i++)
        {
         post->cecha[i] = db->value_att[i];
         strcpy(post->n_cechy[i],db->name_att[i]);
        }
 post->il_um  = 0;
 post->il_ekw = 0;
 post->numMM = db->mm;
 fprintf(LOG,"Data DATA base taken to RPG\n");
}
void RandomAtt(RPG* post,DATA* db)
{
 fprintf(LOG,"Running RandomAtt(RPG*,DATA*)\n");
 fprintf(LOG,"RPG offset is 0x%x\n",post);
 fprintf(LOG,"DATA offset is 0x%x\n",db);
 #if OSTYPE == LINUX
 int var = 0;
 #endif
 int pl = 0;
 int a = 0;
 if (a == -1) pl += 3*post->st;
 #if OSTYPE == LINUX
 fprintf(LOG,"Randomizing for OS Linux\n",ostype);
 for (int i=0;i<var;i++) a = random(10);
 #endif
 ln();
 #if OSTYPE == LINUX
 #define WYM 20
 #elif OSTYPE == WINDOWS
 #define WYM 10
 #endif
 int temp[MAX_ATT][WYM];
 cout<<"(s)mall or (b)ig random? ";
 char zn;takechar(&zn);
 if (zn == 's') pl += post->st;
 if (zn == 'b') pl += 2*post->st;
 else pl = 0;
 for (int i=0;i<post->ilosc_cech;i++)
        {
         for (int j=0;j<WYM;j++)
                {
                 temp[i][j] = post->cecha[i] + random(2*pl+1)-pl;
                }
        }
 cout<<"I've randomized:\n";
 #if OSTYPE == LINUX
 printf("%-25s ","Attribute, Random number>>");
 #elif OSTYPE == WINDOWS
 fprintf(LOG,"Randomizing for OS Windows NT\n",ostype);
 printf("%-20s ","Attribute, Random number>>");
 #endif
 for (int j=0;j<WYM;j++)
        {
         printf("%-3d ",j);
        }
 lns(2);
 for (int i=0;i<post->ilosc_cech;i++)
        {
          #if OSTYPE == LINUX
          printf("%-25s ",post->n_cechy[i]);
          #elif OSTYPE == WINDOWS
          printf("%-20s ",post->n_cechy[i]);
          #endif
         for (int j=0;j<WYM;j++)
                {
                 printf("%-3d ",temp[i][j]);
                }
         ln();
        }
 cout<<"Sumes of shown:\n";
 int suma;
 for (int i=0;i<WYM;i++)
        {
         suma = 0;
         for (int j=0;j<post->ilosc_cech;j++) suma += temp[j][i];
         printf("%d-%d, ",i,suma);
        }
 ln();
 cout<<"Option number: ";
 takeint(&suma);
 if ((suma<0) || (suma>WYM)) return;
 for (int i=0;i<post->ilosc_cech;i++)
        {
         post->cecha[i] = temp[i][suma];
        }
 ln();
 ShowAttChar(post);
}
void OwnAtt(RPG* post)
{
 fprintf(LOG,"Running OwnAtt(RPG*)\n");
 fprintf(LOG,"RPG offset is 0x%x\n",post);
 cout<<"How much pluses <0 - 3>: ";
 int pl;takeint(&pl);
 if ((pl<1) || (pl>3)) return;
 int temp = pl;
 int num;
 int ile;
 int prev = -1;
 while (pl>0)
        {
         ShowAttChar(post);
         back:
         cout<<"Number of attribute to improve: ";
         takeint(&num);
         if (num == prev) {cout<<"You have already modified this attribute!\n";goto back;}
         if (num<0) num = 0;
         if (num>post->ilosc_cech-1) num = post->ilosc_cech-1;
         prev = num;
         cout<<"How much pluses to "<<post->n_cechy[num]<<": ";
         takeint(&ile);
         if (ile<0) ile = 0;
         if (ile>2) ile = 2;
         if (ile>pl) ile = pl;
         post->cecha[num] += ile*post->st;
         pl -= ile;
        }
 pl = temp;
 prev = -1;
 while (pl>0)
        {
         ShowAttChar(post);
         back2:
         cout<<"Number of attribute to deprove: ";
         takeint(&num);
         if (num == prev) {cout<<"You have already modified this attribute!\n";goto back2;}
         if (num<0) num = 0;
         if (num>post->ilosc_cech-1) num = post->ilosc_cech-1;
         prev = num;
         cout<<"How much minuses to "<<post->n_cechy[num]<<": ";
         takeint(&ile);
         if (ile<0) ile = 0;
         if (ile>2) ile = 2;
         if (ile>pl) ile = pl;
         post->cecha[num] -= ile*post->st;
         pl -= ile;
        }
}
void AddSkill(RPG* post, DATA* db, int* lum, short modify = 0)
{
 fprintf(LOG,"Running AddSkill(RPG*,DATA*,int*)\n");
 fprintf(LOG,"RPG offset is 0x%x\n",post);
 fprintf(LOG,"DATA offset is 0x%x\n",db);
 fprintf(LOG,"SP offset is 0x%x value %d\n",lum,*lum);
 fprintf(LOG,"modify is %d\n",modify);
 if ((post->il_um == MAX_UM) && (modify == 0)) {fprintf(LOG,"LD: warning already maximum skills\n");cout<<"You have already "<<MAX_UM<<" skills.\n";return;}
 int current;
 int ret = 0;
 if (modify == TRUE)
        {
         ShowSkill(post);
         ln();
         cout<<"Type number of skill to edit: ";
         int num;
         takeint(&num);
         if (num<0) return;
         if (num>post->il_um-1) return;
         current = num;
         int l = post->pozum[num];
         int t = post->trum[num];
         ret = db->cost[l+t-1];
         cout<<"Previous level "<<l<<" Previous difficulty "<<t<<", points returned "<<ret<<endl;
        }
 else current = post->il_um;
 if (modify != 10) ShowSkill(db,0);
 else FindSkill(db);
 ln();
 cout<<"Type numer of skill or -1 own: ";
 int num;takeint(&num);
 if (num<-1) num = 0;
 if (num>db->ilosc[3]-1) num = db->ilosc[3]-1;
 if (num != -1)strcpy(post->um[current],db->s_name[num]);
 else
        {
         cout<<"Skill name: ";
         takestring(post->um[current]);
        }
 cout<<"Level of "<<post->um[current]<<" difficulty is "<<db->s_mod[num][DIFF]<<": ";
 int poz;takeint(&poz);
 int ipoz;   //int ipoz = 0;
 int tr = 0;
 int ce = 0;
 int mod = 0;
 if (num != -1) ipoz = poz + db->s_mod[num][DIFF] -1;
 else
        {
         cout<<"Difficulty: ";
         takeint(&tr);
         ipoz = poz + tr -1;
        }
 if (num == -1)
        {
         ShowAttChar(post);
         ln();
         cout<<"Number of testing attribute: ";
         takeint(&ce);
         if (ce<0) ce = 0;
         if (ce>post->ilosc_cech-1) ce = post->ilosc_cech-1;
        }
 cout<<"Total level "<<ipoz<<" cost "<<db->cost[ipoz]<<endl;
 int cost = db->cost[ipoz] - ret;
 if (*lum<cost) {fprintf(LOG,"Not enough points\n");cout<<"Not enough points, you have only "<<*lum<<endl;return;}
 *lum -= cost;
 post->pozum[current] = poz;
 if (num != -1) post->trum[current]  = db->s_mod[num][DIFF];else post->trum[current] = tr;
 if (num != -1) post->ceum[current]  = db->s_mod[num][ATTRIBUTE];else post->ceum[current] = ce;
 if (num != -1) post->mulum[current] = db->s_mod[num][MULTIPLE];
 else
        {
         cout<<"Multiplier of skill: ";
         takeint(&post->mulum[current]);
         cout<<"Test modifier: ";
         takeint(&mod);
        }
 if (num != -1) post->magum[current] = db->s_mod[num][MAGIC];
 else
        {
         cout<<"Magic level: ";
         takeint(&post->magum[current]);
        }
 if (num != -1) mod = db->s_mod[num][MOD];
 post->tesum[current] = (post->cecha[post->ceum[current]]/2) + (post->mulum[current]*post->pozum[current])+ mod;
 cout<<"Taking into accout all cost is: "<<cost<<endl;
 MiniShow(post,current);
 if (modify != TRUE) post->il_um++;
 fprintf(LOG,"New value for SP is %d\n",*lum);
}
void DelSkill(RPG* post,DATA* db,int* lum, short inside = FALSE)
{
 fprintf(LOG,"Running DelSkill(RPG*,DATA*,int*,short = 0)\n");
 fprintf(LOG,"RPG offset is 0x%x\n",post);
 fprintf(LOG,"DATA offset is 0x%x\n",db);
 fprintf(LOG,"SP offset is 0x%x value %d\n",lum,*lum);
 fprintf(LOG,"inside = %d\n",inside);
 if (post->il_um == 0) {fprintf(LOG,"LD: warning nothing to erase\n");cout<<"Nothing to delete\n";return;}
 int num,ret;
 if (inside == TRUE)
        {
         ShowSkill(post);
         ln();
         cout<<"Skill number to delete: ";
         takeint(&num);
         if (num<0) return;
         if (num>post->il_um-1) return;
         int l = post->pozum[num];
         int t = post->trum[num];
         ret = db->cost[l+t-1];
         cout<<"Erasing level "<<l<<" difficulty "<<t<<" returned points "<<ret<<endl;
         post->il_um --;
         for (int i=num;i<post->il_um;i++)
                {
                 strcpy(post->um[i],post->um[i+1]);
                 post->tesum[i] = post->tesum[i+1];
                 post->pozum[i] = post->pozum[i+1];
                 post->trum[i]  =  post->trum[i+1];
                 post->ceum[i]  =  post->ceum[i+1];
                 post->mulum[i] = post->mulum[i+1];
                }
         *lum += ret;
        }
 if (inside == FALSE)
        {
         post->il_um --;
         int l = post->pozum[post->il_um];
         int t = post->trum[post->il_um];
         ret = db->cost[l+t-1];
         cout<<"Erasing level "<<l<<" difficulty "<<t<<" points returned "<<ret<<endl;
         *lum += ret;
        }
 if  (inside == ALL)
        {
         int l;
         int t;
         ret   = 0;
         for (int i=0;i<post->il_um;i++)
                {
                 l = post->pozum[i];
                 t = post->trum[i];
                 ret += db->cost[l+t-1];
                 cout<<"For skill nuber  "<<i<<" you have "<<db->cost[l+t-1]<<" points returned.\n";
                }
         cout<<"\n\nTotally you have "<<ret<<" points returned.\n";
         *lum += ret;
         post->il_um = 0;
        }
fprintf(LOG,"New value for SP is %d\n",*lum);
}
void SeeLevCost(DATA* db)
{
 fprintf(LOG,"Running SeeLevCost(DATA*) offset is 0x%x\n",db);
 cout<<"Levels costs\n";
 cout<<"Available levels "<<db->ilosc[5]<<endl;
 for (int i=0;i<db->ilosc[5];i++)
        {
         printf("Level: %-3d\t\tCost: %-4d\n",i,db->cost[i]);
        }
}
void SkillOperator(RPG* post, DATA* db, int* lum, FILE* plik, char* fn)
{
 fprintf(LOG,"Running SkillOperator(RPG*,DATA*,int*,FILE*,char*)\n");
 fprintf(LOG,"RPG offset is 0x%x\n",post);
 fprintf(LOG,"DATA offset is 0x%x\n",db);
 fprintf(LOG,"SP offset is 0x%x value %d\n",lum,*lum);
 fprintf(LOG,"FILE offset is 0x%x\n",plik);
 fprintf(LOG,"fn is %s offset 0x%x\n",fn,fn);
 char zn;
 cout<<"\n\nPoints: "<<*lum<<"\nNumber of skills: "<<post->il_um<<"\n\n";
 cout<<"a\t-Add skill\n";
 cout<<"v\t-Find & add skill\n";
 cout<<"b\t-Modify skill\n";
 cout<<"c\t-See base skill\n";
 cout<<"d\t-See character's skills\n";
 cout<<"e\t-Delete last\n";
 cout<<"f\t-Delete choosen\n";
 cout<<"g\t-Kill'em all\n";
 cout<<"h\t-See levels cost\n";
 cout<<"s\t-Save character\n";
 cout<<"k\t-End\n";
 cout<<"z\t-Kill program\n";
 cout<<"\nChoose? ";
 takechar(&zn);
 switch(zn)
        {
         case 'a': AddSkill(post,db,lum);break;
         case 'v': AddSkill(post,db,lum,10);break;
         case 'b': AddSkill(post,db,lum,TRUE);break;
         case 'c': ShowSkill(db,3);break;
         case 'd': ShowSkill(post);break;
         case 'e': DelSkill(post,db,lum);break;
         case 'f': DelSkill(post,db,lum,TRUE);break;
         case 'g': DelSkill(post,db,lum,ALL);break;
         case 'x': *lum = *lum + 100;break;
         case 's': SaveChar(post,plik,fn);break;
         case 'h': SeeLevCost(db);break;
         case 'k': *lum = 0;break;
         case 'z': fclose(LOG);
         #if OSTYPE == LINUX
         last_op(db,post);
         #endif
         exit( -1 );break;
         default: cout<<"Unknown command.\n";
        }
  fprintf(LOG,"New value for SP is %d\n",*lum);
}
void SetSkill(RPG* post,DATA* db, FILE* plik, char* fn)
{
  //il_um um tesum pozum trum ceum mulum
  fprintf(LOG,"Running SetSkill(RPG*,DATA*,FILE*,char*)\n");
  fprintf(LOG,"RPG offset is 0x%x\n",post);
  fprintf(LOG,"DATA offset is 0x%x\n",db);
  fprintf(LOG,"FILE offset is 0x%x\n",plik);
  fprintf(LOG,"fn is %s offset 0x%x\n",fn,fn);
 int lum;
 cout<<"\nMaximum skills is: "<<MAX_UM<<endl;
 cout<<"Enter number of skill points, default 44: ";
 takeint(&lum);
 while (lum>0)
        {
         SkillOperator(post,db,&lum,plik,fn);
        }
}
void SetEkw(RPG* post)
{
 fprintf(LOG,"Running SetEkw(RPG*) offset is 0x%x\n",post);
 cout<<"Maximum equip: "<<MAX_EKW<<endl;
 cout<<"Equipment, type k-ENTER for end.\n";
 char data[STRING];
 strcpy(data,"x");
 int i = 0;
 while ((strcmp(data,"k") != 0) && (post->il_ekw<MAX_EKW))
        {
         printf("(%-3d) ",i);
         takestring(data);
         if (strcmp(data,"k") != 0)
                {
                 strcpy(post->ekw[i],data);
                 i++;
                 post->il_ekw++;
                }
         else cout<<"\n\nEnd\n";
        }
}
void ProtectorChar(RPG* post)
{
 fprintf(LOG,"Running ProtectorChar(RPG*) offset is 0x%x\n",post);
 cout<<"Should character be protected by password? ";
 char zn;
 takechar(&zn);
 if (zn != 'y') return;
 cout<<"Type character password: ";
 takeint(&post->sv);
}
void CreateChar(DATA* db, RPG* post, FILE* plik, char* fn,char* db_n)
{
 fprintf(LOG,"Running CreateChar(DATA*,RPG*,FILE*,char*,char*)\n");
 fprintf(LOG,"DATA offset is 0x%x\n",db);
 fprintf(LOG,"RPG offset is 0x%x\n",post);
 fprintf(LOG,"FILE offset is 0x%x\n",plik);
 fprintf(LOG,"fn is %s offset 0x%x\n",fn,fn);
 fprintf(LOG,"db_n is %s offset 0x%x\n",db_n,db_n);
 cout<<"If previous run program was unsuccesful\n";
 cout<<"You can restore not ended character, if save is\n";
 cout<<"Correct, program will continue from last crash point\n";
 cout<<"Use restore character? ";
 char pyt;takechar(&pyt);
 if (pyt != 'n')
        {
         fprintf(LOG,"Trying to restore character\n");
         cout<<"Restored runlevel is "<<post->rl<<endl;
         fprintf(LOG,"LD: Loaded RL is %d\n",post->rl);
         switch(post->rl)
                {
                 case 1: goto l1;
                 case 2: goto l2;
                 case 3: goto l3;
                 case 4: goto l4;
                 case 5: goto l5;
                 case 6: goto l6;
                 default: {cout<<"Bad data in file!\n";cout<<"Restored runlevel is 0 or no character read\n";takechar(&pyt);}
                }
        }
 DefaultValues(post,db);
 fclose(plik);
 MainData(post,db);
 post->rl = 1;
 SaveChar(post,plik,fn);
 l1:
 ChooseRace(post,db);
 post->rl = 2;
 SaveChar(post,plik,fn);
 l2:
 ChooseProf(post,db);
 post->rl = 3;
 SaveChar(post,plik,fn);
 l3:
 RandomAtt(post,db);
 post->rl = 4;
 SaveChar(post,plik,fn);
 l4:
 OwnAtt(post);
 post->rl = 5;
 SaveChar(post,plik,fn);
 l5:
 SetSkill(post,db,plik,fn);
 post->rl = 6;
 post->stMM = post->cecha[post->numMM];
 SaveChar(post,plik,fn);
 l6:
 RealMM(post);
 SetEkw(post);
 SaveChar(post,plik,fn);
 AboutHTML(post,plik,fn,db_n);
 ProtectorChar(post);
 ShowAll(post);
 fprintf(LOG,"All data created finish CreateChar(...)\n");
}

void RolePlayConstructor(DATA* db, RPG* post, char* fn,char* base_name)
{
 fprintf(LOG,"Running RolePlayConstructor(DATA*,RPG*,char*,char*)\n");
 fprintf(LOG,"DATA offset is 0x%x\n",db);
 fprintf(LOG,"RPG offset is 0x%x\n",post);
 fprintf(LOG,"fn is %s offset 0x%x\n",fn,fn);
 fprintf(LOG,"base_name is %s offset 0x%x\n",base_name,base_name);
 FILE* plik;
 fprintf(LOG,"Trying to load from file\n");
 if ((plik = fopen(fn,"r+")) != NULL)
        {
         LoadChar(post,plik);
         fprintf(LOG,"Loaded decide what to to\n");
         cout<<"If this is backup save, answering yes may restore it\n";
         fprintf(LOG,"About overwrite\n");
         cout<<"File "<<fn<<" exists, restore/overwrite? ";
         char zn;takechar(&zn);
         if (zn != 'y') {fprintf(LOG,"User: no\n");Experience(db,post,plik,fn,base_name);return;}
         fprintf(LOG,"User: yes\n");
         CheckPassChar(post);
        }
 if ((plik = fopen(fn,"w+")) == NULL)
        {
         fprintf(LOG,"LD: Fatal Error (FE) File cannot be saved\n");
         cout<<"Fatal Reflection: Cannot write to "<<fn<<endl;
         return;
        }
 CreateChar(db,post,plik,fn,base_name);
 cout<<"You've created character "<<fn<<endl;
 fprintf(LOG,"Leaving RolePlayConstructor(...)\n");
}
void AddLine(char* base, char* chr, int log)
{
 fprintf(LOG,"Running AddLine(char*,char*,int)\n");
 fprintf(LOG,"base is %s offset 0x%x\n",base,base);
 fprintf(LOG,"character is %s offset 0x%x\n",chr,chr);
 fprintf(LOG,"log = %d\n",log);
 FILE* data;
 char* name ="DataBase/user_log";
 fprintf(LOG,"Define file DataBase/user_log\n");
 if ((data = fopen(name,"r+")) == NULL)
        {
         fprintf(LOG,"Not find... creating %s\n",name);
         cout<<"Creating file "<<name<<endl;
        }
 else {fclose(data);fprintf(LOG,"Exists, gonna add line\n");}
 if ((data = fopen(name,"a+")) == NULL)
        {
         fprintf(LOG,"LD: Cannot append line to file %s\n",name);
         cout<<"Fatal Reflection:\tCannot write to "<<name<<endl;
         cout<<"Continue without file...\n";
         fprintf(LOG,"Error left unfixed, but continue\n");
        }
 else
        {
         fprintf(data,"Log Number %-5x Uses Base %-25s Create Character %-25s\n",log,base,chr);
         fclose(data);
         fprintf(LOG,"Log saved to %s\n",name);
        }
}
void LoadAndConvertC(RPG* post, FILE* f)
{
 fprintf(LOG,"Running LoadAndConvertC(RPG*,FILE*)\n");
 fprintf(LOG,"RPG offset is 0x%x\n",post);
 fprintf(LOG,"FILE offset is 0x%x\n",f);
 fprintf(LOG,"\n");
 char useless[STRING];
 int useless_int;
 int check;
 check = fscanf(f,"HTML File: %s\n",useless);
 Protection(check,post);
 fscanf(f,"SAVE File: %s\n",useless);
 fscanf(f,"PD: %d\n",&post->PD);
 fscanf(f,"Ilosc Cech: %d\n",&post->ilosc_cech);
 fscanf(f,"Suma: %d\n",&useless_int);
 for (int i=0;i<post->ilosc_cech;i++)
	{
	 fscanf(f,"Nazwa %d: %s\n",&useless_int,post->n_cechy[i]);
	 fscanf(f,"Wartosc: %d\n",&post->cecha[i]);
	 fscanf(f,"Plusow: %d\n",&post->p_c[i]);
	}
 fscanf(f,"Ilosc Umiejetnosci: %d\n",&post->il_um);
 for (int i=0;i<post->il_um;i++)
	{
	 fscanf(f,"Nazwa %d: %s\n",&useless_int,post->um[i]);
	 fscanf(f,"Poziom: %d\n",&post->pozum[i]);
	 fscanf(f,"Test: %d\n",&post->tesum[i]);
	 fscanf(f,"Plusow: %d\n",&post->p_u[i]);
	}
 post->ilosc_danych = 12;
 fscanf(f,"Imie: %s\n",post->dane[IMIE]);
 fscanf(f,"Nazwisko: %s\n",post->dane[NAZWISKO]);
 fscanf(f,"Plec: %s\n",post->dane[PLEC]);
 fscanf(f,"Rasa: %s\n",post->dane[RASA]);
 fscanf(f,"Profesja: %s\n",post->dane[PROFESJA]);
 fscanf(f,"Average: %d\n",&useless_int);
 fscanf(f,"Start Value: %d\n",&useless_int);
 cout<<"Describe character: ";
 takestring(post->dane[CHARAKTER]);
 cout<<"Eyes color: ";
 takestring(post->dane[OCZY]);
 cout<<"Hair color: ";
 takestring(post->dane[WLOSY]);
 cout<<"Skin: ";
 takestring(post->dane[CERA]);
 cout<<"Height: ";
 takestring(post->dane[WZROST]);
 cout<<"Weight: ";
 takestring(post->dane[WAGA]);
 fscanf(f,"Ilosc Ekwipunku: %d\n",&post->il_ekw);
 for (int i=0;i<post->il_ekw;i++)
	{
	 fscanf(f,"Nazwa %d: %s\n",&useless_int,post->ekw[i]);
	}
 fscanf(f,"Postac stworzona przez aplikacje autorstwa Morgoth'a DBMA\n");
 fprintf(LOG,"One half read\n");
  for (int i=0;i<post->il_um;i++)
	{
	 cout<<"Difficulty of "<<post->um[i]<<": ";
         takeint(&post->trum[i]);
        }
  for (int i=0;i<post->il_um;i++)
	{
         cout<<"Multiplier of "<<post->um[i]<<": ";
         takeint(&post->mulum[i]);
        }
  for (int i=0;i<post->il_um;i++)
	{
         cout<<"Magic level of "<<post->um[i]<<": ";
         takeint(&post->magum[i]);
        }
  for (int i=0;i<post->il_um;i++)
	{
         ShowAttChar(post);
         ln();
         cout<<"Number of testing attribute: "<<post->um[i]<<": ";
         takeint(&post->ceum[i]);
         if (post->ceum[i]>post->ilosc_cech-1) post->ceum[i] = post->ilosc_cech-1;
         if (post->ceum[i]<0) post->ceum[i] = 0;
        }
 cout<<"OK\n";
 post->sv = 0;
 post->st = 1;
 post->rl = 0;
 cout<<post->ilosc_cech<<endl;
 ShowAttChar(post);
 fprintf(LOG,"Defined all readable data, adding new...\n");
 cout<<"Type number of attribute wchich is responsible for Magic Power: ";
 int mm;
 takeint(&mm);
 if (mm<0) mm = 0;
 if (mm>post->ilosc_cech-1) mm = post->ilosc_cech-1;
 post->numMM = mm;
 post->stMM = post->cecha[post->numMM];
 fprintf(LOG,"LD: Character may be converted...\n");
}
void LoadAndConvert(DATA* db, FILE* save)
{
 fprintf(LOG,"Running LoadAndConvert(DATA*,FILE*)\n");
 fprintf(LOG,"DATA offset is 0x%x\n",db);
 fprintf(LOG,"FILE offset is 0x%x\n",save);
 char useless[100];
 int useless_int;
 int check;
 cout<<"Reading data.\n";
 fscanf(save,"Base Name: %s\n",useless);
 check = fscanf(save,"Password: 0x%x\n",&useless_int);
 Protection(check,db,save);
 fscanf(save,"Ilosc Cech: %d\n",&db->ilosc[0]);
 fscanf(save,"Ilosc Ras: %d\n",&db->ilosc[1]);
 fscanf(save,"Ilosc Profesji: %d\n",&db->ilosc[2]);
 fscanf(save,"Ilosc Umiejetnosci: %d\n",&db->ilosc[3]);
 for (short i=0;i<db->ilosc[0];i++)
	{
	 fscanf(save,"Nazwa cechy %d: %s\n",&useless_int,db->name_att[i]);
	}
 for (short i=0;i<db->ilosc[1];i++)
	{
	 fscanf(save,"Nazwa Rasy %d: %s\n",&useless_int,db->name[0][i]);
	 for (short j=0;j<db->ilosc[0];j++)
		{
		 fscanf(save,"Mod cechy %d: %d\n",&useless_int,&db->mod[0][i][j]);
		}
	}
 for (int i=0;i<db->ilosc[2];i++)
	{
         fscanf(save,"Nazwa profesji %d: %s\n",&useless_int,db->name[1][i]);
	 for (int j=0;j<db->ilosc[0];j++)
		{
		 fscanf(save,"Mod cechy %d: %d\n",&useless_int,&db->mod[1][i][j]);
		}
	}
 for (int i=0;i<db->ilosc[3];i++)
	{
	 fscanf(save,"Nazwa Umiejetnosci %d: %s\n",&useless_int,db->s_name[i]);
	 fscanf(save,"Cecha test: %d\n",&db->s_mod[i][ATTRIBUTE]);
	 fscanf(save,"Na poz: %d\n",&db->s_mod[i][MULTIPLE]);
	 fscanf(save,"Mod test: %d\n",&db->s_mod[i][MOD]);
	}
fscanf(save,"Start Value: %d\n",&db->sv);
fscanf(save,"Plus: %d\n",&db->st);
fscanf(save,"Small Random: %d\n",&useless_int);
fscanf(save,"Large Random: %d\n",&useless_int);
fscanf(save,"Ilosc Kategorii: %d\n",&db->ilosc[4]);
for (int i=0;i<db->ilosc[4];i++)
	{
	 fscanf(save,"Nazwa %d kategorii: %s\n",&useless_int,db->c_name[i]);
	}
for (int i=0;i<db->ilosc[3];i++)
	{
	 fscanf(save," Umiejetnosc %d Kategoria: %d\n",&useless_int,&db->s_mod[i][CATEG]);
         db->s_mod[i][DIFF] = 1;
	}
fscanf(save,"RAM SEGMENT ADDRESS %x\n",&useless_int);
fscanf(save,"Ilosc Poziomow Postaci: %d\n",&db->ilosc[5]);
for (int i=0;i<db->ilosc[5];i++)
	{
	 fscanf(save,"Poziom %d punktow: %d\n",&useless_int,&useless_int);
	}
fscanf(save,"Ilosc Poziomow Umiejetnosci: %d\n",&db->ilosc[5]);
for (int i=0;i<db->ilosc[5];i++)
	{
	 fscanf(save,"Poziom %d koszt: %d\n",&useless_int,&db->cost[i]);
	}
fscanf(save,"Ilosc Level Up: %d\n",&db->ilosc[6]);
for (int i=0;i<db->ilosc[6];i++)
	{
	 fscanf(save,"Poziom %d koszt plusow: %d\n",&i,&db->pd_cost[i]);
	}
fscanf(save,"This Data Base Has been Created by Morgoth DBMA Program.\n");
fprintf(LOG,"Half of data read\n");
fscanf(save,"Modyfikatory dla kobiet:\n");
 for (int i=0;i<db->ilosc[0];i++)
	{
         fscanf(save,"Cecha %d mod %d\n",&useless_int,&db->p_mod[i]);
        }
for (int i=0;i<db->ilosc[0];i++)
        {
         db->value_att[i] = db->sv;
        }
 cout<<"Done\n";
 db->sv = 0;
 fprintf(LOG,"LD: Base loaded, maybe is correct...\n");
}
void ConversionC(char* name)
{
 fprintf(LOG,"Running ConversionC(char*) parameter %s offset 0x%x\n",name,name);
 char dat[80];
 strcpy(dat,"Characters/");
 strcat(dat,name);
 fprintf(LOG,"Folders prepared\n");
 cout<<"I will try to convert "<<dat<<".\n";
 RPG* post = new RPG;
 FILE* plik;
 if ((plik = fopen(dat,"r+")) == NULL)
        {
         fprintf(LOG,"LD: Fatal Error (FE) File %s deosn't exists\n",dat);
         cout<<"Fatal Reflection:\tNo file "<<dat<<".\n";
         delete post;
         return;
        }
 LoadAndConvertC(post,plik);
 fclose(plik);
 char dat2[80];
 fprintf(LOG,"New name for converted\n");
 cout<<"Type new file name for "<<dat<<": ";
 takestring(dat2);
 strcpy(dat,"Characters/");
 strcat(dat,dat2);
 cout<<"New name: "<<dat<<endl;
 fprintf(LOG,"Is %s\n",dat);
 if ((plik = fopen(dat,"r+")) != NULL)
        {
         char zn;
         fprintf(LOG,"LD: Such file exists\n");
         cout<<"File "<<dat<<" exists, overwrite? ";
         takechar(&zn);
         if (zn != 'y') {fprintf(LOG,"Halted...\n");fclose(plik);delete post;return;}
         fclose(plik);
         fprintf(LOG,"Will be overwritten\n");
        }
 SaveChar(post,plik,dat);
 delete post;
}
void Conversion(char* name)
{
 fprintf(LOG,"unning Conversion(char*) parameter %s offset 0x%x\n",name,name);
 char dat[80];
 strcpy(dat,"DataBase/");
 strcat(dat,name);
 fprintf(LOG,"Folders prepared\n");
 cout<<"I will try to convert file "<<dat<<".\n";
 DATA* db = new DATA;
 FILE* plik;
 if ((plik = fopen(dat,"r+")) == NULL)
        {
         fprintf(LOG,"LD: Fatal Error (FE) File %s cannot be read\n",dat);
         cout<<"Fatal reflection: No file "<<dat<<".\n";
         delete db;
         return;
        }
 LoadAndConvert(db,plik);
 fclose(plik);
 char dat2[80];
 fprintf(LOG,"New name..\n");
 cout<<"New name for base "<<dat<<": ";
 takestring(dat2);
 strcpy(dat,"DataBase/");
 strcat(dat,dat2);
 fprintf(LOG,"Is %s\n",dat);
 cout<<"New name is: "<<dat<<endl;
 if ((plik = fopen(dat,"r+")) != NULL)
        {
         char zn;
         fprintf(LOG,"LD: File exists\n");
         cout<<"Base "<<dat<<" exists, overwrite it? ";
         takechar(&zn);
         if (zn != 'y') {fprintf(LOG,"Halted\n");fclose(plik);delete db;return;}
        }
 if ((plik = fopen(dat,"w+")) == NULL)
        {
         fprintf(LOG,"Cannot write to %s\n",dat);
         cout<<"Fatal Reflection:\tCannot write to "<<dat<<".\n";
         delete db;
         return;
        }
 Save(db,plik);
 fclose(plik);
 delete db;
}
void SayHello()
{
 fprintf(LOG,"Running SayHello(void)\n");
 lns(2);
 #if OSTYPE == LINUX
 fprintf(LOG,"On Linux\n");
 cout<<"**************************************************************************************************************************************************************\n";
 cout<<"*                                                                                                                                                            *\n";
 cout<<"*          PROGRAM 32 BIT                       ****    ****     ****                                           System requires:                             *\n";
 cout<<"*                                               *   *   *   *   *    *            PROGRAM WRITTEN BY                                                         *\n";
 cout<<"*           written in GNU c++                  *   *   *   *   *                                                   200 KB on HDD                            *\n";
 cout<<"*                                               ****    ****    *                   MORGOTH DBMA                                                             *\n";
 cout<<"*          for Linux & Windows NT               *   *   *       *   **                                              512 KB RAM                               *\n";
 cout<<"*                                               *   *   *       *    *                 V.2.1                                                                 *\n";
 cout<<"*                                               *   *   *        ****  **                                           Compiler   C++                           *\n";
 cout<<"*                                                                                                                                                            *\n";
 cout<<"**************************************************************************************************************************************************************\n";
 lns(2);
 #elif OSTYPE == WINDOWS
 fprintf(LOG,"On Windows NT\n");
 cout<<"*********************************************************************\n";
 cout<<"*                                                                   *\n";
 cout<<"*                      ****    ****     ****                        *\n";
 cout<<"*                      *   *   *   *   *    *                       *\n";
 cout<<"*                      *   *   *   *   *                            *\n";
 cout<<"*                      ****    ****    *                            *\n";
 cout<<"*                      *   *   *       *   **                       *\n";
 cout<<"*                      *   *   *       *    *                       *\n";
 cout<<"*                      *   *   *        ****  **                    *\n";
 cout<<"*                                                                   *\n";
 cout<<"*********************************************************************\n";
 lns(2);
 #endif
 #if OSTYPE == LINUX
 cout<<"If data doesn't create regular frame, you should improve resolution or choose smaller font!\n";
 #endif
}
void ToBinary(FILE* in, FILE* out)
{
 fprintf(LOG,"Running ToBinary(FILE*,FILE*)\n");
 fprintf(LOG,"Input offset is 0x%x\n",in);
 fprintf(LOG,"Output offset is 0x%x\n",out);
 int zn;
 int array[8];
 cout<<"Saving binary table...\n";
 while ((zn = fgetc(in)) != EOF)
 	{
    for (int i = 7;i>=0;i--)
    	{
       if (zn >= (int)pow(2,i)) {array[7-i] = 1; zn -= (int)pow(2,i);} else array[7-i] = 0;
      }
    for (int i=0;i<8;i++) fprintf(out,"%d",array[i]);
   }
}
void FromBinary(FILE* in, FILE* out)
{
 fprintf(LOG,"Running FromBinary(FILE*,FILE*)\n");
 fprintf(LOG,"Input offset is 0x%x\n",in);
 fprintf(LOG,"Output offset is 0x%x\n",out);
 int zn;
 int sign;
 int array[8];
 cout<<"Loading binary table...\n";
 while ((zn = fgetc(in)) != EOF)
 	{
    array[0] = zn-48;
    for (int i=1;i<8;i++) array[i] = fgetc(in)-48;
    sign = 0;
    for (int i=0;i<8;i++) sign += int(pow(2,7-i)*array[i]);
    fprintf(out,"%c",sign);
   }
}
void BinaryFiles(char* binB, char* binC,int modal)
{
 fprintf(LOG,"Running BinaryFiles(char*,char*,int)\n");
 fprintf(LOG,"Base offset is 0x%x\n",binB);
 fprintf(LOG,"Character offset is 0x%x\n",binC);
 fprintf(LOG,"modal = %d\n",modal);
 if (modal == 0)
 	{
                fprintf(LOG,"About batch binary files...\n");
    cout<<"Allows you to recover damaged DataBase or Character\n";
    cout<<"Batch binary files? ";
 	 char zn;
 	 takechar(&zn);
 	 if (zn != 'y') return;
   }
 char bB[STRING];
 char nB[STRING];
 char bC[STRING];
 char nC[STRING];
 strcpy(nB,binB);
 strcpy(nC,binC);
 strcpy(bB,binB);
 strcpy(bC,binC);
 strcat(bB,"_bin");
 strcat(bC,"_bin");
 cout<<"Orginal files: \n\t"<<nB<<"\n\t"<<nC<<endl;
 cout<<"Binary files: \n\t"<<bB<<"\n\t"<<bC<<endl;
 FILE* plik;
 FILE* plik2;
 if (modal != 0)
 	{
    if ((plik = fopen(nB,"r+")) != NULL)
    	{
       if ((plik2 = fopen(bB,"w+")) != NULL)
       	{
          ToBinary(plik,plik2);
          fclose(plik2);
         }
       else {fprintf(LOG,"LD: Error Cannot write to %s\n",bB);cout<<"Fatal Reflection:\tCannot write to "<<bB<<endl;}
       fclose(plik);
      }
    else {cout<<"Fatal Reflection:\tCannot read "<<nB<<endl;fprintf(LOG,"LD: Error cannot read from %s \n",nB);}
    if ((plik = fopen(nC,"r+")) != NULL)
    	{
       if ((plik2 = fopen(bC,"w+")) != NULL)
       	{
          ToBinary(plik,plik2);
          fclose(plik2);
         }
       else {cout<<"Fatal Reflection:\tCannot write to "<<bC<<endl;fprintf(LOG,"LD: Error cannot write to %s \n",bC);}
       fclose(plik);
      }
    else {cout<<"Fatal Reflection: Cannot read \t "<<nC<<endl;fprintf(LOG,"LD: Error cannot read from\n",nC);}
   }
 if (modal == 0)
 	{
    if ((plik = fopen(bB,"r+")) != NULL)
    	{
       if ((plik2 = fopen(nB,"w+")) != NULL)
       	{
          FromBinary(plik,plik2);
          fclose(plik2);
         }
       else {cout<<"Fatal Reflection:\tCannot write to "<<nB<<endl;fprintf(LOG,"LD: Error cannot write to %s\n",nB);}
       fclose(plik);
      }
    else {cout<<"Fatal Reflection:\tCannot read "<<bB<<endl;fprintf(LOG,"LD: Error cannot read from %s \n",bB);}
    if ((plik = fopen(bC,"r+")) != NULL)
    	{
       if ((plik2 = fopen(nC,"w+")) != NULL)
       	{
          FromBinary(plik,plik2);
          fclose(plik2);
         }
       else {cout<<"Fatal Reflection:\tCannot write to "<<nC<<endl;fprintf(LOG,"LD: Error cannot write to %s \n",nC);}
       fclose(plik);
      }
    else {cout<<"Fatal Reflection:\tCannot read "<<bC<<endl;fprintf(LOG,"LD: Error cannot read from %s\n",bC);}
   }
}
void Process(char* db, char* cn)
{
 fprintf(LOG,"Running process(char*,char*)\n");
 fprintf(LOG,"db is %s offset 0x%x\n",db,db);
 fprintf(LOG,"cn is %s offset 0x%x\n",cn,cn);
 lns(3);
 #if OSTYPE == LINUX
 fprintf(LOG,"Magic conf for Linux, spying...\n");
 cout<<"R.P.G. program for Linux by morgoth DBMA v3.0\n";
 #elif OSTYPE == WINDOWS
 fprintf(LOG,"Magic conf for WinNT, spying...\n");
 cout<<"R.P.G. program for Windows NT by morgoth DBMA v3.0 (compiled from Linux)\n";
 #endif
 lns(3);
 SayHello();
 #if OSTYPE == LINUX
 init_random();
 fprintf(LOG,"Running init_random(void)\nLD: random enabled\n");
 #endif
 char temp[40],temp2[40];
 strcpy(temp,"DataBase/");
 strcat(temp,db);
 cout<<"Data base:\t"<<temp<<endl;
 if (strcmp(cn,"/dev/null") != 0)strcpy(temp2,"Characters/");
 else strcpy(temp2,"");
 strcat(temp2,cn);
 cout<<"Character: \t"<<temp2<<endl;
 DATA* base = new DATA;
 DataBase(temp,base);
 AddLine(temp,temp2,base->lr);
 if (strcmp(cn,"/dev/null") != 0)
 {
  RPG* post = new RPG;
  BinaryFiles(temp,temp2,0);
  RolePlayConstructor(base,post,temp2,temp);
  BinaryFiles(temp,temp2,1);
  delete base;
  delete post;
 }
 else
 {
 fprintf(LOG,"Character name is /dev/null\n");
 fprintf(LOG,"Output = void, empty console\n");
 delete base;
 }
 #if OSTYPE == LINUX
 kill_random();
 fprintf(LOG,"running kill_random(void)\nLD: random disabled\n");
 #endif
}
void Info()
{
 fprintf(LOG,"Running Info(void);\n");
 cout<<"Display program info? ";
 char zn;takechar(&zn);
 if (zn == 'n') return;
 FILE* info;
 char* name = "DataBase/info";
 if ((info = fopen(name,"r+"))==NULL)
        {
         fprintf(LOG,"File info dataBase/info doesn't exists\n");
         cout<<"Fatal Reflection:\tCannot read "<<name<<endl;
         takechar(&zn);
        }
 else
        {
         int dat;
         fprintf(LOG,"Displaying info file on console %d\n",HIDE);
         while ((dat = fgetc(info)) != EOF) {printf("%c",dat);}
         fclose(info);
         takechar(&zn);
        }
}
int Register()
{
 fprintf(LOG,"Running Register(void)\n");
 cout<<"Checking registry :>\n";
 FILE* reg;
 char* name = "DataBase/reg.so";
 if ((reg=fopen(name,"r+"))==NULL)
        {
         fprintf(LOG,"Registry file not found, I won't tell you its name :)\n");
         cout<<"No required file\tAccess Denied\n";
         return FALSE;
        }
 else
        {
         int r;
         fscanf(reg,"%x\n",&r);
         fprintf(LOG,"Registry found, checking if user registered\n");
         if (r != 1410)
                {
                 fprintf(LOG,"User not registered, doing it now\n");
                 fclose(reg);
                 reg = fopen(name,"w+");
                 fprintf(reg,"%x\n",1410);
                 cout<<"Your name: ";
                 char data[STRING];
                 takestring(data);
                 fprintf(reg,"User name: %s\n",data);
                 cout<<"Your surname: ";
                 takestring(data);
                 fprintf(reg,"User surname: %s\n",data);
                 cout<<"Your xywa: ";
                 takestring(data);
                 fprintf(reg,"User xywa: %s\n",data);
                }
         else fclose(reg);
         return TRUE;
        }
}
void HELP()
{
 fprintf(LOG,"Running HELP(void)\n");
 cout<<"RPG ang version 2.4\n";
 cout<<"programmed for "<<ostype<<".\n";
 cout<<"polish help, english in file DataBase/info\n";
 cout<<"UZYCIE: rpg baza postac, gdzie baza oznacza baze danych na podstawie ktorej tworzysz\n";
 cout<<"a postac oznacza nazwe pliku jaki masz zamiar utworzyc powstanie Characters/postac i Characters/postac.html\n";
 cout<<"Inne uzycie rpg baza tylko do edycji bazy nie utworzy postaci lub tylko rpg\n";
 cout<<"PARAMETRY:\n";
 cout<<"\t-h --help wyswietla ta informacja\n";
 cout<<"\t-t --triada wyswietla informacje o systemie TRIADA i jej autorach\n";
 cout<<"\t-s --source wyswietla wlasny kod zrodlowy\n";
 cout<<"\t-v --version podaje wersje i warunki programu\n";
 cout<<"\trpg [base] --convert lub -c (konwertuje baze danych innego systemu).\n";
 cout<<"\trpg [base] --convchar lub -cc (konwertuje baze danych innego systemu).\n";
 cout<<"\t\tmorgoth lokalizacja DataBase/morgoth sluzy do tworzenia postaci w TRIADZIE\n";
 cout<<"\t\ttriada  lokalizacja DataBase/triada sluzy do skonvertowana: rpg triada.bm --convert z RPG 32bit\n";
 cout<<"\t\tgalthar lokalizacja DataBase/galthar system autorstwa Galthara testy k100 takze analogicznie skonwertowana\n";
 fprintf(LOG,"End of help log  massage\n");
 fclose(LOG);
 #if OSTYPE == LINUX
 last_op(NULL,NULL);
 #endif
 exit( -1 );
}
void TRIADA(char* name)
{
 fprintf(LOG,"Running TRIADA(void)\n");
 FILE* plik;
 if ((plik = fopen(name,"r+"))==NULL)
        {
         fprintf(LOG,"no required file %s\n",name);
         cout<<"Fatal Reflection:\tCannot read "<<name<<endl;
        }
 else
        {
         int dat;
         fprintf(LOG,"read %s\n",name);
         while ((dat = fgetc(plik)) != EOF) {printf("%c",dat);}
         fclose(plik);
        }
 fclose(LOG);
 #if OSTYPE == LINUX
 last_op(NULL,NULL);
 #endif
 exit( -1 );
}
void DataProtector()
{
 fprintf(LOG,"Running DataProtector(void)\n");
 cout<<"Data protector will protect you against bad streams f.e\n";
 cout<<"Character instead digit, but initializes different types\n";
 cout<<"Of linux konsoles, so if there are problem edit console\n";
 cout<<"Type and choose Data Protect to mode 3\n";
 #if OSTYPE == LINUX
 if (HIDE == 3) {cout<<"\n\nNo data protection linux\n\n";fprintf(LOG,"Console not protected :( Linux\n");}
 #elif OSTYPE == WINDOWS
 if (HIDE == 3) {cout<<"\n\nNo data protection windows\n\n";fprintf(LOG,"Console not protected :( Windows NT\n");}
 #endif
}
void ConsoleInfo()
{
 fprintf(LOG,"Running ConsoleInfo(void)\n");
 cout<<"Detected OS:\t"<<ostype<<", or compatible\n";
 cout<<"Starting up for "<<ostype<<".\n";
 fprintf(LOG,"Detected OS is %s or compatible\n",ostype);
 cout<<"Console types\n";
 cout<<"-1\t-Not official variances\n";
 cout<<"0\t-No mask, data protect\n";
 cout<<"1\t-Mask & data protect\n";
 cout<<"2\t-No echo & data protect\n";
 cout<<"3\t-No data protect :(\n";
 cout<<"Diffrent\t-Default value 0\n";
 cout<<"Choose? ";
 fprintf(LOG,"Now Konsole no.%d\n",HIDE);
 cin>>HIDE;
 cout<<"Initializing Konsole\n";
 if ((HIDE<-1) || (HIDE>3)) HIDE = 0;
 fprintf(LOG,"Switching...\nNow Console no.%d\n",HIDE);
 DataProtector();
}
void KonsoleType()
{
 fprintf(LOG,"Running KonsoleType(void)\n");
 FILE* console;
 char* name = "DataBase/Console_inf";
 fprintf(LOG,"Console file is %s\n",name);
 if ((console = fopen(name,"r+")) == 0)
        {
         fprintf(LOG,"Missing console file %s\nCreating...",name);
         cout<<"No Konsole file\nCreating "<<name<<".\n";
         ConsoleInfo();
         if ((console = fopen(name,"w+")) == 0)
                {
                 fprintf(LOG,"Console %s cannot be created\n",name);
                 cout<<"Cannot make file "<<name<<".\n";
                 return;
                }
         fprintf(console,"%d",HIDE);
         fclose(console);
         fprintf(LOG,"New console file saved\n");
         return;
        }
 else
        {
         fscanf(console,"%d",&HIDE);
         fprintf(LOG,"Read console\nAbout Edit\n");
         cout<<"Edit Konsole file? ";
         char zn;
         takechar(&zn);
         fclose(console);
         if (zn != 'y') return;
        }
 if ((console = fopen(name,"w+")) == 0)
        {
         fprintf(LOG,"Cannot refresh file %s\n",name);
         cout<<"Cannot make Konsole file "<<name<<".\n";
         return;
        }
 ConsoleInfo();
 fprintf(console,"%d",HIDE);
 fclose(console);
}
void CreateFile(FILE* plik, char* name,char* data,bool close = true)
{
 fprintf(LOG,"Running CreateFile(FILE,char*,char*)\n");
 fprintf(LOG,"FILE offset is 0x%x\n",plik);
 fprintf(LOG,"name is %s offset 0x%x\n",name,name);
 fprintf(LOG,"data is %s offset 0x%x\n",data,data);
 if ((plik = fopen(name,"r+")) != NULL)
        {
         if (close == true) fprintf(LOG,"File %s exists\n",name);
         if (close == true) cout<<"File "<<name<<" exists, overwrite? ";
         fclose(plik);
         char zn;
         if (close == true) takechar(&zn);
         else zn = 'y';
         if (zn != 'y') return;
         if (close == true) fprintf(LOG,"Overwriting\n");
        }
 if ((plik = fopen(name,"w+")) == NULL)
        {
         fprintf(LOG,"LD: error it's impossible to create file %s\n",name);
         cout<<"Impossible creation "<<name<<".\n";
         return;
        }
 cout<<"I have created "<<name<<".\n";
 fprintf(LOG,"Created %s\n",name);
 fprintf(plik,"%s",data);
 if (close == true) {fclose(plik);
 fprintf(LOG,"Closed %s\n",name);}
 else fprintf(LOG,"THIS file you read, not closed yet...\n");
}
void AboutLog()
{
 fclose(LOG);
 if ((LOG = fopen("DataBase/spy_log","w+")) == NULL)
        {
         cout<<"Critical error with LOG file.\n";
         if ((LOG = fopen("/dev/null","w+")) == NULL)
                {
                 cout<<"Unable to fix it, terminate.\n";
                 #if OSTYPE == LINUX
                 last_op(NULL,NULL);
                 #endif
                }
         else cout<<"LOG info will be send to /dev/null\n";
        }
 else
        {
         fprintf(LOG,"LD: creation forced, starting LOG in the middle of program.\nRegistry and all config files will be created...\n**** FIXING ****\n\n");
         cout<<"LOG file forced.\n";
        }
}
void CheckFolders()
{
 fprintf(LOG,"Running CheckFolders(void)\n");
 #if OSTYPE == LINUX
 cout<<"\n\n";
 char* nameB = "DataBase/";
 char* nameC = "Characters/";
 fprintf(LOG,"There should be %s & %s\n",nameB,nameC);
 DIR* wsk = opendir(nameB);
 if (!wsk)
        {
         char name[STRING];
         char data[STRING*2];
         fprintf(LOG,"%s doesn't exists, creating...\n");
         cout<<nameB<<" does not exist, creating\n";
         FILE* plik;
         strcpy(name,"DataBase/info");
         strcpy(data,"No info available.");
         mkdir(nameB,DIR_EXEC);
         AboutLog();
         CreateFile(plik,name,data);
         strcpy(name,"DataBase/triada.i");
         strcpy(data,"No system info, copy of directories created runtime");
         CreateFile(plik,name,data);
         strcpy(name,"DataBase/reg.so");
         strcpy(data,"581");
         CreateFile(plik,name,data);
        }
 wsk = opendir(nameC);
  if (!wsk)
        {
         fprintf(LOG,"%s doesn't exists, creating...\n",nameC);
         cout<<nameC<<" does not exists, creating.\n";
         mkdir(nameC,DIR_EXEC);
        }
 cout<<"\n\n";
 #elif OSTYPE == WINDOWS
 cout<<"No directory check on Windows (NT)\n";
 fprintf(LOG,"Detected OS Windows, left directories unchanged\n");
 #endif
}
void ExecuteCommand(char* cmd, RPG* post)
{
 fprintf(LOG,"Running executeCommand(char*,RPG*)\n");
 fprintf(LOG,"cmd is %s offset 0x%x\n",cmd,cmd);
 fprintf(LOG,"RPG offset is 0x%x\n",post);
 cout<<"Unknown Command\n";
 if (strcmp(cmd,"st") == 0)
        {
         cout<<"new value? ";
         takeint(&post->st);
         return;
        }
  if (strcmp(cmd,"cecha[]") == 0)
        {
         ShowAttChar(post);
         for (int i=0;i<post->ilosc_cech;i++)
                {
                 cout<<"new value for "<<post->n_cechy[i]<<"? ";
                 takeint(&post->cecha[i]);
                }
         ShowAttChar(post);
         return;
        }
  if (strcmp(cmd,"pozum[]") == 0)
        {
         for (int i=0;i<post->il_um;i++)
                {
                 cout<<"level of "<<post->um[i]<<"? ";
                 takeint(&post->pozum[i]);
                }
         return;
        }
  if (strcmp(cmd,"tesum[]") == 0)
        {
         for (int i=0;i<post->il_um;i++)
                {
                 cout<<"test of "<<post->um[i]<<"? ";
                 takeint(&post->tesum[i]);
                }
         return;
        }
  if (strcmp(cmd,"trum[]") == 0)
        {
         for (int i=0;i<post->il_um;i++)
                {
                 cout<<"difficulity of "<<post->um[i]<<"? ";
                 takeint(&post->trum[i]);
                }
         return;
        }
  if (strcmp(cmd,"ceum[]") == 0)
        {
         for (int i=0;i<post->il_um;i++)
                {
                 ShowAttChar(post);
                 cout<<"number of attribute testing "<<post->um[i]<<"? ";
                 takeint(&post->ceum[i]);
                 if (post->ceum[i]>post->ilosc_cech -1) post->ceum[i] = post->ilosc_cech-1;
                 if (post->ceum[i]<0) post->ceum[i] = 0;
                }
         return;
        }
  if (strcmp(cmd,"mulum[]") == 0)
        {
         for (int i=0;i<post->il_um;i++)
                {
                 cout<<"multiple in  level up for "<<post->um[i]<<"? ";
                 takeint(&post->mulum[i]);
                }
         return;
        }
  if (strcmp(cmd,"magum[]") == 0)
        {
         for (int i=0;i<post->il_um;i++)
                {
                 cout<<"magical level of "<<post->um[i]<<"? ";
                 takeint(&post->magum[i]);
                }
         return;
        }
  if (strcmp(cmd,"PD") == 0)
        {
         cout<<"value? ";
         takeint(&post->PD);
         return;
        }
  if (strcmp(cmd,"p_c[]") == 0)
        {
         for (int i=0;i<post->ilosc_cech;i++)
                {
                 cout<<"current "<<post->n_cechy[i]<<" pluses "<<post->p_c[i]<<" new value? ";
                 takeint(&post->p_c[i]);
                }
         return;
        }
  if (strcmp(cmd,"p_u[]") == 0)
        {
         for (int i=0;i<post->il_um;i++)
                {
                 cout<<"current "<<post->um[i]<<" pluses "<<post->p_u[i]<<" new value? ";
                 takeint(&post->p_u[i]);
                }
         return;
        }
  if (strcmp(cmd,"stMM") == 0)
        {
         cout<<"value? ";
         takeint(&post->stMM);
         return;
        }
  if (strcmp(cmd,"numMM") == 0)
        {
         ShowAttChar(post);
         cout<<"value? ";
         takeint(&post->numMM);
         return;
        }
  if (strcmp(cmd,"morgothdbma") == 0)
        {
         cout<<"overlapping...\n";
         post->PD = 666;
         post->stMM += 100;
         return;
        }
  if (strcmp(cmd,"pwd") == 0)
        {
         ShowAll(post);
         return;
        }
 return;
}
void BigEdit(RPG* post)
{
 fprintf(LOG,"Running BigEdit(RPG*)\n");
 fprintf(LOG,"RPG offset is 0x%x\n",post);
 char command[STRING];
 fprintf(LOG,"Console is no.%d\n",HIDE);
 int temp = HIDE;
 fprintf(LOG,"Changing & remembering in temporary...\n");
 HIDE = 2;
 fprintf(LOG,"New console set no.%d\n",HIDE);
 strcpy(command,"no_data");
 while (strcmp(command,"end") != 0)
        {
         cout<<"Awaiting Command? ";
         takestring(command);
         ExecuteCommand(command,post);
        }
 HIDE = temp;
 fprintf(LOG,"Restoring previous console\nKonsole set no.%d",HIDE);
}
void EDIT(char* name)
{
 fprintf(LOG,"Running EDIT(char*)\n");
 fprintf(LOG,"name is %s offset 0x%x\n",name,name);
 FILE* plik;
 char dat[STRING];
 strcpy(dat,"Characters/");
 fprintf(LOG,"Folders prepared\n");
 strcat(dat,name);
 if ((plik = fopen(dat,"r+")) == NULL)
        {
         fprintf(LOG,"LD: Fatal error (you cheater) no file %s\n",dat);
         cout<<"\nFatal Reflection:\tNo such file: "<<dat<<".\n";
         return;
        }
 cout<<"OK.\n";
 RPG* post = new RPG;
 LoadChar(post,plik);
 fprintf(LOG,"Read file %s\n",dat);
 fclose(plik);
 fprintf(LOG,"Closed file %s\n",dat);
 CheckPassChar(post);
 ShowAll(post);
 BigEdit(post);
 ShowAll(post);
 cout<<"Save? ";
 char zn;
 takechar(&zn);
 if (zn == 'y') {fprintf(LOG,"Saving CHEATER seed...\n");SaveChar(post,plik,dat);}
 else fprintf(LOG,"Cheats unsaved :)\n");
 delete post;
}
int main(int argv, char** data)                  //main segment
{
 OpenLog();
 fprintf(LOG,"Info switched, becouse in main initialized log file (to avoid Segmentation Error)\nRunning main(int,char**)\n");
 fprintf(LOG,"argv is %d\n",argv);
 fprintf(LOG,"data offset is 0x%x \n",data);
 fprintf(LOG,"Data[] are:\n");
 for (int i=0;i<argv;i++) fprintf(LOG,"data[%d] is %s offset 0x%x\n",i,data[i],data[i]);
 CheckFolders();
 KonsoleType();
 if ((argv == 3) && (strcmp(data[2],"--convert") == 0))
        {
         Conversion(data[1]);
         EndWin();
         return 0;
        }
 if ((argv == 3) && (strcmp(data[2],"-c") == 0))
        {
         Conversion(data[1]);
         EndWin();
         return 0;
        }
 if ((argv == 3) && (strcmp(data[2],"-cc") == 0))
        {
         ConversionC(data[1]);
         EndWin();
         return 0;
        }
 if ((argv == 3) && (strcmp(data[2],"--convchar") == 0))
        {
         ConversionC(data[1]);
         EndWin();
         return 0;
        }
 if ((argv == 3) && ((strcmp(data[2],"-e") == 0) || (strcmp(data[2],"--edit") == 0)))
        {
         int check = Register();
         if (!check) {fclose(LOG);exit ( -1 );}
         EDIT(data[1]);
         EndWin();
         return 0;
        }
 if (argv == 2)
        {
         if ((strcmp(data[1],"-h")==0) || (strcmp(data[1],"--help")==0)) HELP();
         if ((strcmp(data[1],"-t")==0) || (strcmp(data[1],"--triada")==0)) TRIADA("DataBase/triada.i");
         if ((strcmp(data[1],"-s")==0) || (strcmp(data[1],"--source")==0)) TRIADA("rpg.c");
         if ((strcmp(data[1],"-v")==0) || (strcmp(data[1],"--version")==0)) {cout<<"VERSION 3.0 FreeCopy; OpenSource; @copyleft\nMorgoth DBMA.\n";return 0;}
         if (data[1][0] == '-') {cout<<"Syntax Error, try rpg -h or rpg --help\n";fclose(LOG);
         #if OSTYPE == LINUX
         last_op(NULL,NULL);
         #endif
         exit( -1 );}
        }
 if (argv == 1)
        {
         int check = Register();
         if (!check) {fclose(LOG);exit ( -1 );}
         cout<<"Data base name: ";
         char based[60];
         takestring(based);
         cout<<"Character name or null: ";
         char plikp[60];
         takestring(plikp);
         if (strcmp(plikp,"null") == 0) strcpy(plikp,"/dev/null");
         Process(based,plikp);
         EndWin();
         return -1;
        }
 if (argv >3)
        {
         cout<<"Too many parameters\nUsage rpg [data base] [character].\n";
         cout<<"Try rpg -h\n";
         cout<<"Parameters:\n";
         for (int i=3;i<argv;i++)
                {
                 cout<<data[i]<<endl;
                }
         cout<<"Will be ignored\n";
         Info();
        }
 cout<<"\nInstead SPACE use _\n";
 Info();
 int check = Register();
 if (!check) {fclose(LOG);exit ( -1 );}
 if (argv != 2) Process(data[1],data[2]);
 if (argv == 2) Process(data[1],"/dev/null");
 #if OSTYPE == LINUX
 last_op(NULL,NULL);
 #endif
 fprintf(LOG,"Halted!\nBefore return OK, last action...\n");
 fclose(LOG);
 return OK;
}
