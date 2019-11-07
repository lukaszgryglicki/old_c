/* Written by Morgoth DBMA, morgothdbma@o2.pl
 This is part of PgXexplorer software, Open Source
 on BSD licence, Libraries(interaces) used:
 GNU GCC, AS (all stuff needed to compile C source into executable binary)
 LibPQ-FE from PostgreSQL, GTK (GIMP Toolkit)
 written in VIM editor, ctags used, CVS used
 Currently only one author: MOrgoth DBMA
 FILE: gtktest.c */
#include "common.h"
#include "gtktest.h"

void callback(GtkWidget *widget, gpointer data)
{
 debug("%s:%d callback",__FILE__,__LINE__);
 g_print("Hello: %s pressed\n", (char *) data);
}


gint delete_event( GtkWidget *widget, GdkEvent *event, gpointer data)
{
 debug("%s:%d delete_event",__FILE__,__LINE__);
 gtk_main_quit();
 return(FALSE);
}


void test_gtk(PGconn* con, char* pass)
{
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *box1;
    PGresult* res;
    char*** lout;
    int a,b;
    char str1[100];
    char str2[100];
    debug("%s:%d test_gtk",__FILE__,__LINE__);
    res = execute_query(con, "SELECT b FROM test WHERE a=10");
    if (!res) { error("getting values from DB to GUI control"); return; }
    lout=pgres2pc3(res,&a,&b, 0);
    clear_result(&res);
    if (a<1) { error("inserted data does not exists in DB.\n"); free_p3c(&lout,a,b); return; }
    strcpy(str1,lout[0][0]);
    free_p3c(&lout,a,b);
    printf("GET BUTTON DATA OK.\n");
    res = execute_query(con, "SELECT b FROM test WHERE a=11");
    if (!res) { error("getting values from DB to GUI control"); return; }
    lout=pgres2pc3(res,&a,&b, 0);
    clear_result(&res);
    if (a<1) { error("inserted data does not exists in DB.\n"); free_p3c(&lout,a,b); return; }
    strcpy(str2,lout[0][0]);
    free_p3c(&lout,a,b);
    printf("GET BUTTON DATA OK.\n");
    disconnect_db(&con);
    if (pass) con = connect_db("template1", pass);
    else      con = connect_db("template1", NULL);
    if (!con) { error("connect to template1\n"); return; }
    usleep(50000);
    res=execute_query(con, "DROP DATABASE pgxtest");
    if (!res) { error("DROPPING DB: pgxtest"); return; }
    disconnect_db(&con);
    gtk_init (NULL,NULL);
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (window), "Hello Buttons!");
    gtk_signal_connect (GTK_OBJECT (window), "delete_event", GTK_SIGNAL_FUNC (delete_event), NULL);
    gtk_container_set_border_width (GTK_CONTAINER (window), 100);
    box1 = gtk_hbox_new(FALSE, 0);
    gtk_container_add (GTK_CONTAINER (window), box1);
    button = gtk_button_new_with_label (str1);
    gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (callback), (gpointer) "DB button I");
    gtk_box_pack_start(GTK_BOX(box1), button, TRUE, TRUE, 0);
    gtk_widget_show(button);
    button = gtk_button_new_with_label (str2);
    gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (callback), (gpointer) "DB button II");
    gtk_box_pack_start(GTK_BOX(box1), button, TRUE, TRUE, 0);
    gtk_widget_show(button);
    gtk_widget_show(box1);
    gtk_widget_show (window);
    gtk_main ();
}

