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
 FILE: pbar.c */
#include "pbar.h"
#include "common.h"
#include "signals.h"
#include <stdlib.h>
struct ProgressData* pdata;
int pid;

int progress_timeout(gpointer data)
{
 gfloat new_val;
 GtkAdjustment *adj;
 new_val = gtk_progress_get_value(GTK_PROGRESS(data))+1;
 adj = GTK_PROGRESS (data)->adjustment;
 if (new_val>adj->upper) new_val=adj->lower;
 gtk_progress_set_value (GTK_PROGRESS (data), new_val);
 return TRUE;
}


void destroy_progress(GtkWidget  *widget, struct ProgressData *pdata)
{
 gtk_timeout_remove(pdata->timer);
 pdata->timer = 0;
 pdata->window = NULL;
 g_free(pdata);
 gtk_main_quit();
 kill(pid, SIGUSR1);
}


int main(int lb, char** par)
{
 GtkWidget     *align;
 GtkAdjustment *adj;
 GtkWidget     *vbox;
 gtk_init(&lb, &par);
 if (lb!=3) return 1;
 pid = 0;
 pid = atoi(par[1]);
 if (pid<=1) return 2;
 pdata = malloc(sizeof(struct ProgressData));
 pdata->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
 gtk_signal_connect (GTK_OBJECT(pdata->window), "destroy", GTK_SIGNAL_FUNC (destroy_progress),pdata);
 gtk_window_set_title (GTK_WINDOW (pdata->window), par[2]);
 gtk_widget_set_usize(pdata->window, 300, 50);
 gtk_container_set_border_width(GTK_CONTAINER (pdata->window), 0);
 vbox = gtk_vbox_new(FALSE, 5);
 gtk_container_set_border_width(GTK_CONTAINER (vbox), 10);
 gtk_container_add(GTK_CONTAINER (pdata->window), vbox);
 gtk_widget_show(vbox);
 align = gtk_alignment_new(0.5, 0.5, 0.8, 0);
 gtk_box_pack_start(GTK_BOX (vbox), align, FALSE, FALSE, 5);
 gtk_widget_show(align);
 adj = (GtkAdjustment *)gtk_adjustment_new (0, 1, 200, 0, 0, 0);
 pdata->pbar = gtk_progress_bar_new_with_adjustment(adj);
 gtk_progress_set_activity_mode(GTK_PROGRESS (pdata->pbar),TRUE);
 gtk_container_add(GTK_CONTAINER(align), pdata->pbar);
 gtk_widget_show(pdata->pbar);
 pdata->timer = gtk_timeout_add(100, progress_timeout, pdata->pbar);
 gtk_widget_show(pdata->window);
 gtk_main();
 return 0;
}

