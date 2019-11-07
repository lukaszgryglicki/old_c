#include <graphics>
#include <conio>
#include <stdio>
#include <stdlib>
#include <fstream>
#include <stdarg>
#if !defined DEFIN_H
#define DEFIN_H

int    GraphDriver;
int    GraphMode;
double AspectRatio;
int    MaxX, MaxY;
int    MaxColors;
int    ErrorCode;
struct palettetype palette;
void changetextstyle(int font, int direction, int charsize)
{
  int ErrorCode;
  graphresult();                        /* clear error code             */
  settextstyle(font, direction, charsize);
  ErrorCode = graphresult();            /* check result                 */
  if( ErrorCode != grOk ){              /* if error occured             */
	 closegraph();
	 printf(" Graphics System Error: %s\n", grapherrormsg( ErrorCode ) );
	 exit( 1 );
  }
}
void Initialize(void)
{
  int x, y;

  GraphDriver = DETECT;
  initgraph( &GraphDriver, &GraphMode, "" );
  ErrorCode = graphresult();
  if( ErrorCode != grOk ){
	 printf(" Graphics System Error: %s\n", grapherrormsg( ErrorCode ) );
	 getch();
	 exit( 1 );
  }

  getpalette( &palette );
  MaxColors = getmaxcolor() + 1;

  MaxX = getmaxx();
  MaxY = getmaxy();

  getaspectratio( &x, &y );
  AspectRatio = (double)x / (double)y;

}
#endif
