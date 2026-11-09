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
 FILE: main.h */
#ifndef __MORGOTHDBMA_MAIN_H__
#define __MORGOTHDBMA_MAIN_H__
#define GTK_TAB_KEY  65289
#define MAX_INSERT_STR_LENGTH 1024
#include "gtkincl.h"
#include "consoleout.h"
#include "gtkdialogs.h"
#include "signals.h"
#include "toolbar.h"
#include "entireDB.h"
#include "mmsystem.h"
#include "sqlparser.h"
#include "wizards.h"
void disconnect();
INT dummy_connect(INT, CHAR*);
void __dummy( GtkWidget*,gpointer);
void close_results( GtkWidget *w,gpointer   data);
void qexec_quit( GtkWidget *w,gpointer   data);
INT menu_quit( GtkWidget *w,gpointer   data);
void about_box( GtkWidget *w,gpointer   data);
void create_DB_from_file(GtkWidget* w, gpointer data);
void copy_database(GtkWidget* w, gpointer data);
void save_database(GtkWidget* w, gpointer data);
CHAR* get_env(CHAR*);
CHAR *dbenv();
void fatal_nodb(CHAR* dbname, CHAR* errstr, INT killed);
void reload_bases(CHAR* db, INT ask);
void load_database(GtkWidget* w, gpointer data);
void drop_database(GtkWidget* w, gpointer data);
void create_database(GtkWidget* w, gpointer data);
void exec_test_prog(GtkWidget* w, gpointer data);
void gtk_table_out(CHAR* title, CHAR*** str_data, INT a, INT b);
void clear_txt_ctrl(GtkWidget* w);
void load_proc(GtkWidget* w, gpointer txt);
void save_proc(GtkWidget* w, gpointer txt);
void execute_proc(GtkWidget* w, gpointer txt);
void execute_fast(GtkWidget* w, gpointer opt);
void execute_fast_one(GtkWidget* w, gpointer opt);
void execute_fast_two(GtkWidget* w, gpointer opt);
void execute_fast_thr(GtkWidget* w, gpointer opt);
void save_results(GtkWidget* w, gpointer resd);
void save_results_mainwnd(GtkWidget* w, gpointer d);
void destroy_all_components();
void update_entry(GtkWidget* w, gpointer d);
void apply_proc(GtkWidget* w, gpointer d);
INT connect_proc(GtkWidget* w, gpointer d);
INT delete_proc(GtkWidget* w, gpointer d);
CHAR* get_env(CHAR* str);
void env_dialog(GtkWidget* www, gpointer d);
void check_exec_proc(GtkWidget* txt, gpointer dlg);
void query_executor(GtkWidget* w, gpointer data);
void get_main_menu( GtkWidget  *window,GtkWidget **menubar );
INT parse_cmdline(INT lb, CHAR** par, GtkWidget* w);
void setup_components();
void construct_components();
void browse_table(CHAR* tabname);
void destruct_tab_view(GtkWidget* w, gpointer data);
void post_start(GtkWidget* w, gpointer data);
void clear_wname();
void set_wname_tab(CHAR* arg);
void destroy_global_auto_complete();
void process_parser(GtkWidget*);
INT changed_field(GtkWidget*, gpointer);
INT numeric(CHAR* val);
void create_table(GtkWidget* w, gpointer opt);
void tdrop_table(GtkWidget* w, gpointer opt);
void tdrop_view(GtkWidget* w, gpointer opt);
void tinsert(GtkWidget* w, gpointer opt);
void tdelete(GtkWidget* w, gpointer opt);
void wizards(GtkWidget* w, gpointer opt);
void drop_table(CHAR*, INT);
void drop_view(CHAR*, INT);
void drop_tables();
void drop_views();
INT dispatch_autocomplete(GtkWidget* w, GdkEvent* event);
struct result_cache
{
 CHAR*** data;
 INT rows,cols;
};
#endif
