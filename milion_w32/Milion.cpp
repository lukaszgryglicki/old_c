#include <stdio.h>
#include <stdlib.h>
#include <fstream.h>
//#include <conio.h>
#include <string.h>
#include <mran.h>
#include <mh.h>

#define N_QUE 15

typedef unsigned int uint;
static uint* qtable = NULL;
static bool h_tel = true;
static bool h_pub = true;
static bool h_pol = true;

struct q_question
{
 char ok;
 int pa;
 int pb;
 int pc;
 int pd;
 char* text;
 char* odpa;
 char* odpb;
 char* odpc;
 char* odpd;
};

struct q_entry
{
 int num;
 int cost;
 q_question* q;
};

static q_entry* data = NULL;

inline void downcase(int& zn)
{
 if (zn>'A' && zn<'Z') zn += 0x20;
}

//inline void ln(){printf("\n");}


void init_questions(int n)
{
 data[n].cost = qtable[data[n].num];
 data[n].q = new q_question[data[n].num];
 if (!data[n].q) {printf("Brak wolnej pamieci RAM!\n"); getch(); ln(); exit(-1);}
}

void scan_line(char*& buf, FILE* in)
{
 char* tmp = new char[51200];
 if (!tmp) {printf("Brak wolnej pamieci RAM!\n"); getch(); ln(); exit(-1);}
 int i=0;
 while ((tmp[i]=(char)fgetc(in)) != 10 && tmp[i] != EOF) i++;
 tmp[i] = '\0';
 buf = new char[i+1];
 if (!buf) {printf("Brak wolnej pamieci RAM!\n"); getch(); ln(); exit(-1);}
 strcpy(buf, tmp);
 if (!strcmp(buf,"")) printf("Wyskanowalem pusta linie: kod bledu (UNKNOWN,0x%x)\n", in);
// printf("%s\n", buf);
}

int read_question(int i, int j, FILE* in)
{
 int ok;
 int itmp;
 ok = fscanf(in, "%d: ", &itmp);
 if (!ok) printf("Pytanie (%d,%d) brak napisu przed pytaniem: \"liczba: tresc\"\n", i,j);
 scan_line(data[i].q[j].text, in);
 ok += fscanf(in, "a) ");
 scan_line(data[i].q[j].odpa, in);
 ok += fscanf(in, "b) ");
 scan_line(data[i].q[j].odpb, in);
 ok += fscanf(in, "c) ");
 scan_line(data[i].q[j].odpc, in);
 ok += fscanf(in, "d) ");
 scan_line(data[i].q[j].odpd, in);
 ok += fscanf(in,"%c,%d,%d,%d,%d\n---\n", &data[i].q[j].ok,
 				&data[i].q[j].pa,&data[i].q[j].pb,
 				&data[i].q[j].pc,&data[i].q[j].pd);
 int sum = data[i].q[j].pa+data[i].q[j].pb+data[i].q[j].pc+data[i].q[j].pd;
 if (sum != 100) printf("Ostrzezenie! suma prawdopodobienstw odpowiedzi nie jest rowna 100; Kod pytania (%d, %d)\n", i,j);
 return (ok == 6);
}

int read_entires(FILE* in)
{
 int itmp;
 int ok;
 for (int i=0;i<N_QUE;i++)
   {
    ok = fscanf(in,"pytan za %d: %d\n---\n", &itmp, &data[i].num);
    if (ok != 2)
      {
       printf("Odczyt dotyczacy ilosci pytan w etapie %d niemozliwy!", i);
       printf(" (kod bledu: %d)\n", ok);
       return 0;
      }
    init_questions(i);
    for (int j=0;j<data[i].num;j++)
     {
      ok = read_question(i, j, in);
      if (!ok) {printf("Odczyt pytania numer %d z kategorii numer %d niemozliwy!\n",j,i); return 0;}
     }
   }

 return 1;
}

void init_qcost()
{
 qtable = new uint[N_QUE];
 if (!qtable) {printf("Brak wolnej pamieci RAM!\n"); getch(); ln(); exit(-1);}
 qtable[0] = 100;
 qtable[1] = 200;
 qtable[2] = 300;
 qtable[3] = 500;
 qtable[4] = 1000;
 qtable[5] = 2000;
 qtable[6] = 4000;
 qtable[7] = 8000;
 qtable[8] = 16000;
 qtable[9] = 32000;
 qtable[10] = 64000;
 qtable[11] = 125000;
 qtable[12] = 250000;
 qtable[13] = 500000;
 qtable[14] = 1000000;
}

void init_nodes()
{
 data = new q_entry[N_QUE];
 if (!data) {printf("Brak wolnej pamieci RAM!\n"); getch(); ln(); exit(-1);}
 for (int i=0;i<N_QUE;i++)
  {
   data[i].num = 0;
   data[i].cost = qtable[i];
  }
}

int read_database()
{
 FILE* db = fopen("milion.txt","r");
 if (!db) {printf("Brak pliku: milion.dat\n"); return 0;}
 int ok = read_entires(db);
 fclose(db);
 printf("Odczytalem baze danych.\n");
 if (!ok) {printf("Byly bledy w bazie danych!\n"); getch(); ln(); return 0;}
 return 1;
}

void wyplac(int num)
{
 printf("Wyplacam %d zl przez drukarke............\n", num);
 printf("czekaj, czekaj, czekaj, czekaj, czekaj, czekaj...\n");
 getch();
 ln();
}

void ask_question(int ran, int i)
{
 printf("OTO PYTANIE ZA %d zl:\n", qtable[i]);
 printf("%s\n", data[i].q[ran].text);
 printf("a) %s\n", data[i].q[ran].odpa);
 printf("b) %s\n", data[i].q[ran].odpb);
 printf("c) %s\n", data[i].q[ran].odpc);
 printf("d) %s\n", data[i].q[ran].odpd);
 printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
 printf("\nNacisnij na klawiaturze litere: a,b,c,d\n");
 printf("lub t(telefon), l(publicznosc), p(pol-na-pol)\n");
 printf("q(zakoncz), r(rezygnuj z dalszej gry)\n");
 printf("\n==> ");
}

int answer(int zn)
{
 if (zn=='a' || zn=='b' || zn=='c' || zn=='d') return 1;
 else return 0;
}

int kolo(int zn)
{
 if (zn=='t' || zn=='p' || zn=='l' || zn=='r') return 1;
 else return 0;
}

void czas_na_reklame();

void verify(int user_odp, int ran, int& lev, bool& crash)
{
 printf("Sprawdzmy czy to jest poprawna odpowiedz..\n");
 if (lev && !(lev%6)) czas_na_reklame();
 if (data[lev].q[ran].ok != user_odp)
  {
   crash = true;
   printf("To jest bledna odpowiedz!\n");
   int tmp  = ((lev/5)*5)-1;
   if (tmp<0) tmp=0;
   lev = tmp;
  }
 else { printf("Gratulacje, poprawna odpowiedz!\n"); lev++;}
}

char odpowiedz(int* a)
{
 printf("...Pomyslmy...\n");
 int max = a[0];
 int index=0;
 for (int i=1;i<4;i++) if (a[i]>max) { max=a[i]; index=i;}
 return index+'a';
}

void use_kolo(int type, int ran, int& lev, bool& crash)
{
 if (type=='r')
   {
    printf("Wiec chcesz zrezygnowac z dalszej gry\n");
    printf("No coz zabierz swoja dotychczasowa wygrana...\n");
    lev--;
    crash=true;
    return;
   }
 printf("A wiec decydujesz sie na kolo ratunkowe\n");
 if (type == 'p')
   {
    if (!h_pol) {printf("Nie masz juz tego kola...\n"); return;}
    printf("Prosze komputer o odrzucenie dwoch blednych odpowiedzi\n");
    printf("Tak aby pozostala jedna poprawna i jedna bledna\n");
    int oki = data[lev].q[ran].ok;
    oki -= 'a';
    int bad;
    int bad2;
    do { bad=random(4);} while (bad==oki);
    do { bad2=random(4);} while (bad2==oki || bad2==bad);
    printf("\nBledne odpowiedzi to: %c, %c\n", bad+'a', bad2+'a');
    h_pol = false;
   }
 if (type == 'l')
   {
    if (!h_pub) {printf("Nie masz juz tego kola...\n"); return;}
    printf("No to prosze Panstwa, glosujemy..\n");
    printf("Dostepne odpowiedzi to a,b,c,d\n");
    int ar,br,cr,dr;
    ar = random(data[lev].q[ran].pa);
    br = random(data[lev].q[ran].pb);
    cr = random(data[lev].q[ran].pc);
    dr = random(data[lev].q[ran].pd);
    int rest = (100-(ar+br+cr+dr))/4;
    ar+=rest;
    br+=rest;
    cr+=rest;
    dr+=rest;
    printf("Glosy publicznosci:\na) [%-3d]\nb) [%-3d]\nc) [%-3d]\nd) [%-3d]\n", ar,br,cr,dr);
    h_pub = false;
   }
 if (type == 't')
   {
    if (!h_tel) {printf("Nie masz juz tego kola...\n"); return;}
    printf("Telefon do przyjaciela, OK do kogo dzwonimy? ");
    char who[100];
    scanf("%s", who);
    printf("Halo %s tu Hubert urbanski z TVN, dzwonie do pana(pani)...\n", who);
    printf("Macie 30 sek od czasu rozpoczecia przeze mnie czytania pytania\n");
    int a[4];
    a[0] = random(data[lev].q[ran].pa);
    a[1] = random(data[lev].q[ran].pb);
    a[2] = random(data[lev].q[ran].pc);
    a[3] = random(data[lev].q[ran].pd);
    int rest = (100-(a[0]+a[1]+a[2]+a[3]))/4;
    a[0]+=rest;
    a[1]+=rest;
    a[2]+=rest;
    a[3]+=rest;
    printf("\nPrzyjaciel %s obstawia: %c)\n", who, odpowiedz(a));
    h_tel = false;
   }
}

void czas_na_reklame()
{
 printf("Teraz czas na krotka przerwe na reklamy...\n");
 printf("Zobaczymy sie po przerwie aby kontynuowac gre o 1,000,000 PLN!\n");
 getch(); ln();
 printf("Jezeli podoba Ci sie ten program to skontaktuj sie z autorem:\n");
 printf("MorgothDBMA (Lukasz Gryglicki) morgothdbms@o2.pl desdaemon@yahoo.com\n");
 printf("Kom: 0-693-582-014\n");
 getch(); ln();
}

void global_free();

int play_game()
{
 printf("Rozpoczynamy gre o 1,000,000 PLN\n");
 int clev = 0;
 int ran = 0;
 bool crashed = 0;
 int zn;
 while ((!crashed) && (clev < N_QUE))
   {
    ran = random(data[clev].num);
    ask_me_again:
    ask_question(ran, clev);
    zn = getch();
    ln();
    downcase(zn);
    if (answer(zn)) verify(zn, ran, clev, crashed);
    else if (kolo(zn))
      {
       use_kolo(zn, ran, clev, crashed);
       if (clev<0) clev=0;
       if (zn=='r') continue;
       goto ask_me_again;
      }
    else if (zn == 'q') {global_free(); exit(0);}
    else
      {
       printf("Nieznany przycisk, uzyj: a,b,c,d,t,l,p,q\n");
       goto ask_me_again;
      }
    if (!clev && crashed) {printf("Porazka!!!\n"); return 0;}
   }
 printf("Gra zakonczona\n");
 printf("Doszedles(as) do pytania %d za %d\n", clev, qtable[clev]);
 if (clev==15)
   {
    printf("\n\n********* GRATULACJE WYGRALES(AS) MILION 1,000,000 ********\n\n");
    return 1000000;
   }
 return (clev) ? qtable[clev] : 0;
}

void global_free()
{
 printf("Zwalniam pamiec\n");
 delete [] qtable;
 for (int i=0;i<N_QUE;i++)
   {
    for (int j=0;j<data[i].num;j++)
      {
       delete [] data[i].q[j].text;
       delete [] data[i].q[j].odpa;
       delete [] data[i].q[j].odpb;
       delete [] data[i].q[j].odpc;
       delete [] data[i].q[j].odpd;
      }
    delete [] data[i].q;
   }
 delete [] data;
 printf("OK\n");
}

int main()
{
 printf("Software Freeware v0.5 by MorgothDBMA, BSD license\n");
 init_qcost();
 init_nodes();
 randomize();
 int ok = read_database();
 if (!ok) {printf("Nie powiodl sie odczyt bazy danych: milion.dat\n"); getch(); ln(); exit(-2);}
 printf("Nacisnij 'Q' aby zakonczyc\n");
 int wygrana = play_game();
 wyplac(wygrana);					//che che che
 printf("Nacisnij cokolwiek aby zakonczyc program...");
 getch();
 ln();
 global_free();
 printf("Return 0 to OS\n");
 return 0;
}
