#if !defined _MHH_HEEE_
#define _MHH_HEEE_

extern int qsortasm(void*, int (*)(const void*, const void*), unsigned int, unsigned inti, int);

#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <termios.h>
#include <math.h>
#include "mran.h"


#define STRING	        100
#define OK		0
#define FALSE		0
#define TRUE		1
#define OS		"FreeBSD"
#define EXISTS		1
#define NO_EXISTS	-1
#define CHECKED 	20
#define NORMAL		21

#define BACKSPACE  0xfb
#define ENTER      0xfe
#define ESCAPE     0xfa
#define SHIFT      0xff
#define INVALID_KEY 0x7



static int GET_T = CHECKED;





inline void ln();
inline void lnf(FILE*);
void takechar(char*);
void takechar(char&);
void takestring(char*,int);
void lns(int);
void takeint(int*);
void takeint(int&);
int random(int);
void DecompressData(char*);
void CompressData(char*);
void FromBinary(FILE*, FILE*);
void ToBinary(FILE*, FILE*);
void Trap();
inline void wait();
inline void Second();
void downcase(char*);
void downcase(char&);
bool StrMore(char* , char*);
int mgetch();
char* String2table(char*, int, int&);
int isLetter(int zn);
void bytecopy(char*, char*, int);
void bytecopy(char*, char*, int, int);




void bytecopy(char* a, char* b, int n)
{
 for (int i=0;i<n;i++) a[i] = b[i];
}

void bytecopy(char* a, char* b, int n, int from)
{
 for (int i=0;i<n;i++) a[i] = b[i+from];
}

inline void ln()
{
#ifdef DEBUG
 printf("DEBUG_MH_H: inline_ln()\n");
#endif
 cout<<endl;
}
inline void lnf(FILE* i)
{
#ifdef DEBUG
 printf("DEBUG_MH_H: inline lnf(FILE* %p)\n", (void*)i);
#endif
 fprintf(i,"\n");
}

int TransformKey(unsigned long int code, int shift)
{
#ifdef DEBUG
 printf("DEBUG_MH_H: TransformKey(%d,%d) code shift\n",(int)code, shift);
#endif
 if (shift) code += 200;
 //printf("%d\n", code);
 switch (code)
   {
    case 38: return 'a'; break;
    case 56: return 'b'; break;
    case 54: return 'c'; break;
    case 40: return 'd'; break;
    case 26: return 'e'; break;
    case 41: return 'f'; break;
    case 42: return 'g'; break;
    case 43: return 'h'; break;
    case 31: return 'i'; break;
    case 44: return 'j'; break;
    case 45: return 'k'; break;
    case 46: return 'l'; break;
    case 58: return 'm'; break;
    case 57: return 'n'; break;
    case 32: return 'o'; break;
    case 33: return 'p'; break;
    case 24: return 'q'; break;
    case 27: return 'r'; break;
    case 39: return 's'; break;
    case 28: return 't'; break;
    case 30: return 'u'; break;
    case 55: return 'v'; break;
    case 25: return 'w'; break;
    case 53: return 'x'; break;
    case 29: return 'y'; break;
    case 52: return 'z'; break;
    case 19: return '0'; break;
    case 10: return '1'; break;
    case 11: return '2'; break;
    case 12: return '3'; break;
    case 13: return '4'; break;
    case 14: return '5'; break;
    case 15: return '6'; break;
    case 16: return '7'; break;
    case 17: return '8'; break;
    case 18: return '9'; break;
    case 20: return '-'; break;
    case 21: return '='; break;
    case 51: return '\\'; break;
    case 65: return ' '; break;
    case 22: return BACKSPACE; break;
    case 61: return '/'; break;
    case 60: return '.'; break;
    case 59: return ','; break;
    case 48: return '\''; break;
    case 47: return ';'; break;
    case 35: return ']'; break;
    case 34: return '['; break;
    case 36: return ENTER; break;
    case 49: return '`'; break;
    case 9: return  ESCAPE; break;
    case 50: return SHIFT; break;
    case 250: return SHIFT; break;


    case 238: return 'A'; break;
    case 256: return 'B'; break;
    case 254: return 'C'; break;
    case 240: return 'D'; break;
    case 226: return 'E'; break;
    case 241: return 'F'; break;
    case 242: return 'G'; break;
    case 243: return 'H'; break;
    case 231: return 'I'; break;
    case 244: return 'J'; break;
    case 245: return 'K'; break;
    case 246: return 'L'; break;
    case 258: return 'M'; break;
    case 257: return 'N'; break;
    case 232: return 'O'; break;
    case 233: return 'P'; break;
    case 224: return 'Q'; break;
    case 227: return 'R'; break;
    case 239: return 'S'; break;
    case 228: return 'T'; break;
    case 230: return 'U'; break;
    case 255: return 'V'; break;
    case 225: return 'W'; break;
    case 253: return 'X'; break;
    case 229: return 'Y'; break;
    case 252: return 'Z'; break;
    case 219: return ')'; break;
    case 210: return '!'; break;
    case 211: return '@'; break;
    case 212: return '#'; break;
    case 213: return '$'; break;
    case 214: return '%'; break;
    case 215: return '^'; break;
    case 216: return '&'; break;
    case 217: return '*'; break;
    case 218: return '('; break;
    case 220: return '_'; break;
    case 221: return '+'; break;
    case 251: return '|'; break;
    case 265: return ' '; break;
    case 222: return BACKSPACE; break;
    case 261: return '?'; break;
    case 260: return '>'; break;
    case 259: return '<'; break;
    case 248: return '"'; break;
    case 247: return ':'; break;
    case 235: return '}'; break;
    case 234: return '{'; break;
    case 236: return ENTER; break;
    case 209: return  ESCAPE; break;
    case 249: return '~'; break;
   }
 return INVALID_KEY;
}

void FSwap(int i, int j, char** table)
{
#ifdef DEBUG
 printf("DEBUG_MH_H: FSwap(%d,%d,%p) i,j inside_pointer\n", i, j, (void*)table);
#endif
 char pom[STRING];

 strcpy(pom, table[j]);
 strcpy(table[j], table[i]);
 strcpy(table[i], pom);
}

/*void FSort(char** table, int num)
{
#ifdef DEBUG
 printf("DEBUG_MH_H: FSort(%p, %d) table,number\n", table,num);
 for (int i=0;i<num;i++)
   {
    if (table[i]) printf("DEBUG_MH_H: table[%d] = %p:%s\n", i,table[i], table[i]);
   }
#endif   
 qsortasm((void*)table, NULL, 0, num-1, 0);
}*/

void FSort(char** table, int num)
{
#ifdef DEBUG
 printf("DEBUG_MH_H: FSort(%p, %d) table,number\n", table,num);
 for (int i=0;i<num;i++)
   {
    if (table[i]) printf("DEBUG_MH_H: table[%d] = %p:%s\n", i,table[i], table[i]);
   }
#endif
 if (!table)
   {
    printf("Fatal in FSort(char**, int) char** points to NULL!\n");
    return;
   }
 if (!num)
   {
    printf("Warning in FSort(char**, int) about to sort 0-length table, aborted!\n");
    return;
   }
 for (int i=0;i<num;i++)
    {
     for (int j=0;j<num-(i+1);j++)
        {
	 if (StrMore(table[j], table[j+1]))
	  {
	   FSwap(j+1, j, table);
	  }
	}
     }
#ifdef DEBUG
 printf("DEBUG_MH_H: After sort\n");
 for (int i=0;i<num;i++)
   {
    if (table[i]) printf("DEBUG_MH_H: table[%d] = %p:%s\n", i,table[i], table[i]);
   }
#endif
}

int isLetter(int zn)
{
#ifdef DEBUG
 printf("DEBUG_MH_H: isLetter(%d:%c)\n", zn, zn);
#endif
 if (((zn >= 'a') && (zn <= 'z')) || ((zn >='A') && (zn <= 'Z'))) return true;
 else return false;
}
void CutStr(char*& str, int pref)
{
#ifdef DEBUG
 if (str) printf("DEBUG_MH_H: CutStr(%p:%s, %d) string, pref\n", str,str,pref);
 else printf("DEBUG_MH_H: CutStr((char*)NULL,%d) string_empty warning!\n", pref);
#endif
 if (!str) return;
 int ip = 0;
 for (int i=0;i<(int)strlen(str);i++)
   {
    if ((ip >= pref-10) && (str[i] == ' ')) { str[i] = '\n'; ip = 0;}
    if (ip >= pref +10) {str[i] = '\n'; ip = 0;}
    ip++;
   }
}

void stop(int* a)
{
#ifdef DEBUG
 printf("DEBUG_MH_H: stop <<WATCH_IT>>\n");
#endif
 printf("Stop! ARG_PTR=%p\n", a);
}

void Trap()
{
#ifdef DEBUG
 printf("DEBUG_MH_H: Trap <<WATCH_IT>>\n");
#endif
 cout<<"STOP!\n";
 int a;
 takeint(&a);
}


bool StrMore(char* str1, char* str2)
{
#ifdef DEBUG
 if ((str1) && (str2)) printf("DEBUG_MH_H: StrMore(%p:%s, %p:%s)\n", str1,str1,str2,str2);
 else printf("DEBUG_MH_H: StrMore(str1,str2) warning one or two is(are) NULL\n");
#endif
 if ((!str1) || (!str2)) return false;
 int len = (strlen(str1) < strlen(str2)) ? strlen(str1) : strlen(str2);
 register int i = 0;
 while ((str1[i] == str2[i]) && (i <= len)) i++;
 if (str1[i] > str2[i]) return true;
 else return false;
}

int mgetch()
{
#ifdef DEBUG
 printf("DEBUG_MH_H: int mgetch()\n");
#endif
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
 cout<<(char)ret;
#ifdef DEBUG
 printf("DEBUG_MH_H: ret ==> %d:%c\n", ret,ret);
#endif
 return ret;
}


void GetStrType(int par)
{
#ifdef DEBUG
 printf("DEBUG_MH_H: GetStrType(%d) parm\n", par);
#endif
 GET_T = par;
}

void SwitchIn()
{
#ifdef DEBUG
 printf("DEBUG_MH_H: SwitchIn()\n");
#endif
 if (GET_T == CHECKED) GET_T = NORMAL;
 else GET_T = CHECKED;
}


void DecompressData(char* data)
{
#ifdef DEBUG
 if (data) printf("DEBUG_MH_H: DecompressData(%p:%s)\n", data,data);
 else printf("DEBUG_MH_H: DecompressData(NULL) warning!\n");
#endif
 if (!data) return;
 for (int i=0;i<(int)strlen(data);i++) if (data[i] == '_') data[i] = ' ';
}


void CompressData(char* data)
{
#ifdef DEBUG
 if (data) printf("DEBUG_MH_H: CompressData(%p:%s)\n", data,data);
 else printf("DEBUG_MH_H: CompressData(NULL) warning!\n");
#endif
 if (!data) return;
 for (int i=0;i<(int)strlen(data);i++) if (data[i] == ' ') data[i] = '_';
}


void FromBinary(FILE* in, FILE* out)
{
#ifdef DEBUG
 printf("DEBUG_MH_H: FromBinary(%p,%p) files\n", in, out);
#endif
 int zn;
 int sign;
 int array[8];
 while ((zn = fgetc(in)) != EOF)
	{
    array[0] = zn-48;
    for (int i=1;i<8;i++) array[i] = fgetc(in)-48;
    sign = 0;
    for (int i=0;i<8;i++) sign += int(pow(2,7-i)*array[i]);
    fprintf(out,"%c",sign);
   }
}



void ToBinary(FILE* in, FILE* out)
{
#ifdef DEBUG
 printf("DEBUG_MH_H: ToBinary(%p, %p) files\n", in, out);
#endif
 int zn;
 int array[8];
 while ((zn = fgetc(in)) != EOF)
	{
    for (int i = 7;i>=0;i--)
	{
       if (zn >= (int)pow(2,i)) {array[7-i] = 1; zn -= (int)pow(2,i);} else array[7-i] = 0;
      }
    for (int i=0;i<8;i++) fprintf(out,"%d",array[i]);
   }
}


void takechar(char& zn)
{
#ifdef DEBUG
 printf("DEBUG_MH_H: takechar(address)\n");
#endif
 char znak[STRING];
 scanf("%s", znak);
 sscanf(znak, "%c", &zn);
 if (strlen(znak)>1) cout<<"podales za dluga nazwe\n";
}

void takechar(char* zn)
{
#ifdef DEBUG
 printf("DEBUG_MH_H: takechar(pointer)\n");
#endif
 char znak[STRING];
 scanf("%s",znak);
 sscanf(znak,"%c",zn);
 if (strlen(znak)>1) cout<<"Podales za dluga nazwe\n";
}

void takestring(char* par, int sec = 0)
{
#ifdef DEBUG
 printf("DEBUG_MH_H: takestring((char*)pointer, %d) parm\n", sec);
#endif
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
	 if (par[i] == 8) {if (i>0)printf("\b \b");i--;if (i<0) {i=0;}goto cont;}
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
#ifdef DEBUG
 printf("DEBUG_MH_H: lns(%d) num_lines\n", par);
#endif
 for (short i=0;i<par;i++) ln();
}


inline void wait()
{
#ifdef DEBUG
 printf("DEBUG_MH_H: wait()\n");
#endif
 char zn;
 takechar(&zn);
}



void takeint(int* par)
{
#ifdef DEBUG
 printf("DEBUG_MH_H: takeint(pointer)\n");
#endif
 char string[STRING];
 scanf("%s",string);
 if (!sscanf(string,"%d",par)) {*par = 0;cout<<"Parametr literowy! Zmnieniam na 0\n";}
}

void takeint(int& par)
{
#ifdef DEBUG
 printf("DEBUG_MH_H: takeint(address)\n");
#endif
 char string[STRING];
 scanf("%s", string);
 if (!sscanf(string,"%d", &par)) {par = 0; cout<<"Parametr literowy! Zmieniam na 0\n";}
}

void takeint(unsigned int* par)
{
#ifdef DEBUG
 printf("DEBUG_MH_H: takeint(unsigned_ptr)\n");
#endif
 char string[STRING];
 scanf("%s",string);
 if (!sscanf(string,"%d",par)) {*par = 0;cout<<"Parametr literowy! Zmnieniam na 0\n";}
}

void takeint(unsigned int& par)
{
#ifdef DEBUG
 printf("DEBUG_MH_H: takeint(unsigned_addr)\n");
#endif
 char string[STRING];
 scanf("%s", string);
 if (!sscanf(string,"%d", &par)) {par = 0; cout<<"Parametr literowy! Zmieniam na 0\n";}
}

inline void downcase(char& str)
{
#ifdef DEBUG
 printf("DEBUG_MH_H: downcase(%d:%c (addr))\n", str, str);
#endif
  if ((str >= 'A') && (str <= 'Z')) str += (char)0x20;
}

void downcase(char* str)
{
#ifdef DEBUG
 if (str) printf("DEBUG_MH_H: downcase(%p:%s) string\n", str, str);
 else printf("DEBUG_MH_H: downcase((char*)NULL) warning\n");
#endif
 if (!str) return;
 for (int i=0;i<(int)strlen(str);i++)
	{
    if ((str[i] >= 'A') && (str[i] <= 'Z')) str[i] += (char)0x20;
   }
}

#endif




