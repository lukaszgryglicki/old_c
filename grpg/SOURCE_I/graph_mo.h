#if !defined GM_H
#define GM_H
void DrawBorder(void);
void MainWindow(char*);
void changetextstyle(int, int, int);
int gprintf( int *, int *, char *, ... );
void intro(char* ,int* ,int* );
void text(void);
void takes(char*);
void takei(int*);
const int NFONTS = 3;
char *Fonts[NFONTS] = {
  "DefaultFont",   "TriplexFont",   "SmallFont",
};
struct PTS {
  int x, y;
};

void MainWindow( char *header )             //tworzy okienko
{
  int height;

  cleardevice();                        /* Clear graphics screen        */
  setcolor( WHITE );            /* Set current color to white   */
  setviewport( 0, 0, MaxX, MaxY, 1 );   /* Open port to full screen     */

  height = textheight( "H" );           /* Get basic text height        */

//  changetextstyle( DEFAULT_FONT, HORIZ_DIR, 1 );
  settextjustify( CENTER_TEXT, TOP_TEXT );
  setcolor( WHITE );
  outtextxy( MaxX/2, 2, header );
  setviewport( 0, height+4, MaxX, MaxY-(height+4), 1 );
  DrawBorder();
  setviewport( 1, height+5, MaxX-1, MaxY-(height+5), 1 );

}
void DrawBorder(void)             //rysuje granice
{
  struct viewporttype vp;

  setcolor( WHITE );            /* Set current color to white   */

  setlinestyle( SOLID_LINE, 0, NORM_WIDTH );

  getviewsettings( &vp );
  rectangle( 0, 0, vp.right-vp.left, vp.bottom-vp.top );

}

int gprintf( int *xloc, int *yloc, char *fmt, ... )
{
  va_list  argptr;                      /* Argument list pointer        */
  char str[140];                        /* Buffer to build sting into   */
  int cnt;                              /* Result of SPRINTF for return */

  va_start( argptr, fmt );              /* Initialize va_ functions     */

  cnt = vsprintf( str, fmt, argptr );   /* prints string to buffer      */
  outtextxy( *xloc, *yloc, str );       /* Send string in graphics mode */
  *yloc += textheight( "H" ) + 2;       /* Advance to next line         */

  va_end( argptr );                     /* Close va_ functions          */

  return( cnt );                        /* Return the conversion count  */

}
void Table(int x, int y,int len)
{
 setcolor(LIGHTBLUE);
 rectangle(x,y,x+len*textheight("H"),y+textheight("H")+2);
 setcolor(WHITE);
}
void gln(int* x,int* y)
{
 *y += textheight( "H" ) + 2;
 *x =25;
}
void glns(int* x,int* y ,int il)
{
 for (int i=0;i<il;i++)gln(x,y);
}
int ggetch(int zn,int* x,int* y,int col1,int col2)
{
 setcolor(col1);
 line(0,*y+10,MaxX,*y+10);
 setcolor(col2);
 line(*x+3,0,*x+3,MaxY);
 setcolor(YELLOW);
 setfillstyle(SOLID_FILL,YELLOW);
 fillellipse(*x+3,*y+10,2,2);
 setcolor(WHITE);
 gln(x,y);
 zn = getch();
 *x = 25;
 return zn;
}
int ggetch(int zn,int* x,int* y)
{
 setcolor(GREEN);
 line(0,*y+10,MaxX,*y+10);
 setcolor(BLUE);
 line(*x+3,0,*x+3,MaxY);
 setcolor(YELLOW);
 setfillstyle(SOLID_FILL,YELLOW);
 fillellipse(*x+3,*y+10,2,2);
 setcolor(WHITE);
 gln(x,y);
 zn = getch();
 *x = 25;
 return zn;
}
int gprint( bool ln,int *xloc, int *yloc, char *fmt, ... )
{
  va_list  argptr;                      /* Argument list pointer        */
  char str[140];                        /* Buffer to build sting into   */
  int cnt;                              /* Result of SPRINTF for return */

  va_start( argptr, fmt );              /* Initialize va_ functions     */

  cnt = vsprintf( str, fmt, argptr );   /* prints string to buffer      */
  outtextxy( *xloc, *yloc, str );       /* Send string in graphics mode */
  if (ln==true)*yloc += textheight( "H" ) + 2;
  if (ln==false) *xloc += strlen(fmt) * textheight("H");

  va_end( argptr );                     /* Close va_ functions          */

  return( cnt );                        /* Return the conversion count  */

}
int tgprint(int len, bool ln,int *xloc, int *yloc, char *fmt, ... )
{
  va_list  argptr;                      /* Argument list pointer        */
  char str[140];                        /* Buffer to build sting into   */
  int cnt;                              /* Result of SPRINTF for return */

  va_start( argptr, fmt );              /* Initialize va_ functions     */

  cnt = vsprintf( str, fmt, argptr );   /* prints string to buffer      */
  Table(*xloc,*yloc,strlen(fmt)+len);
  outtextxy( *xloc, *yloc, str );       /* Send string in graphics mode */
  if (ln==true)*yloc += textheight( "H" ) + 2;
  if (ln==false) *xloc += strlen(fmt) * textheight("H");

  va_end( argptr );                     /* Close va_ functions          */

  return( cnt );                        /* Return the conversion count  */

}
void text()
{
 int font = 2;
 setusercharsize( 5, 6, 2, 3 );
 changetextstyle( font, HORIZ_DIR, USER_CHAR_SIZE );
 settextjustify(LEFT_TEXT,TOP_TEXT);
}
void intro(char* par,int* x,int* y)
{
 cleardevice();
 gotoxy(1,1);
 MainWindow(par);
 *x=25;
 *y=25;
 text();
}
void takes(char* string, int color)
{
 setcolor(WHITE);
 line(MaxX*3/4,MaxY-10,MaxX*3/4,4);
 setcolor(color);
 setfillstyle(SOLID_FILL,color);
 bar(MaxX*3/4+3,40,MaxX-5,8);
 setcolor(BLACK);
 setfillstyle(SOLID_FILL,BLACK);
 bar(MaxX*3/4+5,38,MaxX-7,10);
 gotoxy(62,3);
 gets(string);
 setcolor(WHITE);
 gotoxy(1,1);
}
void takes(char* string)
{
 setcolor(WHITE);
 line(MaxX*3/4,MaxY-10,MaxX*3/4,4);
 setcolor(LIGHTGREEN);
 setfillstyle(SOLID_FILL,LIGHTGREEN);
 bar(MaxX*3/4+3,40,MaxX-5,8);
 setcolor(BLACK);
 setfillstyle(SOLID_FILL,BLACK);
 bar(MaxX*3/4+5,38,MaxX-7,10);
 gotoxy(62,3);
 gets(string);
 setcolor(WHITE);
 gotoxy(1,1);
}
void takei(int* zn)
{
 setcolor(WHITE);
 line(MaxX*3/4,MaxY-10,MaxX*3/4,4);
 setcolor(LIGHTGREEN);
 setfillstyle(SOLID_FILL,LIGHTGREEN);
 bar(MaxX*3/4+3,40,MaxX-5,8);
 setcolor(BLACK);
 setfillstyle(SOLID_FILL,BLACK);
 bar(MaxX*3/4+5,38,MaxX-7,10);
 gotoxy(62,3);
 cin>>*zn;
 setcolor(WHITE);
 gotoxy(1,1);
}
void taked(double* zn)
{
 setcolor(WHITE);
 line(MaxX*3/4,MaxY-10,MaxX*3/4,4);
 setcolor(LIGHTBLUE);
 setfillstyle(SOLID_FILL,LIGHTBLUE);
 bar(MaxX*3/4+3,40,MaxX-5,8);
 setcolor(BLACK);
 setfillstyle(SOLID_FILL,BLACK);
 bar(MaxX*3/4+5,38,MaxX-7,10);
 gotoxy(62,3);
 cin>>*zn;
 setcolor(WHITE);
 gotoxy(1,1);
}
void takesh(short* zn)
{
 setcolor(WHITE);
 line(MaxX*3/4,MaxY-10,MaxX*3/4,4);
 setcolor(YELLOW);
 setfillstyle(SOLID_FILL,YELLOW);
 bar(MaxX*3/4+3,40,MaxX-5,8);
 setcolor(BLACK);
 setfillstyle(SOLID_FILL,BLACK);
 bar(MaxX*3/4+5,38,MaxX-7,10);
 gotoxy(62,3);
 cin>>*zn;
 setcolor(WHITE);
 gotoxy(1,1);
}
void takei(int* zn,int color)
{
 setcolor(WHITE);
 line(MaxX*3/4,MaxY-10,MaxX*3/4,4);
 setcolor(color);
 setfillstyle(SOLID_FILL,color);
 bar(MaxX*3/4+3,40,MaxX-5,8);
 setcolor(BLACK);
 setfillstyle(SOLID_FILL,BLACK);
 bar(MaxX*3/4+5,38,MaxX-7,10);
 gotoxy(62,3);
 cin>>*zn;
 setcolor(WHITE);
 gotoxy(1,1);
}
void takel(unsigned long* zn,int color)
{
 setcolor(WHITE);
 line(MaxX*3/4,MaxY-10,MaxX*3/4,4);
 setcolor(color);
 setfillstyle(SOLID_FILL,color);
 bar(MaxX*3/4+3,40,MaxX-5,8);
 setcolor(BLACK);
 setfillstyle(SOLID_FILL,BLACK);
 bar(MaxX*3/4+5,38,MaxX-7,10);
 gotoxy(62,3);
 cin>>*zn;
 setcolor(WHITE);
 gotoxy(1,1);
}
#endif
