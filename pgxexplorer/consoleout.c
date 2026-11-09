/* Written by Morgoth DBMA, morgothdbma@o2.pl
 This is part of PgXexplorer software, Open Source
 on BSD licence, Libraries(interaces) used:
 GNU GCC, AS (all stuff needed to compile C source into executable binary)
 LibPQ-FE from PostgreSQL, GTK (GIMP Toolkit)
 written in VIM editor, ctags used, CVS used
 Currently only one author: MOrgoth DBMA
 FILE: consoleout.c */
#include "common.h"
#include "consoleout.h"
#include "mmsystem.h"

FILE* fopen_printf(CHAR* mode, CHAR* fmt, ...)
{
 va_list ap;
 CHAR fnam[PATH];
 debug("%s:%d fopen_printf(%s,%s)",__FILE__,__LINE__,mode,fmt);
 if (!fmt) return NULL;
 va_start(ap,fmt);
 vsprintf(fnam,fmt,ap);
 va_end(ap);
 return fopen(fnam,mode);
}


INT sdebug(CHAR* fmt, ...)
{
#ifdef DEBUG
 va_list ap;
 INT err;
 if (!fmt) return -1;
 va_start(ap,fmt);
 printf("debug: ");
 err = vprintf(fmt,ap);
 va_end(ap);
 printf("\n");
 fflush(stdout);
 return err;
#else
#ifdef SMALLOC
 va_list ap;
 INT err;
 if (!fmt) return -1;
 va_start(ap,fmt);
 printf("debug: ");
 err = vprintf(fmt,ap);
 va_end(ap);
 printf("\n");
 fflush(stdout);
 return err;
#else
 return 0;
#endif
#endif
}


INT debug(CHAR* fmt, ...)
{
#ifdef DEBUG
 va_list ap;
 INT err;
 if (!fmt) return -1;
 va_start(ap,fmt);
 printf("debug: ");
 err = vprintf(fmt,ap);
 va_end(ap);
 printf("\n");
 fflush(stdout);
 return err;
#endif
#ifndef DEBUG
 return 0;
#endif
}


INT fatal(CHAR* fmt, ...)
{
 va_list ap;
 debug("%s:%d fatal(%s)",__FILE__,__LINE__,fmt);
 if (!fmt) return -1;
 va_start(ap,fmt);
 printf("\n\nPANIC\n\t%c%cfatal: ",7,7);
 vprintf(fmt,ap);
 printf("\nApplication Terminated.%c%c\n\n", 7,7);
 va_end(ap);
 printf("\nSend a bug report to: morgothdbma@o2.pl\n");
 fflush(stdout);
#ifdef CORE_DUMP
 printf("Core dump for user request, DEBUG ME IN GDB\n");
 abort();
#else
 exit(1);
#endif
}


INT fatal_here(CHAR* file, INT line, CHAR* fmt, ...)
{
 va_list ap;
 debug("%s:%d fatal_here(%s,%d,%d)",__FILE__,__LINE__,file,line,fmt);
 if (!fmt) return -1;
 va_start(ap,fmt);
 printf("\n\nPANIC FROM FILE: %s, LINE: %d\n\t%c%cfatal: ",file,line,7,7);
 vprintf(fmt,ap);
 printf("\nApplication Terminated.%c%c\n\n", 7,7);
 va_end(ap);
 printf("PostMortem MemDump (global_memlist)\n");
#ifdef SMALLOC
 dprint_memlist(HERE);
#else
 print_memlist();
#endif
 printf("\nSend a bug report to: morgothdbma@o2.pl\n");
 fflush(stdout);
 exit(1);
}


void downcase(CHAR* ptr)
{
 INT i;
 INT s;
 debug("%s:%d downcase(%s)",__FILE__,__LINE__,ptr);
 if (!ptr) return;
 s = strlen(ptr);
 for (i=0;i<s;i++) if (ptr[i]>='A' && ptr[i]<='Z') ptr[i]+=0x20;
}


INT error(CHAR* fmt, ...)
{
 va_list ap;
 INT err;
 debug("%s:%d error(%s)",__FILE__,__LINE__,fmt);
 if (!fmt) return -1;
 va_start(ap,fmt);
 printf("error: ");
 err = vprintf(fmt,ap);
 va_end(ap);
 printf("\n");
 fflush(stdout);
 return err;
}


INT warn(CHAR* fmt, ...)
{
 va_list ap;
 INT err;
 debug("%s:%d warn(%s)",__FILE__,__LINE__,fmt);
 if (!fmt) return -1;
 va_start(ap,fmt);
 printf("warning: ");
 err = vprintf(fmt,ap);
 printf("\n");
 fflush(stdout);
 va_end(ap);
 return err;
}


INT system_printf(CHAR* fmt, ...)
{
 va_list ap;
 CHAR syscmd[MAX_SYSTEM_CMD];
 debug("%s:%d system_printf(%s)",__FILE__,__LINE__,fmt);
 debug("system_printf: %s", fmt);
 if (!fmt) return -1;
 va_start(ap,fmt);
 vsprintf(syscmd,fmt,ap);
 va_end(ap);
 return system(syscmd);
}


void print_results(CHAR*** res, INT a, INT b)
{
 INT i,j;
 debug("%s:%d print_results(%p,%d,%d)",__FILE__,__LINE__,res,a,b);
 ln();
 if (!res) return;
 for (i=0;i<a;i++)
   {
    if (res[i]) for (j=0;j<b;j++) if (res[i][j]) printf("%-16s ", res[i][j]);
    ln();
   }
 ln();
}

