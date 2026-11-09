/* Written by Morgoth DBMA, morgothdbma@o2.pl
 This is part of PgXexplorer software, Open Source
 on BSD licence, Libraries(interaces) used:
 GNU GCC, AS (all stuff needed to compile C source into executable binary)
 LibPQ-FE from PostgreSQL, GTK (GIMP Toolkit)
 written in VIM editor, ctags used, CVS used
 Currently only one author: MOrgoth DBMA
 FILE: mmsystem.c */
#include "mmsystem.h"
#include "common.h"
struct dmem_list* dmlist= NULL;
struct mem_list*  mlist = NULL;				/* ALWAYS INITIALIZE WITH NULL, OR WILL BE TROUBLE !!!! */

void add_to_special_memlist(struct mem_list** head, void* ptr)
{
 struct mem_list* newh;
 sdebug("%s:%d add_to_special_memlist(%p, %p)",__FILE__,__LINE__, *head, ptr);
 if (!(*head))
   {
    *head = (struct mem_list*)malloc(sizeof(struct mem_list));
    if (!(*head)) { fatal("smalloc failed!"); return; }
    (*head)->ptr  = ptr;
    (*head)->next = NULL;
    return;
   }
 newh = (struct mem_list*)malloc(sizeof(struct mem_list));
 if (!newh) { fatal("smalloc failed!"); return; }
 newh->ptr   = ptr;
 newh->next  = *head;
 *head       = newh;
}


void print_special_memlist(struct mem_list* head)
{
#ifdef NO_TRACE_MEM
 return;
#else
 struct mem_list* tmp;
 sdebug("%s:%d print_special_memlist(%p)",__FILE__,__LINE__, head);
 tmp = head;
 debug("MEMLIST_SPECIAL:\n");
 while (tmp)
   {
    debug("%p\n", tmp->ptr);
    tmp = tmp->next;
   }
 debug("MEMLIST_SPECIAL END\n");
#endif
}


void del_from_special_memlist(struct mem_list** head, void* ptr)
{
 struct mem_list *tmp, *prev;
 sdebug("%s:%d del_from_special_memlist(%p, %p)",__FILE__,__LINE__, *head,ptr);
 tmp = prev = *head;
 while (tmp)
   {
    if (tmp->ptr==ptr)
      {
       if (tmp==(*head))
         {
          *head = tmp->next;
	  free(tmp);
	  tmp = NULL;
	  return ;
         }
       prev->next = tmp->next;
       free(tmp);
       tmp = NULL;
       return ;
      }
    prev = tmp;
    tmp = tmp->next;
   }
 fatal("SPECIAL_MEMLIST: No such pointer: %p", ptr);
}


void add_to_memlist(void* ptr)
{
 struct mem_list* newh;
 sdebug("%s:%d add_to_memlist(%p)",__FILE__,__LINE__, ptr);
 if (!mlist)
   {
    mlist = (struct mem_list*)malloc(sizeof(struct mem_list));
    if (!mlist) { fatal("malloc mlist failed!"); return; }
    mlist->ptr  = ptr;
    mlist->next = NULL;
    return;
   }
 newh = (struct mem_list*)malloc(sizeof(struct mem_list));
 if (!newh) { fatal("malloc mlist failed!"); return; }
 newh->ptr   = ptr;
 newh->next  = mlist;
 mlist       = newh;
}


void print_memlist()
{
#ifdef NO_TRACE_MEM
 return ;
#else
 struct mem_list* tmp;
 sdebug("%s:%d print_memlist()",__FILE__,__LINE__);
 tmp = mlist;
 debug("MEMLIST:\n");
 while (tmp)
   {
    debug("%p\n", tmp->ptr);
    tmp = tmp->next;
   }
 debug("MEMLIST END\n");
#endif
}


void del_from_memlist(void* ptr)
{
 struct mem_list *tmp, *prev;
 sdebug("%s:%d del_from_memlist(%p)",__FILE__,__LINE__,ptr);
 tmp = prev = mlist;
 while (tmp)
   {
    if (tmp->ptr==ptr)
      {
       if (tmp==mlist)
         {
          mlist = tmp->next;
	  free(tmp);
	  tmp = NULL;
	  return ;
         }
       prev->next = tmp->next;
       free(tmp);
       tmp = NULL;
       return ;
      }
    prev = tmp;
    tmp = tmp->next;
   }
 fatal("MEMLIST: No such pointer: %p", ptr);
}


void shred_memlist()
{
#ifdef NO_TRACE_MEM
 return;
#else
 struct mem_list *tmp, *prev;
 sdebug("%s:%d shred_memlist()",__FILE__,__LINE__);
 tmp = prev = mlist;
 while (tmp)
   {
    prev = tmp;
    tmp = tmp->next;
    free(prev);
    prev = NULL;
   }
 mlist = NULL;
#endif
}


void shred_special_memlist(struct mem_list** head)
{
#ifdef NO_TRACE_MEM
 return ;
#else
 struct mem_list *tmp, *prev;
 sdebug("%s:%d shred_special_memlist(%p)",__FILE__,__LINE__,*head);
 tmp = prev = *head;
 while (tmp)
   {
    prev = tmp;
    tmp = tmp->next;
    free(prev);
    prev = NULL;
   }
 *head = NULL;
#endif
}


void* mmalloc(INT siz)
{
#ifdef NO_TRACE_MEM
 return malloc(siz);
#else
 void* ptr;
 sdebug("%s:%d mmalloc(%d)",__FILE__,__LINE__,siz);
 if (siz==0) siz=sizeof(void*);		//FIXME strange
 if (siz<=0) fatal("mmalloc: want negative memory or zero memory: %d", siz);
 ptr = malloc(siz);
 if (!ptr) { gtk_dialog_printf("mmalloc failed!"); fatal("mmalloc failed"); return NULL; }
 add_to_memlist(ptr);
 return ptr;
#endif
}


void* mrealloc(void* orig, INT siz)
{
#ifdef NO_TRACE_MEM
 return realloc(orig, siz);
#else
 void* ptr;
 sdebug("%s:%d mrealloc(%p,%d)",__FILE__,__LINE__,orig,siz);
 if (siz==0) siz=sizeof(void*);		//FIXME strange
 if (siz<=0) fatal("mmalloc: want negative memory or zero memory: %d", siz);
 ptr = realloc(orig, siz);
 if (!ptr) { gtk_dialog_printf("mmalloc failed!"); fatal("mmalloc failed"); return NULL; }
 if (ptr != orig) warn("ORGINAL ARRDRESS: %p, NEW %p - possible memory leak", orig, ptr);
// add_to_memlist(ptr);
 return ptr;
#endif
}


void mfree(void** ptr)
{
#ifdef NO_TRACE_MEM
 free(*ptr);
#else
 sdebug("%s:%d mfree(%p)",__FILE__,__LINE__,*ptr);
 del_from_memlist(*ptr);
 if (*ptr) free(*ptr);
 *ptr = NULL;
#endif
}


void* smalloc(struct mem_list** headp, INT siz)
{
#ifdef NO_TRACE_MEM
 return malloc(siz);
#else
 void* ptr;
 sdebug("%s:%d smalloc(%p,%d)",__FILE__,__LINE__,*headp,siz);
 if (siz==0) siz=sizeof(void*);		//FIXME strange
 if (siz<=0) fatal("smalloc: want negative memory or zero memory: %p:%d", (void*)headp, siz);
 ptr = malloc(siz);
 if (!ptr) { gtk_dialog_printf("smalloc failed!"); fatal("smalloc failed"); return NULL; }
 add_to_special_memlist(headp, ptr);
 return ptr;
#endif
}


void sfree(struct mem_list** headp, void** ptr)
{
#ifdef NO_TRACE_MEM
 free(*ptr);
#else
 sdebug("%s:%d sfree(%p, %p)",__FILE__,__LINE__,*headp, *ptr);
 del_from_special_memlist(headp, *ptr);
 if (*ptr) free(*ptr);
 *ptr = NULL;
#endif
}

/* VERY DEBUG ONES... */

void* dsmalloc(struct dmem_list** headp, INT siz, CHAR* file, INT line)
{
#ifdef NO_TRACE_MEM
 return malloc(siz);
#else
 void* ptr;
 sdebug("%s:%d dsmalloc(%p) called from %s:%d, want %d bytes",__FILE__,__LINE__, *headp,file,line,siz);
 if (siz==0) siz=sizeof(void*);		//FIXME strange
 if (siz<=0) fatal("dsmalloc: want negative memory or zero memory: %p:%d, called from %s:%d", (void*)headp,siz,file,line);
 ptr = malloc(siz);
 if (!ptr) { gtk_dialog_printf("dsmalloc failed!"); fatal("dsmalloc failed"); return NULL; }
 dadd_to_special_memlist(headp, ptr,file,line,siz);
 return ptr;
#endif
}


void dsfree(struct dmem_list** headp, void** ptr, CHAR* file, INT line)
{
#ifdef NO_TRACE_MEM
 free(*ptr);
#else
 sdebug("%s:%d dsfree(%p, %p) called from %s:%d",__FILE__,__LINE__,*headp, *ptr, file,line);
 ddel_from_special_memlist(headp,*ptr,file,line);
 if (*ptr) free(*ptr);
 *ptr = NULL;
#endif
}


void dshred_special_memlist(struct dmem_list** head, CHAR* file, INT line)
{
#ifdef NO_TRACE_MEM
 return ;
#else
 struct dmem_list *tmp, *prev;
 sdebug("%s:%d dshred_special_memlist(%p) called from %s:%d",__FILE__,__LINE__,*head, file,line);
 tmp = prev = *head;
 while (tmp)
   {
    prev = tmp;
    tmp = tmp->next;
    sdebug("freeing memory (%d bytes) allocated at: %s:%d", prev->count, prev->file, prev->line);
    free(prev);
    prev = NULL;
   }
 *head = NULL;
#endif
}


void dadd_to_special_memlist(struct dmem_list** head, void* ptr, CHAR* file, INT line, INT size)
{
 struct dmem_list* newh;
 sdebug("%s:%d dadd_to_special_memlist(%p,%p) called from <delegation> %s:%d, bytes: %d",__FILE__,__LINE__,*head, ptr, file,line,size);
 if (!(*head))
   {
    *head = (struct dmem_list*)malloc(sizeof(struct dmem_list));
    if (!(*head)) { fatal("dsmalloc failed!"); return; }
    (*head)->ptr  = ptr;
    (*head)->next = NULL;
    (*head)->file = file;
    (*head)->line = line;
    (*head)->count = size;
    return;
   }
 newh = (struct dmem_list*)malloc(sizeof(struct dmem_list));
 if (!newh) { fatal("dsmalloc failed!"); return; }
 newh->ptr   = ptr;
 newh->next  = *head;
 newh->file = file;
 newh->line = line;
 newh->count = size;
 *head       = newh;
}


void dprint_special_memlist(struct dmem_list* head, CHAR* file, INT line)
{
#ifdef NO_TRACE_MEM
 return;
#else
 struct dmem_list* tmp;
 sdebug("%s:%d dprint_special_memlist(%p) called from %s:%d",__FILE__,__LINE__,head,file,line);
 tmp = head;
 debug("DEBUG_MEMLIST_SPECIAL:\n");
 while (tmp)
   {
    debug("-->PTR:%p WHERE: %s:%d, COUNT: %d\n", tmp->ptr,tmp->file,tmp->line,tmp->count);
    tmp = tmp->next;
   }
 debug("DEBUG_MEMLIST_SPECIAL END\n");
#endif
}


void ddel_from_special_memlist(struct dmem_list** head, void* ptr, CHAR* file, INT line)
{
 struct dmem_list *tmp, *prev;
 sdebug("%s:%d ddel_from_special_memlist(%p,%p) called from <delegation> %s:%d",__FILE__,__LINE__,*head,ptr,file,line);
 tmp = prev = *head;
 while (tmp)
   {
    if (tmp->ptr==ptr)
      {
       if (tmp==(*head))
         {
          *head = tmp->next;
	  sdebug("deleted list head: %p, from:%s:%d, count:%d", tmp->ptr, tmp->file,tmp->line,tmp->count);
	  free(tmp);
	  tmp = NULL;
	  return ;
         }
       sdebug("deleted list element: %p, from:%s:%d, count:%d", tmp->ptr, tmp->file,tmp->line,tmp->count);
       prev->next = tmp->next;
       free(tmp);
       tmp = NULL;
       return ;
      }
    prev = tmp;
    tmp = tmp->next;
   }
 fatal("DEBUG_SPECIAL_MEMLIST: No such pointer: %p", ptr);
}


void* dmmalloc(INT siz, CHAR* file, INT line)
{
#ifdef NO_TRACE_MEM
 return malloc(siz);
#else
 void* ptr;
 sdebug("%s:%d dmmalloc() called from %s:%d, want %d bytes",__FILE__,__LINE__, file,line,siz);
 if (siz==0) siz=sizeof(void*);		//FIXME strange
 if (siz<=0) fatal("dmmalloc: want negative memory or zero memory: %p:%d, called from %s:%d", (void*)dmlist,siz,file,line);
 ptr = malloc(siz);
 if (!ptr) { gtk_dialog_printf("dmmalloc failed!"); fatal("dmmalloc failed"); return NULL; }
 dadd_to_memlist(ptr,file,line,siz);
 return ptr;
#endif
}


void* dmrealloc(void* orig, INT siz, CHAR* file, INT line)
{
#ifdef NO_TRACE_MEM
 return realloc(orig, siz);
#else
 void* ptr;
 sdebug("%s:%d dmrealloc called from %s:%d, want %d bytes, at addres: %p",__FILE__,__LINE__, file,line,siz, orig);
 if (siz==0) siz=sizeof(void*);		//FIXME strange
 if (siz<=0) fatal("dmrealloc: want negative memory or zero memory: %p:%d, called from %s:%d", (void*)dmlist,siz,file,line);
 ptr = realloc(orig, siz);
 if (!ptr) { gtk_dialog_printf("dmrealloc failed!"); fatal("dmrealloc failed"); return NULL; }
 if (ptr != orig) warn("ORGINAL ARRDRESS: %p, NEW %p - possible memory leak", orig, ptr);
// dadd_to_memlist(ptr,file,line,siz);
 return ptr;
#endif
}


void dmfree(void** ptr, CHAR* file, INT line)
{
#ifdef NO_TRACE_MEM
 free(*ptr);
#else
 sdebug("%s:%d dmfree(%p) called from %s:%d",__FILE__,__LINE__,*ptr,file,line);
 ddel_from_memlist(*ptr,file,line);
 if (*ptr) free(*ptr);
 *ptr = NULL;
#endif
}


void dshred_memlist(CHAR* file, INT line)
{
#ifdef NO_TRACE_MEM
 return ;
#else
 struct dmem_list *tmp, *prev;
 sdebug("%s:%d dmhred_memlist called from %s:%d",__FILE__,__LINE__,file,line);
 tmp = prev = dmlist;
 while (tmp)
   {
    prev = tmp;
    tmp = tmp->next;
    sdebug("freeing memory (%d bytes) allocated at: %s:%d", prev->count, prev->file, prev->line);
    free(prev);
    prev = NULL;
   }
 dmlist = NULL;
#endif
}


void dadd_to_memlist(void* ptr, CHAR* file, INT line, INT size)
{
 struct dmem_list* newh;
 sdebug("%s:%d dadd_to_memlist(%p) called from <delegation> %s:%d, bytes: %d",__FILE__,__LINE__,ptr, file,line,size);
 if (!dmlist)
   {
    dmlist = (struct dmem_list*)malloc(sizeof(struct dmem_list));
    if (!dmlist) { fatal("dsmalloc failed!"); return; }
    dmlist->ptr  = ptr;
    dmlist->next = NULL;
    dmlist->file = file;
    dmlist->line = line;
    dmlist->count = size;
    return;
   }
 newh = (struct dmem_list*)malloc(sizeof(struct dmem_list));
 if (!newh) { fatal("dsmalloc failed!"); return; }
 newh->ptr   = ptr;
 newh->next  = dmlist;
 newh->file = file;
 newh->line = line;
 newh->count = size;
 dmlist       = newh;
}


void dprint_memlist(CHAR* file, INT line)
{
#ifdef NO_TRACE_MEM
 return ;
#else
 struct dmem_list* tmp;
 sdebug("%s:%d dprint_memlist() called from %s:%d",__FILE__,__LINE__,file,line);
 tmp = dmlist;
 debug("DEBUG_MEMLIST:\n");
 while (tmp)
   {
    debug("-->PTR:%p WHERE: %s:%d, COUNT: %d\n", tmp->ptr,tmp->file,tmp->line,tmp->count);
    tmp = tmp->next;
   }
 debug("DEBUG_MEMLIST END\n");
#endif
}


void ddel_from_memlist(void* ptr, CHAR* file, INT line)
{
 struct dmem_list *tmp, *prev;
 sdebug("%s:%d ddel_from_memlist(%p) called from <delegation> %s:%d",__FILE__,__LINE__,ptr,file,line);
 tmp = prev = dmlist;
 while (tmp)
   {
    if (tmp->ptr==ptr)
      {
       if (tmp==dmlist)
         {
          dmlist = tmp->next;
	  sdebug("deleted list head: %p, from:%s:%d, count:%d", tmp->ptr, tmp->file,tmp->line,tmp->count);
	  free(tmp);
	  tmp = NULL;
	  return ;
         }
       sdebug("deleted list element: %p, from:%s:%d, count:%d", tmp->ptr, tmp->file,tmp->line,tmp->count);
       prev->next = tmp->next;
       free(tmp);
       tmp = NULL;
       return ;
      }
    prev = tmp;
    tmp = tmp->next;
   }
 fatal("DEBUG_MEMLIST: No such pointer: %p", ptr);
}

