#include "definitions.h"
#include "graph_module.h"

const int OK = -1;                  // nie - falsz
const int STRING = 25;              //dlugosc nazw literowych
const int MAX_CECH = 15;            //maksymalnie cech w bazie
const int MAX_RACES = 50;           //maksymalnie ras w bazie
const int MAX_PROF = 100;            //maksymalnie profesji w bazie
const int MAX_UM = 1000;             //maksymalnie umiejetnosci w bazie
const int VISIBLE = 1;              // widzialny - prawda
const int UM_ATTR = 3;              // ilosc atrybutow pojedynczej umiejetnosci
const int TEST_FROM = 0;            //testowana od..
const int MULTIPLE = 1;             //ile razy dodajemy poziom 1 oznacz ze to 1 element tablicy
const int MOD = 2;                  //modyfikator test umiejetnosci 2 w tablicy
typedef unsigned long ULONG;     //typ ULONG 0 - 6*10e4
enum {FALSE, TRUE};                  //boolean wartosci
void KILL() { getch(); exit( -1 );}        // funkcja zabraniajaca dostepu
class App
{                      //glowna klasa bazy danych
 public:               //przechowuje bazy danych w plikach db\xxx.BM
 App();                    //gdzie xxx nazwa bazy danych o ktora pyta program
 ~App();
 void SetCurrentProcess();  //jezeli program ma watki stworz folder db i char    !!!!!!
 void LoadProc();           //wgrywa baze danych
 void SaveProc();           //zapisuje baze danych
 void CheckPassword();      //sprawdza poziom dostepu do bazy danych
 void SetAttributes();      //ustala cechy
 void SetRaces();           //ustala rasy
 void ShowAttributes(int*,int*);     //pokazuje cechy
 void ShowRaces(int*,int*,short);     //pokazuje rasy parametr domyslny -1 lub VISIBLE (tabela szczegolow)
 void ChangeRaces();        //zmienia rasy
 void SetProf();            //ustala profesje
 void ShowProf(int*,int*,short);      //patrz ShowRaces(short);
 void ChangeProf();         //zmienia profesje
 void TryProc();            //proboj wgrac procesor lub go utworz
 void SetSkill();           //ustal umiejetnosc
 void ShowSkill(int*,int*,short);     //analogicznie w.w.
 void ChangeSkill();        //zmienia umiejetnosci
 virtual int Run();         //uruchamia cala baze danych inicjujac siebie
 void WriteFile(FILE*);
 void ReadFile(FILE*);
 void AboutStructWrite();

 char proc[STRING];         //nazwa bazy danych postaci db\xxx.BM
 ULONG pass;                //kod liczbowy strzegacy dostepu  jesli = 0 nie ma kodu
 short il_cech;                   //ilosc zdefiniowanych cech
 char ncechy[MAX_CECH][STRING];   //nazwy cech
 short il_ras;                    //ilosc ras
 char nrasy[MAX_RACES][STRING];   //nazwy ras
 short modr[MAX_RACES][MAX_CECH]; //modyfikatory ras
 short il_prof;                   //ilosc profesji
 char nprof[MAX_PROF][STRING];    //nazwy profesji
 short modp[MAX_PROF][MAX_CECH];  //modyfikatory profesji
 int il_um;                     //ilosc umiejetn
 char n_um[MAX_UM][STRING];
 short um[MAX_UM][UM_ATTR];       //analog.
 int avv;
 int Plus;
 int mL;
 int dL;
// int multi;                //przy tescie ceche mnozymy przez multi domyslnie 0.5
};
void App :: AboutStructWrite()                //zapewnia ze w pliku nie zostana zapisane
{                                             //tz. "biale znaki" gdyz zapis np
 for (int i=0;i<il_cech;i++)                  //"walka mieczem" jest traktowany JAKO 2 STRINGI !!!
	{                                          //                                ~~~~~~~~~~~~~~~~~~
	 if (strcmp(ncechy[i],"")==0)strcpy(ncechy[i],"_");
	 for (int j=0;j<STRING;j++)                //zamienia na walka_mieczem
		{
		 if (ncechy[i][j] ==' ') ncechy[i][j] ='_';
		}
	}
 for (int i=0;i<il_ras;i++)
	{
	 if (strcmp(nrasy[i],"")==0)strcpy(nrasy[i],"_");
	 for (int j=0;j<STRING;j++)
		{
		 if (nrasy[i][j] ==' ') nrasy[i][j] ='_';
		}
	}
 for (int i=0;i<il_prof;i++)
	{
	 if (strcmp(nprof[i],"")==0)strcpy(nprof[i],"_");
	 for (int j=0;j<STRING;j++)
		{
		 if (nprof[i][j] ==' ') nprof[i][j] ='_';
		}
	}
 for (int i=0;i<il_um;i++)
	{
	 if (strcmp(n_um[i],"")==0)strcpy(n_um[i],"_");
	 for (int j=0;j<STRING;j++)
		{
		 if (n_um[i][j] ==' ') n_um[i][j] ='_';
		}
	}
}
void App :: WriteFile(FILE* save)
{
 AboutStructWrite();                               //bardzo wazne !!!!!!!!!!!!!
 fprintf(save,"Base Name: %s\n",proc);
 fprintf(save,"Password: 0x%x\n",pass);
 fprintf(save,"Ilosc Cech: %d\n",il_cech);
 fprintf(save,"Ilosc Ras: %d\n",il_ras);
 fprintf(save,"Ilosc Profesji: %d\n",il_prof);
 fprintf(save,"Ilosc Umiejetnosci: %d\n",il_um);
 for (short i=0;i<il_cech;i++)
	{
	 fprintf(save,"Nazwa cechy %d: %s\n",i,ncechy[i]);
	}
 for (short i=0;i<il_ras;i++)
	{
	 fprintf(save,"Nazwa Rasy %d: %s\n",i,nrasy[i]);
	 for (short j=0;j<il_cech;j++)
		{
		 fprintf(save,"Mod cechy %d: %d\n",j,modr[i][j]);
		}
	}
 for (int i=0;i<il_prof;i++)
	{
	 fprintf(save,"Nazwa profesji %d: %s\n",i,nprof[i]);
	 for (int j=0;j<il_cech;j++)
		{
		 fprintf(save,"Mod cechy %d: %d\n",j,modp[i][j]);
		}
	}
 for (int i=0;i<il_um;i++)
	{
	 fprintf(save,"Nazwa Umiejetnosci %d: %s\n",i,n_um[i]);
	 fprintf(save,"Cecha test: %d\n",um[i][TEST_FROM]);
	 fprintf(save,"Na poz: %d\n",um[i][MULTIPLE]);
	 fprintf(save,"Mod test: %d\n",um[i][MOD]);
	}
fprintf(save,"Start Value: %d\n",avv);
fprintf(save,"Plus: %d\n",Plus);
fprintf(save,"Small Random: %d\n",mL);
fprintf(save,"Large Random: %d\n",dL);
}
void App :: ReadFile(FILE* save)
{
// cout<<"Size of Data Base (bytes in RAM): "<<sizeof(App)<<".\n";getch();
 fscanf(save,"Base Name: %s\n",proc);
 fscanf(save,"Password: 0x%x\n",&pass);
 fscanf(save,"Ilosc Cech: %d\n",&il_cech);
 fscanf(save,"Ilosc Ras: %d\n",&il_ras);
 fscanf(save,"Ilosc Profesji: %d\n",&il_prof);
 fscanf(save,"Ilosc Umiejetnosci: %d\n",&il_um);
 for (short i=0;i<il_cech;i++)
	{
	 fscanf(save,"Nazwa cechy %d: %s\n",&i,ncechy[i]);
	}
 for (short i=0;i<il_ras;i++)
	{
	 fscanf(save,"Nazwa Rasy %d: %s\n",&i,nrasy[i]); //odczytuje stringa wyzej jest zagwarantowane ze
	 for (short j=0;j<il_cech;j++)                        //ow STRING to jeden ciag  np walka_mieczem
		{
		 fscanf(save,"Mod cechy %d: %d\n",&j,&modr[i][j]);
		}
	}
 for (int i=0;i<il_prof;i++)
	{
	 fscanf(save,"Nazwa profesji %d: %s\n",&i,nprof[i]);
	 for (int j=0;j<il_cech;j++)
		{
		 fscanf(save,"Mod cechy %d: %d\n",&j,&modp[i][j]);
		}
	}
 for (int i=0;i<il_um;i++)
	{
	 fscanf(save,"Nazwa Umiejetnosci %d: %s\n",&i,n_um[i]);
	 fscanf(save,"Cecha test: %d\n",&um[i][TEST_FROM]);
	 fscanf(save,"Na poz: %d\n",&um[i][MULTIPLE]);
	 fscanf(save,"Mod test: %d\n",&um[i][MOD]);
	}
fscanf(save,"Start Value: %d\n",&avv);
fscanf(save,"Plus: %d\n",&Plus);
fscanf(save,"Small Random: %d\n",&mL);
fscanf(save,"Large Random: %d\n",&dL);
}
void App :: SetCurrentProcess()
{
 int x,y;
 intro("Wczytywanie bazy danych",&x,&y);
 gprintf(&x,&y,"Wczytam baze danych, nie podawaj rozszerzenia.");
 gprintf(&x,&y,"Podaj nazwe bazy danych: ");
 char nazw[STRING];
 takes(nazw);
 strcpy(proc,"db\\");
 strcat(proc,nazw);                   //szuka w folderze db
 if (strcmp(proc,"db\\")==0) strcpy(proc,"db\\default");
 strcat(proc,".BM");                                   //domyslna nazwa db\default.BM
 gprintf(&x,&y,"Sprobuje wczytac baze danych: %s.",proc);
}
void App :: SaveProc()
{
 FILE* cmd;                              //this wskazuje strukture macierzysta typedef void* this = &Post
 if ((cmd = fopen(proc,"w"))!=NULL)
	{
	 WriteFile(cmd);          //no comment
	 fclose(cmd);
 }
 else
	{
	 int x,y;
	 setcolor(RED);
	 intro("Blad zapisu",&x,&y);
	 gprintf(&x,&y,"Nie moge zapisac %s.",proc);
	 gprintf(&x,&y,"Utworz w katalogu programu folder db.");
	 setcolor(WHITE);
	}
}
void App :: LoadProc()
{                                  //prubuje wgrac baze danych
 FILE* cmd;
 int x,y;
 intro("Wczytywanie Bazy Danych",&x,&y);
 if ((cmd = fopen(proc,"r"))==NULL)
	{
	 setcolor(RED);
	 gprintf(&x,&y,"Nie znalazlem bazy danych %s.",proc);
	 fclose(cmd);
	 setcolor(GREEN);
	 gprintf(&x,&y,"Tworze domyslna baze danych. . .");getch();
	 cmd = fopen(proc,"w");
	 WriteFile(cmd);          //nie znajdzie to stworzy
	 fclose(cmd);
	 gprintf(&x,&y,"Stworzylem %s.",proc);
	}
 else
	{
	 gprintf(&x,&y,"Szukam bazy danych %s. . .",proc);
	 ReadFile(cmd);        //wczyta
	 fclose(cmd);
	 gprintf(&x,&y,"Wczytalem %s.",proc);
	}
}
void App :: CheckPassword()
{
 int x,y;
 intro("Zabezpieczenia bazy",&x,&y);
 if (pass!=0)
	{
	 setcolor(RED);
	 gprintf(&x,&y,"Podaj AKTUALNY Kod: ");
	 unsigned long p;
	 takel(&p,RED);
	 setcolor(WHITE);
	 if (p != pass)
		{
		 for (int i=0;i<1024;i++)
			{
			 setcolor(random(MAXCOLORS-1)+1);
			 gprintf(&x,&y,"Zabraniam dostepu do %s",proc);
			 y -= textheight("H") + 2;
			}
		 KILL();
		}
	 setcolor(LIGHTGREEN);
	 gprintf(&x,&y,"Poprawny kod.");
	 setcolor(WHITE);
	}                                             //po sprawdzeniu spyta czy chcesz zmienic kod
 else gprintf(&x,&y,"Procesor nie chroniony kodem.");
 intro("Nowy kod",&x,&y);
 gprint(false,&x,&y,"Ustalic nowy kod?");
 int z = ggetch(z,&x,&y);
 intro("Nie zmodyfikowalem kodu",&x,&y);
 if (z == 't')
	{
	 intro("Kod cyfrowy",&x,&y);
	 gprintf(&x,&y,"Kod ochronny to maxymalnie 9 cyfrowa liczba.");
	 gprintf(&x,&y,"Gdy wpiszesz 0 to anulujesz kod.");
	 gprintf(&x,&y,"Podaj nowy kod (liczbowy): ");
	 takel(&pass,LIGHTBLUE);                                    //pass jest typu ULONG !!!
	}
 else gprintf(&x,&y,"Nie zmodyfikowalem kodu.");
}
void App :: SetAttributes()
{
 int x,y;
 intro("Wartosci poczatkowe",&x,&y);
 gprintf(&x,&y,"Cecha: %d, Plus %d, Maly los %d, Duzy los %d",avv,Plus,mL,dL);
 gprint(false,&x,&y,"Zmienic?: ");  //mnoznik to multi
 int zz = ggetch(zz,&x,&y);
 intro("Cecha Domyslna",&x,&y);
 if (zz == 't')
	{
	 gprintf(&x,&y,"Podaj wartosc  poczatkowa cechy: ");
	 takei(&avv);
	 gprintf(&x,&y,"Podaj wartosc  plusa przy rozdzielaniu: ");
	 takei(&Plus);
	 gprintf(&x,&y,"Podaj o ile modyfikuje maly los: ");
	 takei(&mL);
	 gprintf(&x,&y,"Podaj o ile modyfikuje duzy los: ");
	 takei(&dL);
	}
 intro("Znalezione Cechy",&x,&y);
 ShowAttributes(&x,&y);
 gprint(false,&x,&y,"Znalezlem %d cech, zmienic to? ",il_cech);
 int zn = ggetch(zn,&x,&y);
 intro("Cechy bazy",&x,&y);
 if (zn == 't')
	{
	 setcolor(LIGHTRED);
	 gprintf(&x,&y,"Nadano mi ograniczenie cech do %d.",MAX_CECH);
	 setcolor(WHITE);
	 il_cech = -1;
	 while ((il_cech <= 0) || (il_cech > MAX_CECH))
		{
		 gprintf(&x,&y,"Podaj nowa ilosc cech: ");
		 takesh(&il_cech);
		 if (il_cech <= 0) gprintf(&x,&y,"Nie moge przyjac ujemnej lub zerowej ilosci cech.");
		 if (il_cech > MAX_CECH) gprintf(&x,&y,"Przekracza to ilosc maksymalna cech.");
		}
	 gprintf(&x,&y,"Pamietaj, ze licze od 0.");
	 for (int i=0;i<il_cech;i++)                  //liczenie cech od 0
		{
		 gprintf(&x,&y,"Podaj nazwa cechy numer %d: ",i);
		 takes(ncechy[i]);
		}
	 ShowAttributes(&x,&y);
	}
}
void Stop(int ile, int par,int* x,int* y)
{
 if ((par>ile/2) && ((par % ile)==0))
	{
	 gprintf(x,y,"Nacisnij cokolwiek, aby kontynuowac. . .");
	 getch();
	 cleardevice();
	 intro("Kontynuacja. . .",x,y);
	 *x = 25;
	 *y =25;
	}
}
void App :: ShowAttributes(int* x,int* y)
{
 gprintf(x,y,"Znalalem %d zdefiniowanych cech.",il_cech);
 gprintf(x,y,"Oto one:");
 for (int i = 0;i < il_cech;i++)
	{
	 gprintf(x,y,"      (%3d) %s",i,ncechy[i]);
	}
}
void App :: ShowRaces(int* x, int* y, short visible = 0)
{
 if (visible !=VISIBLE)                 //VISIBLE = 1
	{
	 gprintf(x,y,"Znalazlem %d ras.",il_ras);
	 for (int i=0;i<il_ras;i++)
		{
		 Stop(45,i,x,y);
		 gprintf(x,y,"   (%3d) %s",i,nrasy[i]);
		}
	}
 else
	{
	 gprintf(x,y,"Znalazlem %d ras.",il_ras);
	 for (int i=0;i<il_ras;i++)
		{
		 Stop(45,i,x,y);
		 gprint(false,x,y,"(%2d) %s ",i,nrasy[i]);   //tabela ras
		 *x = 210;
		 for (int j=0;j<il_cech;j++)
			{
			 tgprint(0,false,x,y,"%3d ",modr[i][j]);
			}
		 gln(x,y);
		}
	}
}
void App :: ChangeRaces()
{
 int x,y;
 intro("Zmienianie Ras",&x,&y);
 int zn ='a';
 while (zn !='k')
	{
	 glns(&x,&y,3);
	 gprintf(&x,&y,"    MENU");
	 gprintf(&x,&y,"Nacisnij d     aby dodac nowa rase");
	 gprintf(&x,&y,"Nacisnij z     aby zobaczyc rasy");
	 gprintf(&x,&y,"Nacisnij u     aby usunac wszystkie rasy");
	 gprintf(&x,&y,"Nacisnij m     aby zmodyfikowac wybrana rase");
	 gprintf(&x,&y,"Nacisnij e     aby usunac ostatnia rase");
	 gprintf(&x,&y,"Nacisnij k     aby zakonczyc");
	 gprint(false,&x,&y,"Twoj wybor___________");
	 zn =ggetch(zn,&x,&y);
	 intro("Opcje Menu",&x,&y);
	 if (zn == 'e')
		{
		 if (il_ras==0)
			{
			 gprintf(&x,&y,"Ilosc ras wynosi 0.");
			 gprintf(&x,&y,"Nie ma ras do zniszczenia !");
			}
		 else {il_ras--;gprintf(&x,&y,"Zniszczylem ostatnia rasa !");}
		}
	 if (zn =='u')
		{
		il_ras = 0;
		 for (int i=0;i<MAX_RACES;i++)
			{
			 strcpy(nrasy[i],"");
			 for (int j=0;j<MAX_CECH;j++) modr[i][j] = 0;
			}
		 setcolor(RED);
		 gprintf(&x,&y,"Przykro mi WSZYSTKIE rasy ulegly destrukcji.");
		 setcolor(WHITE);
		}
	 if (zn == 'm')
		{
		 gprintf(&x,&y,"Podaj numer rasy do modyfikacji: ");
		 int num;
		 takei(&num);
		 if ((num<0) || (num>il_ras-1)) gprintf(&x,&y,"Takiego numeru nie znalazlem w tablicy.");
		 else
			{
			 gprintf(&x,&y,"Podaj nazwe nowej rasy numer %d: ",num);
			 takes(nrasy[num]);
			 for (int j=0;j<il_cech;j++)
				{
				 gprintf(&x,&y,"Podaj jak rasa (%2d) %s modyfikuje (%2d) %s: ",il_ras,nrasy[num],j,ncechy[j]);
				 takesh(&modr[num][j]);
				}
			}
		}
	 if (zn == 'd')
		{
		 if (il_ras == MAX_RACES)
			{
			 setcolor(LIGHTRED);
			 gprintf(&x,&y,"Ilosc ras wynosi %d.",MAX_RACES);
			 gprintf(&x,&y,"Wypchalem tablice ras po brzegi wiecej nie wejdzie !");
			 setcolor(WHITE);
			}
		 else
			{
			 gprintf(&x,&y,"Podaj nazwe nowej rasy numer %d: ",il_ras);
			 takes(nrasy[il_ras]);
			 for (int j=0;j<il_cech;j++)
				{
				 gprintf(&x,&y,"Podaj jak rasa (%2d) %s modyfikuje (%2d) %s:",il_ras,nrasy[il_ras],j,ncechy[j]);
				 takesh(&modr[il_ras][j]);
				}
			 il_ras ++;
			 if (il_ras == MAX_RACES) gprintf(&x,&y,"Uff.. To juz maksimum ras.");
			}
		}
	 if (zn =='z')
		{
		 ShowRaces(&x,&y,VISIBLE);         //VISIBLE czyli pokaze tabele
		}
	}
}
void App :: ChangeProf()
{
 int x,y;
 intro("Zmienianie Profesji",&x,&y);
 int zn ='a';
 while (zn !='k')
	{
	 glns(&x,&y,3);
	 gprintf(&x,&y,"    MENU");
	 gprintf(&x,&y,"Nacisnij d     aby dodac nowa profesja");
	 gprintf(&x,&y,"Nacisnij z     aby zobaczyc profesje");
	 gprintf(&x,&y,"Nacisnij u     aby usunac wszystkie profesje");
	 gprintf(&x,&y,"Nacisnij m     aby zmodyfikowac wybrana profesje");
	 gprintf(&x,&y,"Nacisnij e     aby usunac ostatnia profesje");
	 gprintf(&x,&y,"Nacisnij k     aby zakonczyc");
	 gprint(false,&x,&y,"Twoj wybor___________");
	 zn =ggetch(zn,&x,&y);
	 intro("Opcje Menu",&x,&y);
	 if (zn == 'e')
		{
		 if (il_prof==0)
			{
			 gprintf(&x,&y,"Ilosc profesji wynosi 0.");
			 gprintf(&x,&y,"Nie ma profesji do zniszczenia !");
			}
		 else {il_prof--;gprintf(&x,&y,"Zniszczylem ostatnia profesja !");}
		}
	 if (zn =='u')
		{
		il_prof = 0;
		 for (int i=0;i<MAX_PROF;i++)
			{
			 strcpy(nprof[i],"");
			 for (int j=0;j<MAX_CECH;j++) modp[i][j] = 0;
			}
		 setcolor(RED);
		 gprintf(&x,&y,"Przykro mi WSZYSTKIE profesje ulegly destrukcji.");
		 setcolor(WHITE);
		}
	 if (zn == 'm')
		{
		 gprintf(&x,&y,"Podaj numer profesji do modyfikacji: ");
		 int num;
		 takei(&num);
		 if ((num<0) || (num>il_prof-1)) gprintf(&x,&y,"Takiego numeru nie znalazlem w tablicy.");
		 else
			{
			 gprintf(&x,&y,"Podaj nazwe nowej profesji numer %d: ",num);
			 takes(nprof[num]);
			 for (int j=0;j<il_cech;j++)
				{
				 gprintf(&x,&y,"Podaj jak profesja (%2d) %s modyfikuje (%2d) %s: ",il_prof,nprof[num],j,ncechy[j]);
				 takesh(&modp[num][j]);
				}
			}
		}
	 if (zn == 'd')
		{
		 if (il_prof == MAX_PROF)
			{
			 setcolor(LIGHTRED);
			 gprintf(&x,&y,"Ilosc profesji wynosi %d.",MAX_PROF);
			 gprintf(&x,&y,"Wypchalem tablice profesji po brzegi wiecej nie wejdzie !");
			 setcolor(WHITE);
			}
		 else
			{
			 gprintf(&x,&y,"Podaj nazwe nowej profesji numer %d: ",il_prof);
			 takes(nprof[il_prof]);
			 for (int j=0;j<il_cech;j++)
				{
				 gprintf(&x,&y,"Podaj jak profesja (%2d) %s modyfikuje (%2d) %s:",il_prof,nprof[il_prof],j,ncechy[j]);
				 takesh(&modp[il_prof][j]);
				}
			 il_prof ++;
			 if (il_prof == MAX_PROF) gprintf(&x,&y,"Uff.. To juz maksimum profesji.");
			}
		}
	 if (zn =='z')
		{
		 ShowProf(&x,&y,VISIBLE);         //VISIBLE czyli pokaze tabele
		}
	}
}
void App :: SetRaces()
{
 int x,y;
 intro("Ustawianie Ras",&x,&y);
 ShowRaces(&x,&y);
 gprint(false,&x,&y,"Zmienic te rasy? ");
 int zn = ggetch(zn,&x,&y,LIGHTGREEN,LIGHTBLUE);
 intro("Zmiana ras.",&x,&y);
 if (zn == 't')
	{
	 setcolor(RED);
	 gprintf(&x,&y,"Nadano mi ograniczenie ilosci ras: %d.",MAX_RACES);
	 setcolor(WHITE);
	 getch();
	 ChangeRaces();
	}
 else gprintf(&x,&y,"Nie zmienilem ras.");
}
void App :: ShowProf(int* x, int* y, short visible = 0)
{
 if (visible !=VISIBLE)                 //VISIBLE = 1
	{
	 gprintf(x,y,"Znalazlem %d profesji.",il_prof);
	 for (int i=0;i<il_prof;i++)
		{
		 Stop(45,i,x,y);
		 gprintf(x,y,"   (%3d) %s",i,nprof[i]);
		}
	}
 else
	{
	 gprintf(x,y,"Znalazlem %d profesji.",il_prof);
	 for (int i=0;i<il_prof;i++)
		{
		 Stop(45,i,x,y);
		 gprint(false,x,y,"(%2d) %s ",i,nprof[i]);   //tabela ras
		 *x = 210;
		 for (int j=0;j<il_cech;j++)
			{
			 tgprint(0,false,x,y,"%3d ",modp[i][j]);
			}
		 gln(x,y);
		}
	}
}

void App :: SetProf()
{
 int x,y;
 intro("Ustawianie Profesji",&x,&y);
 ShowProf(&x,&y);
 gprint(false,&x,&y,"Zmienic te profesje? ");
 int zn = ggetch(zn,&x,&y,LIGHTBLUE,RED);
 intro("Zmiana profesji.",&x,&y);
 if (zn == 't')
	{
	 setcolor(RED);
	 gprintf(&x,&y,"Nadano mi ograniczenie ilosci profesji: %d.",MAX_PROF);
	 setcolor(WHITE);
	 getch();
	 ChangeProf();
	}
 else gprintf(&x,&y,"Nie zmienilem profesji.");
}
void ERROR()
{
 int x,y;
 setcolor(RED);
 intro("Blad w zapisie danych",&x,&y);
 gprintf(&x,&y,"Plik bazy danych jest uszkodzony !!!");
 KILL();
}
void App :: TryProc()
{
 int x,y;
 intro("Sprawdzanie poprawnosci bazy",&x,&y);
 gprintf(&x,&y,"Sprawdzam poprawnosc zapisu...");
 gprintf(&x,&y,"Plik powinien zawierac %d znakow.",sizeof(*this));
 if ((il_cech<0) || (il_cech>MAX_CECH)) ERROR();
 if ((il_ras<0) || (il_ras>MAX_RACES)) ERROR();
 if ((il_prof<0) || (il_prof>MAX_PROF)) ERROR();        //sprawdza poprawnosc zapisu struktury
 if ((il_um<0) || (il_um>MAX_UM)) ERROR();          //program nie dopuszcza pisania poza tablicami
																	 //wiec odpowiednie il-xxx nie moga miec
}                                                   //wartosci ujemnych lub przekraczajacych
void App :: ShowSkill(int*x, int* y,short visible = 0)            //wartosc tabeli
{
 if (visible !=VISIBLE)
	{
	 gprintf(x,y,"Znalazlem %d umiejetnosci.",il_um);
	 for (int i=0;i<il_um;i+=4)
		{
		 *x = 0;
		 Stop(180,i,x,y);
		 *x = 0;
		 tgprint(13,false,x,y,"(%3d)%s",i,n_um[i]);
		 if ((strcmp(n_um[i+1],"")!=0) && (i+1<MAX_UM)) { *x += MaxX/6;tgprint(13,false,x,y,"(%3d)%s",i+1,n_um[i+1]);}
		 if ((strcmp(n_um[i+2],"")!=0) && (i+2<MAX_UM)) { *x +=MaxX/6;tgprint(13,false,x,y,"(%3d)%s",i+2,n_um[i+2]);}
		 if ((strcmp(n_um[i+3],"")!=0) && (i+3<MAX_UM)) { *x +=MaxX/6;tgprint(13,false,x,y,"(%3d)%s",i+3,n_um[i+3]);}
		 gprintf(x,y," ");
		}
	}
 else
	{
	 gprintf(x,y,"Znalazlem %d umiejetnosci.",il_um);
	 for (int i=0;i<il_um;i++)
		{
		 *x = 0;
		 Stop(45,i,x,y);
		 *x = 0;
		 tgprint(18,false,x,y,"(%3d)%s",i,n_um[i]); *x += MaxX/4;
		 tgprint(15,false,x,y,"c: (%2d)%s",um[i][TEST_FROM],ncechy[um[i][TEST_FROM]]); *x += MaxX/5;
		 tgprint(6,false,x,y,"test: %d, mod: %d",um[i][MULTIPLE],um[i][MOD]);
		 gprintf(x,y,"");
		}
	}
 *x = 15;
}
void App :: SetSkill()
{
 int x,y;
 intro("Ustawianie Umiejetnosci",&x,&y);
 ShowSkill(&x,&y);
 gprint(false,&x,&y,"Zmienic te umiejetnosci? ");
 int zn = ggetch(zn,&x,&y,LIGHTBLUE,GREEN);
 intro("Zmiana umiejetnosci.",&x,&y);
 if (zn == 't')
	{
	 setcolor(RED);
	 gprintf(&x,&y,"Nadano mi ograniczenie ilosci umiejetnosci: %d.",MAX_UM);
	 setcolor(WHITE);
	 getch();
	 ChangeSkill();
	}
 else gprintf(&x,&y,"Nie zmienilem umiejetnosci.");
}
void App :: ChangeSkill()
{
 int x,y;
 intro("Umiejetnosci",&x,&y);
 int zn ='a';
 while (zn !='k')
	{
	 glns(&x,&y,3);
	 gprintf(&x,&y,"        MENU");
	 gprintf(&x,&y,"Nacisnij d    aby dodac nowa umiejetnosc  ");
	 gprintf(&x,&y,"Nacisnij z    aby zobaczyc umiejetnosci  ");
	 gprintf(&x,&y,"Nacisnij u    aby usunac wszystkie umiejetnosci  ");
	 gprintf(&x,&y,"Nacisnij m    aby zmodyfikowac wybrana umiejetnosc  ");
	 gprintf(&x,&y,"Nacisnij e    aby usunac ostatnia umiejetnosc  ");
	 gprintf(&x,&y,"Nacisnij k    aby zakonczyc  ");
	 gprint(false,&x,&y,"Twoj wybor____________");
	 zn = ggetch(zn,&x,&y,LIGHTGREEN,LIGHTBLUE);
	 intro("Opcje Menu",&x,&y);
	 if (zn == 'e')
		{
		 if (il_um==0)
			{
			 gprintf(&x,&y,"Ilosc umiejetnosci wynosi 0.");
			 gprintf(&x,&y,"Nie ma umiejetnosci do zniszczenia !");
			}
		 else {il_um--;gprintf(&x,&y,"Zniszczylem ostatnia umiejetnosc !");}
		}
	 if (zn =='u')
		{
		il_um  = 0;
		for (int i = 0;i<MAX_UM;i++)
			{
			 strcpy(n_um[i],"");
			 um[i][TEST_FROM] = 0;
			 um[i][MULTIPLE] = 0;
			 um[i][MOD] = 0;
			}
		 setcolor(RED);
		 gprintf(&x,&y,"Przykro mi WSZYSTKIE umiejetnosci ulegly destrukcji.");
		 setcolor(WHITE);
		}
	 if (zn == 'm')
		{
		 gprintf(&x,&y,"Podaj numer umiejetnosci do modyfikacji: ");
		 int num;
		 takei(&num);
		 if ((num<0) || (num>il_um-1)) gprintf(&x,&y,"Takiego numeru nie znalazlem w tablicy.");
		 else
			{
			 gprintf(&x,&y,"Podaj nazwe nowej umiejetnosci numer %d: ",num);
			 takes(n_um[num]);
			 ShowAttributes(&x,&y);
			 gprintf(&x,&y,"Oczekuje na liczbe.");
			 gprintf(&x,&y,"Podaj ceche od ktorej testujemy %s: ",n_um[num]);
			 takesh(&um[num][TEST_FROM]);
			 if (um[num][TEST_FROM]>il_cech-1){gprintf(&x,&y,"Poza zakresem wzielem (%d)%s",il_cech-1,ncechy[il_cech-1]);um[num][TEST_FROM] = il_cech;um[num][TEST_FROM]--;}
			 if (um[num][TEST_FROM]<0){gprintf(&x,&y,"Poza zakresem wzielem (%d)%s",0,ncechy[0]);um[num][TEST_FROM] = 0;}
			 gprintf(&x,&y,"Dla Triady dodajemy 3 razy.");
			 gprintf(&x,&y,"Podaj ile razy mam dodawac poziom: ");
			 takesh(&um[num][MULTIPLE]);
			 gprintf(&x,&y,"Domyslna wartosc 0.");
			 gprintf(&x,&y,"Podaj modyfikator testu: ");
			 takesh(&um[num][MOD]);
			}
		}
	 if (zn == 'd')
		{
		 if (il_um == MAX_UM)
			{
			 setcolor(LIGHTRED);
			 gprintf(&x,&y,"Ilosc umiejetnosci %d.",MAX_UM);
			 gprintf(&x,&y,"Wypchalem tablice umiejetnosci po brzegi wiecej nie wejdzie !");
			 setcolor(WHITE);
			}
		 else
			{
			 gprintf(&x,&y,"Podaj nazwe nowej umiejetnosci numer %d: ",il_um);
			 takes(n_um[il_um]);
			 ShowAttributes(&x,&y);
			 gprintf(&x,&y,"Oczekuje numeru.");
			 gprintf(&x,&y,"Podaj ceche od ktorej testujemy %s: ",n_um[il_um]);
			 takesh(&um[il_um][TEST_FROM]);
			 if (um[il_um][TEST_FROM]>il_cech-1){gprintf(&x,&y,"Poza zakresem wzielem (%d)%s",il_cech-1,ncechy[il_cech-1]);um[il_um][TEST_FROM] = il_cech;um[il_um][TEST_FROM]--;}
			 if (um[il_um][TEST_FROM]<0){gprintf(&x,&y,"Poza zakresem wzielem (%d)%s",0,ncechy[0]);um[il_um][TEST_FROM] = 0;}
			 gprintf(&x,&y,"Dana liczbowa, domyslnie dla Triady 3.");
			 gprintf(&x,&y,"Podaj ile razy mam dodawac poziom: ");
			 takesh(&um[il_um][MULTIPLE]);
			 gprintf(&x,&y,"Domyslnie dla Triady 0.");
			 gprintf(&x,&y,"Podaj modyfikator testu: ");
			 takesh(&um[il_um][MOD]);
			 il_um ++;
			 if (il_um == MAX_UM) gprintf(&x,&y,"Uff.. To juz maksimum umiejetnosci.");
			}
		}
	 if (zn =='z')
		{
		 ShowSkill(&x,&y,VISIBLE);
		}
	}
}
App :: App()            //konstruktor App wywolywany przy deklaracji tego obiektu
{
 strcpy(proc,"");     //obiekt nie ma nazwy
 pass = 0;            //ani kodu
 avv = 10;
 Plus = 1;
 mL = 1;
 dL = 2;
 il_cech = MAX_CECH;          //ma maxymalna ilosc cech
 for (int i=0;i<il_cech;i++)
	{
	 strcpy(ncechy[i],"_");
	}
 il_ras = MAX_RACES;                   //a takze ras
 for (int i=0;i<il_ras;i++)
	{
	 strcpy(nrasy[i],"_");
	 for (int j=0;j<MAX_CECH;j++)
		{
		 modr[i][j] = 0;             //ich modyfikatory wynosza 0
		}
	}
 il_prof = MAX_PROF;          //profesji tez max
 for (int i=0;i<il_prof;i++)
	{
	 strcpy(nprof[i],"_");
	 for (int j=0;j<MAX_CECH;j++)
		{
		 modp[i][j] = 0;           //podobnie
		}
	}
 il_um = MAX_UM;                 //umiejetnosci max
 for (int i=0;i<il_um;i++)
	{
	 strcpy(n_um[i],"_");
	 um[i][TEST_FROM] = 0 ;          //testowane od 0 cechy gdyz ta zawsze istnieje
	 um[i][MULTIPLE] = 3;         //poziom dodawany 3 razy
	 um[i][MOD] = 0;              //modyfikator = 0
	}
}
void lnf(FILE* f)
{
 fprintf(f,"\n");             //napisz linie wolna w pliku f
}
void Generate(App* c,char* n)
{
 FILE* f;                              //na temat HTML bazy
 strcat(n,".html");
 if ((f = fopen(n,"r")) != NULL)       //utworz na dysku db\xxx.html
	{
	 int x,y;
	 setcolor(YELLOW);
	 intro("Plik istnieje",&x,&y);
	 gprint(false,&x,&y,"Ta baza danych %s istnieje, zastapic? ",n);
	 setcolor(WHITE);
	 int z = ggetch(z,&x,&y);                           //istnieje, zastapic
	 intro("OK",&x,&y);
	 if (z != 't') return;
	}
 if ((f = fopen(n,"w")) == NULL)
	{
	 int x,y;
	 setcolor(RED);
	 intro("Blad Programu",&x,&y);
	 gprintf(&x,&y,"Utworz folder char w katalogu programu !");
	 gprintf(&x,&y,"Nie moge utworzyc pliku %s.",n);    //blad zapisu
	 setcolor(WHITE);
	}
 else
	{
	fprintf(f,"<html>\n");                       //tworzenie kodu html w c++
	fprintf(f,"<head>\n");
	fprintf(f,"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-2\">\n");
	fprintf(f,"<title>");                         //kodowanie znakow dla Explorera
	fprintf(f,"Baza danych: %s",c->proc);
	fprintf(f,"</title>\n");
	fprintf(f,"</head>\n");
	fprintf(f,"<body bgcolor=\"#ffffff\" text=\"000000\">\n");
	fprintf(f,"<body>\n");
	fprintf(f,"<font size=\"4\">\n");
	fprintf(f,"<center>\n");
	fprintf(f,"<pre>\nIlosc cech: %d\n",c->il_cech);
	fprintf(f,"</center>\n");
	lnf(f);lnf(f);
	for (int i=0;i<c->il_cech;i+=3)
		{
		 fprintf(f,"%-2d.  %s,  ",i+1,c->ncechy[i]);
		 if ((i+1<c->il_cech) && (strcmp(c->ncechy[i+1],"")!=0))fprintf(f,"%-2d.  %s,  ",i+2,c->ncechy[i+1]);
		 if ((i+2<c->il_cech) && (strcmp(c->ncechy[i+2],"")!=0))fprintf(f,"%-2d.  %s,\n",i+3,c->ncechy[i+2]);
		}
	lnf(f);
	fprintf(f,"<center>\n");
	fprintf(f,"<pre>\nIlosc ras: %d\n",c->il_ras);
	fprintf(f,"</center>\n");
	fprintf(f,"<table border=\"2\">\n");
	fprintf(f,"<tr>\n<td>Nazwa rasy\n");
	for (int i=0;i<c->il_cech;i++)
		{
		 fprintf(f,"<td>%s\n",c->ncechy[i]);
		}
	for (int i=0;i<c->il_ras;i++)
		{
		 fprintf(f,"<tr>\n<td>%d.%s\n",i,c->nrasy[i]);
		 for (int j=0;j<c->il_cech;j++)
			{
			 fprintf(f,"<td>%d\n",c->modr[i][j]);
			}
		}
	fprintf(f,"</table>\n");
	fprintf(f,"<center>\n");
	fprintf(f,"<pre>\nIlosc profesji: %d\n",c->il_prof);
	fprintf(f,"</center>\n");
	fprintf(f,"<table border=\"2\">\n");
	fprintf(f,"<tr>\n<td>Nazwa profesji\n");
	for (int i=0;i<c->il_cech;i++)
		{
		 fprintf(f,"<td>%s\n",c->ncechy[i]);
		}
	for (int i=0;i<c->il_prof;i++)
		{
		 fprintf(f,"<tr>\n<td>%d.%s\n",i,c->nprof[i]);
		 for (int j=0;j<c->il_cech;j++)
			{
			 fprintf(f,"<td>%d\n",c->modp[i][j]);
			}
		}
	fprintf(f,"</table>\n");
	fprintf(f,"<center>\n");
	fprintf(f,"<pre>\nIlosc umiejetnosci: %d\n",c->il_um);
	fprintf(f,"</center>\n");
	fprintf(f,"<table border=\"2\">\n");
	fprintf(f,"<tr>\n<td>Nazwa umiejetnosci\n");
	fprintf(f,"<td>Testujaca cecha\n");
	fprintf(f,"<td>Stopien rozwoju na poziom\n");
	fprintf(f,"<td>Modyfikator Testu\n");
	for (int i=0;i<c->il_um;i++)
		{
		 fprintf(f,"<tr>\n<td>%d.%s\n",i,c->n_um[i]);
		 fprintf(f,"<td>%d. %s\n",c->um[i][TEST_FROM],c->ncechy[c->um[i][TEST_FROM]]);
		 fprintf(f,"<td>+ %d\n",c->um[i][MULTIPLE]);
		 fprintf(f,"<td>[%d]\n",c->um[i][MOD]);
		}
	fprintf(f,"</table>\n");
	fprintf(f,"<pre>\nAutor Aplikacji Morgoth DBMA</pre>\n");
	//end of html.
	fprintf(f,"</body>\n");
	fprintf(f,"</html>\n");
	}
}
void HTML(App* cmd)                  //pyta o HTML
{
 int x,y;
 intro("Baza HTML",&x,&y);
 gprint(false,&x,&y,"Wygenerowac baze danych w HTML? ");
 int zn = ggetch(zn,&x,&y,LIGHTBLUE,LIGHTBLUE);
 intro("Plik HTML",&x,&y);
 if (zn == 't')
	{
	 intro("Nazwa Bazy",&x,&y);
	 gprintf(&x,&y,"Podaj nazwe bazy (bez rozszerzenia): ");
	 char nazw[STRING];
	 char nazwa[STRING];
	 takes(nazw);
	 strcpy(nazwa,"db\\");
	 strcat(nazwa,nazw);
	 if (strcmp(nazwa,"db\\")==0)strcpy(nazwa,"db\\default");
	 Generate(cmd,nazwa);
	}
 else gprintf(&x,&y,"Nie wygenerowalem pliku HTML.");
}
App :: ~App()
{
}

void Pozegnanie()
{
 int x,y;
 intro("Dzieki za uzycie Programu Applicat",&x,&y);
 glns(&x,&y,6);
 setcolor(LIGHTBLUE);
 gprintf(&x,&y,"Dziekuje za uzycie mojego programu.");
 gprintf(&x,&y,"Wszelkie zastrzezenia dzwon 025-758-16-76");
 gprintf(&x,&y,"Pytaj o Programiste...");
 gprintf(&x,&y,"   Morgoth Daemon Black Metal Art.");
 glns(&x,&y,6);
}
void CreateCharacter(App* cmd);      //zapowiedz funkcji zdefiniowanej ponizej
int App :: Run()
{
 int x,y;
 intro("Uruchamianie Programu. . .",&x,&y);
 randomize();                       //wirtualny Run dla App
 SetCurrentProcess();
 LoadProc();
 intro("Baza Danych",&x,&y);
 gprint(false,&x,&y,"Wgrac baze danych do edycji? ");
 int zn = ggetch(zn,&x,&y);
 intro("Konfiguracja",&x,&y);
 if (zn == 't')
	{
	 CheckPassword();
	 TryProc();
	 SaveProc();
	 SetAttributes();
	 SaveProc();
	 SetRaces();
	 SaveProc();
	 SetProf();
	 SaveProc();
	 SetSkill();
	 SaveProc();
	}
 else gprintf(&x,&y,"Baza danych pozostawiona bez zmian.");
 SaveProc();
 HTML(this);
 CreateCharacter(this);
 getch();
 return OK;             // -1
}




									  //glowna klasa postaci
class Post
{
 public:
 Post(short);                     //tworzy postac o ilosci cech = short
 void LoadPost(FILE*);
 void SavePost();
 void WritePost(FILE*);
 void AboutWriteFile();


 char rasa[STRING];               //jej rasa
 char prof[STRING];               //profesja

 short cecha[MAX_CECH];            //wartosc cechy
 short il_cech;                    //ilosc cech
 char ncechy[MAX_CECH][STRING];    //nazwy cech
 int c_pl[MAX_CECH];
 int suma;                         //suma cech powyzej (ponizej) przecietnej

 short il_um;                       //ilosc umiejetnosci
 char n_um[MAX_UM][STRING];         //nazwy um.
 int u_pl[MAX_UM];
 short pozum[MAX_UM];               //poziomy um.
 short tesum[MAX_UM];               //test um.

 char imie[STRING];            //imie, nazw, plec
 char nazw[STRING];
 char plec[STRING];

 //protected file operations
 char proc[STRING];                 //nazwa postaci char\xxx.html
 char save[STRING];
 int PD;
 ULONG procent;
 int avv;
};                                                //jezeli nie moze utworzyc stworz folder char   !!!!!
void Post :: AboutWriteFile()
{
if (strcmp(imie,"")==0)strcpy(imie,"_");
if (strcmp(nazw,"")==0)strcpy(nazw,"_");
if (strcmp(rasa,"")==0)strcpy(rasa,"_");
if (strcmp(prof,"")==0)strcpy(prof,"_");
 for (int i=0;i<STRING;i++)
	{
	 if (rasa[i] == ' ') rasa[i] = '_';
	 if (prof[i] == ' ') prof[i] = '_';
	 if (imie[i] == ' ') imie[i] = '_';
	 if (nazw[i] == ' ') nazw[i] = '_';
	 if (plec[i] == ' ') plec[i] = '_';
	}
 for (int i=0;i<il_cech;i++)
	{
	 if (strcmp(ncechy[i],"")==0)strcpy(ncechy[i],"_");
	 for (int j=0;j<STRING;j++)
		{
		 if (ncechy[i][j] == ' ') ncechy[i][j] = '_';
		}
	}
 for (int i=0;i<il_um;i++)
	{
	 if (strcmp(n_um[i],"")==0)strcpy(n_um[i],"_");
	 for (int j=0;j<STRING;j++)
		{
		 if (n_um[i][j] == ' ') n_um[i][j] = '_';
		}
	}
}
void Post :: WritePost(FILE* f)
{
 AboutWriteFile();
 fprintf(f,"HTML File: %s\n",proc);
 fprintf(f,"SAVE File: %s\n",save);
 fprintf(f,"PD: %d\n",PD);
 fprintf(f,"Ilosc Cech: %d\n",il_cech);
 fprintf(f,"Suma: %d\n",suma);
 for (int i=0;i<il_cech;i++)
	{
	 fprintf(f,"Nazwa %d: %s\n",i,ncechy[i]);
	 fprintf(f,"Wartosc: %d\n",cecha[i]);
	 fprintf(f,"Plusow: %d\n",c_pl[i]);
	}
 fprintf(f,"Ilosc Umiejetnosci: %d\n",il_um);
 for (int i=0;i<il_um;i++)
	{
	 fprintf(f,"Nazwa %d: %s\n",i,n_um[i]);
	 fprintf(f,"Poziom: %d\n",pozum[i]);
	 fprintf(f,"Test: %d\n",tesum[i]);
	 fprintf(f,"Plusow: %d\n",u_pl[i]);
	}
 fprintf(f,"Imie: %s\n",imie);
 fprintf(f,"Nazwisko: %s\n",nazw);
 fprintf(f,"Plec: %s\n",plec);
 fprintf(f,"Rasa: %s\n",rasa);
 fprintf(f,"Profesja: %s\n",prof);
 fprintf(f,"Average: %d\n",procent);
 fprintf(f,"Start Value: %d\n",avv);
}
void Post :: LoadPost(FILE* f)
{
 fscanf(f,"HTML File: %s\n",proc);
 fscanf(f,"SAVE File: %s\n",save);
 fscanf(f,"PD: %d\n",&PD);
 fscanf(f,"Ilosc Cech: %d\n",&il_cech);
 fscanf(f,"Suma: %d\n",&suma);
 for (int i=0;i<il_cech;i++)
	{
	 fscanf(f,"Nazwa %d: %s\n",&i,ncechy[i]);
	 fscanf(f,"Wartosc: %d\n",&cecha[i]);
	 fscanf(f,"Plusow: %d\n",&c_pl[i]);
	}
 fscanf(f,"Ilosc Umiejetnosci: %d\n",&il_um);
 for (int i=0;i<il_um;i++)
	{
	 fscanf(f,"Nazwa %d: %s\n",&i,n_um[i]);
	 fscanf(f,"Poziom: %d\n",&pozum[i]);
	 fscanf(f,"Test: %d\n",&tesum[i]);
	 fscanf(f,"Plusow: %d\n",&u_pl[i]);
	}
 fscanf(f,"Imie: %s\n",imie);
 fscanf(f,"Nazwisko: %s\n",nazw);
 fscanf(f,"Plec: %s\n",plec);
 fscanf(f,"Rasa: %s\n",rasa);
 fscanf(f,"Profesja: %s\n",prof);
 fscanf(f,"Average: %d\n",&procent);
 fprintf(f,"Start Value: %d\n",&avv);
}
void CChr(App* cmd, Post* post);    //zapowiedz funkcji
void CreateCharacter(App* cmd)
{
 int x,y;
 intro("Stworzyc postac?",&x,&y);
 gprintf(&x,&y,"Postac bedzie tworzona na podstawie bazy %s.",cmd->proc);
 gprint(false,&x,&y,"Stworzyc postac w oparciu o baze %s? ",cmd->proc);
 int zn = ggetch(zn,&x,&y);
 intro("Postac",&x,&y);
 if (zn =='t')
	{                         //nowy obiekt natychmiast wola konstruktora Post :: Post(10);
	 Post* post = new Post(cmd->avv);    // tworzy dynamiczny obiekt post CECHA DOMYSLNA AVV
	 CChr(cmd,post);               //operacje na obiekcje
	 delete post;                  //destrukcja i zwolnienie pamieci
	}
 else gprintf(&x,&y,"Nie stworzylem postaci.");
 Pozegnanie();
}

void Draw(int x,int y,int av, int value)
{
 float val = (float) value/av;
 int col = YELLOW;
 if (val<0.70) col = RED;
 if ((val>=0.70) && (val<0.85)) col = LIGHTRED;
 if ((val>=0.85) && (val<1.15)) col = YELLOW;
 if ((val>=1.15) && (val<1.30)) col = LIGHTGREEN;
 if (val>1.30) col = GREEN;
 setcolor(col);
 setfillstyle(SOLID_FILL,col);
 bar(x+1,y-1,x+100*val,y+3);
 setcolor(WHITE);
}

void ShowAttributes(int* x,int*y,Post* post)      //wskaznik jako parametr gdyz:
{                                                  //szybciej przekazywany
 setcolor(LIGHTBLUE);
 gprintf(x,y,"Aktualne cechy postaci.");                //mozliwosc modyfikacji obiektu przez adres
 setcolor(WHITE);
 int sr = 0;
 for (int i=0;i<post->il_cech;i++)
	{
	 sr += post->cecha[i];
	}
 sr /= post->il_cech;
 for (int i=0;i<post->il_cech;i++)
	{
	 *x = 15;
	 tgprint(19,false,x,y,"(%2d) %s",i,post->ncechy[i]); *x +=MaxX/4;
	 tgprint(1,false,x,y,"%d [+%d]",post->cecha[i],post->c_pl[i]); *x +=MaxX/7; Draw(*x,*y,sr,post->cecha[i]);
	 gprintf(x,y,"");
//	 gprintf(x,y,"(%2d) %s    %d",i,post->ncechy[i],post->cecha[i]);
	}
 *x = 15;
}
void SetRace(Post* post,App* cmd)     //cmd to wskaznik do bazy danych
{
 strcpy(post->rasa,"");
 int x,y;
 intro("Wybor Rasy",&x,&y);
 gprint(false,&x,&y,"Chcesz wybrac rase? ");
 int zn = ggetch(zn,&x,&y,BLUE,GREEN);
 intro("Wybor Rasy",&x,&y);
 if (zn == 't')
	{
	 gprintf(&x,&y,"Wybierz rase swojej postaci: ");
	 cmd->ShowRaces(&x,&y);
	 gprintf(&x,&y,"Podaj numer rasy: ");
	 int num;
	 takei(&num);
	 if (num>cmd->il_ras-1) num = cmd->il_ras - 1;
	 if (num<0) num = 0;
	 strcpy(post->rasa,cmd->nrasy[num]);
	 for (int i=0;i<post->il_cech;i++)
		 {
		  post->cecha[i] += cmd->modr[num][i];
		 }
	 gprintf(&x,&y,"Wybrano rase %s.",post->rasa);
	}
 else gprintf(&x,&y,"Nie wybrano rasy.");
 intro("Cechy Postaci",&x,&y);
 ShowAttributes(&x,&y,post);
 getch();
}
void SetProf(Post* post,App* cmd)
{
 strcpy(post->prof,"");
 int x,y;
 intro("Wybor Profesji",&x,&y);
 gprint(false,&x,&y,"Chcesz wybrac profesje? ");
 int zn = ggetch(zn,&x,&y,RED,BROWN);
 intro("Wybor Postaci",&x,&y);
 if (zn == 't')
	{
	 gprintf(&x,&y,"Wybierz profesje swojej postaci: ");
	 cmd->ShowProf(&x,&y);
	 gprintf(&x,&y,"Podaj numer profesji: ");
	 int num;
	 takei(&num);
	 if (num>cmd->il_prof-1) num = cmd->il_prof - 1;
	 if (num<0) num = 0;
	 strcpy(post->prof,cmd->nprof[num]);
	 for (int i=0;i<post->il_cech;i++)
		 {
		  post->cecha[i] += cmd->modp[num][i];
		 }
	 gprintf(&x,&y,"Wybrano profesje %s.",post->prof);
	}
 else gprintf(&x,&y,"Nie wybrano profesji.");
 intro("Cechy Postaci",&x,&y);
 ShowAttributes(&x,&y,post);
 getch();
}
void GetValuesDB(App* cmd, Post* post)   //pobierz wartosci z bazy danych
{
 post->il_cech = cmd->il_cech;         //ilosc cech postaci
 for (int i=0;i<post->il_cech;i++)
	{
	 strcpy(post->ncechy[i],cmd->ncechy[i]);   //nazwy tych cech
	}
}
void SetRandom(Post* post,App* cmd)                     //losowanie cech +\- 1,2
{
 int x,y;
 intro("Losowanie Cech",&x,&y);
 gprint(false,&x,&y,"Modyfikowac losowo cechy postaci? ");
 int z = ggetch(z,&x,&y);
 intro("Losowanie Cech",&x,&y);
 if (z == 't')
  {
	int zn ='e';
	int base = 0;
	int mod = 0;
	int suma;
	short c[MAX_CECH];
	for (int i=0;i<post->il_cech;i++) c[i] = post->cecha[i];
	while (zn !='k')
		{
		 suma = 0;
		 gprintf(&x,&y,"Nacisnij d    aby zmodyfikowac cechy +/-2");
		 gprintf(&x,&y,"Nacisnij m    aby zmodyfikowac cechy +/-1");
		 gprint(false,&x,&y,"Twoj wybor? ");
		 int znk = ggetch(znk,&x,&y,LIGHTBLUE,GREEN);
		 intro("Typ Losowania",&x,&y);
		 if (znk =='d') {base = (2*cmd->dL)+1; mod = -cmd->dL; gprintf(&x,&y,"Wybrano duza modyfikacje.");}
		 if (znk =='m') {base = (2*cmd->mL)+1; mod = -cmd->mL; gprintf(&x,&y,"Wybrano mala modyfikacje.");}
		 for (int i=0;i<post->il_cech;i++)
			{
			 post->cecha[i] += short(random(base) + mod);
			 suma += post->cecha[i];
			}
		 suma -= cmd->avv * post->il_cech;
		 gprintf(&x,&y,"Wynik losowania: ");
		 ShowAttributes(&x,&y,post);
		 gprintf(&x,&y,"Suma:       %d.",suma);
		 glns(&x,&y,2);
		 gprint(false,&x,&y,"   Nacisnij k   aby zachowac te cechy: ");
		 zn = ggetch(zn,&x,&y);
		 intro("Losowanie",&x,&y);
		 if (zn !='k')
			{
			 for (int i=0;i<post->il_cech;i++) post->cecha[i] = c[i];
			 intro("Zachowalem cechy",&x,&y);
			}
		}
  }
 else gprintf(&x,&y,"Nie zmodyfikowalem cech.");
 for (int i=0;i<post->il_cech;i++) post->suma += post->cecha[i];
 post->suma -= 10 * post->il_cech;
 ShowAttributes(&x,&y,post);
 getch();
}
int GetPoz(int p)
{
 if (p==1) return 10;            //zwraca ilosc punktow um. w zaleznosci od poz. postaci
 if (p==2) return 20;
 if (p==3) return 30;
 if (p==4) return 45;
 if (p==5) return 60;
 if (p==6) return 100;
 if (p==7) return 200;
 if (p==8) return 400;
 if (p==9) return 800;
 if (p==10) return 1600;
 return -1;
}
int Subtract(int p)
{
 if (p==0) return 0;             //odejmuje ilosc p. umiejetnosci za dany poziom
 if (p==1) return 1;
 if (p==2) return 3;
 if (p==3) return 6;
 if (p==4) return 15;
 if (p==5) return 35;
 if (p==6) return 75;
 if (p==7) return 155;
 if (p==8) return 315;
 if (p==9) return 635;
 if (p==10) return 1275;
 return -1;
}
void ShowSkills(int* x,int* y,Post* post, int num = -1)         //Visible lub bez parametru
{                                        //gdy nie ma parametru domyslnie -1 czyli ~VISIBLE
 if (num == -1)
	{
	 for (int i=0;i<post->il_um;i++)
		{
		 *x = 15;
		 Stop(45,i,x,y);
		 *x = 15;
		 tgprint(19,false,x,y,"%2d.%s",i,post->n_um[i]); *x +=MaxX/4;
		 tgprint(4,false,x,y,"%2d  %3d [+%d]",post->pozum[i],post->tesum[i],post->u_pl[i]);
		 gprintf(x,y,"");
//		 gprintf(x,y,"%2d.%-20s %2d  %3d\n",i,post->n_um[i],post->pozum[i],post->tesum[i]);
		}
	}
 else
	{
	 *x = 15;
	 tgprint(19,false,x,y,"%2d.%s",num,post->n_um[num]); *x +=MaxX/4;
	 tgprint(4,false,x,y,"%2d  %3d",post->pozum[num],post->tesum[num]);
	 gprintf(x,y,"");
//	 gprintf(x,y,"%2d.%-20s %2d  %3d\n",num,post->n_um[num],post->pozum[num],post->tesum[num]);
	}
 *x = 15;
}
void Operate(int* x,int* y,int* lum, int zn, Post* post, App* cmd)
{
 if (zn =='y')
	{
	 ShowSkills(x,y,post);          //operacje dodatkowe przy wyborze umiejetnosci z bazy
	}                           //*lum wskaznik na ilosc p. umiejetnosci (do modyfikacji)
 if (zn =='x')                 //zn wybrana operacja
	{
	 ShowAttributes(x,y,post);
	}
 if (zn =='v')
	{
	 cmd->ShowSkill(x,y,VISIBLE);
	}
 if (zn =='z')
	{
	 gprintf(x,y,"Poziom 0        koszt 0");
	 gprintf(x,y,"Poziom 1        koszt 1");
	 gprintf(x,y,"Poziom 2        koszt 3");
	 gprintf(x,y,"Poziom 3        koszt 6");
	 gprintf(x,y,"Poziom 4        koszt 15");
	 gprintf(x,y,"Poziom 5        koszt 35");
	 gprintf(x,y,"Poziom 6        koszt 75");
	 gprintf(x,y,"Poziom 7        koszt 155");
	 gprintf(x,y,"Poziom 8        koszt 315");
	 gprintf(x,y,"Poziom 9        koszt 635");
	 gprintf(x,y,"Poziom 10        koszt 1275");
	}
 if (zn =='d')
	{
	  if (post->il_um>0)
	  {
		setcolor(LIGHTRED);
		gprintf(x,y,"Usunelem ostatnia umiejetnosc.");
		setcolor(WHITE);
		post->il_um --;
		*lum += Subtract(post->pozum[post->il_um]);  //zwraca tyle punktow ile warty poziom
	  }                                             //usuwanej umiejetnosci
	  else gprintf(x,y,"Usunelem juz wszystkie umiejetnosci.");
	}
}
void SetSkill(App* cmd, Post* post)
{
 int x,y;
 intro("Umiejetnosci",&x,&y);
 gprintf(&x,&y,"Najwazniejsza czesc tworzenia postaci.");
 gprint(false,&x,&y,"Rozdawac umiejetnosci? ");
 int zn = ggetch(zn,&x,&y,YELLOW,LIGHTGREEN);
 intro("Umiejetnosci",&x,&y);
 if (zn == 't')
	{
	 gprintf(&x,&y,"Domyslny poziom 3 - 30 punktow umiejetnosci.");
	 gprintf(&x,&y,"Podaj poziom postaci (1-10): ");          //poziom postaci
	 int poz;
	 takei(&poz);
	 if (poz>10) poz = 10;
	 if (poz<1)  poz = 1;
	 int lum = GetPoz(poz);
	 int z ='e';
	 while ((z != 'k') && (lum>0) && (post->il_um < MAX_UM))
		{
		 take:
		 gprintf(&x,&y,"Nacisnij cokolwiek. . .");getch();
		 intro("Umiejetnosci w Bazie",&x,&y);
		 setcolor(LIGHTGREEN);
		 gprintf(&x,&y,"Oto umiejetnosci:");
		 setcolor(WHITE);
		 cmd->ShowSkill(&x,&y);
		 setcolor(LIGHTBLUE);
		 gprintf(&x,&y,"Masz %d punktow.",lum);
		 setcolor(WHITE);
		 gprintf(&x,&y,"Umiejetnosc numer: %d.",post->il_um);
		 gprintf(&x,&y,"Podaj numer wybranej umiejetnosci (-1 opcje dodatkowe): ");
		 int num;
		 takei(&num);
		 if (num == -1)
			{
			 intro("Opcje Dodatkowe",&x,&y);
			 gprintf(&x,&y,"    MENU OPCJI DODATKOWYCH");
			 gprintf(&x,&y,"Nacisnij y      aby zobaczyc umiejetnosci postaci");
			 gprintf(&x,&y,"Nacisnij x      aby zobaczyc cechy");
			 gprintf(&x,&y,"Nacisnij v      aby zobaczyc dokladnie baze");
			 gprintf(&x,&y,"Nacisnij z      aby zobaczyc koszty poziomow");
			 gprintf(&x,&y,"Nacisnij d      aby usunac ostatnia umiejetnosc");
			 gprintf(&x,&y,"Nacisnij k      aby zakonczyc");
			 gprint(false,&x,&y,"Twoj Wybor__________");
			 int a = ggetch(a,&x,&y);
			 intro("Opcje Dodatkowe",&x,&y);
			 if (a =='k') goto end;
			 Operate(&x,&y,&lum,a,post,cmd);
			 goto take;
			}
		 if ((num>cmd->il_um-1) || (num<0)) {gprintf(&x,&y,"Poza zakresem tablicy umiejetnosci.");goto take;}
		 strcpy(post->n_um[post->il_um],cmd->n_um[num]);
		 gprintf(&x,&y,"Podaj poziom %s: ",post->n_um[post->il_um]);
		 takesh(&post->pozum[post->il_um]);
		 if (post->pozum[post->il_um]>10) post->pozum[post->il_um] = 10;
		 if (post->pozum[post->il_um]<0) post->pozum[post->il_um] = 0;
		 lum -= Subtract(post->pozum[post->il_um]);
		 if (lum<0)
			{
			lum += Subtract(post->pozum[post->il_um]);
			setcolor(RED);
			gprintf(&x,&y,"Za malo punktow !");
			gprintf(&x,&y,"Zobacz koszty -1, z");
			setcolor(WHITE);
			goto take;
			}
		post->tesum[post->il_um] = short((post->cecha[cmd->um[num][TEST_FROM]]*0.5) + (cmd->um[num][MULTIPLE] * post->pozum[post->il_um]));
		ShowSkills(&x,&y,post,post->il_um);
		post->il_um++;
		if (post->il_um == MAX_UM) gprintf(&x,&y,"Rozdano juz maksymalna ilosc umiejetnosci %d.",post->il_um);
		}
	 end:
	 gprintf(&x,&y,"Zakonczylem rozdawanie umiejetnosci.");
	 ShowSkills(&x,&y,post);
	 getch();
	}
 else gprintf(&x,&y,"Nie rozdano umiejetnosci.");
}
void SetHeaders(Post* post)             //informacje naglowkowe
{
 int x,y;
 intro("Dane Glowne",&x,&y);
 setcolor(LIGHTBLUE);
 gprintf(&x,&y,"Podaj imie postaci: ");
 takes(post->imie);
 setcolor(LIGHTBLUE);
 gprintf(&x,&y,"Podaj nazwisko postaci: ");
 takes(post->nazw);
 int pl ='a';
 while ((pl != 'k') && (pl != 'm'))
	{
	 setcolor(LIGHTBLUE);
	 gprint(false,&x,&y,"Podaj plec postaci (m/k): ");
	 pl = ggetch(pl,&x,&y,RED,YELLOW);
	 intro("Plec Postaci",&x,&y);
	 if (pl =='k') strcpy(post->plec,"kobieta");
	 if (pl =='m') strcpy(post->plec,"mezczyzna");
	}
setcolor(WHITE);
}
void SaveHtml(Post* post)             //zapisuje HTML
{
 FILE* save;
 int x,y;
 intro("Zapis w HTML",&x,&y);
 if ((save=fopen(post->proc,"r"))!=NULL)
	{
	 fclose(save);
	 setcolor(YELLOW);
	 gprint(false,&x,&y,"Podany plik %s istnieje, zastapic? ",post->proc);
	 int zn = ggetch(zn,&x,&y);                       //jesli istnieje, zastapic
	 intro("Podany Plik Istnieje",&x,&y);
	 if (zn != 't')
		{
		 gprintf(&x,&y,"Podaj nowa nazwe pliku: ");
		 char n[STRING];
		 strcpy(post->proc,"char\\");
		 takes(n);                   //nowa nazwa
		 strcat(post->proc,n);
		 strcat(post->proc,".html");
		}            //lub zastepuje
	 setcolor(WHITE);
	}
 if ((save=fopen(post->proc,"w"))==NULL)
	{
	 setcolor(RED);
	 intro("Blad Programu",&x,&y);
	 gprintf(&x,&y,"Utworz folder char w katalogu programu.");
	 gprintf(&x,&y,"Nie moge utworzyc pliku %s.",post->proc);  //blad zapisu moze nie ma
	 setcolor(WHITE);
	}                                                      //folderu char
 else
	{
	fprintf(save,"<html>\n");                    //zapis w formacie HTML
	fprintf(save,"<head>\n");
	fprintf(save,"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-2\">\n");
	fprintf(save,"<title>");
	fprintf(save,"Karta Postaci: %s PD %d Srednia %d %%",post->imie,post->PD,post->procent);
	fprintf(save,"</title>\n");
	fprintf(save,"</head>\n");
	fprintf(save,"<body bgcolor=\"#ffffff\" text=\"000000\">\n");
	fprintf(save,"<body>\n");
	fprintf(save,"<font size=\"7\" face=\"mariaged\">\n");
	fprintf(save,"<center>\n");
	fprintf(save,"%s %s\n",post->imie,post->nazw);
	fprintf(save,"</center>\n");
	fprintf(save,"<center>\n");
	fprintf(save,"%s %s\n",post->rasa,post->prof);
	fprintf(save,"</center>\n");
	fprintf(save,"<center>\n");
	fprintf(save,"plec: %s\n",post->plec);
	fprintf(save,"</center>\n");
	fprintf(save,"</font>\n");
	fprintf(save,"<font size=\"4\">\n");
	fprintf(save,"<table border =\"2\">\n");

	fprintf(save,"<tr>\n");
	fprintf(save,"<td>Lp.\n");              //zapis a ? b : c
	fprintf(save,"<td>Plusy\n");                //zwraca b jezeli a jest prawda
	fprintf(save,"<td>Cecha\n");                //inaczej zwraca c
	fprintf(save,"<td>Wartosc\n");          //np (5>3) ? 1 : 0 zwroci 1 gdyz 5>3
	fprintf(save,"<td>Rozwiniecie\n");
	fprintf(save,"<td>Umiejetnosc\n");
	fprintf(save,"<td>Poziom\n");
	fprintf(save,"<td>Test\n");
	fprintf(save,"<td>Uwagi  MG.\n");
	int max = (post->il_um > post->il_cech) ? post->il_um : post->il_cech; //patrz wyzej
	for (int i=0;i<max;i++)
		{
		 fprintf(save,"<tr>\n");
		 fprintf(save,"<td>%d.\n",i);
		 fprintf(save,"<td>%d\n",post->c_pl[i]);
		 if (i<post->il_cech)fprintf(save,"<td>%s\n",post->ncechy[i]); else fprintf(save,"<td>.\n");
		 if (i<post->il_cech)fprintf(save,"<td>%d\n",post->cecha[i]); else fprintf(save,"<td>.\n");
		 fprintf(save,"<td>%d\n",post->u_pl[i]);
		 if (i<post->il_um)fprintf(save,"<td>%s\n",post->n_um[i]); else fprintf(save,"<td>.\n");
		 if (i<post->il_um)fprintf(save,"<td>%d\n",post->pozum[i]); else fprintf(save,"<td>.\n");
		 if (i<post->il_um)fprintf(save,"<td>%d\n",post->tesum[i]); else fprintf(save,"<td>.\n");
		 fprintf(save,"<td>.\n");
		}
	fprintf(save,"</table>\n");
	fprintf(save,"<pre>\n\t\tTen Program napisal Morgoth DBMA.</pre>\n");
	fprintf(save,"</body>\n");
	fprintf(save,"</html>\n");
	gprintf(&x,&y,"Zapisalem postac %s %s w %s.",post->imie,post->nazw,post->proc);
	fclose(save);
	}
}
void ProtectDB(App* cmd)            //ochraniaj baze danych jezeli jest kod pytaj
{                                   //nie daj go zmodyfikowac na tym etapie
 int x,y;
 intro("Poziom Zabezpieczen Bazy Danych",&x,&y);
 if (cmd->pass != 0)
	{
	 setcolor(RED);
	 gprintf(&x,&y,"   Baza danych %s jest chroniona kodem.",cmd->proc);
	 gprintf(&x,&y,"Aby jej uzywac podaj kod dostepu: ");
	 ULONG kod;
	 takel(&kod,RED);
	 if (kod == cmd->pass)
		{
		 setcolor(WHITE);
		 gprintf(&x,&y,"Podales poprawny kod. Zezwalam na uzywanie bazy danych.");
		}
	 else
		{
		 setcolor(RED);
		 gprintf(&x,&y,"Zabraniam uzycia bazy %s.",cmd->proc);
		 KILL();
		}
	}
 else gprintf(&x,&y,"Baza danych %s nie chroniona kodem.",cmd->proc);
}
void SetOwnAtt(Post* post,App* cmd)                           //rozdaj wlasne plusy / minusy
{
 int x,y;
 intro("Wlasne plusy i minusy",&x,&y);
 gprintf(&x,&y,"Podaj 0 aby pominac.");
 gprintf(&x,&y,"Podaj ilosc plusow dla postaci (0-3): ");
 short pl;
 short temp;
 short rem = -123;
 takesh(&pl);
 if (pl<0) pl = 0;
 if (pl>3) pl = 3;
 temp = pl;
 while (pl > 0)
	{
	 intro("Oto Cechy Postaci",&x,&y);
	 ShowAttributes(&x,&y,post);
	 short num;              //rem poprzednia zwiekszana cecha
	 do                          //nie daj drugi raz gdyz zwiekszyloby to ceche o 3
		{
		 gprintf(&x,&y,"Podaj numer cechy do zwiekszania: ");
		 takesh(&num);
		 if (num == rem) gprintf(&x,&y,"Te ceche zwiekszales !");
		}
	 while (num == rem);
	 short ile;
	 if (num<0) num = 0;
	 if (num>post->il_cech - 1) num = short(post->il_cech - 1);  //numer cechy
	 rem = num;
	 gprintf(&x,&y,"O ile zwiekszyc %s: ",post->ncechy[num]);
	 takesh(&ile);
	 if (ile < 1) ile = 1;                   //o ile zwiekszyc
	 if (ile > 2) ile = 2;
	 pl -=ile;
	 post->cecha[num] += ile*cmd->Plus;
	 gprintf(&x,&y,"Zwiekszylem %s o %d.",post->ncechy[num],ile);
	}
 pl = temp;                    //tyle zmniejszen ile zwiekszen
 rem = -123;                   //poprzednio zmniejszano ceche numer -123
 while (pl > 0)                    //liczba -123 nie istotna aby poza tabela cech !
	{
	 intro("Oto Cechy Postaci",&x,&y);
	 ShowAttributes(&x,&y,post);
	 short num;
	 do
		{
		 gprintf(&x,&y,"Podaj numer cechy do zmniejszania: ");
		 takesh(&num);
		 if (num == rem) gprintf(&x,&y,"Te ceche zmniejszales !");
		}
	 while (num == rem);
	 short ile;                            //patrz wyzej.
	 if (num<0) num = 0;
	 if (num>post->il_cech - 1) num = short(post->il_cech - 1);
	 rem = num;
	 gprintf(&x,&y,"O ile zmniejszyc %s: ",post->ncechy[num]);
	 takesh(&ile);
	 if (ile < 1) ile = 1;
	 if (ile > 2) ile = 2;
	 pl -=ile;
	 post->cecha[num] -= ile*cmd->Plus;
	 gprintf(&x,&y,"Zmniejszylem %s o %d.",post->ncechy[num],ile);
	}
}
void SeeAll(Post* post)
{
 unsigned long suma = 0;
 for (int i=0;i<post->il_cech;i++)
	{
	 suma += post->cecha[i];
	}

	suma *= 100;
	suma /=post->il_cech*post->avv;
	post->procent = suma;
 int x,y;
 intro("Karta Postaci",&x,&y);
 x = 15;
 gprint(false,&x,&y,"Imie :%s",post->imie); x += MaxX/2;
 gprint(false,&x,&y,"Nazwisko :%s",post->nazw);gprintf(&x,&y,"");
 x = 15;
 gprint(false,&x,&y,"Rasa :%s",post->rasa); x += MaxX/2;
 gprint(false,&x,&y,"Profesja :%s",post->prof);gprintf(&x,&y,"");
 x = MaxX/2 - 200;
 gprintf(&x,&y,"Plec :%s   PD: %d  Srednia %d %%",post->plec,post->PD,post->procent); x += MaxX/2;
 x = 15;
 ShowAttributes(&x,&y,post);
 gprintf(&x,&y,"Umiejetnosci:");
 ShowSkills(&x,&y,post);
 getch();
}
bool checkLS(int val ,int poz)
{
 if ((poz==1) && (val==3)) return true;
 if ((poz==2) && (val==5)) return true;
 if ((poz==3) && (val==8)) return true;
 if ((poz==4) && (val==13)) return true;
 if ((poz==5) && (val==21)) return true;
 if ((poz==6) && (val==34)) return true;
 if ((poz==7) && (val==55)) return true;
 if ((poz==8) && (val==89)) return true;
 if ((poz==9) && (val==144)) return true;
 if ((poz==10) && (val==243)) return true;
 return false;
}
bool checkLA(int val ,int cecha)
{
 if (val > (cecha/2+2)) return true;
 return false;
}
void PlusA(Post* p,int num)
{
 p->c_pl[num]++;
 bool LevelUp = checkLA(p->c_pl[num],p->cecha[num]);
 if (LevelUp == true)
	{
	 int x,y;
	 intro("Level Up!!!",&x,&y);
	 p->c_pl[num] = 0;
	 p->cecha[num]++;
	 getch();
	}
}
void PlusS(Post* p,int num)
{
 p->u_pl[num]++;
 bool LevelUp = checkLS(p->u_pl[num],p->pozum[num]);
 if (LevelUp == true)
	{
	 int x,y;
	 intro("Level Up!!!",&x,&y);
	 p->u_pl[num] = 0;
	 p->pozum[num]++;
	 gprintf(&x,&y,"O ile zwiekszyc test %s?",p->n_um[num]);
	 int ile;
	 takei(&ile);
	 p->tesum[num] += ile;
	}
}
void AttAdd(Post* p)
{
 int x,y;
 intro("Cechy",&x,&y);
 ShowAttributes(&x,&y,p);
 gprintf(&x,&y,"Wybierz numer cechy do ktorej dodac plus:");
 int num;
 takei(&num);
 if (num<0) num = 0;
 if (num>p->il_cech-1) num = p->il_cech-1;
 PlusA(p,num);
 p->PD++;
}
void SkiAdd(Post* p)
{
 int x,y;
 intro("Umiejetnosci",&x,&y);
 ShowSkills(&x,&y,p);
 gprintf(&x,&y,"Wybierz numer umiejetnosci do ktorej dodac plus:");
 int num;
 takei(&num);
 if (num<0) num = 0;
 if (num>p->il_um-1) num = p->il_um-1;
 PlusS(p,num);
 p->PD++;
}
void AboutSave(Post* p)
{
 int x,y;
 intro("Zmiany w postaci",&x,&y);
 gprint(false,&x,&y,"Zachowac zmiany? ");
 int zn = ggetch(zn,&x,&y,LIGHTBLUE,LIGHTGREEN);
 intro("Zapis",&x,&y);
 if (zn == 't')
	{
	 p->SavePost();
	 SaveHtml(p);
	}
 else gprintf(&x,&y,"Nie zapisalem zmian !");
}
void Experience(Post* p)
{
 int x,y;
 intro("Odczytalem. . .",&x,&y);
 SeeAll(p);
 intro("Rozdawanie Plusow",&x,&y);
 gprint(false,&x,&y,"Czy jestes mistrzem Gry? ");
 int zn = ggetch(zn,&x,&y);
 intro("Rozdawanie Plusow",&x,&y);
 if (zn == 't')
	{
	 int pl;
	 gprintf(&x,&y,"Podaj ilosc plusow (0-20): ");
	 takei(&pl);
	 if (pl<0) pl = 0;
	 if (pl>20) pl = 20;
	 while (pl>0)
		{
		 intro("Cecha czy umiejetnosc:",&x,&y);
		 gprintf(&x,&y,"Pozostalo plusow: %d",pl);
		 gprint(false,&x,&y,"plus do cechy (c), czy do umiejetnosci (u)");
		 int zn = ggetch(zn,&x,&y);
		 intro("Plusy. . .",&x,&y);
		 switch(zn)
			{
			 case 'c': AttAdd(p);pl--;break;
			 case 'u': SkiAdd(p);pl--;break;
			}
		}
	 gprintf(&x,&y,"Koniec plusow !");
	 SeeAll(p);
	 AboutSave(p);
	}
 Pozegnanie();
 KILL();
}
void TryOpen(Post* post)
{
 FILE* load;
 if ((load = fopen(post->save,"r"))!=NULL)
	{
	 int x,y;
	 intro("Odczyt Pliku",&x,&y);
	 gprintf(&x,&y,"Odczytalem %s.",post->save);
	 post->LoadPost(load);
	 fclose(load);
	 Experience(post);
	}
}
void Post :: SavePost()
{
 FILE* sav;
 sav = fopen(save,"w");
 WritePost(sav);
 fclose(sav);
 int x,y;
 intro("Zapis Pliku",&x,&y);
 gprintf(&x,&y,"Zapisalem %s.",save);
}
void CreateChar(App* cmd, Post* post)     //stworz postac post na podstawie cmd
{
 int x,y;
 intro("Tworzenie Postaci",&x,&y);
 ProtectDB(cmd);                                 //chron baze
 intro("Nazwa Pliku",&x,&y);
 gprintf(&x,&y,"Podaj nazwe pliku do zapisu / odczytu: ");
 char nazw[STRING];                      //pobierz nazwe pliku HTML
 takes(nazw);
 strcpy(post->proc,"char\\");
 strcat(post->proc,nazw);
 if (strcmp(post->proc,"char\\")==0) strcpy(post->proc,"char\\postac_1");
 strcpy(post->save,post->proc);
 strcat(post->proc,".html");
 strcat(post->save,".chr");
 GetValuesDB(cmd, post);             //pobierz dane z bazy
 TryOpen(post);
 SetRace(post,cmd);
 SetProf(post,cmd);
 post->SavePost();
 SetRandom(post,cmd);
 post->SavePost();
 SetOwnAtt(post,cmd);
 post->SavePost();
 SetSkill(cmd, post);
 post->SavePost();
 SetHeaders(post);
 post->SavePost();
 SaveHtml(post);                   //zapisz w HTML
 SeeAll(post);
}
Post :: Post(short par)           //konstruktor postaci domyslny
{
 il_cech = MAX_CECH;         //il cech maksymalna
 il_um = 0;
 suma = 0;                //umiejetnosci i suma = 0
 PD = 0;
 procent = 100;
 avv = par;
 strcpy(imie,"_");
 strcpy(plec,"_");            //imie, nazw i plec nie znane :)8-< che! che!
 strcpy(nazw,"_");
 strcpy(rasa,"_");
 strcpy(prof,"_");
 for (int i=0;i<MAX_CECH;i++)
	{
	 cecha[i] = par;                  //wszystkie cechy o wartosci parametru przekazywanego do
	 strcpy(ncechy[i],"_");
	 c_pl[i] = 0;
	}                                 //konstruktora
 for (int i=0;i<MAX_UM;i++)
	{
	 strcpy(n_um[i],"_");            //umiejetnosci puste
	 pozum[i] = 0;
	 tesum[i] = 0;
	 u_pl[i] = 0;
	}
}
void CChr(App* cmd, Post* post)
{
CreateChar(cmd, post);
}
int main()   //Tu program zaczyna wykonanie , nowy obiekt wola konstruktora App :: App
{                                 //tworzenie dynamiczne uzywa swobodnej RAM
 Initialize();
 App* TApplication = new App;     //TApplication to struktura bazy danych obslugujaca takze tworzenie postaci
 TApplication->Run();             //virtual Run (uruchamia aplikacje)
 delete TApplication;             //zwalnia wskaznik i pamiec RAM
 closegraph();
 return 0;                       //zwraca do systemu 0
}
