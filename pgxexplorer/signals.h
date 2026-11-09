/* Written by Morgoth DBMA, morgothdbma@o2.pl
 This is part of PgXexplorer software, Open Source
 on BSD licence, Libraries(interaces) used:
 GNU GCC, AS (all stuff needed to compile C source into executable binary)
 LibPQ-FE from PostgreSQL, GTK (GIMP Toolkit)
 written in VIM editor, ctags used, CVS used
 Currently only one author: MOrgoth DBMA
 FILE: signals.h */
#ifndef __MORGOTHDBMA_SIGNALS_H__
#define __MORGOTHDBMA_SIGNALS_H__
#include <unistd.h>
#include <signal.h>
#include "gtkdialogs.h"
void alarm_clock(INT);
void alarm_stop();
void setup_signals();
#endif
