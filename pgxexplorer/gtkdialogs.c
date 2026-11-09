/* Written by Morgoth DBMA, morgothdbma@o2.pl
 This is part of PgXexplorer software, Open Source
 on BSD licence, Libraries(interaces) used:
 GNU GCC, AS (all stuff needed to compile C source into executable binary)
 LibPQ-FE from PostgreSQL, GTK (GIMP Toolkit)
 written in VIM editor, ctags used, CVS used
 Currently only one author: MOrgoth DBMA
 FILE: gtkdialogs.c */
#include "common.h"
#include "gtkdialogs.h"
static INT answer = ANSWER_YES;
static CHAR tentry[MAX_ENTRY_LENGTH];
static CHAR fmask[MASK_LENGTH];
static CHAR*** ptr = NULL;
static INT row = 0;
static INT col = 0;

INT strhasdot(CHAR* str)
{
 INT l,i;
 debug("%s:%d strhasdot(%s)",__FILE__,__LINE__,str);
 if (!str) return 0;
 l = strlen(str)-1;
 for (i=l;i>=0;i--)
 {
  if (str[i]=='/') break;
  if (str[i]=='\\') break;
  if (str[i]=='.') return 1;
 }
 return 0;
}


void destroy_widget(GtkWidget* w)
{
 debug("%s:%d destroy_widget()",__FILE__,__LINE__);
 if (GTK_IS_WIDGET(w)) gtk_widget_destroy(w);
}


INT mask_match()
{
 CHAR tmps[MAX_ENTRY_LENGTH];
 CHAR tmpm[MASK_LENGTH];
 INT i,j;
 debug("%s:%d mask_match()",__FILE__,__LINE__);
 strcpy(tmps, tentry);
 strcpy(tmpm, fmask);
 downcase(tmps);
 downcase(tmpm);
 i = strlen(tmps)-1;
 j = strlen(tmpm)-1;
 if (tentry[i]=='/') { strcpy(tentry,""); return 0; }
 if (j>i)
   {
    if (!strhasdot(tentry)) { strcat(tentry, fmask); return 1; }
    else return 0;
   }
 while (j>=0)
   {
    if (tmps[i]!=tmpm[j])
      {
       if (!strhasdot(tentry)) { strcat(tentry, fmask); return 1; }
       else return 0;
      }
    j--;
    i--;
   }
 return 1;
}


void destroy_fs(GtkWidget* w, gpointer d)
{
 debug("%s:%d destroy_fs()",__FILE__,__LINE__);
 gtk_main_quit();
 if (!d) return;
 destroy_widget((GtkWidget*)(d));
}


void cancel_fs(GtkWidget* w, gpointer d)
{
 debug("%s:%d cancel_fs()",__FILE__,__LINE__);
 gtk_main_quit();
 if (!d) return;
 destroy_widget((GtkWidget*)(d));
 strcpy(tentry,"");
}


void ok_fs(GtkWidget* w, gpointer d)
{
 debug("%s:%d ok_fs()",__FILE__,__LINE__);
 if (!d) return;
 strcpy(tentry,gtk_file_selection_get_filename((GTK_FILE_SELECTION(d))));
 if (!mask_match()) strcpy(tentry,"");
 gtk_main_quit();
 destroy_widget((GtkWidget*)(d));
}


INT delete_event_proc()
{
 debug("%s:%d delete_event_proc()",__FILE__,__LINE__);
 answer = ANSWER_NO;
 strcpy(tentry, "");
 gtk_main_quit();
 return FALSE;
}


INT delete_combos_proc(GtkWidget* w, gpointer d)
{
 debug("%s:%d delete_combos_proc()",__FILE__,__LINE__);
 answer = ANSWER_NO;
 strcpy(tentry, "");
 gtk_main_quit();
 free_p3c(&ptr, row, col);
 ptr=NULL;
 return TRUE;
}


void ok_proc(GtkWidget* w, gpointer d)
{
 debug("%s:%d ok_proc()",__FILE__,__LINE__);
 gtk_main_quit();
 if (!d) return;
 destroy_widget((GtkWidget*)(d));
}


void ok_combo_proc(GtkWidget* w, gpointer cmb)
{
 debug("%s:%d ok_combo_proc()",__FILE__,__LINE__);
 if (!cmb) return;
 strcpy(tentry,gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(cmb)->entry)));
 gtk_main_quit();
 free_p3c(&ptr, row, col);
 ptr=NULL;
}


void entry_changed_proc(GtkWidget* w, gpointer d)
{
 debug("%s:%d entry_changed_proc()",__FILE__,__LINE__);
 strcpy(tentry,gtk_entry_get_text(GTK_ENTRY(w)));
}


void entry_proc(GtkWidget* w, gpointer d)
{
 debug("%s:%d entry_proc()",__FILE__,__LINE__);
 strcpy(tentry,gtk_entry_get_text(GTK_ENTRY(w)));
 gtk_main_quit();
 if (!d) return;
 destroy_widget((GtkWidget*)(d));
}


void yes_proc(GtkWidget* w, gpointer d)
{
 debug("%s:%d yes_proc()",__FILE__,__LINE__);
 answer = ANSWER_YES;
 gtk_main_quit();
 if (!d) return;
 destroy_widget((GtkWidget*)(d));
}


void return_value(GtkWidget* w, gpointer ptr)
{
 GtkAdjustment* adj;
 struct carrier* car;
 car = (struct carrier*)ptr;
 debug("%s:%d return_value()",__FILE__,__LINE__);
 adj = gtk_range_get_adjustment(GTK_RANGE(&(GTK_SCALE(car->ptr)->range)));
 answer = (INT)(adj->value);
 gtk_main_quit();
 destroy_widget(car->widget);
}


void no_proc(GtkWidget* w, gpointer d)
{
 debug("%s:%d no_proc()",__FILE__,__LINE__);
 answer = ANSWER_NO;
 gtk_main_quit();
 if (!d) return;
 destroy_widget((GtkWidget*)(d));
}


CHAR* gtk_dialog_getfile_printf(const CHAR* mask, const CHAR* fmt, ...)
{
 va_list ap;
 CHAR msg[MAX_MSG_LENGTH];
 debug("%s:%d gtk_dialog_getfile_printf(%s,%s)",__FILE__,__LINE__,mask,fmt);
 if (!fmt) return NULL;
 va_start(ap, fmt);
 vsprintf(msg,fmt,ap);
 va_end(ap);
 return gtk_dialog_getfile(mask, msg);
}


CHAR* gtk_dialog_gettext_printf(const CHAR* fmt, ...)
{
 va_list ap;
 CHAR msg[MAX_MSG_LENGTH];
 debug("%s:%d gtk_dialog_gettext_printf(%s)",__FILE__,__LINE__,fmt);
 if (!fmt) return NULL;
 va_start(ap, fmt);
 vsprintf(msg,fmt,ap);
 va_end(ap);
 return gtk_dialog_gettext(msg);
}


CHAR* gtk_dialog_gettext_big_printf(const CHAR* fmt, ...)
{
 va_list ap;
 CHAR msg[MAX_MSG_LENGTH];
 debug("%s:%d gtk_dialog_gettext_big_printf(%s)",__FILE__,__LINE__,fmt);
 if (!fmt) return NULL;
 va_start(ap, fmt);
 vsprintf(msg,fmt,ap);
 va_end(ap);
 return gtk_dialog_gettext_big(msg);
}


INT gtk_dialog_getnum_printf(INT a, INT b, CHAR* fmt, ...)
{
 va_list ap;
 CHAR msg[MAX_MSG_LENGTH];
 debug("%s:%d gtk_dialog_getnum_printf(%d,%d,%s)",__FILE__,__LINE__,a,b,fmt);
 if (!fmt) return -1;
 va_start(ap, fmt);
 vsprintf(msg,fmt,ap);
 va_end(ap);
 return gtk_dialog_getnum(a,b,msg);
}


INT gtk_dialog_getnum(INT ua, INT ub, CHAR* msg)
{
 GtkWidget *scale, *box, *window, *button;
 GtkObject *adj;
 struct carrier* car;
 INT a,b;
 debug("%s:%d gtk_dialog_getnum(%d,%d,%s)",__FILE__,__LINE__,ua,ub,msg);
 a = (ua<=ub)?ua:ub;
 b = (ua>ub) ?ua:ub;
 if (a==b) return a;
 window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
 gtk_window_set_modal(GTK_WINDOW(window), TRUE);
 gtk_widget_set_usize(window,500,100);
 gtk_window_set_title (GTK_WINDOW(window), msg);
 box = gtk_vbox_new(FALSE, 0);
 gtk_container_add(GTK_CONTAINER(window), box);
 gtk_container_set_border_width(GTK_CONTAINER(box), 10);
 gtk_widget_show(box);
 adj = gtk_adjustment_new(0.0,(gfloat)(a),(gfloat)(b)+1.0, 1.0, 1.0, 1.0);
 scale = gtk_hscale_new(GTK_ADJUSTMENT(adj));
 gtk_scale_set_digits(GTK_SCALE(scale), 0);
 gtk_scale_set_draw_value(GTK_SCALE(scale), TRUE);
 gtk_box_pack_start(GTK_BOX(box), scale, TRUE, TRUE, 0);
 button = gtk_button_new_with_label("OK");
 car = (struct carrier*)malloc(sizeof(struct carrier));
 car->widget = window;
 car->ptr    = (void*)scale;
 gtk_signal_connect (GTK_OBJECT(window), "destroy",GTK_SIGNAL_FUNC(return_value),(gpointer)(car));
 gtk_signal_connect(GTK_OBJECT(button), "clicked",GTK_SIGNAL_FUNC(return_value),(gpointer)(car));
 gtk_widget_show(button);
 gtk_box_pack_start(GTK_BOX(box), button, TRUE, TRUE, 0);
 gtk_widget_show(scale);
 gtk_widget_show(window);
 gtk_main();
 free(car);
 return answer;
}


CHAR* gtk_dialog_getfile(const CHAR* mask, const CHAR* question)
{
 GtkWidget* filew, *flist;
 debug("%s:%d gtk_dialog_getfile(%s,%s)",__FILE__,__LINE__,mask,question);
 strcpy(fmask,"");
 if (!question)  filew = gtk_file_selection_new("NULL");
 if (strlen(mask)>=MASK_LENGTH) { error("mask too long %s:%d", __FILE__,__LINE__); return NULL; }
 else filew = gtk_file_selection_new(question);
 gtk_window_set_modal(GTK_WINDOW(filew), TRUE);
 strcpy(fmask, mask);
 flist = GTK_FILE_SELECTION(filew)->file_list;
 gtk_clist_set_auto_sort(GTK_CLIST(flist), TRUE);
 gtk_signal_connect(GTK_OBJECT(filew), "destroy", GTK_SIGNAL_FUNC(destroy_fs), (gpointer)filew);
 gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(filew)->ok_button),
		 "clicked", GTK_SIGNAL_FUNC(ok_fs), (gpointer)filew);
 gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(filew)->cancel_button),
		 "clicked", GTK_SIGNAL_FUNC(cancel_fs), (gpointer)filew);
 gtk_widget_show(filew);
 gtk_main();
 return tentry;
}


CHAR* gtk_dialog_gettext_big(const CHAR* question)
{
 GtkWidget* dialog, *label, *button, *entry;
 debug("%s:%d gtk_dialog_gettext_big(%s)",__FILE__,__LINE__,question?question:"null");
 if (!question) warn("question=NULL: %s:%d",__FILE__,__LINE__);
 strcpy(tentry, "");
 dialog = gtk_dialog_new();
 gtk_window_set_title(GTK_WINDOW(dialog), "Enter text");
 gtk_widget_set_usize(dialog, 600, 100);
 gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
 label = gtk_label_new(question);
 entry = gtk_entry_new();
 gtk_entry_set_max_length(GTK_ENTRY(entry), MAX_ENTRY_LENGTH-2);
 gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), label, TRUE, TRUE, 0);
 gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), entry, TRUE, TRUE, 0);
 gtk_widget_show(label);
 gtk_widget_show(entry);
 gtk_widget_grab_focus(entry);
 button = gtk_button_new_with_label("OK");
 gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area), button, TRUE, TRUE, 0);
 gtk_signal_connect(GTK_OBJECT(dialog), "delete_event", GTK_SIGNAL_FUNC(delete_event_proc), (gpointer)dialog);
 gtk_signal_connect(GTK_OBJECT(entry), "activate", GTK_SIGNAL_FUNC(entry_proc), (gpointer)dialog);
 gtk_signal_connect(GTK_OBJECT(entry), "changed", GTK_SIGNAL_FUNC(entry_changed_proc), (gpointer)dialog);
 gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(ok_proc), (gpointer)dialog);
 gtk_widget_show(button);
 gtk_widget_show(dialog);
 gtk_main();
 return tentry;
}


CHAR* gtk_dialog_getpass(const CHAR* question)
{
 GtkWidget* dialog, *label, *button, *entry;
 debug("%s:%d gtk_dialog_gettext(%s)",__FILE__,__LINE__,question?question:"null");
 if (!question) warn("question=NULL: %s:%d",__FILE__,__LINE__);
 strcpy(tentry, "");
 dialog = gtk_dialog_new();
 gtk_window_set_title(GTK_WINDOW(dialog), "Enter text");
 gtk_widget_set_usize(dialog, 300, 100);
 gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
 label = gtk_label_new(question);
 entry = gtk_entry_new();
 gtk_entry_set_max_length(GTK_ENTRY(entry), MAX_ENTRY_LENGTH-2);
 gtk_entry_set_visibility(GTK_ENTRY(entry), FALSE);
 gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), label, TRUE, TRUE, 0);
 gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), entry, TRUE, TRUE, 0);
 gtk_widget_show(label);
 gtk_widget_show(entry);
 gtk_widget_grab_focus(entry);
 button = gtk_button_new_with_label("OK");
 gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area), button, TRUE, TRUE, 0);
 gtk_signal_connect(GTK_OBJECT(dialog), "delete_event", GTK_SIGNAL_FUNC(delete_event_proc), (gpointer)dialog);
 gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(ok_proc), (gpointer)dialog);
 gtk_signal_connect(GTK_OBJECT(entry), "activate", GTK_SIGNAL_FUNC(entry_proc), (gpointer)dialog);
 gtk_signal_connect(GTK_OBJECT(entry), "changed", GTK_SIGNAL_FUNC(entry_changed_proc), (gpointer)dialog);
 gtk_widget_show(button);
 gtk_widget_show(dialog);
 gtk_main();
 return tentry;
}


CHAR* gtk_dialog_gettext(const CHAR* question)
{
 GtkWidget* dialog, *label, *button, *entry;
 debug("%s:%d gtk_dialog_gettext(%s)",__FILE__,__LINE__,question?question:"null");
 if (!question) warn("question=NULL: %s:%d",__FILE__,__LINE__);
 strcpy(tentry, "");
 dialog = gtk_dialog_new();
 gtk_window_set_title(GTK_WINDOW(dialog), "Enter text");
 gtk_widget_set_usize(dialog, 300, 100);
 gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
 label = gtk_label_new(question);
 entry = gtk_entry_new();
 gtk_entry_set_max_length(GTK_ENTRY(entry), MAX_ENTRY_LENGTH-2);
 gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), label, TRUE, TRUE, 0);
 gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), entry, TRUE, TRUE, 0);
 gtk_widget_show(label);
 gtk_widget_show(entry);
 gtk_widget_grab_focus(entry);
 button = gtk_button_new_with_label("OK");
 gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area), button, TRUE, TRUE, 0);
 gtk_signal_connect(GTK_OBJECT(dialog), "delete_event", GTK_SIGNAL_FUNC(delete_event_proc), (gpointer)dialog);
 gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(ok_proc), (gpointer)dialog);
 gtk_signal_connect(GTK_OBJECT(entry), "activate", GTK_SIGNAL_FUNC(entry_proc), (gpointer)dialog);
 gtk_signal_connect(GTK_OBJECT(entry), "changed", GTK_SIGNAL_FUNC(entry_changed_proc), (gpointer)dialog);
 gtk_widget_show(button);
 gtk_widget_show(dialog);
 gtk_main();
 return tentry;
}


void gtk_dialog_printf(const CHAR* fmt, ...)
{
 va_list ap;
 CHAR msg[MAX_MSG_LENGTH];
 debug("%s:%d gtk_dialog_printf(%s)",__FILE__,__LINE__,fmt);
 if (!fmt) return ;
 va_start(ap, fmt);
 vsprintf(msg,fmt,ap);
 va_end(ap);
 gtk_dialog_mbox("Information", msg);
 va_end(ap);
}


void gtk_dialog_printf_big_with_title(const CHAR* title, const CHAR* fmt, ...)
{
 GtkWidget* dialog, *label, *button;
 CHAR msg[MAX_MSG_LENGTH];
 va_list ap;
 debug("%s:%d gtk_dialog_printf_big_with_title(%s,%s)",__FILE__,__LINE__,title?title:"null",fmt);
 if (!title) warn("title=NULL: %s:%d",__FILE__,__LINE__);
 va_start(ap,fmt);
 vsprintf(msg,fmt,ap);
 va_end(ap);
 dialog = gtk_dialog_new();
 gtk_window_set_title(GTK_WINDOW(dialog), title);
 gtk_widget_set_usize(dialog, 650, 350);
 gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
 label = gtk_label_new(msg);
 gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), label, TRUE, TRUE, 0);
 gtk_widget_show(label);
 button = gtk_button_new_with_label("OK");
 gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area), button, TRUE, TRUE, 0);
 gtk_signal_connect(GTK_OBJECT(dialog), "delete_event", GTK_SIGNAL_FUNC(delete_event_proc), (gpointer)dialog);
 gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(ok_proc), (gpointer)dialog);
 gtk_widget_grab_focus(button);
 gtk_widget_show(button);
 gtk_widget_show(dialog);
 gtk_main();
}


void gtk_dialog_printf_big(const CHAR* fmt, ...)
{
 GtkWidget* dialog, *label, *button;
 CHAR msg[MAX_MSG_LENGTH];
 va_list ap;
 debug("%s:%d gtk_dialog_big_printf(%s)",__FILE__,__LINE__,fmt);
 if (!fmt) return ;
 va_start(ap,fmt);
 vsprintf(msg,fmt,ap);
 va_end(ap);
 dialog = gtk_dialog_new();
 gtk_window_set_title(GTK_WINDOW(dialog), "Long information");
 gtk_widget_set_usize(dialog, 650, 350);
 gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
 label = gtk_label_new(msg);
 gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), label, TRUE, TRUE, 0);
 gtk_widget_show(label);
 button = gtk_button_new_with_label("OK");
 gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area), button, TRUE, TRUE, 0);
 gtk_signal_connect(GTK_OBJECT(dialog), "delete_event", GTK_SIGNAL_FUNC(delete_event_proc), (gpointer)dialog);
 gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(ok_proc), (gpointer)dialog);
 gtk_widget_grab_focus(button);
 gtk_widget_show(button);
 gtk_widget_show(dialog);
 gtk_main();
}


void gtk_dialog_printf_with_title(const CHAR* title, const CHAR* fmt, ...)
{
 GtkWidget* dialog, *label, *button;
 CHAR msg[MAX_MSG_LENGTH];
 va_list ap;
 debug("%s:%d gtk_dialog_printf_with_title(%s,%s)",__FILE__,__LINE__,title,fmt);
 if (!fmt) return;
 va_start(ap,fmt);
 vsprintf(msg,fmt,ap);
 va_end(ap);
 dialog = gtk_dialog_new();
 gtk_window_set_title(GTK_WINDOW(dialog), title);
 gtk_widget_set_usize(dialog, 400, 150);
 gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
 label = gtk_label_new(msg);
 gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), label, TRUE, TRUE, 0);
 gtk_widget_show(label);
 button = gtk_button_new_with_label("OK");
 gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area), button, TRUE, TRUE, 0);
 gtk_signal_connect(GTK_OBJECT(dialog), "delete_event", GTK_SIGNAL_FUNC(delete_event_proc), (gpointer)dialog);
 gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(ok_proc), (gpointer)dialog);
 gtk_widget_grab_focus(button);
 gtk_widget_show(button);
 gtk_widget_show(dialog);
 gtk_main();
}


void gtk_dialog_mbox(const CHAR* title, const CHAR* msg)
{
 GtkWidget* dialog, *label, *button;
 debug("%s:%d gtk_dialog_mbox(%s,%s)",__FILE__,__LINE__,title,msg);
 if (!msg || !title) return;
 dialog = gtk_dialog_new();
 gtk_window_set_title(GTK_WINDOW(dialog), title);
 gtk_widget_set_usize(dialog, 400, 150);
 gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
 label = gtk_label_new(msg);
 gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), label, TRUE, TRUE, 0);
 gtk_widget_show(label);
 button = gtk_button_new_with_label("OK");
 gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area), button, TRUE, TRUE, 0);
 gtk_signal_connect(GTK_OBJECT(dialog), "delete_event", GTK_SIGNAL_FUNC(delete_event_proc), (gpointer)dialog);
 gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(ok_proc), (gpointer)dialog);
 gtk_widget_grab_focus(button);
 gtk_widget_show(button);
 gtk_widget_show(dialog);
 gtk_main();
}

INT gtk_dialog_yes_no_printf_big(const CHAR* fmt, ...)
{
 GtkWidget* dialog, *label, *button_yes, *button_no;
 CHAR question[MAX_MSG_LENGTH];
 va_list ap;
 debug("%s:%d gtk_dialog_yes_no_printf(%s)",__FILE__,__LINE__,fmt);
 if (!fmt) return 0;
 va_start(ap,fmt);
 vsprintf(question,fmt,ap);
 va_end(ap);
 dialog = gtk_dialog_new();
 gtk_window_set_title(GTK_WINDOW(dialog), "Confirmation needed.");
 gtk_widget_set_usize(dialog, 680, 400);
 gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
 label = gtk_label_new(question);
 gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), label, TRUE, TRUE, 0);
 gtk_widget_show(label);
 button_yes = gtk_button_new_with_label("Yes");
 button_no  = gtk_button_new_with_label("No");
 gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area), button_yes, TRUE, TRUE, 0);
 gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area), button_no, TRUE, TRUE, 0);
 gtk_signal_connect(GTK_OBJECT(dialog), "delete_event", GTK_SIGNAL_FUNC(delete_event_proc), (gpointer)dialog);
 gtk_signal_connect(GTK_OBJECT(button_yes), "clicked", GTK_SIGNAL_FUNC(yes_proc), (gpointer)dialog);
 gtk_signal_connect(GTK_OBJECT(button_no),  "clicked", GTK_SIGNAL_FUNC(no_proc), (gpointer)dialog);
 gtk_widget_grab_focus(button_yes);
 gtk_widget_show(button_yes);
 gtk_widget_show(button_no);
 gtk_widget_show(dialog);
 gtk_main();
 return (answer==ANSWER_YES)?1:0;
}


INT gtk_dialog_yes_no_printf(const CHAR* fmt, ...)
{
 GtkWidget* dialog, *label, *button_yes, *button_no;
 CHAR question[MAX_MSG_LENGTH];
 va_list ap;
 debug("%s:%d gtk_dialog_yes_no_printf(%s)",__FILE__,__LINE__,fmt);
 if (!fmt) return 0;
 va_start(ap,fmt);
 vsprintf(question,fmt,ap);
 va_end(ap);
 dialog = gtk_dialog_new();
 gtk_window_set_title(GTK_WINDOW(dialog), "Confirmation needed.");
 gtk_widget_set_usize(dialog, 400, 150);
 gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
 label = gtk_label_new(question);
 gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), label, TRUE, TRUE, 0);
 gtk_widget_show(label);
 button_yes = gtk_button_new_with_label("Yes");
 button_no  = gtk_button_new_with_label("No");
 gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area), button_yes, TRUE, TRUE, 0);
 gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area), button_no, TRUE, TRUE, 0);
 gtk_signal_connect(GTK_OBJECT(dialog), "delete_event", GTK_SIGNAL_FUNC(delete_event_proc), (gpointer)dialog);
 gtk_signal_connect(GTK_OBJECT(button_yes), "clicked", GTK_SIGNAL_FUNC(yes_proc), (gpointer)dialog);
 gtk_signal_connect(GTK_OBJECT(button_no),  "clicked", GTK_SIGNAL_FUNC(no_proc), (gpointer)dialog);
 gtk_widget_grab_focus(button_yes);
 gtk_widget_show(button_yes);
 gtk_widget_show(button_no);
 gtk_widget_show(dialog);
 gtk_main();
 return (answer==ANSWER_YES)?1:0;
}


INT gtk_dialog_with_2_buttons(const CHAR* question, CHAR* but1, CHAR* but2)
{
 GtkWidget* dialog, *label, *button_yes, *button_no;
 debug("%s:%d gtk_dialog_with_2_buttons()",__FILE__,__LINE__);
 if (!question || !but1 || !but2) { warn("parameters! %s:%d", __FILE__,__LINE__); return 0; }
 dialog = gtk_dialog_new();
 gtk_window_set_title(GTK_WINDOW(dialog), "Confirmation needed.");
 gtk_widget_set_usize(dialog, 400, 150);
 gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
 label = gtk_label_new(question);
 gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), label, TRUE, TRUE, 0);
 gtk_widget_show(label);
 button_yes = gtk_button_new_with_label(but1);
 button_no  = gtk_button_new_with_label(but2);
 gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area), button_yes, TRUE, TRUE, 0);
 gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area), button_no, TRUE, TRUE, 0);
 gtk_signal_connect(GTK_OBJECT(dialog), "delete_event", GTK_SIGNAL_FUNC(delete_event_proc), (gpointer)dialog);
 gtk_signal_connect(GTK_OBJECT(button_yes), "clicked", GTK_SIGNAL_FUNC(yes_proc), (gpointer)dialog);
 gtk_signal_connect(GTK_OBJECT(button_no),  "clicked", GTK_SIGNAL_FUNC(no_proc), (gpointer)dialog);
 gtk_widget_grab_focus(button_yes);
 gtk_widget_show(button_yes);
 gtk_widget_show(button_no);
 gtk_widget_show(dialog);
 gtk_main();
 return (answer==ANSWER_YES)?1:0;
}


INT gtk_dialog_yes_no(const CHAR* question)
{
 GtkWidget* dialog, *label, *button_yes, *button_no;
 debug("%s:%d gtk_dialog_yes_no(%s)",__FILE__,__LINE__,question);
 if (!question) { warn("question=NULL, %s:%d", __FILE__,__LINE__); return 0; }
 dialog = gtk_dialog_new();
 gtk_window_set_title(GTK_WINDOW(dialog), "Confirmation needed.");
 gtk_widget_set_usize(dialog, 400, 150);
 gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
 label = gtk_label_new(question);
 gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), label, TRUE, TRUE, 0);
 gtk_widget_show(label);
 button_yes = gtk_button_new_with_label("Yes");
 button_no  = gtk_button_new_with_label("No");
 gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area), button_yes, TRUE, TRUE, 0);
 gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area), button_no, TRUE, TRUE, 0);
 gtk_signal_connect(GTK_OBJECT(dialog), "delete_event", GTK_SIGNAL_FUNC(delete_event_proc), (gpointer)dialog);
 gtk_signal_connect(GTK_OBJECT(button_yes), "clicked", GTK_SIGNAL_FUNC(yes_proc), (gpointer)dialog);
 gtk_signal_connect(GTK_OBJECT(button_no),  "clicked", GTK_SIGNAL_FUNC(no_proc), (gpointer)dialog);
 gtk_widget_grab_focus(button_yes);
 gtk_widget_show(button_yes);
 gtk_widget_show(button_no);
 gtk_widget_show(dialog);
 gtk_main();
 return (answer==ANSWER_YES)?1:0;
}


INT setup_combo(PGconn* c, GtkWidget* w, GtkWidget** cmb)
{
 CHAR errstr[MAX_QUERY_LENGTH];
 INT i;
 GList* list;
 debug("%s:%d setup_combo()",__FILE__,__LINE__);
 list = NULL;
 if (!c || !w) error("setup_combo, %s:%d",__FILE__,__LINE__);
 ptr=p3c_execute_printf(c,&row,&col,errstr, 0, "SELECT datname FROM pg_database");
 if (col<0 || row<0)
 {
  gtk_dialog_printf("%s", errstr);
  cmb=NULL;
  return -1;
 }
/* list = g_list_append(list," Select DB");*/
 for (i=0;i<row;i++) list = g_list_append(list, ptr[i][0]);
 /*free_p3c(&ptr,a,b);*/
 *cmb = gtk_combo_new();
 gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(*cmb)->entry),"Select DB from list");
 gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(*cmb)->entry), FALSE);
 gtk_combo_set_popdown_strings(GTK_COMBO(*cmb), list);
 g_list_free(list);
 //free_p3c(&ptr, row, col);	//XXX can we free it? WE CANT
 				//will be freed later
 return 0;
}


CHAR* gtk_dialog_getdbs(PGconn* connection, CHAR* fmt, ...)
{
 GtkWidget* dialog, *combo, *button, *label;
 CHAR msg[MAX_MSG_LENGTH];
 va_list ap;
 debug("%s:%d gtk_dialog_getdbs(%s)",__FILE__,__LINE__,fmt);
 if (!connection || !fmt) error("gtk_getdbs, params: %s:%d", __FILE__,__LINE__);
 va_start(ap,fmt);
 vsprintf(msg,fmt,ap);
 va_end(ap);
 strcpy(tentry,"");
 dialog = gtk_dialog_new();
 gtk_window_set_title(GTK_WINDOW(dialog),msg);
 gtk_widget_set_usize(dialog, 500, 80);
 gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
 label = gtk_label_new("Select One:");
 gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), label, TRUE, TRUE, 0);
 gtk_widget_show(label);
 button = gtk_button_new_with_label("Select");
 combo=NULL;
 if (setup_combo(connection, dialog, &combo)==-1) return NULL;
 gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area), combo, TRUE, TRUE, 0);
 gtk_widget_show(combo);
 gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area), button, TRUE, TRUE, 0);
 gtk_signal_connect(GTK_OBJECT(dialog), "delete_event", GTK_SIGNAL_FUNC(delete_combos_proc),(gpointer)dialog);
 gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(ok_combo_proc), (gpointer)combo);
 gtk_widget_grab_focus(combo);
 gtk_widget_show(button);
 gtk_widget_show(dialog);
 gtk_main();
 destroy_widget(dialog);
 return tentry;
}

