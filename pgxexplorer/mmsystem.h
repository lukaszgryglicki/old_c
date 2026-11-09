/* Written by Morgoth DBMA, morgothdbma@o2.pl
This is part of PgXexplorer software, Open Source
 on BSD licence, Libraries(interaces) used:
 GNU GCC, AS (all stuff needed to compile C source into executable binary)
 LibPQ-FE from PostgreSQL, GTK (GIMP Toolkit)
 written in VIM editor, ctags used, CVS used
 Currently only one author: MOrgoth DBMA
 FILE: mmsystem.h */
#ifndef __MORGOTHDBMA_MMSYSTEM_H__
#define __MORGOTHDBMA_MMSYSTEM_H__
#include "common.h"
struct mem_list
{
 void* ptr;
 struct mem_list* next;
};
struct dmem_list
{
 void* ptr;
 CHAR* file;
 INT line;
 INT count;
 struct dmem_list* next;
};
#include "gtkdialogs.h"
#include "consoleout.h"
#define HERE __FILE__,__LINE__
void add_to_memlist(void* ptr);
void print_memlist();
void del_from_memlist(void* ptr);
void* mmalloc(INT siz);
void* mrealloc(void*, INT siz);
void mfree(void** ptr);
void add_to_special_memlist(struct mem_list** hp, void* ptr);
void print_special_memlist(struct mem_list* h);
void del_from_special_memlist(struct mem_list** hp, void* ptr);
void* smalloc(struct mem_list** hp, INT siz);
void sfree(struct mem_list** hp, void** ptr);
void shred_memlist();
void shred_special_memlist(struct mem_list**);
void* dsmalloc(struct dmem_list** hp, INT siz, CHAR* file, INT line);
void dsfree(struct dmem_list** hp, void** ptr, CHAR* file, INT line);
void dshred_special_memlist(struct dmem_list**, CHAR* file, INT line);
void dadd_to_special_memlist(struct dmem_list** hp, void* ptr, CHAR* file, INT line, INT size);
void dprint_special_memlist(struct dmem_list* h, CHAR* file, INT line);
void ddel_from_special_memlist(struct dmem_list** hp, void* ptr, CHAR* file, INT line);
void* dmmalloc(INT siz, CHAR* file, INT line);
void* dmrealloc(void*, INT siz, CHAR* file, INT line);
void dmfree(void** ptr, CHAR* file, INT line);
void dmhred_memlist(CHAR* file, INT line);
void dadd_to_memlist(void* ptr, CHAR* file, INT line, INT size);
void dprint_memlist(CHAR* file, INT line);
void ddel_from_memlist(void* ptr, CHAR* file, INT line);
void dshred_memlist(CHAR* file, INT line);
#endif
