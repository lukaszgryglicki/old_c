#include "16bitm.h"

void SayHello(unsigned int mem)
{
 cout<<"Kolekcjoner plyt wersja 2.2 + wyszukiwarka, 16 bit\nPrzeznaczona na system MS-DOS\n";
 cout<<"\n\nAutor:\tmorgothdbma wymagana pamiec "<<mem<<" bajtow, copyleft@\n";
 cout<<"Maksymalnie wpisow "<<MAX_CD<<"\n";
}


int main()
{
 SayHello(sizeof(DosApp));
 DosApp* Application = new DosApp;
 Application->Run();
 delete Application;
}

