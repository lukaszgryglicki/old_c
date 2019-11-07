/* Written by Morgoth DBMA, morgothdbma@o2.pl
 This is part of PgXexplorer software, Open Source
 on BSD licence, Libraries(interaces) used:
 GNU GCC, AS (all stuff needed to compile C source into executable binary)
 LibPQ-FE from PostgreSQL, GTK (GIMP Toolkit)
 written in VIM editor, ctags used, CVS used
 Currently only one author: MOrgoth DBMA
 ! THIS FILE IS BASED ON GTK EXAMPLE FROM INTERNET !
 ! MANY GTK UI FUNCTIONS ARE BASED ON SOMEBODY'S EXAMLE CODE !
 ! BUT I DON'T KNOW THEIR PERSONALITY SO SORRY !
 ! I CANNOT ADD THEM TO AUTORS, BUT THANKS FOR SUPPORT ME !
 FILE: toolbar.h */
#ifndef __MORGOTHDBMA_WIZARDS_H__
#define __MORGOTHDBMA_WIZARDS_H__
#include "main.h"
#define STDSTRING 1024
#define DEL_TYPE_EXACTLY	0
#define DEL_TYPE_BEGINS		1
#define DEL_TYPE_ENDS		2
#define DEL_TYPE_CONTAINS	3
#define DEL_TYPE_NONE   	4
#define MAX_TABLE_NAME	     2048
struct PassInsertData
{
 CHAR* tabname;
 INT n;
 CHAR** names;
 CHAR** values;
 INT*   types;
};
struct WizData
{
 GtkWidget* label;
 GtkWidget* label2;
 GtkWidget* scale;
 GtkWidget* table;
 GtkWidget* load;
 GtkWidget* save;
 GtkWidget* vscrl;
 GtkWidget* hscrl;
 GtkWidget* text;
 GtkWidget* lname;
 GtkWidget* ltype;
 GtkWidget* lprops;
 GtkWidget* lanull;
 GtkWidget* ldef;
 GtkWidget* ltab;
 GtkWidget* tab;
 GtkWidget* sav;
 GtkWidget** name;
 GtkWidget** type;
 GtkWidget** props;
 GtkWidget** anull;
 GtkWidget** def;
 INT ncols;
 CHAR tabname[STDSTRING+1];
};
CHAR* insert_wizard(CHAR* tabname);
CHAR* delete_wizard(CHAR* tabname);
CHAR* select_table_wizard(INT all_opt);
CHAR* select_view_wizard();
CHAR* ctable_wizard();
void clean_vars();
#endif
