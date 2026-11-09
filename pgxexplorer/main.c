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
 FILE: main.c */
#include "common.h"
#include "main.h"
#include "pgxexplorer.h"
PGconn* connection=NULL;	/* variables here */
CHAR* currentbase=NULL;
CHAR tmpstr[STDSTRING];
GtkWidget* main_wnd=NULL;
GtkWidget* main_vbox=NULL;
GtkWidget* query_exec=NULL;
GtkWidget* qtext=NULL;
GtkWidget* glob_cm=NULL;
INT connected=0;
INT timeout=10;
INT want_all_elems=0;
INT max_tabs=32;
INT cont=1;
struct main_components components;
extern struct mem_list* mlist;
extern struct dmem_list dmlist;
CHAR** autostr=0;
INT    nautostr=0;
INT*   autoidx=0;
CHAR*  pass=0;

void disconnect()
{
 debug("%s:%d disconnect()",__FILE__,__LINE__);
 if (connected) disconnect_db(&connection);
 connected=0;
 connection=0;
 destroy_all_components();
}


void __dummy( GtkWidget *w,gpointer data)
{
 debug("%s:%d __dummy()",__FILE__,__LINE__);
 error("Not yet implemented.\n");
}


void close_results( GtkWidget *w,gpointer data)
{
 debug("%s:%d close_results()",__FILE__,__LINE__);
 gtk_main_quit();
 if (!data) return;
 destroy_widget((GtkWidget*)(data));
}


void qexec_quit(GtkWidget *w,gpointer data)
{
 debug("%s:%d quexec_quit()",__FILE__,__LINE__);
 gtk_main_quit();
 destroy_widget(query_exec);
 query_exec = NULL;
}


INT menu_quit(GtkWidget *w, gpointer data)
{
 debug("%s:%d menu_quit()",__FILE__,__LINE__);
  if (gtk_dialog_yes_no("Are you sure?"))
   {
    disconnect();
    if (currentbase) free(currentbase);
    currentbase=NULL;
    gtk_main_quit();
    if (query_exec)
       {
	qexec_quit(query_exec, NULL);
        destroy_global_auto_complete();
	exit(0);
       }
    dprint_memlist(HERE);
    dshred_memlist(HERE);
    shred_memlist();
    return FALSE;
   }
 return TRUE;
}


INT can_connect(CHAR* templ)
{
 CHAR errstr[MAX_ENTRY_LENGTH];
 debug("%s:%d can_connect(%s)",__FILE__,__LINE__,templ);
 connection = gtk_connect_db(templ, pass, errstr);
 if (!connection) return 0;
 connected=1;
 disconnect();
 return 1;
}


void select_child_main_tree(GtkWidget* w, gpointer data)	/* s, d, c */
{
 debug("%s:%d select_child_main_tree()",__FILE__,__LINE__);
 debug("select_child_main_tree\n");
 /*components.seltype = 0;
 strcpy(components.selected,"");*/
}

void select_tree(GtkWidget* w, gpointer data)		/* ts,td,te,tc,vs,vd,ve,vc,rs,rd,re,rc */
{							/* re - trigger expand, ts table select etc. */
 CHAR* str;
 debug("%s:%d select_tree()",__FILE__,__LINE__);
 debug("select_tree %s\n", (CHAR*)data);
 str = (CHAR*)data;
 switch (str[0])
 {
  case 't': components.seltype= 'T'; break;
  case 'v': components.seltype= 'V'; break;
  case 'r': components.seltype= 'R'; break;
 }
}


void select_child_tree(GtkWidget* w, gpointer data)	/* ts,td,vs,vd,rs,rd */
{
 debug("%s:%d select_child_tree()",__FILE__,__LINE__);
 debug("select_child_tree %s\n",(CHAR*)data);
}


INT dispatch_cmenu(GtkWidget* w, GdkEvent* event)
{
 GdkEventButton* evb;
/* debug("%s:%d dispatch_cmenu()", __FILE__,__LINE__);*/
 if (event->type == GDK_2BUTTON_PRESS || event->type == GDK_3BUTTON_PRESS)
 {
  evb = (GdkEventButton*)event;
  gtk_menu_popup(GTK_MENU(w), NULL,NULL,NULL,NULL, evb->button, evb->time);
  return TRUE;
 }
 return FALSE;
}


void select_final(GtkWidget* w, gpointer data)		/* ts,td,vs,vd,rs,rd */
{
 CHAR* str;
 GtkItem* item;
 GtkWidget* label;
 GtkTreeItem* ti;
 CHAR* name;
 debug("%s:%d select_final()",__FILE__,__LINE__);
 if (!data) { error("no data passed to select_final %s:%d", __FILE__,__LINE__); return ; }
 debug("select_final %s\n",(CHAR*)data);	/*WHAT WAS HAPPEN IN STR[1] */
 str = (CHAR*)data;
 ti = (GtkTreeItem*)(w);
 item = &(ti->item);
 label = GTK_WIDGET(GTK_BIN(item)->child);
 gtk_label_get(GTK_LABEL(label), &name);
 components.last_selected = w;
 switch (str[0])
 {
  case 't': components.seltype=str[0]; strcpy(components.selected, name); break;
  case 'v': components.seltype=str[0]; strcpy(components.selected, name); break;
  case 'r': components.seltype=str[0]; strcpy(components.selected, name); break;
 }
}


void construct_components()
{
 debug("%s:%d construct_components()",__FILE__,__LINE__);
 components.table = NULL;
 components.scrolled_win = NULL;
 components.tree_item_tables = NULL;
 components.subtree_tables = NULL;
 components.tree_item_views = NULL;
 components.subtree_views = NULL;
 components.tree_item_triggers = NULL;
 components.subtree_triggers = NULL;
 components.tree = NULL;
 components.cmenu_tables=NULL;
 components.cmenu_table=NULL;
 components.cmenu_views=NULL;
 components.cmenu_view=NULL;
 components.cmenu_triggers=NULL;
 components.cmenu_trigger=NULL;
 components.table_view=NULL;
 components.ntables=components.nviews=components.ntriggers=0;
 strcpy(components.selected, "");
 components.seltype=0;
 components.rows = components.cols = -1;
 components.arows = components.acols = -1;
 components.rrow = components.rcol = -1;
 components.table_view = NULL;
 components.scroller_view = NULL;
 components.button_save = NULL;
 components.button_destroy = NULL;
 components.table_entry = NULL;
 components.data = NULL;
 components.last_selected = NULL;
 shred_memlist();
}


gpointer mkptr(CHAR* str)
{
 debug("%s:%d mkptr(%s)",__FILE__,__LINE__,str);
 return (gpointer)(str);
}


void subtree_DB_triggers()
{
 CHAR ***ptr;
 INT row,col,i;
 CHAR errstr[MAX_QUERY_LENGTH];
 GtkWidget* subtmp;
#ifdef SMALLOC
 struct dmem_list* mem;				/* ALWAYS INITIALIZE WITH NULL!!! */
 mem = NULL;
#endif
#ifdef SMALLOC
 debug("%s:%d seubtree_DB_triggers()",__FILE__,__LINE__);
 if (!want_all_elems)
 dmem_execute_printf(&mem, connection, &ptr, &row, &col, errstr, 0, HERE,
		 "SELECT tgname from pg_trigger WHERE tgname NOT LIKE 'pg_%%' ORDER BY tgname");
 else
 dmem_execute_printf(&mem, connection, &ptr, &row, &col, errstr, 0, HERE,
		 "SELECT tgname from pg_trigger ORDER BY tgname");
#else
 if (!want_all_elems)
 execute_printf(connection, &ptr, &row, &col, errstr, 0,
		 "SELECT tgname from pg_trigger WHERE tgname NOT LIKE 'pg_%%' ORDER BY tgname");
 else
 execute_printf(connection, &ptr, &row, &col, errstr, 0,
		 "SELECT tgname from pg_trigger ORDER BY tgname");
#endif
 if (!ptr) return;
 if (row<0 || col<0)
   {
    gtk_dialog_printf_big("Error executing query:\nReturn from DBMS: %s\n", errstr);
    return;
   }
 for (i=0;i<row;i++)
  {
   subtmp = gtk_tree_item_new_with_label(ptr[i][0]);
   gtk_signal_connect_object(GTK_OBJECT(subtmp), "event",      GTK_SIGNAL_FUNC(dispatch_cmenu), GTK_OBJECT(components.cmenu_trigger));
   gtk_signal_connect(GTK_OBJECT(subtmp), "select",     GTK_SIGNAL_FUNC(select_final), mkptr("rs"));
   gtk_signal_connect(GTK_OBJECT(subtmp), "deselect",   GTK_SIGNAL_FUNC(select_final), mkptr("rd"));
   gtk_tree_append(GTK_TREE(components.subtree_triggers), subtmp);
   gtk_widget_show(subtmp);
  }
 components.ntriggers=row;
#ifdef SMALLOC
 dmem_free_p3c(&mem, &ptr, row, col, HERE);
 dshred_special_memlist(&mem, HERE);
#else
 free_p3c(&ptr, row, col);
#endif
}


void subtree_DB_views()
{
 CHAR ***ptr;
 INT row,col,i;
 CHAR errstr[MAX_QUERY_LENGTH];
 CHAR* tmps;
 GtkWidget* subtmp;
#ifdef SMALLOC
 struct dmem_list* mem;
 mem = NULL;
#endif
 debug("%s:%d subtree_DB_views()",__FILE__,__LINE__);
#ifdef SMALLOC
 if (!want_all_elems)
 dmem_execute_printf(&mem, connection, &ptr, &row, &col, errstr, 0, HERE,
		 "SELECT schemaname,viewname from pg_views WHERE viewname NOT LIKE 'pg_%%' AND schemaname != 'information_schema' ORDER BY schemaname");
 else
 dmem_execute_printf(&mem, connection, &ptr, &row, &col, errstr, 0, HERE,
		 "SELECT schemaname,viewname from pg_views ORDER BY schemaname");
#else
 if (!want_all_elems)
 execute_printf(connection, &ptr, &row, &col, errstr, 0,
		 "SELECT schemaname,viewname from pg_views WHERE viewname NOT LIKE 'pg_%%' AND schemaname != 'information_schema' ORDER BY schemaname");
 else
 execute_printf(connection, &ptr, &row, &col, errstr, 0,
		 "SELECT schemaname,viewname from pg_views ORDER BY schemaname");
#endif
 if (!ptr) return;
 if (row<0 || col<0)
   {
    gtk_dialog_printf_big("Error executing query:\nReturn from DBMS: %s\nThere: %s %d", errstr, HERE);
    return;
   }
 for (i=0;i<row;i++)
  {
   tmps = malloc(strlen(ptr[i][0])+strlen(ptr[i][1])+3);
   strcpy(tmps,ptr[i][0]);
   strcat(tmps,".");
   strcat(tmps,ptr[i][1]);
   subtmp = gtk_tree_item_new_with_label(tmps);
   gtk_signal_connect_object(GTK_OBJECT(subtmp), "event",      GTK_SIGNAL_FUNC(dispatch_cmenu), GTK_OBJECT(components.cmenu_view));
   gtk_signal_connect(GTK_OBJECT(subtmp), "select",     GTK_SIGNAL_FUNC(select_final), mkptr("vs"));
   gtk_signal_connect(GTK_OBJECT(subtmp), "deselect",   GTK_SIGNAL_FUNC(select_final), mkptr("vd"));
   gtk_tree_append(GTK_TREE(components.subtree_views), subtmp);
   gtk_widget_show(subtmp);
   free(tmps);
  }
 components.nviews=row;
#ifdef SMALLOC
 dmem_free_p3c(&mem, &ptr, row, col, HERE);
 dshred_special_memlist(&mem, HERE);
#else
 free_p3c(&ptr, row, col);
#endif
}


void subtree_DB_tables()
{
 CHAR ***ptr;
 INT row,col,i;
 CHAR errstr[MAX_QUERY_LENGTH];
 GtkWidget* subtmp;
#ifdef SMALLOC
 struct dmem_list* mem;
 mem = NULL;
#endif
 debug("%s:%d subtree_DB_tables()",__FILE__,__LINE__);
 if (!want_all_elems)
#ifdef SMALLOC
 dmem_execute_printf(&mem, connection, &ptr, &row, &col, errstr, 0, HERE,
		 "SELECT tablename from pg_tables WHERE tablename NOT LIKE 'pg_%%' AND tablename NOT LIKE 'sql_%%' ORDER BY tablename");
 else
 dmem_execute_printf(&mem, connection, &ptr, &row, &col, errstr, 0, HERE,
		 "SELECT tablename from pg_tables ORDER BY tablename");
#else
 execute_printf(connection, &ptr, &row, &col, errstr, 0,
		 "SELECT tablename from pg_tables WHERE tablename NOT LIKE 'pg_%%' AND tablename NOT LIKE 'sql_%%' ORDER BY tablename");
 else
 execute_printf(connection, &ptr, &row, &col, errstr, 0,
		 "SELECT tablename from pg_tables ORDER BY tablename");
#endif
 if (!ptr) return;
 if (row<0 || col<0)
   {
    gtk_dialog_printf_big("Error executing query:\nReturn from DBMS: %s\n", errstr);
    return;
   }
 for (i=0;i<row;i++)
  {
   subtmp = gtk_tree_item_new_with_label(ptr[i][0]);
   gtk_signal_connect_object(GTK_OBJECT(subtmp), "event",      GTK_SIGNAL_FUNC(dispatch_cmenu), GTK_OBJECT(components.cmenu_table));
   gtk_signal_connect(GTK_OBJECT(subtmp), "select",     GTK_SIGNAL_FUNC(select_final), mkptr("ts"));
   gtk_signal_connect(GTK_OBJECT(subtmp), "deselect",   GTK_SIGNAL_FUNC(select_final), mkptr("td"));
   gtk_tree_append(GTK_TREE(components.subtree_tables), subtmp);
   gtk_widget_show(subtmp);
  }
 components.ntables=row;
#ifdef SMALLOC
 dmem_free_p3c(&mem, &ptr, row, col, HERE);
 dshred_special_memlist(&mem, HERE);
#else
 free_p3c(&ptr, row, col);
#endif
}


void refresh_all()
{
 CHAR basen[STDSTRING];
 CHAR errstr[MAX_QUERY_LENGTH+100];
 CHAR winname[STDSTRING];
 debug("%s:%d refresh_all()",__FILE__,__LINE__);
 strcpy(basen, currentbase);
 disconnect();
 shred_memlist();
 if (currentbase) free(currentbase);
 currentbase=NULL;
 connection = gtk_connect_db(basen, pass, errstr);
 if (!connection)
     {
      fatal_nodb(basen,errstr,0);
      return ;
     }
 else
     {
      connected=1;
      currentbase = (CHAR*)malloc(strlen(basen)+2);
      strcpy(currentbase, basen);
      sprintf(winname, "PgXexplorer: %s", currentbase);
      gtk_window_set_title (GTK_WINDOW(main_wnd), winname);
      setup_components();
     }
}


void cmenu_response(GtkWidget* w, gpointer data)
{
 debug("%s:%d cmenu_response",__FILE__,__LINE__);	/* ON SELTYPE t one of tables, T All tables (node in tree) */
 					/* analogically v,V,r,R <view,views,trigger,triggers> */
 debug("CMENU: %c %s\n", components.seltype, components.selected);
}


void clear_wname()
{
 CHAR name[STDSTRING];
 debug("%s:%d clear_wname()",__FILE__,__LINE__);
 if (currentbase) sprintf(name,"PgXexplorer: %s", currentbase);
 else sprintf(name, "PgXexplorer (no DB opened currently)");
 gtk_window_set_title (GTK_WINDOW(main_wnd), name);
}


void set_wname_tab(CHAR* arg)
{
 CHAR name[STDSTRING];
 debug("%s:%d set_wname_tab(%s)",__FILE__,__LINE__,arg?arg:"null");
 if (!arg) { error("arg is NULL: %s:%d", __FILE__,__LINE__); sprintf(name,"Error, table name is NULL"); }
 else
   {
    if (currentbase) sprintf(name,"PgXexplorer: %s:%s", currentbase,arg);
    else sprintf(name, "PgXexplorer (no DB opened currently snd try to browse table)");
   }
 gtk_window_set_title (GTK_WINDOW(main_wnd), name);
}


void refresh_tab_view(GtkWidget* w, gpointer data)
{
 debug("%s:%d refresh_tab_view()", __FILE__,__LINE__);
 destruct_tab_view(w,data);
 browse_table(components.selected);
}


INT numeric(CHAR* val)
{
 INT i;
 float v;
 debug("%s:%d numeric(%ws)", __FILE__,__LINE__,val);
 if (!val) return 0;
 i = sscanf(val,"%f", &v);
 if (i==1) return 1;
 else return 0;
}


void delete_current_row(GtkWidget* w, gpointer data)
{
 CHAR*** ptr;
 INT row,col;
 CHAR errstr[MAX_QUERY_LENGTH+100];
 CHAR* querystr;
 CHAR* tmps;
 CHAR colname[STDSTRING];
 INT i,j,x;
 INT wbytes;
 INT nmrc;
 debug("%s:%d delete_current_row()", __FILE__,__LINE__);
 j = 0;
 i = components.last_row;
 if (i<0)
 {
	 gtk_dialog_printf("No row is selected!");
	 return;
 }
 execute_printf(connection, &ptr, &row, &col, errstr, 1, "SELECT * FROM %s LIMIT 1", components.selected);
 if (row<0 || col<0)
   {
    fatal_here(HERE, "get column name failed");
    return;
   }
 strcpy(colname, ptr[0][j]);
 debug("delete row from field field: %d, %d %s:%s\n", i, j, colname,components.data[i+components.rrow][j+components.rcol]);
 wbytes = STDSTRING;
 for (x=0;x<col;x++) wbytes += (strlen(ptr[0][x])+1);
 querystr = (CHAR*)malloc(wbytes+1);
 if (!querystr) fatal_here(HERE, "malloc failed");
 sprintf(querystr, "DELETE FROM %s WHERE ", components.selected);
 for (x=0;x<col;x++)
 {
  if (numeric(ptr[1][x])) nmrc = 1;
  else nmrc = 0;
  tmps = (CHAR*)malloc(strlen(ptr[0][x])+strlen(components.data[i+components.rrow][x+components.rcol])+10);
  if (!tmps) fatal_here(HERE, "malloc failed");
  if (x < col-1)
  {
  	if (nmrc) sprintf(tmps, "%s = %s AND ", ptr[0][x], components.data[i+components.rrow][x+components.rcol]);
  	else sprintf(tmps, "%s = '%s' AND ", ptr[0][x], components.data[i+components.rrow][x+components.rcol]);
  }
  else
  {
  	if (nmrc) sprintf(tmps, "%s = %s", ptr[0][x], components.data[i+components.rrow][x+components.rcol]);
  	else sprintf(tmps, "%s = '%s'", ptr[0][x], components.data[i+components.rrow][x+components.rcol]);
  }
  strcat(querystr, tmps);
  free(tmps);
 }
 debug("query: %d, %d, %s\n", components.rrow, components.rcol, querystr);
 free_p3c(&ptr, row, col);
 execute_printf(connection, &ptr, &row, &col, errstr, 0, querystr);
 if (row<0 || col<0)
   {
    gtk_dialog_printf_big("Delete error!\n%s\n", errstr);
    free(querystr);
    return;
   }
 free_p3c(&ptr, row, col);
 free(querystr);
 destruct_tab_view(w,data);
 browse_table(components.selected);
 return ;
}


void destruct_tab_view(GtkWidget* w, gpointer data)
{
 INT i,rows,cols,j;
 create_pbar("freeing memory...");
 debug("%s:%d destruct_tab_view()",__FILE__,__LINE__);
 clear_wname();
 if (components.button_save) destroy_widget(components.button_save);
 components.button_save = NULL;
 if (components.button_destroy) destroy_widget(components.button_destroy);
 components.button_destroy = NULL;
 if (components.button_refresh) destroy_widget(components.button_refresh);
 components.button_refresh = NULL;
 if (components.button_update) destroy_widget(components.button_update);
 components.button_update = NULL;
 if (components.button_delete) destroy_widget(components.button_delete);
 components.button_delete = NULL;
 rows = components.rows;
 cols = components.cols;
 components.last_selected = NULL;
 if (components.table_view) destroy_widget(components.table_view);
 components.table_view = NULL;
 if (components.rows && components.cols)
    {
#ifdef SMALLOC
     dmfree_p3c(&components.data, components.rows, components.cols, HERE);
#else
     mfree_p3c(&components.data, components.rows, components.cols);
#endif
     components.data = NULL;
     for (i=0;i<rows;i++) for (j=0;j<cols;j++)
       {
	if (components.table_entry[i][j]) destroy_widget(components.table_entry[i][j]);
	components.table_entry[i][j] = NULL;
       }
     for (i=0;i<rows;i++) if (components.table_entry[i]) { free(components.table_entry[i]); components.table_entry[i] = NULL; }
     if (components.table_entry) { free(components.table_entry); components.table_entry = NULL; }
    }
 components.data = NULL;
 components.table_entry = NULL;
 components.rows = -1;
 components.cols = -1;
 components.arows = -1;
 components.acols = -1;
 components.rrow = -1;
 components.rcol = -1;
 if (components.scroller_view) destroy_widget(components.scroller_view);
 components.scroller_view = NULL;
 shred_memlist();
 dshred_memlist(HERE);
 destroy_pbar();
}


void drop_view(CHAR* tabname, INT force)
{
 CHAR*** ptr;
 INT row,col;
 INT ans;
 CHAR errstr[MAX_QUERY_LENGTH+100];
 debug("%s:%d drop_view(%s,%d)",__FILE__,__LINE__,tabname,force);
 execute_printf(connection, &ptr, &row, &col, errstr, 1, "DROP VIEW %s", tabname);
 if (row<0 || col<0)
   {
    if (strstr(errstr,"CASCADE"))
    {
	if (force) ans = 1;
	else ans = gtk_dialog_yes_no_printf("This view is referenced by other views\nDelete cascade (dangerous)?");
	if (!ans) return;
 	execute_printf(connection, &ptr, &row, &col, errstr, 1, "DROP VIEW %s CASCADE", tabname);
	if (row<0 || col<0)
	{
    		if (!force) gtk_dialog_printf_big("Error executing query:\nReturn from DBMS: %s\n", errstr);
		else printf("Error dropping %s:\n%s\n", tabname,errstr);
    		return;
	}
    }
    else
    {
    	if (!force) gtk_dialog_printf_big("Error executing query:\nReturn from DBMS: %s\n", errstr);
	else printf("Error dropping %s:\n%s\n", tabname,errstr);
    	return;
    }
   }
 /*gtk_tree_remove_item(components.tree_item_tables, components.last_selected);*/
 refresh_all();						/* FIXME hardcore!!! */
 free_p3c(&ptr, row, col);
 if (!force) gtk_dialog_printf("View %s dropped", tabname);
}


void drop_table(CHAR* tabname, INT force)
{
 CHAR*** ptr;
 INT row,col;
 INT ans;
 CHAR errstr[MAX_QUERY_LENGTH+100];
 debug("%s:%d drop_table(%s,%d)",__FILE__,__LINE__,tabname,force);
 execute_printf(connection, &ptr, &row, &col, errstr, 1, "DROP TABLE %s", tabname);
 if (row<0 || col<0)
   {
    if (strstr(errstr,"CASCADE"))
    {
	if (force) ans = 1;
	else ans = gtk_dialog_yes_no_printf("This table is referenced by other tables\nDelete cascade (dangerous)?");
	if (!ans) return;
 	execute_printf(connection, &ptr, &row, &col, errstr, 1, "DROP TABLE %s CASCADE", tabname);
	if (row<0 || col<0)
	{
    		if (!force) gtk_dialog_printf_big("Error executing query:\nReturn from DBMS: %s\n", errstr);
		else printf("Error dropping %s:\n%s\n", tabname,errstr);
    		return;
	}
    }
    else
    {
    	if (!force) gtk_dialog_printf_big("Error executing query:\nReturn from DBMS: %s\n", errstr);
	else printf("Error dropping %s:\n%s\n", tabname,errstr);
    	return;
    }
   }
 /*gtk_tree_remove_item(components.tree_item_tables, components.last_selected);*/
 refresh_all();						/* FIXME hardcore!!! */
 free_p3c(&ptr, row, col);
 if (!force) gtk_dialog_printf("Table %s dropped", tabname);
}


void update_database(GtkWidget* w, gpointer data)
{
 INT i,j;
 CHAR* gent;
 INT errs,err,succ;
 debug("%s:%d update_database()", __FILE__,__LINE__);
 errs = 0;
 err  = 0;
 succ = 0;
 for (i=1;i<components.rows;i++)
 for (j=0;j<components.cols;j++)
   {
    gent = gtk_entry_get_text(GTK_ENTRY(components.table_entry[i][j]));
    if(strcmp(gent,components.data[i+components.rrow][j+components.rcol]))
     {
#ifdef _MACHINE_32
	    err = changed_field(components.table_entry[i][j],(void*)((i<<0x10)+j+(1<<0x1F)));
#else
	    err = changed_field(components.table_entry[i][j],(void*)((INT64)((i<<0x10)+j+(1<<0x1F))));
#endif
	    if (err==0) succ++;
	    errs += err;
     }
   }
 if (errs)
 {
	gtk_dialog_printf("There were %d update errors\n%d updates success\n", errs, succ);
 }
}


/*INT changed_field(GtkWidget* field, gpointer which)
{
 CHAR*** ptr;
 INT row,col;
 CHAR errstr[MAX_QUERY_LENGTH+100];
 CHAR querystr[STDSTRING];
 CHAR colname[STDSTRING];
 CHAR* gent;
 INT i,j;
 INT nmrc;
 INT out;
 out = 0;
 if ((INT)which & (1<<0x1C)) out = 1;
 j = (INT)which & 0x7FFF;
 i = ((INT)which & 0x7FFF0000)>>0x10;
 execute_printf(connection, &ptr, &row, &col, errstr, 1, "SELECT * FROM %s LIMIT 1", components.selected);
 if (row<0 || col<0)
   {
    fatal_here(HERE, "get column name failed");
    return 1;
   }
 strcpy(colname, ptr[0][j]);
 free_p3c(&ptr, row, col);
 printf("changed field: %d, %d %s:%s\n", i, j, colname,components.data[i][j]);
 gent = gtk_entry_get_text(GTK_ENTRY(field));
 if (!gent) fatal_here(HERE, "text is null");
 if (numeric(components.data[i][j])) nmrc = 1;
 else nmrc = 0;
 if (!nmrc) sprintf(querystr, "UPDATE %s SET %s='%s' WHERE %s='%s'", components.selected, colname, gent, colname, components.data[i][j]);
 else sprintf(querystr, "UPDATE %s SET %s=%s WHERE %s=%s", components.selected, colname, gent, colname, components.data[i][j]);
 printf("%s\n", querystr);
 execute_printf(connection, &ptr, &row, &col, errstr, 0, querystr);
 if (row<0 || col<0)
   {
    if (out) printf("DBMS warning: %s\n", errstr);
    else gtk_dialog_printf_big("Update error!\n%s\n", errstr);
    gtk_entry_set_text(GTK_ENTRY(field), components.data[i][j]);
    return 1;
   }
 free_p3c(&ptr, row, col);
 free(components.data[i][j]);
 components.data[i][j] = 0;
#ifdef SMALLOC
 components.data[i][j] = (CHAR*)dmmalloc(strlen(gent)+1, HERE);
#else
 components.data[i][j] = (CHAR*)mmalloc(strlen(gent)+1);
#endif
 if (!components.data[i][j]) fatal_here(HERE,"malloc failed");
 strcpy(components.data[i][j], gent);
 return 0;
}*/

INT changed_field(GtkWidget* field, gpointer which)
{
 CHAR*** ptr;
 INT row,col;
 CHAR errstr[MAX_QUERY_LENGTH+100];
 CHAR* querystr;
 CHAR* tmps;
 CHAR colname[STDSTRING];
 CHAR* gent;
 INT i,j,x;
 INT nmrc;
 INT out;
 INT wbytes;
 debug("%s:%d changed_field()", __FILE__,__LINE__);
 out = 0;
#ifdef _MACHINE_32
 if ((INT)which & (1<<0x1F)) out = 1;
 j = (INT)which & 0x7FFF;
 i = ((INT)which & 0x7FFF0000)>>0x10;
#else
 if ((INT64)which & (1<<0x1F)) out = 1;
 j = (INT64)which & 0x7FFF;
 i = ((INT64)which & 0x7FFF0000)>>0x10;
#endif
 execute_printf(connection, &ptr, &row, &col, errstr, 1, "SELECT * FROM %s LIMIT 2", components.selected);
 if (row<0 || col<0)
   {
    fatal_here(HERE, "get column name failed");
    return 1;
   }
 strcpy(colname, ptr[0][j]);
 debug("changed field: %d, %d %s:%s\n", i, j, colname,components.data[i+components.rrow][j+components.rcol]);
 gent = gtk_entry_get_text(GTK_ENTRY(field));
 if (!gent) fatal_here(HERE, "text is null");
 /*if (!strcmp(gent,"")) */
 if (numeric(components.data[i+components.rrow][j+components.rcol])) nmrc = 1;
 else nmrc = 0;
 wbytes = STDSTRING;
 for (x=0;x<col;x++) wbytes += (strlen(ptr[0][x])+1);
 querystr = (CHAR*)malloc(wbytes+1);
 if (!querystr) fatal_here(HERE, "malloc failed");
 if (!nmrc) sprintf(querystr, "UPDATE %s SET %s='%s' WHERE ", components.selected, colname, gent); //colname, components.data[i][j]);
 else sprintf(querystr, "UPDATE %s SET %s=%s WHERE ", components.selected, colname, gent); //colname, components.data[i][j]);
 for (x=0;x<col;x++)
 {
  debug("x = %d\n", x);
  if (numeric(ptr[1][x])) nmrc = 1;
  else nmrc = 0;
  tmps = (CHAR*)malloc(strlen(ptr[0][x])+strlen(components.data[i+components.rrow][x+components.rcol])+10);
  if (!tmps) fatal_here(HERE, "malloc failed");
  if (x < col-1)
  {
  	if (nmrc) sprintf(tmps, "%s = %s AND ", ptr[0][x], components.data[i+components.rrow][x+components.rcol]);
  	else sprintf(tmps, "%s = '%s' AND ", ptr[0][x], components.data[i+components.rrow][x+components.rcol]);
  }
  else
  {
  	if (nmrc) sprintf(tmps, "%s = %s", ptr[0][x], components.data[i+components.rrow][x+components.rcol]);
  	else sprintf(tmps, "%s = '%s'", ptr[0][x], components.data[i+components.rrow][x+components.rcol]);
  }
  strcat(querystr, tmps);
  free(tmps);
 }
 debug("query: %d, %d, %s\n", components.rrow, components.rcol, querystr);
 free_p3c(&ptr, row, col);
 execute_printf(connection, &ptr, &row, &col, errstr, 0, querystr);
 if (row<0 || col<0)
   {
    if (out) printf("DBMS warning: %s\n", errstr);
    else gtk_dialog_printf_big("Update error!\n%s\n", errstr);
    gtk_entry_set_text(GTK_ENTRY(field), components.data[i+components.rrow][j+components.rcol]);
    free(querystr);
    return 1;
   }
 free_p3c(&ptr, row, col);
 free(querystr);
 /*free(components.data[i+components.rrow][j+components.rcol]);
 components.data[i+components.rrow][j+components.rcol] = 0;*/
#ifdef SMALLOC
 components.data[i+components.rrow][j+components.rcol] = (CHAR*)dmrealloc(components.data[i+components.rrow][j+components.rcol], strlen(gent)+1, HERE);
#else
 components.data[i+components.rrow][j+components.rcol] = (CHAR*)mrealloc(components.data[i+components.rrow][j+components.rcol], strlen(gent)+1);
#endif
 if (!components.data[i+components.rrow][j+components.rcol]) fatal_here(HERE,"malloc failed");
 strcpy(components.data[i+components.rrow][j+components.rcol], gent);
 return 0;
}


void selected_field(GtkWidget* w, GdkEventAny* ev, gpointer which)
{
 //HERE when MORE ARGS PASSED BY EVENT ^             ^ here addictional DATA
 debug("%s:%d selected_field()", __FILE__,__LINE__);
#ifdef _MACHINE_32
 components.last_row = (INT)which;
#else
 components.last_row = (INT)((INT64)which);
#endif
}


void browse_table(CHAR* tabname)
{
 CHAR*** ptr;
 INT row,col;
 CHAR errstr[MAX_QUERY_LENGTH+100];
 INT i,j;
 INT rrow,rcol;
 debug("%s:%d browse_table(%s)",__FILE__,__LINE__,tabname);
 if (!connection) error("no_connection %s:%d", __FILE__,__LINE__);
#ifdef SMALLOC
 dmexecute_printf(connection, &ptr, &row, &col, errstr, 1, HERE, "SELECT * FROM %s", tabname);
#else
 mexecute_printf(connection, &ptr, &row, &col, errstr, 1, "SELECT * FROM %s", tabname);
#endif
 if (row<0 || col<0)
   {
    gtk_dialog_printf_big("Error executing query:\nReturn from DBMS: %s\n", errstr);
    return;
   }
 /*res_cache.cols = col;
 res_cache.rows = row;*/
 debug("%d:%d, %d:%d\n",row,components.rows,col,components.cols);
 rrow=rcol=0;
 components.acols = col;
 components.arows = row;
 components.last_row = -1;
 if (col>32 || row>512)
   {
    gtk_dialog_printf("Result is too big, rows=%d, columns=%d\nwill be truncated!", row, col);
    if (row>512) col = (col>12 )?11 :col;
    else if (row>256)  col = (col>24 )?24 :col;
    else if (row>128)  col = (col>48 )?48 :col;
    else             col = (col>64)?64:col;
    row = (row>512)?512:row;
    col = (col>512)?512:col;
    debug("%d:%d, %d:%d\n",row,components.rows,col,components.cols);
    if (row<components.arows) rrow = gtk_dialog_getnum(0, components.arows-row,"Wchich rows display? (start from)");
    if (col<components.acols) rcol = gtk_dialog_getnum(0, components.acols-col,"Wchich columns display? (start from)");
   }
 debug("row=%d, col=%d\n",row,col);
 components.cols = col;
 components.rows = row;
 components.rcol = rcol;
 components.rrow = rrow;
 components.data = ptr;
 components.table_entry = (GtkWidget***)malloc(row*sizeof(GtkWidget**));
 for (i=0;i<row;i++) components.table_entry[i] = (GtkWidget**)malloc(col*sizeof(GtkWidget*));
 components.table_view = gtk_table_new(row, col, FALSE);
/* res_cache.data = components.data;*/
 for (i=0;i<row;i++)
 for (j=0;j<col;j++) components.table_entry[i][j] = NULL;
 create_pbar("Wait while filling table...");
 for (i=0;i<row;i++)
 for (j=0;j<col;j++)
   {
    components.table_entry[i][j] = gtk_entry_new();						/*XXX there were malloc error */
    if (i) gtk_entry_set_text(GTK_ENTRY(components.table_entry[i][j]), ptr[i+rrow][j+rcol]);
    else   gtk_entry_set_text(GTK_ENTRY(components.table_entry[i][j]), ptr[0][j+rcol]);
    if (!i) gtk_entry_set_editable(GTK_ENTRY(components.table_entry[i][j]), FALSE);
    else
    {
#ifdef _MACHINE_32
       	gtk_signal_connect(GTK_OBJECT(components.table_entry[i][j]),"activate", GTK_SIGNAL_FUNC(changed_field), (void*)((i<<0x10)+j));
   	gtk_signal_connect(GTK_OBJECT(components.table_entry[i][j]),"focus_in_event", GTK_SIGNAL_FUNC(selected_field), (void*)(i));
#else
       	gtk_signal_connect(GTK_OBJECT(components.table_entry[i][j]),"activate", GTK_SIGNAL_FUNC(changed_field), (void*)((INT64)((i<<0x10)+j)));
   	gtk_signal_connect(GTK_OBJECT(components.table_entry[i][j]),"focus_in_event", GTK_SIGNAL_FUNC(selected_field), (void*)((INT64)i));
#endif
    }
    gtk_table_attach(GTK_TABLE (components.table_view), components.table_entry[i][j], j, j+1, i, i+1,GTK_FILL,GTK_FILL, 0, 0);
    gtk_widget_show(components.table_entry[i][j]);
    if (!cont) { gtk_dialog_printf("Interrupted!"); goto out_loop; }
   }
 out_loop:
 gtk_widget_show (components.table_view);
 destroy_pbar();
 components.scroller_view = gtk_scrolled_window_new(NULL,NULL);
 gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(components.scroller_view), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
 gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(components.scroller_view), components.table_view);
 gtk_container_set_border_width(GTK_CONTAINER(components.scroller_view), 2);
 /*gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(close_results), (gpointer)(window));*/
 gtk_widget_show(components.scroller_view);
 gtk_table_attach(GTK_TABLE(components.table), components.scroller_view, 20, 80, 0, 77, 	/* CAN BE CHANGED */
		 GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND, 0, 0);
 components.button_destroy = gtk_button_new_with_label("Destroy");
 gtk_signal_connect(GTK_OBJECT(components.button_destroy),"clicked", GTK_SIGNAL_FUNC(destruct_tab_view), NULL);
 gtk_table_attach (GTK_TABLE(components.table), components.button_destroy, 20, 32, 78, 80,GTK_FILL|GTK_SHRINK,GTK_FILL|GTK_SHRINK, 0, 0);
 gtk_widget_show(components.button_destroy);
 components.button_save = gtk_button_new_with_label("Save");
 gtk_signal_connect(GTK_OBJECT(components.button_save),"clicked", GTK_SIGNAL_FUNC(save_results_mainwnd), NULL);
 gtk_table_attach (GTK_TABLE (components.table), components.button_save, 32, 44, 78, 80,GTK_FILL|GTK_SHRINK,GTK_FILL|GTK_SHRINK, 0, 0);
 gtk_widget_show(components.button_save);
 components.button_refresh = gtk_button_new_with_label("Refresh");
 gtk_signal_connect(GTK_OBJECT(components.button_refresh),"clicked", GTK_SIGNAL_FUNC(refresh_tab_view), NULL);
 gtk_table_attach (GTK_TABLE (components.table), components.button_refresh, 44, 56, 78, 80,GTK_FILL|GTK_SHRINK,GTK_FILL|GTK_SHRINK, 0, 0);
 gtk_widget_show(components.button_refresh);
 components.button_update = gtk_button_new_with_label("Update");
 gtk_signal_connect(GTK_OBJECT(components.button_update),"clicked", GTK_SIGNAL_FUNC(update_database), NULL);
 gtk_table_attach (GTK_TABLE (components.table), components.button_update, 56, 68, 78, 80,GTK_FILL|GTK_SHRINK,GTK_FILL|GTK_SHRINK, 0, 0);
 gtk_widget_show(components.button_update);
 components.button_delete = gtk_button_new_with_label("Delete Row");
 gtk_signal_connect(GTK_OBJECT(components.button_delete),"clicked", GTK_SIGNAL_FUNC(delete_current_row), NULL);
 gtk_table_attach (GTK_TABLE (components.table), components.button_delete, 68, 80, 78, 80,GTK_FILL|GTK_SHRINK,GTK_FILL|GTK_SHRINK, 0, 0);
 gtk_widget_show(components.button_delete);
 set_wname_tab(tabname);
}


void cmenu_browse(GtkWidget* w, gpointer data)
{
 debug("%s:%d cmenu_browse()",__FILE__,__LINE__);	/* ON SELTYPE t one of tables, T All tables (node in tree) */
 							/* analogically v,V,r,R <view,views,trigger,triggers> */
 print_memlist();
 if (components.seltype=='t')
 {
  destruct_tab_view(NULL,NULL);
  browse_table(components.selected);
 }
 else if (components.seltype=='v')
 {
  destruct_tab_view(NULL,NULL);
  browse_table(components.selected);
 }
 else gtk_dialog_printf("Not yet implemented!");
}


void drop_views()
{
 CHAR ***ptr;
 INT row,col,i;
 CHAR errstr[MAX_QUERY_LENGTH];
 CHAR* tmps;
#ifdef SMALLOC
 struct dmem_list* mem;
 mem = NULL;
#endif
 debug("%s:%d drop_views()",__FILE__,__LINE__);
#ifdef SMALLOC
 if (!want_all_elems)
 dmem_execute_printf(&mem, connection, &ptr, &row, &col, errstr, 0, HERE,
		 "SELECT schemaname,viewname from pg_views WHERE viewname NOT LIKE 'pg_%%' AND schemaname != 'information_schema' ORDER BY schemaname");
 else
 dmem_execute_printf(&mem, connection, &ptr, &row, &col, errstr, 0, HERE,
		 "SELECT schemaname,viewname from pg_views ORDER BY schemaname");
#else
 if (!want_all_elems)
 execute_printf(connection, &ptr, &row, &col, errstr, 0,
		 "SELECT schemaname,viewname from pg_views WHERE viewname NOT LIKE 'pg_%%' AND schemaname != 'information_schema' ORDER BY schemaname");
 else
 execute_printf(connection, &ptr, &row, &col, errstr, 0,
		 "SELECT schemaname,viewname from pg_views ORDER BY schemaname");
#endif
 if (!ptr) return;
 if (row<0 || col<0)
   {
    gtk_dialog_printf_big("Error executing query:\nReturn from DBMS: %s\nThere: %s %d", errstr, HERE);
    return;
   }
 for (i=0;i<row;i++)
  {
   tmps = malloc(strlen(ptr[i][0])+strlen(ptr[i][1])+3);
   strcpy(tmps,ptr[i][0]);
   strcat(tmps,".");
   strcat(tmps,ptr[i][1]);
   drop_view(tmps, 1);
   free(tmps);
  }
#ifdef SMALLOC
 dmem_free_p3c(&mem, &ptr, row, col, HERE);
 dshred_special_memlist(&mem, HERE);
#else
 free_p3c(&ptr, row, col);
#endif
}


void drop_tables()
{
 CHAR ***ptr;
 INT row,col,i;
 CHAR errstr[MAX_QUERY_LENGTH];
#ifdef SMALLOC
 struct dmem_list* mem;
 mem = NULL;
#endif
 debug("%s:%d drop_tables()",__FILE__,__LINE__);
 if (!want_all_elems)
#ifdef SMALLOC
 dmem_execute_printf(&mem, connection, &ptr, &row, &col, errstr, 0, HERE,
		 "SELECT tablename from pg_tables WHERE tablename NOT LIKE 'pg_%%' AND tablename NOT LIKE 'sql_%%'");
 else
 dmem_execute_printf(&mem, connection, &ptr, &row, &col, errstr, 0, HERE,
		 "SELECT tablename from pg_tables");
#else
 execute_printf(connection, &ptr, &row, &col, errstr, 0,
		 "SELECT tablename from pg_tables WHERE tablename NOT LIKE 'pg_%%' AND tablename NOT LIKE 'sql_%%'");
 else
 execute_printf(connection, &ptr, &row, &col, errstr, 0,
		 "SELECT tablename from pg_tables");
#endif
 if (!ptr) return;
 if (row<0 || col<0)
   {
    gtk_dialog_printf_big("Error executing query:\nReturn from DBMS: %s\n", errstr);
    return;
   }
 for (i=0;i<row;i++) drop_table(ptr[i][0], 1);
#ifdef SMALLOC
 dmem_free_p3c(&mem, &ptr, row, col, HERE);
 dshred_special_memlist(&mem, HERE);
#else
 free_p3c(&ptr, row, col);
#endif
}


void cmenu_delete(GtkWidget* w, gpointer data)
{
  CHAR* query;
  CHAR ***ptr;
  INT row,col;
  CHAR errstr[MAX_QUERY_LENGTH];
#ifdef SMALLOC
  struct dmem_list* mem;
  mem = NULL;
#endif
  debug("%s:%d cmenu_delete()",__FILE__,__LINE__);
  query = delete_wizard(components.selected);
  if (!query)
  {
	gtk_dialog_printf("Delete wizard returned NULL query!");
	return ;
  }
  debug("QUERY = %s\n", query);
#ifdef SMALLOC
 dmem_execute_printf(&mem, connection, &ptr, &row, &col, errstr, 0, HERE,query);
#else
 execute_printf(connection, &ptr, &row, &col, errstr, 0, query);
#endif
  if (row < 0 || col < 0)
  {
    gtk_dialog_printf_big("Delete error:\nReturn from DBMS: %s\n", errstr);
    free(query);
    return;
  }
  destruct_tab_view(NULL,NULL);
/*  browse_table(components.selected);*/
  free(query);
#ifdef SMALLOC
 dmem_free_p3c(&mem, &ptr, row, col, HERE);
 dshred_special_memlist(&mem, HERE);
#else
 free_p3c(&ptr, row, col);
#endif
}


void cmenu_insert(GtkWidget* w, gpointer data)
{
  CHAR* query;
  CHAR ***ptr;
  INT row,col;
  CHAR errstr[MAX_QUERY_LENGTH];
#ifdef SMALLOC
  struct dmem_list* mem;
  mem = NULL;
#endif
  debug("%s:%d cmenu_insert()",__FILE__,__LINE__);
  query = insert_wizard(components.selected);
  if (!query)
  {
	gtk_dialog_printf("Insert wizard returned NULL query!");
	return ;
  }
  debug("QUERY = %s\n", query);
#ifdef SMALLOC
 dmem_execute_printf(&mem, connection, &ptr, &row, &col, errstr, 0, HERE,query);
#else
 execute_printf(connection, &ptr, &row, &col, errstr, 0, query);
#endif
  if (row < 0 || col < 0)
  {
    gtk_dialog_printf_big("Insert error:\nReturn from DBMS: %s\n", errstr);
    free(query);
    return;
  }
  destruct_tab_view(NULL,NULL);
/*  browse_table(components.selected);*/
  free(query);
#ifdef SMALLOC
 dmem_free_p3c(&mem, &ptr, row, col, HERE);
 dshred_special_memlist(&mem, HERE);
#else
 free_p3c(&ptr, row, col);
#endif
}


void cmenu_drop(GtkWidget* w, gpointer data)
{
 debug("%s:%d cmenu_drop()",__FILE__,__LINE__);
 if (components.seltype=='t')
 {
  destruct_tab_view(NULL,NULL);
  drop_table(components.selected, 0);
 }
 else if (components.seltype=='v')
 {
  destruct_tab_view(NULL,NULL);
  drop_view(components.selected, 0);
 }
 else if (components.seltype=='T')
 {
  destruct_tab_view(NULL,NULL);
  drop_tables();
 }
 else if (components.seltype=='V')
 {
  destruct_tab_view(NULL,NULL);
  drop_views();
 }
 else gtk_dialog_printf("Not implemented/Not known!");
}


void create_context_menus()
{
 GtkWidget* menu_item;
 debug("%s:%d create_context_menus()",__FILE__,__LINE__);
 /* TABLE */
 components.cmenu_table = gtk_menu_new();
 menu_item = gtk_menu_item_new_with_label("Browse table");		//DONE (but no dlete row)
 gtk_menu_append(GTK_MENU(components.cmenu_table), menu_item);
 gtk_signal_connect(GTK_OBJECT(menu_item), "activate", GTK_SIGNAL_FUNC(cmenu_browse), NULL);
 gtk_widget_show(menu_item);
 menu_item = gtk_menu_item_new_with_label("Drop this table");		//DONE
 gtk_menu_append(GTK_MENU(components.cmenu_table), menu_item);
 gtk_signal_connect(GTK_OBJECT(menu_item), "activate", GTK_SIGNAL_FUNC(cmenu_drop), NULL);
 gtk_widget_show(menu_item);
 menu_item = gtk_menu_item_new_with_label("Browse Entire DB");		//DONE
 gtk_menu_append(GTK_MENU(components.cmenu_table), menu_item);
 gtk_signal_connect(GTK_OBJECT(menu_item), "activate", GTK_SIGNAL_FUNC(cmenu_browse_all), NULL);
 gtk_widget_show(menu_item);
 menu_item = gtk_menu_item_new_with_label("Insert");			//DONE
 gtk_menu_append(GTK_MENU(components.cmenu_table), menu_item);
 gtk_signal_connect(GTK_OBJECT(menu_item), "activate", GTK_SIGNAL_FUNC(cmenu_insert), NULL);
 gtk_widget_show(menu_item);
 menu_item = gtk_menu_item_new_with_label("Delete");			//DONE
 gtk_menu_append(GTK_MENU(components.cmenu_table), menu_item);
 gtk_signal_connect(GTK_OBJECT(menu_item), "activate", GTK_SIGNAL_FUNC(cmenu_delete), NULL);
 gtk_widget_show(menu_item);
 /* TABLE ENDS */
 /* TABLES */
 components.cmenu_tables = gtk_menu_new();
 menu_item = gtk_menu_item_new_with_label("Browse entire DB");		//DONE
 gtk_menu_append(GTK_MENU(components.cmenu_tables), menu_item);
 gtk_signal_connect(GTK_OBJECT(menu_item), "activate", GTK_SIGNAL_FUNC(cmenu_browse_all), NULL);
 gtk_widget_show(menu_item);
 menu_item = gtk_menu_item_new_with_label("Drop all tables");		//DONE
 gtk_menu_append(GTK_MENU(components.cmenu_tables), menu_item);
 gtk_signal_connect(GTK_OBJECT(menu_item), "activate", GTK_SIGNAL_FUNC(cmenu_drop), NULL);
 gtk_widget_show(menu_item);
 menu_item = gtk_menu_item_new_with_label("Create table");
 gtk_menu_append(GTK_MENU(components.cmenu_tables), menu_item);
 gtk_signal_connect(GTK_OBJECT(menu_item), "activate", GTK_SIGNAL_FUNC(create_table), NULL);
 gtk_widget_show(menu_item);
 /* TABLE ENDS */
 /* VIEW */
 components.cmenu_view = gtk_menu_new();
 menu_item = gtk_menu_item_new_with_label("Browse View");		//DONE
 gtk_menu_append(GTK_MENU(components.cmenu_view), menu_item);
 gtk_signal_connect(GTK_OBJECT(menu_item), "activate", GTK_SIGNAL_FUNC(cmenu_browse), NULL);
 gtk_widget_show(menu_item);
 menu_item = gtk_menu_item_new_with_label("Drop this view");		//DONE
 gtk_menu_append(GTK_MENU(components.cmenu_view), menu_item);
 gtk_signal_connect(GTK_OBJECT(menu_item), "activate", GTK_SIGNAL_FUNC(cmenu_drop), NULL);
 gtk_widget_show(menu_item);
 menu_item = gtk_menu_item_new_with_label("Browse Entire DB");		//DONE
 gtk_menu_append(GTK_MENU(components.cmenu_view), menu_item);
 gtk_signal_connect(GTK_OBJECT(menu_item), "activate", GTK_SIGNAL_FUNC(cmenu_browse_all), mkptr("bla"));
 gtk_widget_show(menu_item);
 /* VIEW ENDS */
 /* VIEWS */
 components.cmenu_views = gtk_menu_new();
 menu_item = gtk_menu_item_new_with_label("Testing Context menu");	//CREATE TABLE
 gtk_menu_append(GTK_MENU(components.cmenu_views), menu_item);
 gtk_signal_connect(GTK_OBJECT(menu_item), "activate", GTK_SIGNAL_FUNC(cmenu_response), NULL);
 gtk_widget_show(menu_item);
 menu_item = gtk_menu_item_new_with_label("Drop all views");		//DONE
 gtk_menu_append(GTK_MENU(components.cmenu_views), menu_item);
 gtk_signal_connect(GTK_OBJECT(menu_item), "activate", GTK_SIGNAL_FUNC(cmenu_drop), NULL);
 gtk_widget_show(menu_item);
 menu_item = gtk_menu_item_new_with_label("Browse Entire DB");		//DONE
 gtk_menu_append(GTK_MENU(components.cmenu_views), menu_item);
 gtk_signal_connect(GTK_OBJECT(menu_item), "activate", GTK_SIGNAL_FUNC(cmenu_browse_all), mkptr("bla"));
 gtk_widget_show(menu_item);
 /* VIEWS ENDS */
 /* TRIGGER */
 components.cmenu_trigger = gtk_menu_new();
 menu_item = gtk_menu_item_new_with_label("Browse trigger");
 gtk_menu_append(GTK_MENU(components.cmenu_trigger), menu_item);
 gtk_signal_connect(GTK_OBJECT(menu_item), "activate", GTK_SIGNAL_FUNC(cmenu_browse), NULL);
 gtk_widget_show(menu_item);
 menu_item = gtk_menu_item_new_with_label("Delete this trigger");
 gtk_menu_append(GTK_MENU(components.cmenu_trigger), menu_item);
 gtk_signal_connect(GTK_OBJECT(menu_item), "activate", GTK_SIGNAL_FUNC(cmenu_drop), NULL);
 gtk_widget_show(menu_item);
 menu_item = gtk_menu_item_new_with_label("Browse Entire DB");
 gtk_menu_append(GTK_MENU(components.cmenu_trigger), menu_item);
 gtk_signal_connect(GTK_OBJECT(menu_item), "activate", GTK_SIGNAL_FUNC(cmenu_browse_all), mkptr("bla"));
 gtk_widget_show(menu_item);
 /* TRIGGER ENDS */
 /* TRIGGERSS */
 components.cmenu_triggers = gtk_menu_new();
 menu_item = gtk_menu_item_new_with_label("Testing Context menu");
 gtk_menu_append(GTK_MENU(components.cmenu_triggers), menu_item);
 gtk_signal_connect(GTK_OBJECT(menu_item), "activate", GTK_SIGNAL_FUNC(cmenu_response), NULL);
 gtk_widget_show(menu_item);
 menu_item = gtk_menu_item_new_with_label("Delete all trigers");
 gtk_menu_append(GTK_MENU(components.cmenu_triggers), menu_item);
 gtk_signal_connect(GTK_OBJECT(menu_item), "activate", GTK_SIGNAL_FUNC(cmenu_drop), NULL);
 gtk_widget_show(menu_item);
 menu_item = gtk_menu_item_new_with_label("Browse Entire DB");
 gtk_menu_append(GTK_MENU(components.cmenu_triggers), menu_item);
 gtk_signal_connect(GTK_OBJECT(menu_item), "activate", GTK_SIGNAL_FUNC(cmenu_browse_all), mkptr("bla"));
 gtk_widget_show(menu_item);
 /* TRIGGERS ENDS */
}


void destroy_all_components()		/* FIXME SOMEWHERE GTKWIDGET IS ALREADY FREED?? */
{
 debug("%s:%d destroy_all_components()",__FILE__,__LINE__);
 destruct_tab_view(NULL,NULL);
 components.seltype=0;
 strcpy(components.selected,"");
 if (components.scrolled_win) destroy_widget(components.scrolled_win);
 components.scrolled_win = NULL;
 if (components.table) destroy_widget(components.table);
 components.table = NULL;
 if (components.table_view) destroy_widget(components.table_view);
 components.table_view = NULL;
 if (components.tree) destroy_widget(components.tree);
 components.tree = NULL;
 if (components.subtree_tables) destroy_widget(components.subtree_tables);
 components.subtree_tables = NULL;
 if (components.tree_item_tables) destroy_widget(components.tree_item_tables);
 components.tree_item_tables = NULL;
 if (components.subtree_views) destroy_widget(components.subtree_views);
 components.subtree_views = NULL;
 if (components.tree_item_views) destroy_widget(components.tree_item_views);
 components.tree_item_views = NULL;
 if (components.subtree_triggers) destroy_widget(components.subtree_triggers);
 components.subtree_triggers = NULL;
 if (components.tree_item_triggers) destroy_widget(components.tree_item_triggers);
 components.tree_item_triggers = NULL;
 if (components.cmenu_table) destroy_widget(components.cmenu_table);
 components.cmenu_table = NULL;
 if (components.cmenu_tables) destroy_widget(components.cmenu_tables);
 components.cmenu_tables = NULL;
 if (components.cmenu_view) destroy_widget(components.cmenu_view);
 components.cmenu_view = NULL;
 if (components.cmenu_views) destroy_widget(components.cmenu_views);
 components.cmenu_views = NULL;
 if (components.cmenu_trigger) destroy_widget(components.cmenu_trigger);
 components.cmenu_trigger = NULL;
 if (components.cmenu_triggers) destroy_widget(components.cmenu_triggers);
 components.cmenu_triggers = NULL;
 components.button_save = components.button_destroy = NULL;
}


void setup_components()
{
 debug("%s:%d setup_components()",__FILE__,__LINE__);
 if (!connection) return;
 destroy_all_components();
 components.table = gtk_table_new(80,80,FALSE);
 components.scrolled_win = gtk_scrolled_window_new(NULL,NULL);
 gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(components.scrolled_win),
		 GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
 gtk_widget_set_usize(components.scrolled_win, 160,540);
 create_context_menus();
 components.tree = gtk_tree_new();
 gtk_signal_connect(GTK_OBJECT(components.tree), "select_child",           GTK_SIGNAL_FUNC(select_child_main_tree), mkptr("s"));
 gtk_signal_connect(GTK_OBJECT(components.tree), "unselect_child",         GTK_SIGNAL_FUNC(select_child_main_tree), mkptr("d"));
 gtk_signal_connect(GTK_OBJECT(components.tree), "selection_changed",      GTK_SIGNAL_FUNC(select_child_main_tree), mkptr("c"));
 gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(components.scrolled_win), components.tree);
 gtk_tree_set_selection_mode(GTK_TREE(components.tree), GTK_SELECTION_SINGLE);
 gtk_widget_show(components.tree);
 /* TABLES */
 components.tree_item_tables = gtk_tree_item_new_with_label("Tables");
 gtk_signal_connect_object(GTK_OBJECT(components.tree_item_tables), "event",GTK_SIGNAL_FUNC(dispatch_cmenu), GTK_OBJECT(components.cmenu_tables));
 gtk_signal_connect(GTK_OBJECT(components.tree_item_tables), "select",   GTK_SIGNAL_FUNC(select_tree), mkptr("ts"));
 gtk_signal_connect(GTK_OBJECT(components.tree_item_tables), "deselect", GTK_SIGNAL_FUNC(select_tree), mkptr("td"));
 gtk_signal_connect(GTK_OBJECT(components.tree_item_tables), "expand",   GTK_SIGNAL_FUNC(select_tree), mkptr("te"));
 gtk_signal_connect(GTK_OBJECT(components.tree_item_tables), "collapse", GTK_SIGNAL_FUNC(select_tree), mkptr("tc"));
 gtk_tree_append(GTK_TREE(components.tree), components.tree_item_tables);
 gtk_widget_show(components.tree_item_tables);
 components.subtree_tables = gtk_tree_new();
 gtk_signal_connect(GTK_OBJECT(components.subtree_tables), "select_child",   GTK_SIGNAL_FUNC(select_child_tree), mkptr("ts"));
 gtk_signal_connect(GTK_OBJECT(components.subtree_tables), "unselect_child", GTK_SIGNAL_FUNC(select_child_tree), mkptr("td"));
 gtk_tree_set_selection_mode(GTK_TREE(components.subtree_tables), GTK_SELECTION_SINGLE);
 gtk_tree_set_view_mode(GTK_TREE(components.subtree_tables), GTK_TREE_VIEW_ITEM);
 /*gtk_tree_item_collapse(GTK_TREE_ITEM(components.tree_item_tables));*/
 subtree_DB_tables();
 gtk_tree_item_set_subtree(GTK_TREE_ITEM(components.tree_item_tables), components.subtree_tables);
 gtk_widget_show(components.subtree_tables);
 /* TABLES ENDS */
 /* VIEWS */
 components.tree_item_views = gtk_tree_item_new_with_label("Views");
 gtk_signal_connect_object(GTK_OBJECT(components.tree_item_views), "event",GTK_SIGNAL_FUNC(dispatch_cmenu), GTK_OBJECT(components.cmenu_views));
 gtk_signal_connect(GTK_OBJECT(components.tree_item_views), "select",   GTK_SIGNAL_FUNC(select_tree), mkptr("vs"));
 gtk_signal_connect(GTK_OBJECT(components.tree_item_views), "deselect", GTK_SIGNAL_FUNC(select_tree), mkptr("vd"));
 gtk_signal_connect(GTK_OBJECT(components.tree_item_views), "expand",   GTK_SIGNAL_FUNC(select_tree), mkptr("ve"));
 gtk_signal_connect(GTK_OBJECT(components.tree_item_views), "collapse", GTK_SIGNAL_FUNC(select_tree), mkptr("vc"));
 gtk_tree_append(GTK_TREE(components.tree), components.tree_item_views);
 gtk_widget_show(components.tree_item_views);
 components.subtree_views = gtk_tree_new();
 gtk_signal_connect(GTK_OBJECT(components.subtree_views), "select_child",   GTK_SIGNAL_FUNC(select_child_tree), mkptr("vs"));
 gtk_signal_connect(GTK_OBJECT(components.subtree_views), "unselect_child", GTK_SIGNAL_FUNC(select_child_tree), mkptr("vd"));
 gtk_tree_set_selection_mode(GTK_TREE(components.subtree_views), GTK_SELECTION_SINGLE);
 gtk_tree_set_view_mode(GTK_TREE(components.subtree_views), GTK_TREE_VIEW_ITEM);
 subtree_DB_views();
 gtk_tree_item_set_subtree(GTK_TREE_ITEM(components.tree_item_views), components.subtree_views);
 gtk_widget_show(components.subtree_views);
 /* VIEWS ENDS */
 /* TRIGGERS */
 components.tree_item_triggers = gtk_tree_item_new_with_label("Triggers");
 gtk_signal_connect_object(GTK_OBJECT(components.tree_item_triggers), "event",GTK_SIGNAL_FUNC(dispatch_cmenu), GTK_OBJECT(components.cmenu_triggers));
 gtk_signal_connect(GTK_OBJECT(components.tree_item_triggers), "select",   GTK_SIGNAL_FUNC(select_tree), mkptr("rs"));
 gtk_signal_connect(GTK_OBJECT(components.tree_item_triggers), "deselect", GTK_SIGNAL_FUNC(select_tree), mkptr("rd"));
 gtk_signal_connect(GTK_OBJECT(components.tree_item_triggers), "expand",   GTK_SIGNAL_FUNC(select_tree), mkptr("re"));
 gtk_signal_connect(GTK_OBJECT(components.tree_item_triggers), "collapse", GTK_SIGNAL_FUNC(select_tree), mkptr("rc"));
 gtk_tree_append(GTK_TREE(components.tree), components.tree_item_triggers);
 gtk_widget_show(components.tree_item_triggers);
 components.subtree_triggers = gtk_tree_new();
 gtk_signal_connect(GTK_OBJECT(components.subtree_triggers), "select_child",   GTK_SIGNAL_FUNC(select_child_tree), mkptr("rs"));
 gtk_signal_connect(GTK_OBJECT(components.subtree_triggers), "unselect_child", GTK_SIGNAL_FUNC(select_child_tree), mkptr("rd"));
 gtk_tree_set_selection_mode(GTK_TREE(components.subtree_triggers), GTK_SELECTION_SINGLE);
 gtk_tree_set_view_mode(GTK_TREE(components.subtree_triggers), GTK_TREE_VIEW_ITEM);
 subtree_DB_triggers();
 gtk_tree_item_set_subtree(GTK_TREE_ITEM(components.tree_item_triggers), components.subtree_triggers);
 gtk_widget_show(components.subtree_triggers);
 /* TRIGGERS ENDS */
 gtk_table_attach(GTK_TABLE(components.table), components.scrolled_win, 0, 20, 0, 80, 	/* CAN BE CHANGED */
		 GTK_FILL,GTK_FILL|GTK_EXPAND, 0, 0);
 gtk_widget_show(components.scrolled_win);
 gtk_box_pack_start (GTK_BOX (main_vbox), components.table, TRUE, TRUE, 0);
 gtk_widget_show(components.table);
}


void about_box(GtkWidget *w,gpointer data)
{
 debug("%s:%d about_box()",__FILE__,__LINE__);
 about_window_new();
}


void copy_database(GtkWidget* w, gpointer data)
{
 INT emerg;
 CHAR *dbname, dbs[MAXPATH];
 CHAR errstr[MAX_QUERY_LENGTH];
 CHAR db2[MAXPATH];
 CHAR winname[STDSTRING];
 CHAR*** ptr;
 INT row,col;
 debug("%s:%d copy_database()",__FILE__,__LINE__);
 if (pass)
   {
    gtk_dialog_printf("This option is currently not implemented when\n"
		    "password is used, beause cannot automatically pass\n"
		    "password to psql, try removing 'password' option from\n"
		    "pg_hba.conf and replacing it to 'trust', if You are\n"
		    "a DB admin, else You probably cannot create DB anyway\n");
    return;
   }
 emerg = !connected;
 if (!connected && !dummy_connect(1,TEMPLATE))
    {
     gtk_dialog_printf("Cannot connect to template DB\nCannot get DB list.");
     return ;
    }
 dbname = gtk_dialog_getdbs(connection, "Which DB copy?");
 if (emerg) { dummy_connect(0,TEMPLATE); connected=0; }
 strcpy(dbs, dbname);
 dbname = gtk_dialog_gettext_printf("Copy %s to:", dbs);
 strcpy(db2, dbname);
 if (!strcmp(db2,""))
      {
	gtk_dialog_printf("Cannot save %s to empty name.\n", dbs);
        return;
      }
 emerg = !connected;
 if (!connected && !dummy_connect(1,TEMPLATE))
   {
    gtk_dialog_printf("Cannot connect to template DB\nResult: Cannot create new DB");
    return ;
   }
 execute_printf(connection, &ptr, &row, &col, errstr, 0, "CREATE DATABASE %s", db2);
 if (emerg) { dummy_connect(0,TEMPLATE); connected = 0; }
 if (row<0 || col<0)
   {
    gtk_dialog_printf_big("Error executing query:\nReturn from DBMS: %s\n", errstr);
    return;
   }
 free_p3c(&ptr, row, col);
 system_printf("pg_dump -i %s | psql %s", dbs,db2);
 if (!emerg) return;
 connection = gtk_connect_db(db2, pass, errstr);
 if (!connection)
     {
      fatal_nodb(db2,errstr,0);
      return ;
     }
 else
     {
      connected=1;
      currentbase = (CHAR*)malloc(strlen(db2)+2);
      strcpy(currentbase, db2);
      sprintf(winname, "PgXexplorer: %s", currentbase);
      gtk_window_set_title(GTK_WINDOW(main_wnd), winname);
      setup_components();
     }
}


void create_DB_from_file(GtkWidget* w, gpointer data)
{
 CHAR *filen,*dbn;
 CHAR errstr[MAX_QUERY_LENGTH];
 CHAR dbname[MAXPATH];
 FILE* f;
 CHAR*** ptr;
 INT l,row,col,emerg;
 CHAR winname[STDSTRING];
 debug("%s:%d create_DB_from_file()",__FILE__,__LINE__);
 if (pass)
   {
    gtk_dialog_printf("This option is currently not implemented when\n"
		    "password is used, beause cannot automatically pass\n"
		    "password to psql, try removing 'password' option from\n"
		    "pg_hba.conf and replacing it to 'trust', if You are\n"
		    "a DB admin, else You probably cannot create DB anyway\n");
    return;
   }
 dbn = gtk_dialog_gettext_printf("Need DB name to fill from a SQL file:");
 if (!strcmp(dbn,""))
    {
     gtk_dialog_printf("Will not create empty name DB\nTry once again.");
     return;
    }
 strcpy(dbname,dbn);
 /*if (!strcmp(dbname,""))
   {
    gtk_dialog_printf("Cannot save DB to non-SQL file\n");
    return;
   }*/
 filen = gtk_dialog_getfile(".SQL", "Select SQL dump file (*.SQL)");
 if (!strcmp(filen,""))
   {
    gtk_dialog_printf("Cannot load DB from non-SQL file\n");
    return;
   }
 f = fopen(filen, "r");
 if (!f)
   {
    gtk_dialog_printf("Cannot read from\n%s", filen);
    return ;
   }
 fclose(f);
 emerg = !connected;
 if (!connected && !dummy_connect(1,TEMPLATE))
   {
    gtk_dialog_printf("Cannot connect to template DB\nResult: Cannot create new DB");
    return ;
   }
 execute_printf(connection, &ptr, &row, &col, errstr, 0, "CREATE DATABASE %s", dbname);
 if (emerg) { dummy_connect(0,TEMPLATE); connected = 0; }
 if (row<0 || col<0)
   {
    gtk_dialog_printf_big("Error executing query:\nReturn from DBMS: %s\n", errstr);
    return;
   }
 free_p3c(&ptr, row, col);
 system_printf("psql %s -f %s", dbname, filen);
 if (!connected) l = 1;
 else l = gtk_dialog_yes_no_printf("Switch from database\n%s to %s now?", currentbase, dbname);
 if (l)
  {
   if (!emerg)
     {
      disconnect();
      if (currentbase) free(currentbase);
      currentbase=NULL;
      sprintf(winname, "PgXexplorer: (no database opened currently)");
      gtk_window_set_title (GTK_WINDOW(main_wnd), winname);
     }
   connection = gtk_connect_db(dbname, pass, errstr);
   if (!connection)
     {
      fatal_nodb(dbname,errstr,0);
      return ;
     }
   else
     {
      connected=1;
      currentbase = (CHAR*)malloc(strlen(dbname)+2);
      strcpy(currentbase, dbname);
      sprintf(winname, "PgXexplorer: %s", currentbase);
      gtk_window_set_title (GTK_WINDOW(main_wnd), winname);
      setup_components();
     }
  }
}


void save_database(GtkWidget* w, gpointer data)
{
 INT a;
 CHAR* filename;
 CHAR* dbname;
 CHAR dbs[STDSTRING];
 INT emerg;
 debug("%s:%d save_database()",__FILE__,__LINE__);
 if (pass)
   {
    gtk_dialog_printf("This option is currently not implemented when\n"
		    "password is used, beause cannot automatically pass\n"
		    "password to psql, try removing 'password' option from\n"
		    "pg_hba.conf and replacing it to 'trust', if You are\n"
		    "a DB admin, else You probably cannot create DB anyway\n");
    return;
   }
 if (!connected) a=0;
 else
 a = gtk_dialog_with_2_buttons("Do You want to save Current database or Other database?", "Current", "Other");
 if (a)
   {
    /*filename = gtk_dialog_gettext_printf("Enter filename, %s will be saved to:\n(extension .SQL will be added automatically)", currentbase);*/
    filename = gtk_dialog_getfile_printf(".SQL", "Enter filename, %s will be saved to", currentbase);
    if (!strcmp(filename,""))
      {
	gtk_dialog_printf("Cannot save %s to non-SQL file", currentbase);
        return;
      }
    system_printf("pg_dump -i '%s' > '%s'", currentbase, filename);
   }
 else
 {
  emerg = !connected;
  if (!connected && !dummy_connect(1,TEMPLATE))
    {
     gtk_dialog_printf("Cannot connect to template DB\nCannot get DB list.");
     return ;
    }
  dbname = gtk_dialog_getdbs(connection, "Which DB save?");
  if (emerg) { dummy_connect(0,TEMPLATE); connected=0; }
  strcpy(dbs, dbname);  /* HEADERS WILL USE OLD POINTER IN DIALOGS, SO MUST COPY IT */
  			/* NOW VARIABLE dbs IS A COPY, AND dbname CAN (NOT MUST) */
  			/* BE CHANGED BY DIALOGS */
  filename = gtk_dialog_getfile_printf(".SQL", "Enter filename, %s will be saved to",dbs);
  if (!strcmp(filename,""))
      {
	gtk_dialog_printf("Cannot save %s to non-SQL file\n", dbs);
        return;
      }
  system_printf("pg_dump -i '%s' > '%s'", dbs, filename);
 }
}

CHAR* get_env(CHAR*);

CHAR* dbenv()
{
 debug("%s:%d dbenv()",__FILE__,__LINE__);
 sprintf(tmpstr,"PGDATA=\"%s\"\nPGPORT=\"%s\"\nPGHOST=\"%s\"", get_env("PGDATA"), get_env("PGPORT"), get_env("PGHOST"));
 return tmpstr;
}


void fatal_nodb(CHAR* dbname, CHAR* errstr, INT killed)
{
 debug("%s:%d fatal_nodb(%s,%s,%d)",__FILE__,__LINE__,dbname?dbname:"null", errstr?errstr:"null", killed);
 if (killed)
 gtk_dialog_printf_big_with_title("PgXexplorer Fatal Error",
 	"Cannot connect to DB: %s\nDBMS returned: \n%s\nSystem will now exit.\nENV=\n%s",
  	dbname,errstr,dbenv());
 else
 gtk_dialog_printf_big_with_title("PgXexplorer Error",
 	"Cannot connect to DB: %s\nDBMS returned: \n%s\nENV=\n%s",
  	dbname,errstr,dbenv());
 if(killed) exit(1);
}


void reload_bases(CHAR* db, INT ask)
{
 INT a;
 CHAR errstr[MAX_QUERY_LENGTH];
 CHAR winname[STDSTRING];
 CHAR dbname[STDSTRING];
 debug("%s:%d reload_bases(%s,%d)",__FILE__,__LINE__,db,ask);
 strcpy(dbname,db);
 if (!strcmp(dbname,""))
   {
    gtk_dialog_printf("Cannot switch to an empty DB");
    return ;
   }
 if (ask) a = gtk_dialog_yes_no_printf("Switch to database \"%s\" now?", db);
 else a=1;
 /**/
 if (!a) return;
 disconnect();
 if (currentbase) free(currentbase);
 currentbase=NULL;
 /*while (!strcmp(dbname,""))
   {
    gtk_dialog_printf("Cannot switch to an empty DB");
    strcpy(dbname, gtk_dialog_gettext("Null DB name cannot be accepted\nEnter database name: "));
   }*/
 sprintf(winname, "PgXexplorer: (no database opened currently)");
 gtk_window_set_title (GTK_WINDOW(main_wnd), winname);
 connection = gtk_connect_db(dbname, pass, errstr);
 if (!connection)
   {
    fatal_nodb(dbname,errstr,0);
    return ;
   }
 else
   {
    connected=1;
    currentbase = (CHAR*)malloc(strlen(dbname)+2);
    strcpy(currentbase, dbname);
    sprintf(winname, "PgXexplorer: %s", currentbase);
    gtk_window_set_title (GTK_WINDOW(main_wnd), winname);
    setup_components();
   }
}


void wizards(GtkWidget* w, gpointer data)
{
 debug("%s:%d wizards()",__FILE__,__LINE__);
 GtkWidget* window, *vbox, *button1, *button2, *button3, *button4, *button5, *buttone;
 window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
 gtk_widget_set_usize(GTK_WIDGET(window), 185, 200);
 vbox = gtk_vbox_new(FALSE, 1);
 gtk_container_set_border_width(GTK_CONTAINER(window), 5);
 gtk_widget_show(vbox);
 button1 = gtk_button_new_with_label("Create table");
 gtk_box_pack_start(GTK_BOX(vbox), button1, TRUE, TRUE, 0);
 gtk_widget_show(button1);
 gtk_signal_connect(GTK_OBJECT(button1), "clicked", GTK_SIGNAL_FUNC(create_table), NULL);
 button2 = gtk_button_new_with_label("Drop table");
 gtk_box_pack_start(GTK_BOX(vbox), button2, TRUE, TRUE, 0);
 gtk_widget_show(button2);
 gtk_signal_connect(GTK_OBJECT(button2), "clicked", GTK_SIGNAL_FUNC(tdrop_table), NULL);
 button3 = gtk_button_new_with_label("Drop view");
 gtk_box_pack_start(GTK_BOX(vbox), button3, TRUE, TRUE, 0);
 gtk_widget_show(button3);
 gtk_signal_connect(GTK_OBJECT(button3), "clicked", GTK_SIGNAL_FUNC(tdrop_view), NULL);
 button4 = gtk_button_new_with_label("Insert");
 gtk_box_pack_start(GTK_BOX(vbox), button4, TRUE, TRUE, 0);
 gtk_widget_show(button4);
 gtk_signal_connect(GTK_OBJECT(button4), "clicked", GTK_SIGNAL_FUNC(tinsert), NULL);
 button5 = gtk_button_new_with_label("Delete");
 gtk_box_pack_start(GTK_BOX(vbox), button5, TRUE, TRUE, 0);
 gtk_widget_show(button5);
 gtk_signal_connect(GTK_OBJECT(button5), "clicked", GTK_SIGNAL_FUNC(tdelete), NULL);
 buttone = gtk_button_new_with_label("Close");
 gtk_box_pack_start(GTK_BOX(vbox), buttone, TRUE, TRUE, 0);
 gtk_widget_show(buttone);
 gtk_signal_connect(GTK_OBJECT(buttone), "clicked", GTK_SIGNAL_FUNC(gtk_main_quit), NULL);
 gtk_container_add(GTK_CONTAINER(window), vbox);
 gtk_widget_show(vbox);
 gtk_signal_connect(GTK_OBJECT(window), "delete_event", GTK_SIGNAL_FUNC(gtk_main_quit), NULL);
 gtk_window_set_title(GTK_WINDOW(window), "Select wizard...");
 gtk_window_set_modal(GTK_WINDOW(window), TRUE);
 gtk_widget_show(window);
 gtk_main();
 gtk_widget_destroy(window);
}


void load_database(GtkWidget* w, gpointer data)
{
 CHAR* dbname;
 CHAR winname[STDSTRING];
 CHAR errstr[MAX_QUERY_LENGTH];
 INT emerg;
 debug("%s:%d load_database()",__FILE__,__LINE__);
 emerg = !connected;
 if (!connected && !dummy_connect(1,TEMPLATE))
   {
    gtk_dialog_printf("Cannot connect to template DB\nServer is dead?");
    return;
   }
 dbname = gtk_dialog_getdbs(connection, "Which DB load?");
 if (emerg) { dummy_connect(0,TEMPLATE); connected = 0; }
 if (!strcmp(dbname,""))
    {
     gtk_dialog_printf("Will not load empty DB name");
     return;
    }
 if (!emerg && connected && !strcmp(dbname,currentbase))
    {
     gtk_dialog_printf("This DB: %s, You are currently using", currentbase);
     return ;
    }
 if (!emerg)
   {
    disconnect();
    if (currentbase) free(currentbase);
    currentbase=NULL;
    sprintf(winname, "PgXexplorer: (no database opened currently)");
    gtk_window_set_title (GTK_WINDOW(main_wnd), winname);
   }
 connection = gtk_connect_db(dbname, pass, errstr);
 if (!connection)
     {
      fatal_nodb(dbname,errstr,0);
      return ;
     }
   else
     {
      connected=1;
      currentbase = (CHAR*)malloc(strlen(dbname)+2);
      strcpy(currentbase, dbname);
      sprintf(winname, "PgXexplorer: %s", currentbase);
      gtk_window_set_title (GTK_WINDOW(main_wnd), winname);
      setup_components();
     }
}


void drop_database(GtkWidget* w, gpointer data)
{
 CHAR* dbname;
 CHAR errstr[MAX_QUERY_LENGTH];
 PGresult *result;
 INT emerg;
 debug("%s:%d drop_database()",__FILE__,__LINE__);
 emerg = !connection;
 if (!connected && !dummy_connect(1,TEMPLATE))
   {
     gtk_dialog_printf("Cannot connect to template DB\nCannot get DB list");
     return;
   }
 dbname = gtk_dialog_getdbs(connection, "Which DB drop?");
 if (!strcmp(dbname,""))
    {
     gtk_dialog_printf("Will not drop empty DB name\nTry once again.");
     if (emerg) { dummy_connect(0,TEMPLATE); connected = 0; }
     return;
    }
 if (!emerg && connected && !strcmp(dbname,currentbase))
    {
     gtk_dialog_printf("Cannot drop DB: %s, currently used!", currentbase);
     return ;
    }
 if (strstr(dbname, "template"))
   {
    if (emerg)
      {
       dummy_connect(0,TEMPLATE);
       connected = 0;
       gtk_dialog_printf("Cannot drop template\nwhile it is beeing used\nas emergency DB!");
       return ;
      }
    if (!gtk_dialog_yes_no_printf(
    "Are You sure want to drop\ntemplate DB: %s\nThis can prevent PgXexplorer working properly",dbname))
	    return ;
   }
 execute_printf_query(connection, &result, errstr,  "DROP DATABASE %s", dbname);
 if (emerg) { dummy_connect(0,TEMPLATE); connected = 0; }
 if (!result)
   {
    gtk_dialog_printf_big("Error executing query:\nReturn from DBMS: %s\n", errstr);
    return;
   }
}


void create_database(GtkWidget* w, gpointer data)
{
 CHAR* dbname;
 CHAR errstr[MAX_QUERY_LENGTH];
 CHAR winname[STDSTRING];
 INT a;
 INT emerg;
 PGresult *result;
 debug("%s:%d create_database()",__FILE__,__LINE__);
 emerg=!connected;
 dbname = gtk_dialog_gettext("Enter database name: ");
 if (!strcmp(dbname,""))
    {
     gtk_dialog_printf("Will not create empty name DB\nTry once again.");
     return;
    }
 result = NULL;
 if (!connected && !dummy_connect(1,TEMPLATE))
   {
    gtk_dialog_printf("Cannot connect to template DB\nResult: Cannot create new DB");
    return ;
   }
 execute_printf_query(connection, &result, errstr,  "CREATE DATABASE %s", dbname);
 if (emerg) { dummy_connect(0,TEMPLATE); connected = 0; }
 if (!result)
   {
    gtk_dialog_printf_big("Error executing query:\nReturn from DBMS: %s\n", errstr);
    return;
   }
 if (!connected) a = 1;
 else a = gtk_dialog_yes_no_printf("Switch from database\n%s to %s now?", currentbase, dbname);
 if (a)
  {
   if (!emerg)
     {
      disconnect();
      if (currentbase) free(currentbase);
      currentbase=NULL;
      sprintf(winname, "PgXexplorer: (no database opened currently)");
      gtk_window_set_title (GTK_WINDOW(main_wnd), winname);
     }
   connection = gtk_connect_db(dbname, pass, errstr);
   if (!connection)
     {
      fatal_nodb(dbname,errstr,0);
      return ;
     }
   else
     {
      connected=1;
      currentbase = (CHAR*)malloc(strlen(dbname)+2);
      strcpy(currentbase, dbname);
      sprintf(winname, "PgXexplorer: %s", currentbase);
      gtk_window_set_title (GTK_WINDOW(main_wnd), winname);
      setup_components();
     }
  }
}


void exec_test_prog(GtkWidget* w, gpointer data)
{
 debug("%s:%d exec_test_prog()",__FILE__,__LINE__);
 system("./pgxtest");
}


void save_results_mainwnd(GtkWidget* w, gpointer d)
{
 CHAR ***data, *filen;
 INT a,i,j,rows,cols;
 FILE* f;
 INT want_meta;
 INT start;
 debug("%s:%d save_results_mainwnd()",__FILE__,__LINE__);
 data = components.data;
 rows = components.arows;
 cols = components.acols;
 filen = gtk_dialog_getfile(".HTML", "Save Results to: (*.HTML)");
 if (!strcmp(filen,""))
   {
    gtk_dialog_printf("Cannot save results to non-HTML file\n");
    return;
   }
 f = fopen(filen,"r");
 if (f)
   {
    fclose(f);
    a = gtk_dialog_yes_no_printf("Already exists:\n%s\nOverwrite?", filen);
    if (!a) return;
   }
 f = fopen(filen,"w");
 if (!f)
   {
    gtk_dialog_printf("Cannot write to %s", filen);
    return;
   }
 start=0;
 want_meta = gtk_dialog_yes_no_printf("Do You want to save table names?");
 if (!want_meta) start=1;
 fprintf(f,"<HTML>\n");
 fprintf(f,"<TITLE>\n");
 fprintf(f,"Results\n");
 fprintf(f,"</TITLE>\n");
 fprintf(f,"<HEAD>\n");
 fprintf(f,"</HEAD>\n");
 fprintf(f,"<BODY>\n");
 fprintf(f,"<TABLE BORDER=\"3\">\n");
 for (i=start;i<rows;i++)
   {
    fprintf(f,"<TR>\n");
    for (j=0;j<cols;j++) fprintf(f,"<TD>%s</TD>\n", data[i][j]);
   }
 fprintf(f,"</BODY>\n");
 fprintf(f,"</HTML>\n");
 fclose(f);
}


void save_results(GtkWidget* w, gpointer resd)		/* SAVES ALL NOT TRUNCATED ROWS, SO OK DON'T NEED START_ROW */
{
 CHAR ***data, *filen;
 struct result_cache* resc;
 INT rows,cols,a,i,j;
 FILE* f;
 debug("%s:%d save_results()",__FILE__,__LINE__);
 if (!resd) { error("resd points NULL %s:%d",__FILE__,__LINE__); return; }
 resc = resd;
 data = resc->data;
 rows = resc->rows;
 cols = resc->cols;
 filen = gtk_dialog_getfile(".HTML", "Save Results to: (*.HTML)");
 if (!strcmp(filen,""))
   {
    gtk_dialog_printf("Cannot save results to non-HTML file\n");
    return;
   }
 f = fopen(filen,"r");
 if (f)
   {
    fclose(f);
    a = gtk_dialog_yes_no_printf("Already exists:\n%s\nOverwrite?", filen);
    if (!a) return;
   }
 f = fopen(filen,"w");
 if (!f)
   {
    gtk_dialog_printf("Cannot write to %s", filen);
    return;
   }
 fprintf(f,"<HTML>\n");
 fprintf(f,"<TITLE>\n");
 fprintf(f,"Results\n");
 fprintf(f,"</TITLE>\n");
 fprintf(f,"<HEAD>\n");
 fprintf(f,"</HEAD>\n");
 fprintf(f,"<BODY>\n");
 fprintf(f,"<TABLE BORDER=\"3\">\n");
 for (i=0;i<rows;i++)
   {
    fprintf(f,"<TR>\n");
    for (j=0;j<cols;j++) fprintf(f,"<TD>%s</TD>\n", data[i][j]);
   }
 fprintf(f,"</BODY>\n");
 fprintf(f,"</HTML>\n");
 fclose(f);
}


void gtk_table_out(CHAR* title, CHAR*** str_data, INT a, INT b)
{
 GtkWidget* window, *table, *button, *scrolled_window, *btn_sav;
 GtkWidget*** entry;
 struct result_cache res_cache;
 INT i,j,rrow,rcol;
 debug("%s:%d gtk_table_out(%s,%p,%d,%d)",__FILE__,__LINE__,title?title:"null",str_data,a,b);
 if (!str_data) { gtk_dialog_printf("Error, table of results to display is equal NULL"); return; }
 res_cache.data = str_data;
 res_cache.rows = a;
 res_cache.cols = b;
 rrow=rcol=0;
 if (b>32 || a>512)
   {
    gtk_dialog_printf("Result is too big, rows=%d, columns=%d\nwill be truncated!", a, b);
    if (a>512) b = (b>12 )?12 :b;
    else if (a>256)  b = (b>24 )?24 :b;
    else if (a>128)  b = (b>48 )?48 :b;
    else             b = (b>64 )?64:b;
    a = (a>512)?512:a;
    if (a<res_cache.rows) rrow = gtk_dialog_getnum(0, res_cache.rows-a,"Wchich rows display? (start from)");
    if (b<res_cache.cols) rcol = gtk_dialog_getnum(0, res_cache.cols-b,"Wchich columns display? (start from)");
   }
 create_pbar("Wait for query result...");
 entry = (GtkWidget***)malloc(a*sizeof(GtkWidget**));
 for (i=0;i<a;i++) entry[i] = (GtkWidget**)malloc(b*sizeof(GtkWidget*));
 window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
 gtk_window_set_modal(GTK_WINDOW(window), TRUE);
 gtk_widget_set_usize (GTK_WIDGET(window), 600, 600);
 debug("a=%d, b=%d\n",a,b);
 table = gtk_table_new (a+2, b, FALSE);
 for (i=0;i<a;i++)
 for (j=0;j<b;j++)
   {
    entry[i][j] = gtk_entry_new();
    if (i) gtk_entry_set_text(GTK_ENTRY(entry[i][j]), str_data[i+rrow][j+rcol]);
    else   gtk_entry_set_text(GTK_ENTRY(entry[i][j]), str_data[0][j+rcol]);
    gtk_entry_set_editable(GTK_ENTRY(entry[i][j]), FALSE);
    gtk_table_attach (GTK_TABLE (table), entry[i][j], j, j+1, i+2, i+3,GTK_FILL,GTK_FILL, 0, 0);
    gtk_widget_show(entry[i][j]);
    if (!cont) { gtk_dialog_printf("interrupted!"); goto out_side; }
   }
 out_side:
 button = gtk_button_new_with_label("Close this window");
 gtk_table_attach (GTK_TABLE (table), button, 0, (b>3)?4:(b+1), 0, 1,GTK_FILL,GTK_FILL, 0, 0);
 gtk_widget_show(button);
 btn_sav = gtk_button_new_with_label("Save results");
 gtk_table_attach (GTK_TABLE (table), btn_sav, 0, (b>3)?4:(b+1), 1, 2,GTK_FILL,GTK_FILL, 0, 0);
 gtk_widget_show(btn_sav);
 gtk_widget_show (table);
 scrolled_window = gtk_scrolled_window_new(NULL,NULL);
 gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
 gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window), table);
 gtk_container_set_border_width(GTK_CONTAINER(window), 2);
 gtk_container_set_border_width(GTK_CONTAINER(scrolled_window), 2);
 gtk_signal_connect(GTK_OBJECT(window), "destroy", GTK_SIGNAL_FUNC(gtk_main_quit), NULL);
 gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(close_results), (gpointer)(window));
 gtk_signal_connect(GTK_OBJECT(btn_sav),"clicked", GTK_SIGNAL_FUNC(save_results),  (gpointer)(&res_cache));
 gtk_widget_show(scrolled_window);
 gtk_container_add (GTK_CONTAINER(window), scrolled_window);
 gtk_window_set_title(GTK_WINDOW(window), title);
 gtk_widget_show(window);
 destroy_pbar();
 gtk_main();
 for (i=0;i<a;i++) free(entry[i]);
 free(entry);
}

/*                    TEST QUERY EXEC FUNC

void test_queries()
{
 CHAR*** ptr;
 CHAR errstr[MAX_QUERY_LENGTH];
 INT a,b;
 ptr=p3c_execute_printf(connection,&a,&b,errstr, "SELECT * FROM %s", "ocena");
 if (a<0 || b<0)
 {
  gtk_dialog_printf("%s", errstr);
  return ;
 }
 free_p3c(&ptr,a,b);
}

*/

void clear_txt_ctrl(GtkWidget* w)
{
 INT l;
 debug("%s:%d clear_txt_ctrl()",__FILE__,__LINE__);
 l = gtk_text_get_length(GTK_TEXT(w));
 gtk_text_set_point(GTK_TEXT(w), 0);
 gtk_text_forward_delete(GTK_TEXT(w), l);
}


void load_proc(GtkWidget* w, gpointer txt)
{
 CHAR* filen;
 FILE* f;
 INT l,max;
 CHAR* str;
 debug("%s:%d load_proc()",__FILE__,__LINE__);
 if (!connected)
   {
    gtk_dialog_printf("Connect to DB first!");
    return;
   }
 filen = gtk_dialog_getfile(".QUE", "Select query file (*.QUE)");
 if (!strcmp(filen,""))
   {
    gtk_dialog_printf("Cannot load query from non-QUE file\n");
    return;
   }
 f = fopen(filen,"r");
 if (!f)
   {
    gtk_dialog_printf("Cannot read from: %s\n", filen);
    return ;
   }
 clear_txt_ctrl(txt);
 fseek(f,0,SEEK_END);
 l=ftell(f);
 fseek(f,0,SEEK_SET);
 if (l>=MAX_QUERY_LENGTH-100)
   {
    gtk_dialog_printf("File is too long!\nTruncating...");
    l = MAX_QUERY_LENGTH-100;
   }
 str = malloc(l+2);
 max=l;
 l=0;
 while (l<=max && (str[l++]=fgetc(f))!=EOF) ;
 str[--l]=0;
 gtk_text_insert(GTK_TEXT(txt), NULL,NULL,NULL,str,l);
 fclose(f);
 free(str);
}


void save_proc(GtkWidget* w, gpointer txt)	/* if w == NULL then silent */
{
 CHAR *t,*filen;
 CHAR filename[MAXPATH];
 FILE* f;
 INT l,i;
 debug("%s:%d save_proc()",__FILE__,__LINE__);
 l = gtk_text_get_length(GTK_TEXT(txt));
 if (l<3)
    {
     gtk_dialog_printf("Will not save empty query.");
     return;
    }
 t = malloc(l+2);
 for (i=0;i<l;i++) t[i] = GTK_TEXT_INDEX(GTK_TEXT(txt), i);
 t[i] = 0;
 filen = gtk_dialog_getfile(".QUE", "Enter filename to save query");
 if (!strcmp(filen,""))
   {
    gtk_dialog_printf("Cannot save query to non-QUE file\n");
    free(t);
    return;
   }
 strcpy(filename,filen);
 f = fopen(filename,"r");
 if (f)
   {
    fclose(f);
    if (!gtk_dialog_yes_no_printf("Overwite file: %s?", filename))
      {
       free(t);
       return ;
      }
   }
 f = fopen(filename,"w");
 if (!f)
   {
    gtk_dialog_printf("Cannot write to: %s\n", filename);
    free(t);
    return ;
   }
 fprintf(f,"%s", t);
 free(t);
 fclose(f);
 if (w) gtk_dialog_printf("Saved to %s\n", filename);
}


INT create_query_from_fname(CHAR* fname, GtkWidget* txt)
{
 FILE* f;
 INT l,max;
 CHAR* str;
 debug("%s:%d create_query_from_fname(%s)",__FILE__,__LINE__,fname);
 f = fopen_printf("r", "%s.que", fname); if (f) goto file_got;
 f = fopen_printf("r", "%s.Que", fname); if (f) goto file_got;
 f = fopen_printf("r", "%s.qUe", fname); if (f) goto file_got;
 f = fopen_printf("r", "%s.quE", fname); if (f) goto file_got;
 f = fopen_printf("r", "%s.QUe", fname); if (f) goto file_got;
 f = fopen_printf("r", "%s.QuE", fname); if (f) goto file_got;
 f = fopen_printf("r", "%s.qUE", fname); if (f) goto file_got;
 f = fopen_printf("r", "%s.QUE", fname); if (f) goto file_got;
 else return 0;
 file_got:
 fseek(f,0,SEEK_END);
 l=ftell(f);
 fseek(f,0,SEEK_SET);
 if (l>=MAX_QUERY_LENGTH-100)
   {
    gtk_dialog_printf("File is too long!\nTruncating...");
    l = MAX_QUERY_LENGTH-100;
   }
 str = malloc(l+2);
 max=l;
 l=0;
 while (l<=max && (str[l++]=fgetc(f))!=EOF);
 str[--l]=0;
 gtk_text_insert(GTK_TEXT(txt), NULL,NULL,NULL,str,l);
 fclose(f);
 free(str);
 return 1;
}


void tdelete(GtkWidget* w, gpointer opt)
{
  debug("%s:%d tdrop_table()",__FILE__,__LINE__);
  CHAR* tab;
  tab = select_table_wizard(0);
  if (!tab)
  {
	gtk_dialog_printf("Select table wizard returned NULL query!");
	return ;
  }
 else
   {
    CHAR* query;
    CHAR ***ptr;
    INT row,col;
    CHAR errstr[MAX_QUERY_LENGTH];
#ifdef SMALLOC
    struct dmem_list* mem;
    mem = NULL;
#endif
    query = delete_wizard(tab);
    if (!query)
       {
	gtk_dialog_printf("Delete wizard returned NULL query!");
	return ;
       }
    debug("QUERY = %s\n", query);
#ifdef SMALLOC
    dmem_execute_printf(&mem, connection, &ptr, &row, &col, errstr, 0, HERE,query);
#else
    execute_printf(connection, &ptr, &row, &col, errstr, 0, query);
#endif
    if (row < 0 || col < 0)
      {
       gtk_dialog_printf_big("Insert error:\nReturn from DBMS: %s\n", errstr);
       free(query);
       return;
      }
    destruct_tab_view(NULL,NULL);
    free(query);
#ifdef SMALLOC
    dmem_free_p3c(&mem, &ptr, row, col, HERE);
    dshred_special_memlist(&mem, HERE);
#else
    free_p3c(&ptr, row, col);
#endif
   }
 free(tab);
}


void tinsert(GtkWidget* w, gpointer opt)
{
  debug("%s:%d tdrop_table()",__FILE__,__LINE__);
  CHAR* tab;
  tab = select_table_wizard(0);
  if (!tab)
  {
	gtk_dialog_printf("Select table wizard returned NULL query!");
	return ;
  }
 else
   {
    CHAR* query;
    CHAR ***ptr;
    INT row,col;
    CHAR errstr[MAX_QUERY_LENGTH];
#ifdef SMALLOC
    struct dmem_list* mem;
    mem = NULL;
#endif
    query = insert_wizard(tab);
    if (!query)
       {
	gtk_dialog_printf("Insert wizard returned NULL query!");
	return ;
       }
    debug("QUERY = %s\n", query);
#ifdef SMALLOC
    dmem_execute_printf(&mem, connection, &ptr, &row, &col, errstr, 0, HERE,query);
#else
    execute_printf(connection, &ptr, &row, &col, errstr, 0, query);
#endif
    if (row < 0 || col < 0)
      {
       gtk_dialog_printf_big("Insert error:\nReturn from DBMS: %s\n", errstr);
       free(query);
       return;
      }
    destruct_tab_view(NULL,NULL);
    free(query);
#ifdef SMALLOC
    dmem_free_p3c(&mem, &ptr, row, col, HERE);
    dshred_special_memlist(&mem, HERE);
#else
    free_p3c(&ptr, row, col);
#endif
   }
 free(tab);
}


void tdrop_table(GtkWidget* w, gpointer opt)
{
  debug("%s:%d tdrop_table()",__FILE__,__LINE__);
  CHAR* tab;
  tab = select_table_wizard(1);
  if (!tab)
  {
	gtk_dialog_printf("Select table wizard returned NULL query!");
	return ;
  }
 if (!strcmp(tab,"*all*"))
 {
	 if (gtk_dialog_yes_no("Are you sure want to drop all tables?")) drop_tables();
 }
 else drop_table(tab, 0);
 free(tab);
}


void tdrop_view(GtkWidget* w, gpointer opt)
{
  debug("%s:%d tdrop_table()",__FILE__,__LINE__);
  CHAR* tab;
  tab = select_view_wizard();
  if (!tab)
  {
	gtk_dialog_printf("Select view wizard returned NULL query!");
	return ;
  }
 if (!strcmp(tab,"*all*"))
 {
	 if (gtk_dialog_yes_no("Are you sure want to drop all views?")) drop_views();
 }
 else drop_view(tab, 0);
 free(tab);
}


void create_table(GtkWidget* w, gpointer opt)
{
 debug("%s:%d create_table()",__FILE__,__LINE__);
  CHAR* query;
  CHAR ***ptr;
  INT row,col;
  CHAR errstr[MAX_QUERY_LENGTH*24];
#ifdef SMALLOC
  struct dmem_list* mem;
  mem = NULL;
#endif
  query = NULL;
  query = ctable_wizard();
  if (!query)
  {
	gtk_dialog_printf("CreateTable returned NULL query!");
	return ;
  }
  debug("QUERY = %s\n", query);
#ifdef SMALLOC
 dmem_execute_printf(&mem, connection, &ptr, &row, &col, errstr, 0, HERE, query);
#else
 execute_printf(connection, &ptr, &row, &col, errstr, 0, query);
#endif
  if (row < 0 || col < 0)
  {
    gtk_dialog_printf_big("Create Table error:\nReturn from DBMS: %s\n", errstr);
    free(query);
    query = NULL;
    return;
  }
  free(query);
  query = NULL;
  clean_vars();
#ifdef SMALLOC
 dmem_free_p3c(&mem, &ptr, row, col, HERE);
 dshred_memlist(HERE);			// FIXME especially here added in v0.27
#else
 free_p3c(&ptr, row, col);
#endif
 refresh_all();		 		/* FIXME HARDCORE !!! */
 destroy_pbar();
}


void execute_fast_one(GtkWidget* w, gpointer opt)
{
 debug("%s:%d execute_fats_one()",__FILE__,__LINE__);
 execute_fast(NULL,(gpointer)(1));
}


void execute_fast_two(GtkWidget* w, gpointer opt)
{
 debug("%s:%d execute_fat_two()",__FILE__,__LINE__);
 execute_fast(NULL,(gpointer)(2));
}


void execute_fast_thr(GtkWidget* w, gpointer opt)
{
 debug("%s:%d execute_fast_thr()",__FILE__,__LINE__);
 execute_fast(NULL,(gpointer)(3));
}


void execute_fast(GtkWidget* w, gpointer opt)
{
 GtkWidget* text;
 CHAR* filen;
 debug("%s:%d execute_fast()",__FILE__,__LINE__);
#ifdef _MACHINE_32
 debug("Execute fast! %d\n",(INT)(opt));
#else
 debug("Execute fast! %lld\n",(INT64)(opt));
#endif
 if (!connected) { gtk_dialog_printf("Connect to DB first!"); return; }
 text = gtk_text_new (NULL, NULL);
#ifdef _MACHINE_32
 switch ((INT)(opt))
#else
 switch ((INT64)(opt))
#endif
 {
  case 1:
    load_proc(NULL, (gpointer)(text));
    execute_proc(NULL, (gpointer)(text));
    break;
  case 2:	/*WriteFname*/
    filen = gtk_dialog_gettext("Enter filename (without extension): ");
    if (!strcmp(filen,"")) { gtk_dialog_printf("Empty filename"); return; }
    if (!create_query_from_fname(filen, text)) { gtk_dialog_printf("Cannot load query from file\n%s\nTried with various masks", filen); return; }
    execute_proc(NULL, (gpointer)(text));
    break;
  case 3:	/*SQL cmd*/
    filen = gtk_dialog_gettext_big("Enter short SQL Query:");
    if (!strcmp(filen,"")) { gtk_dialog_printf("Will not execute empty query!"); return; }
    gtk_text_insert(GTK_TEXT(text), NULL,NULL,NULL,filen,strlen(filen));
    execute_proc(NULL, (gpointer)(text));
    break;
  default: error("bad opt value, %s:%d",__FILE__,__LINE__); return;
 }
}


void execute_proc(GtkWidget* w, gpointer txt)
{
 CHAR* t;
 INT l,i,a,b;
 PGresult *result;
 CHAR errstr[MAX_QUERY_LENGTH+0x100];
 CHAR*** out;
 debug("%s:%d execute_proc()",__FILE__,__LINE__);
 if (!connected)
   {
    gtk_dialog_printf("Connect to DB first!");
    return;
   }
 l = gtk_text_get_length(GTK_TEXT(txt));
 if (l>=MAX_QUERY_LENGTH-100)
   {
    gtk_dialog_printf("Query too long!\n");
    return ;
   }
 if (l<3)
    {
     gtk_dialog_printf("Will not execute empty query\nTry once again.");
     return;
    }
 t = malloc(l+2);
 for (i=0;i<l;i++) t[i] = GTK_TEXT_INDEX(GTK_TEXT(txt), i);
 t[i] = 0;
 result = NULL;
 execute_printf_query(connection, &result, errstr, t);
 free(t);
 if (!result)
   {
    gtk_dialog_printf_big("Error executing query:\nReturn from DBMS: %s\n", errstr);
    return;
   }
#ifdef SMALLOC
 out = dmpgres2pc3(result, &a, &b, 1, HERE);
#else
 out = mpgres2pc3(result, &a, &b, 1);
#endif
 clear_result(&result);
 if (a>0 && b>0) gtk_table_out("Results of Query:", out, a, b);
 else gtk_dialog_printf("Query was OK, but didn't returned any result\nThis probably was INSERT OR CREATE...");
 create_pbar("freeing memory...");
#ifdef SMALLOC
 dmfree_p3c(&out, a, b, HERE);
#else
 mfree_p3c(&out, a, b);
#endif
 shred_memlist();			/* FIXME - will not destroy other important structs ?? */
 //dshred_memlist(HERE);
 refresh_all();		 		/* FIXME HARDCORE !!! */
 destroy_pbar();
}


void update_entry(GtkWidget* w, gpointer d)
{
 debug("%s:%d update_entry()",__FILE__,__LINE__);
 strcpy((CHAR*)d,gtk_entry_get_text(GTK_ENTRY(w)));
}


void apply_proc(GtkWidget* w, gpointer d)
{
 CHAR** ptr;
 debug("%s:%d apply_proc()",__FILE__,__LINE__);
 ptr = (CHAR**)d;
 if (setenv("PGPORT",ptr[0],1)==-1) gtk_dialog_printf("Cannot set environment variable: PGPORT");
 if (setenv("PGDATA",ptr[1],1)==-1) gtk_dialog_printf("Cannot set environment variable: PGDATA");
 if (setenv("PGHOST",ptr[2],1)==-1) gtk_dialog_printf("Cannot set environment variable: PGHOST");
 reload_bases(ptr[3], 1);
}


INT connect_proc(GtkWidget* w, gpointer d)
{
 CHAR** ptr;
 debug("%s:%d connect_proc()",__FILE__,__LINE__);
 ptr = (CHAR**)d;
 if (setenv("PGPORT",ptr[0],1)==-1) gtk_dialog_printf("Cannot set environment variable: PGPORT");
 if (setenv("PGDATA",ptr[1],1)==-1) gtk_dialog_printf("Cannot set environment variable: PGDATA");
 if (setenv("PGHOST",ptr[2],1)==-1) gtk_dialog_printf("Cannot set environment variable: PGHOST");
 reload_bases(ptr[3], 0);
 gtk_main_quit();
 return TRUE;
}


INT delete_proc(GtkWidget* w, gpointer d)
{
 debug("%s:%d delete_proc()",__FILE__,__LINE__);
 gtk_main_quit();
 /*destroy_widget((GtkWidget*)d);*/
 return TRUE;
}


CHAR* get_env(CHAR* str)
{
 CHAR* ret;
 debug("%s:%d get_env(%s)",__FILE__,__LINE__, str?str:"null");
 ret = getenv(str);
 if (ret) return ret;
 else return "";
}


void env_dialog(GtkWidget* www, gpointer d)
{
 GtkWidget* dialog, *label[4], *button[3], *entry[4], *table;
 CHAR **t_entry;
 INT i;
 debug("%s:%d env_dialog()",__FILE__,__LINE__);
 t_entry = (CHAR**)malloc(4*sizeof(CHAR*));
 for (i=0;i<4;i++) t_entry[i] = (CHAR*)malloc(STDSTRING+1);
 for (i=0;i<4;i++)strcpy(t_entry[i], "");
 dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
 if (!d) gtk_window_set_title(GTK_WINDOW(dialog), "Environment settings");
 else gtk_window_set_title(GTK_WINDOW(dialog), "Write at least database name, empty fields means default");
 gtk_widget_set_usize(dialog, 500, 160);
 gtk_window_set_modal(GTK_WINDOW(dialog), 1);
 label[0] = gtk_label_new("Port:");
 label[1] = gtk_label_new("DataDir:");
 label[2] = gtk_label_new("Host:");
 label[3] = gtk_label_new("DataBase:");
 for (i=0;i<4;i++)
  {
   entry[i] = gtk_entry_new();
   gtk_entry_set_max_length(GTK_ENTRY(entry[i]), STDSTRING-1);
   gtk_entry_set_editable(GTK_ENTRY(entry[0]), TRUE);
   gtk_signal_connect(GTK_OBJECT(entry[i]), "activate", GTK_SIGNAL_FUNC(update_entry), (gpointer)t_entry[i]);
   gtk_signal_connect(GTK_OBJECT(entry[i]), "changed",  GTK_SIGNAL_FUNC(update_entry), (gpointer)t_entry[i]);
  }
 table = gtk_table_new (5, 3, TRUE);
 gtk_entry_set_text(GTK_ENTRY(entry[0]), get_env("PGPORT"));
 gtk_entry_set_text(GTK_ENTRY(entry[1]), get_env("PGDATA"));
 gtk_entry_set_text(GTK_ENTRY(entry[2]), get_env("PGHOST"));
 gtk_entry_set_text(GTK_ENTRY(entry[3]), currentbase?currentbase:"");
 for (i=0;i<4;i++)
   {
    gtk_table_attach (GTK_TABLE (table),label[i], 0, 1, i, i+1,GTK_FILL,GTK_FILL, 0, 0);
    gtk_table_attach (GTK_TABLE (table),entry[i], 1, 3, i, i+1,GTK_FILL|GTK_EXPAND,GTK_FILL, 0, 0);
    gtk_widget_show(entry[i]);
    gtk_widget_show(label[i]);
   }
 if (!d)
  {
   button[0] = gtk_button_new_with_label("Apply");
   button[1] = gtk_button_new_with_label("Close");
   gtk_signal_connect(GTK_OBJECT(button[0]), "clicked", GTK_SIGNAL_FUNC(apply_proc), (gpointer)(t_entry));
   gtk_signal_connect(GTK_OBJECT(button[1]), "clicked", GTK_SIGNAL_FUNC(delete_proc), (gpointer)dialog);
  }
 button[2] = gtk_button_new_with_label("Connect");
 gtk_signal_connect(GTK_OBJECT(button[2]), "clicked", GTK_SIGNAL_FUNC(connect_proc),(gpointer)(t_entry));
 if (!d)
   {
    gtk_table_attach (GTK_TABLE (table), button[0], 1, 2, 4, 5,GTK_FILL,GTK_FILL|GTK_EXPAND, 0, 0);
    gtk_table_attach (GTK_TABLE (table), button[1], 2, 3, 4, 5,GTK_FILL,GTK_FILL|GTK_EXPAND, 0, 0);
    gtk_table_attach (GTK_TABLE (table), button[2], 0, 1, 4, 5,GTK_FILL,GTK_FILL|GTK_EXPAND, 0, 0);
    gtk_widget_show(button[0]);
    gtk_widget_show(button[1]);
   }
 else gtk_table_attach (GTK_TABLE (table), button[2], 0, 3, 4, 5,GTK_FILL,GTK_FILL|GTK_EXPAND, 0, 0);
 gtk_widget_show(button[2]);
 gtk_widget_grab_focus(entry[0]);
 gtk_container_add(GTK_CONTAINER(GTK_WINDOW(dialog)), table);
 gtk_widget_show(table);
 gtk_signal_connect(GTK_OBJECT(dialog), "delete_event", GTK_SIGNAL_FUNC(delete_proc), (gpointer)dialog);
 gtk_widget_show(dialog);
 gtk_main();
 for (i=0;i<4;i++) free(t_entry[i]);
 free(t_entry);
 destroy_widget(dialog);
}


/*GtkWidget* create_global_autocomplete_cmenu_old()
{
 GtkWidget* cm, *menu_item;
 INT i;
 cm = gtk_menu_new();
 for (i=0;i<nautostr;i++)
   {
    menu_item = gtk_menu_item_new_with_label(autostr[i]);
    gtk_menu_append(GTK_MENU(cm), menu_item);
    gtk_widget_show(menu_item);
   }
 return cm;
}*/

void cmenu_auto(GtkWidget* w, gpointer data)
{
 INT l;
 debug("%s:%d cmenu_auto()",__FILE__,__LINE__);
 debug("CMENU_AUTO: QTEXT=%p", (void*)qtext);
 if (!autoidx || !qtext) return ;
 l = gtk_text_get_length(GTK_TEXT(qtext));
#ifdef _MACHINE_32
 gtk_text_set_point(GTK_TEXT(qtext), autoidx[(INT)data]);
 gtk_text_forward_delete(GTK_TEXT(qtext), l-autoidx[(INT)data]);
#else
 gtk_text_set_point(GTK_TEXT(qtext), autoidx[((INT)(INT64)data)]);
 gtk_text_forward_delete(GTK_TEXT(qtext), l-autoidx[(INT)((INT64)data)]);
#endif
 l = gtk_text_get_length(GTK_TEXT(qtext));
 if (l>=1) gtk_text_set_point(GTK_TEXT(qtext), l-1);
 else      gtk_text_set_point(GTK_TEXT(qtext), 0);
#ifdef _MACHINE_32
 if (autoidx[(INT)data]>0) gtk_text_insert(GTK_TEXT(qtext), 0, 0, 0, " ", 1);
 gtk_text_insert(GTK_TEXT(qtext), NULL,NULL,NULL,autostr[(INT)data],strlen(autostr[(INT)data]));
#else
 if (autoidx[(INT)((INT64)data)]>0) gtk_text_insert(GTK_TEXT(qtext), 0, 0, 0, " ", 1);
 gtk_text_insert(GTK_TEXT(qtext), NULL,NULL,NULL,autostr[(INT)((INT64)data)],strlen(autostr[(INT)((INT64)data)]));
#endif
 if (GTK_TEXT_INDEX(GTK_TEXT(qtext), (gtk_text_get_length(GTK_TEXT(qtext))-1))==' ')
  {
   gtk_text_set_point(GTK_TEXT(qtext), gtk_text_get_length(GTK_TEXT(qtext))-1);
   gtk_text_forward_delete(GTK_TEXT(qtext), 1);
  }
 gtk_menu_popdown(GTK_MENU(glob_cm));
 qtext = 0;
 destroy_global_auto_complete();
}


GtkWidget* create_global_autocomplete_cmenu()
{
 GtkWidget* cm, *bt, *sv, *tab, *mi;
 INT i,ysize;
 debug("%s:%d create_global_autocomplete_cmenu()",HERE);
 debug("CREATE_GLOBAL_AUTOCOMPLETE_CMENU: nautostr=%d", nautostr);
 if (nautostr<=0) return NULL;
 ysize = 15 + 20 * nautostr;
 if (ysize>500) ysize = 500;
 cm = gtk_menu_new();
 sv = gtk_scrolled_window_new(NULL,NULL);
 gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sv), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
 gtk_widget_show(sv);
 gtk_container_set_border_width(GTK_CONTAINER(sv), 1);
 mi = gtk_menu_item_new();
 gtk_container_add(GTK_CONTAINER(mi), sv);
 gtk_widget_set_usize(mi, 300, ysize);
 gtk_widget_show(mi);
 gtk_menu_append(GTK_MENU(cm), mi);
 tab = gtk_table_new(nautostr, 1, FALSE);
 qsort(autostr, nautostr, sizeof(CHAR*), (int(*)(const void*, const void*))strcmp);
 for (i=0;i<nautostr;i++)
   {
    bt = gtk_button_new_with_label(autostr[i]);
    gtk_table_attach(GTK_TABLE(tab), bt, 0, 1, i, i+1,
		      GTK_FILL | GTK_EXPAND,
		      GTK_FILL | GTK_EXPAND | GTK_SHRINK, 0, 0);
 #ifdef _MACHINE_32
    gtk_signal_connect(GTK_OBJECT(bt), "clicked", GTK_SIGNAL_FUNC(cmenu_auto), (gpointer)i);
 #else
    gtk_signal_connect(GTK_OBJECT(bt), "clicked", GTK_SIGNAL_FUNC(cmenu_auto), (gpointer)((INT64)i));
 #endif
    gtk_widget_show(bt);
   }
 gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(sv), tab);
 gtk_widget_show(tab);
 glob_cm = cm;
 return cm;
}


INT dispatch_autocomplete(GtkWidget* w, GdkEvent* event)
{
 GdkEventButton* evb;
 GtkWidget* cm;
 debug("%s:%d dispatch_autocomplete()",HERE);
 debug("displatch_AUTOCOMPLETE");
 /*debug("%s:%d", __FILE__,__LINE__);*/
 if (event->type == GDK_KEY_PRESS)
 {
  if (event->key.keyval == GTK_TAB_KEY)
    {
     debug("GOT TAB");
     evb = (GdkEventButton*)event;
     cm = create_global_autocomplete_cmenu();
     if (cm) gtk_menu_popup(GTK_MENU(cm), NULL,NULL,NULL,NULL, evb->button, evb->time);
     return TRUE;
    }
  return FALSE;
 }
 return FALSE;
}

void create_global_auto_complete(CHAR** table[8], INT* idx, INT* count)	//argument as CHAR**[80 - UGLY
{									//amybe I fix it in the future
 INT i,j,x;
 INT sum;
 debug("%s:%d create_global_autocomplete()",HERE);
 debug("CREATE GLOBAL AUTOCOMPLETE");
 sum = 0;
 autoidx = 0;
 autostr = 0;
 for (i=1;i<=7;i++) if (idx[i]>=0 && count[i]>=0) sum += count[i];
 nautostr = sum;
 autostr = (CHAR**)malloc(nautostr*sizeof(CHAR*));
 autoidx = (INT*)malloc(nautostr*sizeof(INT));
 if (!autostr) fatal_here(HERE, "malloc failed");
 x = 0;
 for (i=1;i<=7;i++) if (idx[i]>=0 && count[i]>=0)
 for (j=0;j<count[i];j++)
   {
    autoidx[x] = idx[i];
    autostr[x] = (CHAR*)malloc((strlen(table[i][j])+1)*sizeof(CHAR));
    if (!autostr[x]) fatal_here(HERE, "malloc failed");
    strcpy(autostr[x], table[i][j]);
    x++;
   }
}


void destroy_global_auto_complete()
{
 INT i;
 debug("%s:%d destroy_global_autocomplete()",HERE);
 debug("DESTROY GLOBAL AUTOCOMPLETE");
 if (!autostr || !autoidx) { nautostr = 0; autoidx = 0; return; }
 for (i=0;i<nautostr;i++) if (autostr[i]) { free(autostr[i]); autostr[i] = 0; }
 if (autostr) free(autostr);
 if (autoidx) free(autoidx);
 autostr = 0;
 autoidx = 0;
 nautostr = 0;
}


void process_parser(GtkWidget* w)
{
 CHAR* text;
 CHAR words[PARSER_MAX_LENGTH][8];	/* GET PREVIOUS WORDS */
 INT  len,i,j,idx[8],match[8];    /* idx where starts, match how much auto_completes */
 CHAR** tips[8];			/* completes */
 debug("%s:%d process_parser()",HERE);
 debug("PROCESS_PARSER");
 len = gtk_text_get_length(GTK_TEXT(w));
 if (len<=1)
   {
    destroy_global_auto_complete();
    return;
   }
 if (!w) { debug("%s:%d, GTK_TEXT is NULL", HERE); return; }
 text = malloc(len+1);
 for (i=0;i<len;i++) text[i] = GTK_TEXT_INDEX(GTK_TEXT(w), i);
 text[len] = 0;
 if (!text) fatal_here(HERE, "memory allocation");
 for (i=0;i<=7;i++) { tips[i] = NULL; idx[i] = -1; match[i] = 0; strcpy(words[i],""); }
 for (i=7;i>=1;i--)
   {
    if ((idx[i]=get_last_words(words[i], text, i))>=0 && strcmp(words[i],"")) match[i] = auto_complete(words[i], &tips[i]);
   }
 destroy_global_auto_complete();
 create_global_auto_complete(tips, idx, match);
 debug("AUTOCOMPLETE STRUCT READY: %d entires", nautostr);
 for (i=1;i<=7;i++) if (match[i]>0 && tips[i])
   {
    for (j=0;j<match[i];j++) if (tips[i][j]) { free(tips[i][j]); tips[i][j] = 0; }
    if (tips[i]) { free(tips[i]); tips[i] = 0; }
   }
 free(text);
}


void check_exec_proc(GtkWidget* txt, gpointer dlg)
{
 INT l;
 debug("%s:%d check_exec_proc()",__FILE__,__LINE__);
 debug("CHECK_EXEC_PROC");
 qtext = txt;
 glob_cm = 0;
 process_parser(txt);
 l = gtk_text_get_length(GTK_TEXT(txt));
 if (GTK_TEXT_INDEX(GTK_TEXT(txt), l-1)==';')
 {
  gtk_text_set_point(GTK_TEXT(txt), l);
  gtk_text_backward_delete(GTK_TEXT(txt), 1);
  execute_proc(dlg,(GtkWidget*)txt);
 }
}


void post_start(GtkWidget* w, gpointer data)
{
 debug("%s:%d post_start()",__FILE__,__LINE__);
 setenv("PGHOST", "localhost", 1);
 setenv("PGPORT", "5432", 1);
 system("pg_ctl start");
}


void global_free_mem(GtkWidget* w, gpointer data)
{
 debug("%s:%d global_free_mem()",__FILE__,__LINE__);
 destruct_tab_view(NULL,NULL);
 shred_memlist();		//FIXME: is it safe?
 dshred_memlist(HERE);		//and this
}


void query_executor(GtkWidget* w, gpointer data)
{
 GtkWidget* window, *table, *text, *hscrollbar, *vscrollbar, *button[3];
 debug("%s:%d query_executor()",__FILE__,__LINE__);
 if (!connected)
   {
    gtk_dialog_printf("Connect to DB first!");
    return;
   }
 if (query_exec)
   {
    gtk_dialog_printf("Another instance of QueryExecutor is running.");
    return;
   }
 window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
 gtk_widget_set_usize (GTK_WIDGET(window), 400, 300);
 gtk_container_set_border_width(GTK_CONTAINER(window), 10);
 table = gtk_table_new (3, 4, FALSE);
 text = gtk_text_new (NULL, NULL);
 gtk_table_attach (GTK_TABLE (table), text, 0, 3, 1, 2,
		      GTK_FILL | GTK_EXPAND,
		      GTK_FILL | GTK_EXPAND | GTK_SHRINK, 0, 0);
 gtk_text_set_editable(GTK_TEXT(text), TRUE);
 gtk_text_set_word_wrap(GTK_TEXT(text), TRUE);
 gtk_text_set_line_wrap(GTK_TEXT(text), TRUE);
 gtk_widget_show (text);
 hscrollbar = gtk_hscrollbar_new (GTK_TEXT (text)->hadj);
 gtk_table_attach (GTK_TABLE (table), hscrollbar, 0, 3, 2, 3,
		      GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
 gtk_widget_show (hscrollbar);
 vscrollbar = gtk_vscrollbar_new (GTK_TEXT (text)->vadj);
 gtk_table_attach (GTK_TABLE (table), vscrollbar, 3, 4, 1, 2,
		      GTK_FILL, GTK_EXPAND | GTK_FILL | GTK_SHRINK, 0, 0);
 gtk_widget_show (vscrollbar);
 button[0] = gtk_button_new_with_label("Run");
 button[1] = gtk_button_new_with_label("Save");
 button[2] = gtk_button_new_with_label("Load");
 gtk_table_attach (GTK_TABLE (table), button[0], 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
 gtk_table_attach (GTK_TABLE (table), button[1], 1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
 gtk_table_attach (GTK_TABLE (table), button[2], 2, 3, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
 gtk_widget_show(button[0]);
 gtk_widget_show(button[1]);
 gtk_widget_show(button[2]);
 gtk_container_add (GTK_CONTAINER(window), table);
 gtk_widget_show (table);
 gtk_widget_grab_focus(text);
 gtk_signal_connect(GTK_OBJECT(window), "delete_event", GTK_SIGNAL_FUNC(qexec_quit), NULL);
 gtk_signal_connect_object(GTK_OBJECT(text),"event", GTK_SIGNAL_FUNC(dispatch_autocomplete), NULL);
 gtk_signal_connect(GTK_OBJECT(text),   "changed", GTK_SIGNAL_FUNC(check_exec_proc), (gpointer)(window));
 gtk_signal_connect(GTK_OBJECT(window), "destroy", GTK_SIGNAL_FUNC(qexec_quit), NULL);
 gtk_signal_connect(GTK_OBJECT(button[0]), "clicked", GTK_SIGNAL_FUNC(execute_proc), (gpointer)(text));
 gtk_signal_connect(GTK_OBJECT(button[1]), "clicked", GTK_SIGNAL_FUNC(save_proc), (gpointer)(text));
 gtk_signal_connect(GTK_OBJECT(button[2]), "clicked", GTK_SIGNAL_FUNC(load_proc), (gpointer)(text));
 gtk_window_set_title(GTK_WINDOW(window), "SQLExec, `;' to execute, 'TAB' to autocomplete");
 gtk_widget_show(window);
 query_exec = window;
 gtk_main();
 destroy_global_auto_complete();
 qtext = glob_cm = 0;
}

static GtkItemFactoryEntry menu_items[] = {
  { "/_File",         		NULL,         NULL, 0, "<Branch>" },
  { "/File/_New DB",     	"<control>N", create_database, 0, NULL },	/* DONE */
  { "/File/_Open DB",    	"<control>O", load_database, 0, NULL },		/* DONE */
  { "/File/_Create DB from File","<control>C", create_DB_from_file, 0, NULL },  /* DONE */
  { "/File/_Save DB to File",	"<control>S", save_database, 0, NULL },		/* DONE */
  { "/File/_Drop DB",	"<control>D", drop_database, 0, NULL },			/* DONE */
  { "/File/_Copy DB",	"<control>Y", copy_database, 0, NULL },			/* DONE */
  { "/File/_Wizards",	"<control>W", wizards, 0, NULL },			/* DONE */
  { "/File/_Execute SQL Query",	"<control>E", query_executor, 0, NULL },	/* DONE */
  { "/File/sep1",     NULL,         NULL, 0, "<Separator>" },
  { "/File/Quit",     "<control>Q", (void (*)(GtkWidget*,gpointer))menu_quit, 0, NULL }, /* DONE */
  { "/_Options",      NULL,         NULL, 0, "<Branch>" },			/* DONE */
  { "/Options/Test",  "<control>T",         exec_test_prog, 0, NULL },		/* DONE */
  { "/Options/Start _Postmaster",  "<control>P",post_start, 0, NULL },		/* DONE */
  { "/Options/Global FreeMem",  NULL,        global_free_mem, 0, NULL },	/* DONE */
  { "/Options/Environment",     NULL,       env_dialog, 0, NULL },		/* DONE */
  { "/_Execute",      NULL,         NULL, 0, "<Branch>" },
  { "/Execute/_SQL Query Editor",  NULL        ,   query_executor, 0, NULL },
  { "/Execute/_Fast Exec OpenDlg",     NULL,       execute_fast_one, 0, NULL },
  { "/Execute/_Fast Exec WriteFileName",     NULL, execute_fast_two, 0, NULL },
  { "/Execute/_Fast Exec Query",     NULL,         execute_fast_thr, 0, NULL},
  { "/_Help",         NULL,         NULL, 0, "<LastBranch>" },
  { "/_Help/About",   NULL,         about_box, 0, NULL },			/* DONE */
};

void get_main_menu(GtkWidget *window, GtkWidget **menubar)
{
  GtkItemFactory *item_factory;
  GtkAccelGroup *accel_group;
  INT nmenu_items = sizeof (menu_items) / sizeof (menu_items[0]);
  debug("%s:%d get_main_menu()",__FILE__,__LINE__);
  accel_group = gtk_accel_group_new ();
  item_factory = gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<main>",accel_group);
  gtk_item_factory_create_items (item_factory, nmenu_items, menu_items, NULL);
  gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);
  if (menubar) *menubar = gtk_item_factory_get_widget (item_factory, "<main>");
}


INT dummy_connect(INT cn, CHAR* templ)
{
 CHAR errstr[MAX_ENTRY_LENGTH];
 debug("%s:%d dummy_connect(%d,%s)",__FILE__,__LINE__,cn,templ?templ:"null");
 if (cn==1)
  {
   connection = gtk_connect_db(templ, pass, errstr);
   if (!connection)
     {
      gtk_dialog_printf("Connect to template DB: %s failed\nDBMS returned:\n%s", templ,errstr);
      return 0;
     }
   connected=1;
   return 1;
  }
 disconnect();
 return 0;
}


INT parse_cmdline(INT lb, CHAR** par, GtkWidget* w)
{
 CHAR* dbname;
 CHAR errstr[MAX_ENTRY_LENGTH];
 INT gotdb,ch;
 INT tmp;
 debug("%s:%d parse_cmdline(%d)",__FILE__,__LINE__,lb);
 gotdb=0;
 dbname = NULL;
 while ((ch = getopt(lb,par,"d:s:p:b:t:m:P:hev")) != -1)
   {
    switch (ch)
      {
       case 's': setenv("PGHOST", optarg, 1); break;
       case 'p': setenv("PGPORT", optarg, 1); break;
       case 'P': pass = malloc(strlen(optarg)+1);
		 if (!pass) fatal_here(HERE, "malloc");
		 strcpy(pass, optarg); break;
       case 'b': setenv("PGDATA", optarg, 1); break;
       case 't': tmp=atoi(optarg); if (tmp<=0) fatal("timeout value: %d too small", tmp); timeout=tmp; break;
       case 'd': gotdb=1; dbname = malloc(strlen(optarg)+1); strcpy(dbname, optarg); break;
       case 'e': gotdb=1; env_dialog(w,(gpointer)1); return 0;
       case 'v': want_all_elems=1; break;
       case 'm': max_tabs = atoi(optarg); if (max_tabs<=0) fatal("max tabs cannot be less or equal 0");  break;
       case 'h':
		 gtk_dialog_printf_big("Options are:\n-d [database_name]\n"
				 "-e (use advanced environment)\n"
				 "-h this help\n"
				 "-s server name\n"
				 "-p port\n"
				 "-P password\n"
				 "-b base dir\n"
				 "-t timeout connection\n"
				 "-m max_tables in EntireDB\n"
				 "-v verbose, display all TABLES/VIEWS etc\n"
				 ); return 1;
       default:
		gtk_dialog_printf("Unrecognized command line option, try:\n"
				   "-h help"); return 1;
      }
   }
 if (!pass)
   {
    CHAR* tmp;
    if (!can_connect(TEMPLATE))
     {
      tmp = gtk_dialog_getpass("CANNOT CONNECT; ENTER PASSWORD:");
      if (!tmp) fatal_here(HERE, "tmp is NULL");
      pass = malloc(strlen(tmp)+1);
      if (!pass) fatal_here(HERE, "cannot malloc pass");
      strcpy(pass, tmp);
     }
   }
 if (!gotdb)
    {
     if (dummy_connect(1,TEMPLATE))
       {
	dbname = gtk_dialog_getdbs(connection, "Connect to database: ");
        dummy_connect(0,TEMPLATE);
       }
     else dbname = gtk_dialog_gettext("Type DB name manually:");
    }
 if (!strcmp(dbname,""))
    {
     gtk_dialog_printf("Empty Database name, cannot be opened.\nSystem will now exit.");
     return 1;
    }
 connection = gtk_connect_db(dbname, pass, errstr);
 if (!connection)
   {
    fatal_nodb(dbname,errstr,1);
    if (gotdb) free(dbname);
    return 1;
   }
 else
   {
    connected=1;
    currentbase = (CHAR*)malloc(strlen(dbname)+2);
    strcpy(currentbase, dbname);
    if (gotdb) free(dbname);
    setup_components();
    return 0;
   }
}


void todo()
{
 debug("%s:%d todo()", __FILE__,__LINE__);
 printf("OBIWAN when CTRL+C/CTRL+V between tables\n");
 printf("LIMIT 3; P233, P235, FUNCTIONS<-TRIGGERS,OPERATORS IN C, SQL\n");
 printf("na ktorej tabeli trigger - p270, 277,w C p288, READ CONTRIB\n");
 printf("read pgsql_book!\n");
 printf("pg_proc - proname, IN TREE (NO!-too much), BUT CAN BE CHOOSEN IN TRIG_WIZ\n");
}


void update_env()
{
 CHAR path[MAXPATH*2];
 debug("%s:%d update_env()", __FILE__,__LINE__);
 strcpy(path,getenv("PATH"));
 strcat(path,":./");
 strcat(path,":/usr/pgsql/bin:/usr/local/pgsql/bin:/usr/local/bin");
 setenv("PATH",path,1);
}


INT main(INT lb, CHAR **par)
{
  GtkWidget *window;
  GtkWidget *menubar;
  GtkWidget* toolbar;
  CHAR winname[MAX_ENTRY_LENGTH];
  INT err;
  debug("%s:%d main(%d, %p)",__FILE__,__LINE__,lb,par);
  printf("PgXexplorer by morgothdbma@o2.pl BSD license: Goodbye\n");
  todo();
  gtk_init(&lb, &par);
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  /*gtk_signal_connect (GTK_OBJECT (window), "destroy", GTK_SIGNAL_FUNC (gtk_main_quit), "WM destroy");*/
  gtk_signal_connect(GTK_OBJECT(window), "delete_event", GTK_SIGNAL_FUNC(menu_quit), NULL);
  gtk_widget_set_usize(GTK_WIDGET(window), 800, 600);
  main_vbox = gtk_vbox_new(FALSE, 1);
  gtk_container_border_width(GTK_CONTAINER (main_vbox), 1);
  gtk_container_add(GTK_CONTAINER (window), main_vbox);
  gtk_widget_show(main_vbox);
  get_main_menu(window, &menubar);
  get_toolbar(window, &toolbar);
  gtk_box_pack_start(GTK_BOX (main_vbox), menubar, FALSE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX (main_vbox), toolbar, FALSE, TRUE, 0);
  gtk_widget_show(toolbar);
  gtk_widget_show(menubar);
  gtk_widget_show(window);
  main_wnd = window;
  update_env();
  setup_signals();
  mlist=NULL;
  //construct_components();
  err = parse_cmdline(lb,par,window);
  if (err) return 1;
  else
    {
     if (currentbase) sprintf(winname, "PgXexplorer: %s", currentbase);
     else sprintf(winname, "PgXexplorer: NULL");
     gtk_window_set_title(GTK_WINDOW(window), winname);
    }
  system("killall -9 pbar");
  gtk_main();
  if (pass) free(pass);
  pass = NULL;
  printf("PgXexplorer by morgothdbma@o2.pl BSD license: Goodbye\n");
  return 0 ;
}

