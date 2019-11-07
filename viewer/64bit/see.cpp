#include <stdlib.h>
#include <stdio.h>
#include <fstream.h>
#include <mgetch.h>

#define STRING		70


static char* conf = "conf.dat";

void execute(char* plik, char* folder, int max_file)
{
 FILE* p;
 if ((p = fopen(plik,"r+")) == NULL)
 	{
    cout<<"File "<<plik<<" does not exists!\n";
    return;
   }
 else
 	{
    char zn;
    int current = 0;
    int i = 0;
    char name[STRING];
    strcpy(name,folder);
    strcat(name,"\\out_");
    char lbd[STRING];
    sprintf(lbd,"_%x",current);
    strcat(name,lbd);
    strcat(name,".txt");
    FILE* out;
    if ((out = fopen(name,"w+")) == NULL)
    	{
       cout<<"Cannot write to "<<name<<".\n";
       return;
      }
    cout<<"Processing "<<name<<"\n";
    while ((zn = fgetc(p)) != EOF)
    	{
       fprintf(out,"%c",zn);
       i++;
       if (i >= max_file)
       	{
          i = 0;
          current++;
          fclose(out);
          strcpy(name,folder);
          strcat(name,"\\out_");
          sprintf(lbd,"_%x",current);
          strcat(name,lbd);
          strcat(name,".txt");
          if ((out = fopen(name,"w+")) == NULL)
    	 		{
             cout<<"Cannot write to "<<name<<"\n";
             return;
      		}
          cout<<"Processing "<<name<<".\n";
         }
      }
    fclose(out);
    fclose(p);
   }
}

int main(int lb, char** par)
{
 char folder[STRING];
 int max_file;
 char file[STRING];
 if (lb == 1)
 	{
    cout<<"File name to view: ";
    gets(file);
   }
 else strcpy(file,par[1]);
 cout<<"Execute "<<file<<" in progress\n";
 FILE* plik;
 if ((plik = fopen(conf,"r+")) == NULL)
 	{
    cout<<"No conf file...create it!\n";
    cout<<"Output directory name: ";
    gets(folder);
    cout<<"Max file length: ";
    cin>>max_file;
    if ((plik = fopen(conf,"w+")) == NULL)
    	{
       cout<<"File "<<file<<" cannot be saved!\n";
      }
    else
    	{
       fprintf(plik,"Directory name is %s\n",folder);
       fprintf(plik,"Max file length is %d bytes\n",max_file);
       fclose(plik);
      }
   }
 else
 	{
    fscanf(plik,"Directory name is %s\n",folder);
    fscanf(plik,"Max file length is %d bytes\n",&max_file);
    fclose(plik);
    printf("Conf read is %s/ Len: %d B\n",folder,max_file);
   }
 cout<<"Max File length is "<<max_file<<endl;
 execute(file,folder,max_file);
 cout<<"Ready...";
 getch();
}
