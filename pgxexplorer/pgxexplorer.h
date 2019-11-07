/* Written by Morgoth DBMA, morgothdbma@o2.pl
 This is part of PgXexplorer software, Open Source
 on BSD licence, Libraries(interaces) used:
 GNU GCC, AS (all stuff needed to compile C source into executable binary)
 LibPQ-FE from PostgreSQL, GTK (GIMP Toolkit)
 written in VIM editor, ctags used, CVS used
 Currently only one author: MOrgoth DBMA
 FILE: pgxexplorer.h */
#define STDSTRING 1024
#define MAXPATH 2048
#define TEMPLATE "template1"
#ifndef __MORGOTHDBMA_PGXEXPLORER_H__
#define __MORGOTHDBMA_PGXEXPLORER_H__
#include "common.h"
struct main_components
{
 GtkWidget* table;
 GtkWidget* scrolled_win;
 GtkWidget* tree;
 GtkWidget* tree_item_tables;
 GtkWidget* subtree_tables;
 GtkWidget* tree_item_views;
 GtkWidget* subtree_views;
 GtkWidget* tree_item_triggers;
 GtkWidget* subtree_triggers;
 GtkWidget* cmenu_tables;
 GtkWidget* cmenu_views;
 GtkWidget* cmenu_triggers;
 GtkWidget* cmenu_table;
 GtkWidget* cmenu_view;
 GtkWidget* cmenu_trigger;
 GtkWidget* table_view;
 GtkWidget* scroller_view;
 GtkWidget* button_save;
 GtkWidget* button_destroy;
 GtkWidget* button_refresh;
 GtkWidget* button_update;
 GtkWidget* button_delete;
 GtkWidget*** table_entry;
 GtkWidget* last_selected;
 INT ntables,nviews,ntriggers,cols,rows,acols,arows,rrow,rcol,last_row;
 /* cols HOW MUCH ON WINDOW, acols ALL COLUMNS, RCOL, ROTATE_COL, OFFSET COL ON WINDOW */
 CHAR selected[STDSTRING];
 CHAR seltype;
 CHAR*** data;
};
#endif
