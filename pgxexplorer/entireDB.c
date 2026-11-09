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
#include "common.h"
#include "entireDB.h"
extern struct main_components components;
extern INT connected;
extern PGconn* connection;
extern INT want_all_elems;
extern INT max_tabs;
extern INT cont;
extern CHAR* currentbase;
struct ProgressData *pdata;

GtkWidget* construct_view_table(CHAR* tabname)
{
 CHAR*** ptr;
 INT row,col;
 CHAR errstr[MAX_QUERY_LENGTH+100];
 struct result_cache res_cache;
#ifdef SMALLOC
 struct dmem_list *memory;
#else
 struct mem_list *memory;
#endif
 INT i,j,orow,ocol;
 GtkWidget*** table_entry;
 GtkWidget* table_view;
 GtkWidget* scroller_view;
 debug("%s:%d construct_table_view(%s)",__FILE__,__LINE__,tabname);
 debug("construct_table_view: %s", tabname);
 memory = NULL;
 if (!connection) { error("no_connection %s:%d", __FILE__,__LINE__); return NULL; }
#ifdef SMALLOC
 dmem_execute_printf(&memory, connection, &ptr, &row, &col, errstr, 1, HERE, "SELECT * FROM %s", tabname);
#else
 mem_execute_printf(&memory, connection, &ptr, &row, &col, errstr, 1, "SELECT * FROM %s", tabname);
#endif
 if (row<0 || col<0) { debug("errstr=%s", errstr); return NULL;	} /* FIXME SHOULDN'T BE THERE DIALOG ?? */
 res_cache.cols = col;
 res_cache.rows = row;
 res_cache.data = ptr;
 table_entry = (GtkWidget***)malloc(row*sizeof(GtkWidget**));
 for (i=0;i<row;i++) table_entry[i] = (GtkWidget**)malloc(col*sizeof(GtkWidget*));
 orow=row;
 ocol=col;
 if (col>32 || row>256)
   {
    if (row>256) col = (col>8 )?8 :col;
    else if (row>128)  col = (col>16 )?16 :col;
    else if (row>64)  col = (col>32 )?32 :col;
    else             col = (col>48)?48:col;
    row = (row>256)?256:row;
    col = (col>256)?256:col;
   }
 debug("row=%d, col=%d\n",row,col);
 table_view = gtk_table_new(row, col, FALSE);
 for (i=0;i<row;i++)
 for (j=0;j<col;j++)
   {
    table_entry[i][j] = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(table_entry[i][j]), ptr[i][j]);
    gtk_entry_set_editable(GTK_ENTRY(table_entry[i][j]), FALSE);
    gtk_table_attach (GTK_TABLE (table_view), table_entry[i][j], j, j+1, i, i+1,GTK_FILL,GTK_FILL, 0, 0);
    gtk_widget_show(table_entry[i][j]);
   }
 gtk_widget_show(table_view);
 scroller_view = gtk_scrolled_window_new(NULL,NULL);
 gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroller_view), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
 gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scroller_view), table_view);
 gtk_container_set_border_width(GTK_CONTAINER(scroller_view), 1);
 gtk_scrolled_window_set_placement(GTK_SCROLLED_WINDOW(scroller_view), GTK_CORNER_TOP_RIGHT);
 gtk_widget_show(scroller_view);
#ifdef SMALLOC
 dmem_free_p3c(&memory, &ptr, orow, ocol, HERE);
 dshred_special_memlist(&memory, HERE);
#else
 mem_free_p3c(&memory, &ptr, orow, ocol);
 shred_special_memlist(&memory);
#endif
 return scroller_view;
}


INT drop_control(GtkWidget* w, gpointer data)
{
 debug("%s:%d drop_control()",__FILE__,__LINE__);
 create_pbar("freeing memory...");
 gtk_main_quit();
 return FALSE;
}


void rotate_book(GtkButton *button,GtkNotebook *notebook)
{
 debug("%s:%d rotate_book()",__FILE__,__LINE__);
 gtk_notebook_set_tab_pos(notebook,(notebook->tab_pos+1)%4);
}


void write_html_header(FILE* f)
{
 debug("%s:%d write_html_header()",__FILE__,__LINE__);
 fprintf(f,"<HTML>\n");
 fprintf(f,"<TITLE>\n");
 fprintf(f,"Results\n");
 fprintf(f,"</TITLE>\n");
 fprintf(f,"<HEAD>\n");
 fprintf(f,"</HEAD>\n");
 fprintf(f,"<BODY>\n");
}


void write_html_end(FILE* f)
{
 debug("%s:%d write_html_end()",__FILE__,__LINE__);
 fprintf(f,"</BODY>\n");
 fprintf(f,"</HTML>\n");
}


INT save_table(FILE* f, CHAR** tables, INT page, INT ntabs, INT fast)
{
 INT start,want_meta,i,j,row,col;
 CHAR errstr[MAX_QUERY_LENGTH+100];
 CHAR*** ptr;
#ifdef SMALLOC
 struct dmem_list* mem;
#else
 struct mem_list* mem;
#endif
 mem = NULL;
 debug("%s:%d save_table(%p,%p,%d,%d,%d)",__FILE__,__LINE__,f,tables,page,ntabs,fast);
 if (!f) { gtk_dialog_printf("Cannot write to NULL file!"); return 0; }
 if (ntabs<0 || page<0 || page>=ntabs) { gtk_dialog_printf("Cannot save page: %d of %d", page, ntabs); return 0; }
#ifdef SMALLOC
 dmem_execute_printf(&mem, connection, &ptr, &row, &col, errstr, 1, HERE, "SELECT * FROM %s", tables[page]);
#else
 mem_execute_printf(&mem, connection, &ptr, &row, &col, errstr, 1, "SELECT * FROM %s", tables[page]);
#endif
 if (row<0 || col<0)
   {
    gtk_dialog_printf_big("Error executing query:\nReturn from DBMS: %s\n", errstr);
    return 0;
   }
 start=0;
 if (!fast) want_meta = gtk_dialog_yes_no_printf("Do You want to save table names?");
 else want_meta = 1;
 if (!want_meta) start=1;
 if (!fast) write_html_header(f);
 fprintf(f,"<TABLE BORDER=\"3\">\n");
 for (i=start;i<row;i++)
   {
    fprintf(f,"<TR>\n");
    for (j=0;j<col;j++) fprintf(f,"<TD>%s</TD>\n", ptr[i][j]);
   }
 if (!fast) write_html_end(f);
 fprintf(f,"</BODY>\n");
 fprintf(f,"</HTML>\n");
 if (!fast) fclose(f);
#ifdef SMALLOC
 dmem_free_p3c(&mem, &ptr, row, col, HERE);
 dshred_special_memlist(&mem, HERE);
#else
 mem_free_p3c(&mem, &ptr, row, col);
 shred_special_memlist(&mem);
#endif
 return 1;
}


void save_entire_book(GtkButton *button, GtkNotebook *notebook)
{
#ifdef SMALLOC
 struct dmem_list* memory;
#else
 struct mem_list* memory;
#endif
 CHAR** tables;
 CHAR* filen;
 INT ntabs,a,i;
 void* tmp;
 FILE* f;
 debug("%s:%d save_entire_book()",__FILE__,__LINE__);
 tmp=NULL;
 memory = NULL;
#ifdef SMALLOC
 if (!dmem_get_db_tables(&memory, connection,&tables, &ntabs, want_all_elems,HERE))
#else
 if (!mem_get_db_tables(&memory, connection,&tables, &ntabs, want_all_elems))
#endif
    {
     gtk_dialog_printf("Cannot get DB tables list!\n");
     return;
    }
 if (ntabs<=0) { gtk_dialog_printf("No tables detected!"); return; }
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
 write_html_header(f);
 fprintf(f,"<CENTER><BIG><BIG> DataBase: %s </BIG></BIG></CENTER>\n", currentbase);
 for (i=0;i<ntabs;i++)
   {
    fprintf(f, "<CENTER><BIG>table: %s</BIG></CENTER>\n", tables[i]);
    if (!save_table(f, tables, i, ntabs, 1)) break;
   }
 write_html_end(f);
 fclose(f);
#ifdef SMALLOC
 for (i=0;i<ntabs;i++) if (tables[i]) { dsfree(&memory, (void**)(&tables[i]), HERE); tables[i] = NULL; }
 if (tables) { dsfree(&memory, (void**)(&tables), HERE); tables = NULL; }
 dshred_special_memlist(&memory, HERE);
#else
 for (i=0;i<ntabs;i++) if (tables[i]) { sfree(&memory, (void**)(&tables[i])); tables[i] = NULL; }
 if (tables)
   {
    tmp = (void*)tables;
    sfree(&memory, &tmp);
    tables = NULL;
   }
 shred_special_memlist(&memory);
#endif
}


void save_table_book(GtkButton *button, GtkNotebook *notebook)
{
 INT page;
#ifdef SMALLOC
 struct dmem_list* memory;
#else
 struct mem_list* memory;
#endif
 CHAR** tables;
 CHAR* filen;
 void* tmp;
 INT ntabs,a,i;
 FILE* f;
 debug("%s:%d save_table_book()",__FILE__,__LINE__);
 memory = NULL;
 tmp=NULL;
 page = gtk_notebook_get_current_page(notebook);
 debug("PAGE=%d\n",page);
#ifdef SMALLOC
 if (!dmem_get_db_tables(&memory, connection,&tables, &ntabs, want_all_elems, HERE))
#else
 if (!mem_get_db_tables(&memory, connection,&tables, &ntabs, want_all_elems))
#endif
    {
     gtk_dialog_printf("Cannot get DB tables list!\n");
     return;
    }
 if (ntabs<=0) { gtk_dialog_printf("No tables detected!"); return; }
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
 save_table(f, tables, page, ntabs, 0);
 fclose(f);
#ifdef SMALLOC
 for (i=0;i<ntabs;i++) if (tables[i]) { dsfree(&memory, (void**)(&tables[i]), HERE); tables[i] = NULL; }
 if (tables) { dsfree(&memory, (void**)(&tables), HERE); tables = NULL; }
 dshred_special_memlist(&memory, HERE);
#else
 for (i=0;i<ntabs;i++) if (tables[i]) { sfree(&memory, (void**)(&tables[i])); tables[i] = NULL; }
 if (tables)
   {
    tmp = (void*)tables;
    sfree(&memory, &tmp);
    tables = NULL;
   }
 shred_special_memlist(&memory);
#endif
}


void tabsborder_book(GtkButton *button, GtkNotebook *notebook)
{
 INT tval = FALSE;
 INT bval = FALSE;
 debug("%s:%d tabsborder_book()",__FILE__,__LINE__);
 if (notebook->show_tabs == 0)   tval = TRUE;
 if (notebook->show_border == 0) bval = TRUE;
 gtk_notebook_set_show_tabs(notebook, tval);
 gtk_notebook_set_show_border(notebook, bval);
}


void create_pbar(CHAR* arg)
{
 debug("%s:%d create_pbar(%s)",__FILE__,__LINE__,arg);
 cont=1;
 system_printf("pbar %d '%s' 1>/dev/null 2>/dev/null &", getpid(), arg);
}


void destroy_pbar()
{
 debug("%s:%d destroy_pbar()",__FILE__,__LINE__);
 system("killall -9 pbar 1>/dev/null 2>/dev/null");
 cont=1;
}


void cmenu_browse_all(GtkWidget* w, gpointer data)
{
 GtkWidget *window;
 GtkWidget *button;
 GtkWidget *table;
 GtkWidget *notebook;
 GtkWidget *frame;
 GtkWidget *label;
 GtkWidget *scroller;
 GtkWidget *vtable;
#ifdef SMALLOC
 struct dmem_list* memoryl;
#else
 struct mem_list* memoryl;
#endif
 CHAR** tables;
 void*  tmp;
 INT ntabs;
 INT i;
 debug("%s:%d cmenu_browse_all()",__FILE__,__LINE__);
 memoryl=NULL;
 tmp=NULL;
 if (!connected)  { gtk_dialog_printf("Not connected to DB!"); return; }
#ifdef SMALLOC
 if (!dmem_get_db_tables(&memoryl, connection,&tables, &ntabs, want_all_elems, HERE))
#else
 if (!mem_get_db_tables(&memoryl, connection,&tables, &ntabs, want_all_elems))
#endif
   {
    gtk_dialog_printf("Cannot get DB tables list!\n");
    return;
   }
 if (ntabs<=0) { gtk_dialog_printf("No tables detected!"); return; }
 window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
 gtk_widget_set_usize(window, 800,600);
 gtk_window_set_title (GTK_WINDOW(window), "Preview of DB (results will be truncated if too long)");
 gtk_window_set_modal(GTK_WINDOW(window), TRUE);
 table = gtk_table_new(8,10,FALSE);
 notebook = gtk_notebook_new();
 gtk_notebook_set_tab_pos(GTK_NOTEBOOK (notebook), GTK_POS_TOP);
 scroller = gtk_scrolled_window_new(NULL,NULL);
 gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroller), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
 gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scroller), notebook);
 gtk_container_set_border_width(GTK_CONTAINER(scroller), 2);
 gtk_widget_show(scroller);
 gtk_container_set_border_width (GTK_CONTAINER (window), 1);
 gtk_table_attach(GTK_TABLE(table), scroller, 0,8,0,9, GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND,0,0);
 gtk_widget_show(notebook);
 gtk_signal_connect (GTK_OBJECT (window), "delete_event",GTK_SIGNAL_FUNC(drop_control), (gpointer)(window));
 gtk_container_add (GTK_CONTAINER(window), table);
 if (ntabs>max_tabs) ntabs=max_tabs;
 create_pbar("Wait while filling controls...");
 for (i=0;i<ntabs;i++)
     {
      debug("TABLES[%d] = %s\n", i, tables[i]);
      frame = gtk_frame_new (tables[i]);
      gtk_container_set_border_width (GTK_CONTAINER(frame),2);
      gtk_widget_set_usize (frame, 100, 25);
      gtk_widget_show(frame);
      label = gtk_label_new(tables[i]);
      gtk_notebook_append_page(GTK_NOTEBOOK(notebook), frame, label);
      vtable = construct_view_table(tables[i]);
      if (vtable) gtk_container_add(GTK_CONTAINER(frame), construct_view_table(tables[i]));
      else gtk_container_add(GTK_CONTAINER(frame), gtk_label_new("Connot read table!"));
      if (!cont)
        {
         gtk_window_set_title (GTK_WINDOW(window), "Preview of DB, incomplete - interrupted!");
         gtk_dialog_printf("Interrupted!");
	 break;
	}
    }
 destroy_pbar();
/* git(tk_notebook_set_page (GTK_NOTEBOOK(notebook), 3);*/
 button = gtk_button_new_with_label ("Close");
 gtk_signal_connect_object (GTK_OBJECT (button), "clicked",GTK_SIGNAL_FUNC (drop_control), NULL);
 gtk_table_attach(GTK_TABLE(table), button, 0,1,9,10,GTK_FILL|GTK_EXPAND,GTK_FILL,0,0);
 gtk_widget_show(button);
 button = gtk_button_new_with_label ("Next Table");
 gtk_signal_connect_object (GTK_OBJECT (button), "clicked",(GtkSignalFunc)gtk_notebook_next_page,GTK_OBJECT (notebook));
 gtk_table_attach(GTK_TABLE(table), button, 1,2,9,10,GTK_FILL|GTK_EXPAND,GTK_FILL,0,0);
 gtk_widget_show(button);
 button = gtk_button_new_with_label ("Previous Table");
 gtk_signal_connect_object (GTK_OBJECT (button), "clicked",(GtkSignalFunc)gtk_notebook_prev_page,GTK_OBJECT (notebook));
 gtk_table_attach(GTK_TABLE(table), button, 2,3,9,10,GTK_FILL|GTK_EXPAND,GTK_FILL,0,0);
 gtk_widget_show(button);
 button = gtk_button_new_with_label ("Tab Position");
 gtk_signal_connect (GTK_OBJECT (button), "clicked",(GtkSignalFunc)rotate_book,GTK_OBJECT(notebook));
 gtk_table_attach(GTK_TABLE(table), button, 3,4,9,10,GTK_FILL|GTK_EXPAND,GTK_FILL,0,0);
 gtk_widget_show(button);
 button = gtk_button_new_with_label ("Border on/off");
 gtk_signal_connect (GTK_OBJECT (button), "clicked",(GtkSignalFunc)tabsborder_book,GTK_OBJECT (notebook));
 gtk_table_attach(GTK_TABLE(table), button, 4,5,9,10,GTK_FILL|GTK_EXPAND,GTK_FILL,0,0);
 gtk_widget_show(button);
 button = gtk_button_new_with_label("Save entire DB");
 gtk_signal_connect(GTK_OBJECT (button), "clicked",(GtkSignalFunc)save_entire_book,GTK_OBJECT(notebook));
 gtk_table_attach(GTK_TABLE(table), button, 5,6,9,10,GTK_FILL|GTK_EXPAND,GTK_FILL,0,0);
 gtk_widget_show(button);
 button = gtk_button_new_with_label("Save this table");
 gtk_signal_connect(GTK_OBJECT (button), "clicked", (GtkSignalFunc)save_table_book,GTK_OBJECT(notebook));
 gtk_table_attach(GTK_TABLE(table), button, 6,7,9,10,GTK_FILL|GTK_EXPAND,GTK_FILL,0,0);
 gtk_widget_show(button);
 gtk_widget_show(table);
 gtk_widget_show(window);
/* gtk_window_set_title (GTK_WINDOW(window), "Preview of DB");*/
 gtk_main();
 if (GTK_IS_WIDGET(window)) gtk_widget_destroy(window);
#ifdef SMALLOC
 for (i=0;i<ntabs;i++) if (tables[i]) { dsfree(&memoryl, (void**)(&tables[i]), HERE); tables[i]=NULL; }
 if (tables)
   { dsfree(&memoryl, (void**)(&tables), HERE); tables=NULL; }
 dshred_special_memlist(&memoryl, HERE);
#else
 for (i=0;i<ntabs;i++) if (tables[i])
   {
    sfree(&memoryl, (void**)(&tables[i]));
    tables[i]=NULL;
   }
 if (tables)
   {
    tmp = (void*)tables;
    sfree(&memoryl, &tmp);
    tables=NULL;
   }
 shred_special_memlist(&memoryl);
#endif
 destroy_pbar();
}

