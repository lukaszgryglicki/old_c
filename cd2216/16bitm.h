#include "mh16d.h"


#define MAX_CD			1200

#define N_CDC		0x10
#define N_CD      0x11
#define N_CDD     0x12
#define N_COPY    0x13
#define N_LP      0x14
#define N_MC      0x15
#define N_INNY    0x16

#define T_H_METAL 0x17
#define T_T_METAL 0x18
#define T_D_METAL 0x19
#define T_P_METAL 0x1a
#define T_ROCK    0x1b
#define T_H_ROCK  0x1c
#define T_H_CORE  0x1d
#define T_POP     0x1e
#define T_INNY    0x1f


class DosApp
{
 public:

 DosApp();
 ~DosApp();
 void Run();

 private:

 void Execute(char*);
 void Dodaj();
 void Usun();
 void PokazZespoly();
 void Nosnik(int);
 void Typ(int);
 void UsunZespol(char*);
 void UsunPlyteZ(char*);
 void PokazWszystko();
 void PokazZespol(char*,char);
 void ZapiszZespol(char*,FILE*);
 void PokazLinie(int);
 void ZapiszLinie(int,FILE*);
 void PokazTyp(int);
 void PokazNosnik(int);
 void ZapiszTyp(int,FILE*);
 void ZapiszNosnik(int,FILE*);
 void ZobaczZespol();
 void PoliczWartosc();
 void Oceny();
 void SzukajPlyty();
 void SzukajZespolu();
 void Zmien();
 void Modyfikuj(int);
 void CzyZapisac();
 void ZapiszBinaria();
 void ZapiszPlikTekstowy();
 void ZapiszWszystko(FILE*);
 void WczytajBinaria();


 int il_zesp;
 long wartosc;
 int il_cd;

 char nazwa[MAX_CD][STRING];
 char plyta[MAX_CD][STRING2];
 unsigned char nosnik[MAX_CD];
 unsigned char typ[MAX_CD];
 unsigned char ocena[MAX_CD];
 unsigned char cena[MAX_CD];
 char npliku[STRING];
};

DosApp :: DosApp()
{
 cout<<"Tworze glowna klase...\n";
 il_zesp = 0;
 wartosc = 0;
 il_cd   = 0;
 strcpy(npliku,"Nie_Zapisany");
 for (int i=0;i<MAX_CD;i++)
 	{
    strcpy(nazwa[i],"UNKNOWN");
    strcpy(nazwa[i],"UNTITLED");
    nosnik[i] = N_CD;
    typ[i]    = T_ROCK;
    ocena[i]  = 5;
    cena[i]   = 50;
   }
}
DosApp :: ~DosApp()
{
 cout<<"Nastepuje teraz destrukcja obiektu...\n";
}

void DosApp :: Run()
{
 char cmd[STRING];
 strcpy(cmd,"NO_COMMAND_YET");
 cout<<"Wpisz pomoc(h), aby zobaczyc spis polecen.\n";
 while ((strcmp(cmd,"k") != 0) && (strcmp(cmd,"koniec") != 0))
 	{
    cout<<"Linux.morgothdbma(2.2) "<<npliku<<" ->> ";
    takestring(cmd);
    Execute(cmd);
   }
}
void Wersja()
{
 lns(2);
 cout<<"CDD16DOS.EXE wersja 2.2 + przegladarka\n";
 cout<<"16 bit max. pamieci 64k (65536 bajtow)\n";
 cout<<"Max zespolow 1000, prawa: kopiowanie, powielanie\n";
 cout<<"Fragmentowanie zezwolone (nawet zalecane).Program\n";
 cout<<"Napisany w Borland C++ 5.02 Real Dos Mode 16bit\n";
 cout<<"Mem Huge, pelny kod zrodlowy to pliki cdd16dos.cpp\n";
 cout<<"16bitmodule.h, mh16d.h oraz plik projektu cdd16dos.ide\n";
 cout<<"Powinny byc dolaczone. Autor morgothdbma, copyleft&^\n";
 cout<<"Kompilacja z dnia 21.06.2001r. godz. 10.16\n";
 lns(2);
}
void Skala()
{
 cout<<"\n10\t-Zloto\n";
 cout<<"9\t-Srebro\n";
 cout<<"8\t-Braz\n";
 cout<<"7\t-Wyrozniajaca sie plyta\n";
 cout<<"6\t-Ciekawa plyta\n";
 cout<<"5\t-Mozna posluchac\n";
 cout<<"4\t-Cierpliwie wysluchac\n";
 cout<<"3\t-Wlaczyc i zajac sie czyms innym\n";
 cout<<"2\t-Nieudana plyta\n";
 cout<<"1\t-Nie warto spojrzec\n\n";
}
void HelpMe()
{
 lns(2);
 cout<<"Spis polecen, polecenie a w nawiasie jego skrot\n";
 cout<<"\tpomoc(h)\t-wyswietla ten tekst\n";
 cout<<"\tkoniec(k)\t-konczy program (ALE NAJPIERW ZAPISZ DANE W PLIKU!)\n";
 cout<<"\tdodaj(d)\t-dodaje plyte lub zespol\n";
 cout<<"\tusun(u) \t-usuwa plyte lub zespol\n";
 cout<<"\tpokazz(pz) \t-pokazuje zespoly\n";
 cout<<"\tpokazw(pw) \t-pokazuje wszystko\n";
 cout<<"\todswiez(ods) \t-niszczy rekord(I DANE REKORDU AKTUALNEGO TRACISZ!!)\n";
 cout<<"\tzobaczz(zz) \t-zobacz zespol\n";
 cout<<"\tskala(s) \t-skala ocen\n";
 cout<<"\toceny(oc) \t-wyswietla oceny poszczegolnych plyt\n";
 cout<<"\twartosc(lw) \t-oblicza wartosc kolekcji\n";
 cout<<"\tszukajp(sp) \t-poszukuje plyty w/g wzorca\n";
 cout<<"\tszukajz(sz) \t-poszukuje zespolu w/g wzorca\n";
 cout<<"\tmodyfikuj(m) \t-modyfikuje wybrana plyta wybranego zespolu\n";
 cout<<"\tzapisz(z) \t-zapisuje dane\n";
 cout<<"\twczytaj(w) \t-wczytuje dane\n";
 cout<<"\twersja(v) \t-wyswietla wersje programu\n";
 lns(2);
}
void DosApp :: Execute(char* cmd)
{
 if ((strcmp(cmd,"h") == 0) || (strcmp(cmd,"pomoc") == 0))         HelpMe();
 else if ((strcmp(cmd,"k") == 0) || (strcmp(cmd,"koniec") == 0))   CzyZapisac();
 else if ((strcmp(cmd,"d") == 0) || (strcmp(cmd,"dodaj") == 0))    Dodaj();
 else if ((strcmp(cmd,"u") == 0) || (strcmp(cmd,"usun") == 0))     Usun();
 else if ((strcmp(cmd,"pz") == 0) || (strcmp(cmd,"pokazz") == 0))  PokazZespoly();
 else if ((strcmp(cmd,"pw") == 0) || (strcmp(cmd,"pokazw") == 0))  PokazWszystko();
 else if ((strcmp(cmd,"zz") == 0) || (strcmp(cmd,"zobaczz") == 0)) ZobaczZespol();
 else if ((strcmp(cmd,"ods") == 0) || (strcmp(cmd,"odswiez") == 0)){il_cd = 0;strcpy(npliku,"Nie_Zapisany");printf("%c%c\tDane zniszczone!\n",1,2);}
 else if ((strcmp(cmd,"s") == 0) || (strcmp(cmd,"skala") == 0))    Skala();
 else if ((strcmp(cmd,"oc") == 0) || (strcmp(cmd,"oceny") == 0))   Oceny();
 else if ((strcmp(cmd,"lw") == 0) || (strcmp(cmd,"wartosc") == 0)) PoliczWartosc();
 else if ((strcmp(cmd,"sp") == 0) || (strcmp(cmd,"szukajp") == 0)) SzukajPlyty();
 else if ((strcmp(cmd,"sz") == 0) || (strcmp(cmd,"szukajz") == 0)) SzukajZespolu();
 else if ((strcmp(cmd,"m") == 0) || (strcmp(cmd,"modyfikuj") == 0))Zmien();
 else if ((strcmp(cmd,"z") == 0) || (strcmp(cmd,"zapisz") == 0))   {ZapiszBinaria();ZapiszPlikTekstowy();}
 else if ((strcmp(cmd,"w") == 0) || (strcmp(cmd,"wczytaj") == 0))  WczytajBinaria();
 else if ((strcmp(cmd,"v") == 0) || (strcmp(cmd,"wersja") == 0))   Wersja();

 else cout<<"Nieznane polecenie, wpisz pomoc(h)\n";
}
void DosApp :: PoliczWartosc()
{
 wartosc = 0;
 for (int i=0;i<il_cd;i++)
        {
        wartosc += cena[i];
        }
 cout<<"Razem wszystkich plyt "<<il_cd<<", ogolny koszt kolekcji "<<wartosc<<" zl."<<endl;
}
void DosApp :: Oceny()
{
 cout<<"Podaj kotegorie oceny od jakiej pokazywac: ";
 int cat;
 takeint(&cat);
 cout<<"W gore, czy w dol: ";
 char zn;
 takechar(&zn);
 int disp = 0;
 for (int i=0;i<il_cd;i++)
        {
         if (((ocena[i] >= cat) && (zn == 'g')) || ((ocena[i] <= cat) && (zn == 'd')))
               {
                PokazLinie(i);
                disp++;
                if (disp == 18) {disp = 0;cout<<"Nacisnij cokolwiek aby kontynuowac...\n";getch();ln();}
               }
        }
}
void DosApp :: Dodaj()
{
 cout<<"Aktualnie plyt "<<il_cd<<endl;
 if (il_cd == MAX_CD)
 	{
    cout<<"Nie mozesz dodac plyty/zespolu, poniewaz rekord zawiera juz "<<MAX_CD<<" danych\n";
    cout<<"Jest to maksimum dla 16bit aplikacji, 64KB\n";
    return;
   }
 char next = 'n';
 nastepna:
 int current = il_cd;
 if (next == 't') strcpy(nazwa[current],nazwa[current-1]);
 else
 {
 cout<<"Podaj nazwe zespolu: ";
 takestring(nazwa[current]);
 }
 for (int i=0;i<il_cd;i++)
 	{
    if (strcmp(nazwa[i],nazwa[current]) == 0)
    	{
       cout<<"Zespol "<<nazwa[current]<<" ma juz plyte "<<plyta[i]<<endl;
      }
   }
 cout<<"Podaj nazwe plyty: ";
 takes(plyta[current],STRING2);
  for (int i=0;i<il_cd;i++)
 	{
    if ((strcmp(plyta[i],plyta[current]) == 0) && (strcmp(nazwa[i],nazwa[current]) == 0))
    	{
       cout<<"Juz istnieje "<<nazwa[i]<<" \""<<plyta[i]<<"\", kontynuowac? ";
       char zn;
       takechar(&zn);
       if (zn != 't') return;
      }
   }
 Nosnik(current);
 Typ(current);
 cout<<"Podaj cene "<<nazwa[current]<<" \""<<plyta[current]<<"\": ";
 takeuchar(&cena[current]);
 cout<<"Podaj ocene "<<nazwa[current]<<" \""<<plyta[current]<<"\": ";
 takeuchar(&ocena[current]);
 il_cd++;
 cout<<"Dodac kolejna plyta dla zespolu "<<nazwa[current]<<"? ";
 takechar(&next);
 if (next == 't')goto nastepna;
}
void DosApp :: Modyfikuj(int current)
{
 cout<<"Modyfikowac nazwe zespolu? ";
 char zk;
 takechar(&zk);
 if (zk == 't')
 {
 cout<<"Podaj nazwe zespolu: ";
 takestring(nazwa[current]);
 }
 cout<<"Podaj nazwe plyty: ";
 takes(plyta[current],STRING2);
 Nosnik(current);
 Typ(current);
 cout<<"Podaj cene "<<nazwa[current]<<" \""<<plyta[current]<<"\": ";
 takeuchar(&cena[current]);
 cout<<"Podaj ocene "<<nazwa[current]<<" \""<<plyta[current]<<"\": ";
 takeuchar(&ocena[current]);
}
void DosApp :: PokazTyp(int num)
{
 switch(typ[num])
 	{
    case T_H_METAL: cout<<"H.Metal ";break;
    case T_D_METAL: cout<<"D.Metal ";break;
    case T_T_METAL: cout<<"T.Metal ";break;
    case T_P_METAL: cout<<"P.Metal ";break;
    case T_ROCK:    cout<<"Rock    ";break;
    case T_H_ROCK:  cout<<"H.Rock  ";break;
    case T_H_CORE:  cout<<"H.Core  ";break;
    case T_POP:     cout<<"Pop     ";break;
    case T_INNY:    cout<<"Inn.typ ";break;
    default: cout<<"Inn.typ ";
   }
}
void DosApp :: ZapiszTyp(int num,FILE* plik)
{
 switch(typ[num])
 	{
    case T_H_METAL: fprintf(plik,"H.Metal ");break;
    case T_D_METAL: fprintf(plik,"D.Metal ");break;
    case T_T_METAL: fprintf(plik,"T.Metal ");break;
    case T_P_METAL: fprintf(plik,"P.Metal ");break;
    case T_ROCK:    fprintf(plik,"Rock    ");break;
    case T_H_ROCK:  fprintf(plik,"H.Rock  ");break;
    case T_H_CORE:  fprintf(plik,"H.Core  ");break;
    case T_POP :    fprintf(plik,"Pop     ");break;
    case T_INNY:    fprintf(plik,"Inn.typ ");break;
    default:fprintf(plik,"Inn.typ ");
   }
}
void DosApp :: Typ(int num)
{
 cout<<"Wybierz typ muzyki: \n";
 cout<<"1\t-Heavy Metal\n";
 cout<<"2\t-Thrash Metal\n";
 cout<<"3\t-Death Metal\n";
 cout<<"4\t-Power Metal\n";
 cout<<"5\t-Rock\n";
 cout<<"6\t-Hard Rock\n";
 cout<<"7\t-Hard Core\n";
 cout<<"8\t-Pop\n";
 cout<<"Dowolna inna liczba - inny typ\n";
 cout<<"Podaj liczbe: ";
 int lb;
 takeint(&lb);
 switch(lb)
 	{
    case 1: typ[num] = T_H_METAL;break;
    case 2: typ[num] = T_T_METAL;break;
    case 3: typ[num] = T_D_METAL;break;
    case 4: typ[num] = T_P_METAL;break;
    case 5: typ[num] = T_ROCK;break;
    case 6: typ[num] = T_H_ROCK;break;
    case 7: typ[num] = T_H_CORE;break;
    case 8: typ[num] = T_POP;break;
    default: typ[num] = T_INNY;
   }
}
void DosApp :: PokazNosnik(int num)
{
 switch(nosnik[num])
 	{
    case N_CDC:    cout<<"CD(cp) ";break;
    case N_CD:     cout<<"CD     ";break;
    case N_CDD:    cout<<"CDD    ";break;
    case N_COPY:   cout<<"MC(cp) ";break;
    case N_LP:     cout<<"LP     ";break;
    case N_MC:     cout<<"MC     ";break;
    case N_INNY:    cout<<"inny   ";break;
    default:cout<<"inny   ";
   }
}
void DosApp :: ZapiszNosnik(int num,FILE* plik)
{
 switch(nosnik[num])
 	{
    case N_CDC:   fprintf(plik,"CD(cp) ");break;
    case N_CD:    fprintf(plik,"CD     ");break;
    case N_CDD:   fprintf(plik,"CDD    ");break;
    case N_COPY:  fprintf(plik,"MC(cp) ");break;
    case N_LP:    fprintf(plik,"LP     ");break;
    case N_MC:    fprintf(plik,"MC     ");break;
    case N_INNY:  fprintf(plik,"inny   ");break;
    default:fprintf(plik,"inny   ");
   }
}
void DosApp :: Nosnik(int num)
{
 cout<<"Wybierz typ nosnika: \n";
 cout<<"1\t-CD kopia\n";
 cout<<"2\t-CD\n";
 cout<<"3\t-CDD\n";
 cout<<"4\t-MC kopia\n";
 cout<<"5\t-LP\n";
 cout<<"6\t-MC\n";
 cout<<"Dowolna inna liczba - inny nosnik\n";
 cout<<"Podaj liczbe: ";
 int lb;
 takeint(&lb);
 switch(lb)
 	{
    case 1: nosnik[num] = N_CDC;break;
    case 2: nosnik[num] = N_CD;break;
    case 3: nosnik[num] = N_CDD;break;
    case 4: nosnik[num] = N_COPY;break;
    case 5: nosnik[num] = N_LP;break;
    case 6: nosnik[num] = N_MC;break;
    default: nosnik[num] = N_INNY;
   }

}
void DosApp :: Usun()
{
 if (il_cd == 0)
 	{
    cout<<"Nie ma nic do usuniecia, rekord pusty!\n";
    return;
   }
 cout<<"Usunac zespol czy plyte\njezeli chcesz usunac zespol wpisz z\n";
 cout<<"Jezeli chcesz usunac plyte wpisz p\n";
 cout<<"Podaj odpowiednia litera: ";
 char zn;
 takechar(&zn);
 if (zn == 'z')
 	{
    char end = 'n';
    while (end == 'n')
    {
    PokazZespoly();
    cout<<"Podaj nazwe zespolu do usuniecia: ";
    char name[STRING];
    takestring(name);
    UsunZespol(name);
    cout<<"Zakonczyc usuwanie? ";
    takechar(&end);
    }
   }
 if (zn == 'p')
 	{
    PokazZespoly();
    cout<<"Podaj nazwe zespolu: ";
    char name[STRING];
    takestring(name);
    char end = 'n';
    while (end == 'n')
    {
    UsunPlyteZ(name);
    cout<<"Zakonczyc usuwanie plyt z zespolu "<<name<<"? ";
    takechar(&end);
    }
   }
}
void DosApp :: PokazZespoly()
{
 lns(2);
 il_zesp = 0;
 int is[MAX_CD];
 for (int i=0;i<MAX_CD;i++) is[i] = 1;
 int disp = 0;
 for (int i=0;i<il_cd;i++)
 	{
    for (int j=0;j<i;j++)
    	{
       if (strcmp(nazwa[j],nazwa[i]) == 0) is[i]++;
      }
    if (is[i] == 1) {printf("%-20s\t",nazwa[i]);disp++;il_zesp++;if (!(disp % 3)) ln();}
    if (disp == 54) {cout<<"Nacisnij cokolwiek aby kontynuowac...";getch();ln();disp = 0;}
   }
  cout<<"\nIlosc zespolow wynosi "<<il_zesp<<endl;
 lns(2);
}
void DosApp :: PokazLinie(int num)
{
 printf("%-16s %-22s ",nazwa[num],plyta[num]);
 PokazTyp(num);
 PokazNosnik(num);
 printf(" oc:%2d ce:%2d zl\n",ocena[num],cena[num]);
}
void DosApp :: ZapiszLinie(int num,FILE* plik)
{
 fprintf(plik,"%-16s %-22s ",nazwa[num],plyta[num]);
 ZapiszTyp(num,plik);
 ZapiszNosnik(num,plik);
 fprintf(plik," oc:%2d ce:%2d zl\n",ocena[num],cena[num]);
}
void DosApp :: PokazZespol(char* zespol,char stop)
{
 cout<<"\tZespol "<<zespol<<endl<<endl;
 int lines = 0;
 for (int i=0;i<il_cd;i++)
 	{
    if (strcmp(zespol,nazwa[i]) == 0)
    	{
       lines++;
       PokazLinie(i);
       if (lines >= 16) {lines = 0;if (stop != 'p'){cout<<"Nacisnij cokolwiek aby kontynuowac...\n";getch();}ln();}
      }
   }
}
void DosApp :: ZapiszZespol(char* zespol,FILE* plik)
{
 fprintf(plik,"\t%s\n\n",zespol);
 for (int i=0;i<il_cd;i++)
 	{
    if (strcmp(zespol,nazwa[i]) == 0)
    	{
       ZapiszLinie(i,plik);
      }
   }
}
void DosApp :: PokazWszystko()
{
 lns(2);
 char p = 'n';
 il_zesp = 0;
 int is[MAX_CD];
 for (int i=0;i<MAX_CD;i++) is[i] = 1;
 for (int i=0;i<il_cd;i++)
 	{
    for (int j=0;j<i;j++)
    	{
       if (strcmp(nazwa[j],nazwa[i]) == 0) is[i]++;
      }
    if (is[i] == 1) {PokazZespol(nazwa[i],p);il_zesp++;if (p != 'p'){cout<<"Nacisnij cos aby kontynuowac lub p aby pominac..."; p = getch();}ln();}
   }
  cout<<"\nIlosc zespolow wynosi "<<il_zesp<<endl;
 lns(2);
}
void DosApp :: UsunZespol(char* name)
{
 cout<<"Usuwam rekordy o nazwie "<<name<<endl;
 for (int i=0;i<il_cd;i++)
 	{
    if (strcmp(name,nazwa[i]) == 0)
    	{
       il_cd--;
       for (int j=i;j<il_cd;j++)
       	{
          strcpy(nazwa[j],nazwa[j+1]);
          strcpy(plyta[j],plyta[j+1]);
          nosnik[j]   =  nosnik[j+1];
          typ[j]      =  typ[j+1];
          ocena[j]    =  ocena[j+1];
          cena[j]     =  cena[j+1];
         }
       i = 0;
      }
   }
    if (strcmp(name,nazwa[0]) == 0)
    	{
       il_cd--;
       for (int j=0;j<il_cd;j++)
       	{
          strcpy(nazwa[j],nazwa[j+1]);
          strcpy(plyta[j],plyta[j+1]);
          nosnik[j]   =  nosnik[j+1];
          typ[j]      =  typ[j+1];
          ocena[j]    =  ocena[j+1];
          cena[j]     =  cena[j+1];
         }
      }
}
void DosApp :: UsunPlyteZ(char* zespol)
{
 int num = 0;
 int search = 0;
 for (int i=0;i<il_cd;i++)
 	{
    if (strcmp(nazwa[i],zespol) == 0)
    	{
       num++;
       cout<<num<<")\t"<<plyta[i]<<endl;
       if (!(num % 18)) {cout<<"Nacisnij cokolwiek aby kontynuowac...";getch();ln();}
      }
   }
 cout<<"Podaj numer plyty do usuniecia: ";
 search = num;
 takeint(&num);
 if ((num <0) || (num>search)) {cout<<"Poza zakresem tablicy.\n";return;}
 search = 0;
 for (int i=0;i<il_cd;i++)
 	{
    if (strcmp(nazwa[i],zespol) == 0)
    	{
       search++;
       if (search == num)
       	{
          il_cd--;
          for (int j=i;j<il_cd;j++)
          	{
          	 strcpy(nazwa[j],nazwa[j+1]);
          	 strcpy(plyta[j],plyta[j+1]);
          	 nosnik[j]   =  nosnik[j+1];
          	 typ[j]      =  typ[j+1];
          	 ocena[j]    =  ocena[j+1];
          	 cena[j]     =  cena[j+1];
            }
         }
      }
   }
}
void DosApp :: ZobaczZespol()
{
 PokazZespoly();
 cout<<"Podaj nazwe zespolu: ";
 char name[STRING];
 takestring(name);
 PokazZespol(name,'n');
}
void DosApp :: SzukajPlyty()
{
 cout<<"Podaj pierwsze litery nazwy plyty: ";
 char data[STRING];
 takestring(data);
 if (strcmp(data,"") == 0) return;
 cout<<"Znalazlem nastepujace plyty:\n";
 int k = 0;
 int lines = 0;
 for (int i=0;i<il_cd;i++)
        {
         k = 0;
         for (int j=0;j<(int)strlen(data);j++)
                {
                 if ((data[j] >=0x41) && (data[j] <=0x5a))
                 	{
                   if (plyta[i][j] == data[j]) k++;
                   if (plyta[i][j] == data[j] + 0x20) k++;
                  }
                 else if ((data[j] >=0x61) && (data[j] <=0x7a))
                 	{
                   if (plyta[i][j] == data[j]) k++;
                   if (plyta[i][j] == data[j] - 0x20) k++;
                  }
                 else
                 	{
                   if (plyta[i][j] == data[j]) k++;
                  }
                }
         if (k == (int)strlen(data))
         	{
             PokazLinie(i);
             lines++;
             if (lines == 18){cout<<"Nacisnij cokolwiek aby kontynuowac...\n";getch();ln();lines = 0;}
            }
        }
}
void DosApp :: SzukajZespolu()
{
 lns(2);
 cout<<"Podaj kilka pierwszych liter nazwy zespolu: ";
 char data[STRING];
 takestring(data);
 if (strcmp(data,"") == 0) return;
 cout<<"Znalazlem nastepujace zespoly:\n";
 int k = 0;
 int lines = 0;
 int is[MAX_CD];
 for (int i=0;i<MAX_CD;i++) is[i] = 1;
 int disp = 0;
 for (int i=0;i<il_cd;i++)
 	{
    for (int j=0;j<i;j++)
    	{
       if (strcmp(nazwa[j],nazwa[i]) == 0) is[i]++;
      }
    if (is[i] == 1)
    	{
       k = 0;
       for (int z=0;z<(int)strlen(data);z++)
       	{
          if ((data[z] >= 0x41) && (data[z] <= 0x5a))
          	{
             if (nazwa[i][z] == data[z]) k++;
             if (nazwa[i][z] == data[z] + 0x20) k++;
            }
          else if ((data[z] >= 0x61) && (data[z] <= 0x7a))
          	{
             if (nazwa[i][z] == data[z]) k++;
             if (nazwa[i][z] == data[z] - 0x20) k++;
            }
          else
          	{
             if (nazwa[i][z] == data[z]) k++;
            }
         }
       if (k == (int)strlen(data))
         	{
             cout<<nazwa[i]<<endl;
             lines++;
             if (lines == 18){cout<<"Nacisnij cokolwiek aby kontynuowac...\n";getch();ln();lines = 0;}
            }
      }
    if (disp == 54) {cout<<"Nacisnij cokolwiek aby kontynuowac...";getch();ln();disp = 0;}
   }
 lns(2);
}
void DosApp :: Zmien()
{
 PokazZespoly();
 cout<<"Podaj nazwe zespolu: ";
 char zespol[STRING];
 takestring(zespol);
 if (strcmp(zespol,"") == 0) return;
 int num = 0;
 int search = 0;
 for (int i=0;i<il_cd;i++)
 	{
    if (strcmp(nazwa[i],zespol) == 0)
    	{
       num++;
       cout<<num<<")\t"<<plyta[i]<<endl;
       if (!(num % 18)) {cout<<"Nacisnij cokolwiek aby kontynuowac...";getch();ln();}
      }
   }
 cout<<"Podaj numer plyty do modyfikacji: ";
 search = num;
 takeint(&num);
 if ((num <0) || (num>search)) {cout<<"Poza zakresem tablicy.\n";return;}
 search = 0;
 for (int i=0;i<il_cd;i++)
 	{
    if (strcmp(nazwa[i],zespol) == 0)
    	{
       search++;
       if (search == num)
       	{
          Modyfikuj(i);
         }
      }
   }
}

void DosApp :: CzyZapisac()
{
 cout<<"Konczysz dzialanie programu, zapisac? ";
 char zn;
 takechar(&zn);
 if (zn == 't')
 	{
    ZapiszBinaria();
    ZapiszPlikTekstowy();
   }
}
void DosApp :: ZapiszBinaria()
{
 FILE* plik;
 char name[STRING];
 PoliczWartosc();
 start:
 if (strcmp(npliku,"Nie_Zapisany") == 0)
 	{
 	 cout<<"Podaj nazwe pliku: ";
 	 takestring(name);
 	 if (strcmp(name,"") == 0) goto start;
 	}
 else strcpy(name,npliku);
 if ((plik = fopen(name,"r+")) != NULL)
 	{
    cout<<"Plik "<<name<<" istnieje nadpisac go? ";
    char zn;
    takechar(&zn);
    fclose(plik);
    if (zn != 't') {strcpy(npliku,"Nie_Zapisany");goto start;}
   }
 if ((plik = fopen(name,"w+")) == NULL)
 	{
    cout<<"Operacja zapisu NIE powiodla sie!\n";
    return;
   }
fprintf(plik,"%d\n",il_zesp);
fprintf(plik,"%d\n",il_cd);
fprintf(plik,"%d\n",wartosc);
for (int i=0;i<il_cd;i++)
	{
    fprintf(plik,"%s\n",nazwa[i]);
    fprintf(plik,"%s\n",plyta[i]);
    fprintf(plik,"0x%x\n",typ[i]);
    fprintf(plik,"0x%x\n",nosnik[i]);
    fprintf(plik,"%d\n",cena[i]);
    fprintf(plik,"%d\n",ocena[i]);
   }
strcpy(npliku,name);
fclose(plik);
}
void DosApp :: ZapiszPlikTekstowy()
{
  FILE* plik;
 char name[STRING];
 start:
 if (strcmp(npliku,"Nie_Zapisany") == 0)
 	{
 	 cout<<"Podaj nazwe pliku tekstowego (bez rozszerzenia .txt): ";
 	 takestring(name);
 	 if (strcmp(name,"") == 0) goto start;
    strcpy(npliku,name);
 	}
 else strcpy(name,npliku);
 strcat(name,".txt");
 if ((plik = fopen(name,"r+")) != NULL)
 	{
    cout<<"Plik "<<name<<" istnieje nadpisac go? ";
    char zn;
    takechar(&zn);
    fclose(plik);
    if (zn != 't') {strcpy(npliku,"Nie_Zapisany");goto start;}
   }
 if ((plik = fopen(name,"w+")) == NULL)
 	{
    cout<<"Operacja zapisu NIE powiodla sie!\n";
    return;
   }
ZapiszWszystko(plik);
fclose(plik);
}
void DosApp :: ZapiszWszystko(FILE* plik)
{
 fprintf(plik,"\n\n");
 fprintf(plik,"Plik tekstowy automatycznie wygenerowany\nPrzez aplikacje cdd16dos.exe autorstwa morgoth'a-dbma\n");
 fprintf(plik,"Prawa autorskie:	copyleft@\n");
 fprintf(plik,"Ilosc plyt %d, ilosc zespolow %d, wartosc kolekji %d zl\n\n",il_cd,il_zesp,wartosc);
 il_zesp = 0;
 int is[MAX_CD];
 for (int i=0;i<MAX_CD;i++) is[i] = 1;
 for (int i=0;i<il_cd;i++)
 	{
    for (int j=0;j<i;j++)
    	{
       if (strcmp(nazwa[j],nazwa[i]) == 0) is[i]++;
      }
    if (is[i] == 1) {ZapiszZespol(nazwa[i],plik);il_zesp++;fprintf(plik,"\n");}
   }
 lns(2);
 fprintf(plik,"\n\n################### End Of File ###################\n");
}
void DosApp :: WczytajBinaria()
{
 FILE* plik;
 char name[STRING];
 start:
 if (strcmp(npliku,"Nie_Zapisany") == 0)
 	{
 	 cout<<"Podaj nazwe pliku do wczytania: ";
 	 takestring(name);
 	 if (strcmp(name,"") == 0) goto start;
   }
 else
 	{
    strcpy(name,npliku);
    cout<<"Wczytac ponownie plik "<<name<<"? ";
    char zn;
    takechar(&zn);
    if (zn != 't')
    	{
       char temp[STRING];
       cout<<"Podaj nazwe pliku: ";
       takestring(temp);
       strcpy(name,temp);
      }
   }
 if ((plik = fopen(name,"r+")) == NULL)
 	{
    cout<<"Nie moge otworzyc pliku "<<name<<".\n";
    return;
   }
il_cd = 0;
fscanf(plik,"%d\n",&il_zesp);
fscanf(plik,"%d\n",&il_cd);
fscanf(plik,"%d\n",&wartosc);
for (int i=0;i<il_cd;i++)
	{
    fscanf(plik,"%s\n",nazwa[i]);
    fscanf(plik,"%s\n",plyta[i]);
    fscanf(plik,"0x%x\n",&typ[i]);
    fscanf(plik,"0x%x\n",&nosnik[i]);
    fscanf(plik,"%d\n",&cena[i]);
    fscanf(plik,"%d\n",&ocena[i]);
   }
strcpy(npliku,name);
fclose(plik);
}



