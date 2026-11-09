/* Written by Morgoth DBMA, morgothdbma@o2.pl
 This is part of PgXexplorer software, Open Source
 on BSD licence, Libraries(interaces) used:
 GNU GCC, AS (all stuff needed to compile C source into executable binary)
 LibPQ-FE from PostgreSQL, GTK (GIMP Toolkit)
 written in VIM editor, ctags used, CVS used
 Currently only one author: MOrgoth DBMA
 FILE: gtkdialogs.h */
#ifndef __MORGOTHDBMA_GTKTEST__
#define __MORGOTHDBMA_GTKTEST__
#include "gtkincl.h"
#include "query.h"
#include "consoleout.h"
#define ANSWER_YES 1
#define ANSWER_NO  0
#define MAX_ENTRY_LENGTH 1024
#define MAX_MSG_LENGTH   4608
#define MASK_LENGTH        32
INT gtk_dialog_yes_no(const CHAR*);
INT gtk_dialog_with_2_buttons(const CHAR*, CHAR*, CHAR*);
INT gtk_dialog_yes_no_printf(const CHAR*, ...);
INT gtk_dialog_yes_no_printf_big(const CHAR*, ...);
INT gtk_dialog_getnum(INT, INT, CHAR*);
INT gtk_dialog_getnum_printf(INT, INT, CHAR*, ...);
CHAR* gtk_dialog_gettext(const CHAR*);
CHAR* gtk_dialog_getpass(const CHAR*);
CHAR* gtk_dialog_gettext_printf(const CHAR*, ...);
CHAR* gtk_dialog_gettext_big(const CHAR*);
CHAR* gtk_dialog_gettext_big_printf(const CHAR*, ...);
CHAR* gtk_dialog_getfile_printf(const CHAR*, const CHAR*, ...);
CHAR* gtk_dialog_getfile(const CHAR*, const CHAR*);
void gtk_dialog_mbox(const CHAR*, const CHAR*);
void gtk_dialog_printf_with_title(const CHAR*, const CHAR*, ...);
void gtk_dialog_printf_big_with_title(const CHAR*, const CHAR*, ...);
void gtk_dialog_printf(const CHAR*, ...);
void gtk_dialog_printf_big(const CHAR*, ...);
CHAR* gtk_dialog_getdbs(PGconn*, CHAR*,...);
void destroy_widget(GtkWidget* w);
struct carrier
{
 GtkWidget* widget;
 void* ptr;
};
#endif
