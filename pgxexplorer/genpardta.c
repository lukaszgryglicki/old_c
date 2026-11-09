#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define __ASM_STR_CMP  (int(*)(const void*,const void*))(0)
#define __ASM_INS_MEDIUM 6
extern int qsortasm(void*, int(*)(const void*, const void*), unsigned int, unsigned int, int);

void push_results(FILE* f, char** tab, int len, int max_len)
{
 int i;
 printf("newl=%d\n", len);
 if (!f || !tab) { printf("fatal: null pointer.\n"); return; }
 fprintf(f,"struct parser_data\n{\n  int type;\n  char keyword[%d];\n};\n\n", max_len+1);
 fprintf(f,"struct parser_data parser[%d] = {\n", len);
 for (i=0;i<len-1;i++) fprintf(f,"{ 0,  \"%s\" },\n", tab[i]);
 fprintf(f,"{ 0,  \"%s\" }\n", tab[len-1]);
 fprintf(f,"};\n");
}


void remove_dups(char** tab, int len, char*** tabn, int* lenn, int max_len)
{
 int i,j;
 int* tabref;
 *lenn = 0;
 *tabn = 0;
 tabref = (int*)malloc(len*sizeof(int));
 for (i=0;i<len;i++) tabref[i] = -1;
 for (i=0;i<len;i++) for (j=i;j<len;j++)
      if (!strcmp(tab[i], tab[j])) tabref[i]++;
 for (i=0;i<len;i++) if (!tabref[i]) (*lenn)++;
 *tabn = (char**)malloc(((*lenn)+1)*sizeof(char*));
 j=0;
 for (i=0;i<len;i++) if (!tabref[i])
    {
     (*tabn)[j] = tab[i];
     j++;
    }
 for (i=0;i<(*lenn);i++) printf("%s\n", (*tabn)[i]);
 free(tabref);
}


void run(FILE* in, FILE* out)
{
 char** table;
 char** newtable;
 int lines,len,max,i,zn,newlines;
 lines=0;
 len=0;
 max=0;
 newtable=0;
 while ((zn=fgetc(in))!=EOF)
   {
    if (zn=='\n')
      {
       lines++;
       if (len>max) max = len;
       len = 0;
      }
    else len++;
   }
 printf("file has %d lines, longest one has %d chars\n", lines, max);
 if (lines<=2) { printf("too little data to create parser struct.\n"); return; }
 table = (char**)malloc((lines+1)*sizeof(char*));
 for (i=0;i<lines;i++) table[i] = (char*)malloc(max+1);
 fseek(in, 0, SEEK_SET);
 lines=0;
 len=0;
 while ((zn=fgetc(in))!=EOF)
   {
    if (zn=='\n')
      {
       table[lines][len] = 0;
       lines++;
       if (len>max) max = len;
       len = 0;
      }
    else
      {
       table[lines][len] = zn;
       len++;
      }
   }
 max++;
 lines--;
 qsortasm((void*)table, __ASM_STR_CMP, 0, lines-1, __ASM_INS_MEDIUM);
 remove_dups(table, lines, &newtable, &newlines, max);
 push_results(out, newtable, newlines, newlines);
 for (i=0;i<=lines;i++) free(table[i]);
 free(table);
 free(newtable);
}


void parser_create(char* i, char* o)
{
 FILE *in,*out;
 char z;
 int size;
 if ((in=fopen(i,"r"))==NULL) { printf("cannot open: %s\n", i); return; }
 if ((out=fopen(o,"r"))!=NULL)
   {
    fclose(out);
    printf("overwrite: %s? (y/n): ", o);
    z = getchar();
    if (z!='y' && z!='Y') { fclose(in); return; }
   }
 if ((out=fopen(o,"w"))==NULL) { printf("cannot open for writing: %s\n", o); return; }
 fseek(in,0,SEEK_END);
 size = ftell(in);
 fseek(in,0,SEEK_SET);
 if (size<1024*1024) run(in,out);
 else printf("input file too big.\n");
 fclose(out);
 fclose(in);
}


int main(int lb, char** par)
{
 if (lb!=3) { printf("no filenames given: in,out\n"); return 1; }
 parser_create(par[1], par[2]);
 return 0;
}

