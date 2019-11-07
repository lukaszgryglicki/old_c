#include <stdlib.h>
#include <stdio.h>
#include <fstream.h>
#include <mgetch.h>

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
 struct termios st,end;
 tcgetattr(fileno(stdin),&st);
 end = st;
 end.c_lflag &= ~ICANON;
 end.c_lflag &= ~ECHO;
 end.c_cc[VMIN] = 1;
 end.c_cc[VTIME] = 0;
 if (tcsetattr(fileno(stdin),TCSAFLUSH,&end) != 0) return;
 int i;
 st:
 i = 0;
 par[0] = 0;
 while (i<STRING-1)
        {
         par[i] = getchar();
         if ((i ==0) && (par[i] == 10)) goto cont;
         if (par[i] == ' ') par[i] = '_';
         if (par[i] == 10) {par[i] = '\0';goto lab;}
         if (par[i] == 127) {if (i>0)printf("\b \b");i--;if (i<0) {i=0;}goto cont;}
         if (par[i] == 27) {cout<<"Sorry, you have used illegal character UNICODE is 0x1b, do it again.\n";goto st;}
         cout<<par[i];
         i++;
         cont:;
        }
 lab:
 if (i == 0) {strcpy(par,"-");cout<<"-";}
 ln();
 tcsetattr(fileno(stdin),TCSAFLUSH,&st);
}

void takes(char* par, int sec)
{
 struct termios st,end;
 tcgetattr(fileno(stdin),&st);
 end = st;
 end.c_lflag &= ~ICANON;
 end.c_lflag &= ~ECHO;
 end.c_cc[VMIN] = 1;
 end.c_cc[VTIME] = 0;
 if (tcsetattr(fileno(stdin),TCSAFLUSH,&end) != 0) return;
 int i;
 st:
 i = 0;
 par[0] = 0;
 while (i<STRING-1)
        {
         par[i] = getchar();
         if ((i ==0) && (par[i] == 10)) goto cont;
         if (par[i] == ' ') par[i] = '_';
         if (par[i] == 10) {par[i] = '\0';goto lab;}
         if (par[i] == 127) {if (i>0)printf("\b \b");i--;if (i<0) {i=0;}goto cont;}
         if (par[i] == 27) {cout<<"Sorry, you have used illegal character UNICODE is 0x1b, do it again.\n";goto st;}
         if (sec != 0xfade) cout<<par[i];
         i++;
         cont:;
        }
 lab:
 if (i == 0) {strcpy(par,"-");cout<<"-";}
 ln();
 tcsetattr(fileno(stdin),TCSAFLUSH,&st);
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

