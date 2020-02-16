#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#define PATH_MAX 1024

struct mflist
{
 char* name;
 int fsize;
 struct mflist* next;
};
static struct mflist* flist=NULL;
static struct mflist* last=NULL;
static FILE*  logf = NULL;
static int shmode = 0;
static int cmp = 0;

int is_regfile(char* file)
{
 int err;
 struct stat st;
 err=stat(file, &st);
 if (err==-1)
   {
    perror("stat");
    return 0;
   }
 if (st.st_mode & S_IFREG) return 1;
 else if (st.st_mode & S_IFDIR) return 2;
 else return 0;
}


void add_element(char* elem, int siz)
{
 if (!elem) return;
 if (last == NULL)
   {
    flist = malloc(sizeof(struct mflist));
    flist->name = malloc(strlen(elem)+1);
    flist->fsize = siz;
    strcpy(flist->name, elem);
    flist->next = NULL;
    last = flist;
   }
 else
   {
    last->next = malloc(sizeof(struct mflist));
    last = last->next;
    last->next = NULL;
    last->name = malloc(strlen(elem)+1);
    strcpy(last->name, elem);
    last->fsize = siz;
   }
}


void open_log()
{
 logf = fopen("difuser.LOG", "w");
 if (!logf) printf("Cannot Create difuser.LOG file.\n");
 else
   {
    fprintf(logf,"This is DIFUSER LOG FILE\n");
    if (!shmode)
       fprintf(logf, "Working in PREVIEW mode, nothing will be erased\n");
   }
}


void close_log()
{
 if (logf)
   {
    fprintf(logf, "End of SHRED LOG\n");
    fclose(logf);
   }
 logf = NULL;
}


void logfail(char* fn, char* dup)
{
 if (!logf) return;
 fprintf(logf, "====\nFailed to remove file: %s\nDuplicate of: %s\n====\n"
		 , fn, dup);
}


void logsucc(char* fn, char* dup)
{
 if (!logf) return;
 fprintf(logf, "Removed: %s (copy of %s)\n", fn, dup);
}


void logwish(char* fn, char* dup)
{
 if (!logf) return;
 fprintf(logf, "Would remove: %s (copy of %s)\n", fn, dup);
}


int generate_flist(char* fname)
{
 char buff[PATH_MAX+1];
 DIR* dp;
 FILE* f;
 int type;
 struct dirent *entry;
 struct stat statbuffer;
 char start_dir[PATH_MAX+1];
 char tmp[PATH_MAX+1];
 strcpy(buff, fname);
/* printf("Opening directory: %s\n", buff);*/
 dp = opendir(buff);
 if (dp==NULL) return 1;
 if (!getcwd(start_dir, PATH_MAX)) return 2;
 while ((entry=readdir(dp))!=NULL)
     {
      strcpy(tmp, buff);
      if (tmp[strlen(tmp)-1]!='/') strcat(tmp, "/");
      strcat(tmp, entry->d_name);
/*      printf("tmp = %s\n", tmp);*/
      if (stat(tmp,&statbuffer)==-1) return 4;
      if (!strcmp(entry->d_name,".") || !strcmp(entry->d_name, "..")) continue;
      if (S_ISLNK(statbuffer.st_mode)) continue;
      if (S_ISDIR(statbuffer.st_mode))
	  {
	   strcpy(tmp, buff);
	   if (tmp[strlen(tmp)-1]!='/')
		   strcat(tmp, "/");
	   strcat(tmp, entry->d_name);
	   if (tmp[strlen(tmp)-1]!='/')
		   strcat(tmp, "/");
/*	   printf("DIVE to %s\n", tmp);*/
	   generate_flist(tmp);
	  }
      else
          {
	   strcpy(tmp, buff);
	   if (tmp[strlen(tmp)-1]!='/')
		   strcat(tmp, "/");
	   strcat(tmp, entry->d_name);
/*	   printf("FILE: %s\n", tmp);*/
           if ((type=is_regfile(tmp))!=1) continue;
           else
	     {
 	      f = fopen(tmp, "r");
              if (!f) continue;
 	      fseek(f, 0, SEEK_END);
              type = ftell(f);
	      if (type<=0)
	         {
	          fclose(f);
		  continue;
		 }
	      add_element(tmp, type);
	      fclose(f);
	     }
          }
     }
 chdir(start_dir);
/* printf("done.\n");*/
 closedir(dp);
 return 0;
}


int do_compare(struct mflist* p1, struct mflist* p2)
{
 FILE *f1, *f2;
 int z1,z2;
 if (p1->fsize != p2->fsize) return 0;
 f1 = fopen(p1->name, "r");
 f2 = fopen(p2->name, "r");
 if (!f1 || !f2) return 0;
 cmp++;
 z1=z2=0;
 while (z1==z2)
      {
       z1 = fgetc(f1);
       z2 = fgetc(f2);
       if (z1==EOF)
         {
          fclose(f1);
          fclose(f2);
	  return 1;
	 }
      }
 fclose(f1);
 fclose(f2);
 return 0;
}


void list_count()
{
 struct mflist* tmp;
 int cnt;
 cnt=0;
 tmp  = flist;
 while (tmp)
   {
    cnt++;
    tmp = tmp->next;
   }
 printf("list#: %d\n", cnt);
}


void shred_list()
{
 struct mflist* tmp1;
 struct mflist* tmp2;
 struct mflist* prev;
 cmp=0;
 tmp1  = flist;
 if (!shmode) printf("Working in PREVIEW mode, to change add -r option\n");
 list_count();
 open_log();
 while (tmp1)
   {
    prev = tmp1;
    tmp2 = tmp1->next;
    while (tmp2)
      {
       if (do_compare(tmp1, tmp2))
         {
/*          printf("IDENT: %s,%s\n", tmp1->name, tmp2->name);*/
	  printf("*");
	  fflush(stdout);
	  prev->next = tmp2->next;
	  if (shmode)
	   {
	    if (unlink(tmp2->name))
	      {
	       printf("X");
	       fflush(stdout);
	       logfail(tmp2->name, tmp1->name);
	      }
	    else logsucc(tmp2->name, tmp1->name);
	   }
	  else logwish(tmp2->name, tmp1->name);
	  free(tmp2->name);
	  free(tmp2);
	  tmp2 = prev;
         }
       prev = tmp2;
       if (tmp2) tmp2 = tmp2->next;
      }
    tmp1 = tmp1->next;
    printf(".");
    fflush(stdout);
   }
 printf("\ncompared# %d\n", cmp);
 close_log();
 list_count();
}


void catch_int(int signo)
{
 printf("!\n");
 if (logf) fprintf(logf,"DIFUSER was INTERRUPTED by SIGINT\n");
 close_log();
 exit(2);
}


void signal_setup()
{
 static struct sigaction act;
 act.sa_handler = catch_int;
 sigfillset(&(act.sa_mask));
 sigaction(SIGINT, &act, NULL);
}


void free_list()
{
 struct mflist* tmp, *prev;
 tmp  = flist;
 while (tmp)
   {
    prev = tmp;
    tmp = tmp->next;
    free(prev);
   }
}


void help()
{
 printf("--------------------------------------------------------\n");
 printf("DIFUSER by MorgothDBMA, ver 0.2, license GNU GPL\n");
 printf("Compiled: %s %s File: %s, line %d\n", __DATE__, __TIME__, __FILE__, __LINE__);
 printf("This program expects one argument - a directory name\n");
 printf("When no argument is given, program uses ``.''\n");
 printf("Example:\tdifuser /home/kelly\n");
 printf("Searches given dir for duplicate files (by contents)\n");
 printf("And removes duplicates, log may be found in: difuser.LOG\n");
 printf("Ohhh and to really remove files you must add -r option\n");
 printf("difuser -r is denied (you can't accidently work on .)\n");
 printf("instead (if you know what you're doing) try difuser . -r\n");
 printf("Typical: difuser /somewhere/far/beyond -r\n");
 printf("Enjoy the software\n");
 printf("Autor contact: morgothdbma@o2.pl, tel: (+48)693582014\n");
 printf("--------------------------------------------------------\n");
}


int main(int lb, char** par)
{
 int err;
 char syscmd[PATH_MAX+32];
 help();
 signal_setup();
 printf("generating file list. . .");
 fflush(stdout);
 if (lb<2)
  {
   sprintf(syscmd,"chmod -R u+rw .");
   system(syscmd);
   err = generate_flist(".");
  }
 else
  {
   if (par[1][0]=='-')
     {
      printf("error\n\tSpecify directory when using ADVANCED-options\n");
      return 2;
     }
   sprintf(syscmd,"chmod -R u+rw %s", par[1]);
   system(syscmd);
   err = generate_flist(par[1]);
  }
 if (err)
   {
    printf("generate list error: %d\n", err);
    return 2;
   }
 printf(" . done\n");
 if (lb==3 && !strcmp(par[2], "-r")) shmode=1;
 shred_list();
 printf("freeying resources used...\n");
 free_list();
 return 0;
}

