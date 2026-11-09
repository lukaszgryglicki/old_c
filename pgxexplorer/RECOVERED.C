
#include "common.h"
#include "wizards.h"
extern PGconn* connection;
extern INT want_all_elems;
static struct WizData wdata;
static CHAR* qptr = NULL;
static struct PassInsertData pidata;
static INT wizstep = 0;
static INT ieditor = 0;

INT abort_query(GtkWidget* w, gpointer data)
{
 debug("%s:%d abort_query()",__FILE__,__LINE__);
 gtk_main_quit();
 if (qptr) free(qptr);
 qptr = NULL;
 return FALSE;
}


void update_pidata(GtkWidget* w, gpointer data)
{
 debug("%s:%d update_pidata()",__FILE__,__LINE__);
 INT i;
 INT idx = (INT)data;
 strcpy(pidata.values[idx], gtk_entry_get_text(GTK_ENTRY(w)));
 for (i=0;i<pidata.n;i++)
 {
	debug("[%s] = \"%s\", type=%d\n", pidata.names[i], pidata.values[i], pidata.types[i]);
 }
}


void update_pidata_combo(GtkWidget* w, gpointer data)
{
 debug("%s:%d update_pidata_combo()",__FILE__,__LINE__);
 INT i;
 INT idx = (INT)data;
 CHAR* ctext = gtk_entry_get_text(GTK_ENTRY(w));
 debug("Combo type is: %s\n", ctext);
 if (!strcmp(ctext,"exactly"))       pidata.types[idx] = DEL_TYPE_EXACTLY;
 else if (!strcmp(ctext,"begins"))   pidata.types[idx] = DEL_TYPE_BEGINS;
 else if (!strcmp(ctext,"ends"))     pidata.types[idx] = DEL_TYPE_ENDS;
 else if (!strcmp(ctext,"contains")) pidata.types[idx] = DEL_TYPE_CONTAINS;
 else if (!strcmp(ctext,"none"))     pidata.types[idx] = DEL_TYPE_NONE;
 else fatal_here(HERE, "bad string result from combobox: %s", ctext);
 for (i=0;i<pidata.n;i++)
 {
	debug("[%s] = \"%s\", type=%d\n", pidata.names[i], pidata.values[i], pidata.types[i]);
 }
}


void generate_delete_query(GtkWidget* w, gpointer data)
{
 debug("%s:%d generate_delete_query()",__FILE__,__LINE__);
 INT nbytes;
 INT i,n,x;
 INT ins;
 INT max;
 CHAR* tmps;
 nbytes = MAX_QUERY_LENGTH;
 n = pidata.n;
 max = 0;
 for (i=0;i<n;i++)
 {
	 x = (strlen(pidata.values[i])+strlen(pidata.names[i])+0x20);
	 if (x>max) max = x;
	 nbytes += x;
 }
 tmps = (CHAR*)malloc(2*max+10);
 qptr = (CHAR*)malloc(nbytes);
 if (!qptr || !tmps) fatal_here(HERE, "malloc failed");
 sprintf(qptr, "DELETE FROM %s WHERE ", pidata.tabname);
 ins = 0;
 for (i=0;i<n;i++)
 {
  if (!strcmp(pidata.values[i], "")) continue;
  if (pidata.types[i] == DEL_TYPE_NONE) continue;
  ins++;
  if (!strcmp(pidata.values[i], "NULL"))
  {
	if (i==n-1) sprintf(tmps, "%s IS NULL ", pidata.names[i]);
	else        sprintf(tmps, "%s IS NULL AND ", pidata.names[i]);
	strcat(qptr, tmps);
  }
  else
  {
	if (i==n-1)
	{
	 	if (numeric(pidata.values[i]))		//LOL %%%% is translated to %%, and then finally by execute_printf in
		{					//main.c to %, we need '%' 4 times to have '%' in query to DB
			if (pidata.types[i] == DEL_TYPE_EXACTLY)  sprintf(tmps, "%s = %s ", pidata.names[i], pidata.values[i]);
			if (pidata.types[i] == DEL_TYPE_BEGINS)   sprintf(tmps, "%s LIKE %s%%%% ", pidata.names[i], pidata.values[i]);
			if (pidata.types[i] == DEL_TYPE_ENDS)     sprintf(tmps, "%s LIKE %%%%%s ", pidata.names[i], pidata.values[i]);
			if (pidata.types[i] == DEL_TYPE_CONTAINS) sprintf(tmps, "%s LIKE %%%%%s%%%% ", pidata.names[i], pidata.values[i]);
			strcat(qptr, tmps);
		}
		else
		{
			if (pidata.types[i] == DEL_TYPE_EXACTLY)  sprintf(tmps, "%s = '%s' ", pidata.names[i], pidata.values[i]);
			if (pidata.types[i] == DEL_TYPE_BEGINS)   sprintf(tmps, "%s LIKE '%s%%%%' ", pidata.names[i], pidata.values[i]);
			if (pidata.types[i] == DEL_TYPE_ENDS)     sprintf(tmps, "%s LIKE '%%%%%s' ", pidata.names[i], pidata.values[i]);
			if (pidata.types[i] == DEL_TYPE_CONTAINS) sprintf(tmps, "%s LIKE '%%%%%s%%%%' ", pidata.names[i], pidata.values[i]);
			strcat(qptr, tmps);
		}
	}
	else
	{
	 	if (numeric(pidata.values[i]))
		{
			if (pidata.types[i] == DEL_TYPE_EXACTLY)  sprintf(tmps, "%s = %s AND ", pidata.names[i], pidata.values[i]);
			if (pidata.types[i] == DEL_TYPE_BEGINS)   sprintf(tmps, "%s LIKE %s%%%% AND ", pidata.names[i], pidata.values[i]);
			if (pidata.types[i] == DEL_TYPE_ENDS)     sprintf(tmps, "%s LIKE %%%%%s AND ", pidata.names[i], pidata.values[i]);
			if (pidata.types[i] == DEL_TYPE_CONTAINS) sprintf(tmps, "%s LIKE %%%%%s%%%% AND ", pidata.names[i], pidata.values[i]);
			strcat(qptr, tmps);
		}
		else
		{
			if (pidata.types[i] == DEL_TYPE_EXACTLY)  sprintf(tmps, "%s = '%s' AND ", pidata.names[i], pidata.values[i]);
			if (pidata.types[i] == DEL_TYPE_BEGINS)   sprintf(tmps, "%s LIKE '%s%%%%' AND ", pidata.names[i], pidata.values[i]);
			if (pidata.types[i] == DEL_TYPE_ENDS)     sprintf(tmps, "%s LIKE '%%%%%s' AND ", pidata.names[i], pidata.values[i]);
			if (pidata.types[i] == DEL_TYPE_CONTAINS) sprintf(tmps, "%s LIKE '%%%%%s%%%%' AND ", pidata.names[i], pidata.values[i]);
			strcat(qptr, tmps);
		}
	}
  }
 }
 if (qptr[strlen(qptr)-2]=='D' && qptr[strlen(qptr)-3]=='N' && qptr[strlen(qptr)-4]=='A')
 {
  qptr[strlen(qptr)-4] = 0;
 }
 debug("QUERY >> '%s'\n", qptr);
 free(tmps);
 gtk_main_quit();
 destroy_widget((GtkWidget*)(data));
}


void generate_insert_query(GtkWidget* w, gpointer data)
{
 debug("%s:%d generate_insert_query()",__FILE__,__LINE__);
 INT nbytes;
 INT i,n;
 INT ins;
 nbytes = MAX_QUERY_LENGTH;
 n = pidata.n;
 for (i=0;i<n;i++) nbytes += (strlen(pidata.values[i])+10);
 qptr = (CHAR*)malloc(nbytes);
 if (!qptr) fatal_here(HERE, "malloc failed");
 sprintf(qptr, "INSERT INTO %s VALUES(", pidata.tabname);
 ins = 0;
 for (i=0;i<n;i++)
 {
  if (!strcmp(pidata.values[i], "")) continue;
  ins++;
  if (!strcmp(pidata.values[i], "NULL"))
  {
	if (i==n-1) strcat(qptr,"NULL)");
	else        strcat(qptr,"NULL,");
  }
  else
  {
	if (i==n-1)
	{
	 	if (numeric(pidata.values[i]))
		{
			strcat(qptr, pidata.values[i]);
			strcat(qptr, ")");
		}
		else
		{
			strcat(qptr, "'");
			strcat(qptr, pidata.values[i]);
			strcat(qptr, "'");
			strcat(qptr, ")");
		}
	}
	else
	{
	 	if (numeric(pidata.values[i]))
		{
			strcat(qptr, pidata.values[i]);
			strcat(qptr, ",");
		}
		else
		{
			strcat(qptr, "'");
			strcat(qptr, pidata.values[i]);
			strcat(qptr, "'");
			strcat(qptr, ",");
		}
	}
  }
 }
 if (ins) qptr[strlen(qptr)-1] = ')';
 else strcat(qptr,")");
 debug("QUERY >> %s\n", qptr);
 gtk_main_quit();
 destroy_widget((GtkWidget*)(data));
}


void free_pidata()
{
 INT x,i;
 debug("%s:%d free_pixdata()", __FILE__,__LINE__);
 x = pidata.n;
 for (i=0;i<x;i++)
 {
	 free(pidata.names[i]);
	 free(pidata.values[i]);
 }
 free(pidata.names);
 free(pidata.values);
 free(pidata.tabname);
 free(pidata.types);
}


CHAR* insert_wizard(CHAR* tabname)
{
 CHAR*** ptr;
 INT row,col;
 CHAR errstr[MAX_QUERY_LENGTH+100];
 INT i;
 GtkWidget* window, *table, *entry, *button, *label, *scroller_view, *vbox;
 debug("%s:%d insert_wizard(%s)",__FILE__,__LINE__,tabname?tabname:"null");
 if (!connection) error("no_connection %s:%d", __FILE__,__LINE__);
#ifdef SMALLOC
 dmexecute_printf(connection, &ptr, &row, &col, errstr, 1, HERE, "SELECT * FROM %s", tabname);
#else
 mexecute_printf(connection, &ptr, &row, &col, errstr, 1, "SELECT * FROM %s", tabname);
#endif
 if (row<0 || col<0)
   {
    gtk_dialog_printf_big("Error getting table attributes:\nReturn from DBMS: %s\n", errstr);
    return NULL;
   }
 //
 window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
 gtk_widget_set_usize(GTK_WIDGET(window), 350, 350);
 gtk_container_set_border_width(GTK_CONTAINER(window), 5);
 table = gtk_table_new(2, col, FALSE);
 pidata.n = col;
 pidata.types  = (INT*)malloc(col*sizeof(INT));
 pidata.names  = (CHAR**)malloc(col*sizeof(CHAR*));
 pidata.values = (CHAR**)malloc(col*sizeof(CHAR*));
 pidata.tabname = (CHAR*)malloc(strlen(tabname)+1);
 if (!pidata.names || !pidata.values || !pidata.tabname) fatal_here(HERE, "malloc failed.");
 strcpy(pidata.tabname, tabname);
 for (i=0;i<col;i++)
 {
  pidata.names[i] = (CHAR*)malloc(strlen(ptr[0][i])+1);
  if (!pidata.names[i]) fatal_here(HERE, "malloc failed");
  strcpy(pidata.names[i], ptr[0][i]);
  pidata.values[i] = (CHAR*)malloc(MAX_INSERT_STR_LENGTH+1);
  if (!pidata.values[i]) fatal_here(HERE, "malloc failed");
  strcpy(pidata.values[i], "");
  pidata.types[i] = -1;
 }
 for (i=0;i<col;i++)
 {
  label = gtk_label_new(ptr[0][i]);
  gtk_table_attach(GTK_TABLE (table), label, 0, 1, i, i+1,
		      GTK_FILL,GTK_FILL, 0, 0);
  gtk_widget_show(label);
  entry = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(entry), MAX_INSERT_STR_LENGTH);
  gtk_signal_connect(GTK_OBJECT(entry), "changed", GTK_SIGNAL_FUNC(update_pidata),(gpointer)(i));
  gtk_table_attach(GTK_TABLE (table), entry, 1, 2, i, i+1,
		      GTK_FILL,GTK_FILL, 0, 0);
  gtk_widget_show(entry);
 }
 gtk_widget_show(table);
 scroller_view = gtk_scrolled_window_new(NULL,NULL);
 gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroller_view), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
 gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scroller_view), table);
 gtk_container_set_border_width(GTK_CONTAINER(scroller_view), 2);
 gtk_scrolled_window_set_placement(GTK_SCROLLED_WINDOW(scroller_view), GTK_CORNER_TOP_RIGHT);
 gtk_widget_show(scroller_view);
 button = gtk_button_new_with_label("Insert");
 gtk_widget_show(button);
 vbox = gtk_vbox_new(FALSE, 1);
 gtk_box_pack_start(GTK_BOX(vbox),scroller_view,  TRUE, TRUE, 0);
 gtk_box_pack_start(GTK_BOX(vbox),button, TRUE, TRUE, 0);
 gtk_widget_show(vbox);
 gtk_container_add(GTK_CONTAINER(window), vbox);
 gtk_window_set_title(GTK_WINDOW(window), "Fill in values, You can type NULL also");
 gtk_window_set_modal(GTK_WINDOW(window), TRUE);
 gtk_widget_show(window);
 gtk_signal_connect(GTK_OBJECT(window), "delete_event", GTK_SIGNAL_FUNC(abort_query),(gpointer)(window));
 gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(generate_insert_query), (gpointer)(window));
 gtk_main();
#ifdef SMALLOC
 dmfree_p3c(&ptr, row, col, HERE);
#else
 mfree_p3c(&ptr, row, col);
#endif
 free_pidata();
 return qptr;
}


CHAR* delete_wizard(CHAR* tabname)
{
 CHAR*** ptr;
 INT row,col;
 CHAR errstr[MAX_QUERY_LENGTH+100];
 INT i;
 GtkWidget* window, *table, *entry, *button, *label, *scroller_view, *vbox, *combo;
 GList* list;
 debug("%s:%d delete_wizard(%s)",__FILE__,__LINE__,tabname?tabname:"null");
 if (!connection) error("no_connection %s:%d", __FILE__,__LINE__);
#ifdef SMALLOC
 dmexecute_printf(connection, &ptr, &row, &col, errstr, 1, HERE, "SELECT * FROM %s", tabname);
#else
 mexecute_printf(connection, &ptr, &row, &col, errstr, 1, "SELECT * FROM %s", tabname);
#endif
 if (row<0 || col<0)
   {
    gtk_dialog_printf_big("Error getting table attributes:\nReturn from DBMS: %s\n", errstr);
    return NULL;
   }
 if (row==1 || col==0)
   {
    gtk_dialog_printf("Nothing to delete!");
    return NULL;
   }
 //
 window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
 gtk_widget_set_usize(GTK_WIDGET(window), 450, 400);
 gtk_container_set_border_width(GTK_CONTAINER(window), 5);
 table = gtk_table_new(3, col, FALSE);
 pidata.n = col;
 pidata.types  = (INT*)malloc(col*sizeof(INT));
 pidata.names  = (CHAR**)malloc(col*sizeof(CHAR*));
 pidata.values = (CHAR**)malloc(col*sizeof(CHAR*));
 pidata.tabname = (CHAR*)malloc(strlen(tabname)+1);
 if (!pidata.names || !pidata.values || !pidata.tabname) fatal_here(HERE, "malloc failed.");
 strcpy(pidata.tabname, tabname);
 for (i=0;i<col;i++)
 {
  pidata.names[i] = (CHAR*)malloc(strlen(ptr[0][i])+1);
  if (!pidata.names[i]) fatal_here(HERE, "malloc failed");
  strcpy(pidata.names[i], ptr[0][i]);
  pidata.values[i] = (CHAR*)malloc(MAX_INSERT_STR_LENGTH+1);
  if (!pidata.values[i]) fatal_here(HERE, "malloc failed");
  strcpy(pidata.values[i], "");
  pidata.types[i] = DEL_TYPE_NONE;
 }
 for (i=0;i<col;i++)
 {
  label = gtk_label_new(ptr[0][i]);
  gtk_table_attach(GTK_TABLE (table), label, 0, 1, i, i+1,
		      GTK_FILL,GTK_FILL, 0, 0);
  gtk_widget_show(label);
  entry = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(entry), MAX_INSERT_STR_LENGTH);
  gtk_signal_connect(GTK_OBJECT(entry), "changed", GTK_SIGNAL_FUNC(update_pidata),(gpointer)(i));
  gtk_table_attach(GTK_TABLE (table), entry, 1, 2, i, i+1,
		      GTK_FILL,GTK_FILL, 0, 0);
  gtk_widget_show(entry);
  list = NULL;
  list = g_list_append(list, "none");
  list = g_list_append(list, "exactly");
  list = g_list_append(list, "begins");
  list = g_list_append(list, "ends");
  list = g_list_append(list, "contains");
  combo = gtk_combo_new();
  gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(combo)->entry),"Select match type");
  gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(combo)->entry), FALSE);
  gtk_combo_set_popdown_strings(GTK_COMBO(combo), list);
  g_list_free(list);
  gtk_table_attach(GTK_TABLE (table), combo, 2, 3, i, i+1,
		      GTK_FILL,GTK_FILL, 0, 0);
  gtk_signal_connect(GTK_OBJECT(GTK_COMBO(combo)->entry), "changed", GTK_SIGNAL_FUNC(update_pidata_combo),(gpointer)(i));
  gtk_widget_show(combo);
 }
 gtk_widget_show(table);
 scroller_view = gtk_scrolled_window_new(NULL,NULL);
 gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroller_view), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
 gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scroller_view), table);
 gtk_container_set_border_width(GTK_CONTAINER(scroller_view), 2);
 gtk_scrolled_window_set_placement(GTK_SCROLLED_WINDOW(scroller_view), GTK_CORNER_TOP_RIGHT);
 gtk_widget_show(scroller_view);
 button = gtk_button_new_with_label("Delete");
 gtk_widget_show(button);
 vbox = gtk_vbox_new(FALSE, 1);
 gtk_box_pack_start(GTK_BOX(vbox),scroller_view,  TRUE, TRUE, 0);
 gtk_box_pack_start(GTK_BOX(vbox),button, TRUE, TRUE, 0);
 gtk_widget_show(vbox);
 gtk_container_add(GTK_CONTAINER(window), vbox);
 gtk_window_set_title(GTK_WINDOW(window), "Select delete conditions");
 gtk_window_set_modal(GTK_WINDOW(window), TRUE);
 gtk_widget_show(window);
 gtk_signal_connect(GTK_OBJECT(window), "delete_event", GTK_SIGNAL_FUNC(abort_query),(gpointer)(window));
 gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(generate_delete_query), (gpointer)(window));
 gtk_main();
#ifdef SMALLOC
 dmfree_p3c(&ptr, row, col, HERE);
#else
 mfree_p3c(&ptr, row, col);
#endif
 free_pidata();
 return qptr;
}


INT delete_combos_proc_wiz(GtkWidget* w, gpointer d)
{
 debug("%s:%d delete_combos_proc_wiz()",__FILE__,__LINE__);
 gtk_main_quit();
 if (qptr) free(qptr);
 qptr = NULL;
 return TRUE;
}


void ok_combo_proc_wiz(GtkWidget* w, gpointer cmb)
{
 debug("%s:%d ok_combo_proc_wiz()",__FILE__,__LINE__);
 if (!cmb) return;
 strcpy(qptr,gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(cmb)->entry)));
 gtk_main_quit();
}


INT setup_combo_views(PGconn* c, GtkWidget* w, GtkWidget** cmb)
{
 CHAR errstr[MAX_QUERY_LENGTH];
 debug("%s:%d setup_combo_views()", __FILE__,__LINE__);
 INT row,col,i;
 CHAR*** ptr;
 CHAR* tmps;
 CHAR first[MAX_TABLE_NAME*2+4];
 GList* list;
 list = NULL;
 debug("%s:%d setup_combo",__FILE__,__LINE__);
 list = NULL;
 if (!c || !w) error("setup_combo_tables, %s:%d",__FILE__,__LINE__);
//
 if (!want_all_elems)
 execute_printf(connection, &ptr, &row, &col, errstr, 0,
		 "SELECT schemaname,viewname from pg_views WHERE viewname NOT LIKE 'pg_%%' AND schemaname != 'information_schema' ORDER BY schemaname");
 else
 execute_printf(connection, &ptr, &row, &col, errstr, 0,
		 "SELECT schemaname,viewname from pg_views ORDER BY schemaname");
//
 if (!ptr)
 {
  gtk_dialog_printf("There is no views in DB!");
  cmb=NULL;
  return -1;
 }
 if (col<0 || row<0)
 {
  gtk_dialog_printf("%s", errstr);
  cmb=NULL;
  return -1;
 }
 for (i=0;i<row;i++)
 {
	 tmps = (CHAR*)malloc(strlen(ptr[i][0])+strlen(ptr[i][0])+4);
	 if (!tmps) fatal_here(HERE, "malloc failed");
	 strcpy(tmps, ptr[i][0]);
	 strcat(tmps,".");
	 strcat(tmps, ptr[i][1]);
	 debug("tmps = %s\n", tmps);
	 list = g_list_append(list, tmps);
	 if (i==0) strcpy(first, tmps);
/*	 free(tmps);*/				//NOT NEEDED
 }
 list = g_list_append(list,"*all*");
 *cmb = gtk_combo_new();
 gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(*cmb)->entry),first);
 gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(*cmb)->entry), FALSE);
 gtk_combo_set_popdown_strings(GTK_COMBO(*cmb), list);
 g_list_free(list);				//HERE FREED
 free_p3c(&ptr, row, col);
 return 0;
}


INT setup_combo_tables(PGconn* c, GtkWidget* w, GtkWidget** cmb, INT all_opt)
{
 CHAR errstr[MAX_QUERY_LENGTH];
 debug("%s:%d setup_combo_tables(%d)", __FILE__,__LINE__, all_opt);
 INT row,col,i;
 CHAR*** ptr;
 GList* list;
 list = NULL;
 debug("%s:%d setup_combo",__FILE__,__LINE__);
 list = NULL;
 if (!c || !w) error("setup_combo_tables, %s:%d",__FILE__,__LINE__);
 if (!want_all_elems)
 execute_printf(c, &ptr, &row, &col, errstr, 0,
		 "SELECT tablename from pg_tables WHERE tablename NOT LIKE 'pg_%%' AND tablename NOT LIKE 'sql_%%' ORDER BY tablename");
 else
 execute_printf(connection, &ptr, &row, &col, errstr, 0,
		 "SELECT tablename from pg_tables ORDER BY tablename");
 if (!ptr)
 {
  gtk_dialog_printf("There is no tables in DB!");
  cmb=NULL;
  return -1;
 }
 if (col<0 || row<0)
 {
  gtk_dialog_printf("%s", errstr);
  cmb=NULL;
  return -1;
 }
 for (i=0;i<row;i++) list = g_list_append(list, ptr[i][0]);
 if (all_opt) list = g_list_append(list,"*all*");
 *cmb = gtk_combo_new();
 gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(*cmb)->entry),ptr[0][0]);
 gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(*cmb)->entry), FALSE);
 gtk_combo_set_popdown_strings(GTK_COMBO(*cmb), list);
 g_list_free(list);
 free_p3c(&ptr, row, col);
 return 0;
}


CHAR* select_table_wizard(INT all_opt)
{
 GtkWidget* dialog, *combo, *button, *label;
 qptr = (CHAR*)malloc(MAX_TABLE_NAME+1);
 if (!qptr) fatal_here(HERE, "malloc failed");
 debug("%s:%d select_table_wizard(%d)",__FILE__,__LINE__,all_opt);
 if (!connection) error("select_table_wizard: %s:%d", __FILE__,__LINE__);
 dialog = gtk_dialog_new();
 gtk_window_set_title(GTK_WINDOW(dialog),"Select table");
 gtk_widget_set_usize(dialog, 500, 80);
 gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
 label = gtk_label_new("Select table:");
 gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), label, TRUE, TRUE, 0);
 gtk_widget_show(label);
 button = gtk_button_new_with_label("Select");
 combo=NULL;
 if (setup_combo_tables(connection, dialog, &combo, all_opt)==-1)
 {
	 free(qptr);
	 qptr = NULL;
	 return NULL;
 }
 gtk_entry_set_max_length(GTK_ENTRY(GTK_COMBO(combo)->entry), MAX_TABLE_NAME);
 gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area), combo, TRUE, TRUE, 0);
 gtk_widget_show(combo);
 gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area), button, TRUE, TRUE, 0);
 gtk_signal_connect(GTK_OBJECT(dialog), "delete_event", GTK_SIGNAL_FUNC(delete_combos_proc_wiz),(gpointer)dialog);
 gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(ok_combo_proc_wiz), (gpointer)combo);
 gtk_widget_grab_focus(combo);
 gtk_widget_show(button);
 gtk_widget_show(dialog);
 gtk_main();
 destroy_widget(dialog);
 return qptr;
}


CHAR* select_view_wizard()
{
 GtkWidget* dialog, *combo, *button, *label;
 qptr = (CHAR*)malloc(MAX_TABLE_NAME+1);
 if (!qptr) fatal_here(HERE, "malloc failed");
 debug("%s:%d select_view_wizard()",__FILE__,__LINE__);
 if (!connection) error("select_view_wizard: %s:%d", __FILE__,__LINE__);
 dialog = gtk_dialog_new();
 gtk_window_set_title(GTK_WINDOW(dialog),"Select view");
 gtk_widget_set_usize(dialog, 500, 80);
 gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
 label = gtk_label_new("Select view:");
 gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), label, TRUE, TRUE, 0);
 gtk_widget_show(label);
 button = gtk_button_new_with_label("Select");
 combo=NULL;
 if (setup_combo_views(connection, dialog, &combo)==-1)
   {
    free(qptr);
    qptr = NULL;
    return NULL;
   }
 gtk_entry_set_max_length(GTK_ENTRY(GTK_COMBO(combo)->entry), MAX_TABLE_NAME);
 gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area), combo, TRUE, TRUE, 0);
 gtk_widget_show(combo);
 gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area), button, TRUE, TRUE, 0);
 gtk_signal_connect(GTK_OBJECT(dialog), "delete_event", GTK_SIGNAL_FUNC(delete_combos_proc_wiz),(gpointer)dialog);
 gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(ok_combo_proc_wiz), (gpointer)combo);
 gtk_widget_grab_focus(combo);
 gtk_widget_show(button);
 gtk_widget_show(dialog);
 gtk_main();
 destroy_widget(dialog);
 return qptr;
}


INT gtk_scale_get_value(GtkWidget* w, gpointer ptr)	//SIZEOF(GFLOAT) CAN BE MORE THAN SIZEOF(GPOINTER)
{
 GtkAdjustment* adj;
 debug("%s:%d gtk_scale_get_value()",__FILE__,__LINE__);
 adj = gtk_range_get_adjustment(GTK_RANGE(&(GTK_SCALE(ptr)->range)));
 return (INT)(adj->value);
}


void gtk_scale_set_value(GtkWidget* scale, gfloat val)
{
 debug("%s:%d gtk_scale_set_value(%f)",__FILE__,__LINE__,val);
 GtkObject* adj;
 adj = gtk_adjustment_new(0.0,0.0,49.0, 1.0, 1.0, 1.0);
 gtk_adjustment_set_value(GTK_ADJUSTMENT(adj), val);
 gtk_range_set_adjustment(GTK_RANGE(&(GTK_SCALE(scale)->range)), GTK_ADJUSTMENT(adj));
}


void scale_changed_func(GtkWidget* scale, gfloat dummy)
{
 debug("%s:%d scale_changed_func()",__FILE__,__LINE__);
 GtkWidget* label;
 INT val = gtk_scale_get_value(wdata.scale, wdata.scale);
 if (val)
   {
    gtk_widget_destroy(wdata.label2);
    label = gtk_label_new("Select number of columns (1-48):\nSelect 0 if you want to\nLoad prepared table from file");
    wdata.label2 = label;
    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
    gtk_widget_show(label);
    gtk_table_attach(GTK_TABLE(wdata.table), label, 0, 50, 0, 30, GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND, 0, 0);
    ieditor = 0;
   }
 else
   {
    gtk_widget_destroy(wdata.label2);
    label = gtk_label_new("Select number of columns (1-48):\nYou have selcted 0 as number of columns\nTable will be loaded from file in next step");
    wdata.label2 = label;
    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
    gtk_widget_show(label);
    gtk_table_attach(GTK_TABLE(wdata.table), label, 0, 50, 0, 30, GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND, 0, 0);
    ieditor = 1;
   }
}


void ctab_prop_combos(GtkWidget** combo, GtkWidget* table, INT I)
{
 debug("%s:%d ctab_prop_combos(%d)",__FILE__,__LINE__, I);
  CHAR*** ptr;
  INT row, col, i;
  CHAR errstr[MAX_QUERY_LENGTH];
  GList* list;
  CHAR **tmps;
  if (!want_all_elems)
  execute_printf(connection, &ptr, &row, &col, errstr, 0,
	"SELECT tablename from pg_tables WHERE tablename NOT LIKE 'pg_%%' AND tablename NOT LIKE 'sql_%%' ORDER BY tablename");
  else
  execute_printf(connection, &ptr, &row, &col, errstr, 0,
		 "SELECT tablename from pg_tables ORDER BY tablename");
  if (!ptr) return;
  if (row<0 || col<0)
   {
    gtk_dialog_printf_big("Error executing query:\nReturn from DBMS: %s\n", errstr);
    return;
   }
  list = NULL;
  list = g_list_append(list, "-other properties-");
  list = g_list_append(list, "primary key");
  tmps = (CHAR**)malloc(row*sizeof(CHAR**));
  for (i=0;i<row;i++)
    {
     tmps[i] = (CHAR*)malloc(strlen(ptr[i][0])+16);
     sprintf(tmps[i], "references %s", ptr[i][0]);
     list = g_list_append(list, tmps[i]);
    }
  *combo = gtk_combo_new();
  gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(*combo)->entry),"");
  gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(*combo)->entry), FALSE);
  gtk_combo_set_popdown_strings(GTK_COMBO(*combo), list);
  g_list_free(list);
  gtk_table_attach(GTK_TABLE (table), *combo, 16, 24, I+1, I+2,GTK_FILL,GTK_FILL, 0, 0);
  //gtk_signal_connect(GTK_OBJECT(GTK_COMBO(combo)->entry), "changed", GTK_SIGNAL_FUNC(update_pidata_combo),(gpointer)(i));
  gtk_widget_show(*combo);
  for(i=0;i<row;i++) free(tmps[i]);
  free(tmps);
  free_p3c(&ptr, row, col);
}


void ctab_type_combos(GtkWidget** combo, GtkWidget* table, INT i)
{
 debug("%s:%d ctab_type_combos(%d)",__FILE__,__LINE__, i);
  GList* list;
  list = NULL;
  list = g_list_append(list, "text");
  list = g_list_append(list, "smallint");
  list = g_list_append(list, "integer");
  list = g_list_append(list, "bigint");
  list = g_list_append(list, "boolean");
  list = g_list_append(list, "float4");
  list = g_list_append(list, "float8");
  list = g_list_append(list, "decimal");
  list = g_list_append(list, "time");
  list = g_list_append(list, "timestamp");
  list = g_list_append(list, "data");
  list = g_list_append(list, "varchar(1)");
  list = g_list_append(list, "varchar(2)");
  list = g_list_append(list, "varchar(4)");
  list = g_list_append(list, "varchar(10)");
  list = g_list_append(list, "varchar(20)");
  list = g_list_append(list, "varchar(40)");
  list = g_list_append(list, "varchar(100)");
  list = g_list_append(list, "varchar(255)");
  list = g_list_append(list, "varchar(1024)");
  *combo = gtk_combo_new();
  gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(*combo)->entry),"Select type");
  gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(*combo)->entry), FALSE);
  gtk_combo_set_popdown_strings(GTK_COMBO(*combo), list);
  g_list_free(list);
  gtk_table_attach(GTK_TABLE (table), *combo, 8, 16, i+1, i+2,GTK_FILL,GTK_FILL, 0, 0);
  //gtk_signal_connect(GTK_OBJECT(GTK_COMBO(combo)->entry), "changed", GTK_SIGNAL_FUNC(update_pidata_combo),(gpointer)(i));
  gtk_widget_show(*combo);
}


void setup_mainctrls(GtkWidget* table)
{
 GtkWidget **name, **type, **props, **anull, **def, *lname, *ltype, *lprops, *lanull, *ldef;
 INT i;
 debug("%s:%d setup_mainctrls()",__FILE__,__LINE__);
 name  = (GtkWidget**)malloc(wdata.ncols*sizeof(GtkWidget*));
 if (!name)  fatal_here(HERE, "malloc failed");
 type  = (GtkWidget**)malloc(wdata.ncols*sizeof(GtkWidget*));
 if (!type)  fatal_here(HERE, "malloc failed");
 props = (GtkWidget**)malloc(wdata.ncols*sizeof(GtkWidget*));
 if (!props) fatal_here(HERE, "malloc failed");
 anull = (GtkWidget**)malloc(wdata.ncols*sizeof(GtkWidget*));
 if (!anull) fatal_here(HERE, "malloc failed");
 def   = (GtkWidget**)malloc(wdata.ncols*sizeof(GtkWidget*));
 if (!def)   fatal_here(HERE, "malloc failed");
 lname  = gtk_label_new("Column Name:");
 ltype  = gtk_label_new("Type:");
 lprops = gtk_label_new("Properties:");
 lanull = gtk_label_new("NULL control:");
 ldef   = gtk_label_new("Default value:");
 gtk_table_attach(GTK_TABLE(table), lname , 0,   8, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
 gtk_table_attach(GTK_TABLE(table), ltype , 8,  16, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
 gtk_table_attach(GTK_TABLE(table), lprops, 16, 24, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
 gtk_table_attach(GTK_TABLE(table), lanull, 24, 32, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
 gtk_table_attach(GTK_TABLE(table), ldef  , 32, 40, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
 gtk_widget_show(lname);
 gtk_widget_show(ltype);
 gtk_widget_show(lprops);
 gtk_widget_show(lanull);
 gtk_widget_show(ldef);
 for (i=0;i<wdata.ncols;i++)
  {
   name[i] = gtk_entry_new();
   gtk_entry_set_max_length(GTK_ENTRY(name[i]), 127);
   gtk_entry_set_editable(GTK_ENTRY(name[i]), TRUE);
   //gtk_signal_connect(GTK_OBJECT(name[i]), "activate", GTK_SIGNAL_FUNC(update_entry), NULL);
   //gtk_signal_connect(GTK_OBJECT(name[i]), "changed",  GTK_SIGNAL_FUNC(update_entry), NULL);
   gtk_table_attach(GTK_TABLE(table), name[i], 0, 8, i+1, i+2, GTK_FILL, GTK_FILL, 0, 0);
   gtk_widget_show(name[i]);
   ctab_type_combos(&type[i], table, i);
   ctab_prop_combos(&props[i], table, i);
   anull[i] = gtk_check_button_new_with_label("Allow NULL");
   //gtk_signal_connect(GTK_OBJECT(anull[i]), "toggled",GTK_SIGNAL_FUNC(entry_toggle_editable), entry);
   gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(anull[i]), FALSE);
   gtk_table_attach(GTK_TABLE(table), anull[i], 24, 32, i+1, i+2, GTK_FILL, GTK_FILL, 0, 0);
   gtk_widget_show(anull[i]);
   def[i] = gtk_entry_new();
   gtk_entry_set_max_length(GTK_ENTRY(def[i]), STDSTRING);
   gtk_entry_set_editable(GTK_ENTRY(def[i]), TRUE);
   //gtk_signal_connect(GTK_OBJECT(def[i]), "activate", GTK_SIGNAL_FUNC(update_entry), NULL);
   //gtk_signal_connect(GTK_OBJECT(def[i]), "changed",  GTK_SIGNAL_FUNC(update_entry), NULL);
   gtk_table_attach(GTK_TABLE(table), def[i], 32, 40, i+1, i+2, GTK_FILL, GTK_FILL, 0, 0);
   gtk_widget_show(def[i]);
  }
 wdata.name   = name;
 wdata.type   = type;
 wdata.props  = props;
 wdata.anull  = anull;
 wdata.def    = def;
 wdata.lname  = lname;
 wdata.ltype  = ltype;
 wdata.lprops = lprops;
 wdata.lanull = lanull;
 wdata.ldef   = ldef;
}


void setup_txtscene(GtkWidget* table)
{
 debug("%s:%d setup_txt_scene()", __FILE__,__LINE__);
 GtkWidget* text, *hscrollbar, *vscrollbar, *button[2];
 text = gtk_text_new(NULL, NULL);
 gtk_table_attach(GTK_TABLE(table), text, 0, 40, 10, 40,
		      GTK_FILL | GTK_EXPAND,
		      GTK_FILL | GTK_EXPAND | GTK_SHRINK, 0, 0);
 gtk_text_set_editable(GTK_TEXT(text), TRUE);
 gtk_text_set_word_wrap(GTK_TEXT(text), TRUE);
 gtk_text_set_line_wrap(GTK_TEXT(text), TRUE);
 gtk_widget_show(text);
 hscrollbar = gtk_hscrollbar_new(GTK_TEXT (text)->hadj);
 gtk_table_attach(GTK_TABLE(table), hscrollbar, 0, 40, 40, 50,
		      GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
 gtk_widget_show(hscrollbar);
 vscrollbar = gtk_vscrollbar_new(GTK_TEXT (text)->vadj);
 gtk_table_attach (GTK_TABLE (table), vscrollbar, 40, 50, 10, 40,
		      GTK_FILL, GTK_EXPAND | GTK_FILL | GTK_SHRINK, 0, 0);
 gtk_widget_show(vscrollbar);
 button[0] = gtk_button_new_with_label("Load");
 button[1] = gtk_button_new_with_label("Save");
 gtk_table_attach(GTK_TABLE(table), button[0], 0, 20, 0, 10, GTK_FILL, GTK_FILL, 0, 0);
 gtk_table_attach(GTK_TABLE(table), button[1], 20,40, 0, 10, GTK_FILL, GTK_FILL, 0, 0);
 gtk_widget_show(button[0]);
 gtk_widget_show(button[1]);
 gtk_widget_grab_focus(text);
 gtk_signal_connect(GTK_OBJECT(button[0]), "clicked", GTK_SIGNAL_FUNC(load_proc), (gpointer)(text));
 gtk_signal_connect(GTK_OBJECT(button[1]), "clicked", GTK_SIGNAL_FUNC(save_proc), (gpointer)(text));
 wdata.text = text;
 wdata.load = button[0];
 wdata.save = button[1];
 wdata.hscrl = hscrollbar;
 wdata.vscrl = vscrollbar;
}


void prev_ctable_func(GtkWidget* w, gpointer data)
{
 debug("%s:%d prev_ctable_func()", __FILE__,__LINE__);
 GtkWidget* tab = *((GtkWidget**)(data));
 GtkWidget* label, *scale;
 GtkObject *adj;
 switch (wizstep)
 {
  case 0:	/* 0 --> abort */
  	if (gtk_dialog_yes_no_printf("Going back at first step\nmeans closing Wizard. OK?"))
   	  {
    	   qptr = NULL;
    	   gtk_main_quit();
           return;
   	  }
	break;
  case 1:	/* 1 --> 0 */
	    gtk_widget_destroy(wdata.label2);
	    wdata.ncols = gtk_scale_get_value(wdata.scale, wdata.scale);
	    gtk_widget_destroy(wdata.scale);
	    if (wdata.ncols>0) ieditor = 0;
 	    label = gtk_label_new("Welcome to Create Table Wizard\nthis wizard will guide You\nthrough the rest of Create Table process\nGood luck!");
 	    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_FILL);
	    gtk_table_attach(GTK_TABLE(tab), label, 0, 50, 0, 50, GTK_FILL,GTK_FILL, 0, 0);
   	    gtk_widget_show(label);
            wdata.label = label;
	    wizstep--;
	    break;
  case 2:	/* 2 -->  1 */
	    if (ieditor)
	      {
               gtk_widget_destroy(wdata.text);
               gtk_widget_destroy(wdata.save);
               gtk_widget_destroy(wdata.load);
               gtk_widget_destroy(wdata.hscrl);
               gtk_widget_destroy(wdata.vscrl);
	      }
	    else
	      {
	       INT x;
               gtk_widget_destroy(wdata.lname);
               gtk_widget_destroy(wdata.ltype);
               gtk_widget_destroy(wdata.lprops);
               gtk_widget_destroy(wdata.lanull);
               gtk_widget_destroy(wdata.ldef);
	       for (x=0;x<wdata.ncols;x++)
	         {
                  gtk_widget_destroy(wdata.name[x]);
                  gtk_widget_destroy(wdata.type[x]);
                  gtk_widget_destroy(wdata.props[x]);
                  gtk_widget_destroy(wdata.anull[x]);
                  gtk_widget_destroy(wdata.def[x]);
	         }
	       free(wdata.name);
	       free(wdata.type);
	       free(wdata.props);
	       free(wdata.anull);
	       free(wdata.def);
	      }
 	    adj = gtk_adjustment_new(0.0,0.0,49.0, 1.0, 1.0, 1.0);
	    scale = gtk_hscale_new(GTK_ADJUSTMENT(adj));
	    gtk_scale_set_digits(GTK_SCALE(scale), 0);
	    gtk_scale_set_value(scale, (gfloat)wdata.ncols);
	    gtk_scale_set_draw_value(GTK_SCALE(scale), TRUE);
	    gtk_scale_set_value(scale, (gfloat)(wdata.ncols));
	    gtk_widget_show(scale);
	    wdata.scale = scale;
 	    gtk_table_attach(GTK_TABLE(tab), scale, 0, 50, 30, 50, GTK_FILL,GTK_FILL|GTK_EXPAND, 0, 0);
 	    label = gtk_label_new("Select number of columns (1-48):\nSelect 0 if you want to\nLoad prepared table from file");
	    wdata.label2 = label;
 	    gtk_lab
l_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
 	    gtk_table_attach(GTK_TABLE(tab), label, 0, 50, 0, 30, GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND, 0, 0);
	    gtk_widget_show(label);
	    gtk_widget_show(tab);
 	    gtk_signal_connect(GTK_OBJECT(adj), "value_changed", GTK_SIGNAL_FUNC(scale_changed_func), NULL);
	    wizstep--;
	    break;
  default:	/* other */
	    gtk_dialog_printf("Not implemented!");
	    qptr = NULL;
	    gtk_main_quit();
	    return;
 }
}


void next_ctable_func(GtkWidget* w, gpointer data)
{
 debug("%s:%d next_ctable_func()", __FILE__,__LINE__);
 GtkWidget *scale, *label;
 GtkObject *adj;
 GtkWidget* tab = *((GtkWidget**)(data));
 switch (wizstep)
   {
    case 0:	/* 0 --> 1 */
	    gtk_widget_destroy(wdata.label);
 	    adj = gtk_adjustment_new(0.0,0.0,49.0, 1.0, 1.0, 1.0);
	    scale = gtk_hscale_new(GTK_ADJUSTMENT(adj));
	    gtk_scale_set_digits(GTK_SCALE(scale), 0);
	    gtk_scale_set_value(scale, (gfloat)wdata.ncols);
	    gtk_scale_set_draw_value(GTK_SCALE(scale), TRUE);
	    gtk_scale_set_value(scale, (gfloat)(wdata.ncols));
	    gtk_widget_show(scale);
	    wdata.scale = scale;
 	    gtk_table_attach(GTK_TABLE(tab), scale, 0, 50, 30, 50, GTK_FILL,GTK_FILL|GTK_EXPAND, 0, 0);
 	    label = gtk_label_new("Select number of columns (1-48):\nSelect 0 if you want to\nLoad prepared table from file");
	    wdata.label2 = label;
 	    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
 	    gtk_table_attach(GTK_TABLE(tab), label, 0, 50, 0, 30, GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND, 0, 0);
	    gtk_widget_show(label);
	    gtk_widget_show(tab);
 	    gtk_signal_connect(GTK_OBJECT(adj), "value_changed", GTK_SIGNAL_FUNC(scale_changed_func), NULL);
	    wizstep++;
	    break;
    case 1:	/* 1 --> 2 */
	    wdata.ncols = gtk_scale_get_value(wdata.scale, wdata.scale);
	    gtk_widget_destroy(wdata.label2);
	    gtk_widget_destroy(wdata.scale);
	    if (wdata.ncols == 0)
	      {
	       setup_txtscene(tab);
	       wizstep++;
	       ieditor = 1;
	       gtk_widget_show(tab);
	       break;
	      }
	    ieditor = 0;
	    setup_mainctrls(tab);
	    gtk_widget_show(tab);
	    wizstep++;
	    break;
    case 2:	/* currently ready for load/save options */
	    if (ieditor)
	      {
		INT l,i;
		CHAR* tmp;
 		l = gtk_text_get_length(GTK_TEXT(wdata.text));
		if (l > MAX_QUERY_LENGTH-10)
		  {
                   gtk_dialog_printf("Query is too long!");
		   break;
		  }
 		if (l<3)
    		  {
		   gtk_dialog_printf("Will not execute empty query");
		   break;
		  }
		 qptr = malloc(l+2);
		 for (i=0;i<l;i++) qptr[i] = GTK_TEXT_INDEX(GTK_TEXT(wdata.text), i);
		 qptr[i] = 0;
		 tmp = malloc(strlen(qptr)+1);
		 strcpy(tmp, qptr);
		 downcase(tmp);
		 if (!strstr(tmp,"create table"))
		   {
		    gtk_dialog_printf("CREATE TABLE QUERY must contain\n\"CREATE TABLE\" statement");
                    free(qptr);
		    qptr = NULL;
		    free(tmp);
		    break;
		   }
		 free(tmp);
  	         if (gtk_dialog_yes_no("Do you want to save wizard query?"))
	           {
		    save_proc(NULL, (gpointer)wdata.text);
	           }
                 gtk_widget_destroy(wdata.text);
                 gtk_widget_destroy(wdata.save);
                 gtk_widget_destroy(wdata.load);
                 gtk_widget_destroy(wdata.hscrl);
                 gtk_widget_destroy(wdata.vscrl);
	         wizstep++;
	         gtk_main_quit();
		 break;
	      }
	    //FREE ELEMENTS...
	    //FIXME there check if informations are OK
	    //execute to check if OK??
	    INT x;
            gtk_widget_destroy(wdata.lname);
            gtk_widget_destroy(wdata.ltype);
            gtk_widget_destroy(wdata.lprops);
            gtk_widget_destroy(wdata.lanull);
            gtk_widget_destroy(wdata.ldef);
	    for (x=0;x<wdata.ncols;x++)
	         {
                  gtk_widget_destroy(wdata.name[x]);
                  gtk_widget_destroy(wdata.type[x]);
                  gtk_widget_destroy(wdata.props[x]);
                  gtk_widget_destroy(wdata.anull[x]);
                  gtk_widget_destroy(wdata.def[x]);
	         }
	    free(wdata.name);
	    free(wdata.type);
	    free(wdata.props);
	    free(wdata.anull);
	    free(wdata.def);
	    /* NOW PROCESS GATHERED INFARMATIONS... */
	    /* HERE CASE WHEN USER WANTS TO CREATE COLUMNS */
	    gtk_dialog_printf("Not implemented!");
	    qptr = NULL;
	    gtk_main_quit();
	    break;
    default:	/* other - finish? */
	    gtk_dialog_printf("Not implemented!");
	    qptr = NULL;
	    gtk_main_quit();
	    return;
   }
}


void init_wdata()
{
 debug("%s:%d init_wdata()", __FILE__,__LINE__);
 ieditor = 0;
 wdata.ncols = 4;
 wdata.label = wdata.label2 = wdata.scale  = wdata.table =
 wdata.text  = wdata.load   = wdata.save   = wdata.hscrl = wdata.vscrl =
 wdata.lname = wdata.ltype  = wdata.lanull = wdata.ldef  = NULL;
 wdata.name  = wdata.type   = wdata.anull  = wdata.def   = NULL;
}


CHAR* ctable_wizard()
{
 debug("%s:%d ctable_wizard()", __FILE__,__LINE__);
 if (qptr) free(qptr);
 qptr = NULL;
 wizstep = 0;
 init_wdata();
 GtkWidget* window, *table, *button_next, *button_prev, *scroller_view, *vbox, *hbox, *label;
 window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
 gtk_widget_set_usize(GTK_WIDGET(window), 800, 600);
 gtk_container_set_border_width(GTK_CONTAINER(window), 5);
 table = gtk_table_new(50, 50, FALSE);
 label = gtk_label_new("Welcome to Create Table Wizard\nthis wizard will guide You\nthrough the rest of Create Table process\nGood luck!");
 gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_FILL);
 gtk_table_attach(GTK_TABLE (table), label, 0, 4, 0, 4, GTK_FILL,GTK_FILL, 0, 0);
 gtk_widget_show(label);
 wdata.label = label;
 gtk_widget_show(table);
 scroller_view = gtk_scrolled_window_new(NULL,NULL);
 gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroller_view), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
 gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scroller_view), table);
 gtk_container_set_border_width(GTK_CONTAINER(scroller_view), 2);
 gtk_scrolled_window_set_placement(GTK_SCROLLED_WINDOW(scroller_view), GTK_CORNER_TOP_RIGHT);
 gtk_widget_show(scroller_view);
 button_prev = gtk_button_new_with_label("Previous");
 gtk_widget_show(button_prev);
 button_next = gtk_button_new_with_label("Next");
 gtk_widget_show(button_next);
 vbox = gtk_vbox_new(FALSE, 1);
 hbox = gtk_hbox_new(FALSE, 1);
 gtk_box_pack_start(GTK_BOX(hbox),button_prev,  TRUE, TRUE, 0);
 gtk_box_pack_start(GTK_BOX(hbox),button_next,  TRUE, TRUE, 0);
 gtk_widget_show(hbox);
 gtk_box_pack_start(GTK_BOX(vbox),scroller_view,  TRUE, TRUE, 0);
 gtk_box_pack_start(GTK_BOX(vbox),hbox, FALSE, FALSE, 0);
 gtk_widget_show(vbox);
 gtk_container_add(GTK_CONTAINER(window), vbox);
 gtk_window_set_title(GTK_WINDOW(window), "Create Table Wizard");
 gtk_window_set_modal(GTK_WINDOW(window), TRUE);
 gtk_widget_show(window);
 gtk_signal_connect(GTK_OBJECT(window), "delete_event", GTK_SIGNAL_FUNC(abort_query),(gpointer)(window));
 gtk_signal_connect(GTK_OBJECT(button_prev), "clicked", GTK_SIGNAL_FUNC(prev_ctable_func), (gpointer)(&table));
 gtk_signal_connect(GTK_OBJECT(button_next), "clicked", GTK_SIGNAL_FUNC(next_ctable_func), (gpointer)(&table));
 wdata.table = table;
 gtk_main();
 destroy_widget(window);
 return qptr;
}


