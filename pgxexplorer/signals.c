/* Written by Morgoth DBMA, morgothdbma@o2.pl
 This is part of PgXexplorer software, Open Source
 on BSD licence, Libraries(interaces) used:
 GNU GCC, AS (all stuff needed to compile C source into executable binary)
 LibPQ-FE from PostgreSQL, GTK (GIMP Toolkit)
 written in VIM editor, ctags used, CVS used
 Currently only one author: MOrgoth DBMA
 FILE: signals.c */
#include "common.h"
#include "signals.h"
#include "ilibpq.h"
extern INT ping_conn;
extern INT cont;
extern PGconn* connection;

void priv_sig_proc()
{
 debug("%s:%d priv_sig_proc()", __FILE__,__LINE__);
 ping_conn=0;
 gtk_dialog_printf("Tiomeout!\nConnection lost!\nSystem will now exit");
 exit(1);
 ping_conn=0;
}


void sig_abort()
{
 debug("%s:%d sig_abort()", __FILE__,__LINE__);
 cont=0;
}


void discoteka()
{
 debug("%s:%d discoteka()", __FILE__,__LINE__);
 if (connection) PQfinish(connection);
 else warn("connect is null: %s:%d", __FILE__,__LINE__);
 connection = NULL;
}


void sig_int()
{
 debug("%s:%d sig_kill()", __FILE__,__LINE__);
 cont=0;
 system("killall -9 pbar");
 printf("Exiting by the SIGNAL\n");
 discoteka();
 exit(0);
}


void alarm_clock(INT sec)
{
 debug("%s:%d alarm_clock(%d)", __FILE__,__LINE__,sec);
 alarm(sec);
}


void alarm_stop()
{
 debug("%s:%d alarm_stop()", __FILE__,__LINE__);
 alarm(0);
}


void setup_signals()
{
 static struct sigaction act;
 static struct sigaction acu;
 static struct sigaction acv;
 ping_conn=0;
 debug("%s:%d setup_signals()", __FILE__,__LINE__);
 act.sa_handler = priv_sig_proc;
 sigfillset(&(act.sa_mask));
 sigaction(SIGALRM, &act, NULL);
 acu.sa_handler = sig_abort;
 sigfillset(&(acu.sa_mask));
 sigaction(SIGUSR1, &acu, NULL);
 acv.sa_handler = sig_int;
 sigfillset(&(acv.sa_mask));
 sigaction(SIGINT, &acv, NULL);
}

