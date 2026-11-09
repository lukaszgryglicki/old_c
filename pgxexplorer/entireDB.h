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
 FILE: entireDB.h */
#ifndef __MORGOTHDBMA_ENTIREDB_H__
#define __MORGOTHDBMA_ENTIREDB_H__
#include "main.h"
#include "pgxexplorer.h"
#include "common.h"
#include "mmsystem.h"
void cmenu_browse_all(GtkWidget* w, gpointer data);
void create_pbar(CHAR*);
void destroy_pbar();
#endif
