/* Written by Morgoth DBMA, morgothdbma@o2.pl
 This is part of PgXexplorer software, Open Source
 on BSD licence, Libraries(interaces) used:
 GNU GCC, AS (all stuff needed to compile C source into executable binary)
 LibPQ-FE from PostgreSQL, GTK (GIMP Toolkit)
 written in VIM editor, ctags used, CVS used
 Currently only one author: MOrgoth DBMA
 FILE: sqlparser.h */
#ifndef __MORGOTHDBMA_SQLPARSER_H__
#define __MORGOTHDBMA_SQLPARSER_H__
#define MAX_ERROR_STRING 4096
#define MAX_QUERY_LENGTH 4096
#define PARSER_MAX_LENGTH 150
#define NUM_PARSE_STRINGS 732
#include <string.h>
#include "consoleout.h"
#include "mmsystem.h"
struct parser_data
{
  INT type;
  CHAR keyword[PARSER_MAX_LENGTH];
};
INT get_last_words(CHAR* target, CHAR* from, INT count);
INT auto_complete(CHAR* str, CHAR*** table);
#endif
