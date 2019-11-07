/* Written by Morgoth DBMA, morgothdbma@o2.pl
 This is part of PgXexplorer software, Open Source
 on BSD licence, Libraries(interaces) used:
 GNU GCC, AS (all stuff needed to compile C source into executable binary)
 LibPQ-FE from PostgreSQL, GTK (GIMP Toolkit)
 written in VIM editor, ctags used, CVS used
 Currently only one author: MOrgoth DBMA
 FILE: gtktest.h */
#ifndef __MORGOTHDBMA_GTKTEST__
#define __MORGOTHDBMA_GTKTEST__
#include "gtkincl.h"
#include "query.h"
void callback(GtkWidget*, gpointer);
gint delete_event( GtkWidget*, GdkEvent*, gpointer);
void test_gtk(PGconn* con, char*);
#endif
