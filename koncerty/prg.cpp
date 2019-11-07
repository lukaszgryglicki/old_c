#include "prg.h"


int main(int, char**)
{
 KASETA* kaseta = new KASETA;
 cout<<"Kaseta: "<<sizeof(KASETA)<<" bajtow\t\t\t[OK]\n";
 Run(kaseta); 
 delete kaseta;
 EndWin();
 return OK;
}
