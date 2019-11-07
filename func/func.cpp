#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#define false 0
#define true  1

static int debug = 0;


void ln()
{
 putchar('\n');
}	

void compile_f(char* par, bool D, bool bx, bool by, double sx, double sy)
{
 FILE* header;

 printf("Got Environment: par=%s D=%d, bx=%d, by=%d, sx=%f, sy=%f\n",par,D,bx,by,sx,sy); 
 header = fopen("function.h","w");
 if (!header) {printf("Cannot open file: function.h for writing!\n"); return;}
 fprintf(header,"#define DEBUG %d\n", debug);
 fprintf(header,"long double function(long double x, long double i)\n");
 fprintf(header,"{\n");
 if (!D)
 	{
    fprintf(header," return %s;\n",par);
   }
 if (D)
 	{
    if ((!bx) && (!by)) fprintf(header," return %s;\n",par);
    if ((bx) && (!by))
    	{
       fprintf(header,"if (x<%f) return 0;\n", sx);
       fprintf(header,"else return %s;\n",par);
      }
    if ((!bx) && (by))
    	{
       fprintf(header,"if (x>%f) return 0;\n", sy);
       fprintf(header,"else return %s;\n",par);
      }
    if ((bx) && (by))
    	{
       fprintf(header,"if ((x>%f) || (x<%f)) return 0;\n", sy,sx);
       fprintf(header,"else return %s;\n",par);
      }
   }
 fprintf(header,"}\n");
 fclose(header);
 printf("Header 'function.h' written now compiling...\n");
 system("make -ffunction.mak");
 system("del *.bak");
 system("del *.cfg");
 system("del *.~de");
 system("del *.il*");
 system("del *.obj");
 system("del *.tds");
 system("del *.swp");
 system("del *.dsw");
 system("del *.mbt");
 system("del *.mrt");
 system("del *.obr");
 system("function");
}

int main()
{
 char pa[1024];
 char zn;
 bool rx,ry;
 double sx,sy;
 printf("Debug? (Y/N) ");
 zn = (char)getch();
 ln();
 if ((zn == 'y') || (zn == 'Y')) debug = 1;
 printf("Type function (example sin(x+3)): ");
 scanf("%s",pa);
 printf("Got function: %s\n",pa);
 printf("Range? (Y/N) ");
 zn = (char)getch();
 ln();
 if ((zn == 'n') || (zn == 'N')) {compile_f(pa, false, false, false, 0, 0); return 0;}
 printf("range x? (Y/N) ");
 zn = (char)getch();
 ln();
 if ((zn == 'n') || (zn == 'N')) rx = false;
 else { rx = true;printf("start x: "); scanf("%lf",&sx);}
 printf("range y? (Y/N) ");
 zn = (char)getch();
 ln();
 if ((zn == 'n') || (zn == 'N')) ry = false;
 else { ry = true; printf("end x: "); scanf("%lf",&sy);}
 compile_f(pa, true, rx, ry,sx,sy);
 return 0;
}
