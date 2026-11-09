#include <X11/Xlib.h>
typedef unsigned long int ulong;

void DrawFrame(Display* dsp, Window& win, ulong col, GC& gc, long fade, int thin, int sx, int ex, int sy, int ey)
{
#ifdef DEBUG
 printf("DEBUG_MGRAPH_H: DrawFrame(%p,window,%d,GC,%d,%d,%d,%d,%d,%d) dsp,win,col,gc,fade,this,sx,ex,sy,ey\n", dsp,(int)col,(int)fade,thin,sx,ex,sy,ey);
#endif
 for (int i=0;i<thin;i++)
   {
    XSetForeground(dsp, gc, col+i*fade);
    XDrawLine(dsp, win ,gc, sx+i, sy+i, ex-i, sy+i);
    XDrawLine(dsp, win ,gc, sx+i, sy+i, sx+i, ey-i);
    XDrawLine(dsp, win ,gc, sx+i, ey-i, ex-i, ey-i);
    XDrawLine(dsp, win ,gc, ex-i, sy+i, ex-i, ey-i);
   }
#ifdef DEBUG
 printf("DEBUG_MGRAPH_H: OK\n");
#endif
}

void DrawHalfFrame(Display* dsp, Window& win, ulong col, GC& gc, long fade, int thin, int sx, int ex, int sy, int ey, bool upper)
{
#ifdef DEBUG
 printf("DEBUG_MGRAPH_H: DrawHalfFrame(%p,window,%d,GC,%d,%d,%d,%d,%d,%d,%d) dsp,win,col,gc,fade,this,sx,ex,sy,ey,upper\n", dsp,(int)col,(int)fade,thin,sx,ex,sy,ey,upper);
#endif
 for (int i=0;i<thin;i++)
   {
    XSetForeground(dsp, gc, col+i*fade);
    if (upper)
     {
      XDrawLine(dsp, win ,gc, sx+i, sy+i, ex-i, sy+i);
      XDrawLine(dsp, win ,gc, sx+i, sy+i, sx+i, ey-i);
     }
    else
     {
      XDrawLine(dsp, win ,gc, sx+i, ey-i, ex-i, ey-i);
      XDrawLine(dsp, win ,gc, ex-i, sy+i, ex-i, ey-i);
     }
   }
#ifdef DEBUG
 printf("DEBUG_MGRAPH_H: OK\n");
#endif
}

void XFillA(Display* dsp, Window& win, GC& gc, ulong col, int sx, int sy, int x, int y)
{
#ifdef DEBUG
 printf("DEBUG_MGRAPH_H: XFillA(%p,win,gc,%d,%d,%d,%d,%d) dsp,win,gc,col,sx,sy,x,y\n", dsp,(int)col,sx,sy,x,y);
#endif
 XSetForeground(dsp, gc, col);
 int less = (x < y) ? x : y;
 if (less%2) less++;
 less /= 2;
 for (int i=0;i<=less;i++)
   {
    XDrawLine(dsp, win ,gc, sx+i, sy+i, sx+x-i, sy+i);
    XDrawLine(dsp, win ,gc, sx+i, sy+i, sx+i, sy+y-i);
    XDrawLine(dsp, win ,gc, sx+i, sy+y-i, sx+x-i, sy+y-i);
    XDrawLine(dsp, win ,gc, sx+x-i, sy+i, sx+x-i, sy+y-i);
   }
#ifdef DEBUG
 printf("DEBUG_MGRAPH_H: OK\n");
#endif
}

