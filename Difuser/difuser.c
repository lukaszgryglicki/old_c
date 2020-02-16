#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <signal.h>
#define PATH_MAX 1024

#define ULL unsigned long long
/* ASSOC: this is needed */
/*struct ptrlist;*/

struct mflist
{
 char* name;
 ULL fsize;
 unsigned long ino;
 struct mflist* next;
/* struct ptrlist *ids, *last;*/
};

/* ASSOC: this is needed */
/*
struct ptrlist
{
    struct mflist* ptr;
    struct ptrlist* next;
};
*/

static struct mflist* flist=NULL;
static struct mflist* last=NULL;
static FILE*  logf = NULL;
static int shmode = 0;			/* 0 = preview, 1 = shred, 2 = hardlink */
static int cmp = 0;
static ULL gain_bytes = 0;
static int gain_files = 0;
static unsigned long glob_nfiles = 0;
static int follow = 0;
char* cwd = NULL;
void* cmpbuf1 = NULL;
void* cmpbuf2 = NULL;
int cmpbufn = 0;	/* buffer size */
int maxr = 0;		/* read maximum maxr buffers, 0=off, 10=10 buffer sizes */
int nskip = 0;		/* after reading single buffer, skip nskip next buffers */

int is_regfile(char* file, unsigned long* ino)
{
 int err;
 struct stat st;
 if (follow) err=stat(file, &st);
 else err=lstat(file, &st);
 if (err==-1)
   {
    perror("stat");
    return 0;
   }
 *ino = st.st_ino;
#ifdef DBG
 printf("is_regfile: %s, i=%d, reg=%d, dir=%d\n", 
	 file, (int)(*ino), (int)(st.st_mode & S_IFREG), (int)(st.st_mode & S_IFDIR));
#endif
 if (st.st_mode & S_IFREG) return 1;
 else if (st.st_mode & S_IFDIR) return 2;
 else return 0;
}

/* ASSOC: 1: this function is used */
/*
void add_ptr(struct mflist* list, struct mflist* ptr)
{
 if (!list || !ptr) return;
 if (list->last == NULL)
   {
    list->ids = malloc(sizeof(struct ptrlist));
    list->ids->ptr = ptr; 
    list->ids->next = NULL;
    list->last = list->ids;
   }
 else
   {
    list->last->next = malloc(sizeof(struct ptrlist));
    list->last = list->last->next;
    list->last->ptr = ptr;
    list->last->next = NULL;
   }
}

void print_alist(struct mflist* list)
{
 struct ptrlist* tmp;
 int cnt;
 cnt=0;
 tmp  = list->ids;
 while (tmp)
   {
    cnt++;
    tmp = tmp->next;
   }
}

int common_assoc(struct mflist* p1, struct mflist* p2)
{
 struct ptrlist *tmp1, *tmp2;

 tmp1 = p1->ids;
 while (tmp1)
 {
     tmp2 = p2->ids;
     while (tmp2)
     {
	 if (tmp1->ptr == tmp2->ptr)
	 {
	     return 1;
	 }
	 tmp2 = tmp2->next;
     }
     tmp1 = tmp1->next;
 }
 return 0;
}

void assoc(struct mflist* p1, struct mflist* p2)
{
    add_ptr(p1, p2);
    add_ptr(p2, p1);
}
*/

void add_element(char* elem, ULL siz, unsigned long ino)
{
 if (!elem) return;
#ifdef DBG
 printf("add_element: %s, siz=%lld, ino=%d\n", elem, siz, (int)ino);
#endif
 if (last == NULL)
   {
    flist = malloc(sizeof(struct mflist));
    flist->name = malloc(strlen(elem)+1);
    flist->fsize = siz;
    flist->ino   = ino;
    strcpy(flist->name, elem);
    /* ASSOC: needs this */
/*    flist->ids = malloc(sizeof(struct mflist));*/
/*    flist->ids->ptr = flist;*/
/*    flist->ids->next = NULL;*/
/*    flist->last = flist->ids;*/
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
    last->ino = ino;
    /* ASSOC: needs this */
/*    last->ids = malloc(sizeof(struct mflist));*/
/*    last->ids->ptr = last;*/
/*    last->ids->next = NULL;*/
/*    last->last = last->ids;*/
   }
}


void close_log()
{
 if (logf)
   {
    fprintf(logf, "End of LOG\n\n");
    fclose(logf);
   }
 logf = NULL;

 if (cmpbuf1) 
 {
     free(cmpbuf1);
     cmpbuf1 = NULL;
 }
 if (cmpbuf2) 
 {
     free(cmpbuf2);
     cmpbuf2 = NULL;
 }
 cmpbufn = 0;
}


void open_log()
{
 system("date >> difuser.LOG 2>/dev/null");
 logf = fopen("difuser.LOG", "a+");
 if (!logf) 
 {
     printf("Cannot Create difuser.LOG file.\n");
     exit(0);
 }
 else
   {
    fprintf(logf,"This is DIFUSER LOG FILE\n");
   }
}

void log_type()
{
    if (shmode == 0) fprintf(logf, "Working in PREVIEW mode, nothing will be erased/linked\n");
    else if (shmode == 1) fprintf(logf, "Working in ERASE mode\n");
    else if (shmode == 2) fprintf(logf, "Working in HARDLINK mode\n");
    else { fprintf(logf, "Unkonown mode\n"); close_log(); exit(2); }
}


void logfail(char* fn, char* dup, int idino)
{
 if (!logf) return;
 if (idino) fprintf(logf, "====\nFailed to remove file: %s\nHardlink of: %s\n====\n", fn, dup);
 else fprintf(logf, "====\nFailed to remove file: %s\nDuplicate of: %s\n====\n", fn, dup);
#ifdef DBG
 if (idino) fprintf(stderr, "====\nFailed to remove file: %s\nHardlink of: %s\n====\n", fn, dup);
 else fprintf(stderr, "====\nFailed to remove file: %s\nDuplicate of: %s\n====\n", fn, dup);
#endif
 fflush( logf );
}

void logfaillink(char* fn, char* dup, int idino)
{
 if (!logf) return;
 if (idino) fprintf(logf, "====\nFailed to relink file: %s\nHardlink of: %s\n====\n", fn, dup);
 else fprintf(logf, "====\nFailed to link file: %s\nDuplicate of: %s\n====\n", fn, dup);
#ifdef DBG
 if (idino) fprintf(stderr, "====\nFailed to relink file: %s\nHardlink of: %s\n====\n", fn, dup);
 else fprintf(stderr, "====\nFailed to link file: %s\nDuplicate of: %s\n====\n", fn, dup);
#endif
 fflush( logf );
}

void gain(char* fn)
{
    FILE* f;

    f = fopen(fn, "rb+");
    if (!f) return;
    fseek(f, 0, SEEK_END);
    gain_bytes += (ULL)ftello(f);
    gain_files ++;
    fclose(f);

#ifdef DBG
    printf("Gain: %lld kbytes\n", gain_bytes >> 8);
#endif
}

void log_size(ULL s)
{
 if (!logf) return;

    if (s < (1LL<<10)) fprintf(logf, "%4lldb  ", s);
    else if (s < (1LL<<20)) fprintf(logf, "%4lldkb ", s >> 10);
    else if (s < (1LL<<30)) fprintf(logf, "%4lldmb ", s >> 20);
    else if (s < (1LL<<40)) fprintf(logf, "%4lldgb ", s >> 30);
    else fprintf(logf, "%4lldtb ", s >> 40);
}

void logsucc(char* fn, char* dup, int idino, ULL fsize)
{
 if (!idino) gain(dup);
 if (!logf) return;
 log_size( fsize );
 if (shmode==1) 
 {
     if (idino) 
     {
	 if (follow) fprintf(logf, "Removed: %s (link of %s)\n", fn, dup);
	 else fprintf(logf, "Removed: %s (hardlink of %s)\n", fn, dup);
     }
     else fprintf(logf, "Removed: %s (copy of %s)\n", fn, dup);
 }
 else if (shmode==2) 
 {
     if (idino) 
     {
	 if (follow) fprintf(logf, "Rehardlinked: %s (link of %s)\n", fn, dup);
	 else fprintf(logf, "Rehardlinked: %s (hardlink of %s)\n", fn, dup);
     }
     else fprintf(logf, "Hardlinked: %s (copy of %s)\n", fn, dup);
 }
 fflush( logf );
}


void logwish(char* fn, char* dup, int idino, ULL fsize)
{
 if (!idino) gain(dup);
 if (!logf) return;
 log_size( fsize );
 if (idino) 
 {
     if (follow) fprintf(logf, "Would remove: %s (link of %s)\n", fn, dup);
     else fprintf(logf, "Would remove: %s (hardlink of %s)\n", fn, dup);
 }
 else fprintf(logf, "Would remove: %s (copy of %s)\n", fn, dup);
 fflush( logf );
}


int generate_flist(char* fname)
{
 char buff[PATH_MAX+1];
 DIR* dp;
 FILE* f;
 int type;
 ULL fsize;
 unsigned long ino;
 struct dirent *entry;
 struct stat statbuffer;
 char start_dir[PATH_MAX+1];
 char tmp[PATH_MAX+1];
 strcpy(buff, fname);
#ifdef DBG
 printf("Opening directory: %s\n", buff);
#endif
 if (logf) 
 {
     fprintf(logf, "Dir: %s\n", buff);
     fflush( logf );
 }
 dp = opendir(buff);
 if (dp==NULL) return 1;
 if (!getcwd(start_dir, PATH_MAX)) return 2;
 while ((entry=readdir(dp))!=NULL)
     {
      strcpy(tmp, buff);
      if (tmp[strlen(tmp)-1]!='/') strcat(tmp, "/");
      strcat(tmp, entry->d_name);
#ifdef DBG
      printf("readdir: %s\n", tmp);
#endif
      if (follow) 
      {
	  if (stat(tmp,&statbuffer)==-1) return 4;
      }
      else 
      {
	  if (lstat(tmp,&statbuffer)==-1) return 4;
      }
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
#ifdef DBG
	   printf("DIVE to %s\n", tmp);
#endif
	   generate_flist(tmp);
	  }
      else
          {
	   strcpy(tmp, buff);
	   if (tmp[strlen(tmp)-1]!='/')
		   strcat(tmp, "/");
	   strcat(tmp, entry->d_name);
#ifdef DBG
	   printf("FILE: %s\n", tmp);
#endif
	   ino = 0;
           if ((type=is_regfile(tmp, &ino))!=1) 
	   {
#ifdef DBG
	       printf("Not regular file or dir: %s, skipped\n", tmp);
#endif
	       continue;
	   }
           else
	     {
 	      f = fopen(tmp, "rb+");
              if (!f) continue;
 	      fseek(f, 0, SEEK_END);
              fsize = ftello(f);
	      if (fsize<=0)
	         {
	          fclose(f);
		  continue;
		 }
	      add_element(tmp, fsize, ino);
	      fclose(f);
	      glob_nfiles ++;
	      if (!(glob_nfiles % 100)) 
	      {
		  printf("+");
		  fflush(stdout);
	      }
	     }
          }
     }
 chdir(start_dir);
#ifdef DBG
 printf("done.\n");
#endif
 closedir(dp);
 return 0;
}


int do_compare(struct mflist* p1, struct mflist* p2, int* idino)
{
 FILE *f1, *f2;
/* struct stat st1, st2;*/
/* printf("%ld %ld\n", p1->ino, p2->ino);*/
/* register volatile int z1,z2;*/
 register volatile int n, nr;
 if (p1->fsize != p2->fsize) return 0;
#ifdef DBG
 printf("compare of: %s and %s\n", p1->name, p2->name);
#endif
 f1 = fopen(p1->name, "rb+");
 f2 = fopen(p2->name, "rb+");
 if (!f1 || !f2) 
 {
     if (f1) fclose(f1);
     if (f2) fclose(f2);
     printf("E");
     return 0;
 }
/* fstat(fileno(f1), &st1);*/
/* fstat(fileno(f2), &st2);*/
/* if (st1.st_ino == st2.st_ino)	*/
 if (p1->ino == p2->ino)	/* identical indoes --> probably hardlinked together */
/* if (0)*/
 {
     *idino = 1;
     printf("l");
     fclose(f1);
     fclose(f2);
     if (shmode <= 1)		/* if mode preview or mode remove we can tell just now these files are identical */
     {
	 /* This is not needed, such associations are eliminated in upper algorithm */
	 /* but it is for testing */
	 /* ASSOC: 1: uncomment */
/*	 assoc(p1, p2);*/
	 return 1;
     }
     else if (shmode == 2)	/* for hardlinks mode if we tell they're identical it will hardlink them again */
	 			/* so it is better to tell that they are different and leave them as they are already hardlinked */
	 return 0;
 }
 else *idino = 0;
 cmp++;
 /*getc_unlocked --> was fgetc */
 /*
 z1=z2=0;
 while (z1==z2)
      {
       z1 = getc_unlocked(f1);
       z2 = getc_unlocked(f2);
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
 */
 /* There it is possible to test associations: comment/uncomment lines with ASSOC tag */
 /* ASSOC 1: uncomment */
 /*
 if (common_assoc(p1, p2))
 {
     printf("A");
     fclose(f1);
     fclose(f2);
     assoc(p1, p2);
     return 1;
 }
 */
 nr = 0;
 while((n=fread(cmpbuf1, 1, cmpbufn, f1))>0 && (n=fread(cmpbuf2, 1, cmpbufn, f2))>0)
 {
     if (memcmp(cmpbuf1, cmpbuf2, n))
     {
#ifdef DBG
	 printf("differ at %d\n", n);
#endif
	 fclose(f1);
	 fclose(f2);
	 return 0;
     }
     nr ++;
     if (maxr > 0 && nr >= maxr) 
     {
#ifdef DBG
	 printf("Max buffers cmp reached: nr=%d, maxr=%d, nskip=%d\n", nr, maxr, nskip);
#endif
	 break;
     }
     if (nskip > 0)
     {
#ifdef DBG
	 printf("Skip %d buffers of %d bytes, offset: %d, nr=%d, maxr=%d, now at: %lld\n", 
		 nskip, cmpbufn, nskip * cmpbufn, nr, maxr, (ULL)ftello(f1));
#endif
	 fseek(f1, cmpbufn * nskip, SEEK_CUR);
	 fseek(f2, cmpbufn * nskip, SEEK_CUR);
     }
 }
#ifdef DBG
 printf("ident ");
#endif
 fclose(f1);
 fclose(f2);
 /* ASSOC: 1: uncomment */
/* assoc(p1, p2);*/
 return 1;
}


int list_count()
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
 if (logf) fprintf(logf, "list#: %d\n", cnt);
 return cnt;

}


void gain_info()
{
    printf("Gain: %d files\t\t%lldb\t%lldkb\t%lldMb\t%lldGb\n", 
	    gain_files,
	    gain_bytes,
	    gain_bytes >> 10,
	    gain_bytes >> 20,
	    gain_bytes >> 30
	    );

    if (logf)
    {
         fprintf(logf, "Gain: %d files\t\t%lldb\t%lldkb\t%lldMb\t%lldGb\n", 
            gain_files, 
	    gain_bytes,
	    gain_bytes >> 10,
	    gain_bytes >> 20,
	    gain_bytes >> 30
	    );
    }
}


void shred_list()
{
 struct mflist* tmp1;
 struct mflist* tmp2;
 struct mflist* prev;
 char* syscmd;
 int idino, nf, cf;
 double prc;
 ULL mcmp;

 cmp=cf=0;
 tmp1  = flist;
 if (!shmode) printf("Working in PREVIEW mode, to change add option -rR or -lL\n");
 nf = list_count();
 mcmp = ((ULL)nf*(ULL)nf)/2LL -(ULL)nf/2;
 printf("Max possible comparisions: %lld (%lldK, %lldM, %lldG)\n", mcmp, mcmp >> 10, mcmp >> 20, mcmp >> 30);
 if (logf) fprintf(logf, "Max possible comparisions: %lld (%lldK, %lldM, %lldG)\n", mcmp, mcmp >> 10, mcmp >> 20, mcmp >> 30);
/* open_log();*/
 log_type();
 while (tmp1)
   {
    prev = tmp1;
    tmp2 = tmp1->next;
    while (tmp2)
      {
       if (do_compare(tmp1, tmp2, &idino))
         {
	    if (idino) 
	    {
		if (!follow) fprintf(logf, "Notice: %s is hardlinked with %s\n", tmp1->name, tmp2->name);
		else fprintf(logf, "Notice: %s is linked with %s\n", tmp1->name, tmp2->name);
 		fflush( logf );
	    }
#ifdef DBG
          printf("IDENT: %s,%s\n", tmp1->name, tmp2->name);
#endif
	  if (!idino) printf("*");
	  fflush(stdout);
	  prev->next = tmp2->next;
	  cf ++;
	  if (shmode)
	   {
	    if (unlink(tmp2->name))
	      {
	       printf("X");
	       fflush(stdout);
	       logfail(tmp2->name, tmp1->name, idino);
	      }
	    else 
	    {
		if (shmode == 2)
		{
#ifdef DBG
		    printf("Removed: %s, hardlinking %s to %s\n", tmp2->name, tmp1->name, tmp2->name);
#endif
		    if (link(tmp1->name, tmp2->name))
		    {
		       /* we succeded in remove but failed in linking, so we need to recover removed file */
		       /* we can do it by copying another to which this file was identical */
		       syscmd = (char*)malloc(2048*sizeof(char));
		       sprintf(syscmd, "cp \"%s\" \"%s\"", tmp1->name, tmp2->name);
		       system(syscmd);
		       free((void*)syscmd);
	               printf("#");
	               fflush(stdout);
	               logfaillink(tmp2->name, tmp1->name, idino);
		    }
		    else logsucc(tmp2->name, tmp1->name, idino, tmp1->fsize);
		}
		else logsucc(tmp2->name, tmp1->name, idino, tmp1->fsize);
	    }
	   }
	  else logwish(tmp2->name, tmp1->name, idino, tmp1->fsize);
	  free(tmp2->name);
	  free(tmp2);
	  /*NOTICE: comment next line and enjoy common assocs*/
	  /* ASSOC: 1: comment out next line */
	  tmp2 = prev;
         }
       prev = tmp2;
       if (tmp2) tmp2 = tmp2->next;
      }
    tmp1 = tmp1->next;
    cf ++;
    prc = ((double)cf * 100.) / (double)nf;
    if (cf && !(cf % 100)) printf("(%d/%d:%7.3f%%)", cf, nf, prc);
    printf(".");
    fflush(stdout);
   }
 prc = ((double)cf * 100.) / (double)nf;
 printf("(%d/%d:%7.3f%%)", cf, nf, prc);
 printf("\ncompared# %d\n", cmp);
 if (logf) fprintf(logf, "\ncompared# %d\n", cmp);

 gain_info();
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
 sigaction(SIGTERM, &act, NULL);
 sigaction(SIGUSR1, &act, NULL);
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
 printf("DIFUSER by MorgothDBMA, ver 0.3, license GNU GPL\n");
 printf("Compiled: %s %s File: %s, line %d\n", __DATE__, __TIME__, __FILE__, __LINE__);
 printf("This program expects one argument - a directory name\n");
 printf("When no argument is given, program uses ``.''\n");
 printf("Example:\tdifuser /home/kelly\n");
 printf("Searches given dir for duplicate files (by contents)\n");
 printf("And removes or hardlinks duplicates, log may be found in: difuser.LOG\n");
 printf("Ohhh and to really remove files you must add -r option (-R for follow symlinks mode)\n");
 printf("And to hardlink files you must add -l option (-L for follow symlinks mode)\n");
 printf("Special preview with follow is -P\n");
 printf("difuser -rR or difuser -lL is denied (you can't accidently work on .)\n");
 printf("Instead (if you know what you're doing) try difuser . -rRlL\n");
 printf("Typical: difuser /somewhere/far/beyond -rRlL\n");
 printf("Enjoy the software\n");
 printf("Autor contact: lukaszgryglicki@o2.pl, tel: (+48)693582014\n");
 printf("--------------------------------------------------------\n");
}


int main(int lb, char** par)
{
 int err;
 char syscmd[PATH_MAX+32];
 struct timeval tv1, tv2, tvd;
 ULL llt;
 double dblt;
 open_log();

 cmpbufn = (1 << 20);		/* buffer size is 1M */
 maxr	 = (1 << 6);		/* check max 64 buffers */
 nskip	 = (1 << 6);		/* check 1M, skip 64M, check another 1M ... */
 				/* effective is (1+64)*64 = 4160 Mb, with 64Mb checked */
 cmpbuf1 = malloc(cmpbufn);
 cmpbuf2 = malloc(cmpbufn);
 gettimeofday(&tv1, NULL);
 help();
 signal_setup();
 printf("Generating file list.\n");
 fflush(stdout);
 if (lb<2)
  {
/*   sprintf(syscmd,"chmod -R u+rw .");*/
   sprintf(syscmd,"chmod -R u+r .");
   system(syscmd);
   follow = 0;
   shmode = 0;
   cwd = getwd(NULL);
   if (logf && cwd) fprintf(logf, "Working on path: %s\n", cwd);
   free((void*)cwd);
   err = generate_flist(".");
   printf("\nFiles processed: %ld\n", glob_nfiles);
   if (logf) fprintf(logf, "Files processed: %ld\n", glob_nfiles);
  }
 else
  {
   if (par[1][0]=='-')
     {
      printf("Error\n\tSpecify directory when using ADVANCED-options\n");
      return 2;
     }
   sprintf(syscmd,"chmod -R u+rw %s", par[1]);
   system(syscmd);
   follow = 0;
   if (lb==3 && !strcmp(par[2], "-r")) shmode=1;
   else if (lb==3 && !strcmp(par[2], "-l")) shmode=2;
   else if (lb==3 && !strcmp(par[2], "-R")) { shmode=1; follow = 1; }
   else if (lb==3 && !strcmp(par[2], "-L")) { shmode=2; follow = 1; }
   else if (lb==3 && !strcmp(par[2], "-P")) { shmode=0; follow = 1; }
   printf("Mode: %d, Follow: %d\n", shmode, follow);
   if (logf) fprintf(logf, "Mode: %d, Follow: %d\n", shmode, follow);
   cwd = par[1];
   if (logf && cwd) fprintf(logf, "Working on path: %s\n", cwd);
   err = generate_flist(par[1]);
   printf("\nFiles processed: %ld\n", glob_nfiles);
   if (logf) fprintf(logf, "Files processed: %ld\n", glob_nfiles);
  }
 if (err)
   {
    printf("Generate list error: %d\n", err);
    return 2;
   }
 printf("\nDone\n");
 shred_list();
 printf("Freeying resources used...\n");
 free_list();
 gettimeofday(&tv2, NULL);
 timersub(&tv2, &tv1, &tvd);
 llt = (ULL)tvd.tv_sec * 1000000 + (ULL)tvd.tv_usec;
 dblt = (double)llt / 1000000.;
 printf("Time: %lldus (%.3fs, %.1fm, %.1fh, %.1fd) cmpps: %f, avgcmp: %fms\n", llt, dblt, dblt/60., dblt/3600., dblt/86400., cmp/dblt, (1000.*dblt)/cmp); 
 if (logf) fprintf(logf, "Time: %lldus (%.3fs, %.1fm, %.1fh, %.1fd), cmpps: %f, avgcmp: %fms\n", llt, dblt, dblt/60., dblt/3600., dblt/86400., cmp/dblt, (1000.*dblt)/cmp); 
 close_log();
 return 0;
}

