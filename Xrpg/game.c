#include "xmwnd.h"
#include "dbase.h"
//this is ver 11 (I hope bugeless!)
// FOR THE ETERNAL GLORY OF              *** UNIX ***
static bool dbase = false;
static DATA* db = NULL;
static RPG* post = NULL;
void Compress(DATA*);
void Uncompress(DATA*);
void Compress(RPG*);
void Uncompress(RPG*);

bool DisplayChar(char* nname)
{
#ifdef DBG
 char dstr[LONG_STR];
 if (nname) sprintf(dstr,"DisplayChar(%p:%s)",nname,nname);
 else sprintf(dstr,"DisplayChar((char*)NULL)");
 tout(dstr);
#endif
 char name[STRING];
 strcpy(name, nname);
 downcase(name);
 int len = strlen(name);
 if ((name[0] == '_') || (name[0] == '-')) return false;;
 if (len <= 4) return true;
 if ((name[len-1] == 'n') && (name[len-2] == 'i') && (name[len-3] == 'b') && (name[len-4] == '_')) return false;
 if ((name[len-1] == 'o') && (name[len-2] == 's') && (name[len-3] == '.')) return false;
 if ((name[len-1] == 'x') && (name[len-2] == '.')) return false;
 if ((name[len-1] == 'n') && (name[len-2] == 'i') && (name[len-3] == 'b') && (name[len-4] == '.')) return false;
 else if ((name[len-1] == 'g') && (name[len-2] == 'o') && (name[len-3] == 'l') && (name[len-4] == '_')) return false;
 else if ((name[len-1] == 'f') && (name[len-2] == 'n') && (name[len-3] == 'i') && (name[len-4] == '_')) return false;
 else if ((name[len-1] == 'l') && (name[len-2] == 'm') && (name[len-3] == 't') && (name[len-4] == 'h')) return false;
 else if ((name[len-1] == 'm') && (name[len-2] == 't') && (name[len-3] == 'h')) return false;
 else if ((name[0] == 'k') && (name[1] == 'o') && (name[2] == 'p') && (name[3] == 'i')) return false;
 else return true;
}


char** ReadFolderToTable(int& n, char* kat)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"ReadFolderToTable(%d,%p:%s) num, directory_name",n,kat,kat);
 tout(dstr);
#endif
 int num = 0;
 DIR* dp;
 struct dirent* wpis;
 struct stat statbufor;
 if ((dp = opendir(kat)) == NULL) return NULL;
 chdir(kat);
 while ((wpis = readdir(dp)) != NULL)
        {
         stat(wpis->d_name, &statbufor);
         if ((!S_ISDIR(statbufor.st_mode))  && (DisplayChar(wpis->d_name))) num++;
        }
 chdir("..");
 closedir(dp);
 if (!num)
   {
    n = 0;
    return NULL;
   }
 char** tmp;
 tmp = new char*[num+1];
 if (!tmp) panic("memory_exception_in_new: tmp");
 for (int i=0;i<num;i++)
   {
    tmp[i] = new char[TINY_TXT];
    if (!tmp[i]) panic("memory_exception_in_new: tmp[]");
   }
 num = 0;
 if ((dp = opendir(kat)) == NULL) return NULL;
 chdir(kat);
 while ((wpis = readdir(dp)) != NULL)
        {
         stat(wpis->d_name, &statbufor);
         if ((!S_ISDIR(statbufor.st_mode))  && (DisplayChar(wpis->d_name)))
	   {
	    strcpy(tmp[num], wpis->d_name);
	    num++;
	   }
        }
 chdir("..");
 closedir(dp);
 n = num;
#ifdef DBG
  for (int i=0;i<n;i++)
    {
     sprintf(dstr,"tmp[%d]=%p:%s",i,tmp[i],tmp[i]);
     tout(dstr);
    }
#endif
 /*for (int i=0;i<n;i++)
   {
    printf("tmp[%d] = 0x%x :: %s\n", i, tmp[i], tmp[i]);
   }
 printf("tmp = 0x%x\n", tmp);*/
 return tmp;
}


inline void __unimp(Wnd& wnd)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"__unimp(window=%p)",&wnd);
 tout(dstr);
#endif
 wnd.CreateMBoxChild("Ta opcja jest dopiero tworzona!\nPogadaj z programista");
}


void Protection(int check, DATA* db, FILE* plik, Wnd& wnd)
{
 if (!plik)
   {
    wnd.CreateMBoxChild("Struktura \"FILE*\" wskazuje NULL!");
    return;
   }
 if (!check)
        {
         int zn = wnd.CreateYesNoChild("Plik bazy danych jest uszkodzony!\nKontynuowac?");
         if (!zn) {if (db) delete db;panic("Baza danych uszkodzona!"); fclose(plik); plik = NULL;}
        }
}


void LoadDBFromFile(DATA* db,FILE* plik, char* bnm, Wnd& w)
{
 tout("LoadDBFromFile");
 if (!plik)
   {
    w.CreateMBoxChild("Struktura \"FILE*\" wskazuje NULL!");
    return;
   }
 int check;
 check = fscanf(plik,"%d\n",&db->ilosc[0]);
 Protection(check,db,plik,w);
 for (int i=0;i<db->ilosc[0];i++)
        {
         fscanf(plik,"%d %s\n",&db->value_att[i],db->name_att[i]);
         DecompressData(db->name_att[i]);
        }
 fscanf(plik,"%d\n",&db->ilosc[1]);
 check = fscanf(plik,"%d\n",&db->ilosc[2]);
 for (int i=0;i<db->ilosc[1];i++)
        {
         fscanf(plik,"%s ",db->name[0][i]);
         DecompressData(db->name[0][i]);
         for (int j=0;j<db->ilosc[0];j++)
                {
                 fscanf(plik,"%d ",&db->mod[0][i][j]);
                }
         fscanf(plik,"\n");
        }
 for (int i=0;i<db->ilosc[2];i++)
        {
         fscanf(plik,"%s ",db->name[1][i]);
         DecompressData(db->name[1][i]);
         for (int j=0;j<db->ilosc[0];j++)
                {
                 fscanf(plik,"%d ",&db->mod[1][i][j]);
                }
         fscanf(plik,"\n");
        }
 check = fscanf(plik,"%d\n",&db->ilosc[3]);
 Protection(check,db,plik,w);
 for (int i=0;i<db->ilosc[3];i++)
        {
         fscanf(plik,"%s ",db->s_name[i]);
         DecompressData(db->s_name[i]);
         fscanf(plik,"%d %d %d %d %d\n",&db->s_mod[i][0],&db->s_mod[i][1],&db->s_mod[i][2],&db->s_mod[i][3],&db->s_mod[i][4]);
        }
 fscanf(plik,"%d\n",&db->ilosc[4]);
 for (int i=0;i<db->ilosc[4];i++)
        {
         fscanf(plik,"%s\n",db->c_name[i]);
         DecompressData(db->c_name[i]);
        }
 fscanf(plik,"%d\n",&db->ilosc[5]);
 for (int i=0;i<db->ilosc[5];i++)
        {
         fscanf(plik,"%d\n",&db->cost[i]);
        }
 fscanf(plik,"%d\n",&db->sv);
 fscanf(plik,"%d\n",&db->st);
 fscanf(plik,"%d\n",&db->ilosc[6]);
 for (int i=0;i<db->ilosc[6];i++)
        {
         fscanf(plik,"%d\n",&db->pd_cost[i]);
        }
 for (int i=0;i<db->ilosc[0];i++)
        {
         fscanf(plik,"%d\n",&db->p_mod[i]);
        }
 fscanf(plik,"%d\n",&db->lr);
 for (int i=0;i<db->ilosc[3];i++)
        {
         fscanf(plik,"%d\n",&db->s_mod[i][MAGIC]);
        }
 fscanf(plik,"%d\n",&db->mm);
 fscanf(plik,"%s\n",db->dbname);
 DecompressData(db->dbname);
 fscanf(plik,"%d\n",&db->def_cash);
 fscanf(plik,"%d\n",&db->ilosc_ekw);
 for (int i=0;i<db->ilosc_ekw;i++)
 	{
    fscanf(plik,"%s\n",db->ekw[i]);
    DecompressData(db->ekw[i]);
    fscanf(plik,"%d\n",&db->c_ekw[i]);
   }
  check = fscanf(plik,"%d\n",&db->ilosc_poziomow);
  Protection(check,db,plik,w);
  for (int i=0;i<db->ilosc_poziomow;i++)
        {
         fscanf(plik,"%d\n",&db->ilosc_czarow[i]);
         for (int j=0;j<db->ilosc_czarow[i];j++)
                {
                 fscanf(plik,"%d\n",&db->spell_cost[i][j]);
                 fscanf(plik,"%s\n",db->req_magic[i][j]);
                 DecompressData(db->req_magic[i][j]);
                 fscanf(plik,"%s\n",db->zaklecie[i][j]);
                 DecompressData(db->zaklecie[i][j]);
                }
        }
 dbase = true;
 Uncompress(db);
}


void Compress(DATA* db)
{
 tout("Compress(db)");
 for (int i=0;i<db->ilosc[0];i++)
        {
         CompressData(db->name_att[i]);
	 if (!strcmp(db->name_att[i],"")) strcpy(db->name_att[i],"-");
        }
 for (int i=0;i<db->ilosc[1];i++)
        {
         CompressData(db->name[0][i]);
	 if (!strcmp(db->name[0][i],"")) strcpy(db->name[0][i],"-");
        }
 for (int i=0;i<db->ilosc[2];i++)
        {
         CompressData(db->name[1][i]);
	 if (!strcmp(db->name[1][i],"")) strcpy(db->name[1][i],"-");
        }
 for (int i=0;i<db->ilosc[3];i++)
        {
         CompressData(db->s_name[i]);
	 if (!strcmp(db->s_name[i],"")) strcpy(db->s_name[i],"-");
        }
 for (int i=0;i<db->ilosc[4];i++)
        {
         CompressData(db->c_name[i]);
	 if (!strcmp(db->c_name[i],"")) strcpy(db->c_name[i],"-");
        }
 for (int i=0;i<db->ilosc_ekw;i++)
        {
         CompressData(db->ekw[i]);
	 if (!strcmp(db->ekw[i],"")) strcpy(db->ekw[i],"-");
        }
 for (int i=0;i<db->ilosc_poziomow;i++)
        {
         for (int j=0;j<db->ilosc_czarow[i];j++)
                {
                 CompressData(db->req_magic[i][j]);
		 if (!strcmp(db->req_magic[i][j],"")) strcpy(db->req_magic[i][j],"-");
                 CompressData(db->zaklecie[i][j]);
		 if (!strcmp(db->zaklecie[i][j],"")) strcpy(db->zaklecie[i][j],"-");
                }
        }
 CompressData(db->dbname);
 if (!strcmp(db->dbname,"")) strcpy(db->dbname,"-");
}


void Uncompress(DATA* db)
{
 tout("Uncompress(db)");
 for (int i=0;i<db->ilosc[0];i++)
        {
         DecompressData(db->name_att[i]);
	 if (!strcmp(db->name_att[i],"")) strcpy(db->name_att[i],"-");
        }
 for (int i=0;i<db->ilosc[1];i++)
        {
         DecompressData(db->name[0][i]);
	 if (!strcmp(db->name[0][i],"")) strcpy(db->name[0][i],"-");
        }
 for (int i=0;i<db->ilosc[2];i++)
        {
         DecompressData(db->name[1][i]);
	 if (!strcmp(db->name[1][i],"")) strcpy(db->name[1][i],"-");
        }
 for (int i=0;i<db->ilosc[3];i++)
        {
         DecompressData(db->s_name[i]);
	 if (!strcmp(db->s_name[i],"")) strcpy(db->s_name[i],"-");
        }
 for (int i=0;i<db->ilosc[4];i++)
        {
         DecompressData(db->c_name[i]);
	 if (!strcmp(db->c_name[i],"")) strcpy(db->c_name[i],"-");
        }
 for (int i=0;i<db->ilosc_ekw;i++)
        {
         DecompressData(db->ekw[i]);
	 if (!strcmp(db->ekw[i],"")) strcpy(db->ekw[i],"-");
        }
 for (int i=0;i<db->ilosc_poziomow;i++)
        {
         for (int j=0;j<db->ilosc_czarow[i];j++)
                {
                 DecompressData(db->req_magic[i][j]);
		 if (!strcmp(db->req_magic[i][j],"")) strcpy(db->req_magic[i][j],"-");
                 DecompressData(db->zaklecie[i][j]);
		 if (!strcmp(db->zaklecie[i][j],"")) strcpy(db->zaklecie[i][j],"-");
                }
        }
 DecompressData(db->dbname);
 if (!strcmp(db->dbname,"")) strcpy(db->dbname,"-");
}


void SaveDBToFile(DATA* db, FILE* plik, char* bnm)
{
 if (!plik) return;
 tout("SaveDBToFile");
 Compress(db);
 fprintf(plik,"%d\n",db->ilosc[0]);
 for (int i=0;i<db->ilosc[0];i++)
        {
         CompressData(db->name_att[i]);
         fprintf(plik,"%d %s\n",db->value_att[i],db->name_att[i]);
        }
 fprintf(plik,"%d\n",db->ilosc[1]);
 fprintf(plik,"%d\n",db->ilosc[2]);
 for (int i=0;i<db->ilosc[1];i++)
        {
         CompressData(db->name[0][i]);
         fprintf(plik,"%s ",db->name[0][i]);
         for (int j=0;j<db->ilosc[0];j++)
                {
                 fprintf(plik,"%d ",db->mod[0][i][j]);
                }
         fprintf(plik,"\n");
        }
 for (int i=0;i<db->ilosc[2];i++)
        {
         CompressData(db->name[1][i]);
         fprintf(plik,"%s ",db->name[1][i]);
         for (int j=0;j<db->ilosc[0];j++)
                {
                 fprintf(plik,"%d ",db->mod[1][i][j]);
                }
         fprintf(plik,"\n");
        }
 fprintf(plik,"%d\n",db->ilosc[3]);
 for (int i=0;i<db->ilosc[3];i++)
        {
         CompressData(db->s_name[i]);
         fprintf(plik,"%s ",db->s_name[i]);
         fprintf(plik,"%d %d %d %d %d\n",db->s_mod[i][0],db->s_mod[i][1],db->s_mod[i][2],db->s_mod[i][3],db->s_mod[i][4]);
        }
 fprintf(plik,"%d\n",db->ilosc[4]);
 for (int i=0;i<db->ilosc[4];i++)
        {
         CompressData(db->c_name[i]);
         fprintf(plik,"%s\n",db->c_name[i]);
        }
 fprintf(plik,"%d\n",db->ilosc[5]);
 for (int i=0;i<db->ilosc[5];i++)
        {
         fprintf(plik,"%d\n",db->cost[i]);
        }
 fprintf(plik,"%d\n",db->sv);
 fprintf(plik,"%d\n",db->st);
fprintf(plik,"%d\n",db->ilosc[6]);
 for (int i=0;i<db->ilosc[6];i++)
        {
         fprintf(plik,"%d\n",db->pd_cost[i]);
        }
 for (int i=0;i<db->ilosc[0];i++)
        {
         fprintf(plik,"%d\n",db->p_mod[i]);
        }
 fprintf(plik,"%d\n",db->lr);
  for (int i=0;i<db->ilosc[3];i++)
        {
         fprintf(plik,"%d\n",db->s_mod[i][MAGIC]);
        }
 fprintf(plik,"%d\n",db->mm);
 CompressData(db->dbname);
 fprintf(plik,"%s\n",db->dbname);
 fprintf(plik,"%d\n",db->def_cash);
 fprintf(plik,"%d\n",db->ilosc_ekw);
 for (int i=0;i<db->ilosc_ekw;i++)
 	{
    CompressData(db->ekw[i]);
    fprintf(plik,"%s\n",db->ekw[i]);
    fprintf(plik,"%d\n",db->c_ekw[i]);
   }
  fprintf(plik,"%d\n",db->ilosc_poziomow);
  for (int i=0;i<db->ilosc_poziomow;i++)
        {
         fprintf(plik,"%d\n",db->ilosc_czarow[i]);
         for (int j=0;j<db->ilosc_czarow[i];j++)
                {
                 fprintf(plik,"%d\n",db->spell_cost[i][j]);
                 CompressData(db->req_magic[i][j]);
                 fprintf(plik,"%s\n",db->req_magic[i][j]);
                 CompressData(db->zaklecie[i][j]);
                 fprintf(plik,"%s\n",db->zaklecie[i][j]);
                }
        }
 Uncompress(db);
}


int SaveBase(Wnd& wnd)
{
 tout("SaveBase");
 FILE* plik = NULL;
 if ((plik = fopen(db->dbname,"w"))==NULL)
        {
         wnd.CreateMBoxChild("Nie moge zapisac bazy w pliku");
	 //wnd.Invalidate();
	 return 0;
        }
 SaveDBToFile(db,plik, db->dbname);
 if (plik) fclose(plik);
 plik = NULL;
 //wnd.Invalidate();
 return 1;
}


int LoadBase(Wnd& wnd)
{
 tout("LoadBase");
 FILE* plik = NULL;
 if ((plik = fopen(db->dbname,"r"))==NULL)
        {
         wnd.CreateMBoxChild("Nie moge odczytac bazy danych");
	 //wnd.Invalidate();
	 dbase = false;
	 return 0;
        }
 LoadDBFromFile(db,plik, db->dbname,wnd);
 if (plik) fclose(plik);
 plik = NULL;
 //wnd.Invalidate();
 return 1;
}


int CheckPassword(DATA* db, Wnd& wnd)
{
 tout("CheckPassword(db)");
 if (db->sv == -1)
   {
    wnd.CreateMBoxChild("Baza nie jest dostepna do edycji!");
    //wnd.Invalidate();
    return 0;
   }
 if (db->sv != 0)
        {
         char* tmp = wnd.CreateWriteBox("Podaj kod dostepu", true);
	 int a = 0;
	 sscanf(tmp,"%d", &a);
	 //wnd.Invalidate();
         if (a != db->sv)
                {
                 wnd.CreateMBoxChild("Podano bledny kod!");
		 //wnd.Invalidate();
		 return 0;
                }
	 else return 1;
        }
 return 1;
}


int CreateDB(char* name, bool ab_new = false)
{
 tout("CreateDB");
 char temp[STRING];
 strcpy(temp,"DataBase/");
 strcat(temp,name);
 db = new DATA;
 if (!db) panic("memory_exception_in_new: db");
 strcpy(db->dbname,temp);
 if (ab_new)
   {
    FILE* test = NULL;
    if ((test = fopen(db->dbname,"r")) != NULL)
      {
       if (test) fclose(test);
       test = NULL;
       return 0;
      }
   }
 return 1;
}


int LoadDB(char* name, Wnd& wnd)
{
 tout("LoadDB");
 FILE* plik = NULL;
 if ((plik = fopen(db->dbname,"r")) == NULL)
        {
         wnd.CreateMBoxChild("Nie moge wczytac podanej bazy danych!");
	 //wnd.Invalidate();
	 dbase = false;
	 return 0;
        }
 LoadDBFromFile(db,plik, db->dbname, wnd);
 db->lr++;
 if (plik) fclose(plik);
 plik = NULL;
 if ((plik = fopen(db->dbname,"w"))==NULL)
        {
         wnd.CreateMBoxChild("Nie moge zapisac bazy w pliku");
	 //wnd.Invalidate();
	 return 1;
        }
 SaveDBToFile(db,plik, db->dbname);
 if (plik) fclose(plik);
 plik = NULL;
 //wnd.Invalidate();
 return 1;
}


void LoadFunc(Wnd& wnd, void* ptr)
{
 tout("LoadFunc");
 int num = 0;
 char** table = ReadFolderToTable(num, "./DataBase");
  if (!table)
   {
    wnd.CreateMBoxChild("Nie podales nazwy bazy\nMozliwe ze brak folderu DataBase\nwiec nie pojawilo sie okienko wyboru :(\nit will cause PANIC");
    trace("cannot read dir_entires for ./DataBase!");
    //wnd.Invalidate();
   }
 if (!num)
   {
    wnd.CreateMBoxChild("Brak plikow w folderze: ./DataBase");
    return ;
   }
 FSort(table, num);
 char* selected = wnd.CreateCheckBoxChild("Wybierz Baze danych:", table, num);
 CreateDB(selected);
 int ok = LoadDB(selected, wnd);
 if (!ok) return ;
 wnd.AboutQuit(1);
 dbase = true;
 //wnd.Invalidate();
}


void EraseFunc(Wnd& wnd, void* ptr)
{
 tout("EraseFunc");
 int num = 0;
 char** table = ReadFolderToTable(num, "./DataBase");
  if (!table)
   {
    wnd.CreateMBoxChild("Nie podales nazwy bazy\nMozliwe ze brak folderu DataBase\nwiec nie pojawilo sie okienko wyboru :(\nit will cause PANIC");
    trace("cannot read dir_entires for ./DataBase!");
    //wnd.Invalidate();
   }
 if (!num)
   {
    wnd.CreateMBoxChild("Brak plikow w folderze: ./DataBase");
    return ;
   }
 FSort(table, num);
 char* selected = wnd.CreateCheckBoxChild("Wybierz Baze danych:", table, num);
 CreateDB(selected);
 LoadDB(selected, wnd);
 int ok = CheckPassword(db, wnd);
 if (!ok)
    {
     wnd.CreateMBoxChild("Blad procedury dostepu!");
     if (db) delete db;
     db = NULL;
     dbase = false;
     //wnd.Invalidate();
     return;
    }
 char todel[STRING];
 sprintf(todel,"rm -f ./DataBase/%s", selected);
 ok = wnd.CreateYesNoChild("Na pewno usunac baze danych?");
 if (!ok)
   {
    wnd.DoQuit();
    if (db) delete db;
    db = NULL;
    dbase = false;
    return;
   }
 system(todel);
 wnd.AboutQuit(1);
 dbase = false;
 if (db) delete db;
 db = NULL;
 //wnd.Invalidate();
}


void ChangeBasePswd(Wnd& wnd, void* ptr)
{
 tout("CheckBasePswd");
 char* new_pass = wnd.CreateWriteBox("Podaj nowy kod\n0 -brak kodu\n-1 -odmowa dostepu", true);
 int val = 0;
 if (!new_pass) return ;
 sscanf(new_pass,"%d", &val);
 db->sv = val;
 SaveBase(wnd);
 //wnd.Invalidate();
}


void BaseToHTML(FILE* plik, char* name)
{
 if (!plik) return;
 tout("BaseToHTML");
 fprintf(plik,"<html>\n");
 fprintf(plik,"<head>\n");
 fprintf(plik,"<title>\n");
 fprintf(plik,"Data Base %s\n",name);
 fprintf(plik,"</title>\n");
 fprintf(plik,"</head>\n");
 fprintf(plik,"<body>\n");
 fprintf(plik,"<center><big>Data Base %s</center></big>\n",name);
 fprintf(plik,"<br>\n");
 fprintf(plik,"<center>Number of attributes %d</center>\n",db->ilosc[0]);
 fprintf(plik,"<center><table border =\"3\">\n");
 fprintf(plik,"<tr>\n");
 fprintf(plik,"<td>Number\n");
 fprintf(plik,"<td>Attribute\n");
 fprintf(plik,"<td>Start Value\n");
 for (int i=0;i<db->ilosc[0];i++)
        {
         fprintf(plik,"<tr>\n");
         fprintf(plik,"<td>%d\n",i+1);
         fprintf(plik,"<td>%s\n",db->name_att[i]);
         fprintf(plik,"<td>%d\n",db->value_att[i]);
        }
 fprintf(plik,"</table></center>\n");
 fprintf(plik,"<br>\n");
 fprintf(plik,"<center>Number of races %d</center>\n",db->ilosc[1]);
 fprintf(plik,"<center><table border =\"3\">\n");
 fprintf(plik,"<tr>\n");
 fprintf(plik,"<td>Number\n");
 fprintf(plik,"<td>Race\n");
 for (int i=0;i<db->ilosc[0];i++)
        {
         fprintf(plik,"<td>%c%c%c\n",db->name_att[i][0],db->name_att[i][1],db->name_att[i][2]);
        }
 for (int i=0;i<db->ilosc[1];i++)
        {
         fprintf(plik,"<tr>\n");
         fprintf(plik,"<td>%d\n",i+1);
         fprintf(plik,"<td>%s\n",db->name[0][i]);
         for (int j=0;j<db->ilosc[0];j++)
                {
                 fprintf(plik,"<td>%d\n",db->mod[0][i][j]);
                }
        }
 fprintf(plik,"</table></center>\n");
 fprintf(plik,"<br>\n");
 fprintf(plik,"<center>Number of professies %d</center>\n",db->ilosc[2]);
 fprintf(plik,"<center><table border =\"3\">\n");
 fprintf(plik,"<tr>\n");
 fprintf(plik,"<td>Number\n");
 fprintf(plik,"<td>Profession\n");
 for (int i=0;i<db->ilosc[0];i++)
        {
         fprintf(plik,"<td>%c%c%c\n",db->name_att[i][0],db->name_att[i][1],db->name_att[i][2]);
        }
 for (int i=0;i<db->ilosc[2];i++)
        {
         fprintf(plik,"<tr>\n");
         fprintf(plik,"<td>%d\n",i+1);
         fprintf(plik,"<td>%s\n",db->name[1][i]);
         for (int j=0;j<db->ilosc[0];j++)
                {
                 fprintf(plik,"<td>%d\n",db->mod[1][i][j]);
                }
        }
 fprintf(plik,"</table></center>\n");
 fprintf(plik,"<br>\n");
 fprintf(plik,"<center>Number of skills %d</center>\n",db->ilosc[3]);
 fprintf(plik,"<center><table border =\"3\">\n");
 fprintf(plik,"<tr>\n");
 fprintf(plik,"<td>Number\n");
 fprintf(plik,"<td>Skil\n");
 fprintf(plik,"<td>Multiple\n");
 fprintf(plik,"<td>Num.Attrib\n");
 fprintf(plik,"<td>Attrib\n");
 fprintf(plik,"<td>Mod\n");
 fprintf(plik,"<td>Diff\n");
 fprintf(plik,"<td>Num.Categ\n");
 fprintf(plik,"<td>Category\n");
 fprintf(plik,"<td>Magic\n");
 for (int i=0;i<db->ilosc[3];i++)
        {
         fprintf(plik,"<tr>\n");
         fprintf(plik,"<td>%d\n",i+1);
         fprintf(plik,"<td>%s\n",db->s_name[i]);
         fprintf(plik,"<td>%d\n",db->s_mod[i][MULTIPLE]);
         fprintf(plik,"<td>%d\n",db->s_mod[i][ATTRIBUTE]);
         fprintf(plik,"<td>%s\n",db->name_att[db->s_mod[i][ATTRIBUTE]]);
         fprintf(plik,"<td>%d\n",db->s_mod[i][MOD]);
         fprintf(plik,"<td>%d\n",db->s_mod[i][DIFF]);
         fprintf(plik,"<td>%d\n",db->s_mod[i][CATEG]);
         fprintf(plik,"<td>%s\n",db->c_name[db->s_mod[i][CATEG]]);
         fprintf(plik,"<td>%d\n",db->s_mod[i][MAGIC]);
        }
 fprintf(plik,"</table></center>\n");
 fprintf(plik,"<br>\n");
 fprintf(plik,"<center>Number of categories %d</center>\n",db->ilosc[4]);
 fprintf(plik,"<center><table border =\"3\">\n");
 fprintf(plik,"<tr>\n");
 fprintf(plik,"<td>Number\n");
 fprintf(plik,"<td>category\n");
 for (int i=0;i<db->ilosc[4];i++)
        {
         fprintf(plik,"<tr>\n");
         fprintf(plik,"<td>%d\n",i+1);
         fprintf(plik,"<td>%s\n",db->c_name[i]);
        }
 fprintf(plik,"</table></center>\n");
 fprintf(plik,"<br>\n");
 fprintf(plik,"<center>Number of skill levels %d</center>\n",db->ilosc[5]);
 fprintf(plik,"<center><table border =\"3\">\n");
 fprintf(plik,"<tr>\n");
 fprintf(plik,"<td>Level\n");
 fprintf(plik,"<td>Cost in SP\n");
 for (int i=0;i<db->ilosc[5];i++)
        {
         fprintf(plik,"<tr>\n");
         fprintf(plik,"<td>%d\n",i);
         fprintf(plik,"<td>%d\n",db->cost[i]);
        }
 fprintf(plik,"</table></center>\n");
 fprintf(plik,"<br>\n");
 fprintf(plik,"<center>Number of Experience levels %d</center>\n",db->ilosc[6]);
 fprintf(plik,"<center><table border =\"3\">\n");
 fprintf(plik,"<tr>\n");
 fprintf(plik,"<td>Level\n");
 fprintf(plik,"<td>Cost in XP\n");
 for (int i=0;i<db->ilosc[6];i++)
        {
         fprintf(plik,"<tr>\n");
         fprintf(plik,"<td>%d\n",i);
         fprintf(plik,"<td>%d\n",db->pd_cost[i]);
        }
 fprintf(plik,"</table></center>\n");
 fprintf(plik,"<br>\n");
 fprintf(plik,"<center>Gender modifier(woman)</center>\n");
 fprintf(plik,"<center><table border =\"3\">\n");
 fprintf(plik,"<tr>\n");
 fprintf(plik,"<td>Attrib\n");
 fprintf(plik,"<td>Modifier\n");
 for (int i=0;i<db->ilosc[0];i++)
        {
         fprintf(plik,"<tr>\n");
         fprintf(plik,"<td>%s\n",db->name_att[i]);
         fprintf(plik,"<td>%d\n",db->p_mod[i]);
        }
 fprintf(plik,"</table></center>\n");
 fprintf(plik,"<br>\n");
 fprintf(plik,"<center>Items in DataBase</center>\n");
 fprintf(plik,"<center><table border =\"3\">\n");
 fprintf(plik,"<br>\n");
 fprintf(plik,"<center>Number of equipment %d. Default cash for character: %d</center>\n",db->ilosc_ekw,db->def_cash);
 fprintf(plik,"<center><table border =\"3\">\n");
 fprintf(plik,"<tr>\n");
 fprintf(plik,"<td>Cost\n");
 fprintf(plik,"<td>Item\n");
 for (int i=0;i<db->ilosc_ekw;i++)
 	{
    fprintf(plik,"<tr><td>%d Cu\n<td>%s",db->c_ekw[i],db->ekw[i]);
   }
 fprintf(plik,"</table></center>\n");
 fprintf(plik,"<br>\n");
 fprintf(plik,"<center>Spells in DataBase</center>\n");
 fprintf(plik,"<center>Maximum levels is %d\n",db->ilosc_poziomow);
 for (int i=0;i<db->ilosc_poziomow;i++)
 {
 fprintf(plik,"<center> Spells Level %d\n",i);
 fprintf(plik,"<center><table border =\"3\">\n");
 fprintf(plik,"<tr>\n");
 fprintf(plik,"<td>Lp.\n");
 fprintf(plik,"<td>Spell name\n");
 fprintf(plik,"<td>Required magic\n");
 fprintf(plik,"<td>Spell Points Cost\n");
 for (int j=0;j<db->ilosc_czarow[i];j++)
 	{
               fprintf(plik,"<tr>\n");
               fprintf(plik,"<td>%d\n",j);
               fprintf(plik,"<td>%s\n",db->zaklecie[i][j]);
               fprintf(plik,"<td>%s\n",db->req_magic[i][j]);
               fprintf(plik,"<td>%d SP\n",db->spell_cost[i][j]);
              }
 fprintf(plik,"</table></center>\n");
 fprintf(plik,"<br>\n");
 }
 fprintf(plik,"<center>Data Base %s generated by rpg by morgothdbma copyleft@</center>\n",name);
 fprintf(plik,"</body>\n");
 fprintf(plik,"</html>\n");
}

void SortAll(Wnd&, void*);

void GenerateHTML(Wnd& wnd, void* ptr)
{
 tout("GenerateHTML");
 char name[100];
 strcpy(name,db->dbname);
 strcat(name,".html");
 FILE* html = NULL;
 if ((html = fopen(name,"r")) != NULL)
        {
	 char str[70];
	 sprintf(str,"Baza danych %s istnieje\n Nadpisac?", name);
         int ans = wnd.CreateYesNoChild(str);
	 //wnd.Invalidate();
	 if (!ans)
	   {
	    if (html) fclose(html);
	    html = NULL;
	    return ;
	   }
        }
 if ((html = fopen(name,"w")) == NULL)
        {
         wnd.CreateMBoxChild("Nie moge zapisac pliku HTML");
	 //wnd.Invalidate();
	 return ;
        }
 SortAll(wnd, NULL);
 BaseToHTML(html,name);
 if (html) fclose(html);
 html = NULL;
}


void CreateGenderText(Wnd& wnd, void* ptr)
{
 tout("CreateGenderText");
 char tmp[4096];
 char tmp2[100];
 sprintf(tmp, "modyfikatory plci dla kobiety.\n");
 for (int i=0;i<db->ilosc[0];i++)
     {
      sprintf(tmp2,"(%-3d)%-25s  %-3d\n",i,db->name_att[i],db->p_mod[i]);
      strcat(tmp, tmp2);
     }
 wnd.PrepareWndText(tmp);
 wnd.BlackWings();
 wnd.Invalidate();
}


void EnterGModifier(Wnd& wnd, void* ptr)
{
 tout("EnterGModifier");
 char tmp[STRING];
 int val=0;
 sprintf(tmp,"Podaj modyfikator kobiety dla %s", db->name_att[wnd.RetPressed()]);
 char* mod = wnd.CreateWriteBox(tmp);
 sscanf(mod,"%d", &val);
 db->p_mod[wnd.RetPressed()] = val;
 wnd.ParentCallFunc(CreateGenderText);
}


void EditGenderField(Wnd& wnd, void* ptr)
{
 tout("EditGenderField");
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 220, 400);
 if (!wybor) panic("memory exception in creating win_child!");
 wybor->WinDefaults("Edytuj plec");
 wybor->SetButtonsNumber(db->ilosc[0]+1);
 char tmp[STRING];
 for (int i=0;i<db->ilosc[0];i++)
   {
    sprintf(tmp,"Edytuj %s", db->name_att[i]);
    if (!i) wybor->CreateButton(0, tmp, 20 , 20, EnterGModifier, true, 180);
    else wybor->CreateButton(i, tmp, 20 , 20+i*26, EnterGModifier, false, 180);
   }
 wybor->CreateButton(db->ilosc[0], "Powrot", 50 , 25+(db->ilosc[0]*26), StdOKFunc, false, 120);
 wybor->SetParent(&wnd);
 wybor->DefaultResponse();
 wnd.AboutQuit(1);
 wnd.Invalidate();
}


void GenderEdit(Wnd& wnd, void* ptr)
{
 tout("GenderEdit");
 char tmp[4096];
 char tmp2[100];
 sprintf(tmp, "modyfikatory plci dla kobiety.\n");
 for (int i=0;i<db->ilosc[0];i++)
     {
      sprintf(tmp2,"(%-3d)%-25s  %-3d\n",i,db->name_att[i],db->p_mod[i]);
      strcat(tmp, tmp2);
     }
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 300, 400);
 if (!wybor) panic("memory exception in creating win_child!");
 wybor->CreateGC();
 wybor->SetFont("*-helvetica-*-12-*");
 wybor->SetCaption("Modyfikatory plci: kobieta");
 wybor->DeclareResponseTable(MEDI_RESPONSES);
 wybor->SelectExposeFunc(StdExposeFunc);
 wybor->SelectConfigureFunc(DenyResizeFunc);
 wybor->SelectKeyPressFunc(eXtendedKeyPress);
 wybor->SelectKeyReleaseFunc(eXtendedKeyRelease);
 wybor->SelectButtonPressFunc(eXtendedButtonPress);
 wybor->SelectButtonReleaseFunc(eXtendedButtonRelease);
 wybor->PrepareWndText(tmp);
 wybor->StdTextWrite();
 wybor->SetButtonsNumber(2);
 wybor->CreateButton(0, "Edytuj", 35 , 350, EditGenderField, true);
 wybor->CreateButton(1, "Powrot", 175 , 350, StdOKFunc, false);
 wybor->SetParent(&wnd);
 wybor->DefaultResponse();
 wnd.Invalidate();
}


void FSaveBase(Wnd& wnd, void* ptr)
{
 tout("FSaveBase");
 int val = SaveBase(wnd);
 if (!val) wnd.CreateMBoxChild("Nie udalo sie zapisac bazy");
}


void AboutSave(Wnd& wnd, void* ptr)
{
 tout("AboutSave");
 int sav = wnd.CreateYesNoChild("Zapisac baze?");
 if (sav) FSaveBase(wnd, NULL);
}


void EnterRaceModifier(Wnd& wnd, void* ptr)
{
 tout("EnterRaceModifier");
 char tmp[STRING];
 int val=0;
 sprintf(tmp,"Podaj modyfikator %s dla %s", db->name[0][db->ilosc[1]], db->name_att[wnd.RetPressed()]);
 char* mod = wnd.CreateWriteBox(tmp);
 sscanf(mod,"%d", &val);
 db->mod[0][db->ilosc[1]][wnd.RetPressed()] = val;
}


void EnterProfModifier(Wnd& wnd, void* ptr)
{
 tout("EnterProfModifier");
 char tmp[STRING];
 int val=0;
 sprintf(tmp,"Podaj modyfikator %s dla %s", db->name[1][db->ilosc[2]], db->name_att[wnd.RetPressed()]);
 char* mod = wnd.CreateWriteBox(tmp);
 sscanf(mod,"%d", &val);
 db->mod[1][db->ilosc[2]][wnd.RetPressed()] = val;
}


void EditRaceField(Wnd& wnd, void* ptr)
{
 tout("EditRaceField");
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 220, 400);
 if (!wybor) panic("memory exception in creating win_child!");
 wybor->WinDefaults("Edytuj rase");
 wybor->SetButtonsNumber(db->ilosc[0]+1);
 char tmp[STRING];
 for (int i=0;i<db->ilosc[0];i++)
   {
    sprintf(tmp,"Edytuj %s", db->name_att[i]);
    if (!i) wybor->CreateButton(0, tmp, 20 , 20, EnterRaceModifier, true, 180);
    else wybor->CreateButton(i, tmp, 20 , 20+i*26, EnterRaceModifier, false, 180);
   }
 wybor->CreateButton(db->ilosc[0], "Powrot", 50 , 25+(db->ilosc[0]*26), StdOKFunc, false, 120);
 wybor->SetParent(&wnd);
 wybor->DefaultResponse();
 //wnd.AboutQuit(1);
 wnd.Invalidate();
}


void EditProfField(Wnd& wnd, void* ptr)
{
 tout("EditProfField");
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 220, 400);
 if (!wybor) panic("memory exception in creating win_child!");
 wybor->WinDefaults("Edytuj profesje");
 wybor->SetButtonsNumber(db->ilosc[0]+1);
 char tmp[STRING];
 for (int i=0;i<db->ilosc[0];i++)
   {
    sprintf(tmp,"Edytuj %s", db->name_att[i]);
    if (!i) wybor->CreateButton(0, tmp, 20 , 20, EnterProfModifier, true, 180);
    else wybor->CreateButton(i, tmp, 20 , 20+i*26, EnterProfModifier, false, 180);
   }
 wybor->CreateButton(db->ilosc[0], "Powrot", 50 , 25+(db->ilosc[0]*26), StdOKFunc, false, 120);
 wybor->SetParent(&wnd);
 wybor->DefaultResponse();
 //wnd.AboutQuit(1);
 wnd.Invalidate();
}


void AddItemFunc(Wnd& wnd, void* ptr)
{
 tout("AddItemFunc");
 if (db->ilosc_ekw == MAX_EKW)
    {
     wnd.CreateMBoxChild("Osiagnieto maksymalna ilosc ekwipunku");
     return;
    }
 int temp = db->ilosc_ekw;
 strcpy(db->ekw[temp], wnd.CreateWriteBox("Podaj nazwe przedmiotu"));
 if (!strcmp(db->ekw[temp],"")) {wnd.CreateMBoxChild("Nazwa nie moze byc pusta"); return;}
 char* str = wnd.CreateWriteBox("Podaj cene przedmiotu:");
 int val=0;
 sscanf(str,"%d", &val);
 db->c_ekw[temp] = val;
 db->ilosc_ekw++;
}


void AddRaceFunc(Wnd& wnd, void* ptr)
{
 tout("AddRaceFunc");
 if (db->ilosc[1] == MAX_TYPE)
    {
     wnd.CreateMBoxChild("Osiagnieto maksymalna ilosc ras");
     return;
    }
 int temp = db->ilosc[1];
 strcpy(db->name[0][temp], wnd.CreateWriteBox("Podaj nazwe rasy"));
 if (!strcmp(db->name[0][temp],"")) {wnd.CreateMBoxChild("Nazwa nie moze byc pusta"); return;}
 EditRaceField(wnd, NULL);
 db->ilosc[1]++;
}


void SwapSkill(int i, int j)
{
 tout("SwapSkill");
 char pom[STRING];
 int mod[6];
 strcpy(pom,db->s_name[j]);
 mod[0] =   db->s_mod[j][MULTIPLE];
 mod[1] =   db->s_mod[j][ATTRIBUTE];
 mod[2] =   db->s_mod[j][MOD];
 mod[3] =   db->s_mod[j][DIFF];
 mod[4] =   db->s_mod[j][CATEG];
 mod[5] =   db->s_mod[j][MAGIC];
 strcpy(db->s_name[j],db->s_name[i]);
 db->s_mod[j][MULTIPLE]   =   db->s_mod[i][MULTIPLE];
 db->s_mod[j][ATTRIBUTE]  =   db->s_mod[i][ATTRIBUTE];
 db->s_mod[j][MOD]        =   db->s_mod[i][MOD];
 db->s_mod[j][DIFF]       =   db->s_mod[i][DIFF];
 db->s_mod[j][CATEG]      =   db->s_mod[i][CATEG];
 db->s_mod[j][MAGIC]      =   db->s_mod[i][MAGIC];
 strcpy(db->s_name[i], pom);
 db->s_mod[i][MULTIPLE]   =  mod[0];
 db->s_mod[i][ATTRIBUTE]  =  mod[1];
 db->s_mod[i][MOD]        =  mod[2];
 db->s_mod[i][DIFF]       =  mod[3];
 db->s_mod[i][CATEG]      =  mod[4];
 db->s_mod[i][MAGIC]      =  mod[5];
}


void ASortSkillFunc(Wnd& wnd, void* ptr)
{
 tout("ASortSkill");
 for (int i=0;i<db->ilosc[3];i++)
    {
     for (int j=0;j<db->ilosc[3]-(i+1);j++)
        {
	 if (StrMore(db->s_name[j], db->s_name[j+1]))
	  {
	   SwapSkill(j+1, j);
	  }
	}
     }
}


void DelSpellFunc(Wnd& wnd, void* ptr)
{
 tout("DelSpellFunc");
 if (db->ilosc_poziomow == 0)
    {
     wnd.CreateMBoxChild("Nie ma nic do usuniecia\nbrak nawet ! poziomow !");
     return;
    }
 char ** tmp;
 char tmp2[120];
 tmp = new char*[db->ilosc_poziomow+1];
 if (!tmp) panic("memory exception in SetNamesFunc - tmp");
 for (int i=0;i<db->ilosc_poziomow;i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in SetNamesFunc - tmp[]");
     sprintf(tmp2, "poziom %d", i);
     strcpy(tmp[i], tmp2);
    }
 char* nam = wnd.CreateCheckBoxChild("Wybierz poziom z ktorego usuniesz zaklecie:", tmp, db->ilosc_poziomow);
 int poz=0;
 sscanf(nam,"poziom %d", &poz);
 if (db->ilosc_czarow[poz] == 0)
    {
     wnd.CreateMBoxChild("Na tym poziomie brakuje czarow");
     return;
    }
 for (int i=1;i<db->ilosc_poziomow;i++) if (tmp[i]) delete [] tmp[i];
 if (tmp) delete [] tmp;
 tmp = NULL;
 tmp = new char*[db->ilosc[1]+1];
 if (!tmp) panic("memory exception in DelRaceFunc - tmp");
 for (int i=0;i<db->ilosc_czarow[poz];i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in DelRaceFunc - tmp[]");
     strcpy(tmp[i], db->zaklecie[poz][i]);
    }
 nam = wnd.CreateCheckBoxVert("Wybierz zaklecie do usuniecia", tmp, db->ilosc_czarow[poz]);
 int temp = -1;
 for (int i=0;i<db->ilosc_czarow[poz];i++)
    {
     if (!strcmp(db->zaklecie[poz][i], nam)) { temp = i; break; }
    }
 if (temp == -1) return ;
 db->ilosc_czarow[poz]--;
 for (int i=temp;i<db->ilosc_czarow[poz];i++)
    {
     strcpy(db->zaklecie[poz][i],db->zaklecie[poz][i+1]);
     strcpy(db->req_magic[poz][i],db->req_magic[poz][i+1]);
     db->spell_cost[poz][i] = db->spell_cost[poz][i+1];
    }
}


void ShredSpells(Wnd& wnd, void* ptr)
{
 tout("ShredSpells");
 int ok = wnd.CreateYesNoChild("Na pewno usunac WSZYSTKO?");
 if (ok)
   {
    for (int i=0;i<db->ilosc_poziomow;i++) db->ilosc_czarow[i] = 0;
    db->ilosc_poziomow = 0;
   }
}


void DelAllSpellFunc(Wnd& wnd, void* ptr)
{
 tout("DelAllSpellFunc");
 if (db->ilosc_poziomow == 0)
    {
     wnd.CreateMBoxChild("Zdefiniuj najpierw poziomy");
     return;
    }
 char ** tmp;
 char tmp2[120];
 tmp = new char*[db->ilosc_poziomow+1];
 if (!tmp) panic("memory exception in SetNamesFunc - tmp");
 for (int i=0;i<db->ilosc_poziomow;i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in SetNamesFunc - tmp[]");
     sprintf(tmp2, "poziom %d", i);
     strcpy(tmp[i], tmp2);
    }
 char* nam = wnd.CreateCheckBoxChild("Wybierz poziom", tmp, db->ilosc_poziomow);
 int poz=0;
 sscanf(nam,"poziom %d", &poz);
 if (db->ilosc_czarow[poz] == 0)
    {
     wnd.CreateMBoxChild("Brak czarow na tym poziomie");
     return;
    }
 db->ilosc_czarow[poz] = 0;
}


void DelLastSpell(Wnd& wnd, void* ptr)
{
 tout("DelLastSpell");
 if (db->ilosc_poziomow == 0)
    {
     wnd.CreateMBoxChild("Zdefiniuj najpierw poziomy");
     return;
    }
 char ** tmp;
 char tmp2[120];
 tmp = new char*[db->ilosc_poziomow+1];
 if (!tmp) panic("memory exception in SetNamesFunc - tmp");
 for (int i=0;i<db->ilosc_poziomow;i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in SetNamesFunc - tmp[]");
     sprintf(tmp2, "poziom %d", i);
     strcpy(tmp[i], tmp2);
    }
 char* nam = wnd.CreateCheckBoxChild("Wybierz poziom", tmp, db->ilosc_poziomow);
 int poz=0;
 sscanf(nam,"poziom %d", &poz);
 if (db->ilosc_czarow[poz] == 0)
    {
     wnd.CreateMBoxChild("Brak czarow na tym poziomie");
     return;
    }
 db->ilosc_czarow[poz]--;
}


void AddSpellFunc(Wnd& wnd, void* ptr)
{
 tout("AddSpellFunc");
 again:
 if (db->ilosc_poziomow == 0)
    {
     wnd.CreateMBoxChild("Zdefiniuj najpierw poziomy");
     return;
    }
 char ** tmp;
 char tmp2[120];
 tmp = new char*[db->ilosc_poziomow+1];
 if (!tmp) panic("memory exception in SetNamesFunc - tmp");
 for (int i=0;i<db->ilosc_poziomow;i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in SetNamesFunc - tmp[]");
     sprintf(tmp2, "poziom %d", i);
     strcpy(tmp[i], tmp2);
    }
 char* nam = wnd.CreateCheckBoxChild("Wybierz poziom", tmp, db->ilosc_poziomow);
 int poz=0;
 sscanf(nam,"poziom %d", &poz);
 if (db->ilosc_czarow[poz] == MAX_SPELL)
    {
     wnd.CreateMBoxChild("Ilosc czarow dla tego poziomu to juz maksimum!");
     return;
    }
 int temp = db->ilosc_czarow[poz];
 strcpy(db->zaklecie[poz][temp], wnd.CreateWriteBox("Podaj nazwe zaklecia"));
 if (!strcmp(db->zaklecie[poz][temp],"")) {wnd.CreateMBoxChild("Nazwa nie moze byc pusta"); return;}
 char* str = wnd.CreateWriteBox("Podaj koszt zaklecia (PM):");
 int val=0;
 sscanf(str,"%d", &val);
 db->spell_cost[poz][temp] = val;
 ASortSkillFunc(wnd, NULL);
 for (int i=1;i<db->ilosc_poziomow;i++) if (tmp[i]) delete [] tmp[i];
 if (tmp) delete [] tmp;
 tmp = NULL;
 tmp = new char*[db->ilosc[3]+1];
 if (!tmp) panic("memory exception in DelRaceFunc - tmp");
 for (int i=0;i<db->ilosc[3];i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in DelRaceFunc - tmp[]");
     sprintf(tmp[i],"(%d)%s", i, db->s_name[i]);
    }
 nam = wnd.CreateCheckBoxVert("Wybierz umiejetnosc\npotrzebna do rzucenia zaklecia", tmp, db->ilosc[3]);
 val = 0;
 sscanf(nam,"(%d)%s", &val, tmp2);
 strcpy(db->req_magic[poz][temp],db->s_name[val]);
 char dat[600];
 sprintf(dat,"Czar: (%d:%d)%s\nPoziom: %d, koszt: %d\nWymagana um: (%d)%s\nOk?"
 , poz, db->ilosc_czarow[poz],db->zaklecie[poz][temp],poz,db->spell_cost[poz][temp], val,db->s_name[val]);
 int ok = wnd.CreateYesNoChild(dat);
 if (!ok) goto again;
 db->ilosc_czarow[poz]++;
}


void EdSpellFunc(Wnd& wnd, void* ptr)
{
 tout("EdSpellFunc");
 char* str = wnd.RetBtnName(1);
 int num=0;
 int poz=0;
 sscanf(str,"Edytuj %d:%d", &num, &poz);
 again:
 strcpy(db->zaklecie[poz][num], wnd.CreateWriteBox("Podaj nazwe zaklecia"));
 if (!strcmp(db->zaklecie[poz][num],"")) {wnd.CreateMBoxChild("Nazwa nie moze byc pusta"); return;}
 str = wnd.CreateWriteBox("Podaj koszt zaklecia (PM):");
 int val=0;
 sscanf(str,"%d", &val);
 db->spell_cost[poz][num] = val;
 ASortSkillFunc(wnd, NULL);
 char **tmp = new char*[db->ilosc[3]+1];
 if (!tmp) panic("memory exception in DelRaceFunc - tmp");
 for (int i=0;i<db->ilosc[3];i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in DelRaceFunc - tmp[]");
     sprintf(tmp[i],"(%d)%s", i, db->s_name[i]);
    }
 char* nam = wnd.CreateCheckBoxVert("Wybierz umiejetnosc\npotrzebna do rzucenia zaklecia", tmp, db->ilosc[3]);
 char tmp2[100];
 val=0;
 sscanf(nam,"(%d)%s", &val, tmp2);
 strcpy(db->req_magic[poz][num],db->s_name[val]);
 char dat[600];
 sprintf(dat,"Czar: (%d:%d)%s\nPoziom: %d, koszt: %d\nWymagana um: (%d)%s\nOk?"
 , poz, db->ilosc_czarow[poz],db->zaklecie[poz][num],poz,db->spell_cost[poz][num], val,db->s_name[val]);
 int ok = wnd.CreateYesNoChild(dat);
 if (!ok) goto again;
 wnd.AboutQuit(1);
}


void ShowSpellNum(Wnd& wnd, int num, int poz)
{
 tout("ShowSpellNum");
 char* tmp = new char[256];
 if (!tmp) panic("memory_exception_in_new",__LINE__,__FILE__);
 sprintf(tmp,"Czar: (%d)%s\nPoziom: %d\nKoszt PM: %d\nWymagana um.: %s",
 num, db->zaklecie[poz][num],poz, db->spell_cost[poz][num], db->req_magic[poz][num]);
 char str[50];
 sprintf(str,"Edytuj %d:%d", num, poz);
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 400, 450);
 if (!wybor) panic("memory exception in creating win_child!");
 wybor->WinDefaults("Wlasciwosci zaklecia");
 wybor->PrepareWndText(tmp);
 wybor->StdTextWrite();
 wybor->SetParent(&wnd);
 wybor->SetButtonsNumber(2);
 wybor->CreateButton(0, "OK", 60 , 410, StdOKFunc, true);
 wybor->CreateButton(1, str, 260 , 410, EdSpellFunc, false);
 wybor->DefaultResponse();
 wnd.Invalidate();
}


void ShowSpellFunc(Wnd& wnd, void* ptr)
{
 tout("ShowSpellFunc");
 if (db->ilosc_poziomow == 0)
    {
     wnd.CreateMBoxChild("Zdefiniuj najpierw poziomy");
     return;
    }
 char ** tmp;
 char tmp2[120];
 tmp = new char*[db->ilosc_poziomow+1];
 if (!tmp) panic("memory exception in SetNamesFunc - tmp");
 for (int i=0;i<db->ilosc_poziomow;i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in SetNamesFunc - tmp[]");
     sprintf(tmp2, "poziom %d", i);
     strcpy(tmp[i], tmp2);
    }
 char* nam = wnd.CreateCheckBoxChild("Wybierz poziom", tmp, db->ilosc_poziomow);
 int poz=0;
 sscanf(nam,"poziom %d", &poz);
 if (db->ilosc_czarow[poz] == 0)
    {
     wnd.CreateMBoxChild("Brak czarow na tym poziomie");
     return;
    }
 for (int i=1;i<db->ilosc_poziomow;i++) if (tmp[i]) delete [] tmp[i];
 if (tmp) delete [] tmp;
 tmp = NULL;
 tmp = new char*[db->ilosc_czarow[poz]+1];
 if (!tmp) panic("memory exception in DelRaceFunc - tmp");
 for (int i=0;i<db->ilosc_czarow[poz];i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in DelRaceFunc - tmp[]");
     sprintf(tmp[i],"(%d)%s", i, db->zaklecie[poz][i]);
    }
 nam = wnd.CreateCheckBoxVert("Wybierz zaklecie", tmp, db->ilosc_czarow[poz]);
 int val=0;
 sscanf(nam,"(%d)%s", &val, tmp2);
 ShowSpellNum(wnd, val, poz);
}


void AddProfFunc(Wnd& wnd, void* ptr)
{
 tout("AddprofFunc");
 if (db->ilosc[2] == MAX_TYPE)
    {
     wnd.CreateMBoxChild("Osiagnieto maksymalna ilosc profesji");
     return;
    }
 int temp = db->ilosc[2];
 strcpy(db->name[1][temp], wnd.CreateWriteBox("Podaj nazwe profesji"));
 if (!strcmp(db->name[1][temp],"")) {wnd.CreateMBoxChild("Nazwa nie moze byc pusta"); return;}
 EditProfField(wnd, NULL);
 db->ilosc[2]++;
}


void DelLastRace(Wnd& wnd, void* ptr)
{
 tout("DelLastRace");
 if (db->ilosc[1] == 0)
    {
     wnd.CreateMBoxChild("Nie ma nic do usuniecia");
     return;
    }
 db->ilosc[1]--;
}


void DelLastItem(Wnd& wnd, void* ptr)
{
 tout("DelLastItem");
 if (db->ilosc_ekw == 0)
    {
     wnd.CreateMBoxChild("Nie ma nic do usuniecia");
     return;
    }
 db->ilosc_ekw--;
}


void DelLastProf(Wnd& wnd, void* ptr)
{
 tout("DelLastProf");
 if (db->ilosc[2] == 0)
    {
     wnd.CreateMBoxChild("Nie ma nic do usuniecia");
     return;
    }
 db->ilosc[2]--;
}


void DelRaceFunc(Wnd& wnd, void* ptr)
{
 tout("DelRaceFunc");
 if (db->ilosc[1] == 0)
    {
     wnd.CreateMBoxChild("Nie ma nic do usuniecia");
     return;
    }
 char ** tmp;
 tmp = new char*[db->ilosc[1]+1];
 if (!tmp) panic("memory exception in DelRaceFunc - tmp");
 for (int i=0;i<db->ilosc[1];i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in DelRaceFunc - tmp[]");
     strcpy(tmp[i], db->name[0][i]);
    }
 char* nam = wnd.CreateCheckBoxVert("Wybierz rase do usuniecia", tmp, db->ilosc[1]);
 int temp = -1;
 for (int i=0;i<db->ilosc[1];i++)
    {
     if (!strcmp(db->name[0][i], nam)) { temp = i; break; }
    }
 if (temp == -1) return ;
 db->ilosc[1]--;
 for (int i=temp;i<db->ilosc[1];i++)
    {
     strcpy(db->name[0][i],db->name[0][i+1]);
     for (int j=0;j<db->ilosc[0];j++)
        {
         db->mod[0][i][j] = db->mod[0][i+1][j];
        }
    }
}


void DelItemFunc(Wnd& wnd, void* ptr)
{
 tout("DelItemFunc");
 if (db->ilosc_ekw == 0)
    {
     wnd.CreateMBoxChild("Nie ma nic do usuniecia");
     return;
    }
 char ** tmp;
 tmp = new char*[db->ilosc_ekw+1];
 if (!tmp) panic("memory exception in DelRaceFunc - tmp");
 for (int i=0;i<db->ilosc_ekw;i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in DelRaceFunc - tmp[]");
     strcpy(tmp[i], db->ekw[i]);
    }
 char* nam = wnd.CreateCheckBoxVert("Wybierz przedmiot do usuniecia", tmp, db->ilosc_ekw);
 int temp = -1;
 for (int i=0;i<db->ilosc_ekw;i++)
    {
     if (!strcmp(db->ekw[i], nam)) { temp = i; break; }
    }
 if (temp == -1) return ;
 db->ilosc_ekw--;
 for (int i=temp;i<db->ilosc[1];i++)
    {
     strcpy(db->ekw[i],db->ekw[i+1]);
     db->c_ekw[i] = db->c_ekw[i+1];
    }
}


void DelProfFunc(Wnd& wnd, void* ptr)
{
 tout("DelProfFunc");
 if (db->ilosc[2] == 0)
    {
     wnd.CreateMBoxChild("Nie ma nic do usuniecia");
     return;
    }
 char ** tmp;
 tmp = new char*[db->ilosc[2]+1];
 if (!tmp) panic("memory exception in DelRaceFunc - tmp");
 for (int i=0;i<db->ilosc[2];i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in DelRaceFunc - tmp[]");
     strcpy(tmp[i], db->name[1][i]);
    }
 char* nam = wnd.CreateCheckBoxVert("Wybierz profesje do usuniecia", tmp, db->ilosc[2]);
 int temp = -1;
 for (int i=0;i<db->ilosc[2];i++)
    {
     if (!strcmp(db->name[1][i], nam)) { temp = i; break; }
    }
 if (temp == -1) return ;
 db->ilosc[2]--;
 for (int i=temp;i<db->ilosc[2];i++)
    {
     strcpy(db->name[1][i],db->name[1][i+1]);
     for (int j=0;j<db->ilosc[0];j++)
        {
         db->mod[1][i][j] = db->mod[1][i+1][j];
        }
    }
}


void EdNameRaceFunc(Wnd& wnd, void* ptr)
{
 tout("EdNameRaceFunc");
 char* str = wnd.RetBtnName(0);
 int num=0;
 sscanf(str,"Edytuj nazwe %d", &num);
 char tmp[100];
 sprintf(tmp, "Podaj nazwe dla %d\ndawna nazwa %s", num, db->name[0][num]);
 char* nam = wnd.CreateWriteBox(tmp);
 if (!nam) return;
 strcpy(db->name[0][num], nam);
}


void EdItemNumFunc(Wnd& wnd, void* ptr)
{
 tout("EdItemNumFunc");
 char* str = wnd.RetBtnName(1);
 int num=0;
 sscanf(str,"Edytuj %d", &num);
 char tmp[100];
 sprintf(tmp, "Podaj nazwe dla %d\ndawna nazwa %s", num, db->ekw[num]);
 char* nam = wnd.CreateWriteBox(tmp);
 if (!nam) return;
 strcpy(db->ekw[num], nam);
 nam = wnd.CreateWriteBox("Podaj koszt $");
 int val=0;
 sscanf(nam,"%d", &val);
 db->c_ekw[num] = val;
 wnd.AboutQuit(1);
}


void EdModRaceFunc(Wnd& wnd, void* ptr)
{
 tout("EdModRaceFunc");
 char* str = wnd.RetBtnName(1);
 int num=0;
 sscanf(str,"Edytuj mod. %d", &num);
 char t[100];
 sprintf(t, "Modyfikatory dla %d\ndawna nazwa %s", num, db->name[0][num]);
 if (db->ilosc[0] == 0)
    {
     wnd.CreateMBoxChild("Najpierw zdefiniuj ilosc i nazwy cech");
     return;
    }
 char ** tmp;
 char tmp2[120];                  //it were too short in the past
 tmp = new char*[db->ilosc[0]+1]; //and there was FUCKIN' SIGSEGV
 if (!tmp) panic("memory exception in SetNamesFunc - tmp");
 for (int i=0;i<db->ilosc[0];i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in SetNamesFunc - tmp[]");
     sprintf(tmp2, "(%d)%s", i, db->name_att[i]);
     strcpy(tmp[i], tmp2);
    }
 char* nam = wnd.CreateCheckBoxChild(t, tmp, db->ilosc[0]);
 int val=0;
 sscanf(nam,"(%d)", &val);
 sprintf(tmp2,"Podaj modyfikator (%d) %s\n dla rasy (%d)%s:", val, db->name_att[val], num, db->name[0][num]);
 nam = wnd.CreateWriteBox(tmp2);
 int mmod=0;
 sscanf(nam,"%d", &mmod);
 db->mod[0][num][val] = mmod;
}


void EdNameProfFunc(Wnd& wnd, void* ptr)
{
 tout("EdNameProfFunc");
 char* str = wnd.RetBtnName(0);
 int num=0;
 sscanf(str,"Edytuj nazwe %d", &num);
 char tmp[100];
 sprintf(tmp, "Podaj nazwe dla %d\ndawna nazwa %s", num, db->name[1][num]);
 char* nam = wnd.CreateWriteBox(tmp);
 if (!nam) return;
 strcpy(db->name[1][num], nam);
}


void EdModProfFunc(Wnd& wnd, void* ptr)
{
 tout("EdModProfFunc");
 char* str = wnd.RetBtnName(1);
 int num=0;
 sscanf(str,"Edytuj mod. %d", &num);
 char t[100];
 sprintf(t, "Modyfikatory dla %d\ndawna nazwa %s", num, db->name[1][num]);
 if (db->ilosc[0] == 0)
    {
     wnd.CreateMBoxChild("Najpierw zdefiniuj ilosc i nazwy cech");
     return;
    }
 char ** tmp;
 char tmp2[120];                  //it were too short in the past
 tmp = new char*[db->ilosc[0]+1]; //and there was FUCKIN' SIGSEGV
 if (!tmp) panic("memory exception in SetNamesFunc - tmp");
 for (int i=0;i<db->ilosc[0];i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in SetNamesFunc - tmp[]");
     sprintf(tmp2, "(%d)%s", i,db->name_att[i]);
     strcpy(tmp[i], tmp2);
    }
 char* nam = wnd.CreateCheckBoxChild(t, tmp, db->ilosc[0]);
 int val=0;
 sscanf(nam,"(%d)", &val);
 sprintf(tmp2,"Podaj modyfikator (%d) %s\n dla prof. (%d)%s:", val, db->name_att[val], num, db->name[1][num]);
 nam = wnd.CreateWriteBox(tmp2);
 int mmod=0;
 sscanf(nam,"%d", &mmod);
 db->mod[1][num][val] = mmod;
}


void EdRaceFunc(Wnd& wnd, void* ptr)
{
 tout("EdRaceFunc");
 char* str = wnd.RetBtnName(1);
 int num=0;
 sscanf(str,"Edytuj %d", &num);
 char edit_name[50];
 char edit_mod[50];
 char tmp[100];
 sprintf(edit_name,"Edytuj nazwe %d", num);
 sprintf(edit_mod,"Edytuj mod. %d", num);
 sprintf(tmp,"Edytuj rase: (%-3d)%s", num, db->name[0][num]);
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 400, 250);
 if (!wybor) panic("memory exception in creating win_child!");
 wybor->WinDefaults(tmp);
 wybor->PrepareWndText("Wybierz opcje");
 wybor->StdTextWrite();
 wybor->SetParent(&wnd);
 wybor->SetButtonsNumber(3);
 wybor->CreateButton(0, edit_name, 20 , 180, EdNameRaceFunc, true, 160);
 wybor->CreateButton(1, edit_mod, 220 , 180, EdModRaceFunc, false, 160);
 wybor->CreateButton(2, "Powrot", 160 , 210, StdOKFunc, false);
 wybor->DefaultResponse();
 wnd.Invalidate();
 wnd.AboutQuit(1);
}


void EdProfFunc(Wnd& wnd, void* ptr)
{
 tout("EdProfFunc");
 char* str = wnd.RetBtnName(1);
 int num=0;
 sscanf(str,"Edytuj %d", &num);
 char edit_name[50];
 char edit_mod[50];
 char tmp[100];
 sprintf(edit_name,"Edytuj nazwe %d", num);
 sprintf(edit_mod,"Edytuj mod. %d", num);
 sprintf(tmp,"Edytuj prof: (%-3d)%s", num, db->name[1][num]);
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 400, 250);
 if (!wybor) panic("memory exception in creating win_child!");
 wybor->WinDefaults(tmp);
 wybor->PrepareWndText("Wybierz opcje");
 wybor->StdTextWrite();
 wybor->SetParent(&wnd);
 wybor->SetButtonsNumber(3);
 wybor->CreateButton(0, edit_name, 20 , 180, EdNameProfFunc, true, 160);
 wybor->CreateButton(1, edit_mod, 220 , 180, EdModProfFunc, false, 160);
 wybor->CreateButton(2, "Powrot", 160 , 210, StdOKFunc, false);
 wybor->DefaultResponse();
 wnd.Invalidate();
 wnd.AboutQuit(1);
}


void ShowRaceNum(Wnd& wnd, int num)
{
 tout("ShowRaceNum");
 char* tmp = new char[2048];
 char tmp2[100];
 if (!tmp) panic("memory exception in ShowRaceFunc - tmp");
 sprintf(tmp,"Wlasciwosci rasy: %s\n", db->name[0][num]);
 for (int i=0;i<db->ilosc[0];i++)
        {
         sprintf(tmp2, "%-25s %d\n",db->name_att[i], db->mod[0][num][i]);
	 strcat(tmp, tmp2);
        }
 char str[50];
 sprintf(str,"Edytuj %d", num);
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 400, 450);
 if (!wybor) panic("memory exception in creating win_child!");
 wybor->WinDefaults("Wlasciwosci rasy");
 wybor->PrepareWndText(tmp);
 wybor->StdTextWrite();
 wybor->SetParent(&wnd);
 wybor->SetButtonsNumber(2);
 wybor->CreateButton(0, "OK", 60 , 410, StdOKFunc, true);
 wybor->CreateButton(1, str, 260 , 410, EdRaceFunc, false);
 wybor->DefaultResponse();
 wnd.Invalidate();
}


void ShowItemNum(Wnd& wnd, int num)
{
 tout("ShowItemNum");
 char str[50];
 char txt[100];
 sprintf(str,"Edytuj %d", num);
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 400, 450);
 if (!wybor) panic("memory exception in creating win_child!");
 wybor->WinDefaults("Wlasciwosci przedmiotu");
 sprintf(txt,"Przedmiot numer : %-3d %-25s, koszt: %-4d", num, db->ekw[num], db->c_ekw[num]);
 wybor->PrepareWndText(txt);
 wybor->StdTextWrite();
 wybor->SetParent(&wnd);
 wybor->SetButtonsNumber(2);
 wybor->CreateButton(0, "OK", 60 , 410, StdOKFunc, true);
 wybor->CreateButton(1, str, 260 , 410, EdItemNumFunc, false);
 wybor->DefaultResponse();
 wnd.Invalidate();
}


void ShowProfNum(Wnd& wnd, int num)
{
 tout("ShowProfNum");
 char* tmp = new char[2048];
 char tmp2[100];
 if (!tmp) panic("memory exception in ShowRaceFunc - tmp");
 sprintf(tmp,"Wlasciwosci prof: %s\n", db->name[1][num]);
 for (int i=0;i<db->ilosc[0];i++)
        {
         sprintf(tmp2, "%-25s %d\n",db->name_att[i], db->mod[1][num][i]);
	 strcat(tmp, tmp2);
        }
 char str[50];
 sprintf(str,"Edytuj %d", num);
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 400, 450);
 if (!wybor) panic("memory exception in creating win_child!");
 wybor->WinDefaults("Wlasciwosci profesji");
 wybor->PrepareWndText(tmp);
 wybor->StdTextWrite();
 wybor->SetParent(&wnd);
 wybor->SetButtonsNumber(2);
 wybor->CreateButton(0, "OK", 60 , 410, StdOKFunc, true);
 wybor->CreateButton(1, str, 260 , 410, EdProfFunc, false);
 wybor->DefaultResponse();
 wnd.Invalidate();
}


void ShowRaceFunc(Wnd& wnd, void* ptr)
{
 tout("ShowRaceFunc");
 if (db->ilosc[1] == 0)
    {
     wnd.CreateMBoxChild("Brak ras w bazie danych");
     return;
    }
 char ** tmp;
 tmp = new char*[db->ilosc[1]+1];
 if (!tmp) panic("memory exception in ShowRaceFunc - tmp");
 for (int i=0;i<db->ilosc[1];i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in DelRaceFunc - tmp[]");
     strcpy(tmp[i], db->name[0][i]);
    }
 char str[100];
 sprintf(str,"ilosc ras: %d, wybierz rase:", db->ilosc[1]);
 char* nam = wnd.CreateCheckBoxVert(str, tmp, db->ilosc[1]);
 int temp = -1;
 for (int i=0;i<db->ilosc[1];i++)
    {
     if (!strcmp(db->name[0][i], nam)) { temp = i; break; }
    }
 if (temp == -1) return ;
 ShowRaceNum(wnd, temp);
}


void ShowItemFunc(Wnd& wnd, void* ptr)
{
 tout("ShowItemFunc");
 if (db->ilosc_ekw == 0)
    {
     wnd.CreateMBoxChild("Brak przedmiotow w bazie danych");
     return;
    }
 char ** tmp;
 tmp = new char*[db->ilosc_ekw+1];
 if (!tmp) panic("memory exception in ShowRaceFunc - tmp");
 for (int i=0;i<db->ilosc_ekw;i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in DelRaceFunc - tmp[]");
     strcpy(tmp[i], db->ekw[i]);
    }
 char str[100];
 sprintf(str,"Ilosc przedmiotow: %d\nDomyslna ilosc kasy: %-4d\n wybierz przedmiot:", db->ilosc_ekw, db->def_cash);
 char* nam = wnd.CreateCheckBoxVert(str, tmp, db->ilosc_ekw);
 int temp = -1;
 for (int i=0;i<db->ilosc_ekw;i++)
    {
     if (!strcmp(db->ekw[i], nam)) { temp = i; break; }
    }
 if (temp == -1) return ;
 ShowItemNum(wnd, temp);
}


void ShowProfFunc(Wnd& wnd, void* ptr)
{
 tout("ShowProfFunc");
 if (db->ilosc[2] == 0)
    {
     wnd.CreateMBoxChild("Brak profesji w bazie danych");
     return;
    }
 char ** tmp;
 tmp = new char*[db->ilosc[2]+1];
 if (!tmp) panic("memory exception in ShowRaceFunc - tmp");
 for (int i=0;i<db->ilosc[2];i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in DelRaceFunc - tmp[]");
     strcpy(tmp[i], db->name[1][i]);
    }
 char str[100];
 sprintf(str,"ilosc prof: %d, wybierz prof:", db->ilosc[2]);
 char* nam = wnd.CreateCheckBoxVert(str, tmp, db->ilosc[2]);
 int temp = -1;
 for (int i=0;i<db->ilosc[2];i++)
    {
     if (!strcmp(db->name[1][i], nam)) { temp = i; break; }
    }
 if (temp == -1) return ;
 ShowProfNum(wnd, temp);
}


void DelAllRaceFunc(Wnd& wnd, void* ptr)
{
 tout("DelAllRaceFunc");
 if (db->ilosc[1] == 0)
    {
     wnd.CreateMBoxChild("Brak ras w bazie danych");
     return;
    }
 else db->ilosc[1] = 0;
}


void DelAllItemFunc(Wnd& wnd, void* ptr)
{
 tout("DelAllItemFunc");
 if (db->ilosc_ekw == 0)
    {
     wnd.CreateMBoxChild("Brak ekwpiunku w bazie danych");
     return;
    }
 else db->ilosc_ekw = 0;
}


void DelAllProfFunc(Wnd& wnd, void* ptr)
{
 tout("DelAllProfFunc");
 if (db->ilosc[2] == 0)
    {
     wnd.CreateMBoxChild("Brak profesji w bazie danych");
     return;
    }
 else db->ilosc[2] = 0;
}


void SwapTrial(int trial, int i, int j)
{
 tout("SwapTrial");
 char pom[STRING];
 int mod;
 strcpy(pom, db->name[trial][j]);
 strcpy(db->name[trial][j], db->name[trial][i]);
 strcpy(db->name[trial][i], pom);
 for (int x=0;x<db->ilosc[0];x++)
        {
         mod = db->mod[trial][j][x];
         db->mod[trial][j][x] = db->mod[trial][i][x];
         db->mod[trial][i][x] = mod;
        }
}


void ASortProfFunc(Wnd& wnd, void* ptr)
{
 tout("ASortProfFunc");
 for (int i=0;i<db->ilosc[2];i++)
    {
     for (int j=0;j<db->ilosc[2]-(i+1);j++)
        {
	 if (StrMore(db->name[1][j], db->name[1][j+1]))
	  {
	   SwapTrial(1, j+1, j);
	  }
	}
     }
}


void ASortRaceFunc(Wnd& wnd, void* ptr)
{
 tout("ASortRaceFunc");
 for (int i=0;i<db->ilosc[1];i++)
    {
     for (int j=0;j<db->ilosc[1]-(i+1);j++)
        {
	 if (StrMore(db->name[0][j], db->name[0][j+1]))
	  {
	   SwapTrial(0, j+1, j);
	  }
	}
     }
}


void SwapItem(int i, int j)
{
 tout("SwapItem");
 char pom[STRING];
 int mod;
 strcpy(pom, db->ekw[j]);
 strcpy(db->ekw[j], db->ekw[i]);
 strcpy(db->ekw[i], pom);
 mod = db->c_ekw[j];
 db->c_ekw[j] = db->c_ekw[i];
 db->c_ekw[i] = mod;
}


void ASortItemFunc(Wnd& wnd, void* ptr)
{
 tout("ASortItemFunc");
 for (int i=0;i<db->ilosc_ekw;i++)
    {
     for (int j=0;j<db->ilosc_ekw-(i+1);j++)
        {
	 if (StrMore(db->ekw[j], db->ekw[j+1]))
	  {
	   SwapItem(j+1, j);
	  }
	}
     }
}


void EditRacesFunc(Wnd& wnd, void* ptr)
{
 tout("EditRacesFunc");
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 400, 300);
 if (!wybor) panic("memory exception in creating win_child!");
 wybor->WinDefaults("Wybierz opcje");
 wybor->SetButtonsNumber(8);
 wybor->CreateButton(0, "Dodaj",30 , 100, AddRaceFunc, true, 140);  //ok
 wybor->CreateButton(1, "Usun", 230 , 100, DelRaceFunc, false, 140);//ok
 wybor->CreateButton(2, "Pokaz/edytuj", 30 , 130, ShowRaceFunc, false, 140); //ok
 wybor->CreateButton(3, "Usun ostatnia", 230 , 130, DelLastRace, false, 140); //ok
 wybor->CreateButton(4, "Usun wszystkie!",30 , 160, DelAllRaceFunc, false, 140); //ok
 wybor->CreateButton(5, "Zapisz", 230 , 160, FSaveBase, false, 140); //ok
 wybor->CreateButton(6, "Powrot", 30 , 190, StdOKFunc, false, 140);  //ok
 wybor->CreateButton(7, "Sortuj", 230 , 190, ASortRaceFunc, false, 140);//ok
 wybor->SetParent(&wnd);
 wybor->DefaultResponse();
 wnd.Invalidate();
}


void SetOthItemFunc(Wnd& wnd, void* ptr)
{
 tout("SetOthItemFunc");
 char* str = wnd.CreateWriteBox("Podaj domyslna ilosc pieniedzy:");
 int val=0;
 sscanf(str,"%d", &val);
 db->def_cash = val;
}


void EdItemsFunc(Wnd& wnd, void* ptr)
{
 tout("EdItemsFunc");
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 400, 300);
 if (!wybor) panic("memory exception in creating win_child!");
 wybor->WinDefaults("Wybierz opcje");
 wybor->SetButtonsNumber(9);
 wybor->CreateButton(0, "Dodaj",30 , 100, AddItemFunc, true, 140);//ok
 wybor->CreateButton(1, "Usun", 230 , 100, DelItemFunc, false, 140);//ok
 wybor->CreateButton(2, "Pokaz/edytuj", 30 , 130, ShowItemFunc, false, 140);//ok
 wybor->CreateButton(3, "Usun ostatni", 230 , 130, DelLastItem, false, 140);//ok
 wybor->CreateButton(4, "Usun wszystkie!",30 , 160, DelAllItemFunc, false, 140);//ok
 wybor->CreateButton(5, "Zapisz", 230 , 160, FSaveBase, false, 140);//ok
 wybor->CreateButton(6, "Inne", 30 , 190, SetOthItemFunc, false, 140);//ok
 wybor->CreateButton(7, "Sortuj", 230 , 190, ASortItemFunc, false, 140);//ok
 wybor->CreateButton(8, "Powrot", 130 , 220, StdOKFunc, false, 140);//ok
 wybor->SetParent(&wnd);
 wybor->DefaultResponse();
 wnd.Invalidate();
}


void EditProfsFunc(Wnd& wnd, void* ptr)
{
 tout("EditProfsFunc");
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 400, 300);
 if (!wybor) panic("memory exception in creating win_child!");
 wybor->WinDefaults("Wybierz opcje");
 wybor->SetButtonsNumber(8);
 wybor->CreateButton(0, "Dodaj",30 , 100, AddProfFunc, true, 140);//ok
 wybor->CreateButton(1, "Usun", 230 , 100, DelProfFunc, false, 140);//ok
 wybor->CreateButton(2, "Pokaz/edytuj", 30 , 130, ShowProfFunc, false, 140);//ok
 wybor->CreateButton(3, "Usun ostatnia", 230 , 130, DelLastProf, false, 140);
 wybor->CreateButton(4, "Usun wszystkie!",30 , 160, DelAllProfFunc, false, 140);
 wybor->CreateButton(5, "Zapisz", 230 , 160, FSaveBase, false, 140); //ok
 wybor->CreateButton(6, "Powrot", 30 , 190, StdOKFunc, false, 140);  //ok
 wybor->CreateButton(7, "Sortuj", 230 , 190, ASortProfFunc, false, 140);//ok
 wybor->SetParent(&wnd);
 wybor->DefaultResponse();
 wnd.Invalidate();
}


void SetNumAttrFunc(Wnd& wnd, void* ptr)
{
 tout("SetNumAttrFunc");
 char* text = wnd.CreateWriteBox("Podaj ilosc cech:");
 int val=0;
 sscanf(text,"%d", &val);
 if ((val < 1) || (val > MAX_ATT))
   {
    wnd.CreateMBoxChild("Zbyt duza lub zbyt mala ilosc cech");
    return;
   }
 else db->ilosc[0] = val;
}


void SetNumPDFunc(Wnd& wnd, void* ptr)
{
 tout("SetNumPDFunc");
 char* text = wnd.CreateWriteBox("Podaj ilosc poziomow PD:");
 int val=0;
 sscanf(text,"%d", &val);
 if ((val < 1) || (val > MAX_LEVELS))
   {
    wnd.CreateMBoxChild("Zbyt duza lub zbyt mala ilosc poziomow PD");
    return;
   }
 else db->ilosc[6] = val;
}


void SetNumPUFunc(Wnd& wnd, void* ptr)
{
 tout("SetNumPUFunc");
 char* text = wnd.CreateWriteBox("Podaj ilosc poziomow PU:");
 int val=0;
 sscanf(text,"%d", &val);
 if ((val < 1) || (val > MAX_LEVELS))
   {
    wnd.CreateMBoxChild("Zbyt duza lub zbyt mala ilosc poziomow PU");
    return;
   }
 else db->ilosc[5] = val;
}


void SetNumCategFunc(Wnd& wnd, void* ptr)
{
 tout("SetNumCategFunc");
 char* text = wnd.CreateWriteBox("Podaj ilosc kategorii:");
 int val=0;
 sscanf(text,"%d", &val);
 if ((val < 1) || (val > MAX_CATEG))
   {
    wnd.CreateMBoxChild("Zbyt duza lub zbyt mala ilosc cech");
    return;
   }
 else db->ilosc[4] = val;
}


void OtherAttrFunc(Wnd& wnd, void* ptr)
{
 tout("OtherAttrFunc");
 if (db->ilosc[0] == 0)
    {
     wnd.CreateMBoxChild("Najpierw zdefiniuj ilosc i nazwy cech");
     return;
    }
 char ** tmp;
 tmp = new char*[db->ilosc[0]+1];
 if (!tmp) panic("memory exception in ShowRaceFunc - tmp");
 for (int i=0;i<db->ilosc[0];i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in DelRaceFunc - tmp[]");
     strcpy(tmp[i], db->name_att[i]);
    }
 char str[100];
 sprintf(str,"Ilosc cech %d, wybierz ceche\nodpowiedzialna za moc magiczna", db->ilosc[0]);
 char* nam = wnd.CreateCheckBoxChild(str, tmp, db->ilosc[0]);
 int temp;
 for (int i=0;i<db->ilosc[0];i++)
    {
     if (!strcmp(db->name_att[i], nam)) { temp = i; break; }
    }
 db->mm = temp;
 temp = 0;
 nam = wnd.CreateWriteBox("Podaj wartosc plusa:");
 sscanf(nam,"%d", &temp);
 db->st = temp;
}


void ShowAttrFunc(Wnd& wnd, void* ptr)
{
 tout("ShowAttrFunc");
 char* tmp = new char[2048];
 if (!tmp) panic("memory_exception_in_new",__LINE__,__FILE__);
 char tmp2[100];
 sprintf(tmp,"Zdefiniowanych cech: %d\n\n", db->ilosc[0]);
 for (int i=0;i<db->ilosc[0];i++)
   {
    sprintf(tmp2,"(%-3d)   %-25s start: %d\n", i, db->name_att[i], db->value_att[i]);
    strcat(tmp, tmp2);
   }
 sprintf(tmp2,"moc magiczna: %s\n", db->name_att[db->mm]);
 strcat(tmp,tmp2);
 sprintf(tmp2,"wartosc plusa: %d\n", db->st);
 strcat(tmp,tmp2);
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 300, 450);
 if (!wybor) panic("memory exception in creating win_child!");
 wybor->WinDefaults("Zdefioniowane cechy");
 wybor->PrepareWndText(tmp);
 wybor->StdTextWrite();
 wybor->SetParent(&wnd);
 wybor->SetButtonsNumber(1);
 wybor->CreateButton(0, "OK", 160 , 410, StdOKFunc, true);
 wybor->DefaultResponse();
 wnd.Invalidate();
}


void ShowLevPDFunc(Wnd& wnd, void* ptr)
{
 tout("ShowLevPDFunc");
 char* tmp = new char[1024];
 if (!tmp) panic("memory_exception_in_new",__LINE__,__FILE__);
 char tmp2[100];
 sprintf(tmp,"Zdefiniowanych poziomow PD: %d\n\n", db->ilosc[6]);
 for (int i=0;i<db->ilosc[6];i++)
   {
    sprintf(tmp2,"poziom: %-3d,   koszt PD: %-5d\n", i, db->pd_cost[i]);
    strcat(tmp, tmp2);
   }
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 300, 450);
 if (!wybor) panic("memory exception in creating win_child!");
 wybor->WinDefaults("Zdefiniowane poziomy PD");
 wybor->PrepareWndText(tmp);
 wybor->StdTextWrite();
 wybor->SetParent(&wnd);
 wybor->SetButtonsNumber(1);
 wybor->CreateButton(0, "OK", 160 , 410, StdOKFunc, true);
 wybor->DefaultResponse();
 wnd.Invalidate();
}


void ShowLevPUFunc(Wnd& wnd, void* ptr)
{
 tout("ShowLevPUFunc");
 char* tmp = new char[1024];
 if (!tmp) panic("memory_exception_in_new",__LINE__,__FILE__);
 char tmp2[100];
 sprintf(tmp,"Zdefiniowanych poziomow PU: %d\n\n", db->ilosc[5]);
 for (int i=0;i<db->ilosc[5];i++)
   {
    sprintf(tmp2,"poziom: %-3d,   koszt PU: %-5d\n", i, db->cost[i]);
    strcat(tmp, tmp2);
   }
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 300, 450);
 if (!wybor) panic("memory exception in creating win_child!");
 wybor->WinDefaults("Zdefiniowane poziomy PU");
 wybor->PrepareWndText(tmp);
 wybor->StdTextWrite();
 wybor->SetParent(&wnd);
 wybor->SetButtonsNumber(1);
 wybor->CreateButton(0, "OK", 160 , 410, StdOKFunc, true);
 wybor->DefaultResponse();
 wnd.Invalidate();
}


void ShowCategFunc(Wnd& wnd, void* ptr)
{
 tout("ShowCategFunc");
 char* tmp = new char[1024];
 if (!tmp) panic("memory_exception_in_new",__LINE__,__FILE__);
 char tmp2[100];
 sprintf(tmp,"Zdefiniowanych kateg: %d\n\n", db->ilosc[4]);
 for (int i=0;i<db->ilosc[4];i++)
   {
    sprintf(tmp2,"(%-3d)   %-25s\n", i, db->c_name[i]);
    strcat(tmp, tmp2);
   }
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 300, 450);
 if (!wybor) panic("memory exception in creating win_child!");
 wybor->WinDefaults("Zdefioniowane kategorie");
 wybor->PrepareWndText(tmp);
 wybor->StdTextWrite();
 wybor->SetParent(&wnd);
 wybor->SetButtonsNumber(1);
 wybor->CreateButton(0, "OK", 160 , 410, StdOKFunc, true);
 wybor->DefaultResponse();
 wnd.Invalidate();
}


void SetNamesFunc(Wnd& wnd, void* ptr)
{
 tout("SetNamesFunc");
 if (db->ilosc[0] == 0)
    {
     wnd.CreateMBoxChild("Najpierw zdefiniuj ilosc i nazwy cech");
     return;
    }
 char ** tmp;
 char tmp2[120];                  //it were too short in the past
 tmp = new char*[db->ilosc[0]+1]; //and there was FUCKIN' SIGSEGV
 if (!tmp) panic("memory exception in SetNamesFunc - tmp");
 for (int i=0;i<db->ilosc[0];i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in SetNamesFunc - tmp[]");
     sprintf(tmp2, "(%d)%s", i, db->name_att[i]);
     strcpy(tmp[i], tmp2);
    }
 char str[150];
 sprintf(str,"Ilosc cech %d\nwybierz ktora chcesz nazwac", db->ilosc[0]);
 char* nam = wnd.CreateCheckBoxChild(str, tmp, db->ilosc[0]);
 int val=0;
 sscanf(nam,"(%d)", &val);
 sprintf(tmp2,"Cecha numer %d\nDawna nazwa %s\nPodaj nowa:", val, db->name_att[val]);
 nam = wnd.CreateWriteBox(tmp2);
 if (!nam) return;
 strcpy(db->name_att[val], nam);
 nam = wnd.CreateWriteBox("Podaj wartosc startowa cechy:");
 int tnum=0;
 sscanf(nam,"%d", &tnum);
 db->value_att[val] = tnum;
}


void SetNamesPDFunc(Wnd& wnd, void* ptr)
{
 tout("SetNamesPDFunc");
 if (db->ilosc[6] == 0)
    {
     wnd.CreateMBoxChild("Najpierw zdefiniuj ilosc poziomow PD");
     return;
    }
 char ** tmp;
 char tmp2[120];                  //it were too short in the past
 tmp = new char*[db->ilosc[6]+1]; //and there was FUCKIN' SIGSEGV
 if (!tmp) panic("memory exception in SetNamesFunc - tmp");
 for (int i=0;i<db->ilosc[6];i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in SetNamesFunc - tmp[]");
     sprintf(tmp2, "%d", i);
     strcpy(tmp[i], tmp2);
    }
 char str[150];
 sprintf(str,"Ilosc poziomow %d\nwybierz ktorego koszt ustalic", db->ilosc[6]);
 char* nam = wnd.CreateCheckBoxChild(str, tmp, db->ilosc[6]);
 int val=0;
 sscanf(nam,"%d", &val);
 sprintf(tmp2,"Poziom %d\nDawny koszt PD %d\nPodaj nowa:", val, db->pd_cost[val]);
 nam = wnd.CreateWriteBox(tmp2);
 if (!nam) return;
 int tnum=0;
 sscanf(nam,"%d", &tnum);
 db->pd_cost[val] = tnum;
}


void SetNamesPUFunc(Wnd& wnd, void* ptr)
{
 tout("SetNamesPUFunc");
 if (db->ilosc[5] == 0)
    {
     wnd.CreateMBoxChild("Najpierw zdefiniuj ilosc poziomow PU");
     return;
    }
 char ** tmp;
 char tmp2[120];                  //it were too short in the past
 tmp = new char*[db->ilosc[5]+1]; //and there was FUCKIN' SIGSEGV
 if (!tmp) panic("memory exception in SetNamesFunc - tmp");
 for (int i=0;i<db->ilosc[5];i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in SetNamesFunc - tmp[]");
     sprintf(tmp2, "%d", i);
     strcpy(tmp[i], tmp2);
    }
 char str[150];
 sprintf(str,"Ilosc poziomow %d\nwybierz ktorego koszt ustalic", db->ilosc[5]);
 char* nam = wnd.CreateCheckBoxChild(str, tmp, db->ilosc[5]);
 int val=0;
 sscanf(nam,"%d", &val);
 sprintf(tmp2,"Poziom %d\nDawny koszt PD %d\nPodaj nowa:", val, db->cost[val]);
 nam = wnd.CreateWriteBox(tmp2);
 if (!nam) return;
 int tnum=0;
 sscanf(nam,"%d", &tnum);
 db->cost[val] = tnum;
}


void SetNamesCategFunc(Wnd& wnd, void* ptr)
{
 tout("SetNamesCategFunc");
 if (db->ilosc[4] == 0)
    {
     wnd.CreateMBoxChild("Najpierw zdefiniuj dodatnia ilosc kategorii");
     return;
    }
 char ** tmp;
 char tmp2[120];                  //it were too short in the past
 tmp = new char*[db->ilosc[4]+1]; //and there was FUCKIN' SIGSEGV
 if (!tmp) panic("memory exception in SetNamesCategFunc - tmp");
 for (int i=0;i<db->ilosc[4];i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in SetNamesCategFunc - tmp[]");
     sprintf(tmp2, "(%d)%s", i, db->c_name[i]);
     strcpy(tmp[i], tmp2);
    }
 char str[150];
 sprintf(str,"Ilosc kategorii %d\nwybierz ktora chcesz nazwac", db->ilosc[4]);
 char* nam = wnd.CreateCheckBoxChild(str, tmp, db->ilosc[4]);
 int val=0;
 sscanf(nam,"(%d)", &val);
 sprintf(tmp2,"Kateg numer %d\nDawna nazwa %s\nPodaj nowa:", val, db->c_name[val]);
 nam = wnd.CreateWriteBox(tmp2);
 if (!nam) return;
 strcpy(db->c_name[val], nam);
}


void EditAttrFunc(Wnd& wnd, void* ptr)
{
 tout("EditAttrFunc");
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 300, 200);
 if (!wybor) panic("memory exception in creating win_child!");
 wybor->WinDefaults("Wybierz opcje");
 wybor->SetButtonsNumber(5);
 wybor->CreateButton(0, "Ilosc cech",25 , 65, SetNumAttrFunc, true, 100); //ok
 wybor->CreateButton(1, "Nazwy cech",175 , 65, SetNamesFunc, false, 100);//ok
 wybor->CreateButton(2, "Inne",25 , 105, OtherAttrFunc, false, 100);//ok
 wybor->CreateButton(3, "Powrot",175 , 105, StdOKFunc, false, 100);//ok
 wybor->CreateButton(4, "Pokaz Cechy",100 , 155, ShowAttrFunc, false, 100);//ok
 wybor->SetParent(&wnd);
 wybor->DefaultResponse();
 wnd.Invalidate();
}


void EditLevelsFunc(Wnd& wnd, void* ptr)
{
 tout("EditLevelsFunc");
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 300, 200);
 if (!wybor) panic("memory exception in creating win_child!");
 wybor->WinDefaults("Wybierz opcje");
 wybor->SetButtonsNumber(7);
 wybor->CreateButton(0, "Ilosc poz PD",25 , 65, SetNumPDFunc, true, 100);//ok
 wybor->CreateButton(1, "Ilosc poz PU",175 , 65, SetNumPUFunc, true, 100);//ok
 wybor->CreateButton(2, "Koszt poz PD",25 , 95, SetNamesPDFunc, false, 100);//ok
 wybor->CreateButton(3, "Koszt poz PU",175 , 95, SetNamesPUFunc, false, 100);//ok
 wybor->CreateButton(4, "Pokaz poz PD",25 , 125, ShowLevPDFunc, false, 100);//ok
 wybor->CreateButton(5, "Pokaz poz PU",175 , 125, ShowLevPUFunc, false, 100);//ok
 wybor->CreateButton(6, "Powrot",100 , 155, StdOKFunc, false, 100);//ok
 wybor->SetParent(&wnd);
 wybor->DefaultResponse();
 wnd.Invalidate();
}


void EditCategFunc(Wnd& wnd, void* ptr)
{
 tout("EditCategFunc");
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 300, 200);
 if (!wybor) panic("memory exception in creating win_child!");
 wybor->WinDefaults("Wybierz opcje");
 wybor->SetButtonsNumber(4);
 wybor->CreateButton(0, "Ilosc kateg",25 , 65, SetNumCategFunc, true, 100);//ok
 wybor->CreateButton(1, "Nazwy kateg",175 , 65, SetNamesCategFunc, false, 100);//ok
 wybor->CreateButton(2, "Pokaz kateg",25 , 105, ShowCategFunc, false, 100);//ok
 wybor->CreateButton(3, "Powrot",175 , 105, StdOKFunc, false, 100);//ok
 wybor->SetParent(&wnd);
 wybor->DefaultResponse();
 wnd.Invalidate();
}


void SetLevSpellsNum(Wnd& wnd, void* ptr)
{
 tout("SetLevSpellsNum");
 char str[100];
 sprintf(str,"Aktualna ilosc poziomow to %d\nPodaj nowa:", db->ilosc_poziomow);
 char* dat = wnd.CreateWriteBox(str);
 int ne=0;
 sscanf(dat,"%d", &ne);
 if ((ne < 0) || (ne >= MAX_LEV_S))
   {
    wnd.CreateMBoxChild("Za duzo lub za malo!");
    return;
   }
 db->ilosc_poziomow = ne;
}


void SwapSpell(int lev, int i, int j)
{
 tout("SwapSpell");
 char pom[STRING];
 int mod;
 strcpy(pom,db->zaklecie[lev][j]);
 strcpy(db->zaklecie[lev][j],db->zaklecie[lev][i]);
 strcpy(db->zaklecie[lev][i], pom);
 strcpy(pom,db->req_magic[lev][j]);
 strcpy(db->req_magic[lev][j],db->req_magic[lev][i]);
 strcpy(db->req_magic[lev][i], pom);
 mod = db->spell_cost[lev][j];
 db->spell_cost[lev][j] = db->spell_cost[lev][i];
 db->spell_cost[lev][i] = mod;
}


void ASortSpellsFunc(Wnd& wnd, void* ptr)
{
 tout("ASortSpellsFunc");
 for (int O=0;O<db->ilosc_poziomow;O++)
  {
  for (int i=0;i<db->ilosc_czarow[O];i++)
    {
     for (int j=0;j<db->ilosc_czarow[O]-(i+1);j++)
        {
	 if (StrMore(db->zaklecie[O][j], db->zaklecie[O][j+1]))
	  {
	   SwapSpell(O, j+1, j);
	  }
	}
     }
  }
}


void EditSpellsFunc(Wnd& wnd, void* ptr)
{
 tout("EditSpellsFunc");
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 400, 300);
 if (!wybor) panic("memory exception in creating win_child!");
 wybor->WinDefaults("Wybierz opcje");
 wybor->SetButtonsNumber(10);
 wybor->CreateButton(0, "Dodaj",30 , 100, AddSpellFunc, true, 140);//ok
 wybor->CreateButton(1, "Usun", 230 , 100, DelSpellFunc, false, 140);//ok
 wybor->CreateButton(2, "Pokaz/edytuj", 30 , 130, ShowSpellFunc, false, 140);//ok
 wybor->CreateButton(3, "Usun ostatnia", 230 , 130, DelLastSpell, false, 140);//ok
 wybor->CreateButton(4, "Usun wszystkie!",30 , 160, DelAllSpellFunc, false, 140);//ok
 wybor->CreateButton(5, "Usun wsz. poz!", 230 , 160, ShredSpells, false, 140);//ok
 wybor->CreateButton(6, "Zapisz", 30 , 190, FSaveBase, false, 140);//ok
 wybor->CreateButton(7, "Ilosc poz", 230 , 190, SetLevSpellsNum, false, 140);//ok
 wybor->CreateButton(8, "Sortuj", 30 , 220, ASortSpellsFunc, false, 140);//ok
 wybor->CreateButton(9, "Powrot", 230 , 220, StdOKFunc, false, 140);//ok
 wybor->SetParent(&wnd);
 wybor->DefaultResponse();
 wnd.Invalidate();
}


void AddSkillFunc(Wnd& wnd, void* ptr)
{
 tout("AddSkillFunc");
 if (db->ilosc[3] == MAX_SKI)
    {
     wnd.CreateMBoxChild("Osiagnieto maksymalna ilosc umiejetnosci");
     return;
    }
 int num = db->ilosc[3];
 strcpy(db->s_name[num], wnd.CreateWriteBox("Podaj nazwe umiejetnosci"));
 if (!strcmp(db->s_name[num],"")) {wnd.CreateMBoxChild("Nazwa nie moze byc pusta"); return;}
 char ** tmp;
 tmp = new char*[db->ilosc[4]+1];
 if (!tmp) panic("memory exception in DelRaceFunc - tmp");
 for (int i=0;i<db->ilosc[4];i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in DelRaceFunc - tmp[]");
     strcpy(tmp[i], db->c_name[i]);
    }
 char* nam = wnd.CreateCheckBoxChild("Wybierz kategorie", tmp, db->ilosc[4]);
 int temp = -1;
 for (int i=0;i<db->ilosc[4];i++)
    {
     if (!strcmp(db->c_name[i], nam)) { temp = i; break; }
    }
 if (temp == -1) return ;
 db->s_mod[num][CATEG] = temp;
 //for (int i=1;i<=db->ilosc[4];i++) if (tmp[i]) delete [] tmp[i];
 //if (tmp) delete [] tmp;
 tmp = NULL;
 tmp = new char*[db->ilosc[0]+1];
 if (!tmp) panic("memory exception in DelRaceFunc - tmp");
 for (int i=0;i<db->ilosc[0];i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in DelRaceFunc - tmp[]");
     strcpy(tmp[i], db->name_att[i]);
    }
 nam = wnd.CreateCheckBoxChild("Wybierz ceche testujaca", tmp, db->ilosc[0]);
 temp = -1;
 for (int i=0;i<db->ilosc[0];i++)
    {
     if (!strcmp(db->name_att[i], nam)) { temp = i; break; }
    }
 if (temp == -1) return ;
 db->s_mod[num][ATTRIBUTE] = temp;
 int val=0;
 nam = wnd.CreateWriteBox("Podaj modyfikator testu");
 sscanf(nam,"%d", &val);
 db->s_mod[num][MOD] = val;
 nam = wnd.CreateWriteBox("Podaj mnoznik\nDomyslny: 3");
 val=3;
 sscanf(nam,"%d", &val);
 db->s_mod[num][MULTIPLE] = val;
 nam = wnd.CreateWriteBox("Podaj poziom magii");
 val=0;
 sscanf(nam,"%d", &val);
 db->s_mod[num][MAGIC] = val;
 nam = wnd.CreateWriteBox("Podaj trudnosc");
 val=1;
 sscanf(nam,"%d", &val);
 db->s_mod[num][DIFF] = val;
 db->ilosc[3]++;
}


void DelSkillFunc(Wnd& wnd, void* ptr)
{
 tout("DelSkillFunc");
 if (db->ilosc[3] == 0)
    {
     wnd.CreateMBoxChild("Brak umiejetnosci do usuniecia");
     return;
    }
 char ** tmp;
 tmp = new char*[db->ilosc[4]+1];
 if (!tmp) panic("memory exception in DelRaceFunc - tmp");
 for (int i=0;i<db->ilosc[4];i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in DelRaceFunc - tmp[]");
     strcpy(tmp[i], db->c_name[i]);
    }
 char* nam = wnd.CreateCheckBoxChild("Wybierz kategorie", tmp, db->ilosc[4]);
 int temp = -1;
 for (int i=0;i<db->ilosc[4];i++)
    {
     if (!strcmp(db->c_name[i], nam)) { temp = i; break; }
    }
 if (temp == -1) return ;
 for (int i=1;i<=db->ilosc[4];i++) if (tmp[i]) delete [] tmp[i];
 if (tmp) delete [] tmp;
 tmp = new char*[db->ilosc[3]+1];
 if (!tmp) panic("memory exception in DelRaceFunc - tmp");
 int x=0;
 for (int i=0;i<db->ilosc[3];i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in DelRaceFunc - tmp[]");
     if (db->s_mod[i][CATEG] == temp)
       {
        strcpy(tmp[x], db->s_name[i]);
	x++;
       }
    }
 if (!x)
   {
    wnd.CreateMBoxChild("Brak umiejetnosci w tej kategorii");
    return;
   }
 nam = wnd.CreateCheckBoxVert("Wybierz umiejetnosc", tmp, x);
 temp = -1;
 for (int i=0;i<db->ilosc[3];i++)
    {
     if (!strcmp(db->s_name[i], nam)) { temp = i; break; }
    }
 if (temp == -1) return ;
 db->ilosc[3]--;
 for (int i=temp;i<db->ilosc[3];i++)
    {
     strcpy(db->s_name[i],db->s_name[i+1]);
     db->s_mod[i][ATTRIBUTE] = db->s_mod[i+1][ATTRIBUTE];
     db->s_mod[i][MOD]       = db->s_mod[i+1][MOD];
     db->s_mod[i][MAGIC]     = db->s_mod[i+1][MAGIC];
     db->s_mod[i][MULTIPLE]  = db->s_mod[i+1][MULTIPLE];
     db->s_mod[i][DIFF]      = db->s_mod[i+1][DIFF];
    }
}


void EdSkillFunc(Wnd& wnd, void* ptr)
{
 tout("EdSkillFunc");
 char* str = wnd.RetBtnName(1);
 int num=0;
 sscanf(str,"Edytuj %d", &num);
 strcpy(db->s_name[num], wnd.CreateWriteBox("Podaj nazwe umiejetnosci"));
 if (!strcmp(db->s_name[num],"")) {wnd.CreateMBoxChild("Nazwa nie moze byc pusta"); return;}
 char ** tmp;
 tmp = new char*[db->ilosc[4]+1];
 if (!tmp) panic("memory exception in DelRaceFunc - tmp");
 for (int i=0;i<db->ilosc[4];i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in DelRaceFunc - tmp[]");
     strcpy(tmp[i], db->c_name[i]);
    }
 char* nam = wnd.CreateCheckBoxChild("Wybierz kategorie", tmp, db->ilosc[4]);
 int temp = -1;
 for (int i=0;i<db->ilosc[4];i++)
    {
     if (!strcmp(db->c_name[i], nam)) { temp = i; break; }
    }
 if (temp == -1) return ;
 db->s_mod[num][CATEG] = temp;
 for (int i=1;i<=db->ilosc[4];i++) if (tmp[i]) delete [] tmp[i];
 if (tmp) delete [] tmp;
 tmp = NULL;
 tmp = new char*[db->ilosc[0]+1];
 if (!tmp) panic("memory exception in DelRaceFunc - tmp");
 for (int i=0;i<db->ilosc[0];i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in DelRaceFunc - tmp[]");
     strcpy(tmp[i], db->name_att[i]);
    }
 nam = wnd.CreateCheckBoxChild("Wybierz ceche testujaca", tmp, db->ilosc[0]);
 temp = -1;
 for (int i=0;i<db->ilosc[0];i++)
    {
     if (!strcmp(db->name_att[i], nam)) { temp = i; break; }
    }
 if (temp == -1) return ;
 db->s_mod[num][ATTRIBUTE] = temp;
 int val;
 nam = wnd.CreateWriteBox("Podaj modyfikator testu");
 val=0;
 sscanf(nam,"%d", &val);
 db->s_mod[num][MOD] = val;
 nam = wnd.CreateWriteBox("Podaj mnoznik\nDomyslny: 3");
 val=3;
 sscanf(nam,"%d", &val);
 db->s_mod[num][MULTIPLE] = val;
 nam = wnd.CreateWriteBox("Podaj poziom magii");
 val=0;
 sscanf(nam,"%d", &val);
 db->s_mod[num][MAGIC] = val;
 nam = wnd.CreateWriteBox("Podaj trudnosc");
 val=1;
 sscanf(nam,"%d", &val);
 db->s_mod[num][DIFF] = val;
 wnd.AboutQuit(1);
}


void ShowSkillNum(Wnd& wnd, int num)
{
 tout("ShowSkillNum");
 char tmp[450];
 sprintf(tmp,"Wlasciwosci umiejetnosci:\n (%d)%s\nKategoria: (%d)%s\nCecha testujaca: (%d)%s\nModyfikator testu: %d\nMnoznik: %d\nTrudnosc: %d",
 num, db->s_name[num], db->s_mod[num][CATEG], db->c_name[db->s_mod[num][CATEG]], db->s_mod[num][ATTRIBUTE],
 db->name_att[db->s_mod[num][ATTRIBUTE]], db->s_mod[num][MOD], db->s_mod[num][MULTIPLE], db->s_mod[num][DIFF]);
 char str[30];
 sprintf(str,"Edytuj %d", num);
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 400, 450);
 if (!wybor) panic("memory exception in creating win_child!");
 wybor->WinDefaults("Wlasciwosci rasy");
 wybor->PrepareWndText(tmp);
 wybor->StdTextWrite();
 wybor->SetParent(&wnd);
 wybor->SetButtonsNumber(2);
 wybor->CreateButton(0, "OK", 60 , 410, StdOKFunc, true);
 wybor->CreateButton(1, str, 260 , 410, EdSkillFunc, false);
 wybor->DefaultResponse();
 wnd.Invalidate();
}


void ShowSkillFunc(Wnd& wnd, void* ptr)
{
 tout("ShowSkillFunc");
 if (db->ilosc[3] == 0)
    {
     wnd.CreateMBoxChild("Brak umiejetnosci");
     return;
    }
 char ** tmp;
 tmp = new char*[db->ilosc[4]+1];
 if (!tmp) panic("memory exception in DelRaceFunc - tmp");
 for (int i=0;i<db->ilosc[4];i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in DelRaceFunc - tmp[]");
     strcpy(tmp[i], db->c_name[i]);
    }
 char* nam = wnd.CreateCheckBoxChild("Wybierz kategorie", tmp, db->ilosc[4]);
 int temp = -1;
 for (int i=0;i<db->ilosc[4];i++)
    {
     if (!strcmp(db->c_name[i], nam)) { temp = i; break; }
    }
 if (temp == -1) return ;
 for (int i=1;i<=db->ilosc[4];i++) if (tmp[i]) delete [] tmp[i];
 if (tmp) delete [] tmp;
 tmp = new char*[db->ilosc[3]+1];
 if (!tmp) panic("memory exception in DelRaceFunc - tmp");
 int x=0;
 for (int i=0;i<db->ilosc[3];i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in DelRaceFunc - tmp[]");
     if (db->s_mod[i][CATEG] == temp)
       {
        strcpy(tmp[x], db->s_name[i]);
	x++;
       }
    }
 if (!x)
   {
    wnd.CreateMBoxChild("Brak umiejetnosci w tej kategorii");
    return;
   }
 nam = wnd.CreateCheckBoxVert("Wybierz umiejetnosc", tmp, x);
 temp = -1;
 for (int i=0;i<db->ilosc[3];i++)
    {
     if (!strcmp(db->s_name[i], nam)) { temp = i; break; }
    }
 if (temp == -1) return ;
 ShowSkillNum(wnd, temp);
}


void DelLastSkill(Wnd& wnd, void* ptr)
{
 if (db->ilosc[3] == 0)
    {
     wnd.CreateMBoxChild("Nie ma nic do usuniecia");
     return;
    }
 db->ilosc[3]--;
}


void ShredSkills(Wnd& wnd, void* ptr)
{
 tout("ShredSkills");
 int ok = wnd.CreateYesNoChild("Na pewno usunac WSZYSTKIE?");
 if (ok) db->ilosc[3] = 0;
}


void EditSkillsFunc(Wnd& wnd, void* ptr)
{
 tout("EditSkillsFunc");
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 400, 300);
 if (!wybor) panic("memory exception in creating win_child!");
 wybor->WinDefaults("Wybierz opcje");
 wybor->SetButtonsNumber(8);
 wybor->CreateButton(0, "Dodaj",30 , 100, AddSkillFunc, true, 140);//ok
 wybor->CreateButton(1, "Usun", 230 , 100, DelSkillFunc, false, 140);//ok
 wybor->CreateButton(2, "Pokaz/edytuj", 30 , 130, ShowSkillFunc, false, 140);//ok
 wybor->CreateButton(3, "Usun ostatnia", 230 , 130, DelLastSkill, false, 140);//ok
 wybor->CreateButton(4, "Usun wszystkie", 30 , 160, ShredSkills, false, 140);//ok
 wybor->CreateButton(5, "Zapisz", 230 , 160, FSaveBase, false, 140);//ok
 wybor->CreateButton(6, "Sortuj", 30 , 190, ASortSkillFunc, false, 140);//ok
 wybor->CreateButton(7, "Powrot", 230 , 190, StdOKFunc, false, 140);//ok
 wybor->SetParent(&wnd);
 wybor->DefaultResponse();
 wnd.Invalidate();
}


void SortAll(Wnd& wnd, void* ptr)
{
 tout("SortAll");
 ASortSkillFunc(wnd, NULL);
 ASortRaceFunc(wnd, NULL);
 ASortProfFunc(wnd, NULL);
 ASortItemFunc(wnd, NULL);
 ASortSpellsFunc(wnd, NULL);
}


void Base(Wnd& wnd, void* ptr)
{
 tout("Base");
 if (!dbase)
  {
   int ok = wnd.CreateYesNoChild("Najpierw nalezy wczytac baze!\nWczytac DataBase/morgoth?");
   if (!ok) return;
   else
     {
      CreateDB("morgoth");
      int ok = LoadDB("morgoth", wnd);
      if (!ok) return ;
      dbase = true;
     }
  }
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 400, 300);
 if (!wybor) panic("memory exception in creating win_child!");
 wybor->CreateGC();
 wybor->SetFont("*-helvetica-*-12-*");
 wybor->SetCaption("Baza danych");
 wybor->DeclareResponseTable(MEDI_RESPONSES);
 wybor->SelectExposeFunc(StdExposeFunc);
 wybor->SelectConfigureFunc(DenyResizeFunc);
 wybor->SelectKeyPressFunc(eXtendedKeyPress);
 wybor->SelectKeyReleaseFunc(eXtendedKeyRelease);
 wybor->SelectButtonPressFunc(eXtendedButtonPress);
 wybor->SelectButtonReleaseFunc(eXtendedButtonRelease);
 wybor->PrepareWndText("Wybierz funkcje");
 wybor->StdTextWrite();
 wybor->SetButtonsNumber(13);
 wybor->CreateButton(0, "Zmien kod",30 , 100, ChangeBasePswd, false, 140);  //ok
 wybor->CreateButton(1, "Edytuj cechy", 230 , 100, EditAttrFunc, false, 140);//ok
 wybor->CreateButton(2, "Edytuj rasy", 30 , 130, EditRacesFunc, false, 140); //ok
 wybor->CreateButton(3, "Edytuj profesje", 230 , 130, EditProfsFunc, false, 140);//ok
 wybor->CreateButton(4, "Zmien kategorie",30 , 160, EditCategFunc, false, 140);//ok
 wybor->CreateButton(5, "Edytuj umiejetnosci", 230 , 160, EditSkillsFunc, false, 140);
 wybor->CreateButton(6, "Edytuj zaklecia", 30 , 190, EditSpellsFunc, false, 140);
 wybor->CreateButton(7, "Edytuj przedmioty", 230 , 190, EdItemsFunc, false, 140);//ok
 wybor->CreateButton(8, "Edytuj plec", 30 , 220, GenderEdit, false, 140);     //ok
 wybor->CreateButton(9, "Generuj HTML", 230 , 220, GenerateHTML, true, 140);  //ok
 wybor->CreateButton(10,"Powrot", 230 , 250, StdOKFunc, false, 140); //ok
 wybor->CreateButton(11,"Edytuj poziomy", 30 , 250, EditLevelsFunc, false, 140);//ok
 wybor->CreateButton(12,"Zapisz", 130 , 70, FSaveBase, false, 140);  //ok
 wybor->SetParent(&wnd);
 wybor->DefaultResponse();
 AboutSave(wnd, NULL);
 wnd.Invalidate();
}


void EditFunc(Wnd& wnd, void* ptr)
{
 tout("EditFunc");
 int num = 0;
 char** table = ReadFolderToTable(num, "./DataBase");
 if (!table)
   {
    wnd.CreateMBoxChild("PANIC: Cannot read anything from ./DataBase!!!");
    trace("cannot read dir_entires for ./DataBase!");
   }
 if (!num)
   {
    wnd.CreateMBoxChild("Brak plikow w folderze: ./DataBase");
    return ;
   }
 FSort(table, num);
 char* selected = wnd.CreateCheckBoxChild("Wybierz Baze danych:", table, num);
 CreateDB(selected);
 int ok = LoadDB(selected, wnd);
 if (!ok) return;
 dbase = true;
 ok = CheckPassword(db, wnd);
 if (!ok)
    {
     wnd.CreateMBoxChild("Blad procedury dostepu!");
     //wnd.Invalidate();
     return;
    }
 //edits
 Base(wnd, NULL);
 wnd.AboutQuit(1);
 //wnd.Invalidate();
}


void AccessEditFunc(Wnd& wnd, void* ptr)
{
 tout("AccessEditFunc");
 int ok = CheckPassword(db, wnd);
 if (!ok) return;
 Base(wnd, NULL);
}


void CreateFunc(Wnd& wnd, void* ptr)
{
 tout("CreateFunc");
 char* name = wnd.CreateWriteBox("Podaj nazwe nowej bazy:");
 int ok = CreateDB(name, true);
 if (!ok)
   {
    wnd.CreateMBoxChild("Podaj inna nazwe...\nTaka baza juz istnieje");
    return;
   }
 ok = SaveBase(wnd);
 if (!ok) return ;
 dbase = true;
 //edits
 Base(wnd, NULL);
 wnd.AboutQuit(1); //FIXME continue
 dbase = true;
 //wnd.Invalidate();
}

//POST SECTION

void Protection(int check, RPG* post,FILE* plik, Wnd& wnd)
{
 if (!plik)
   {
    wnd.CreateMBoxChild("Struktura \"FILE*\" wskazuje NULL!");
    return;
   }
 tout("Protection");
 if (!check)
        {
         int zn = wnd.CreateYesNoChild("Plik postaci jest uszkodzony!\nKontynuowac?");
         if (!zn) {if (db) delete db;panic("Postac uszkodzona!"); fclose(plik);plik = NULL;}
         }
}


int LoadChar(FILE* plik, Wnd& wnd)
{
 tout("LoadChar");
 if (!plik)
   {
    wnd.CreateMBoxChild("Struktura \"FILE*\" wskazuje NULL!");
    return 0;
   }
 int check;
 check = fscanf(plik,"%d\n",&post->ilosc_danych);
 Protection(check,post,plik,wnd);
 for (int i=0;i<post->ilosc_danych;i++)
        {
         fscanf(plik,"%s\n",post->dane[i]);
         DecompressData(post->dane[i]);
        }
 fscanf(plik,"%d\n",&post->ilosc_cech);
 for (int i=0;i<post->ilosc_cech;i++)
        {
         fscanf(plik,"%d\n",&post->cecha[i]);
         fscanf(plik,"%s\n",post->n_cechy[i]);
         DecompressData(post->n_cechy[i]);
        }
 fscanf(plik,"%x\n",&post->sv);
 fscanf(plik,"%d\n",&post->st);
 fscanf(plik,"%d\n",&post->il_um);
 for (int i=0;i<post->il_um;i++)
        {
         fscanf(plik,"%s %d %d %d %d %d\n",post->um[i],&post->tesum[i],&post->pozum[i],&post->trum[i],&post->ceum[i],&post->mulum[i]);
         DecompressData(post->um[i]);
        }
 fscanf(plik,"%d\n",&post->il_ekw);
 for (int i=0;i<post->il_ekw;i++)
        {
         fscanf(plik,"%s\n",post->ekw[i]);
         DecompressData(post->ekw[i]);
        }
 fscanf(plik,"%d\n",&post->PD);
 for (int i=0;i<post->ilosc_cech;i++)
        {
         fscanf(plik,"%d\n",&post->p_c[i]);
        }
 for (int i=0;i<post->il_um;i++)
        {
         fscanf(plik,"%d\n",&post->p_u[i]);
        }
 check = fscanf(plik,"%d\n",&post->rl);
 Protection(check,post,plik,wnd);
 for (int i=0;i<post->il_um;i++)
        {
         fscanf(plik,"%d\n",&post->magum[i]);
        }
 fscanf(plik,"%d\n",&post->stMM);
 fscanf(plik,"%d\n",&post->numMM);
 fscanf(plik,"%d\n",&post->IsDead);
 fscanf(plik,"%d\n",&post->lev);
 fscanf(plik,"%d\n",&post->lev_p);
 fscanf(plik,"%s\n",post->pname);
 DecompressData(post->pname);
 fscanf(plik,"%d\n",&post->cash);
 fscanf(plik,"%d\n",&post->sp);
 fscanf(plik,"%d\n",&post->spt);
 fscanf(plik,"%d\n",&post->max_lev);
 check = fscanf(plik,"%d\n",&post->il_spell);
 Protection(check,post,plik,wnd);
 for (int i=0;i<post->il_spell;i++)
 	{
    fscanf(plik,"%d\n",&post->spell_cost[i]);
    fscanf(plik,"%d\n",&post->spell_level[i]);
    fscanf(plik,"%s\n",post->spell[i]);
    DecompressData(post->spell[i]);
   }
 check = fscanf(plik,"%d\n",&post->mage);
 for (int i=0;i<post->il_um;i++)
        {
         fscanf(plik,"%d\n",&post->modum[i]);
        }
 fscanf(plik,"%d\n", &post->randomedAtt);
 fscanf(plik,"%d\n", &post->modAtt);
 fscanf(plik,"%d\n", &post->modSkill);
 fscanf(plik,"%s\n", post->used_base);
 fscanf(plik,"%d\n", &post->modEkw);
 fscanf(plik,"%d\n", &post->modSpell);
 if (strcmp(post->used_base, db->dbname))
   {
    char str[STRING*2];
    sprintf(str,"Ta postac byla tworzona\nna podstawie bazy: %s\nA wczytano baze: %s\nTo moze spowodowac bledy\nKontynuowac?", post->used_base, db->dbname);
    int ok = wnd.CreateYesNoChild(str);
    if (!ok) return 0;
   }
 Protection(check,post,plik,wnd);
 if (post->IsDead)
 	{
    wnd.CreateMBoxChild("Postac jest martwa, nie mozna jej wczytac!");
    //wnd.Invalidate();
    return 0;
   }
 //printf("postac->pname = %s\n", post->pname);
 Uncompress(post);
 return 1;
}


void Uncompress(RPG* post)
{
 tout("Uncompress(post)");
 DecompressData(post->pname);
 if (!strcmp(post->pname,"")) strcpy(post->pname,"-");
 DecompressData(post->used_base);
 if (!strcmp(post->used_base,"")) strcpy(post->used_base,"-");
 for (int i=0;i<post->ilosc_danych;i++)
        {
         DecompressData(post->dane[i]);
	 if (!strcmp(post->dane[i],"")) strcpy(post->dane[i],"-");
        }
 for (int i=0;i<post->ilosc_cech;i++)
        {
         DecompressData(post->n_cechy[i]);
	 if (!strcmp(post->n_cechy[i],"")) strcpy(post->n_cechy[i],"-");
        }
 for (int i=0;i<post->il_um;i++)
        {
         DecompressData(post->um[i]);
	 if (!strcmp(post->um[i],"")) strcpy(post->um[i],"-");
        }
 for (int i=0;i<post->il_ekw;i++)
        {
         DecompressData(post->ekw[i]);
	 if (!strcmp(post->ekw[i],"")) strcpy(post->ekw[i],"-");
        }
 for (int i=0;i<post->il_spell;i++)
        {
         DecompressData(post->spell[i]);
        }
}


void Compress(RPG* post)
{
 tout("Compress(post)");
 CompressData(post->pname);
 if (!strcmp(post->pname,"")) strcpy(post->pname,"-");
 CompressData(post->used_base);
 if (!strcmp(post->used_base,"")) strcpy(post->used_base,"-");
 for (int i=0;i<post->ilosc_danych;i++)
        {
         CompressData(post->dane[i]);
	 if (!strcmp(post->dane[i],"")) strcpy(post->dane[i],"-");
        }
 for (int i=0;i<post->ilosc_cech;i++)
        {
         CompressData(post->n_cechy[i]);
	 if (!strcmp(post->n_cechy[i],"")) strcpy(post->n_cechy[i],"-");
        }
 for (int i=0;i<post->il_um;i++)
        {
         CompressData(post->um[i]);
	 if (!strcmp(post->um[i],"")) strcpy(post->um[i],"-");
        }
 for (int i=0;i<post->il_ekw;i++)
        {
         CompressData(post->ekw[i]);
	 if (!strcmp(post->ekw[i],"")) strcpy(post->ekw[i],"-");
        }
 for (int i=0;i<post->il_spell;i++)
        {
         CompressData(post->spell[i]);
        }
}


void SaveChar(FILE* plik)
{
 tout("SaveChar");
 if (!plik) return;
 Compress(post);
  fprintf(plik,"%d\n",post->ilosc_danych);
 for (int i=0;i<post->ilosc_danych;i++)
        {
         CompressData(post->dane[i]);
         fprintf(plik,"%s\n",post->dane[i]);
        }
 fprintf(plik,"%d\n",post->ilosc_cech);
 for (int i=0;i<post->ilosc_cech;i++)
        {
         fprintf(plik,"%d\n",post->cecha[i]);
         CompressData(post->n_cechy[i]);
         fprintf(plik,"%s\n",post->n_cechy[i]);
        }
 fprintf(plik,"%x\n",post->sv);
 fprintf(plik,"%d\n",post->st);
 fprintf(plik,"%d\n",post->il_um);
 for (int i=0;i<post->il_um;i++)
        {
         CompressData(post->um[i]);
         fprintf(plik,"%s %d %d %d %d %d\n",post->um[i],post->tesum[i],post->pozum[i],post->trum[i],post->ceum[i],post->mulum[i]);
        }
 fprintf(plik,"%d\n",post->il_ekw);
 for (int i=0;i<post->il_ekw;i++)
        {
         CompressData(post->ekw[i]);
         fprintf(plik,"%s\n",post->ekw[i]);
        }
 fprintf(plik,"%d\n",post->PD);
 for (int i=0;i<post->ilosc_cech;i++)
        {
         fprintf(plik,"%d\n",post->p_c[i]);
        }
 for (int i=0;i<post->il_um;i++)
        {
         fprintf(plik,"%d\n",post->p_u[i]);
        }
 fprintf(plik,"%d\n",post->rl);
  for (int i=0;i<post->il_um;i++)
        {
         fprintf(plik,"%d\n",post->magum[i]);
        }
 fprintf(plik,"%d\n",post->stMM);
 fprintf(plik,"%d\n",post->numMM);
 fprintf(plik,"%d\n",post->IsDead);
 fprintf(plik,"%d\n",post->lev);
 fprintf(plik,"%d\n",post->lev_p);
 CompressData(post->pname);
 fprintf(plik,"%s\n",post->pname);
 fprintf(plik,"%d\n",post->cash);
 fprintf(plik,"%d\n",post->sp);
 fprintf(plik,"%d\n",post->spt);
 fprintf(plik,"%d\n",post->max_lev);
 fprintf(plik,"%d\n",post->il_spell);
 for (int i=0;i<post->il_spell;i++)
 	{
    fprintf(plik,"%d\n",post->spell_cost[i]);
    fprintf(plik,"%d\n",post->spell_level[i]);
    CompressData(post->spell[i]);
    fprintf(plik,"%s\n",post->spell[i]);
   }
 fprintf(plik,"%d\n",post->mage);
  for (int i=0;i<post->il_um;i++)
        {
         fprintf(plik,"%d\n",post->modum[i]);
        }
 fprintf(plik,"%d\n", post->randomedAtt);
 fprintf(plik,"%d\n", post->modAtt);
 fprintf(plik,"%d\n", post->modSkill);
 fprintf(plik,"%s\n", post->used_base);
 fprintf(plik,"%d\n", post->modEkw);
 fprintf(plik,"%d\n", post->modSpell);
// fclose(plik);
// plik = NULL;
 Uncompress(post);
}


int CheckPassChar(Wnd& wnd)
{
 tout("CheckPassChar");
 if (post->sv == -1)
   {
    wnd.CreateMBoxChild("Postac nie jest dostepna do edycji!");
    //wnd.Invalidate();
    return 0;
   }
 if (post->sv != 0)
        {
         char* tmp = wnd.CreateWriteBox("Podaj kod dostepu", true);
	 int a=0;
	 sscanf(tmp,"%d", &a);
	 //wnd.Invalidate();
         if (a != post->sv)
                {
                 wnd.CreateMBoxChild("Podano bledny kod!");
		 //wnd.Invalidate();
		 return 0;
                }
	 else return 1;
        }
 return 1;
}


int CreateRPG(char* name, bool ab_new = false)
{
 tout("CreateRPG");
 char temp[STRING];
 strcpy(temp,"Characters/");
 strcat(temp,name);
 post = new RPG;
 if (!post) panic("memory exception in static post");
 strcpy(post->pname,temp);
 if (ab_new)
   {
    FILE* test = NULL;
    if ((test = fopen(post->pname,"r")) != NULL)
      {
       if (test) fclose(test);
       test = NULL;
       return 0;
      }
   }
 return 1;
}


int LoadPost(Wnd& wnd)
{
 tout("LoadPost");
 FILE* plik = NULL;
 if ((plik = fopen(post->pname,"r")) != NULL)
        {
         int ok = LoadChar(plik, wnd);
	 if (!ok)
	   {
            wnd.CreateMBoxChild("Blad przy wczytywaniu postaci!");
	    //wnd.Invalidate();
	    if (plik) fclose(plik);
	    plik = NULL;
	    return 0;
	   }
         //AboutMM0(post,base_name);
         ok = CheckPassChar(wnd);
	 if (!ok)
	   {
	    if (plik) fclose(plik);
	    plik = NULL;
	    return 0;
	   }
        }
 else
   {
    wnd.CreateMBoxChild("Nie mozna odczytac postaci");
    //wnd.Invalidate();
    return 0;
   }
 return 1;
}


int LoadPostNoA(Wnd& wnd)
{
 tout("LoadPost");
 FILE* plik = NULL;
 if ((plik = fopen(post->pname,"r")) != NULL)
        {
         int ok = LoadChar(plik, wnd);
	 if (!ok)
	   {
            wnd.CreateMBoxChild("Blad przy wczytywaniu postaci!");
	    //wnd.Invalidate();
	    if (plik) fclose(plik);
	    plik = NULL;
	    return 0;
	   }
        }
 else
   {
    wnd.CreateMBoxChild("Nie mozna odczytac postaci");
    //wnd.Invalidate();
    return 0;
   }
 return 1;
}


void AboutMM0(RPG* post)
{
 tout("AboutMM0");
 char bname[STRING];
 strcpy(bname, db->dbname);
 downcase(bname);
 if (strcmp(bname,"database/saradok") == 0)
        {
         if (post->cecha[post->numMM] < 0)
                {
                 post->cecha[post->numMM] = 0;
                }
        }
 else return;
}

void RealMM(RPG*);
void SaradokMM();

void RecountSkills()
{
 tout("RecountSkills");
 for (int i=0;i<post->il_um;i++)
   {
    post->tesum[i] = (post->cecha[post->ceum[i]]/2) + (post->mulum[i]*post->pozum[i])+ post->modum[i];
   }
}


void RecountAll()
{
 tout("RecountAll");
 RecountSkills();
 RealMM(post);
 SaradokMM();
 AboutMM0(post);
}


int FastLoad(Wnd& wnd)
{
 tout("FastLoad");
 FILE* plik = NULL;
 if ((plik = fopen(post->pname,"r"))==NULL)
        {
         wnd.CreateMBoxChild("Nie moge odczytac postaci");
	 //wnd.Invalidate();
	 return 0;
        }
 LoadChar(plik, wnd);
 RecountAll();
 if (plik) fclose(plik);
 plik = NULL;
 //wnd.Invalidate();
 return 1;
}


int FastSave(Wnd& wnd)
{
 tout("FastSave");
 FILE* plik = NULL;
 if ((plik = fopen(post->pname,"w"))==NULL)
        {
         wnd.CreateMBoxChild("Nie moge zapisac postaci");
	 return 0;
        }
 RecountAll();
 SaveChar(plik);
 if (plik) fclose(plik);
 plik = NULL;
 return 1;
}

void RealMM(RPG*);

void FFastSave(Wnd& wnd, void* ptr)
{
 tout("FFastSave");
 RecountAll();
 int ok = FastSave(wnd);
 if (!ok) wnd.CreateMBoxChild("Nie udalo sie zapisac postaci!");
}


void DefaultValues()
{
 tout("DefaultValues");
 post->ilosc_cech = db->ilosc[0];
 post->st = db->st;
 for (int i=0;i<db->ilosc[0];i++)
        {
         post->cecha[i] = db->value_att[i];
         strcpy(post->n_cechy[i],db->name_att[i]);
        }
 post->il_um  = 0;
 post->il_ekw = 0;
 post->numMM = db->mm;
 strcpy(post->used_base, db->dbname);
}


void SaradokMM()
{
 tout("SaradokMM");
 char name[STRING];
 strcpy(name, db->dbname);
 downcase(name);
 if (strcmp(name,"database/saradok") == 0)
        {
         int ran = (random(6) + random(6) +2);
         post->cecha[10] += ran;
        }
 if (strcmp(name,"database/base1940") == 0)
        {
         int ran = ((random(10)+1) * (random(10)+1) * (random(10)+1)/100);
         post->cecha[9] += ran;
        }
 else return;
}


void EnterOwnModifier(Wnd& wnd, void* ptr)
{
 tout("EnterOwnModifier");
 if (!ptr) return;
 char tmp[STRING];
 int val=0;
 int* table = (int*)ptr;
 sprintf(tmp,"Podaj modyfikator %s dawny %d", db->name_att[wnd.RetPressed()], table[wnd.RetPressed()]);
 char* mod = wnd.CreateWriteBox(tmp);
 sscanf(mod,"%d", &val);
 table[wnd.RetPressed()] = val;
}


void OwnRaceFunc(Wnd& wnd, void* ptr)
{
 tout("OwnRaceFunc");
 char* name = wnd.CreateWriteBox("Podaj nazwe rasy:");
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 220, 550);
 if (!wybor) panic("memory exception in creating win_child!");
 if (!strcmp(name,""))
  {
   wnd.CreateMBoxChild("nazwa nie moze byc pusta!");
   return ;
  }
 wybor->WinDefaults("Edytuj rase");
 wybor->SetButtonsNumber(db->ilosc[0]+1);
 char tmp[STRING];
 int* mod = new int[db->ilosc[0]];
 if (!mod) panic("memory exception in OwnRacefunc :: int* mod");
 for (int i=0;i<db->ilosc[0];i++)
   {
    mod[i] = 0;
    sprintf(tmp,"(%d)%s", i, db->name_att[i]);
    if (!i) wybor->CreateButton(0, tmp, 20 , 20, EnterOwnModifier, true, 180, 25, (void*)mod);
    else wybor->CreateButton(i, tmp, 20 , 20+i*26, EnterOwnModifier, false, 180, 25, (void*)mod);
   }
 wybor->CreateButton(db->ilosc[0], "Powrot", 50 , 25+(db->ilosc[0]*26), StdOKFunc, false, 120);
 wybor->SetParent(&wnd);
 wybor->DefaultResponse();
 wnd.SetUnusedCharPtr(wnd.RetSelectedButtonName());
 wnd.DoQuit();
 strcpy(post->dane[RASA],name);
 for (int i=0;i<post->ilosc_cech;i++)
        {
         post->cecha[i] += mod[i];
        }
}


void AccessRacesFunc(Wnd& wnd, void* ptr)
{
 tout("AccessRacesFunc");
 int ok = CheckPassword(db, wnd);
 if (!ok) return;
 EditRacesFunc(wnd, NULL);
 wnd.SetUnusedCharPtr(wnd.RetSelectedButtonName());
 wnd.DoQuit();
}


void AccessSkillsFunc(Wnd& wnd, void* ptr)
{
 tout("AccessSkillsFunc");
 int ok = CheckPassword(db, wnd);
 if (!ok) return;
 EditSkillsFunc(wnd, NULL);
}


void AccessItemsFunc(Wnd& wnd, void* ptr)
{
 tout("AccessItemsFunc");
 int ok = CheckPassword(db, wnd);
 if (!ok) return;
 EdItemsFunc(wnd, NULL);
}


void UnselectRace(int num)
{
 tout("UnselectRace");
 for (int i=0;i<db->ilosc[0];i++) post->cecha[i] -= db->mod[0][num][i];
}


int DispatchExistingRace(Wnd& wnd)
{
 tout("DispatchExistingRace");
 if (!strcmp(post->dane[RASA],"SOME DATA")) return 1;
 if (!strcmp(post->dane[RASA],"SOME_DATA")) return 1;
 int ok = wnd.CreateYesNoChild("Najpierw musze cofnac\nmodyfikatory obecnej rasy\nOK?");
 char tmpr[STRING];
 char tmpb[STRING];
 if (!ok) return 0;
 for (int i=0;i<db->ilosc[3];i++)
   {
    strcpy(tmpr, post->dane[RASA]);
    strcpy(tmpb,db->name[0][i]);
    downcase(tmpr);
    downcase(tmpb);
    if (!strcmp(tmpr, tmpb))
      {
       UnselectRace(i);
       RecountAll();
       return 1;
      }
   }
 wnd.CreateMBoxChild("Rasa tej postaci\nnie jest w uwzgledniona w bazie\nNie moge zmienic rasy\ngdyz nie wiem jak cofnac\njej modyfikatory!");
 return 0;
}


void PostRaceFunc(Wnd& wnd, void* ptr)
{
 tout("PostRaceFunc");
  if (db->ilosc[1] == 0)
   {
    wnd.CreateMBoxChild("Brak ras w bazie danych");
    return;
   }
 char ** tmp;
 int access = DispatchExistingRace(wnd);
 if (!access) return;
 again:
 tmp = new char*[db->ilosc[1]+1];
 if (!tmp) panic("memory exception in DelRaceFunc - tmp");
 for (int i=0;i<db->ilosc[1];i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in DelRaceFunc - tmp[]");
     strcpy(tmp[i], db->name[0][i]);
    }
 Button** btn = new Button*[2];
 btn[0] = new Button;
 btn[1] = new Button;
 if (!btn[0]) panic("memory exception in PostRaceFunc - Button* base_btn[0]");
 if (!btn[1]) panic("memory exception in PostRaceFunc - Button* base_btn[0]");
 btn[0]->SetBtnName("Wlasna rasa");
 btn[1]->SetBtnName("Edytuj baze");
 btn[0]->SetBtnFunc(OwnRaceFunc);
 btn[1]->SetBtnFunc(AccessRacesFunc);
 btn[0]->SetBtnSize(100, 24);
 btn[1]->SetBtnSize(100, 24);
 btn[0]->SetBtnPos(175, 500);
 btn[1]->SetBtnPos(625, 500);
 char* nam = wnd.CreateExCheckBoxVert("Wybierz rase", tmp, db->ilosc[1], 60, 900, 600, 2, btn);
 if (!strcmp(nam,"Wlasna rasa")) return;
 if (!strcmp(nam,"Edytuj baze")) goto again;
 int temp = -1;
 for (int i=0;i<db->ilosc[1];i++)
    {
     if (!strcmp(db->name[0][i], nam)) { temp = i; break; }
    }
 if (temp == -1) return ;
 strcpy(post->dane[RASA],db->name[0][temp]);
 for (int i=0;i<post->ilosc_cech;i++)
        {
         post->cecha[i] += db->mod[0][temp][i];
        }
 FFastSave(wnd, NULL);
}


void OwnProfFunc(Wnd& wnd, void* ptr)
{
 tout("OwnProfFunc");
 char* name = wnd.CreateWriteBox("Podaj nazwe profesji:");
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 220, 550);
 if (!wybor) panic("memory exception in creating win_child!");
 if (!strcmp(name,""))
  {
   wnd.CreateMBoxChild("nazwa nie moze byc pusta!");
   return ;
  }
 wybor->WinDefaults("Edytuj profesje");
 wybor->SetButtonsNumber(db->ilosc[1]+1);
 char tmp[STRING];
 int* mod = new int[db->ilosc[0]];
 if (!mod) panic("memory exception in OwnprofFunc :: int* mod");
 for (int i=0;i<db->ilosc[0];i++)
   {
    mod[i] = 0;
    sprintf(tmp,"(%d)%s", i, db->name_att[i]);
    if (!i) wybor->CreateButton(0, tmp, 20 , 20, EnterOwnModifier, true, 180, 25, (void*)mod);
    else wybor->CreateButton(i, tmp, 20 , 20+i*26, EnterOwnModifier, false, 180, 25, (void*)mod);
   }
 wybor->CreateButton(db->ilosc[0], "Powrot", 50 , 25+(db->ilosc[0]*26), StdOKFunc, false, 120);
 wybor->SetParent(&wnd);
 wybor->DefaultResponse();
 wnd.SetUnusedCharPtr(wnd.RetSelectedButtonName());
 wnd.DoQuit();
 strcpy(post->dane[PROFESJA],name);
 for (int i=0;i<post->ilosc_cech;i++)
        {
         post->cecha[i] += mod[i];
        }
}


void AccessProfsFunc(Wnd& wnd, void* ptr)
{
 tout("AccessProfsFunc");
 int ok = CheckPassword(db, wnd);
 if (!ok) return;
 EditProfsFunc(wnd, NULL);
 wnd.SetUnusedCharPtr(wnd.RetSelectedButtonName());
 wnd.DoQuit();
}


void Statistics(RPG* post,char* db2)
{
 tout("Statistics");
 char dbs[STRING];
 strcpy(dbs, db2);
 downcase(dbs);
 int for_skill = 10;
 int for_exp_skill = 4;
 int for_attribute = 10;
 int attribute_minus = 10;
 int for_exp_attribute = 4;
 int next_level = 1000;
 if (strcmp(dbs,"database/galthar") == 0)
        {
         for_skill = 9;
         for_exp_skill = 4;
         for_attribute = 2;
         attribute_minus = 70;
         for_exp_attribute = 5;
         next_level = 1200;
        }
 if (strcmp(dbs,"database/saradok") == 0)
        {
         for_skill = 10;
         for_exp_skill = 4;
         for_attribute = 10;
         attribute_minus = 11;
         for_exp_attribute = 4;
         next_level = 1000;
        }
 if (strcmp(dbs,"database/base1940") == 0)
        {
         for_skill = 10;
         for_exp_skill = 4;
         for_attribute = 15;
         attribute_minus = 5;
         for_exp_attribute = 5;
         next_level = 1000;
        }
 post->lev = 0;
 post->lev_p = 0;
 for (int i=0;i<post->il_um;i++)
 	{
    post->lev_p += for_skill*post->trum[i]*post->pozum[i]*post->pozum[i];
    post->lev_p += for_exp_skill*post->p_u[i];
   }
 for (int i=0;i<post->ilosc_cech;i++)
 	{
    post->lev_p += for_attribute*(post->cecha[i] - attribute_minus);
    post->lev_p += for_exp_attribute*post->p_c[i];
   }
 post->lev = (post->lev_p / next_level)+1;
}


void RealMM(RPG* post)
{
 tout("RealMM");
 post->numMM = db->mm;
 int MM = post->numMM;
 int baseMM = post->cecha[MM];
 post->MOC = baseMM;
 for (int i=0;i<post->il_um;i++)
        {
         post->MOC += post->magum[i]*(post->pozum[i]-1);
        }
  if (post->cecha[MM] < 0) post->cecha[MM] = 0;
 Statistics(post,db->dbname);
}


void Draw(char* par)
{
 tout("Draw");
 strcpy(par," *");
 for (int i=0;i<72;i++) strcat(par,"-");
 strcat(par,"*\n");
}

void DisplayCharPict(Wnd&, void*);

void CharacterCard(Wnd& wnd, void* ptr)
{
 tout("CharacterCard");
 RealMM(post);
 char* str = new char[20480];
 char* line = new char[255];
 if (!str) panic("memory_exception_in_new",__LINE__,__FILE__);
 if (!line) panic("memory_exception_in_new",__LINE__,__FILE__);
 strcpy(str,"");
 sprintf(line,"Poziom postaci %d punkty: %d\n", post->lev, post->lev_p);
 strcat(str, line);
 sprintf(line,"\n\n");
 strcat(str, line);
 Draw(line);
 strcat(str, line);
 sprintf(line, "\n\tPostac        %s %s  PD %d PLEC %s lat %s\n",post->dane[IMIE],post->dane[NAZWISKO],post->PD,post->dane[PLEC],post->dane[WIEK]);
 strcat(str, line);
 Draw(str);
 strcat(str, line);
 sprintf(line, " Imie:     \t%-18s \tNazwisko:\t%-18s\n",post->dane[IMIE],post->dane[NAZWISKO]);
 strcat(str, line);
 sprintf(line, " Rasa:     \t%-18s \tProf    :\t%-18s\n",post->dane[RASA],post->dane[PROFESJA]);
 strcat(str, line);
 sprintf(line, " Oczy:     \t%-18s \tWlosy:   \t%-18s\n",post->dane[OCZY],post->dane[WLOSY]);
 strcat(str, line);
 sprintf(line, " Wzrost:   \t%-18s \tWaga:    \t%-18s\n",post->dane[WZROST],post->dane[WAGA]);
 strcat(str, line);
 sprintf(line, " Charakter:\t%-25s  \t         \t%-5s     \n",post->dane[CHARAKTER],"");
 strcat(str, line);
 Draw(line);
 strcat(str, line);
 int max = (post->ilosc_cech > post->il_um) ? post->ilosc_cech : post->il_um;
 for (int i=0;i<max;i++)
        {
         if (i<post->ilosc_cech)
	   {
            if (i != post->numMM) sprintf(line, "  [%-2d]%-20s  %-3d",post->p_c[i],post->n_cechy[i],post->cecha[i]);
	    else                  sprintf(line, "  [%-2d]%-15s %-3d(%4d)",post->p_c[i],post->n_cechy[i],post->cecha[i], post->MOC);
	    strcat(str, line);
	   }
         else
	   {
            sprintf(line, " %-2s %-20s  %-3s  ","","","");
	    strcat(str, line);
	   }
         if (i<post->il_um)
	   {
            sprintf(line, "  [%-2d](%-2d)%-20s %-2d %-2d\t  ",post->p_u[i],post->trum[i],post->um[i],post->pozum[i],post->tesum[i]);
	    strcat(str, line);
	   }
         else
	   {
            sprintf(line, "    %-2s %-2s %-20s %-2s %-2s\t  ","","","","","");
	    strcat(str, line);
	   }
	 sprintf(line,"\n");
         strcat(str, line);
        }
 Draw(line);
 strcat(str, line);
 if (post->mage)
 	{
    sprintf(line,"                            Znane Zaklecia                               \n");
    strcat(str, line);
    sprintf(line, "         Czarow: %-3d     Max poz.  %-2d      MM / ture   %-3d               \n", post->il_spell, post->max_lev, post->spt);
    strcat(str, line);
    for (int i=0;i<post->il_spell;i++)
    	{
         sprintf(line, " \t%-30s     Poz.  %-2d     MM koszt   %-4d   \n", post->spell[i], post->spell_level[i], post->spell_cost[i]);
         strcat(str, line);
      }
   }
 Draw(line);
 strcat(str, line);
 for (int i=0;i<post->il_ekw;i++)
        {
         sprintf(line, "  %-70s \n",post->ekw[i]);
         strcat(str, line);
        }
 Draw(line);
 strcat(str, line);
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 640, 550);
 if (!wybor) panic("memory exception in creating win_child!");
 wybor->CreateGC();
 wybor->SetFont("*-courier-*-10-*");
 wybor->SetCaption("Karta Postaci");
 wybor->DeclareResponseTable(MEDI_RESPONSES);
 wybor->SelectExposeFunc(StdExposeFunc);
 wybor->SelectConfigureFunc(DenyResizeFunc);
 wybor->SelectKeyPressFunc(KeyPressClipControl);		//change this to eXtendedKeyPressFunc
 wybor->SelectKeyReleaseFunc(eXtendedKeyRelease);		//U'll have option without scrolls
 wybor->SelectButtonPressFunc(eXtendedButtonPress);
 wybor->SelectButtonReleaseFunc(eXtendedButtonRelease);
 wybor->PrepareWndText(str);
 wybor->MakeClipFromWindowText(16,16,624,500);  		//remove this
 wybor->EnableText(0);						// comment this
 //wybor->StdTextWrite();					// uncomment this
 wybor->SetButtonsNumber(2);
 wybor->CreateButton(0, "OK" ,450  , 510, StdOKFunc, true, 45, 20);
 wybor->CreateButton(1, "Rys" ,500  , 510, DisplayCharPict, false, 45, 20);
 wybor->DefaultResponse();
 wnd.Invalidate();
}


void UnselectProf(int num)
{
 tout("UnselectProf");
 for (int i=0;i<db->ilosc[0];i++) post->cecha[i] -= db->mod[1][num][i];
}


int DispatchExistingProf(Wnd& wnd)
{
 tout("DispatchExistingProf");
 if (!strcmp(post->dane[PROFESJA],"SOME_DATA")) return 1;
 if (!strcmp(post->dane[PROFESJA],"SOME DATA")) return 1;
 int ok = wnd.CreateYesNoChild("Najpierw musze cofnac\nmodyfikatory obecnej profesji\nOK?");
 char tmpr[STRING];
 char tmpb[STRING];
 if (!ok) return 0;
 for (int i=0;i<db->ilosc[3];i++)
   {
    strcpy(tmpr, post->dane[PROFESJA]);
    strcpy(tmpb,db->name[1][i]);
    downcase(tmpr);
    downcase(tmpb);
    if (!strcmp(tmpr, tmpb))
      {
       UnselectProf(i);
       RecountAll();
       return 1;
      }
   }
 wnd.CreateMBoxChild("profesja tej postaci\nnie jest w uwzgledniona w bazie\nNie moge zmienic profesji\ngdyz nie wiem jak cofnac\njej modyfikatory!");
 return 0;
}


void PostProfFunc(Wnd& wnd, void* ptr)
{
 tout("PostProfFunc");
  if (db->ilosc[2] == 0)
   {
    wnd.CreateMBoxChild("Brak profesji w bazie danych!");
    return;
   }
 char ** tmp;
 int access = DispatchExistingProf(wnd);
 if (!access) return;
 again:
 tmp = new char*[db->ilosc[2]+1];
 if (!tmp) panic("memory exception in DelRaceFunc - tmp");
 for (int i=0;i<db->ilosc[2];i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in DelRaceFunc - tmp[]");
     strcpy(tmp[i], db->name[1][i]);
    }
 Button** btn = new Button*[2];
 btn[0] = new Button;
 btn[1] = new Button;
 if (!btn[0]) panic("memory exception in PostRaceFunc - Button* base_btn[0]");
 if (!btn[1]) panic("memory exception in PostRaceFunc - Button* base_btn[0]");
 btn[0]->SetBtnName("Wlasna prof");
 btn[1]->SetBtnName("Edytuj baze");
 btn[0]->SetBtnFunc(OwnProfFunc);
 btn[1]->SetBtnFunc(AccessProfsFunc);
 btn[0]->SetBtnSize(100, 24);
 btn[1]->SetBtnSize(100, 24);
 btn[0]->SetBtnPos(175, 500);
 btn[1]->SetBtnPos(625, 500);
 char* nam = wnd.CreateExCheckBoxVert("Wybierz profesje", tmp, db->ilosc[2], 60, 900, 600, 2, btn);
 if (!strcmp(nam,"Wlasna prof")) return;
 if (!strcmp(nam,"Edytuj baze")) goto again;
 int temp = -1;
 for (int i=0;i<db->ilosc[2];i++)
    {
     if (!strcmp(db->name[1][i], nam)) { temp = i; break; }
    }
 if (temp == -1) return ;
 strcpy(post->dane[PROFESJA],db->name[1][temp]);
 for (int i=0;i<post->ilosc_cech;i++)
        {
         post->cecha[i] += db->mod[1][temp][i];
        }
 FFastSave(wnd, NULL);
}


void AboutSavePicture(Wnd& wnd, void* ptr)
{
 tout("AboutSavePicture");
 int ok = wnd.CreateYesNoChild("Zapisac obrazek postaci?");
 if (!ok)
   {
    wnd.DoQuit();
    return;
   }
 FILE* plik = NULL;
 char fname[255];
 strcpy(fname, post->pname);
 strcat(fname, ".x");
 if ((plik = fopen(fname,"w")) == NULL)
   {
    wnd.CreateMBoxChild("nie moge zapisac pliku!");
    return ;
   }
 unsigned char r, g, b;
 for (int i=0;i<wnd.RetCx();i++)
 for (int j=0;j<wnd.RetCy();j++)
   {
    r = ReturnRed(wnd.RetWndPixel(i,j));
    g = ReturnGreen(wnd.RetWndPixel(i,j));
    b = ReturnBlue(wnd.RetWndPixel(i,j));
    fprintf(plik,"%c%c%c", (int)r,(int)g,(int)b);
   }
 if (plik) fclose(plik);
 plik = NULL;
 char bzcmd[255];
 sprintf(bzcmd,"bzip2 -9 %s; mv %s.bz2 %s", fname, fname, fname);
 system(bzcmd);
 wnd.DoQuit();
}


int LoadCharPicture(Wnd& wnd, void* ptr, bool display = true)
{
 tout("LoadCharPicture from BZ2");
 FILE* plik = NULL;
 char fname[255];
 strcpy(fname, post->pname);
 strcat(fname, ".x");
 if ((plik = fopen(fname,"r")) == NULL)
   {
    if (display) wnd.CreateMBoxChild("Brak pliku grafiki :(");
    return 0;
   }
 if (plik) fclose(plik);
 plik = NULL;
 char bzcmd[255];
 sprintf(bzcmd,"mv %s %s.bz2; bunzip2 %s.bz2", fname, fname, fname);
 system(bzcmd);
 if ((plik = fopen(fname,"r")) == NULL)
   {
    wnd.CreateMBoxChild("Blad przy dekompresji pliku");
    return 0;
   }
 unsigned char r, g, b;
 for (int i=0;i<wnd.RetCx();i++)
 for (int j=0;j<wnd.RetCy();j++)
   {
    fscanf(plik,"%c%c%c", &r, &g, &b);
    wnd.SetWndPixel(RGB((int)r, (int)g, (int)b), i, j);
   }
 //if (plik) fclose(plik);
 //plik = NULL;
 sprintf(bzcmd,"bzip2 -9 %s; mv %s.bz2 %s", fname, fname, fname);
 system(bzcmd);
 return 1;
}


void PaletteFunc(Wnd& wnd, void* ptr)
{
 tout("PaletteFunc");
 ulong col = wnd.CreatePaletteChild();
 wnd.SetCurColor(col);
}


void DrawPost(Wnd& wnd, void* ptr)
{
  tout("DrawPost");
  Wnd* w = new Wnd(wnd.RetDspStr(), 0x80, 0x100);
  if (!w) panic("memory_exception_in_new",__LINE__,__FILE__);
  w->CreateGC();
  w->SetFont("*-helvetica-*-12-*");
  w->SetCaption("Narysuj postac!");
  w->DeclareResponseTable(MOST_RESPONSES);
  w->SelectConfigureFunc(NoEDenyResizeFunc);
  w->SelectKeyPressFunc(eXtendedKeyPressQ);
  w->SelectExposeFunc(NoWDrawExposeFunc);
  w->SelectKeyReleaseFunc(eXtendedKeyRelease);
  w->SelectButtonPressFunc(eXtendedButtonPress);
  w->SelectButtonReleaseFunc(eXtendedButtonRelease);
  w->SelectMotionNotifyFunc(DrawLinesOnWindowFunc);
  w->SetButtonsNumber(3);
  w->EnableFrames(0);
  w->EnableGraph(1);
  LoadCharPicture(*w,NULL, false);
  StdExposeFunc(*w, NULL);
  w->CreateButton(0, "O" ,5  , 230, StdExposeFunc, true, 36,25);//ok
  w->CreateButton(1, "X" ,45 , 230, AboutSavePicture, false, 36,25);//ok
  w->CreateButton(2, "P" ,85  , 230, PaletteFunc, false, 36,25);//ok
  w->DefaultResponse();
  wnd.Invalidate();
}


void DisplayCharPict(Wnd& wnd, void* ptr)
{
  tout("DisplayCharPict");
  Wnd* w = new Wnd(wnd.RetDspStr(), 0x80, 0x100);
  if (!w) panic("memory_exception_in_new",__LINE__,__FILE__);
  w->CreateGC();
  w->SetFont("*-helvetica-*-12-*");
  w->SetCaption("Narysuj postac!");
  w->DeclareResponseTable(MEDI_RESPONSES);
  w->SelectConfigureFunc(NoEDenyResizeFunc);
  w->SelectKeyPressFunc(eXtendedKeyPressQ);
  w->SelectExposeFunc(NoWDrawExposeFunc);
  w->SelectKeyReleaseFunc(eXtendedKeyRelease);
  w->SelectButtonPressFunc(eXtendedButtonPress);
  w->SelectButtonReleaseFunc(eXtendedButtonRelease);
  w->EnableFrames(0);
  w->EnableGraph(1);
  int ok = LoadCharPicture(*w,NULL);
  if (!ok)
    {
     w->DoQuit();
     w->DefaultResponse();
     wnd.Invalidate();
     return;
    }
  w->SetButtonsNumber(2);
  w->CreateButton(0, "ok" ,45 , 230, StdOKFunc, true, 36,25);
  w->CreateButton(1, "ex" ,85 , 230, StdExposeFunc, false, 36,25);
  StdExposeFunc(*w, NULL);
  w->DefaultResponse();
  wnd.Invalidate();
}


void PostDataFunc(Wnd& wnd, void* far_ptr)
{
 tout("PostDataFunc bridged by void*");
 int dat = (int)far_ptr;  //Now I have (void*) "by class passed" (int)
 strcpy(post->dane[dat], wnd.CreateWriteBox("Wprowadz dane:"));  //this is the fuckin' easy method
}


void PostWoman(Wnd& w, void* for_the_glory_of_UNIX)
{
 tout("PostWoman (*)(*)");
 if (!strcmp(post->dane[PLEC],"kobieta"))
  {
   w.CreateMBoxChild("Ta postac jest JUZ kobieta!");
   w.DoQuit();
   w.Invalidate();
   return ;
  }
 strcpy(post->dane[PLEC],"kobieta");
 for (int i=0;i<post->ilosc_cech;i++) post->cecha[i] += db->p_mod[i];
 w.DoQuit();
 w.Invalidate();
}


void UndoWoman()
{
 tout("UndoWoman :(");
 for (int i=0;i<post->ilosc_cech;i++) post->cecha[i] -= db->p_mod[i];
}


void PostMan(Wnd& w, void* for_the_glory_of_UNIX)
{
 tout("PostMan");
 if (!strcmp(post->dane[PLEC],"mezczyzna"))
  {
   w.CreateMBoxChild("Ta postac jest JUZ mezczyzna!");
   w.DoQuit();
   w.Invalidate();
   return ;
  }
 if (!strcmp(post->dane[PLEC],"kobieta")) UndoWoman();
 strcpy(post->dane[PLEC],"mezczyzna");
 w.DoQuit();
 w.Invalidate();
}


void PostGenderFunc(Wnd& wnd, void* ptr)
{
 tout("PostGenderFunc");
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 110, 65);
 if (!wybor) panic("memory exception in creating win_child!");
 wybor->WinDefaults("Plec (*)(*)");
 wybor->SetButtonsNumber(2);
 wybor->CreateButton(0, "Mezczyzna" ,5  , 5, PostMan, false, 100);
 wybor->CreateButton(1, "Kobieta" ,2 , 35, PostWoman, true, 100);
 wybor->SetParent(&wnd);
 wybor->DefaultResponse();
 FastSave(wnd);
 wnd.Invalidate();
}


void MainDataFunc(Wnd& wnd, void* ptr)
{
 tout("MainDataFunc");
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 400, 300);
 if (!wybor) panic("memory exception in creating win_child!");
 wybor->WinDefaults("Podaj glowne dane");
 wybor->SetButtonsNumber(11);
 wybor->CreateButton(0, "Imie" ,50  , 50, PostDataFunc, true, 100, 25, (void*)(IMIE));
 wybor->CreateButton(1, "Nazwisko" ,250 , 50, PostDataFunc, false, 100, 25, (void*)(NAZWISKO));
 wybor->CreateButton(2, "Charakter" ,50  , 80, PostDataFunc, false, 100, 25, (void*)(CHARAKTER));
 wybor->CreateButton(3, "Oczy" ,250 , 80, PostDataFunc, false, 100, 25, (void*)(OCZY));
 wybor->CreateButton(4, "Wlosy" ,50  , 110, PostDataFunc, false, 100, 25, (void*)(WLOSY));
 wybor->CreateButton(5, "Skora" ,250 , 110, PostDataFunc, false, 100, 25, (void*)(CERA));
 wybor->CreateButton(6, "Wzrost" ,50  , 140, PostDataFunc, false, 100, 25, (void*)(WZROST));
 wybor->CreateButton(7, "Waga" ,250 , 140, PostDataFunc, false, 100, 25, (void*)(WAGA));
 wybor->CreateButton(8, "Wiek" ,50  , 170, PostDataFunc, false, 100, 25, (void*)(WIEK));
 wybor->CreateButton(9, "Plec" ,250 , 170, PostGenderFunc, false, 100);
 wybor->CreateButton(10, "Powrot" ,150 , 200, StdOKFunc, false, 100);
 wybor->SetParent(&wnd);
 wybor->DefaultResponse();
 FastSave(wnd);
 wnd.Invalidate();
}


void RollAttributes(Wnd& wnd, void* far_ptr)
{
 tout("RollAttributes");
 int pl = 2*post->st;
 int* temp = (int*)far_ptr;
 if (!temp) return;
 wnd.BlackWings();
 for (int i=0;i<post->ilosc_cech;i++) temp[i] = post->cecha[i] + random(2*pl+1)-pl;
 char* tmp = new char[2048];
 if (!tmp) panic("memory_exception_in_new",__LINE__,__FILE__);
 char tmp2[100];
 sprintf(tmp,"Zdefiniowanych cech: %d\n\n", db->ilosc[0]);
 for (int i=0;i<post->ilosc_cech;i++)
   {
    sprintf(tmp2,"(%-3d)%-25s: %d\n", i, db->name_att[i], temp[i]);
    strcat(tmp, tmp2);
   }
 wnd.DeleteWndText();
 wnd.PrepareWndText(tmp);
 wnd.StdTextWrite();
}


void ApplyAttributesFunc(Wnd& wnd, void* far_ptr)
{
 tout("ApplyAttributesFunc");
 int* temp = (int*)far_ptr;
 if (!temp) return;
 for (int i=0;i<post->ilosc_cech;i++) post->cecha[i] = temp[i];
 wnd.DoQuit();
}


void RandomAttFunc(Wnd& wnd, void* ptr)
{
 tout("RandomAttFunc");
 if (post->randomedAtt)
   {
    int ok =wnd.CreateYesNoChild("Ta postac maila juz losowane cechy!\nKontynuowac?");
    if (!ok) return;
    ok = CheckPassword(db, wnd);
    if (!ok) return;
   }
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 400, 500);
 int* temp = new int[MAX_ATT];
 if (!temp) panic("memory exception in creating win_child!");
 if (!wybor) panic("memory exception in creating win_child!");
 wybor->WinDefaults("Losowanie cech");
 wybor->EnableFrames(0);
 wybor->SetButtonsNumber(2);
 RollAttributes(*wybor, (void*)temp);
 wybor->CreateButton(0, "Losuj" ,50  , 450, RollAttributes, true, 100, 24, (void*)temp);
 wybor->CreateButton(1, "OK" ,250 , 450, ApplyAttributesFunc, false, 100, 24, (void*)temp);
 wybor->SetParent(&wnd);
 wybor->DefaultResponse();
 post->randomedAtt = 1;
 FastSave(wnd);
 wnd.Invalidate();
 if (temp) delete [] temp;
}


int countPL(int* table, int len)
{
 tout("countPL");
 int ret = 0;
 for (int i=0;i<len;i++)
   {
    if (table[i] > 0) ret += table[i];
   }
 return ret;
}


int countMI(int* table, int len)
{
 tout("countMI");
 int ret = 0;
 for (int i=0;i<len;i++)
   {
    if (table[i] < 0) ret -= table[i];
   }
 return ret;
}


void PMAttFunc(Wnd& wnd, void* far_ptr)
{
 tout("PMAttFunc");
 int* temp = (int*)far_ptr;
 int pl = countPL(temp, post->ilosc_cech);
 int mi = countMI(temp, post->ilosc_cech);
 char cname[STRING];
 char str[STRING];
 if (pl < temp[post->ilosc_cech])
   {
    temp[wnd.RetPressed()] += db->st;
    if (temp[wnd.RetPressed()] > 2)
      {
       temp[wnd.RetPressed()] = 2;
       wnd.CreateMBoxChild("Za duzo plusow do jednej cechy!\n");
       return;
      }
    sprintf(str,"Pozostalo plusow: %d", temp[post->ilosc_cech] - pl-1);
    wnd.BlackWings();
    wnd.PrepareWndText(str);
    wnd.StdTextWrite();
    for (int i=0;i<post->ilosc_cech;i++)
      {
       Button* btn = wnd.RetBtn(i);
       sprintf(cname,"(%d)%s: %d+%d", i, db->name_att[i], post->cecha[i], temp[i]);
       btn->SetBtnName(cname);
      }
    return;
   }
 if ((pl == temp[post->ilosc_cech]) && (mi < temp[post->ilosc_cech]))
   {
    if (temp[wnd.RetPressed()] > 0)
      {
       wnd.CreateMBoxChild("Te ceche zwiekszales!\nNie mozesz jej teraz zmniejszyc!");
       return;
      }
    temp[wnd.RetPressed()] -= db->st;
    if (temp[wnd.RetPressed()] < -2)
      {
       temp[wnd.RetPressed()] = -2;
       wnd.CreateMBoxChild("Za duzo minusow do jednej cechy!\n");
       return;
      }
    mi++;
    sprintf(str,"Pozostalo minusow: %d", temp[post->ilosc_cech] - mi);
    wnd.BlackWings();
    wnd.PrepareWndText(str);
    wnd.StdTextWrite();
    for (int i=0;i<post->ilosc_cech;i++)
      {
       Button* btn = wnd.RetBtn(i);
       sprintf(cname,"(%d)%s: %d+%d", i, db->name_att[i], post->cecha[i], temp[i]);
       btn->SetBtnName(cname);
      }
     if ((pl == temp[post->ilosc_cech]) && (mi == temp[post->ilosc_cech]))
        {
         for (int i=0;i<post->ilosc_cech;i++) post->cecha[i] += temp[i];
         wnd.DoQuit();
         return ;
        }
   }
 wnd.BlackWings();
 wnd.Invalidate();
}


void ModAttFunc(Wnd& wnd, void* far_ptr)
{
 tout("ModAttFunc");
 int val = (int)far_ptr;
 char str[STRING];
 char cname[STRING];
 sprintf(str,"Pozostalo plusow: %d", val);
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 400, 500);
 int* temp = new int[post->ilosc_cech+1];
 if (!temp) panic("memory exception in creating win_child!");
 for (int i=0;i<post->ilosc_cech;i++) temp[i] = 0;
 temp[post->ilosc_cech] = val;
 if (!wybor) panic("memory exception in creating win_child!");
 wybor->WinDefaults("Modyfikuj cechy...");
 wybor->PrepareWndText(str);
 wybor->StdTextWrite();
 wybor->EnableFrames(0);
 wybor->SetButtonsNumber(post->ilosc_cech);
 for (int i=0;i<post->ilosc_cech;i++)
   {
    sprintf(cname,"(%d)%s: %d+%d", i, db->name_att[i], post->cecha[i], temp[i]);
    if (!i) wybor->CreateButton(0, cname ,120  , 80, PMAttFunc, true, 160, 24, (void*)temp);
    else wybor->CreateButton(i, cname ,120  , 80+i*30, PMAttFunc, false, 160, 24, (void*)temp);
   }
 wybor->SetParent(&wnd);
 wybor->DefaultResponse();
 wnd.DoQuit();
 if (temp) delete [] temp;
}


void CancelFunc(Wnd& wnd, void* ptr)
{
 tout("CancelFunc");
 post->modAtt = 0;
 wnd.DoQuit();
}


void PostModFunc(Wnd& wnd, void* for_the_glory_of_UNIX)
{
 tout("PostModFunc");
 if (post->modAtt)
   {
    int ok =wnd.CreateYesNoChild("Ta postac maila juz modyfikowane cechy!\nKontynuowac?");
    if (!ok) return;
    ok = CheckPassword(db, wnd);
    if (!ok) return;
   }
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 400, 300);
 if (!wybor) panic("memory exception in creating win_child!");
 wybor->WinDefaults("Modyfikatory...");
 post->modAtt = 1;
 wybor->EnableFrames(0);
 wybor->SetButtonsNumber(4);
 wybor->CreateButton(0, "+/-1" ,50  , 150, ModAttFunc, true, 100, 24, (void*)(1));
 wybor->CreateButton(1, "+/-2" ,250  , 150, ModAttFunc, false, 100, 24, (void*)(2));
 wybor->CreateButton(2, "+/-3" ,50  , 180, ModAttFunc, false, 100, 24, (void*)(3));
 wybor->CreateButton(3, "Powrot" ,250 , 180, CancelFunc, false, 100);
 wybor->SetParent(&wnd);
 wybor->DefaultResponse();
 FastSave(wnd);
 wnd.Invalidate();
}


void OwnSkillFunc(Wnd& wnd, void* far_ptr)
{
 tout("OwnSkillFunc");
 wnd.SetUnusedCharPtr(wnd.RetSelectedButtonName());
 int* lum = (int*)far_ptr;
 if (!lum) return;
 int poz=0;
 char* skiname = wnd.CreateWriteBox("Podaj nazwe umiejetnosci:");
 char* nam = wnd.CreateWriteBox("Podaj poziom:");
 sscanf(nam,"%d", &poz);
 if (poz < 0) return;
 int trd=1;
 nam = wnd.CreateWriteBox("Podaj trudnosc:");
 sscanf(nam,"%d", &trd);
 if (trd < 0) return;
 int ipoz;
 ipoz = poz + trd -1;
 if (ipoz < 0) return;
 if (ipoz >= db->ilosc[5]) return;
 int cost = db->cost[ipoz];
 if (*lum<cost)
   {
    wnd.CreateMBoxChild("Za malo punktow!");
    return;
   }
 int current = post->il_um;
 post->pozum[current] = poz;
 post->trum[current]  = trd;
 char ** tmp;
 char tmp2[120];
 tmp = new char*[post->ilosc_cech+1];
 if (!tmp) panic("memory exception in SetNamesFunc - tmp");
 for (int i=0;i<post->ilosc_cech;i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in SetNamesFunc - tmp[]");
     sprintf(tmp2, "(%d)%s", i, post->n_cechy[i]);
     strcpy(tmp[i], tmp2);
    }
 nam = wnd.CreateCheckBoxChild("Wybierz ceche testujaca", tmp, post->ilosc_cech);
 int ce=0;
 sscanf(nam,"(%d)", &ce);
 post->ceum[current]  = ce;
 int mul=3;
 nam = wnd.CreateWriteBox("Podaj mnoznik:");
 sscanf(nam,"%d", &mul);
 if (mul < 0) return;
 if (mul > 10) return;
 post->mulum[current] = mul;
 int mod=0;
 nam = wnd.CreateWriteBox("Podaj modyfikator testu:");
 sscanf(nam,"%d", &mod);
 if (mod < -50) return;
 if (mul > 50) return;
 post->modum[current] = mod;
 int mag=0;
 nam = wnd.CreateWriteBox("Podaj poziom magii:");
 sscanf(nam,"%d", &mag);
 if (mag < -10) return;
 if (mag > 10) return;
 post->magum[current] = mag;
 post->tesum[current] = (post->cecha[post->ceum[current]]/2) + (post->mulum[current]*post->pozum[current])+ post->modum[current];
 strcpy(post->um[current], skiname);
 post->il_um++;
 *lum -= cost;
 char ttt[STRING];
 sprintf(ttt,"Punktow umiejetnosci: %d", *lum);
 wnd.BlackWings();
 wnd.PrepareWndText(ttt);
 wnd.StdTextWrite();
 wnd.DoQuit();
 RecountAll();
}


void PostAddSkillFunc(Wnd& wnd, void* far_ptr)
{
 tout("PostAddSkillFunc");
 int * lum = (int*)far_ptr;
 if (!lum) return;
 if (post->il_um == MAX_UM)
   {
    wnd.CreateMBoxChild("Masz juz maksimum umiejetnosci");
    return;
   }
 if (db->ilosc[3] == 0)
   {
    wnd.CreateMBoxChild("Brak umiejetnosci w bazie danych!");
    return;
   }
 ASortSkillFunc(wnd, NULL);
 char ** tmp;
 char tmp2[120];
 tmp = new char*[db->ilosc[3]+1];
 if (!tmp) panic("memory exception in SetNamesFunc - tmp");
 for (int i=0;i<db->ilosc[3];i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in SetNamesFunc - tmp[]");
     sprintf(tmp2, "(%d)%s", i, db->s_name[i]);
     strcpy(tmp[i], tmp2);
    }
 Button** btn = new Button*[1];
 if (!btn) panic("cannot create addictional_button in CreateExCheckBoxvert!\n");
 btn[0] = new Button;
 if (!btn[0]) panic("memory exception in PostRaceFunc - Button* base_btn[0]");
 btn[0]->SetBtnName("Wlasna um.");
 btn[0]->SetBtnFunc(OwnSkillFunc);
 btn[0]->SetBtnSize(100, 24);
 btn[0]->SetBtnPos(250, 500);
 btn[0]->SetVoid((void*)lum);
 char* nam = wnd.CreateExCheckBoxVert("Wybierz umiejetnosc:", tmp, db->ilosc[3], 60, 900, 600, 1, btn);
 if (!strcmp(nam,"Wlasna um.")) return;
 int num=0;
 sscanf(nam,"(%d)", &num);
 int poz=1;
 nam = wnd.CreateWriteBox("Podaj poziom:");
 sscanf(nam,"%d", &poz);
 if (poz < 0) return;
 int ipoz;
 ipoz = poz + db->s_mod[num][DIFF] -1;
 if (ipoz < 0) return;
 if (ipoz >= db->ilosc[5]) return;
 int cost = db->cost[ipoz];
 if (*lum<cost)
   {
    wnd.CreateMBoxChild("Za malo punktow!");
    return;
   }
 int current = post->il_um;
 *lum -= cost;
 post->pozum[current] = poz;
 post->trum[current]  = db->s_mod[num][DIFF];
 post->ceum[current]  = db->s_mod[num][ATTRIBUTE];
 post->mulum[current] = db->s_mod[num][MULTIPLE];
 post->modum[current] = db->s_mod[num][MOD];
 post->magum[current] = db->s_mod[num][MAGIC];
 post->tesum[current] = (post->cecha[post->ceum[current]]/2) + (post->mulum[current]*post->pozum[current])+ post->modum[current];
 strcpy(post->um[current], db->s_name[num]);
 post->il_um++;
 char ttt[STRING];
 sprintf(ttt,"Punktow umiejetnosci: %d", *lum);
 RecountAll();
 wnd.BlackWings();
 wnd.PrepareWndText(ttt);
 wnd.StdTextWrite();
 RecountAll();
}


void PostAddSkillCFunc(Wnd& wnd, void* far_ptr)
{
 tout("PostAddSkillCFunc");
 int * lum = (int*)far_ptr;
 if (!lum) return;
 if (post->il_um == MAX_UM)
   {
    wnd.CreateMBoxChild("Masz juz maksimum umiejetnosci");
    return;
   }
 if (db->ilosc[3] == 0)
   {
    wnd.CreateMBoxChild("Brak umiejetnosci w bazie danych!");
    return;
   }
 ASortSkillFunc(wnd, NULL);
 char ** tmp;
 tmp = new char*[db->ilosc[4]+1];
 if (!tmp) panic("memory exception in DelRaceFunc - tmp");
 for (int i=0;i<db->ilosc[4];i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in DelRaceFunc - tmp[]");
     strcpy(tmp[i], db->c_name[i]);
    }
 char* nam = wnd.CreateCheckBoxChild("Wybierz kategorie", tmp, db->ilosc[4]);
 int temp = -1;
 for (int i=0;i<db->ilosc[4];i++)
    {
     if (!strcmp(db->c_name[i], nam)) { temp = i; break; }
    }
 if (temp == -1) return ;
 for (int i=1;i<=db->ilosc[4];i++) if (tmp[i]) delete [] tmp[i];
 if (tmp) delete [] tmp;
 tmp = new char*[db->ilosc[3]+1];
 if (!tmp) panic("memory exception in DelRaceFunc - tmp");
 Button** btn = new Button*[1];
 if (!btn) panic("cannot create addictional_button in CreateExCheckBoxvert!\n");
 btn[0] = new Button;
 if (!btn[0]) panic("memory exception in PostRaceFunc - Button* base_btn[0]");
 btn[0]->SetBtnName("Wlasna um.");
 btn[0]->SetBtnFunc(OwnSkillFunc);
 btn[0]->SetBtnSize(100, 24);
 btn[0]->SetBtnPos(250, 500);
 btn[0]->SetVoid((void*)lum);
 int x=0;
 char string[STRING];
 for (int i=0;i<db->ilosc[3];i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in DelRaceFunc - tmp[]");
     if (db->s_mod[i][CATEG] == temp)
       {
        sprintf(string,"(%d)%s", i, db->s_name[i]);
        strcpy(tmp[x], string);
	x++;
       }
    }
 if (!x)
   {
    wnd.CreateMBoxChild("Brak umiejetnosci w tej kategorii");
    return;
   }
 nam = wnd.CreateExCheckBoxVert("Wybierz umiejetnosc", tmp, x, 60, 900, 600, 1, btn);
 if (!strcmp(nam,"Wlasna um.")) return;
 int num=0;
 sscanf(nam,"(%d)", &num);
 int poz=1;
 nam = wnd.CreateWriteBox("Podaj poziom:");
 sscanf(nam,"%d", &poz);
 if (poz < 0) return;
 int ipoz;
 ipoz = poz + db->s_mod[num][DIFF] -1;
 if (ipoz < 0) return;
 if (ipoz >= db->ilosc[5]) return;
 int cost = db->cost[ipoz];
 if (*lum<cost)
   {
    wnd.CreateMBoxChild("Za malo punktow!");
    return;
   }
 int current = post->il_um;
 *lum -= cost;
 post->pozum[current] = poz;
 post->trum[current]  = db->s_mod[num][DIFF];
 post->ceum[current]  = db->s_mod[num][ATTRIBUTE];
 post->mulum[current] = db->s_mod[num][MULTIPLE];
 post->modum[current] = db->s_mod[num][MOD];
 post->magum[current] = db->s_mod[num][MAGIC];
 post->tesum[current] = (post->cecha[post->ceum[current]]/2) + (post->mulum[current]*post->pozum[current])+ post->modum[current];
 strcpy(post->um[current], db->s_name[num]);
 post->il_um++;
 char ttt[STRING];
 sprintf(ttt,"Punktow umiejetnosci: %d", *lum);
 wnd.BlackWings();
 wnd.PrepareWndText(ttt);
 wnd.StdTextWrite();
 RecountAll();
}


void PostDelSkillFunc(Wnd& wnd, void* far_ptr)
{
 tout("PostDelSkillFunc");
 int * lum = (int*)far_ptr;
 if (!lum) return;
 if (post->il_um == 0)
   {
    wnd.CreateMBoxChild("Brak umiejetnosci do usuniecia!");
    return;
   }
 char ** tmp;
 char tmp2[120];
 tmp = new char*[post->il_um+1];
 if (!tmp) panic("memory exception in SetNamesFunc - tmp");
 for (int i=0;i<post->il_um;i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in SetNamesFunc - tmp[]");
     sprintf(tmp2, "(%d)%s", i, post->um[i]);
     strcpy(tmp[i], tmp2);
    }
 char* nam = wnd.CreateCheckBoxChild("Wybierz umiejetnosc do usuniecia", tmp, post->il_um);
 int poz=0;
 sscanf(nam,"(%d)", &poz);
 int l = post->pozum[poz];
 int t = post->trum[poz];
 int ret = db->cost[l+t-1];
 post->il_um--;
 for (int i=poz;i<post->il_um;i++)
   {
    strcpy(post->um[i],post->um[i+1]);
    post->tesum[i] = post->tesum[i+1];
    post->pozum[i] = post->pozum[i+1];
    post->trum[i]  =  post->trum[i+1];
    post->ceum[i]  =  post->ceum[i+1];
    post->mulum[i] = post->mulum[i+1];
    post->p_u[i]   = post->p_u[i+1];
   }
 *lum += ret;
 char ttt[STRING];
 sprintf(ttt,"Punktow umiejetnosci: %d", *lum);
 wnd.BlackWings();
 wnd.PrepareWndText(ttt);
 wnd.StdTextWrite();
 //RecountAll();                                  / ???
}


void PostDelLastSkillFunc(Wnd& wnd, void* far_ptr)
{
 tout("PostDelLastSkillFunc");
 int * lum = (int*)far_ptr;
 if (!lum) return;
 if (post->il_um == 0)
   {
    wnd.CreateMBoxChild("Brak umiejetnosci do usuniecia!");
    return;
   }
 post->il_um --;
 int l = post->pozum[post->il_um];
 int t = post->trum[post->il_um];
 int ret = db->cost[l+t-1];
 *lum += ret;
 char ttt[STRING];
 sprintf(ttt,"Punktow umiejetnosci: %d", *lum);
 wnd.BlackWings();
 wnd.PrepareWndText(ttt);
 wnd.StdTextWrite();
 RecountAll();
}


void __IllegalFunc__(Wnd& wnd, ulong key_code, void* far_ptr)
{
 tout("__IllegalFunc__ !CHEATER!");
 if (!far_ptr) {tout("in __IllegalFunc__ far_ptr=(void*)NULL, aborted!"); return;}
 int key = TransformKey(key_code, 0);
 if (key == 'x')
   {
    int* lum = (int*)far_ptr;
    int code=0;
    char tmp[STRING];
    char* str = wnd.CreateWriteBox("Podaj SUPER_TAJNY KOD:", true);
    sscanf(str,"%d", &code);
    if (code != 0xdead) return;
    tout("SECURITY_CODE_PASSED ===>> modifying (int*)lum");
    str = wnd.CreateWriteBox("Dodac punktow:");
    code = 0;
    sscanf(str,"%d", &code);
    *lum += code;
    sprintf(tmp,"Punktow umiejetnosci: %d", *lum);
    wnd.BlackWings();
    wnd.PrepareWndText(tmp);
    wnd.StdTextWrite();
   }
 RecountAll();
}


void PostModSkillFunc(Wnd& wnd, void* far_ptr)
{
 tout("PostModSkillFunc");
 int * lum = (int*)far_ptr;
 if (!lum) return;
 if (post->il_um == 0)
   {
    wnd.CreateMBoxChild("Brak umiejetnosci do modyfikacji");
    return;
   }
 char ** tmp;
 char tmp2[120];
 tmp = new char*[post->il_um+1];
 if (!tmp) panic("memory exception in SetNamesFunc - tmp");
 for (int i=0;i<post->il_um;i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in SetNamesFunc - tmp[]");
     sprintf(tmp2, "(%d)%s", i, post->um[i]);
     strcpy(tmp[i], tmp2);
    }
 char* nam = wnd.CreateCheckBoxChild("Wybierz umiejetnosc do modyfikacji", tmp, post->il_um);
 int poz=0;
 sscanf(nam,"(%d)", &poz);
 int l = post->pozum[poz];
 int t = post->trum[poz];
 int ret = db->cost[l+t-1];
 char datta[STRING];
 sprintf(datta,"Stary poziom: %d, podaj nowy:", l);
 nam = wnd.CreateWriteBox(datta);
 int new_poz=1;
 sscanf(nam,"%d", &new_poz);
 if (new_poz < 0) return;
 if (new_poz+t-1 >= db->ilosc[5]) return;
 int new_cost = db->cost[new_poz+t-1];
 ret -= new_cost;
 if ((*lum + ret) < 0)
   {
    wnd.CreateMBoxChild("Za malo punktow!");
    return;
   }
 post->pozum[poz] = new_poz;
 post->tesum[poz] = (post->cecha[post->ceum[poz]]/2) + (post->mulum[poz]*post->pozum[poz])+ post->modum[poz];
 *lum += ret;
 char ttt[STRING];
 sprintf(ttt,"Punktow umiejetnosci: %d", *lum);
 wnd.BlackWings();
 wnd.PrepareWndText(ttt);
 wnd.StdTextWrite();
 RecountAll();
}


void PostSkillFunc(Wnd& wnd, void* pyr)
{
 tout("PostSkillFunc");
 if (post->modSkill)
  {
    int ok = wnd.CreateYesNoChild("Ta postac maila juz\ntworzone umiejetnosci!\nKontynuowac?");
    if (!ok) return;
    ok = CheckPassword(db, wnd);
    if (!ok) return;
  }
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 600, 280);
 if (!wybor) panic("memory exception in creating win_child!");
 int* lum = new int;
 if (!lum) panic("memory exception in PostSkillFunc::lum of int*");
 *lum = 40;
 char str[STRING];
 sprintf(str,"Punktow umiejetnosci: %d", *lum);
 wybor->WinDefaults("Wybor umiejetnosci");
 wybor->PrepareWndText(str);
 wybor->StdTextWrite();
 wybor->EnableFrames(0);
 wybor->SetButtonsNumber(10);
 wybor->SelectKeyPressXFunc(__IllegalFunc__);//ok
 wybor->SetEventData(EV_KPRESSX, (void*)lum);
 wybor->CreateButton(0, "Dodaj"          ,50  , 100, PostAddSkillFunc, true, 200, 24, (void*)lum);//ok
 wybor->CreateButton(1, "Usun"           ,350 , 100, PostDelSkillFunc, false, 200, 24, (void*)lum);//ok
 wybor->CreateButton(2, "Usun ostatnia"  ,50  , 130, PostDelLastSkillFunc, false, 200, 24, (void*)lum);//ok
 wybor->CreateButton(3, "Karta postaci"  ,350 , 130, CharacterCard, false, 200);//ok
 wybor->CreateButton(4, "Koszty poziomow",50  , 160, ShowLevPUFunc, false, 200);//ok
 wybor->CreateButton(5, "Edytuj baze"    ,350 , 160, AccessSkillsFunc, false, 200);//ok
 wybor->CreateButton(6, "Modyfukuj"      ,50  , 190, PostModSkillFunc, false, 200, 24, (void*)lum);//ok
 wybor->CreateButton(7, "Dodaj w.g kateg.",350, 190, PostAddSkillCFunc, false, 200, 24, (void*)lum);//ok
 wybor->CreateButton(8, "Zapisz postac"   ,50 , 220, FFastSave, false, 200);//ok
 wybor->CreateButton(9, "Powrot"         ,350 , 220, StdOKFunc, false, 200);//ok
 wybor->SetParent(&wnd);
 wybor->DefaultResponse();
 post->modSkill = 1;
 FastSave(wnd);
 wnd.Invalidate();
}


void ChangePostPass(Wnd& wnd, void* ptr)
{
 tout("ChangepostPass");
 char* new_pass = wnd.CreateWriteBox("Podaj nowy kod\n0 -brak kodu\n-1 -odmowa dostepu", true);
 int val=0;
 if (!new_pass) return ;
 sscanf(new_pass,"%d", &val);
 post->sv = val;
 FastSave(wnd);
 //wnd.Invalidate();
}


void Alternative(RPG* p, FILE* f)
{
 if (!f) return;
 tout("Alternative <<writing HTML>>");
 fprintf(f,"<center>\n");
 fprintf(f,"Character level: %d EXP points: %d</center><br>\n",p->lev,p->lev_p);
 fprintf(f,"<center>\n");
 fprintf(f,"<table border = \"3\">\n");
 fprintf(f,"<tr>\n");
 for (int i=0;i<10;i++) DecompressData(p->dane[i]);
 fprintf(f,"<td>Name: %s\n",p->dane[IMIE]);
 fprintf(f,"<td>Surname: %s\n",p->dane[NAZWISKO]);
 fprintf(f,"<td>Race: %s\n",p->dane[RASA]);
 fprintf(f,"<td>Profession: %s\n",p->dane[PROFESJA]);
 fprintf(f,"<tr>\n");
 fprintf(f,"<td>Height: %s\n",p->dane[WZROST]);
 fprintf(f,"<td>Weight: %s\n",p->dane[WAGA]);
 fprintf(f,"<td>Hair: %s\n",p->dane[WLOSY]);
 fprintf(f,"<td>Eyes: %s\n",p->dane[OCZY]);
 fprintf(f,"</table>\n");
 fprintf(f,"<table border = \"3\">\n");
 fprintf(f,"<tr>\n");
 fprintf(f,"<td>Character: %s\n",p->dane[CHARAKTER]);
 fprintf(f,"<td>Skin: %s\n",p->dane[CERA]);
 fprintf(f,"</table>\n");
 int max = (p->ilosc_cech > p->il_um) ? p->ilosc_cech : p->il_um;
 fprintf(f,"<table border =\"3\">\n");
 fprintf(f,"<tr>\n");
 fprintf(f,"<td>Pl\n");
 fprintf(f,"<td>Attrib\n");
 fprintf(f,"<td>value\n");
 fprintf(f,"<td>Pl\n");
 fprintf(f,"<td>Diff\n");
 fprintf(f,"<td>Skill\n");
 fprintf(f,"<td>Level\n");
 fprintf(f,"<td>Test\n");
 for (int i=0;i<max;i++)
        {
         fprintf(f,"<tr>\n");
         if (i<p->ilosc_cech)
                {
                 DecompressData(p->n_cechy[i]);
                 fprintf(f,"<td>[%d]\n",p->p_c[i]);
                 fprintf(f,"<td>%s\n",p->n_cechy[i]);
                 fprintf(f,"<td>%d\n",p->cecha[i]);
                }
         else
                {
                 fprintf(f,"<td>-\n");
                 fprintf(f,"<td>-\n");
                 fprintf(f,"<td>-\n");
                }
         if (i<p->il_um)
                {
                 DecompressData(p->um[i]);
                 fprintf(f,"<td>[%d]\n",p->p_u[i]);
                 fprintf(f,"<td>(%d)\n",p->trum[i]);
                 fprintf(f,"<td>%s\n",p->um[i]);
                 fprintf(f,"<td>%d\n",p->pozum[i]);
                 fprintf(f,"<td>%d\n",p->tesum[i]);
                }
         else
                {
                 fprintf(f,"<td>-\n");
                 fprintf(f,"<td>-\n");
                 fprintf(f,"<td>-\n");
                 fprintf(f,"<td>-\n");
                 fprintf(f,"<td>-\n");
                }
        }
 fprintf(f,"</table>\n");
 fprintf(f,"</center>\n");
 if (p->mage)
   {
    fprintf(f,"<center>Known Spells SpellPts / turn %d</center>\n",p->spt);
    fprintf(f,"<center><br>\n<table border =\"3\">\n");
    fprintf(f,"<tr>\n");
    fprintf(f,"<td>Spell");
    fprintf(f,"<td>Level");
    fprintf(f,"<td>Spell Cost");
    for (int i=0;i<p->il_spell;i++)
    	{
       fprintf(f,"<tr>\n");
       fprintf(f,"<td>%s", p->spell[i]);
       fprintf(f,"<td>%d", p->spell_level[i]);
       fprintf(f,"<td>%d", p->spell_cost[i]);
      }
    fprintf(f,"</table>\n");
    fprintf(f,"</center>\n");
   }
 fprintf(f,"<center><big><u>Equip:</u></big></big></center>\n");
 fprintf(f,"<pre>\n");
 if (p->il_ekw == 0) fprintf(f,"No equip\n");
 for (int i=0;i<p->il_ekw;i++)
        {
         DecompressData(p->ekw[i]);
         fprintf(f,"%s\n",p->ekw[i]);
        }
 fprintf(f,"\n From v4.1 Cash = %d\n",p->cash);
 fprintf(f,"</pre>\n");
}


void WriteHTML(RPG* post, FILE* plik,char* fn)
{
 if (!plik) return;
 tout("WriteHTML");
 fprintf(plik,"<html>\n");
 fprintf(plik,"<title>\n");
 fprintf(plik,"Charater Card %s %s  XP %d Gender: %s Age: %s\n",post->dane[IMIE],post->dane[NAZWISKO],post->PD, post->dane[PLEC],post->dane[WIEK]);
 fprintf(plik,"</title>\n");
 fprintf(plik,"<head>\n");
 fprintf(plik,"</head>\n");
 fprintf(plik,"<body>\n");
 fprintf(plik,"<center><big>Character %s %s  XP %d Gender: %s Age: %s</big></center>\n",post->dane[IMIE],post->dane[NAZWISKO],post->PD, post->dane[PLEC],post->dane[WIEK]);
 fprintf(plik,"</body>\n");
 Alternative(post,plik);
 fprintf(plik,"<pre>\n\nUsing Data Base %s</pre>\n",fn);
 fprintf(plik,"</html>\n");
}


void PostGenHTMLFunc(Wnd& wnd, void* ptr)
{
 tout("PostGenHTMLFunc");
 char fn2[80];
 strcpy(fn2,post->pname);
 strcat(fn2,".html");
 FILE* plik = NULL;
 if ((plik = fopen(fn2,"w"))==NULL)
   {
    wnd.CreateMBoxChild("Nie moge zapisac\npliku HTML dla postaci :(");
    return;
   }
 else
   {
    WriteHTML(post,plik,db->dbname);
    if (plik) fclose(plik);
    plik = NULL;
   }
}


void AddEkwByHand(Wnd& wnd, void* far_ptr)
{
 tout("AddEkwByHand");
 if (post->il_ekw == MAX_EKW_P)
   {
    wnd.CreateMBoxChild("Masz juz maksymalna ilosc ekwipunku!");
    return;
   }
 char* nam = wnd.CreateWriteBox("Podaj nazwe przedmiotu");
 if (!strcmp(nam,"")) return;
 strcpy(post->ekw[post->il_ekw], nam);
 post->il_ekw++;
}


void AddEkwFromBase(Wnd& wnd, void* far_ptr)
{
 tout("AddEkwByHand");
 if (post->il_ekw == MAX_EKW_P)
   {
    wnd.CreateMBoxChild("Masz juz maksymalna ilosc ekwipunku!");
    return;
   }
 int* cash = (int*)far_ptr;tout("");
 if (!cash) return;
 char ** tmp;
 char tmp2[120];
 tmp = new char*[db->ilosc_ekw+1];
 if (!tmp) panic("memory exception in SetNamesFunc - tmp");
 for (int i=0;i<db->ilosc_ekw;i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in SetNamesFunc - tmp[]");
     sprintf(tmp2, "(%d)%s,$%d", i, db->ekw[i], db->c_ekw[i]);
     strcpy(tmp[i], tmp2);
    }
 char* nam = wnd.CreateCheckBoxVert("Wybierz przedmiot", tmp, db->ilosc_ekw);
 int num=0;
 sscanf(nam,"(%d)", &num);
 if (db->c_ekw[num] > *cash)
   {
    wnd.CreateMBoxChild("Masz za malo pieniedzy!");
    return;
   }
 *cash -= db->c_ekw[num];
 strcpy(post->ekw[post->il_ekw], db->ekw[num]);
 post->il_ekw++;
 char str[STRING];
 sprintf(str,"Kasy: %d", *cash);
 wnd.BlackWings();
 wnd.PrepareWndText(str);
 wnd.StdTextWrite();
}


void EditCashFunc(Wnd& wnd, void* far_ptr)
{
 tout("EditCashFunc");
 int* cash = (int*)far_ptr;
 if (!cash) return;
 int ok = CheckPassword(db, wnd);
 if (!ok) return;
 char* nam = wnd.CreateWriteBox("Nowa ilosc kasy: ");
 *cash=100;
 sscanf(nam,"%d",cash);
 char str[STRING];
 sprintf(str,"Kasy: %d", *cash);
 wnd.BlackWings();
 wnd.PrepareWndText(str);
 wnd.StdTextWrite();
}


void DeletePostEkwFunc(Wnd& wnd, void* far_ptr)
{
 tout("DeletePostEkwFunc");
 int* cash = (int*)far_ptr;
 if (!cash) return;
 if (post->il_ekw == 0)
   {
    wnd.CreateMBoxChild("nie ma nic do usuniecia.");
    return;
   }
 char ** tmp;
 char tmp2[120];
 tmp = new char*[post->il_ekw+1];
 if (!tmp) panic("memory exception in SetNamesFunc - tmp");
 for (int i=0;i<post->il_ekw;i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in SetNamesFunc - tmp[]");
     sprintf(tmp2, "(%d)%s", i, post->ekw[i]);
     strcpy(tmp[i], tmp2);
    }
 char* nam = wnd.CreateCheckBoxVert("Wybierz przedmiot do usuniecia", tmp, post->il_ekw);
 int num=0;
 sscanf(nam,"(%d)", &num);
 bool ret = false;
 for (int i=0;i<db->ilosc_ekw;i++)
   {
    if (!strcmp(db->ekw[i], post->ekw[num])) { *cash += db->c_ekw[i]; ret = true;}
   }
 if (!ret) wnd.CreateMBoxChild("Nie znalazlem\ntego przedmiotu\nw bazie\nnie zwroce za niego\npieniedzy!");
 post->il_ekw--;
 for (int i=num;i<post->il_ekw;i++) strcpy(post->ekw[i], post->ekw[i+1]);
 char str[STRING];
 sprintf(str,"Kasy: %d", *cash);
 wnd.BlackWings();
 wnd.PrepareWndText(str);
 wnd.StdTextWrite();
}


void DelPostLastEkw(Wnd& wnd, void* far_ptr)
{
 tout("DelPostLastEkwFunc");
 int* cash = (int*)far_ptr;
 if (!cash) return;
 if (post->il_ekw == 0)
   {
    wnd.CreateMBoxChild("nie ma nic do usuniecia.");
    return;
   }
 int num = post->il_ekw-1;
 bool ret = false;
 for (int i=0;i<db->ilosc_ekw;i++)
   {
    if (!strcmp(db->ekw[i], post->ekw[num])) { *cash += db->c_ekw[i]; ret = true;}
   }
 if (!ret) wnd.CreateMBoxChild("Nie znalazlem\ntego przedmiotu\nw bazie\nnie zwroce za niego\npieniedzy!");
 post->il_ekw--;
 for (int i=num;i<post->il_ekw;i++) strcpy(post->ekw[i], post->ekw[i+1]);
 char str[STRING];
 sprintf(str,"Kasy: %d", *cash);
 wnd.BlackWings();
 wnd.PrepareWndText(str);
 wnd.StdTextWrite();
}


void AddEkwLineFunc(Wnd& wnd, void* far_ptr)
{
 tout("AddEkwLineFunc");
 if (post->il_ekw == MAX_EKW_P)
   {
    wnd.DoQuit();
    return;
   }
 int* cash = (int*)far_ptr;
 if (!cash)
   {
    wnd.DoQuit();
    return;
   }
 char str[STRING];
 sprintf(str,"ilosc pieniedzy: $%d", *cash);
 char str2[STRING];
 sprintf(str2,"Dodac do ekwipunku linie?\n%s", str);
 int ok = wnd.CreateYesNoChild(str2);
 if (ok)
   {
    strcpy(post->ekw[post->il_ekw], str);
    post->il_ekw++;
   }
 wnd.DoQuit();
}


void PostEkwFunc(Wnd& wnd, void* far_ptr)
{
 tout("PostEkwFunc");
 int* test = (int*)far_ptr;
 int ok;
 if ((test) && (*test == YES_ID)) goto bypass;
 if (post->modEkw)
  {
    ok = wnd.CreateYesNoChild("Ta postac maila juz\ntworzony ekwipunek!\nKontynuowac?");
    if (!ok) return;
    ok = CheckPassword(db, wnd);
    if (!ok) return;
  }
 bypass:
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 600, 270);
 if (!wybor) panic("memory exception in creating win_child!");
 int *cash = new int;
 if (!cash) panic("memory exception in postekwfunc::(int*)cash");
 *cash = db->def_cash;
 char str[STRING];
 sprintf(str,"Kasy: %d", *cash);
 wybor->WinDefaults("Ekwipunek");
 wybor->PrepareWndText(str);
 wybor->StdTextWrite();
 wybor->SetButtonsNumber(9);
 wybor->CreateButton(0, "Dodaj recznie" ,50  , 100, AddEkwByHand, true, 200, 24);//ok
 wybor->CreateButton(1, "Dodaj z bazy"  ,350 , 100, AddEkwFromBase, false, 200, 24, (void*)cash);//ok
 wybor->CreateButton(2, "Usun"          ,50  , 130, DeletePostEkwFunc, false, 200, 24, (void*)cash);//ok
 wybor->CreateButton(3, "Usun ostatni"  ,350 , 130, DelPostLastEkw, false, 200, 24, (void*)cash);//ok
 wybor->CreateButton(4, "Karta postaci" ,50  , 160, CharacterCard, false, 200);//ok
 wybor->CreateButton(5, "Zapisz postac" ,350 , 160, FFastSave, false, 200);//ok
 wybor->CreateButton(6, "Powrot"        ,50  , 190, AddEkwLineFunc, false, 200, 24, (void*)cash);//ok
 wybor->CreateButton(7, "Pieniadze"     ,350 , 190, EditCashFunc, false, 200, 24, (void*)cash);//ok
 wybor->CreateButton(8, "Edytuj baze"   ,200 , 220, AccessItemsFunc, false, 200);//ok
 wybor->SetParent(&wnd);
 wybor->DefaultResponse();
 post->modEkw = 1;
 FastSave(wnd);
 wnd.Invalidate();
}


void Compute(Wnd& wnd, int& max_spell)
{
 tout("Compute");
 char tmp[STRING];
 int lev_rc = 0;
 int lev_me = 0;
 int lev_cer= 0;
 int op_mag = 0;
 for (int i=0;i<post->il_um;i++)
 	{
    strcpy(tmp, post->um[i]);
    downcase(tmp);
    if (!strcmp(tmp,"rzucanie czarow"))    lev_rc = post->pozum[i];
    if (!strcmp(tmp,"magia elementarna"))  lev_me = post->pozum[i];
    if (!strcmp(tmp,"ceremonie magiczne")) lev_cer = post->pozum[i];
    if (!strcmp(tmp,"operacje magiczne"))  op_mag = post->pozum[i];
   }
 post->sp = post->cecha[post->numMM];
 if (op_mag == 0) post->spt = post->sp / 4;
 if (op_mag == 1) post->spt = post->sp / 3;
 if (op_mag == 2) post->spt = post->sp / 2;
 if (op_mag == 3) post->spt = (post->sp*2) / 3;
 if (op_mag == 4) post->spt = (post->sp*3) / 4;
 if (op_mag == 5) post->spt = post->sp;
 if (op_mag == 6) post->spt = (post->sp*5) / 4;
 if (op_mag == 7) post->spt = (post->sp*4) / 3;
 if (op_mag == 8) post->spt = (post->sp*3) / 2;
 if (op_mag >= 9) post->spt = post->sp* 2;
 int max = (lev_rc > lev_me) ? lev_me : lev_rc;
 int t_max = max;
 if (lev_rc)
 {
 if (lev_cer == max) max += 2;
 else if (lev_cer == max-1) max++;
 }
 post->max_lev = max;
 max_spell = max*3;
 char dstr[PREDATE];
 max_spell *= 3;
 sprintf(dstr,"Policzylem\nMOC: %d\nMOC/Ture: %d\nMAX. poziom zaklecia: %d\nMAX. ilosc zaklec: %d\nZasoby: %d", post->sp, post->spt, post->max_lev, max_spell, t_max);
 wnd.CreateMBoxChild(dstr);
}


void PostAddSpellFunc(Wnd& wnd, void* far_ptr)
{
 tout("PostAddSpellFunc");
 if (post->il_spell == MAX_SP)
 	{
    wnd.CreateMBoxChild("Nie mozesz sie wiecej nauczyc!");
    return;
   }
 int* sp = (int*)far_ptr;
 if (!sp) return;
 int mlev = (db->ilosc_poziomow > post->max_lev) ? post->max_lev : db->ilosc_poziomow;
 if (!mlev)
   {
    wnd.CreateMBoxChild("nie mozesz wziac zaklecia!");
    return;
   }
 char ** tmp;
 char tmp2[120];
 tmp = new char*[mlev+1];
 if (!tmp) panic("memory exception in SetNamesFunc - tmp");
 for (int i=0;i<mlev;i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in SetNamesFunc - tmp[]");
     sprintf(tmp2, "poziom %d", i);
     strcpy(tmp[i], tmp2);
    }
 char* nam = wnd.CreateCheckBoxChild("Wybierz poziom", tmp, mlev);
 int poz=1;
 sscanf(nam,"poziom %d", &poz);
 if (!db->ilosc_czarow[poz])
   {
    wnd.CreateMBoxChild("Brak czarow na tym poziomie!");
    return;
   }
 bool v_table[MAX_SPELL];
 for (int i=0;i<MAX_SPELL;i++) v_table[i] = 0;
 for (int i=1;i<mlev;i++) if (tmp[i]) delete [] tmp[i];
 if (tmp) delete [] tmp;
 tmp = NULL;
 int have_mag = 0;
 tmp = new char*[db->ilosc_czarow[poz]+1];
 //int plus = post->max_lev;
 for (int i=0;i<db->ilosc_czarow[poz];i++)
   {
    have_mag = 0;
    for (int j=0;j<post->il_um;j++)
    	{
         if ((!strcmp(post->um[j],db->req_magic[poz][i])) && ((post->pozum[j]) >= poz)) have_mag = 1;
        }
    if (db->spell_cost[poz][i] > *sp) have_mag = 0;
    if (have_mag) sprintf(tmp2,"(%d)%s, PM:%d *", i, db->zaklecie[poz][i], db->spell_cost[poz][i]);
    else sprintf(tmp2,"(%d)%s, PM:%d -", i, db->zaklecie[poz][i], db->spell_cost[poz][i]);
    tmp[i] = new char[STRING];
    if (!tmp[i]) panic("memory exception in PostAddSpellFunc::(char*)tmp[]");
    strcpy(tmp[i], tmp2);
    v_table[i] = have_mag;
   }
 nam = wnd.CreateCheckBoxVert("Wybierz zaklecie (*) dostepne\n(-) niedostepne", tmp, db->ilosc_czarow[poz]);
 int num=0;
 sscanf(nam,"(%d)", &num);
 if (!v_table[num])
   {
    wnd.CreateMBoxChild("Nie masz odpowiednich umiejetnosci!");
    return;
   }
 if (*sp < (db->spell_cost[poz][num]+1))
   {
    wnd.CreateMBoxChild("Brak punktow!");
    return;
   }
 post->spell_level[post->il_spell] = poz;
 post->spell_cost[post->il_spell] = db->spell_cost[poz][num];
 strcpy(post->spell[post->il_spell], db->zaklecie[poz][num]);
 post->il_spell++;
 *sp -= (poz+1);
 char str[STRING];
 sprintf(str,"Pozostalo punktow: %d", *sp);
 wnd.BlackWings();
 wnd.PrepareWndText(str);
 wnd.StdTextWrite();
}


void PostDelSpellFunc(Wnd& wnd, void* far_ptr)
{
 tout("PostDelSpellFunc");
 if (post->il_spell == 0)
 	{
    wnd.CreateMBoxChild("Nie ma nic do usuniecia");
    return;
   }
 int* sp = (int*)far_ptr;
 if (!sp) wnd.CreateMBoxChild("SP_CANNOT_BE_USED\nI feekl panic...");
 char ** tmp;
 char tmp2[120];
 tmp = new char*[post->il_spell];
 if (!tmp) panic("memory exception in SetNamesFunc - tmp");
 for (int i=0;i<post->il_spell;i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in SetNamesFunc - tmp[]");
     sprintf(tmp2, "(%d)%s", i, post->spell[i]);
     strcpy(tmp[i], tmp2);
    }
 char* nam = wnd.CreateCheckBoxChild("Wybierz zaklecie do usuniecia", tmp, post->il_spell);
 int num=0;
 sscanf(nam,"(%d)", &num);
 if (sp) *sp += (post->spell_level[num]+1);
 post->il_spell--;
 for (int i=num;i<post->il_spell;i++)
   {
    strcpy(post->spell[i], post->spell[i+1]);
    post->spell_cost[i] = post->spell_cost[i+1];
    post->spell_level[i] = post->spell_level[i+1];
   }
 char str[STRING];
 if (sp)
   {
    sprintf(str,"Pozostalo punktow: %d", *sp);
    wnd.BlackWings();
    wnd.PrepareWndText(str);
    wnd.StdTextWrite();
   }
}


void AccessSpellsFunc(Wnd& wnd, void* ptr)
{
 tout("AccessSpellsFunc");
 int ok = CheckPassword(db, wnd);
 if (!ok) return;
 EditSpellsFunc(wnd, NULL);
}


void PostSpellFunc(Wnd& wnd, void* far_ptr)
{
 tout("PostSpellFunc");
 if (post->modSpell)
  {
    int ok = wnd.CreateYesNoChild("Ta postac maila juz\ntworzone zaklecia!\nKontynuowac?");
    if (!ok) return;
    ok = CheckPassword(db, wnd);
    if (!ok) return;
  }
 int max_spell;
 Compute(wnd, max_spell);
 if ((!post->max_lev) || (!max_spell))
   {
    wnd.CreateMBoxChild("System nie uznal cie za\nCzarodzieja, rozdawanie czarow\nprzerwane!");
    return;
   }
 post->mage = 1;
 int* sp = new int;
 if (!sp) panic("memory exception in PostSpellFunc::(int*)sp");
 *sp = max_spell;
 if (far_ptr)
   {
    sp = (int*)far_ptr;
   }
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 600, 200);
 if (!wybor) panic("memory exception in creating win_child!");
 wybor->WinDefaults("Zaklecia");
 char str[STRING];
 sprintf(str,"Pozostalo punktow: %d", *sp);
 wybor->PrepareWndText(str);
 wybor->StdTextWrite();
 wybor->SetButtonsNumber(6);
 wybor->CreateButton(0, "Dodaj zaklecie" ,50  , 100, PostAddSpellFunc, true, 200, 24, (void*)sp);//ok
 wybor->CreateButton(1, "Usun zaklecie"  ,350 , 100, PostDelSpellFunc, false, 200, 24, (void*)sp);
 wybor->CreateButton(2, "Karta postaci"  ,50  , 130, CharacterCard, false, 200);//ok
 wybor->CreateButton(3, "Powrot"         ,350 , 130, StdOKFunc, false, 200);//ok
 wybor->CreateButton(4, "Edytuj baze"    ,50 , 160, AccessSpellsFunc, false, 200);//ok
 wybor->CreateButton(5, "Zapisz postac"  ,350 , 160, FFastSave, false, 200);//ok
 wybor->SetParent(&wnd);
 wybor->DefaultResponse();
 post->modSpell = 1;
 FastSave(wnd);
 wnd.Invalidate();
}


void PostSpellPDFunc(Wnd& wnd, void* far_ptr)
{
 tout("PostSpellPDFunc");
 int max_spell;
 Compute(wnd, max_spell);
 if ((!post->max_lev) || (!max_spell))
   {
    int ok = wnd.CreateYesNoChild("Nie zostales uznany za czardzieja!\nChcesz nim zostac?");
    if (!ok) return;
    ok = CheckPassword(db, wnd);
    if (!ok) return;
   }
 post->mage = 1;
 int* sp = new int;
 if (!sp) panic("memory exception in PostSpellPDFunc::(int*)sp");
 *sp = max_spell;
 if (far_ptr)
   {
    sp = (int*)far_ptr;
   }
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 600, 200);
 if (!wybor) panic("memory exception in creating win_child!");
 wybor->WinDefaults("Zaklecia");
 char str[STRING];
 sprintf(str,"Pozostalo punktow: %d", *sp);
 wybor->PrepareWndText(str);
 wybor->StdTextWrite();
 wybor->SetButtonsNumber(6);
 wybor->CreateButton(0, "Dodaj zaklecie" ,50  , 100, PostAddSpellFunc, true, 200, 24, (void*)sp);//ok
 wybor->CreateButton(1, "Usun zaklecie"  ,350 , 100, PostDelSpellFunc, false, 200, 24, (void*)sp);
 wybor->CreateButton(2, "Karta postaci"  ,50  , 130, CharacterCard, false, 200);//ok
 wybor->CreateButton(3, "Powrot"         ,350 , 130, StdOKFunc, false, 200);//ok
 wybor->CreateButton(4, "Edytuj baze"    ,50 , 160, AccessSpellsFunc, false, 200);//ok
 wybor->CreateButton(5, "Zapisz postac"  ,350 , 160, FFastSave, false, 200);//ok
 wybor->SetParent(&wnd);
 wybor->DefaultResponse();
 FastSave(wnd);
 char tmp[STRING];
 sprintf(tmp,"Pozostalo punktow pd: %d", *sp);
 wnd.BlackWings();
 wnd.PrepareWndText(tmp);
 wnd.StdTextWrite();
 wnd.Invalidate();
}


void Character(Wnd& wnd, int exists = true)
{
 tout("Character");
 int ok = 1;
 if (exists) ok = LoadPost(wnd);
 if (!ok) return;
 if (!exists) DefaultValues();
 ok = FastSave(wnd);
 if (!ok) return ;
 RecountAll();
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 600, 400);
 if (!wybor) panic("memory exception in creating win_child!");
 wybor->WinDefaults("Tworzenie postaci...");
 wybor->PrepareWndText("wybierz opcje tworzenia:");
 wybor->StdTextWrite();
 SaradokMM();
 wybor->SetButtonsNumber(14);
 wybor->CreateButton(0, "Glowne"          ,50  , 100, MainDataFunc, true, 200);//ok
 wybor->CreateButton(1, "Rasa"            ,350 , 100, PostRaceFunc, false, 200);//ok
 wybor->CreateButton(2, "Profesja"        ,50  , 130, PostProfFunc, false, 200);//ok
 wybor->CreateButton(3, "Losowanie cech"  ,350 , 130, RandomAttFunc, false, 200);//ok
 wybor->CreateButton(4, "Modyfikatory"    ,50  , 160, PostModFunc, false, 200);//ok
 wybor->CreateButton(5, "Umiejetnosci"    ,350 , 160, PostSkillFunc, false, 200);//ok
 wybor->CreateButton(6, "Ekwipunek"       ,50  , 190, PostEkwFunc, false, 200);//ok
 wybor->CreateButton(7, "Zaklecia"        ,350 , 190, PostSpellFunc, false, 200);//ok
 wybor->CreateButton(8, "Kod dostepu"     ,50  , 220, ChangePostPass, false, 200);//ok
 wybor->CreateButton(9, "HTML"            ,350 , 220, PostGenHTMLFunc, false, 200);//ok
 wybor->CreateButton(10, "Rysunek"        ,50  , 250, DrawPost, false, 200);//ok
 wybor->CreateButton(11, "Zapisz postac"  ,350 , 250, FFastSave, false, 200);//ok
 wybor->CreateButton(12, "Karta postaci"  ,50  , 280, CharacterCard, false, 200);//ok
 wybor->CreateButton(13, "Powrot"         ,350 , 280, StdOKFunc, false, 200);//ok
 wybor->SetParent(&wnd);
 wybor->DefaultResponse();
 FastSave(wnd);
 PostGenHTMLFunc(wnd, NULL);
 wnd.Invalidate();
}


void OwnSkillPDFunc(Wnd& wnd, void* far_ptr)
{
 tout("OwnSkillFunc");
 wnd.SetUnusedCharPtr(wnd.RetSelectedButtonName());
 int* lum = (int*)far_ptr;
 if (!lum) return;
 int poz = 1;
 char* skiname = wnd.CreateWriteBox("Podaj nazwe umiejetnosci:");
 if (poz < 0) return;
 int trd=1;
 char* nam = wnd.CreateWriteBox("Podaj trudnosc:");
 sscanf(nam,"%d", &trd);
 if (trd < 0) return;
 int ipoz;
 ipoz = poz + trd -1;
 if (ipoz < 0) return;
 if (ipoz >= db->ilosc[5]) return;
 int cost = db->pd_cost[ipoz];
 if (*lum<cost)
   {
    wnd.CreateMBoxChild("Za malo punktow PD!");
    return;
   }
 int current = post->il_um;
 post->pozum[current] = poz;
 post->trum[current]  = trd;
 char ** tmp;
 char tmp2[120];
 tmp = new char*[post->ilosc_cech+1];
 if (!tmp) panic("memory exception in SetNamesFunc - tmp");
 for (int i=0;i<post->ilosc_cech;i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in SetNamesFunc - tmp[]");
     sprintf(tmp2, "(%d)%s", i, post->n_cechy[i]);
     strcpy(tmp[i], tmp2);
    }
 nam = wnd.CreateCheckBoxChild("Wybierz ceche testujaca", tmp, post->ilosc_cech);
 int ce=0;
 sscanf(nam,"(%d)", &ce);
 post->ceum[current]  = ce;
 int mul=3;
 nam = wnd.CreateWriteBox("Podaj mnoznik:");
 sscanf(nam,"%d", &mul);
 if (mul < 0) return;
 if (mul > 10) return;
 post->mulum[current] = mul;
 int mod=0;
 nam = wnd.CreateWriteBox("Podaj modyfikator testu:");
 sscanf(nam,"%d", &mod);
 if (mod < -50) return;
 if (mul > 50) return;
 post->modum[current] = mod;
 int mag=0;
 nam = wnd.CreateWriteBox("Podaj poziom magii:");
 sscanf(nam,"%d", &mag);
 if (mag < -10) return;
 if (mag > 10) return;
 post->magum[current] = mag;
 post->tesum[current] = (post->cecha[post->ceum[current]]/2) + (post->mulum[current]*post->pozum[current])+ post->modum[current];
 strcpy(post->um[current], skiname);
 post->il_um++;
 *lum -= cost;
 char ttt[STRING];
 sprintf(ttt,"Pozostalo punktow pd: %d", *lum);
 wnd.BlackWings();
 wnd.PrepareWndText(ttt);
 wnd.StdTextWrite();
 wnd.DoQuit();
 RecountAll();
}


void PostAddSkillPDFunc(Wnd& wnd, void* far_ptr)
{
 tout("PostAddSkillPDFunc");
 int * lum = (int*)far_ptr;
 if (!lum) return;
 if (post->il_um == MAX_UM)
   {
    wnd.CreateMBoxChild("Masz juz maksimum umiejetnosci");
    return;
   }
 if (db->ilosc[3] == 0)
   {
    wnd.CreateMBoxChild("Brak umiejetnosci w bazie danych!");
    return;
   }
 ASortSkillFunc(wnd, NULL);
 char ** tmp;
 char tmp2[120];
 tmp = new char*[db->ilosc[3]+1];
 if (!tmp) panic("memory exception in SetNamesFunc - tmp");
 for (int i=0;i<db->ilosc[3];i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in SetNamesFunc - tmp[]");
     sprintf(tmp2, "(%d)%s", i, db->s_name[i]);
     strcpy(tmp[i], tmp2);
    }
 Button** btn = new Button*[1];
 if (!btn) panic("cannot create addictional_button in CreateExCheckBoxvert!\n");
 btn[0] = new Button;
 if (!btn[0]) panic("memory exception in PostRaceFunc - Button* base_btn[0]");
 btn[0]->SetBtnName("Wlasna um.");
 btn[0]->SetBtnFunc(OwnSkillPDFunc);
 btn[0]->SetBtnSize(100, 24);
 btn[0]->SetBtnPos(250, 500);
 btn[0]->SetVoid((void*)lum);
 char* nam = wnd.CreateExCheckBoxVert("Wybierz umiejetnosc:", tmp, db->ilosc[3], 60, 900, 600, 1, btn);
 if (!strcmp(nam,"Wlasna um.")) return;
 int num=0;
 sscanf(nam,"(%d)", &num);
 int poz = 1;
 int ipoz;
 ipoz = poz + db->s_mod[num][DIFF] -1;
 if (ipoz < 0) return;
 if (ipoz >= db->ilosc[5]) return;
 int cost = db->pd_cost[ipoz];
 if (*lum<cost)
   {
    wnd.CreateMBoxChild("Za malo punktow PD!");
    return;
   }
 int current = post->il_um;
 *lum -= cost;
 post->pozum[current] = poz;
 post->trum[current]  = db->s_mod[num][DIFF];
 post->ceum[current]  = db->s_mod[num][ATTRIBUTE];
 post->mulum[current] = db->s_mod[num][MULTIPLE];
 post->modum[current] = db->s_mod[num][MOD];
 post->magum[current] = db->s_mod[num][MAGIC];
 post->tesum[current] = (post->cecha[post->ceum[current]]/2) + (post->mulum[current]*post->pozum[current])+ post->modum[current];
 strcpy(post->um[current], db->s_name[num]);
 post->il_um++;
 char ttt[STRING];
 sprintf(ttt,"Pozostalo punktow pd: %d", *lum);
 wnd.BlackWings();
 wnd.PrepareWndText(ttt);
 wnd.StdTextWrite();
 RecountAll();
}


void OwnSkillPD0Func(Wnd& wnd, void* far_ptr)
{
 tout("OwnSkillFunc");
 wnd.SetUnusedCharPtr(wnd.RetSelectedButtonName());
 int* lum = (int*)far_ptr;
 if (!lum) return;
 int poz = 0;
 char* skiname = wnd.CreateWriteBox("Podaj nazwe umiejetnosci:");
 if (poz < 0) return;
 int trd=1;
 char* nam = wnd.CreateWriteBox("Podaj trudnosc:");
 sscanf(nam,"%d", &trd);
 if (trd < 0) return;
 int ipoz;
 ipoz = poz + trd -1;
 if (ipoz < 0) return;
 if (ipoz >= db->ilosc[5]) return;
 int next_cost = db->pd_cost[ipoz+1];
 int cost = 0;
 char uuu[STRING];
 sprintf(uuu,"Poziom 1 kosztuje %d\n podaj mniejsza ilosc plusow:", next_cost);
 nam = wnd.CreateWriteBox(uuu);
 sscanf(nam,"%d", &cost);
 if (cost >= next_cost)
   {
    wnd.CreateMBoxChild("Zbyt duzo plusow!");
    return;
   }
 if (*lum<cost)
   {
    wnd.CreateMBoxChild("Za malo punktow PD!");
    return;
   }
 int current = post->il_um;
 post->pozum[current] = poz;
 post->trum[current]  = trd;
 char ** tmp;
 char tmp2[120];
 tmp = new char*[post->ilosc_cech+1];
 if (!tmp) panic("memory exception in SetNamesFunc - tmp");
 for (int i=0;i<post->ilosc_cech;i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in SetNamesFunc - tmp[]");
     sprintf(tmp2, "(%d)%s", i, post->n_cechy[i]);
     strcpy(tmp[i], tmp2);
    }
 nam = wnd.CreateCheckBoxChild("Wybierz ceche testujaca", tmp, post->ilosc_cech);
 int ce=0;
 sscanf(nam,"(%d)", &ce);
 post->ceum[current]  = ce;
 int mul=3;
 nam = wnd.CreateWriteBox("Podaj mnoznik:");
 sscanf(nam,"%d", &mul);
 if (mul < 0) return;
 if (mul > 10) return;
 post->mulum[current] = mul;
 int mod=0;
 nam = wnd.CreateWriteBox("Podaj modyfikator testu:");
 sscanf(nam,"%d", &mod);
 if (mod < -50) return;
 if (mul > 50) return;
 post->modum[current] = mod;
 int mag=0;
 nam = wnd.CreateWriteBox("Podaj poziom magii:");
 sscanf(nam,"%d", &mag);
 if (mag < -10) return;
 if (mag > 10) return;
 post->magum[current] = mag;
 post->tesum[current] = (post->cecha[post->ceum[current]]/2) + (post->mulum[current]*post->pozum[current])+ post->modum[current];
 strcpy(post->um[current], skiname);
 post->il_um++;
 *lum -= cost;
 char ttt[STRING];
 sprintf(ttt,"Pozostalo punktow pd: %d", *lum);
 wnd.BlackWings();
 wnd.PrepareWndText(ttt);
 wnd.StdTextWrite();
 wnd.DoQuit();
 RecountAll();
}


void PostAddSkillPD0Func(Wnd& wnd, void* far_ptr)
{
 tout("PostAddSkillPD0Func");
 int * lum = (int*)far_ptr;
 if (!lum) return;
 if (post->il_um == MAX_UM)
   {
    wnd.CreateMBoxChild("Masz juz maksimum umiejetnosci");
    return;
   }
 if (db->ilosc[3] == 0)
   {
    wnd.CreateMBoxChild("Brak umiejetnosci w bazie danych!");
    return;
   }
 ASortSkillFunc(wnd, NULL);
 char ** tmp;
 char tmp2[120];
 tmp = new char*[db->ilosc[3]+1];
 if (!tmp) panic("memory exception in SetNamesFunc - tmp");
 for (int i=0;i<db->ilosc[3];i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in SetNamesFunc - tmp[]");
     sprintf(tmp2, "(%d)%s", i, db->s_name[i]);
     strcpy(tmp[i], tmp2);
    }
 Button** btn = new Button*[1];
 if (!btn) panic("cannot create addictional_button in CreateExCheckBoxvert!\n");
 btn[0] = new Button;
 if (!btn[0]) panic("memory exception in PostRaceFunc - Button* base_btn[0]");
 btn[0]->SetBtnName("Wlasna um.");
 btn[0]->SetBtnFunc(OwnSkillPD0Func);
 btn[0]->SetBtnSize(100, 24);
 btn[0]->SetBtnPos(250, 500);
 btn[0]->SetVoid((void*)lum);
 char* nam = wnd.CreateExCheckBoxVert("Wybierz umiejetnosc:", tmp, db->ilosc[3], 60, 900, 600, 1, btn);
 if (!strcmp(nam,"Wlasna um.")) return;
 int num=0;
 sscanf(nam,"(%d)", &num);
 int poz = 0;
 int ipoz;
 ipoz = poz + db->s_mod[num][DIFF] -1;
 if (ipoz < 0) return;
 if (ipoz >= db->ilosc[5]) return;
 int next_cost = db->pd_cost[ipoz+1];
 int cost = 0;
 char uuu[STRING];
 sprintf(uuu,"Poziom 1 kosztuje %d\n podaj mniejsza ilosc plusow:", next_cost);
 nam = wnd.CreateWriteBox(uuu);
 sscanf(nam,"%d", &cost);
 if (cost >= next_cost)
   {
    wnd.CreateMBoxChild("Zbyt duzo plusow!");
    return;
   }
 if (*lum<cost)
   {
    wnd.CreateMBoxChild("Za malo punktow PD!");
    return;
   }
 int current = post->il_um;
 *lum -= cost;
 post->p_u[current] = cost;
 post->pozum[current] = poz;
 post->trum[current]  = db->s_mod[num][DIFF];
 post->ceum[current]  = db->s_mod[num][ATTRIBUTE];
 post->mulum[current] = db->s_mod[num][MULTIPLE];
 post->modum[current] = db->s_mod[num][MOD];
 post->magum[current] = db->s_mod[num][MAGIC];
 post->tesum[current] = (post->cecha[post->ceum[current]]/2) + (post->mulum[current]*post->pozum[current])+ post->modum[current];
 strcpy(post->um[current], db->s_name[num]);
 post->il_um++;
 char ttt[STRING];
 sprintf(ttt,"Pozostalo punktow pd: %d", *lum);
 wnd.BlackWings();
 wnd.PrepareWndText(ttt);
 wnd.StdTextWrite();
 RecountAll();
}


void PDKillFunc(Wnd& wnd, void* ptr)
{
 int ok = wnd.CreateYesNoChild("Jestes tego pewien/pewna?");
 if (!ok) return;
 post->IsDead = 1;
 FastSave(wnd);
 PostGenHTMLFunc(wnd, NULL);
 wnd.DoQuit();
}


void __IllegalPDFunc__(Wnd& wnd, ulong key_code, void* far_ptr)
{
 tout("__IllegalPDFunc__ !CHEATER!");
 if (!far_ptr) {tout("in __IllegalPDFunc__ far_ptr=(void*)NULL, aborted!"); return;}
 int key = TransformKey(key_code, 0);
 if (key == 'x')
   {
    int* pd = (int*)far_ptr;
    int code=0;
    char tmp[STRING];
    char* str = wnd.CreateWriteBox("Podaj SUPER_TAJNY KOD:", true);
    sscanf(str,"%d", &code);
    if (code != 0xdead) return;
    tout("SECURITY_CODE_PASSED ===>> modifying (int*)pd");
    str = wnd.CreateWriteBox("Dodac punktow:");
    code = 0;
    sscanf(str,"%d", &code);
    *pd += code;
    sprintf(tmp,"Pozostalo punktow pd: %d", *pd);
    wnd.BlackWings();
    wnd.PrepareWndText(tmp);
    wnd.StdTextWrite();
   }
 RecountAll();
}


void PostDelSkillPDFunc(Wnd& wnd, void* far_ptr)
{
 tout("PostDelSkillFunc");
 if (post->il_um == 0)
   {
    wnd.CreateMBoxChild("Brak umiejetnosci do usuniecia!");
    return;
   }
 char ** tmp;
 char tmp2[120];
 tmp = new char*[post->il_um+1];
 if (!tmp) panic("memory exception in SetNamesFunc - tmp");
 for (int i=0;i<post->il_um;i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in SetNamesFunc - tmp[]");
     sprintf(tmp2, "(%d)%s", i, post->um[i]);
     strcpy(tmp[i], tmp2);
    }
 char* nam = wnd.CreateCheckBoxChild("Wybierz umiejetnosc do usuniecia", tmp, post->il_um);
 int poz=0;
 sscanf(nam,"(%d)", &poz);
 post->il_um--;
 for (int i=poz;i<post->il_um;i++)
   {
    strcpy(post->um[i],post->um[i+1]);
    post->tesum[i] = post->tesum[i+1];
    post->pozum[i] = post->pozum[i+1];
    post->trum[i]  =  post->trum[i+1];
    post->ceum[i]  =  post->ceum[i+1];
    post->mulum[i] = post->mulum[i+1];
    post->p_u[i] = post->p_u[i+1];
   }
 //RecountAll();                                    ????
}


void ImproveSkillFunc(Wnd& wnd, void* far_ptr)
{
 tout("ImproveSkillFunc");
 int* pd = (int*)far_ptr;
 if (!pd) return;
 char ** tmp;
 char tmp2[120];
 tmp = new char*[post->il_um+1];
 if (!tmp) panic("memory exception in SetNamesFunc - tmp");
 for (int i=0;i<post->il_um;i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in SetNamesFunc - tmp[]");
     sprintf(tmp2, "(%d)%s", i, post->um[i]);
     strcpy(tmp[i], tmp2);
    }
 char* nam = wnd.CreateCheckBoxVert("Wybierz umiejetnosc", tmp, post->il_um);
 int num=0;
 sscanf(nam,"(%d)", &num);
 int dat = post->trum[num] + post->pozum[num];
 if (dat >= db->ilosc[6])
   {
    wnd.CreateMBoxChild("poziom na ktory chcesz sie wznosic\nnie jest uwzgledniony w DB\nEdytuj DB!!");
    return;
   }
 int req = db->pd_cost[dat];
 int left = req - post->p_u[num];
 char ttt[LONG_STR];
 sprintf(ttt,"umiejetnosc %s ma \naktualnie %d plusow\nDo zwiekszenia potrzeba %d\nbrakuje %d, ile dodac?",post->um[num], post->p_u[num], req,left);
 nam = wnd.CreateWriteBox(ttt);
 int plusy=1;
 sscanf(nam,"%d",&plusy);
 if (plusy<0) return;
 if (plusy>*pd) return;
 if (plusy>left)
   {
    wnd.CreateMBoxChild("Za duzo!");
    return;
   }
 *pd = *pd - plusy;
 post->p_u[num] += plusy;
 if (post->p_u[num] >= req)
        {
         sprintf(ttt,"Um. %s zwiekszona!", post->um[num]);
	 wnd.CreateMBoxChild(ttt);
         post->p_u[num] = 0;
         post->pozum[num]++;
         post->tesum[num] += post->mulum[num];
	 RecountAll();
        }
 sprintf(ttt,"Pozostalo punktow pd: %d", *pd);
 wnd.BlackWings();
 wnd.PrepareWndText(ttt);
 wnd.StdTextWrite();
}


void ImproveAttrFunc(Wnd& wnd, void* far_ptr)
{
 tout("ImproveAttrFunc");
 int* pd = (int*)far_ptr;
 if (!pd) return;
 char ** tmp;
 char tmp2[120];
 tmp = new char*[post->ilosc_cech+1];
 if (!tmp) panic("memory exception in SetNamesFunc - tmp");
 for (int i=0;i<post->ilosc_cech;i++)
    {
     tmp[i] = new char[STRING];
     if (!tmp) panic("memory exception in SetNamesFunc - tmp[]");
     sprintf(tmp2, "(%d)%s", i, post->n_cechy[i]);
     strcpy(tmp[i], tmp2);
    }
 char* nam = wnd.CreateCheckBoxChild("Wybierz ceche", tmp, post->ilosc_cech);
 int num=0;
 sscanf(nam,"(%d)", &num);
 char ttt[STRING];
 sprintf(ttt,"cecha %s ma aktualnie %d plusow\nDo zwiekszenia potrzeba 13, ile dodac?",post->n_cechy[num],post->p_c[num]);
 nam = wnd.CreateWriteBox(ttt);
 int left = 13 - post->p_c[num];
 int plusy=1;
 sscanf(nam,"%d",&plusy);
 if (plusy<0) return;
 if (plusy>*pd) return;
 if (plusy>left)
   {
    wnd.CreateMBoxChild("Za duzo!");
    return;
   }
 *pd = *pd - plusy;
 post->p_c[num] += plusy;
 if (post->p_c[num] >= 13)
        {
         sprintf(ttt,"Cecha %s zwiekszona!", post->n_cechy[num]);
	 wnd.CreateMBoxChild(ttt);
         post->p_c[num] = 0;
         post->cecha[num]++;
	 RecountAll();
        }
 sprintf(ttt,"Pozostalo punktow pd: %d", *pd);
 wnd.BlackWings();
 wnd.PrepareWndText(ttt);
 wnd.StdTextWrite();
}


void CharacterPD(Wnd& wnd)
{
 tout("CharacterPD");
 int ok = LoadPost(wnd);
 if (!ok) return;
 ok = FastSave(wnd);
 if (!ok) return ;
 RecountAll();
 char* str_scan = wnd.CreateWriteBox("Podaj ilosc PD:");
 int *pd = new int;
 if (!pd) panic("memory exception in CharacterPD::(int*)pd");
 int *pass_ekw = new int;
 if (!pass_ekw) wnd.CreateMBoxChild("Pass_ekw failed with\n code (int*) == (void*)NULL\nDo not use equip\nto avoid SIGSEGV!\n");
 if (pass_ekw) *pass_ekw = YES_ID;
 *pd = 6;
 sscanf(str_scan,"%d", pd);
 char str[STRING];
 sprintf(str,"Pozostalo punktow pd: %d", *pd);
 wnd.Invalidate();
 int start_pd = *pd;
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 600, 500);
 if (!wybor) panic("memory exception in creating win_child!");
 wybor->WinDefaults("Rozdawanie PD...");
 wybor->PrepareWndText(str);
 wybor->StdTextWrite();
 wybor->SelectKeyPressXFunc(__IllegalPDFunc__);//ok
 wybor->SetEventData(EV_KPRESSX, (void*)pd);//ok
 wybor->SetButtonsNumber(15);
 wybor->CreateButton(0, "Zwieksz ceche"          ,50 , 100, ImproveAttrFunc, true, 200, 24, (void*)pd);
 wybor->CreateButton(1, "Zwieksz um."            ,350, 100, ImproveSkillFunc, false, 200, 24, (void*)pd);//ok
 wybor->CreateButton(2, "Karta Postaci"          ,50 , 130, CharacterCard, false, 200);//ok
 wybor->CreateButton(3, "Nowa um."               ,350, 130, PostAddSkillPDFunc, false, 200, 24, (void*)pd);//ok
 wybor->CreateButton(4, "Zaklecia"               ,50 , 160, PostSpellPDFunc, false, 200, 24, (void*)pd);//ok
 wybor->CreateButton(5, "Ekwipunek"              ,350, 160, PostEkwFunc, false, 200, 24, (void*)(pass_ekw));//ok
 wybor->CreateButton(6, "Um. poziomu 0"          ,50 , 190, PostAddSkillPD0Func, false, 200, 24, (void*)pd);//ok
 wybor->CreateButton(7, "Kod dostepu"            ,350, 190, ChangePostPass, false, 200);//ok
 wybor->CreateButton(8, "HTML"                   ,50 , 220, PostGenHTMLFunc, false, 200);//ok
 wybor->CreateButton(9, "Rysunek"                ,350, 220, DrawPost, false, 200);//ok
 wybor->CreateButton(10, "Zapisz postac"         ,50 , 250, FFastSave, false, 200);//ok
 wybor->CreateButton(11, "Usun um."              ,350, 250, PostDelSkillPDFunc, false, 200);//ok
 wybor->CreateButton(12, "Edytuj baze"           ,50 , 280, AccessEditFunc, false, 200);//ok
 wybor->CreateButton(13, "Zabij postac"          ,350, 280, PDKillFunc, false, 200);//ok
 wybor->CreateButton(14, "Zakoncz"               ,200, 310, StdOKFunc, false, 200);//ok
 wybor->SetParent(&wnd);
 RecountAll();
 wybor->DefaultResponse();
 RecountAll();
 post->PD += (start_pd - *pd);
 FastSave(wnd);
 PostGenHTMLFunc(wnd, NULL);
 wnd.Invalidate();
}


void PDFunc(Wnd& wnd, void* ptr)
{
 tout("PDFunc");
 int num = 0;
 char** table = ReadFolderToTable(num, "./Characters");
 if (!table)
   {
    wnd.CreateMBoxChild("HARD_ERROR: Cannot read anything from ./Characters!!!");
    trace("hard_error: cannot read_dir: ./Characters!");
   }
 if (!num)
   {
    wnd.CreateMBoxChild("Brak plikow w folderze: ./Characters");
    return ;
   }
 FSort(table, num);
 char* selected = wnd.CreateCheckBoxVert("Wybierz postac:", table, num);
 CreateRPG(selected);
 CharacterPD(wnd);
 wnd.AboutQuit(1); //FIXME continue
 dbase = true;
}


void EditCharFunc(Wnd& wnd, void* ptr)
{
 tout("EditCharFunc");
 int num = 0;
 char** table = ReadFolderToTable(num, "./Characters");
 if (!table)
   {
    wnd.CreateMBoxChild("HARD_ERROR: Cannot read anything from ./Characters!!!");
    trace("hard_error: cannot read_dir: ./Characters!");
   }
 if (!num)
   {
    wnd.CreateMBoxChild("Brak plikow w folderze: ./Characters");
    return ;
   }
 FSort(table, num);
 char* selected = wnd.CreateCheckBoxVert("Wybierz postac:", table, num);
 CreateRPG(selected);
 Character(wnd);
 wnd.AboutQuit(1); //FIXME continue
 dbase = true;
}


void EraseCharFunc(Wnd& wnd, void* ptr)
{
 tout("EraseCharFunc");
 int num = 0;
 char** table = ReadFolderToTable(num, "./Characters");
 if (!table)
   {
    wnd.CreateMBoxChild("HARD_ERROR: Cannot read anything from ./Characters!!!");
    trace("hard_error: cannot read_dir: ./Characters!");
   }
 if (!num)
   {
    wnd.CreateMBoxChild("Brak plikow w folderze: ./Characters");
    return;
   }
 FSort(table, num);
 char* selected = wnd.CreateCheckBoxVert("Wybierz postac:", table, num);
 CreateRPG(selected);
 LoadPostNoA(wnd);
 int ok = CheckPassChar(wnd);
 if (!ok)
   {
    wnd.CreateMBoxChild("Blad procedury dostepu!");
    if (post) delete post;
    post = NULL;
    return;
   }
 char todel[STRING];
 sprintf(todel,"rm -f ./Characters/%s", selected);
 ok = 0;
 ok = wnd.CreateYesNoChild("Na pewno usunac postac?");
 if (!ok)
   {
    wnd.DoQuit();
    if (post) delete post;
    post = NULL;
    return;
   }
 system(todel);
 wnd.AboutQuit(1); //FIXME continue
 if (post) delete post;
 post = NULL;
 dbase = true;
}


void CreateCharFunc(Wnd& wnd, void* ptr)
{
 tout("CreateCharFunc");
 char* name = wnd.CreateWriteBox("Podaj nazwe nowej postaci:");
 int ok = CreateRPG(name, true);
 if (!ok)
   {
    wnd.CreateMBoxChild("Taka postac juz istnieje\nSprobuj innej nazwy...");
    return;
   }
 Character(wnd, false);
 wnd.AboutQuit(1); //FIXME continue
 dbase = true;
}


void DataBaseFunc(Wnd& wnd, void* ptr)
{
 tout("DataBaseFunc");
 if (dbase)
  {
   int ans = wnd.CreateYesNoChild("Baza danych jest juz wczytana\nWczytac nowa?");
   if (!ans) return ;
   if (db) delete db;
  }
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 400, 250);
 if (!wybor) panic("memory exception in creating win_child!");
 wybor->CreateGC();
 wybor->SetFont("*-helvetica-*-12-*");
 wybor->SetCaption("Baza Danych");
 wybor->DeclareResponseTable(MEDI_RESPONSES);
 wybor->SelectExposeFunc(StdExposeFunc);
 wybor->SelectConfigureFunc(DenyResizeFunc);
 wybor->SelectKeyPressFunc(eXtendedKeyPress);
 wybor->SelectKeyReleaseFunc(eXtendedKeyRelease);
 wybor->SelectButtonPressFunc(eXtendedButtonPress);
 wybor->SelectButtonReleaseFunc(eXtendedButtonRelease);
 wybor->PrepareWndText("Co chcesz zrobic z baza danych?");
 wybor->StdTextWrite();
 wybor->SetButtonsNumber(5);
 wybor->CreateButton(0, "Wczytaj",50 , 100, LoadFunc, true, 100);
 wybor->CreateButton(1, "Utworz", 250 , 100, CreateFunc, false, 100);
 wybor->CreateButton(2, "Edytuj", 50 , 130, EditFunc, false, 100);
 wybor->CreateButton(3, "Usun"  ,250, 130, EraseFunc, false, 100);
 wybor->CreateButton(4, "Powrot", 150 , 160, StdOKFunc, false, 100);
 wybor->SetParent(&wnd);
 wybor->DefaultResponse();
 //wybor->DoQuit();
 wnd.Invalidate();
}


void CharFunc(Wnd& wnd, void* ptr)
{
 //wnd.CreateMBoxChild("panic: because U liked to.");
 //panic("because U liked to.");
 tout("CharFunc");
 if (!dbase)
  {
   int ok = wnd.CreateYesNoChild("Najpierw nalezy wczytac baze!\nWczytac DataBase/morgoth?");
   if (!ok) return;
   else
     {
      CreateDB("morgoth");
      int ok = LoadDB("morgoth", wnd);
      if (!ok) return ;
      dbase = true;
     }
  }
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 400, 250);
 if (!wybor) panic("memory exception in creating win_child!");
 wybor->CreateGC();
 wybor->SetFont("*-helvetica-*-12-*");
 wybor->SetCaption("Postac");
 wybor->DeclareResponseTable(MEDI_RESPONSES);
 wybor->SelectExposeFunc(StdExposeFunc);
 wybor->SelectConfigureFunc(DenyResizeFunc);
 wybor->SelectKeyPressFunc(eXtendedKeyPress);
 wybor->SelectKeyReleaseFunc(eXtendedKeyRelease);
 wybor->SelectButtonPressFunc(eXtendedButtonPress);
 wybor->SelectButtonReleaseFunc(eXtendedButtonRelease);
 wybor->PrepareWndText("Co chcesz zrobic z postacia");
 wybor->StdTextWrite();
 wybor->SetButtonsNumber(5);
 wybor->CreateButton(0, "Rozdaj PD",50 , 100, PDFunc, false, 100);
 wybor->CreateButton(1, "Utworz", 250 , 100, CreateCharFunc, true, 100);
 wybor->CreateButton(2, "Edytuj", 50 , 130, EditCharFunc, false, 100);
 wybor->CreateButton(3, "Usun", 250, 130, EraseCharFunc, false, 100);
 wybor->CreateButton(4, "Powrot", 150 , 160, StdOKFunc, false, 100);
 wybor->SetParent(&wnd);
 wybor->DefaultResponse();
 //wybor->DoQuit();
 wnd.Invalidate();
}


void AboutFunc(Wnd& wnd, void* ptr)
{
 tout("AboutFunc");
 Wnd* wybor = new Wnd(wnd.RetDspStr(), 0x100, 0x80);
 if (!wybor) panic("memory exception in creating win_child!");
 wybor->WinDefaults("Autor: Morgoth DBMA");
 wybor->SelectKeyPressFunc(eXtendedKeyPressQ);
 wybor->SelectConfigureFunc(NoResizeDrawWFunc);
 wybor->EnableGraph(1);
 wybor->EnableFrames(0);
 char* str = new char[0x140];
 sprintf(str,"XLRPG v0.6 alpha (third edition)\nMorgoth:morgothdbma@o2.pl,heroine@o2.pl\nCompilation date: %s %s\nFile %s", __DATE__, __TIME__,__FILE__);
 wybor->PrepareWndText(str);
 wybor->SetButtonsNumber(1);
 wybor->CreateButton(0, "Yahooo!",88 , 90, StdOKFunc, false);
 for (int i=0;i<0x100;i++) for (int j=0;j<0x80;j++) wybor->SetWndPixel(RGB(0, j*2, i), i, j);
 wybor->DefaultResponse();
 wnd.Invalidate();
 delete [] str;
}


void Main_Game()
{
 tout("MainGame");
  //trace("This program contains memory BUG, but I don't know where :(");
  init_random();
  Dsp* main_dsp = new Dsp;
  if (!main_dsp) panic("memory_exception_in_new",__LINE__,__FILE__);
  Wnd* wnd = new Wnd(main_dsp, 400, 200);
  if (!wnd) panic("memory_exception_in_new",__LINE__,__FILE__);
 char* str = new char[0x140];
 sprintf(str,"XLRPG v0.6 alpha (third edition)\nMorgothDBMA: morgothdbma@o2.pl, heroine@o2.pl\nCompilation date: %s %s\nFile %s", __DATE__, __TIME__,__FILE__);
  wnd->CreateGC();
  wnd->SetFont("*-helvetica-*-12-*");
  wnd->SetCaption("XLrpg ver 0.6 by Morgoth");
  wnd->DeclareResponseTable(MEDI_RESPONSES);
  wnd->SelectExposeFunc(StdExposeFunc);
  wnd->SelectConfigureFunc(DenyResizeFunc);
  wnd->SelectKeyPressFunc(eXtendedKeyPressQ);
  wnd->SelectKeyReleaseFunc(eXtendedKeyRelease);
  wnd->SelectButtonPressFunc(eXtendedButtonPress);
  wnd->SelectButtonReleaseFunc(eXtendedButtonRelease);
  wnd->PrepareWndText(str);
  wnd->StdTextWrite();
  wnd->SetButtonsNumber(4);
  wnd->CreateButton(0, "Baza danych",60 , 100, DataBaseFunc, true, 100);
  wnd->CreateButton(1, "Postac", 240 , 100, CharFunc, false, 100);
  wnd->CreateButton(2, "Zakoncz", 60 , 160, StdOKFunc, false, 100);
  wnd->CreateButton(3, "Autor", 240 , 160, AboutFunc, false, 100);
  wnd->DefaultResponse();
  if (str) delete [] str;
  if (wnd) delete wnd;
  if (main_dsp) delete main_dsp;
  if (db) delete db;
  if (post) delete post;
  kill_random();
}


int main(int lb, char** par)
{
 tout("main");
 printf("XRPG started, compilation time: %s %s; file %s\n", __DATE__, __TIME__, __FILE__);
 Main_Game();
 tout("no_SIGSEGV_exit");
 return 0;
}

