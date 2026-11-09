#include <stdlib.h>
#include <stdio.h>
#include <fstream.h>
#include <conio.h>


#define STRING          16
#define STRING2			24
#define OK              0
#define FALSE           0
#define TRUE            1

inline void ln();
void takechar(char*);
void takestring(char*);
void lns(int);
void takeint(int*);
void takesh(int*);
void Trap();

inline void ln() {cout<<endl;}
void Trap()
{
 cout<<"STOP!\n";
 int a;
 takeint(&a);
}
void takechar(char* zn)
{
 char znak[STRING];
 scanf("%s",znak);
 sscanf(znak,"%c",zn);
 if (strlen(znak)>1) cout<<"Podales za dluga nazwe\n";
}
void takestring(char* par)
{
 start:
 par[0] = 0;
 int i = -1;
 while ((par[i] != 0x0d) && (i < STRING-1))
 	{
    i++;
    par[i] = (char)getch();
    if ((par[i] >= 'A') && (par[i] <='Z')) par[i] += 0x20;
    if (par[i] == 27) {par[i] = '\0';cout<<" Anulowano <"<<par<<">\n";goto start;}
    if (par[i] == 32) par[i] = 95;
    if (par[i] == 8) {printf("\b \b");i-=2;if (i < -1) i = -1;goto cont;}
    cout<<par[i];
    cont:
   }
 ln();
 par[i] = '\0';
}
void takes(char* par,int len)
{
 start:
 par[0] = 0;
 int i = -1;
 while ((par[i] != 0x0d) && (i < len-2))
 	{
    i++;
    par[i] = (char)getch();
    if (par[i] == 27) {par[i] = '\0';cout<<" Anulowano <"<<par<<">\n";goto start;}
    if (par[i] == 32) par[i] = 95;
    if (par[i] == 8) {printf("\b \b");i-=2;if (i < -1) i = -1;goto cont;}
    cout<<par[i];
    cont:
   }
 ln();
 par[i] = '\0';
}
void lns(int par)
{
 for (short i=0;i<par;i++) ln();
}
void takeint(int* par)
{
 char string[STRING];
 scanf("%s",string);
 if (!sscanf(string,"%d",par)) {*par = 0;cout<<"Parametr literowy! Zmnieniam na 0\n";}
}
void takesh(short* par)
{
 char string[STRING];
 scanf("%s",string);
 if (!sscanf(string,"%d",par)) {*par = 0;cout<<"Parametr literowy! Zmnieniam na 0\n";}
}
void takeuchar(unsigned char* par)
{
 char string[STRING];
 scanf("%s",string);
 if (!sscanf(string,"%d",par)) {*par = 0;cout<<"Parametr literowy! Zmnieniam na 0\n";}
}

