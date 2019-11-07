#include <mh.h>
#define OK      0

#define MAX_TELE		150
#define MAX_FILM		20
#define MAX_KONC		30
#define T_H_METAL 	0x17
#define T_T_METAL 	0x18
#define T_D_METAL 	0x19
#define T_P_METAL 	0x1a
#define T_ROCK    	0x1b
#define T_H_ROCK  	0x1c
#define T_H_CORE  	0x1d
#define T_POP     	0x1e
#define T_INNY    	0x1f
#define T_KOPIA		0x20
#define T_ORGINAL		0x21
#define T_DOKUMENT	0x22
#define T_SENSACJA	0x23
#define T_WESTERN		0x24
#define T_OBYCZAJ		0x25
#define T_HORROR		0x26
#define T_KOMEDIA		0x27
#define T_DRAMAT		0x28
#define T_WYWIAD		0x29
#define T_LIVE			0x2a
#define T_TELEDYSK	0x2b


static int cur = 0;
static int all = 0;
static char* naz = "NIEZIDENTYFIKOWANA_KASETA";

#if OSTYPE == LINUX
static char* dir = "data/";
#elif OSTYPE == WINDOWS
static char* dir = "data\\";
#endif
class KASETA
{
 public:
 KASETA();
 ~KASETA();

 void dies(void);
 void load(void);
 void save(void);
 void add(void);
 void modify(void);
 void clean(void);
 void show(void);
 void write(FILE*);
 void WriteFType(int,FILE*);
 void WriteFOrg(int,FILE*);
 void WriteKType(int,FILE*);
 void WriteKOrg(int,FILE*);
 void WriteTType(int,FILE*);
 void AddKonc(void);
 void AddTele(void);
 void AddFilm(void);
 void ModKonc(void);
 void ModTele(void);
 void ModFilm(void);
 void ModName(void);
 void OrgKoncertu(void);
 void TypKoncertu(void);
 void OrgKoncertu(int);
 void TypKoncertu(int);
 void TypTeledysku(void);
 void TypTeledysku(int);
 void OrgFilmu(void);
 void TypFilmu(void);
 void OrgFilmu(int);
 void TypFilmu(int);
 void ShowConcerts(void);
 void ShowLives(void);
 void ShowFilms(void);
 void DisplayTType(int);
 void DisplayKType(int);
 void DisplayKOrg(int);
 void DisplayFType(int);
 void DisplayFOrg(int);
 void ModifyKonc(int);
 void ModifyFilm(int);
 void ModifyTele(int);
 void DelKonc(int);
 void DelFilm(int);
 void DelTele(int);
 void SearchFilm(void);
 void SearchKoncert(void);
 void SearchLive(void);


 int numer;
 char nazwa[STRING];
 int ilosc_teledyskow;
 int ilosc_filmow;
 int ilosc_koncertow;
 char t_zespol[MAX_TELE][STRING];
 char t_tytul[MAX_TELE][STRING];
 int t_typ[MAX_TELE];
 char f_tytul[MAX_FILM][STRING];
 int f_typ[MAX_FILM];
 int f_org[MAX_FILM];
 char k_zespol[MAX_KONC][STRING];
 char k_tytul[MAX_KONC][STRING];
 int k_typ[MAX_KONC];
 int k_org[MAX_KONC];
};
KASETA :: KASETA()
{
 cout<<"Konstruowanie obiektu\t\t\t[OK]\n";
}
KASETA :: ~KASETA()
{
 cout<<"Destrukcja obiektu\t\t\t[OK]\n";
}

void KASETA :: add()
{
 char zn = 'x';
 while (zn != 'q')
 	{
    cout<<"f\t\t-dodaj film do "<<nazwa<<endl;
    cout<<"t\t\t-dodaj teledysk do "<<nazwa<<endl;
    cout<<"k\t\t-dodaj koncert do "<<nazwa<<endl;
    cout<<"q\t\t-koniec tego menu\n";
    cout<<"Twoj wybor? ";
    takechar(&zn);
    ln();
    switch(zn)
    	{
       case 'k': AddKonc();break;
       case 't': AddTele();break;
       case 'f': AddFilm();break;
       case 'q': cout<<"Koniec aktualnego menu.\n";break;
       default:  cout<<"Nieznane polecenie.\n";
      }
   }
}

void KASETA :: show()
{
 cout<<"Filmy na kasecie "<<nazwa<<" ("<<ilosc_filmow<<") \n\n";
 ShowFilms();
 if (ilosc_filmow > 0) getch();
 cout<<"Koncerty na kasecie "<<nazwa<<" ("<<ilosc_koncertow<<") \n\n";
 ShowConcerts();
 if (ilosc_koncertow > 0) getch();
 cout<<"Teledyski na kasecie "<<nazwa<<" ("<<ilosc_teledyskow<<") \n\n";
 ShowLives();
 if (ilosc_teledyskow > 0) getch();
 ln();
}

void KASETA :: modify()
{
 char zn = 'x';
 while (zn != 'q')
 	{
    cout<<"f\t\t-modyfikuj film\n";
    cout<<"t\t\t-modyfikuj teledysk\n";
    cout<<"k\t\t-modyfikuj koncert\n";
    cout<<"n\t\t-modyfikuj nazwe\n";
    cout<<"q\t\t-koniec tego menu\n";
    cout<<"Twoj wybor? ";
    takechar(&zn);
    ln();
    switch(zn)
    	{
       case 'k': ModKonc();break;
       case 't': ModTele();break;
       case 'f': ModFilm();break;
       case 'n': ModName();break;
       case 'q': cout<<"Koniec aktualnego menu.\n";break;
       default: cout<<"Nieznane polecenie.\n";
      }
   }
}

void KASETA :: DisplayKType(int par)
{
 switch(k_typ[par])
 	{
    case T_H_METAL : cout<<" H.Metal  ";break;
    case T_T_METAL : cout<<" T.Metal  ";break;
    case T_D_METAL : cout<<" D.Metal  ";break;
    case T_P_METAL : cout<<" P.Metal  ";break;
    case T_ROCK    : cout<<" Rock     ";break;
    case T_H_ROCK  : cout<<" H.Rock   ";break;
    case T_H_CORE  : cout<<" H.Core   ";break;
    case T_POP     : cout<<" Pop      ";break;
    default:         cout<<" nieznany ";
   }
}

void KASETA :: WriteKType(int par,FILE* p)
{
 switch(k_typ[par])
 	{
    case T_H_METAL : fprintf(p," H.Metal  ");break;
    case T_T_METAL : fprintf(p," T.Metal  ");break;
    case T_D_METAL : fprintf(p," D.Metal  ");break;
    case T_P_METAL : fprintf(p," P.Metal  ");break;
    case T_ROCK    : fprintf(p," Rock     ");break;
    case T_H_ROCK  : fprintf(p," H.Rock   ");break;
    case T_H_CORE  : fprintf(p," H.Core   ");break;
    case T_POP     : fprintf(p," Pop      ");break;
    default:         fprintf(p," nieznany ");
   }
}

void KASETA :: DisplayTType(int par)
{
 switch(t_typ[par])
 	{
    case T_WYWIAD   : cout<<" Wywiad   ";break;
    case T_LIVE     : cout<<" Live!    ";break;
    case T_TELEDYSK : cout<<" Teledysk ";break;
    default:          cout<<" nieznany ";
   }
}


void KASETA :: WriteTType(int par,FILE* p)
{
 switch(t_typ[par])
 	{
    case T_WYWIAD   : fprintf(p," Wywiad   ");break;
    case T_LIVE     : fprintf(p," Live!    ");break;
    case T_TELEDYSK : fprintf(p," Teledysk ");break;
    default:          fprintf(p," nieznany ");
   }
}

void KASETA :: DisplayFType(int par)
{
 switch(f_typ[par])
 	{
    case T_DOKUMENT : cout<<" Dokument ";break;
    case T_SENSACJA : cout<<" Sensacja ";break;
    case T_WESTERN  : cout<<" Western  ";break;
    case T_OBYCZAJ  : cout<<" Obyczaj  ";break;
    case T_HORROR   : cout<<" Horror   ";break;
    case T_KOMEDIA  : cout<<" Komedia  ";break;
    case T_DRAMAT   : cout<<" Dramat   ";break;
    default:         cout<<" nieznany ";
   }
}

void KASETA :: WriteFType(int par,FILE* p)
{
 switch(f_typ[par])
 	{
    case T_DOKUMENT : fprintf(p," Dokument ");break;
    case T_SENSACJA : fprintf(p," Sensacja ");break;
    case T_WESTERN  : fprintf(p," Western  ");break;
    case T_OBYCZAJ  : fprintf(p," Obyczaj  ");break;
    case T_HORROR   : fprintf(p," Horror   ");break;
    case T_KOMEDIA  : fprintf(p," Komedia  ");break;
    case T_DRAMAT   : fprintf(p," Dramat   ");break;
    default:         fprintf(p," nieznany ");
   }
}

void KASETA :: DisplayKOrg(int par)
{
 switch(k_org[par])
 	{
    case T_ORGINAL : cout<<" Orginal";break;
    case T_KOPIA   : cout<<" Kopia  ";break;
    default:         cout<<" nieznany";
   }
}

void KASETA :: WriteKOrg(int par,FILE* p)
{
 switch(k_org[par])
 	{
    case T_ORGINAL : fprintf(p," Orginal");break;
    case T_KOPIA   : fprintf(p," Kopia  ");break;
    default:         fprintf(p," nieznany");
   }
}

void KASETA :: DisplayFOrg(int par)
{
 switch(f_org[par])
 	{
    case T_ORGINAL : cout<<" Orginal";break;
    case T_KOPIA   : cout<<" Kopia  ";break;
    default:         cout<<" nieznany";
   }
}

void KASETA :: WriteFOrg(int par,FILE* p)
{
 switch(f_org[par])
 	{
    case T_ORGINAL : fprintf(p," Orginal");break;
    case T_KOPIA   : fprintf(p," Kopia  ");break;
    default:         fprintf(p," nieznany");
   }
}


void KASETA :: ShowFilms()
{
 for (int i=0;i<ilosc_filmow;i++)
 	{
    printf("%-2d.)%-16s ",i,f_tytul[i]);
    DisplayFType(i);
    DisplayFOrg(i);
    ln();
    if ((!(i % 15)) && (i != 0))
    	{
       cout<<"Nacisnij cokolwiek aby kontynuowac...\n";
       getch();
       ln();
      }
   }
}

void KASETA :: ShowLives()
{
 for (int i=0;i<ilosc_teledyskow;i++)
 	{
    printf("%-2d.)%-16s %-16s ",i,t_zespol[i],t_tytul[i]);
    DisplayTType(i);
    ln();
    if ((!(i % 15)) && (i != 0))
    	{
       cout<<"Nacisnij cokolwiek aby kontynuowac...\n";
       getch();
       ln();
      }
   }
}


void KASETA :: ShowConcerts()
{
 for (int i=0;i<ilosc_koncertow;i++)
 	{
    printf("%-2d.)%-16s %-16s",i,k_zespol[i],k_tytul[i]);
    DisplayKType(i);
    DisplayKOrg(i);
    ln();
    if ((!(i % 15)) && (i != 0))
    	{
       cout<<"Nacisnij cokolwiek aby kontynuowac...\n";
       getch();
       ln();
      }
   }
}

void KASETA :: ModKonc()
{
 ShowConcerts();
 cout<<"Podaj numer koncertu (-1 oznacza dodaj): ";
 int lb;
 takeint(&lb);
 if ((lb < -1) || (lb > ilosc_koncertow -1))
 	{
    cout<<"Poza zakresem tabeli...\n";
    return ;
   }
 cout<<"Zmodyfikowac, usunac czy dodac nowy(z/u/d): ";
 char zn;
 takechar(&zn);
 ln();
 if (zn == 'z') ModifyKonc(lb);
 else if (zn == 'u') DelKonc(lb);
 else if (zn == 'd') {AddKonc();}
 else cout<<"Nieznane polecenie.\n";
}

void KASETA :: ModifyKonc(int par)
{
 cout<<"Podaj nazwe zespolu: ";
 takestring(k_zespol[par]);
 cout<<"Podaj nazwe koncertu numer "<<par<<" z kasety "<<nazwa<<": ";
 takestring(k_tytul[par]);
 OrgKoncertu(par);
 TypKoncertu(par);
}

void KASETA :: ModifyTele(int par)
{
 cout<<"Podaj nazwe zespolu: ";
 takestring(t_zespol[par]);
 cout<<"Podaj nazwe teledysku numer "<<par<<" z kasety "<<nazwa<<": ";
 takestring(t_tytul[par]);
 TypTeledysku(par);
}

void KASETA :: ModifyFilm(int par)
{
 cout<<"Podaj tytul filmu numer "<<par<<" z kasety "<<nazwa<<": ";
 takestring(f_tytul[par]);
 OrgFilmu(par);
 TypFilmu(par);
}

void KASETA :: DelKonc(int par)
{
 ilosc_koncertow--;
 for (int i=par;i<ilosc_koncertow;i++)
 	{
    strcpy(k_tytul[i],k_tytul[i+1]);
    strcpy(k_zespol[i],k_zespol[i+1]);
    k_typ[i] = k_typ[i+1];
    k_org[i] = k_org[i+1];
   }
}

void KASETA :: DelTele(int par)
{
 ilosc_teledyskow--;
 for (int i=par;i<ilosc_teledyskow;i++)
 	{
    strcpy(t_tytul[i],t_tytul[i+1]);
    strcpy(t_zespol[i],t_zespol[i+1]);
    t_typ[i] = t_typ[i+1];
   }
}

void KASETA :: DelFilm(int par)
{
 ilosc_filmow--;
 for (int i=par;i<ilosc_filmow;i++)
 	{
    strcpy(f_tytul[i],f_tytul[i+1]);
    f_typ[i] = f_typ[i+1];
    f_org[i] = f_org[i+1];
   }
}

void KASETA :: ModFilm()
{
 ShowFilms();
 cout<<"Podaj numer filmu (-1 oznacza dodaj): ";
 int lb;
 takeint(&lb);
 if ((lb < -1) || (lb > ilosc_filmow -1))
 	{
    cout<<"Poza zakresem tabeli...\n";
    return ;
   }
 cout<<"Zmodyfikowac, usunac czy dodac nowy(z/u/d): ";
 char zn;
 takechar(&zn);
 ln();
 if (zn == 'z') ModifyFilm(lb);
 else if (zn == 'u') DelFilm(lb);
 else if (zn == 'd') {AddFilm();}
 else cout<<"Nieznane polecenie.\n";
}

void KASETA :: ModTele()
{
 ShowLives();
 cout<<"Podaj numer teledysku (-1 oznacza dodaj): ";
 int lb;
 takeint(&lb);
 if ((lb < -1) || (lb > ilosc_teledyskow -1))
 	{
    cout<<"Poza zakresem tabeli...\n";
    return ;
   }
 cout<<"Zmodyfikowac, usunac czy dodac nowy(z/u/d): ";
 char zn;
 takechar(&zn);
 ln();
 if (zn == 'z') ModifyTele(lb);
 else if (zn == 'u') DelTele(lb);
 else if (zn == 'd') {AddTele();}
 else cout<<"Nieznane polecenie.\n";
}

void KASETA :: ModName()
{
 cout<<"Podaj nowa nazwe kasety: ";
 takestring(nazwa);
}

void KASETA :: AddKonc()
{
 start:
 if (ilosc_koncertow == MAX_KONC)
 	{
    cout<<"Niestety maksymalna ilosc koncertow dla tej kasety ("<<MAX_KONC<<")\n";
    return;
   }
 cout<<"Aktualnie koncertow "<<ilosc_koncertow<<endl;
 cout<<"Podaj nazwe zespolu: ";
 takestring(k_zespol[ilosc_koncertow]);
 cout<<"Podaj tytul koncertu: ";
 takestring(k_tytul[ilosc_koncertow]);
 OrgKoncertu();
 TypKoncertu();
 ilosc_koncertow++;
 cout<<"Dodac nastepny koncert? ";
 char zn;
 takechar(&zn);
 ln();
 if (zn == 't') goto start;
}

void KASETA :: OrgKoncertu(int ilosc_koncerto)
{
 cout<<"Oryginal / Kopia\n";
 cout<<"1\t\tOryginal\n";
 cout<<"2\t\tKopia\n";
 cout<<"Podaj liczbe: ";
 int lb;
 takeint(&lb);
 if (lb == 1) k_org[ilosc_koncerto] = T_ORGINAL;
 else         k_org[ilosc_koncerto] = T_KOPIA;
}

void KASETA :: TypKoncertu(int ilosc_koncerto)
{
 cout<<"Wybierz typ koncertu\n";
 cout<<"1\t\tHeavy Metal\n";
 cout<<"2\t\tThrash Metal\n";
 cout<<"3\t\tDeath Metal\n";
 cout<<"4\t\tPower Metal\n";
 cout<<"5\t\tRock\n";
 cout<<"6\t\tHard Rock\n";
 cout<<"7\t\tHard Core\n";
 cout<<"8\t\tPop\n";
 cout<<"9\t\tInny\n";
 cout<<"Podaj liczbe: ";
 int lb;
 takeint(&lb);
 k_typ[ilosc_koncerto] = T_INNY;
 if (lb == 1) k_typ[ilosc_koncerto] = T_H_METAL;
 if (lb == 2) k_typ[ilosc_koncerto] = T_T_METAL;
 if (lb == 3) k_typ[ilosc_koncerto] = T_D_METAL;
 if (lb == 4) k_typ[ilosc_koncerto] = T_P_METAL;
 if (lb == 5) k_typ[ilosc_koncerto] = T_ROCK;
 if (lb == 6) k_typ[ilosc_koncerto] = T_H_ROCK;
 if (lb == 7) k_typ[ilosc_koncerto] = T_H_CORE;
 if (lb == 8) k_typ[ilosc_koncerto] = T_POP;
}

void KASETA :: OrgKoncertu()
{
 cout<<"Oryginal / Kopia\n";
 cout<<"1\t\tOryginal\n";
 cout<<"2\t\tKopia\n";
 cout<<"Podaj liczbe: ";
 int lb;
 takeint(&lb);
 if (lb == 1) k_org[ilosc_koncertow] = T_ORGINAL;
 else         k_org[ilosc_koncertow] = T_KOPIA;
}

void KASETA :: TypKoncertu()
{
 cout<<"Wybierz typ koncertu\n";
 cout<<"1\t\tHeavy Metal\n";
 cout<<"2\t\tThrash Metal\n";
 cout<<"3\t\tDeath Metal\n";
 cout<<"4\t\tPower Metal\n";
 cout<<"5\t\tRock\n";
 cout<<"6\t\tHard Rock\n";
 cout<<"7\t\tHard Core\n";
 cout<<"8\t\tPop\n";
 cout<<"9\t\tInny\n";
 cout<<"Podaj liczbe: ";
 int lb;
 takeint(&lb);
 k_typ[ilosc_koncertow] = T_INNY;
 if (lb == 1) k_typ[ilosc_koncertow] = T_H_METAL;
 if (lb == 2) k_typ[ilosc_koncertow] = T_T_METAL;
 if (lb == 3) k_typ[ilosc_koncertow] = T_D_METAL;
 if (lb == 4) k_typ[ilosc_koncertow] = T_P_METAL;
 if (lb == 5) k_typ[ilosc_koncertow] = T_ROCK;
 if (lb == 6) k_typ[ilosc_koncertow] = T_H_ROCK;
 if (lb == 7) k_typ[ilosc_koncertow] = T_H_CORE;
 if (lb == 8) k_typ[ilosc_koncertow] = T_POP;
}


void KASETA :: TypTeledysku()
{
 cout<<"Wybierz typ teledysku\n";
 cout<<"1\t\tWywiad\n";
 cout<<"2\t\tLive\n";
 cout<<"3\t\tTeledysk\n";
 cout<<"Podaj liczbe: ";
 int lb;
 takeint(&lb);
 t_typ[ilosc_teledyskow] = T_INNY;
 if (lb == 1) t_typ[ilosc_teledyskow] = T_WYWIAD;
 if (lb == 2) t_typ[ilosc_teledyskow] = T_LIVE;
 if (lb == 3) t_typ[ilosc_teledyskow] = T_TELEDYSK;
}

void KASETA :: TypTeledysku(int ilosc_teledysko)
{
 cout<<"Wybierz typ teledysku\n";
 cout<<"1\t\tWywiad\n";
 cout<<"2\t\tLive\n";
 cout<<"3\t\tTeledysk\n";
 cout<<"Podaj liczbe: ";
 int lb;
 takeint(&lb);
 t_typ[ilosc_teledysko] = T_INNY;
 if (lb == 1) t_typ[ilosc_teledysko] = T_WYWIAD;
 if (lb == 2) t_typ[ilosc_teledysko] = T_LIVE;
 if (lb == 3) t_typ[ilosc_teledysko] = T_TELEDYSK;
}


void KASETA :: AddTele()
{
 start:
 if (ilosc_teledyskow == MAX_TELE)
 	{
    cout<<"Niestety maksymalna ilosc teledyskow dla tej kasety ("<<MAX_TELE<<")\n";
    return;
   }
 cout<<"Aktualnie teledyskow "<<ilosc_koncertow<<endl;
 cout<<"Podaj nazwe zespolu: ";
 takestring(t_zespol[ilosc_teledyskow]);
 cout<<"Podaj tytul teledysku: ";
 takestring(t_tytul[ilosc_teledyskow]);
 TypTeledysku();
 ilosc_teledyskow++;
 cout<<"Dodac nastepny teledysk? ";
 char zn;
 takechar(&zn);
 ln();
 if (zn == 't') goto start;
}

void KASETA :: OrgFilmu()
{
 cout<<"Oryginal / Kopia\n";
 cout<<"1\t\tOryginal\n";
 cout<<"2\t\tKopia\n";
 cout<<"Podaj liczbe: ";
 int lb;
 takeint(&lb);
 if (lb == 1) f_org[ilosc_filmow] = T_ORGINAL;
 else         f_org[ilosc_filmow] = T_KOPIA;
}

void KASETA :: OrgFilmu(int ilosc_filmo)
{
 cout<<"Oryginal / Kopia\n";
 cout<<"1\t\tOryginal\n";
 cout<<"2\t\tKopia\n";
 cout<<"Podaj liczbe: ";
 int lb;
 takeint(&lb);
 if (lb == 1) f_org[ilosc_filmo] = T_ORGINAL;
 else         f_org[ilosc_filmo] = T_KOPIA;
}

void KASETA :: TypFilmu()
{
 cout<<"Wybierz typ filmu\n";
 cout<<"1\t\tDokument\n";
 cout<<"2\t\tSensacja\n";
 cout<<"3\t\tWestern\n";
 cout<<"4\t\tObyczaj\n";
 cout<<"5\t\tHorror\n";
 cout<<"6\t\tKomedia\n";
 cout<<"7\t\tDramat\n";
 cout<<"Podaj liczbe: ";
 int lb;
 takeint(&lb);
 f_typ[ilosc_filmow] = T_INNY;
 if (lb == 1) f_typ[ilosc_filmow] = T_DOKUMENT;
 if (lb == 2) f_typ[ilosc_filmow] = T_SENSACJA;
 if (lb == 3) f_typ[ilosc_filmow] = T_WESTERN;
 if (lb == 4) f_typ[ilosc_filmow] = T_OBYCZAJ;
 if (lb == 5) f_typ[ilosc_filmow] = T_HORROR;
 if (lb == 6) f_typ[ilosc_filmow] = T_KOMEDIA;
 if (lb == 7) f_typ[ilosc_filmow] = T_DRAMAT;
}

void KASETA :: TypFilmu(int ilosc_filmo)
{
 cout<<"Wybierz typ filmu\n";
 cout<<"1\t\tDokument\n";
 cout<<"2\t\tSensacja\n";
 cout<<"3\t\tWestern\n";
 cout<<"4\t\tObyczaj\n";
 cout<<"5\t\tHorror\n";
 cout<<"6\t\tKomedia\n";
 cout<<"7\t\tDramat\n";
 cout<<"Podaj liczbe: ";
 int lb;
 takeint(&lb);
 f_typ[ilosc_filmo] = T_INNY;
 if (lb == 1) f_typ[ilosc_filmo] = T_DOKUMENT;
 if (lb == 2) f_typ[ilosc_filmo] = T_SENSACJA;
 if (lb == 3) f_typ[ilosc_filmo] = T_WESTERN;
 if (lb == 4) f_typ[ilosc_filmo] = T_OBYCZAJ;
 if (lb == 5) f_typ[ilosc_filmo] = T_HORROR;
 if (lb == 6) f_typ[ilosc_filmo] = T_KOMEDIA;
 if (lb == 7) f_typ[ilosc_filmo] = T_DRAMAT;
}

void KASETA :: AddFilm()
{
 start:
 if (ilosc_filmow == MAX_FILM)
 	{
    cout<<"Niestety maksymalna ilosc filmow dla tej kasety ("<<MAX_FILM<<")\n";
    return;
   }
 cout<<"Aktualnie filmow "<<ilosc_filmow<<endl;
 cout<<"Podaj tytul filmu: ";
 takestring(f_tytul[ilosc_filmow]);
 OrgFilmu();
 TypFilmu();
 ilosc_filmow++;
 cout<<"Dodac nastepny film? ";
 char zn;
 takechar(&zn);
 ln();
 if (zn == 't') goto start;

}

void KASETA :: dies()
{
 numer = 0;
 strcpy(nazwa,"-");
 ilosc_teledyskow = 0;
 ilosc_filmow = 0;
 ilosc_koncertow = 0;
 cout<<"Reorganizacja pamieci wirtualnej\t[OK]\n";
}
void KASETA :: save()
{
 if (strcmp(naz,"NIEZIDENTYFIKOWANA_KASETA") == 0)
 	{
    cout<<"Stworz najpierw cokolwiek lub wczytaj zapis...\n";
    return;
   }
 numer = cur;
 char snum[6];
 sprintf(snum,"_%x",numer);
 char file[STRING];
 strcpy(file,dir);
 strcat(file,naz);
 strcat(file,snum);
 FILE* plik;
 if ((plik = fopen(file,"w+")) == NULL)
 	{
    cout<<"Straszny blad, mianowicie zapis do "<<file<<" niemozliwy!\n";
    return ;
   }
 fprintf(plik,"Numer %d\n",numer);
 fprintf(plik,"Nazwa %s\n",nazwa);
 fprintf(plik,"Filmow: %d\n",ilosc_filmow);
 fprintf(plik,"Teledyskow: %d\n",ilosc_teledyskow);
 fprintf(plik,"Koncertow: %d\n",ilosc_koncertow);
 for (int i=0;i<ilosc_teledyskow;i++)
 	{
    fprintf(plik,"Teledysk numer %d zespol %s %s typ 0x%x\n",i,t_zespol[i],t_tytul[i],t_typ[i]);
   }
 for (int i=0;i<ilosc_filmow;i++)
 	{
    fprintf(plik,"Film numer %d %s typ 0x%x 0x%x\n",i,f_tytul[i],f_typ[i],f_org[i]);
   }
 for (int i=0;i<ilosc_koncertow;i++)
 	{
    fprintf(plik,"Koncert numer %d zespol %s %s typ 0x%x 0x%x\n",i,k_zespol[i],k_tytul[i],k_typ[i],k_org[i]);
   }
 fclose(plik);
}

void KASETA :: clean()
{
 numer = cur;
 char snum[6];
 sprintf(snum,"_%x",numer);
 char file[STRING];
 strcpy(file,dir);
 strcat(file,naz);
 strcat(file,snum);
 unlink(file);
}

void KASETA :: write(FILE* plik)
{
 fprintf(plik,"\t\tKaseta numer %d \" %s \"\n",cur,nazwa);
 fprintf(plik,"Filmow %d, koncertow %d, teledyskow %d\n",ilosc_filmow,ilosc_koncertow,ilosc_teledyskow);
 if (ilosc_filmow != 0)fprintf(plik,"\tFilmy:\n");
 for (int i=0;i<ilosc_filmow;i++)
 	{
    fprintf(plik,"%d.) %-16s ",i,f_tytul[i]);
    WriteFType(i,plik);
    WriteFOrg(i,plik);
    lnf(plik);
   }
 if (ilosc_koncertow != 0)fprintf(plik,"\tKoncerty:\n");
 for (int i=0;i<ilosc_koncertow;i++)
 	{
    fprintf(plik,"%d.) %-16s %-16s",i,k_zespol[i],k_tytul[i]);
    WriteKType(i,plik);
    WriteKOrg(i,plik);
    lnf(plik);
   }
 if (ilosc_teledyskow != 0)fprintf(plik,"\tTeledyski:\n");
 for (int i=0;i<ilosc_teledyskow;i++)
 	{
    fprintf(plik,"%d.) %-16s %-16s",i,t_zespol[i],t_tytul[i]);
    WriteTType(i,plik);
    lnf(plik);
   }
 for (int i=0;i<4;i++)lnf(plik);
}

void KASETA :: load()
{
 numer = cur;
 char snum[6];
 sprintf(snum,"_%x",numer);
 char file[STRING];
 strcpy(file,dir);
 strcat(file,naz);
 strcat(file,snum);
 FILE* plik;
 if ((plik = fopen(file,"r+")) == NULL)
 	{
    cout<<"Straszny blad, mianowicie odczyt z "<<file<<" niemozliwy!\n";
    cout<<"Stworzyc ten plik? ";
    char zn;
    takechar(&zn);
    ln();
    if (zn == 't')
    	{
       if ((plik = fopen(file,"w+")) != NULL)
 			{
          cout<<"Plik "<<file<<" utworzony...\n";
          fclose(plik);
         }
       else
       	{
          cout<<"Nie mozna utworzyc "<<file<<endl;
         }
      }
    return ;
   }
 fscanf(plik,"Numer %d\n",&numer);
 fscanf(plik,"Nazwa %s\n",nazwa);
 fscanf(plik,"Filmow: %d\n",&ilosc_filmow);
 fscanf(plik,"Teledyskow: %d\n",&ilosc_teledyskow);
 fscanf(plik,"Koncertow: %d\n",&ilosc_koncertow);
 int usl;
 for (int i=0;i<ilosc_teledyskow;i++)
 	{
    fscanf(plik,"Teledysk numer %d zespol %s %s typ 0x%x\n",&usl,t_zespol[i],t_tytul[i],&t_typ[i]);
   }
 for (int i=0;i<ilosc_filmow;i++)
 	{
    fscanf(plik,"Film numer %d %s typ 0x%x 0x%x\n",&usl,f_tytul[i],&f_typ[i],&f_org[i]);
   }
 for (int i=0;i<ilosc_koncertow;i++)
 	{
    fscanf(plik,"Koncert numer %d zespol %s %s typ 0x%x 0x%x\n",&usl,k_zespol[i],k_tytul[i],&k_typ[i],&k_org[i]);
   }
 fclose(plik);
}

void KASETA :: SearchFilm()
{
 cout<<"Podaj ciag znakow: ";
 char str[STRING];
 takestring(str);
 downcase(str);
 cout<<"Poszukam ciagu: "<<str<<endl;
 int isOK = 0;
 int find = 0;
 for (int i=0;i<all;i++)
 	{
    cur = i;
    load();
    for (int j=0;j<ilosc_filmow;j++)
    	{
       isOK = 0;
       for (int k=0;k<strlen(str);k++)
       	{
          if ((str[k] == f_tytul[j][k]) || (str[k] == f_tytul[j][k] + 0x20))
          	{
             isOK++;
            }
          if (isOK == strlen(str))
          	{
             find++;
          	 printf("%d) %-16s ",j,f_tytul[j]);
             DisplayFType(j);
             DisplayFOrg(j);
             cout<<" Na kasecie "<<i<<") "<<nazwa<<endl;
             stop(&find);
            }
         }
      }
   }
 cur = all;
}

void KASETA :: SearchKoncert()
{
 cout<<"Podaj ciag znakow: ";
 char str[STRING];
 takestring(str);
 downcase(str);
 cout<<"Poszukam ciagu: "<<str<<endl;
 int isOK = 0;
 int find = 0;
 for (int i=0;i<all;i++)
 	{
    cur = i;
    load();
    for (int j=0;j<ilosc_koncertow;j++)
    	{
       isOK = 0;
       for (int k=0;k<strlen(str);k++)
       	{
          if (((str[k] == k_tytul[j][k]) || (str[k] == k_tytul[j][k] + 0x20)) || ((str[k] == k_zespol[j][k]) || (str[k] == k_zespol[j][k] + 0x20)))
          	{
             isOK++;
            }
          if (isOK == strlen(str))
          	{
             find++;
          	 printf("%d) %-16s %-16s ",j,k_zespol[j],k_tytul[j]);
             DisplayKType(j);
             DisplayKOrg(j);
             cout<<" Na kasecie "<<i<<") "<<nazwa<<endl;
             stop(&find);
            }
         }
      }
   }
 cur = all;
}

void KASETA :: SearchLive()
{
 cout<<"Podaj ciag znakow: ";
 char str[STRING];
 takestring(str);
 downcase(str);
 cout<<"Poszukam ciagu: "<<str<<endl;
 int isOK = 0;
 int find = 0;
 for (int i=0;i<all;i++)
 	{
    cur = i;
    load();
    for (int j=0;j<ilosc_teledyskow;j++)
    	{
       isOK = 0;
       for (int k=0;k<strlen(str);k++)
       	{
          if (((str[k] == t_tytul[j][k]) || (str[k] == t_tytul[j][k] + 0x20)) || ((str[k] == t_zespol[j][k]) || (str[k] == t_zespol[j][k] + 0x20)))
          	{
             isOK++;
            }
          if (isOK == strlen(str))
          	{
             find++;
          	 printf("%d) %-16s %-16s ",j,t_zespol[j],t_tytul[j]);
             DisplayTType(j);
             cout<<" Na kasecie "<<i<<") "<<nazwa<<endl;
             stop(&find);
            }
         }
      }
   }
 cur = all;
}


void EndWin(void);
void Help(void);
void Apply(KASETA*);
void ExecuteCmd(char*, KASETA*);
void Run(KASETA*);
void Add(KASETA*);
void SaveConf(void);
void ShowCasettes(void);
void Mod(KASETA*);
void DeleteRec(KASETA*);
void ShowDirect(KASETA*);
void ShowAll(KASETA*);
void ShowAllFilms(KASETA*);
void ShowAllConcerts(KASETA*);
void ShowAllLives(KASETA*);
void See(KASETA*);
void Generate(KASETA*);
void LoadConf(void);

void Search09(KASETA* current)
{
 if (all == 0)
 	{
    cout<<"Rekord danych jest pusty, nie mozna nic znalezc!\n";
    return ;
   }
 cout<<"Wybierz opcje:\n";
 cout<<"1\t\t-szukaj filmu\n";
 cout<<"2\t\t-szukaj koncertu (zespol lub tytul)\n";
 cout<<"3\t\t-szukaj teledysku (zespol lub tytul)\n";
 cout<<"Twoj wybor: ";
 int lb;
 takeint(&lb);
 switch(lb)
 	{
    case 1 : current->SearchFilm();break;
    case 2 : current->SearchKoncert();break;
    case 3 : current->SearchLive();break;
    default: cout<<"Nieznane polecenie..\n";

   }
}
void EndWin()
{
 cout<<"Nacisnij cokolwiek aby zakonczyc...\n";
 getch();
}

void Generate(KASETA* current)
{
 if (strcmp(naz,"NIEZIDENTYFIKOWANA_KASETA") == 0)
 	{
    cout<<"Stworz lub wczytaj kolekcje najpierw...\n";
    return;
   }
 char txt[STRING];
 strcpy(txt,naz);
 strcat(txt,".txt");
 cout<<"Zapisze plik tekstowy "<<txt<<endl;
 FILE* plik;
 if ((plik = fopen(txt,"w+")) == NULL)
 	{
    cout<<"Nie moge zapisac danych w "<<txt<<endl;
    return;
   }
 fprintf(plik,"Plik tekstowy wygenerowany przez aplikacje\n");
 fprintf(plik,"Autorstwa Morgothdbma, copyleft@ OpenSource Linux\n\n\n");
 for (int i=0;i<all;i++)
 	{
    cur = i;
    current->load();
    current->write(plik);
   }
 fclose(plik);
 cur = all;
}

void Help()
{
 cout<<"h\t\t-pomoc\n";
 cout<<"d\t\t-dodaj kasete\n";
 cout<<"s\t\t-zapisz dane\n";
 cout<<"l\t\t-wczytaj dane\n";
 cout<<"m\t\t-modyfikuj kasete\n";
 cout<<"u\t\t-usun kasete\n";
 cout<<"z\t\t-zobacz\n";
 cout<<"g\t\t-generuj plik tekstowy\n";
 cout<<"w\t\t-uruchom wyszukiwarke v0.9\n";
}

void ShowCasettes()
{
 FILE* data;
 char name[STRING];
 char lbs[6];
 int err = 0;
 int nume = 0;
 char nazw[STRING];
 strcpy(nazw,"LD Error");
 for (int i=0;i<all;i++)
 	{
    strcpy(name,dir);
    strcat(name,naz);
    sprintf(lbs,"_%x",i);
    strcat(name,lbs);
    if ((data = fopen(name,"r+")) == NULL) err++;
    else
    	{
	    fscanf(data,"Numer %d\n",&nume);
	    fscanf(data,"Nazwa %s\n",nazw);
       fclose(data);
      }
    printf("(%3d)%-10s\t",i,nazw);
    if ((!(i % 3)) && (i != 0)) ln();
    if ((!(i % 45)) && (i != 0)) {cout<<"Nacisnij cokolwiek aby kontynuowac...\n";getch();ln();};
   }
 if (err != 0)
   {
    cout<<"\nNie powiodlo sie otworzenie "<<err<<" plikow z "<<all<<endl;
   }
 ln();
}

void Mod(KASETA* current)
{
 if (all == 0)
 	{
    cout<<"Nie ma nic do modyfikacji.\n";
    return;
   }
 ShowCasettes();
 cout<<"Podaj numer kasety do modyfikacji: ";
 int num;
 takeint(&num);
 if ((num < 0) || (num > all-1))
 	{
    cout<<"Poza zakresem tabeli...\n";
    return;
   }
 cur = num;
 current->load();
 current->modify();
 cout<<"Zapisac zmiany w "<<current->nazwa<<"? ";
 char zn;
 takechar(&zn);
 ln();
 if (zn == 't') current->save();
 cur = all;
}

void LoadConf()
{
 FILE* plik;
 char conf[STRING];
 cout<<"Podaj nazwe pliku: ";
 takestring(conf,5);
 strcat(conf,".dat");
 if ((plik = fopen(conf,"r+")) == NULL)
 	{
    cout<<"Odczyt pliku "<<conf<<" niemozliwy!\n";
    cout<<"Odczyt konfiguracji\t\t\t[FAILED]\n";
    cout<<"Chcesz recznie podac parametry? ";
    char zn;
    takechar(&zn);
    ln();
    if (zn == 't')
    	{
       cout<<"Podaj rdzen nazwy: ";
       takestring(naz,5);
       cout<<"Podaj ilosc kaset: ";
       takeint(&all);
      }
    cur = all;
    return ;
   }
 fscanf(plik,"Conf is %d with %s\n",&all,naz);
 cout<<"Odczyt konfiguracji\t\t\t[OK]\n";
 cout<<"Ilosc kaset to "<<all<<" aktualna "<<all<<"\n";
 cur = all;
 fclose(plik);

}

void Add(KASETA* current)
{
 if (strcmp(naz,"NIEZIDENTYFIKOWANA_KASETA") == 0)
 	{
    cout<<"Podaj nazwe kolekcji kaset: ";
    takestring(naz,5);
   }
 cout<<"Podaj nazwe kasety: ";
 takestring(current->nazwa);
 current->add();
 current->save();
 all++;
 cur++;
 current->dies();
}

void DeleteRec(KASETA* current)
{
 if (all == 0)
 	{
    cout<<"Brak kaset do usuniecia\n";
    return;
   }
 ShowCasettes();
 cout<<"Podaj numer kasety do usuniecia: ";
 int lb;
 takeint(&lb);
 if ((lb <0) || (lb > all - 1))
 	{
    cout<<"Poza zakresem wirtualnej tabeli...\n";
   }
 all--;
 for (int i=lb;i<all;i++)
 	{
    cur = i+1;
    current->load();
    current->numer--;
    cur--;
    current->save();
   }
 cur = all;
 current->clean();
}

void ShowDirect(KASETA* current)
{
 ShowCasettes();
 cout<<"Podaj numer kasety: ";
 int lb;
 takeint(&lb);
 if ((lb < 0) || (lb > all - 1))
 	{
    cout<<"Poza zakresem tabeli...\n";
    return;
   }
 cur = lb;
 current->load();
 current->show();
 cur = all;
 ln();
}

void ShowAllFilms(KASETA* current)
{
 if (all == 0) {cout<<"Nie ma nic do pokazania...\n";return;}
 int k = 0;
 char pass = 'x';
 for (int i=0;i<all;i++)
 	{
    cur = i;
    current->load();
    for (int j=0;j<current->ilosc_filmow;j++)
    	{
       k++;
       printf("%-2d.)%-16s ",j,current->f_tytul[j]);
       current->DisplayFType(j);
       current->DisplayFOrg(j);
       cout<<" kaseta:  ["<<i<<"] "<<current->nazwa<<endl;
       if ((!(k % 12)) && (k != 0))
       	{
          cout<<"Nacisnij cokolwiek aby kontynuowac...(lub p aby pominac) ";
          if (pass != 'p') pass = getch();
          ln();
         }
      }
   }
 cur = all;
}

void ShowAllConcerts(KASETA* current)
{
 if (all == 0) {cout<<"Nie ma nic do pokazania...\n";return;}
 int k = 0;
 char pass = 'x';
 for (int i=0;i<all;i++)
 	{
    cur = i;
    current->load();
    for (int j=0;j<current->ilosc_koncertow;j++)
    	{
       k++;
       printf("%-2d.)%-16s %-16s ",j,current->k_zespol[j],current->k_tytul[j]);
       current->DisplayKType(j);
       current->DisplayKOrg(j);
       cout<<" kaseta:  ["<<i<<"] "<<current->nazwa<<endl;
       if ((!(k % 12)) && (k != 0))
       	{
          cout<<"Nacisnij cokolwiek aby kontynuowac...(lub p aby pominac) ";
          if (pass != 'p') pass = getch();
          ln();
         }
      }
   }
 cur = all;
}

void ShowAllLives(KASETA* current)
{
 if (all == 0) {cout<<"Nie ma nic do pokazania...\n";return;}
 int k = 0;
 char pass = 'x';
 for (int i=0;i<all;i++)
 	{
    cur = i;
    current->load();
    for (int j=0;j<current->ilosc_teledyskow;j++)
    	{
       k++;
       printf("%-2d.)%-16s %-16s ",j,current->t_zespol[j],current->t_tytul[j]);
       current->DisplayTType(j);
       cout<<" kaseta:  ["<<i<<"] "<<current->nazwa<<endl;
       if ((!(k % 12)) && (k != 0))
       	{
          cout<<"Nacisnij cokolwiek aby kontynuowac...(lub p aby pominac) ";
          if (pass != 'p') pass = getch();
          ln();
         }
      }
   }
 cur = all;
}

void ShowAll(KASETA* current)
{
 if (all == 0) {cout<<"Nie ma nic do pokazania...\n";return;}
 cout<<"Filmy:\n";
 ShowAllFilms(current);
 getch();
 cout<<"Koncerty:\n";
 ShowAllConcerts(current);
 getch();
 cout<<"Teledyski:\n";
 ShowAllLives(current);
 getch();
}

void See(KASETA* current)
{
 if (all == 0)
 	{
    cout<<"Slayer:\tnothing to see where the sleeping souls lie!!!\n";
   }
 cout<<"Wybierz jedno z ponizszych.\n";
 cout<<"\n1\t\t-zobacz nazwy kaset\n";
 cout<<"2\t\t-zobacz konkretna kasete\n";
 cout<<"3\t\t-zobacz wszystkie filmy\n";
 cout<<"4\t\t-zobacz wszystkie koncerty\n";
 cout<<"5\t\t-zobacz wszystkie teledyski\n";
 cout<<"6\t\t-zobacz wszystko (!!!)\n";
 int lb;
 cout<<"\nTwoj wybor: ";
 takeint(&lb);
 switch(lb)
 	{
    case 1 : ShowCasettes();break;
    case 2 : ShowDirect(current);break;
    case 3 : ShowAllFilms(current);break;
    case 4 : ShowAllConcerts(current);break;
    case 5 : ShowAllLives(current);break;
    case 6 : ShowAll(current);break;
    default : cout<<"Nieznane polecenie.\n";
   }
 cur = all;
}

void SaveConf()
{
 if (strcmp(naz,"NIEZIDENTYFIKOWANA_KASETA") == 0)
 	{
    cout<<"Zapisz lub wczytaj dane najpierw...\n";
    cout<<"Zapis konfiguracji\t\t\t[FAILED]\n";
    return ;
   }
 FILE* plik;
 char conf[STRING];
 strcpy(conf,naz);
 strcat(conf,".dat");
 if ((plik = fopen(conf,"w+")) == NULL)
 	{
    cout<<"Zapis pliku "<<conf<<" niemozliwy!\n";
    return ;
   }
 fprintf(plik,"Conf is %d with %s\n",all,naz);
 cout<<"Zapis konfiguracji\t\t\t[OK]\n";
 fclose(plik);
}

void Apply()
{
 char zn;
 if (strcmp(naz,"NIEZIDENTYFIKOWANA_KASETA") == 0)
 	{
    cout<<"Konfiguracja nie moze byc zapisana, poniewaz nie zdefiniowano\n";
    cout<<"Pliku rdzenia, zdefiniuj go dodajac conajmniej jedna kasete...\n";

    return;
   }
 cout<<"Zapisac plik danych "<<naz<<".dat? ";
 takechar(&zn);
 ln();
 if (zn == 't') SaveConf();
}
void ExecuteCmd(char* cmd, KASETA* current)
{
 if (strcmp(cmd,"h") == 0) Help();
 else if (strcmp(cmd,"k") == 0) Apply();
 else if (strcmp(cmd,"d") == 0) Add(current);
 else if (strcmp(cmd,"m") == 0) Mod(current);
 else if (strcmp(cmd,"s") == 0) SaveConf();
 else if (strcmp(cmd,"l") == 0) LoadConf();
 else if (strcmp(cmd,"u") == 0) DeleteRec(current);
 else if (strcmp(cmd,"z") == 0) See(current);
 else if (strcmp(cmd,"g") == 0) Generate(current);
 else if (strcmp(cmd,"w") == 0) Search09(current);
 else cout<<"Nieznane polecenie, wpisz h\n";
}


void Run(KASETA* current)
{
 current->dies();
 char cmd[STRING];
 strcpy(cmd,"NO_YET");
 while (strcmp(cmd,"k") != 0)
 	{
    printf("Morgothdbma.linux > [%d/%d] %s ->",cur,all,naz);
    takestring(cmd);
    ExecuteCmd(cmd,current);
    current->dies();
   }
}

