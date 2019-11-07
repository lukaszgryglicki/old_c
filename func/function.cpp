#include <windows.h>
#include <math.h>
#include <stdio.h>

#include "function.h"

typedef  long double D;
typedef  double cast;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

long double sx = -10.0;
long double sy = -10.0;
long double ex =  10.0;
long double ey =  10.0;

long double im = 0.0;
static int another = 0;

long double funcprim(long double x, long double i, int prim);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
 static TCHAR szAppName[] = TEXT("Function Drawer v0.1");
 HWND hwnd;
 MSG msg;
 WNDCLASS wndclass;

 wndclass.style = CS_HREDRAW | CS_VREDRAW;
 wndclass.lpfnWndProc = WndProc;
 wndclass.cbClsExtra = 0;
 wndclass.cbWndExtra = 0;
 wndclass.hInstance = hInstance;
 wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
 wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
 wndclass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
 wndclass.lpszMenuName = NULL;
 wndclass.lpszClassName = szAppName;

 if (!RegisterClass(&wndclass))
 	{
    MessageBox(NULL,TEXT("Failed to register WndClass!"), TEXT("Fatal Error!"), MB_ICONERROR);
   }
 if (DEBUG) MessageBox(NULL,TEXT("WndClass successfully registered!"), TEXT("Debug information!"), MB_OK);
 randomize();
 hwnd = CreateWindow(szAppName, TEXT("Function Drawer v0.1"), WS_OVERLAPPEDWINDOW , CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
 ShowWindow(hwnd, iCmdShow);
 UpdateWindow(hwnd);
 while (GetMessage(&msg, NULL, 0, 0))
 	{
    TranslateMessage(&msg);
    DispatchMessage(&msg);
   }
 return msg.wParam ;
}


void MakeStatus(HDC hdc, int cx, int cy)
{
 char dbg[150];
 int i;
 long double dtmp;
 char ctmp[200];
 long double c;
 long double ty;
 POINT apt[1024];
 HPEN hPen;
 long double table[1024];


 const int RESY = cy/8;
 const int RESX = cx/8;

 if (DEBUG)
 	{
    sprintf(dbg,"RESX=%d, RESY=%d, cx=%d, cy=%d, im=%f", RESX,RESY,cx,cy,(cast)im);
    MessageBox(NULL,TEXT(dbg), TEXT("Debug information!"), MB_OK);
   }
 sprintf(dbg, "F1 for help, IM=%f, f'=%d",(cast)im,another);
 TextOut(hdc, 1, 1, TEXT(dbg), strlen(dbg));
 sprintf(ctmp,"Center [%lf,%lf] ",cast((sx+ex)/2),cast((sy+ey)/2));
 TextOut(hdc, 1, 20, TEXT(ctmp), strlen(ctmp));

 MoveToEx(hdc, cx/2, cy, NULL);
 LineTo(hdc, cx/2, 0);
 MoveToEx(hdc, 0, cy/2, NULL);
 LineTo(hdc, cx, cy/2);
 for (i=0;i<RESY;i++)
 	{
    MoveToEx(hdc, cx/2-5,(cy*i)/RESY, NULL);
    LineTo(hdc, cx/2+5,(cy*i)/RESY);
    if (!(i%8) && (i))
    	{
       MoveToEx(hdc, cx/2-5,(cy*i)/RESY+1, NULL);
       LineTo(hdc, cx/2+5,(cy*i)/RESY+1);
       MoveToEx(hdc, cx/2-5,(cy*i)/RESY-1, NULL);
       LineTo(hdc, cx/2+5,(cy*i)/RESY-1);

       dtmp = ey - ((ey-sy)*i)/RESY;
       sprintf(ctmp, "%f", (cast)dtmp);
       TextOut(hdc, cx/2+15, (cy*i)/RESY-8, TEXT(ctmp), (strlen(ctmp)<=9)?(strlen(ctmp)) : 9);

      }

   }
 for (i=0;i<RESX;i++)
 	{
    MoveToEx(hdc, (cx*i)/RESX, cy/2-5, NULL);
    LineTo(hdc, (cx*i)/RESX, cy/2+5);
    if (!(i%12) && (i))
    	{
       MoveToEx(hdc, (cx*i)/RESX-1, cy/2-5, NULL);
       LineTo(hdc, (cx*i)/RESX-1, cy/2+5);
       MoveToEx(hdc, (cx*i)/RESX+1, cy/2-5, NULL);
       LineTo(hdc, (cx*i)/RESX+1, cy/2+5);

       dtmp = sx + ((ex-sx)*i)/RESX;
       sprintf(ctmp, "%f", (cast)dtmp);
       TextOut(hdc, (cx*i)/RESX-20, cy/2-22, TEXT(ctmp), (strlen(ctmp)<=9)?(strlen(ctmp)) : 9);

      }
   }

 MoveToEx(hdc, 0, cy/2, NULL);
 LineTo(hdc, cx,cy/2);
 for (c=0;c<=cx;c++)
 	{
    //ty = ( ( (function (sx + ((ex-sx)*c)/cx) )-ey)/(sy-ey ) )*cy;
    //ty = ( ((function((D)sx + (((D)ex-(D)sx)*c)/(D)cx))-(D)ey)/((D)sy-(D)ey) )*(D)cy;
    ty = (((function((D)sx + (((D)ex-(D)sx)*c)/(D)cx,im))-(D)ey)/((D)sy-(D)ey))*(D)cy;
    apt[(int)c].x = c;
    apt[(int)c].y = ty;
   }
 Polyline(hdc, apt, cx+1);
 if (another)
 	{
    for (int i=1;i<=another;i++)
      {
       if (i == 2) hPen = CreatePen(PS_SOLID, 1, RGB(0,0,0xff));
       if (i == 1) hPen = CreatePen(PS_SOLID, 1, RGB(0xff,0,0));
       SelectObject(hdc, hPen);
       for (c=0;c<=cx;c++)
 	      {
          ty = (((funcprim((D)sx + (((D)ex-(D)sx)*c)/(D)cx,im,i))-(D)ey)/((D)sy-(D)ey))*(D)cy;
          apt[(int)c].x = c;
          apt[(int)c].y = ty;
         }
       Polyline(hdc, apt, cx);
       DeleteObject(hPen);
      }
   }
  if (DEBUG)
 	{
    sprintf(dbg,"Drawed!");
    MessageBox(NULL,TEXT(dbg), TEXT("Debug information!"), MB_OK);
   }
}

void help()
{
 MessageBox(NULL,
 "This is version using Long Double\n"
 "Use arrows = move function\n"
 "PG-UP-DOWN = zoom-in-out\n"
 "HOME-END   = scale Y\n"
 "INS-DEL    = scale X \n"
 "ESC        = back to scale 1:1\n"
 "F1         = this help message\n"
 "F2         = about\n"
 "Press LButton on point to zoom-it\n"
 "Press RButton on point to zoom-out-it\n"
 "Press MButton on Point to get info\n"
 "If you do not have MButton press LButton and SHIFT\n"
 "Press +/- to change imaginated value im by +/-1\n"
 "Press >/< to change im value by +/- 4%\n"
 "Press p to toggle f'(x), f''(x)\n"
 "Black f(x), Red f'(x), Blue f''(x)\n"
 ,
 "Help context",
 0);
}

void ReactKey(WPARAM key)
{
 char ttt[100];
 long double tmp;
 char dbg[100];
 if (DEBUG)
 	{
    sprintf(dbg,"key_pressed = 0x%x, %c",key,key);
    MessageBox(NULL,TEXT(dbg), TEXT("Debug information!"), MB_OK);
   }
 if (key == 0x70) help();
 if (key == 0x50) {another ++; if (another > 2) another = 0;}
 if (key == 0x22) {sx -= (ex-sx)/20; sy -= (ey-sy)/20;ex += (ex-sx)/20; ey += (ey-sy)/20;}
 if (key == 0x21) {sx += (ex-sx)/40; sy += (ey-sy)/40;ex -= (ex-sx)/40; ey -= (ey-sy)/40;}
 if (key == 0x28) {tmp = (ey-sy)/100; sy += tmp; ey += tmp;}
 if (key == 0x26) {tmp = (ey-sy)/100; sy -= tmp; ey -= tmp;}
 if (key == 0x25) {tmp = (ex-sx)/100; sx += tmp; ex += tmp;}
 if (key == 0x27) {tmp = (ex-sx)/100; sx -= tmp; ex -= tmp;}
 if (key == 0x2d) {sx -= (ex-sx)/20;ex += (ex-sx)/20;}
 if (key == 0x24) {sy -= (ey-sy)/20;ey += (ey-sy)/20;}
 if (key == 0x2e) {sx += (ex-sx)/40;ex -= (ex-sx)/40;}
 if (key == 0x23) {sy += (ey-sy)/40;ey -= (ey-sy)/40;}
 if (key == 0x1b) {sx=-10;sy=-10;ex=10;ey=10;im=0.0;}
 if (key == 0xbb) im += (ex+ey-sx-sy)/100;
 if (key == 0xbd) im -= (ex+ey-sx-sy)/100;
 if (key == 0xbe) im *= 1.04;
 if (key == 0xbc) im *= 0.96;
 if (key == 0x71)
 	{
    MessageBox(NULL,TEXT("Function Drawer v0.1\nAlpha Release written in Borland C++\nby Morgoth DBMA\nEnjoy it (and don't forget to give it to others)"),TEXT("About the Program Function Draver v0.1"),MB_ICONASTERISK);
   }
}


void Stats(LPARAM lParam, int cx, int cy)
{
 long double x = (D)LOWORD(lParam);
 long double y = (D)HIWORD(lParam);
 long double tx = (D)sx + (((D)ex-(D)sx)*x)/(D)cx;
 long double ty = (D)ey - (((D)ey-(D)sy)*y)/(D)cy;
 long double rx = ex-sx;
 long double ry = ey-sy;
 long double val = function(tx,im);
 long double val2 = funcprim(tx,im,1);
 long double val3 = funcprim(tx,im,2);
 char tmp[1000];
 sprintf(tmp, "Get Point:\nMouse_x=%d, Mouse_y=%d\nClient_x=%d, Client_y=%d\n"
 "X=%f, Y=%f, F(X)=%f\n \nF'(X)=%f, F''(X) = %f\nscale_x=%lf scale_y=%lf\nImaginated=%f\nf'=%d\nCenter = <%f,%f>",
  (int)x,(int)y,cx,cy,(cast)tx,(cast)ty,(cast)val,(cast)val2,(cast)val3,(cast)rx,(cast)ry,(cast)im,another,cast((ex+sx)/2),cast((ey+sy)/2));
  MessageBox(NULL,TEXT(tmp),TEXT("Advanced Information!"), MB_ICONASTERISK);
}
ReactMouse(LPARAM lParam, int zoom, int cx, int cy)
{
 long double x = (D)LOWORD(lParam);
 long double y = (D)HIWORD(lParam);
 long double tx = (D)sx + (((D)ex-(D)sx)*x)/(D)cx;
 long double ty = (D)ey - (((D)ey-(D)sy)*y)/(D)cy;
 char tmp[100];
 if (DEBUG)
 	{
    sprintf(tmp,"REAL=<%f, %f>, ON_SCREEN=<%d,%d>",(cast)tx,(cast)ty,(int)x,(int)y);
    MessageBox(NULL,TEXT(tmp), TEXT("Debug information!"), MB_OK);
   }
 if (zoom)
 	{
 	 sx += (tx-sx)/10;
 	 sy += (ty-sy)/10;
 	 ex -= (ex-tx)/10;
 	 ey -= (ey-ty)/10;
 	}
 else
 	{
 	 sx -= (tx-sx)/5;
 	 sy -= (ty-sy)/5;
 	 ex += (ex-tx)/5;
 	 ey += (ey-ty)/5;
   }

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
 static int cx, cy;
 HDC hdc;
 int i;
 PAINTSTRUCT ps;
 switch (message)
 	{
    case WM_SIZE:
    	cx = LOWORD(lParam);
      cy = HIWORD(lParam);
      return 0;
    case WM_PAINT:
    	hdc = BeginPaint(hwnd, &ps);
      MakeStatus(hdc, cx, cy);
      return 0;
    case WM_KEYDOWN:
		ReactKey(wParam);
      InvalidateRect(hwnd, NULL, TRUE);
      return 0;
    case WM_LBUTTONDOWN:
      if (wParam & MK_SHIFT) Stats(lParam, cx, cy);
    	else ReactMouse(lParam, 1,cx,cy);
      InvalidateRect(hwnd, NULL, TRUE);
      return 0;
    case WM_RBUTTONDOWN:
    	ReactMouse(lParam, 0,cx,cy);
      InvalidateRect(hwnd, NULL, TRUE);
      return 0;
    case WM_MBUTTONDOWN:
    	Stats(lParam,cx,cy);
      InvalidateRect(hwnd, NULL, TRUE);
      return 0;
    case WM_DESTROY:
    if (DEBUG)
 	   {
       MessageBox(NULL,TEXT("Post Quit Messtage!"), TEXT("Debug information!"), MB_OK);
      }
    	PostQuitMessage(0);
      return 0;
   }
 return DefWindowProc(hwnd, message, wParam, lParam);
}

int far_zero(long double val, int e)
{
 if (!val) return 0;
 if (val < 0) val *=-1;
 if (val <= exp(-e)) return 0;
 else return 1;
}

long double funcprim(long double x, long double i, int prim)
{
 long double h = sqrt(sqrt((ex-sx)*(ey-sy)))*2.2e-5;
 if (prim <= 0) return function(x,i);
 else return (funcprim(x+h,i,prim-1)-funcprim(x,i,prim-1))*(1.0/h);
}

