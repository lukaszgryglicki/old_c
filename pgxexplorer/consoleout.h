/* Written by Morgoth DBMA, morgothdbma@o2.pl
 This is part of PgXexplorer software, Open Source
 on BSD licence, Libraries(interaces) used:
 GNU GCC, AS (all stuff needed to compile C source into executable binary)
 LibPQ-FE from PostgreSQL, GTK (GIMP Toolkit)
 written in VIM editor, ctags used, CVS used
 Currently only one author: MOrgoth DBMA
 FILE: consoleout.h */
#ifndef __MORGOTHDBMA_CONOUT_H__
#define __MORGOTHDBMA_CONOUT_H__
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "common.h"
#define ERR error("Error at line %d, file %s, compiled %s %s\n", __LINE__,__FILE__,__DATE__,__TIME__);
#define ln() putchar('\n')
#define MAX_SYSTEM_CMD 512
#define PATH 2048
INT system_printf(CHAR*, ...);
INT debug(CHAR*, ...);
INT sdebug(CHAR*, ...);
INT fatal(CHAR*, ...);
INT fatal_here(CHAR*, INT, CHAR*, ...);
INT error(CHAR*, ...);
INT warn(CHAR*, ...);
FILE* fopen_printf(CHAR* mode, CHAR* fmt, ...);
void downcase(CHAR*);
void print_results(CHAR***, INT, INT);
#endif
