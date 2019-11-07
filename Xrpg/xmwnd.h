/* This Library was written by Morgoth DBMA (Lukasz Gryglicki) morgothdbma@o2.pl
   Can be distributed under any GNU-OpenSource compatible license
   I have I condiction, if You use this lib in your program sign me as a one of
   authors and set this text into source "uses xmwnd.h lib by Morgoth DBMA"
   this library is now Alpha experimental version, You should have also:
   mh.h (console function) mran.h (my own random lib) mgraph.h (only few functions)
   Currently program is Optimized for 24-bit Display, to use with other
   Read comments of functions RGB & change its <to proper display 8,16,24,32>
   FOR THE GLORY OF ALL UNIX SYSTEMS
*/
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "mh.h"		//Morgothheader Lib (uses another file: mran.h)
#include "mgraph.h"     //Some graph functions (DrawHalfFrames itp.)
#define MOST_RESPONSES ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | Button1MotionMask | Button3MotionMask | Button2MotionMask | PointerMotionMask | StructureNotifyMask
#define LESS_RESPONSES ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask
#define MEDI_RESPONSES ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask | KeyReleaseMask | StructureNotifyMask
#define KEYB_RESPONSES ExposureMask | KeyPressMask | KeyReleaseMask | StructureNotifyMask
//Response tables defined to explain how much events window reacts on
#define MB_OK 0x12
#define MB_YES 0x13
#define MB_NO 0x14
#define MB_CANCEL 0x15
#define MB_RETRY 0x16                //MB_* mb (mousebutton) types of buttons
#define SHORT_TXT 90
#define MEDIUM_TXT 192
#define TINY_TXT 75
#define LONG_TXT 256
#define LONG_STR 512
#define PREDATE  32768
#define CHAR_PTR  0xada
#define INT_PTR    0x65
#define VOID_PTR  0xdead
#define LEAVE 0xeae
#define REMOVE 0xee1
#define DEFAULT 0xdefa
#define YES_ID   0x1
#define NO_ID    0x0
#define Q_PLUS 0xabc                //define windows quit when 'Q' pressed Yes/No
#define Q_MINUS 0xabd
#define NU_FUNC (void*)0
#define NUM_EVENTS    18
#define EV_PROC        0
#define EV_SETUP       1
#define EV_EXPOSE      2
#define EV_CONFIG      3
#define EV_EXPOSEX     4
#define EV_CONFIGX     5
#define EV_BPRESS      6
#define EV_BRELEASE    7
#define EV_BPRESSX     8
#define EV_BRELEASEX   9
#define EV_MOTION     10
#define EV_MOTIONX    11
#define EV_KPRESS     12
#define EV_KRELEASE   13
#define EV_KPRESSX    14
#define EV_KRELEASEX  15
#define EV_OTHER      16
#define EV_PARENTCALL 17
#define SHIFT_L   50
#define SHIFT_R   62
#define KEY_PGUP   99
#define KEY_PGDOWN 105
#define KEY_HOME    97
#define KEY_END    103
#define KEY_LEFT  100
#define KEY_RIGHT 102
#define KEY_UP    98
#define KEY_DOWN  104
#define KEY_TAB   23
#define CAX	  6
#define	CAY	  13
#define BIT24DISP
//#define BIT16DISP
//#define DBG 1                        //if You want LOTS (really!) of debug set it to non-zero
//#define NSL 1                        //if you want every function to slow by 10 milisec type 1 there
                                    //setting this to 1 VERY (really!) slow down the program execution
                                    //even on PV 4,5 GHz

void out(char* t)                    //debug is written into console
{
#ifdef DBG
 write(1, t, strlen(t));
 write(1,"\n",1);
#endif
#ifdef NSL
 usleep(10000);
#endif
}


void trace(char* t)                    //used debug time
{
 system("tput md 2>/dev/null");
 write(1,t,strlen(t));
 write(1,"\n", 1);
 usleep(10000);
 system("tput me 2>/dev/null");
}


void tout(char* t)                    //used debug time
{
#ifdef DBG
 write(1,"EXTERNAL>> ",11);
 write(1,t,strlen(t));
 write(1,"\n",1);
#endif
#ifdef NSL
 usleep(10000);
#endif
}


void panic(char* why)                          //called terminate program immaditely and writes PANIC!
{
 out("panic");
 printf("%c%c%c\n", 7, 7, 7);
 system("tput md 2>/dev/null");
 system("tput mb 2>/dev/null");
 printf("\n\n\nProgram Panic! Error Reported: %s\n", why);
 system("tput me 2>/dev/null");
 system("tput me 2>/dev/null");
 exit(-1);
}


void panic(char* why, int num, char* fil)                          //called terminate program immaditely and writes PANIC!
{
 out("panic");
 printf("%c%c%c\n", 7, 7, 7);
 system("tput md 2>/dev/null");
 system("tput mb 2>/dev/null");
 printf("\n\n\nProgram Panic! Error Reported: %s\n", why);
 printf("LINE=%d, FILE=%s\n", num,fil);
 system("tput me 2>/dev/null");
 system("tput me 2>/dev/null");
 exit(-1);
}

typedef double D;
typedef unsigned long int ulong;
typedef unsigned int uint;

inline unsigned long RGB(int r, int g, int b)  //return color from R,G,B (this is version for 24-bit color display)
{
#if defined BIT24DISP
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"RGB(%02x, %02x, %02x)", r,g,b);
 out(dstr);
#endif                                  //24 bit does 8r8g8b bit |********|********|********|                  //for 16bit is defined    |*****|******|*****|
 //r/=8; g/=4; b/=8;
 //return 0x800*(r%0x20)+0x20*(g%0x40)+(b%0x20);
 return 0x10000*(r%0x100)+0x100*(g%0x100)+(b%0x100);                                    //  red   green  blue
#elif defined BIT16DISP
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"RGB(%02x, %02x, %02x)", r,g,b);
 out(dstr);
#endif
 r/=8;
 g/=4;
 b/=8;
 return 0x800*(r%0x20)+0x20*(g%0x40)+(b%0x20);
#endif
}                                       // 5r5g5b  return 0x800*r+0x20*g+b;   /or something!/

unsigned long RGB(D rd, D gd, D bd)      //the same but converts from double
{
 out("rgb_double <<INNATURAL>>");
 int r = (int)rd;
 int g = (int)gd;
 int b = (int)bd;
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"RGB(%02x, %02x, %02x)", r,g,b);
 out(dstr);
#endif
#if defined BIT24DISP                   //24 bit does 8r8g8b bit |********|********|********|
 r %= 0x100;                            //                        red bits  green    blue
 g %= 0x100;
 b %= 0x100;                            //for 16bit is defined    |*****|******|*****|
 return 0x10000*r+0x100*g+b;                                    //  red   green  blue
#elif defined BIT16DISP
 r %= 0x20;
 g %= 0x40;
 b %= 0x20;
 return 0x800*r+0x20*g+b;
#endif
}


int ReturnRed(ulong col)                    //at this time RetColor only for 24 bit display!
{
#ifdef DBG
 char dstr[LONG_STR];
 uint uival = (uint)((0xff0000 & col)/0x10000);
 sprintf(dstr,"returnred(%x) will return %x", (uint)col, uival);
 out(dstr);
#endif
#ifdef BIT24DISP
 return (int)((0xff0000 & col)/0x10000);
#elif defined BIT16DISP
 return 8*(int)((0xf800 & col)/0x800);
#endif
}


int ReturnGreen(ulong col)
{
#ifdef DBG
 char dstr[LONG_STR];
 uint uival = (uint)((0x00ff00 & col)/0x100);
 sprintf(dstr,"returngreen(%x) will return %x", (uint)col, uival);
 out(dstr);
#endif
#ifdef BIT24DISP
 return (int)((0x00ff00 & col)/0x100);
#elif defined BIT16DISP
 return 4*(int)((0x7e0 & col)/0x20);
#endif
}


int ReturnBlue(ulong col)
{
#ifdef DBG
 char dstr[LONG_STR];
 uint uival = (int)(0x0000ff & col);
 sprintf(dstr,"returnblue(%x) will return %x", (uint)col, uival);
 out(dstr);
#endif
#ifdef BIT24DISP
 return (int)(0x0000ff & col);
#elif defined BIT16DISP
 return 8*(int)(0x1f & col);
#endif
}


struct Unused                          //this struct is used to return value from Child-Window
{
 Unused();
 ~Unused();
 void SetInt(int);
 void SetCharPtr(char*);               //currently can return int or char*
 void SetVoid(void*);
                                       //(you can sscanf char* whatever you want to) or add void* variable
 int ReturnInt() const;                //and SetVoidPtr(void*) ReturnVoidPtr() functions
 char* ReturnCharPtr() const;
 void* DefaultReturn() const;
 void* ReturnVoid()    const;
 int int_val;
 char* char_ptr_val;
 int type;
 void* other;                          //not yet handled
};

Unused :: Unused()
{
 out("Unused::Unused()");
 type = 0;                        //not yet known type, to use || void* add type #define VOID_PTR something
 char_ptr_val = NULL;
 int_val = 0;
 other = NULL;
}


Unused :: ~Unused()
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"~unused ==> char_ptr_val = %p", char_ptr_val);
 out(dstr);
#endif
 if (char_ptr_val) delete [] char_ptr_val;
}


void Unused :: SetInt(int par)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"unused::setint ==> setting %d", par);
 out(dstr);
#endif
 type = INT_PTR;
 int_val = par;
}


void Unused :: SetVoid(void* par)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"unused::setvoid ==> setting %p", par);
 out(dstr);
#endif
 if (!par) return;
 type = VOID_PTR;
 other = par;
}


void Unused :: SetCharPtr(char* par)
{
#ifdef DBG
 char dstr[LONG_STR];
 if (par) sprintf(dstr,"unused::setcharptr ==> setting %p:%s", par, par);
 else sprintf(dstr,"unused::setcharptr  ==> warning, trying to set NULL");
 out(dstr);
#endif
 type = CHAR_PTR;
 if (char_ptr_val) delete [] char_ptr_val;
 char_ptr_val = NULL;
 int len = strlen(par);
 char_ptr_val = new char[len+2];
 if (!char_ptr_val) panic("memory exception in unused::char_ptr_val");
 if (!par) {strcpy(char_ptr_val, "NO"); return;}
 strcpy(char_ptr_val, par);
}


int Unused :: ReturnInt() const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"unused::returnint, type=%d int_val=%d",type,int_val);
 out(dstr);
#endif
 if (type != INT_PTR) return -1;
 else return int_val;
}


void* Unused :: ReturnVoid() const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"unused::retvoid ==> returning %p, type is %d", other, type);
 out(dstr);
#endif
 if (type != VOID_PTR) return NULL;
 else return other;
}


char* Unused :: ReturnCharPtr() const
{
#ifdef DBG
 char dstr[LONG_STR];
 if (char_ptr_val) sprintf(dstr,"unused::returncharptr  ==> returning %p:%s, type=%d",char_ptr_val, char_ptr_val, type);
 else sprintf(dstr,"unused::returncharptr ==> warning, returning NULL");
 out(dstr);
#endif
 if (!char_ptr_val) out("warning in ReturnCharPtr: is NULL");
 if (type != CHAR_PTR) return NULL;
 else return char_ptr_val;
}


void* Unused :: DefaultReturn() const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"unused::defaultreturn: type=%d, int_val=%d, char_ptr_val=%p, other=%p",type, int_val, char_ptr_val, other);
 out(dstr);
#endif
 if (type == INT_PTR) return (void*)int_val;
 else if (type == CHAR_PTR) return (void*)char_ptr_val;
 else if (type == VOID_PTR) return (void*)other;
 else {out("waring in default return ,returning NULL, undefined type");return NULL;}
}

class Dsp                          //very important class Wnd deriveres from it Display
{                                  //this is the first class You'll use
 public:
 Dsp();
 Display* RetDsp()     const ;
 int      RetSNum()    const ;  //Globally Ret* means ret sth from class (mostly const)
 int      RetClientX() const ;
 int      RetClientY() const ;
 int      RetX()       const ;
 int      RetY()       const ;
 void     SetDsp(Display*) ;    //Globally set* set some variable of class
 void     SetSNum(int)     ;
 void     SetCx(int)       ;
 void     SetCy(int)       ;
 void     SetX(int)        ;
 void     SetY(int)        ;
 private:
 Display* dsp;                  //variables are private. Access denied from outside, only class-methods
 int s_num;                     //methods do some addictional things to avid sending SIGSEGV by the kernel
 int dx,dy;
 int x,y;
};
typedef class Wnd;              //forward of class Wnd (the biggest one)

class Scroll
{
 public:
 Scroll();
 Scroll(int, int);
 Scroll(int, int, int, int);
 ~Scroll();
 void SetScrlFunc(void (*)(Wnd&, void*));
 void SetScrlXY(int, int);
 void SetScrlLen(int);
 bool isDefault() const ;
 bool insideArea(int, int)  const ;
 void SetDefault(bool);
 void SetScrlPos(int);
 void SetMaxPos(int);
 void SetVoid(void*);
 void GetPosFromArea(int);
 void SetScrlFocus(bool);
 int   RetSx()    const ;
 int   RetSy()    const ;
 int   RetLen()   const ;
 int   RetPos()   const ;
 int   RetMaxPos()const ;
 void* RetVoid()  const ;
 bool  isSelected() const ;
 void (*scrlFunc)(Wnd&, void*);
 private:
 int sx, sy;
 int len;
 int pos;
 int max_pos;
 bool def;
 void* data;                            //if set used by KeyPress func
 bool focus;
 void call_std_init();
};
Scroll :: Scroll(){out("scroll::scroll");call_std_init();}

Scroll :: Scroll(int x, int y)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"scroll:;scroll(%d, %d) to set sx,sy", x, y);
 out(dstr);
#endif
 call_std_init();
 sx=x;
 sy=y;
}


Scroll :: Scroll(int xl, int yl, int lenl, int max_posl)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"scroll::scroll(%d,%d,%d,%d) to set sx,sy,len,max_pos",xl,yl,lenl,max_posl);
 out(dstr);
#endif
 call_std_init();
 sx=xl; sy=yl;
 len=lenl; max_pos = max_posl;
}

Scroll :: ~Scroll(){out("scroll::~scroll scrlFunc=NULL");scrlFunc = NULL;}

void Scroll :: SetScrlFunc(void (*f)(Wnd&, void*))
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"scroll::setscrlfunc ==> seting_func %p",f);
 out(dstr);
#endif
 if (f) scrlFunc = f;
 else out("warning in scroll::setscrlfunc: cann't set NULL func!");
}


void Scroll :: SetScrlXY(int x, int y)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"scroll::setscrlxy(%d,%d)",x,y);
 out(dstr);
#endif
 if (x>0) sx=x;
 if (y>0) sy=y;
}


void Scroll :: SetScrlLen(int l)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"scroll::setscrllen(%d)",l);
 out(dstr);
#endif
 if (len>0) len = l;
}


bool Scroll :: isDefault() const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"scroll::isdefault (%d)", def);
 out(dstr);
#endif
 return def;
}


bool Scroll :: insideArea(int x , int y)  const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"scroll::insidearea(%d,%d)",x,y);
 out(dstr);
#endif
 if ((x>=sx-10) && (x<=sx+10) && (y>=sy) && (y<=sy+len)) return true;
 else return false;
}


void Scroll :: SetDefault(bool par)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"scroll::setdefault(%d)", par);
 out(dstr);
#endif
 def = par;
}


void Scroll :: SetVoid(void* ptr)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"scroll::setvoid(%p)",ptr);
 out(dstr);
#endif
 if (ptr) data = ptr;
}


void Scroll :: SetScrlFocus(bool parm)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"scroll::setscrlfocus(%d)",parm);
 out(dstr);
#endif
 focus = parm;
}


void Scroll :: SetScrlPos(int par)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"scroll::setscrlpos(%d)",par);
 out(dstr);
#endif
 if (par < 0) par = 0;
 if (par > max_pos) par = max_pos;
 pos = par;
}


void Scroll :: GetPosFromArea(int ar)
{
 #ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"scroll::getposfromarea(%d)",ar);
 out(dstr);
#endif
 int offset = (sy + len) - ar;
 pos = (offset*max_pos)/len;
 if (pos<0) pos = 0;
 if (pos>max_pos) pos=max_pos;
#ifdef DBG
 sprintf(dstr,"Got: pos=%d", pos);
 out(dstr);
#endif
}


void Scroll :: SetMaxPos(int par)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"scroll::setmaxpos(%d)",par);
 out(dstr);
#endif
 if (par>0) max_pos = par;
}


int   Scroll :: RetSx()      const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"scroll::retsx ==> %d", sx);
 out(dstr);
#endif
 return sx;
}


int   Scroll :: RetSy()      const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"scroll::retsy ==> %d", sy);
 out(dstr);
#endif
return sy;
}


int   Scroll :: RetLen()     const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"scroll::relen ==> %d",len);
 out(dstr);
#endif
 return len;
}


int   Scroll :: RetPos()     const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"scroll::retpos ==> %d", pos);
 out(dstr);
#endif
 return pos;
}


int   Scroll :: RetMaxPos()  const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"scroll::retmaxpos ==> %d",max_pos);
 out(dstr);
#endif
 return max_pos;
}


bool  Scroll :: isSelected() const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"scroll::isselected ==> %d", focus);
 out(dstr);
#endif
 return focus;
}


void* Scroll :: RetVoid()    const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"scroll::retvoid ==> %p", data);
 out(dstr);
#endif
 if (data) return data;
 else {out("warning in scrl::retvoid, returning NULL");return NULL;}
}


void Scroll :: call_std_init()
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"scroll::call_std_init, sx=100, sy=10, len=128, pos=0, max_pos=256,def=0, data=0x0");
 out(dstr);
#endif
 sx = 100;
 sy = 10;
 len = 128;
 pos = 0;
 max_pos = 0x100;
 def = false;
 data = NULL;
 focus = false;
 scrlFunc = NULL;               //very Important (1h searching for cause for SIGSEGV!)
}


class Button
{
 public:                                 //this is button and its abilities
 Button();                                //various constructors
 Button(int, int);                        //the less params the more default button appear
 Button(int, int, int, int);
 Button(char*);
 Button(char*, int, int);
 Button(char*, int, int, int, int);
 ~Button();
 void SetBtnName(char*);
 void SetBtnFunc(void (*)(Wnd&, void*));        //set function of button must be like this: void* is wrapper for ANYTHING! by pointer cast :)
 void SetBtnPos(int, int);               // void FuncButton(Wnd& window) <this template functions address accepted>
 void SetBtnSize(int, int);
 bool isSelected()  const ;              //means button is just pressed at the moment
 bool isDefault() const ;                //default means if you press ENTER this button will do action
 bool insideArea(int, int)  const ;      //is mouse pointer in button area?
 void SetDefault(bool);
 void SetBtnFocus(int);                  //focus <==> pressed
 void SetVoid(void*);
 char* RetText() const ;
 int   RetSx()   const ;
 int   RetEx()   const ;
 int   RetX()    const ;
 int   RetSy()   const ;
 int   RetEy()   const ;
 int   RetY()    const ;
 void* RetVoid() const ;
 void (*btnFunc)(Wnd&, void*);                  //own func you can add any param by cast to void*
 private:
 void call_std_init();                   //this is called by the constructor
 char* text;
 bool focus;
 int sx, sy;
 int x,y;
 bool def;
 void* data;                            //if set used by KeyPress func
};

void Button :: call_std_init()
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Button::call_std_init sx=360,sy=290,text=new[SHORT_STR], focus=0,x=80,y=25,btnFunc=0x0");
 out(dstr);
#endif
 text = new char[SHORT_TXT];
 if (!text) panic("memory exception in Button::text");
 focus = false;
 sx = 360;
 sy = 290;
 x = 80;
 y = 25;
 btnFunc = NULL;
 def = false;
 data = NULL;
}

//constructors

Button :: Button()
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"button::button");
 out(dstr);
#endif
 call_std_init();
}


Button :: Button(int sa, int sb)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"button::button(%d,%d) sx,sy",sa,sb);
 out(dstr);
#endif
 call_std_init(); sx=sa; sy=sb;
}


Button :: Button(int sa, int sb, int a, int b)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"button::button(%d,%d,%d,%d) sx,sy,x,y",sa,sb,a,b);
 out(dstr);
#endif
 call_std_init(); sx=sa; sy=sb; x=a; y=b;
}


Button :: Button(char* nam)
{
#ifdef DBG
 char dstr[LONG_STR];
 if (nam) sprintf(dstr,"button::button(%p:%s) text",nam,nam);
 else sprintf(dstr,"button::button((char*)NULL)");
 out(dstr);
#endif
 call_std_init(); if (nam) strcpy(text, nam); else strcpy(text,"NO");
}


Button :: Button(char* nam, int sa, int sb)
{
#ifdef DBG
 char dstr[LONG_STR];
 if (nam) sprintf(dstr,"button::button(%p:%s,%d,%d) text,sx,sy",nam,nam,sa,sb);
 else sprintf(dstr,"button::button((char*)NULL,%d,%d) sx,sy", sa, sb);
 out(dstr);
#endif
 call_std_init(); sx=sa; sy=sb;if (nam) strcpy(text, nam); else strcpy(text,"NO");
}


Button :: Button(char* nam, int sa, int sb, int a, int b)
{
#ifdef DBG
 char dstr[LONG_STR];
 if (nam) sprintf(dstr,"button::button(%p:%s,%d,%d,%d,%d) text,sx,sy,x,y",nam,nam,sa,sb,a,b);
 else sprintf(dstr,"button::button((char*)NULL,%d,%d,%d,%d) sx,sy,x,y",sa,sb,a,b);
 out(dstr);
#endif
 call_std_init(); sx=sa; sy=sb; x=a; y=b;if (nam) strcpy(text, nam); else strcpy(text,"NO");
}


Button :: ~Button()
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"button::~button ==> killing text if exists");
 out(dstr);
#endif
 if (text) delete [] text;
 btnFunc = NULL;
}


void Button :: SetBtnName(char* name)
{
#ifdef DBG
 char dstr[LONG_STR];
 if (name) sprintf(dstr,"button::setbtnname(%p:%s)",name,name);
 else sprintf(dstr,"button::setbtnname((char*)NULL)");
 out(dstr);
#endif
 if (text) strcpy(text, name); else out("no text field initialized, panic??");
}


void Button :: SetDefault(bool arg)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"button::setdefault(%d)",arg);
 out(dstr);
#endif
 def = arg;
}


void Button :: SetBtnFunc(void (*func)(Wnd&, void*))
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"button::setbtnfunc(%p)",func);
 out(dstr);
#endif
 if (func) btnFunc = func; else out("warning in button::setbtnfunc: about to set NULL, givin' up!");
}


void Button :: SetBtnPos(int x, int y)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"button::setbtnpos(%d,%d)",x,y);
 out(dstr);
#endif
 sx = x; sy = y;
}


void Button :: SetBtnSize(int xx, int yy)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"button::setbtnsize(%d,%d)",xx,yy);
 out(dstr);
#endif
 x = xx; y = yy;
}


bool Button :: isSelected() const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"button::issellected  ==> %d",focus);
 out(dstr);
#endif
 return focus;
}


bool Button :: isDefault() const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"button::isdefault ==> %d",def);
 out(dstr);
#endif
 return def;
}


void Button :: SetBtnFocus(int par)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"button::setbtnfocus(%d)",par);
 out(dstr);
#endif
 focus = par;
}


void Button :: SetVoid(void* ptr)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"button::setvoid(%p)", ptr);
 out(dstr);
#endif
 if (ptr) data = ptr; else out("warning in btn::setvoidptr, cannot set data = (void*)(NULL)");
}


char* Button :: RetText() const
{
#ifdef DBG
 char dstr[LONG_STR];
 if (text) sprintf(dstr,"button::rettext ==> %p:%s",text,text);
 else sprintf(dstr,"button::rettext ==> (char*)NULL");
 out(dstr);
#endif
 if (text) return text; else {out("warning, btn.text returning NULL");return NULL;}
}


int   Button :: RetSx()   const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"button::retsx ==> %d",sx);
 out(dstr);
#endif
 return sx;
}


int   Button :: RetEx()   const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"button::retex ==> %d",sx+x);
 out(dstr);
#endif
 return sx+x;
}


int   Button :: RetX()    const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"button::retx ==> %d",x);
 out(dstr);
#endif
 return x;
}


int   Button :: RetSy()   const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"button::retsy ==> %d",sy);
 out(dstr);
#endif
 return sy;
}


int   Button :: RetEy()   const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"button::retey ==> %d",sy+y);
 out(dstr);
#endif
 return sy+y;
}


int   Button :: RetY()    const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"button::rety ==> %d",y);
 out(dstr);
#endif
 return y;
}


void* Button :: RetVoid() const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"button::retvoid  ==> %p", data);
 out(dstr);
#endif
 if (data) return data;else  return NULL;
}


bool  Button :: insideArea(int xx, int yy)  const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"button::insidearea(%d,%d)", xx, yy);
 out(dstr);
#endif
 if ((xx>=sx) && (xx<=sx+x) && (yy>=sy) && (yy<=sy+y)) return true;
 else return false;
}


class Wbox
{
 public:                            //class Wbox (WriteBox) used when you write in window and returns your
                                    //string when you press ENTER or ESC
 Wbox(int, int , int, int, int);    //constructor
 ~Wbox();
 int RetSx()  const ;
 int RetEx()  const ;
 int RetX()   const ;
 int RetSy()  const ;
 int RetEy()  const ;
 int RetY()   const ;
 char* RetText() const ;
 int RetTextLen() const ;
 void SetSx(int);
 void SetSy(int);
 void SetX(int);
 void SetY(int);
 void SetMaxLen(int);
 void SetHidden(int);
 void SetShiftState(bool);  //set the shift state
 bool RetShiftState() const;      //return shift state
 void ReactOnKey(int);      //
 bool HaveText() const;           //is there any text?
 bool isHidden() const;
 private:
 int sx, sy;        //wbox is all-key-press refreshed max_len is maximum text length
 int x,y;           //len is current length data is the text, current is current letter
 int len;           //Shift tells whatever shift is pressed or not
 int max_len;
 char* data;
 bool Shift;
 int current;
 int hidden;
};

Wbox :: Wbox(int mlen, int xx, int yy, int xl, int yl)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"wbox::wbox(%d,%d,%d,%d,%d) maxlen,sx,sy,x,y",mlen,xx,yy,xl,yl);
 out(dstr);
#endif
 sx = xx;
 sy = yy;
 x = xl;
 y = yl;
 max_len = mlen;
 len = 0;
 data = NULL;
 data = new char[mlen+3];
 if (!data) panic("memory exception in wbox::data[]");
 for (int i=0;i<mlen;i++) data[i] = '\0';
 Shift = false;
 current = 0;
 hidden = 0;
}


void Wbox :: SetMaxLen(int le)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"wbox::setmaxlen(%d)",le);
 out(dstr);
#endif
 if (data) delete [] data;
 data = NULL;
 data = new char[le+3];
 if (!data) panic("memory exception in wbox::data[]");
 for (int i=0;i<le;i++) data[i] = '\0';
 max_len = le;
}


void Wbox :: SetHidden(int p)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"wbox::sethidden(%d)",p);
 out(dstr);
#endif
 hidden = p;
}


bool Wbox :: isHidden() const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"wbox::ishidden ==> %d",hidden);
 out(dstr);
#endif
 return hidden;
}


Wbox :: ~Wbox()
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"wbox::!wbox, deleting data if exists");
 out(dstr);
#endif
 if (data) delete [] data;
 max_len = 0;
}


bool Wbox :: HaveText() const
{
#ifdef DBG
 char dstr[LONG_STR];
 if ((data) && (len)) sprintf(dstr,"wbox::havetext ==> 1");
 else sprintf(dstr,"wbox::havetext ==> 0");
 out(dstr);
#endif
 if ((data) && (len)) return true;
 else return false;
}


void Wbox :: SetShiftState(bool p)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"wbox::setshiftstate(%d)",p);
 out(dstr);
#endif
 this->Shift = p;
}


bool Wbox:: RetShiftState() const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"wbox::retshiftstate ==> %d",Shift);
 out(dstr);
#endif
 return this->Shift;
}


int Wbox :: RetSx()  const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"wbox::retsx ==> %d",sx);
 out(dstr);
#endif
 return this->sx;
}


int Wbox :: RetEx()  const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"wbox::retex ==> %d",sx+x);
 out(dstr);
#endif
 return this->sx+this->x;
}


int Wbox :: RetX()  const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"wbox::retx ==> %d",x);
 out(dstr);
#endif
 return this->x;
}


int Wbox :: RetSy()  const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"wbox::retsy ==> %d",sy);
 out(dstr);
#endif
 return this->sy;
}


int Wbox :: RetEy()  const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"wbox::retey ==> %d",sy+y);
 out(dstr);
#endif
 return this->sy+this->y;
}


int Wbox :: RetY()  const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"wbox::rety ==> %d",y);
 out(dstr);
#endif
 return this->y;
}


char* Wbox :: RetText()   const
{
#ifdef DBG
 char dstr[LONG_STR];
 if (data) sprintf(dstr,"wbox::rettext ==> %p:%s",data,data);
 else sprintf(dstr,"wox::rettext ==> (char*)NULL");
 out(dstr);
#endif
 if (data) return this->data; else {out("in wbox::data, warning returning NULL"); return NULL;}
}


int Wbox :: RetTextLen()   const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"wbox::rettextlen ==> %d",len);
 out(dstr);
#endif
 return this->len;
}


void Wbox :: SetSx(int par)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"wbox::setsx(%d)",par);
 out(dstr);
#endif
 sx = par;
}


void Wbox :: SetX(int par)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"wbox::setx(%d)",par);
 out(dstr);
#endif
 x = par;
}


void Wbox :: SetSy(int par)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"wbox::setsy(%d)",par);
 out(dstr);
#endif
 sy = par;
}


void Wbox :: SetY(int par)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"wbox::sety(%d)",par);
 out(dstr);
#endif
 y = par;
}


void Wbox :: ReactOnKey(int key)     //main react function
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"wbox::reactonkey(%d:%c)",key,(char)key);
 out(dstr);
#endif
 if (current < 0) current = 0;
 if (!data) {out("warning: data field is not initialized, cannot write to NULL!");return;}
 if (key == ESCAPE)
 {
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"ESCAPE pressed, returning len=0, text=(char*)NULL");
 out(dstr);
#endif
  strcpy(data, ""); len = 0;
 }                                                         //return EMPTY (NULL) when ESC pressed
 else if ((key == SHIFT) || (key == INVALID_KEY))
 {
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"SHIFT or INVALID_KEY pressed, ignoring it!");
 out(dstr);
#endif
  return;
 }                                                         //ignore SHIFT and Other keys
 else if (key == ENTER)                                    //terminate string '\0' when ENTER
  {
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"ENTER pressed, EOF editing, will return current");
 out(dstr);
#endif
   data[current] = '\0';
   len = strlen(data);
  }
 else if (key == BACKSPACE)
  {                                                        //if BACKSPACE remove last character if non-zero len
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"BACKSPACE pressed, removing letters...");
 out(dstr);
#endif
   if (current<=0) return;
   current--;
   data[current] = '\0';
   len = strlen(data);
  }
 else
  {
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"NORMAL_LETTER find, adding");
 out(dstr);
#endif
   if (current >= max_len-1) return;                      //else add character and '\0' after it
   data[current] = (char)key;                             //if current != max_length
   data[current+1] = '\0';
   current++;
   len = strlen(data);
  }
}

class CheckBox
{                                                       //this is in fact only virtual "thing"
 public:                                                //manages buttons scroll buttons and Up/Down Buttons
 CheckBox(char**, int, int);
 CheckBox(char**, int);
 ~CheckBox();
 void UpScroll();                     //change scroll state (but all work does Wnd::DrawCheckbox()
 void DownScroll();
 void UpTenScroll();                  //Up-DownTen means to scroll by max_len
 void DownTenScroll();
 int RetScrollDown() const ;
 int RetScrollPeak() const ;
 char* RetName(int)  const ;
 private:
 int num;
 char** data;
 int scroll_pos;                  //actual scroll
 int scroll_max;                  //how much display at the time
};

CheckBox :: CheckBox(char** dane, int lb)   //constructor uses char** table, SHOULD THIS BE INITIALIZED FIRST
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"checkbox::checkbox((char**)%p,%d) data,lb",dane,lb);
 out(dstr);
 for (int i=0;i<lb;i++)
   {
    if (dane[i]) sprintf(dstr,"(char**)[%d] = %p:%s",i,dane[i],dane[i]);
    else sprintf(dstr,"(char**)[%d] = NULL (warning, this may cause panic or SIGSEGV)", i);
    out(dstr);
   }
#endif
 // or somewhere can be SIGSEGV
 if (!dane) return;
 data = NULL;
 if (data) delete [] data;
 data = new char*[lb+1];
 if (!data) panic("memory exception in checkbox::data");
 for (int i=0;i<lb;i++) {data[i] = new char[SHORT_TXT];if (!data[i]) panic("memory exception in data[]");}
 num = lb;
 scroll_pos = 0;
 scroll_max = 6;
 if (!dane) return;
 for (int i=0;i<lb;i++) if (dane[i]) strcpy(data[i], dane[i]); else {out("warning in checkbox::checkbox: attempting to copy from NULL char** element of table[]");return;}
}


CheckBox :: CheckBox(char** dane, int lb, int mscr)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"checkbox::checkbox((char**)%p,%d,%d) data,lb,max_scroll",dane,lb,mscr);
 out(dstr);
 for (int i=0;i<lb;i++)
   {
    if (dane[i]) sprintf(dstr,"(char**)[%d] = %p:%s",i,dane[i],dane[i]);
    else sprintf(dstr,"(char**)[%d] = NULL (warning, this may cause panic or SIGSEGV)", i);
    out(dstr);
   }
#endif
 if (!dane) return;
 data = NULL;
 if (data) delete [] data;
 data = new char*[lb+1];
 if (!data) panic("memory exception in data");
 for (int i=0;i<lb;i++) {data[i] = new char[SHORT_TXT];if (!data[i]) panic("memory exception in data[]");}
 num = lb;
 scroll_pos = 0;
 scroll_max = mscr;
 if (!dane) return;
 for (int i=0;i<lb;i++) if (dane[i]) strcpy(data[i], dane[i]); else return;
}


void CheckBox :: UpScroll()
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"checkbox::upscroll current_scrl_pos=%d", scroll_pos);
 out(dstr);
#endif
 scroll_pos++;
 if (scroll_pos > num-scroll_max) scroll_pos = num-scroll_max;
 if (scroll_pos < 0) scroll_pos = 0;
}


void CheckBox :: DownScroll()
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"checkbox::downscroll current_scrl_pos=%d", scroll_pos);
 out(dstr);
#endif
 scroll_pos--;
 if (scroll_pos < 0) scroll_pos = 0;
}


void CheckBox :: UpTenScroll()
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"checkbox::uptenscroll current_scrl_pos=%d", scroll_pos);
 out(dstr);
#endif
 scroll_pos+=(scroll_max-1);
 if (scroll_pos > num-scroll_max) scroll_pos = num-scroll_max;
 if (scroll_pos < 0) scroll_pos = 0;
}


void CheckBox :: DownTenScroll()
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"checkbox::downtenscroll current_scrl_pos=%d", scroll_pos);
 out(dstr);
#endif
 scroll_pos-=(scroll_max-1);
 if (scroll_pos < 0) scroll_pos = 0;
}


int   CheckBox :: RetScrollDown() const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"checkbox::retscrolldown ==> %d",scroll_pos);
 out(dstr);
#endif
 return scroll_pos;
}


int   CheckBox :: RetScrollPeak() const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"checkbox::retscrollpeak ==> %d (num=%d,scrl_pos=%d,scrl_max=%d)",(((scroll_pos + scroll_max) < num) ? (scroll_pos + scroll_max) : num),num,scroll_pos,scroll_max);
 out(dstr);
#endif
 return ((scroll_pos + scroll_max) < num) ? (scroll_pos + scroll_max) : num;}
char* CheckBox :: RetName(int n)  const {out("checkbox::retname(i)");    if ((data) && (data[n])) return data[n]; else {out("warning in checkbox:retname, returning NULL");return NULL;}}

CheckBox :: ~CheckBox()
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"checkbox::~checkbox, deleting (char**)data if all elements exists");
 out(dstr);
#endif
 if (data) for (int  i=1;i<num;i++) if (data[i]) delete [] data[i];  // deleting from 1 deleta [][] will (???)
 if (data) delete [] data;                                           // delete [0] (???)
}                                          //FIXME   IS_IT_CORRECT ? ?? ???

Dsp :: Dsp()              //this is the GREAT dsp-constructor
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"dsp::dsp s_num = 0, XOpenDisplay will be called immadiately!");
 out(dstr);
#endif
                                                 //init
 dsp = NULL;
 s_num = 0;
 dx = dy = x = y = 0;                             //set
 dsp = XOpenDisplay(NULL);
 if (!dsp) {printf("X-server error\n"); panic("unable to open graphic display");}
 s_num = DefaultScreen(dsp);
 dx = DisplayWidth(dsp, s_num);
 dy = DisplayHeight(dsp, s_num);
}


int      Dsp :: RetX()       const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"dsp::retx ==> %d",x);
 out(dstr);
#endif
 return this->x;
}


int      Dsp :: RetY()       const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"dsp::rety ==> %d",y);
 out(dstr);
#endif
 return this->y;
}


int      Dsp :: RetClientX() const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"dsp::retclientx ==> %d",dx);
 out(dstr);
#endif
 return this->dx;
}


int      Dsp :: RetClientY() const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"dsp::reclienty ==> %d",dy);
 out(dstr);
#endif
 return this->dy;
}


int      Dsp :: RetSNum()    const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"dsp::retsnum ==> %d",s_num);
 out(dstr);
#endif
 return this->s_num;
}


Display* Dsp :: RetDsp()     const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"dsp::retdsp ==> %p", dsp);
 out(dstr);
#endif
 if (!dsp) out("dsp::redsp warning, returning NULL display!");return this->dsp;
}


void     Dsp :: SetDsp(Display* disp)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"dsp::setdsp(%p) dangerous!", disp);
 out(dstr);
#endif
 if (disp)this->dsp = disp;  else out("warning in dsp::setdsp: Cannot set NULL!");
}


void     Dsp :: SetSNum(int num)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"dsp::setsnum(%d)",num);
 out(dstr);
#endif
 this->s_num = num;
}


void     Dsp :: SetCx(int par)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"dsp::setsx(%d)", par);
 out(dstr);
#endif
 this->dx = par;
}


void     Dsp :: SetX(int par)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"dsp::setx(%d)", par);
 out(dstr);
#endif
 this->x = par;
}


void     Dsp :: SetCy(int par)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"dsp::setsy(%d)", par);
 out(dstr);
#endif
 this->dy = par;
}


void     Dsp :: SetY(int par)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"dsp::sety(%d)", par);
 out(dstr);
#endif
 this->y = par;
}

class ClipArea;

class Wnd                                     //great class Wnd
{
 public:
 Wnd(Dsp*);                                   //user must define lots things...
 Wnd(Dsp*, int, int);                         //size initially defined
 Wnd(Dsp*, char*, int);                       //by default creates Mbox with MB_OK or other Button
 Wnd(Dsp*, char*, int, int, int, int, int);   //create Mbox with User-defined Button
 ~Wnd();                                      //destuctor
 int RetCx()                 const ;
 int RetCy()                 const ;
 int RetRx()                 const ;
 int RetRy()                 const ;
 bool isDone()               const ;           //when true windows makes RITUAL SUICIDE :)
 Dsp* RetDspStr()            const ;           //returns Display* ptr
 GC RetGC()                  const ;
 XFontStruct* RetFontInfo()  const ;
 int RetFontH()              const ;
 Display* RetDisp()          const ;
 XEvent RetEvent()           const ;           //reurn XEvent struct
 long  WhatHappened()        const ;           //return event
 int RetX()                  const ;
 int RetY()                  const ;
 int RetDrag()               const ;
 bool WantPaletteInfo()      const ;
 Window RetWin()             const ;
 int RetLines()              const ;
 char* RetCaption()          const ;
 char** RetWndText()         const ;
 int RetNumBtns()            const ;
 Button* RetBtn(int)         const ;
 char* RetBtnName(int)       const ;
 bool validBtn(int)          const ;
 bool validScrl(int)         const ;
 Unused*  RetDta()           const ;
 int   ReturnInt()           const ;
 char* ReturnCharPtr()       const ;
 void* ReturnVoid()          const ;
 Wbox* ReturnWriteBox()      const ;
 CheckBox* ReturnCheckBox()  const ;
 Wnd* ReturnChild()          const ;
 Wnd* ReturnParent()         const ;
 int RetPressed()            const ;
 void* RetEventData(int)     const ;
 bool HaveFrames()           const ;
 bool HaveGraph()            const ;
 ulong RetWndPixel(int, int) const ;
 ulong RetCurColor()         const ;
 bool HavePalette()          const ;
 int RetNumScrl()            const ;
 bool HaveScroll()           const ;
 bool HaveClipArea()	     const ;
 ClipArea* RetClipArea()     const ;
 Scroll* RetScroll(int)      const ;
 inline bool StartedDrawing()const ;
 void SetButtonName(int, char*);
 void CreateGC();
 void DeclareResponseTable(long int);               //declare table of window responses (see #defined responses)
 void SetFont(char*);
 void SelectProcFunction(void* (*)(Wnd&, void*));         //you can declare Full External Proc function (much of coding)
 void AddictionalSetup(void (*)(Wnd&, void*));            //if declared will be called just before message-loop
 void* DefaultResponse();                           //THIS IS MSG_LOOP function default WndProc
 void SetCaption(char*);                            //You should declare standard responses by usigg
 void DoQuit();                                     //Select*Func fe selectButtonPressFunc(FuncName)
 void WindowDies();                                 //default is eXtendedButtonPress (will call ButtonPressXFunc)
 void DrawButtons();                                //if exists... WindowsDies possibly have BUG FIXME!
 void DrawClipArea();
 void DrawBtn(int num);
 void NextDefault();                                //Next-Prev changes default button to next-prev
 void SelectNextScroll();
 void PrevDefault();
 void CharPtrReturn();                              //Sets Unused struct to listen CharPtr and remember
 void IntPtrReturn();                               //and after defaultresponse return as char*
 void VoidPtrReturn();
 void CreateMBoxChild(char*, int, int, int, int, int);              //creates information with OK Button
 int CreateYesNoChild(char*, int, int, int, int, int, int, int);         //Creates Information with YES-NO buttons returns 1-0
 char* CreateWriteBox(char*, bool, int, int ,int);                        //Creates WriteBox and returns user written text
 char* CreateLWriteBox(char*, bool, int, int ,int);                        //Creates WriteBox and returns user written text
 char* CreateExWriteBox(char*, bool, int, int ,int, int, int, int, int);  //More Compicated and user defined (no DEFAULTS)
 void SelectDefaults(char*, char*, int);                            //Set up default window VERY! DEFAULT! (caption & text and Q_REACTION)
 void WinDefaults(char*);
 void DefaultWriteBox(int);                                   //Q_REACTION(int) Q_PLUS <end on 'q' key>
 void HiddenWriteBox(int);
 void DrawWBox();                                             // Q_MINUS <do_not_this>
 void WboxActivateShift(bool);                                //DefaultWBox fills down-par of the window
 bool WboxShiftPressed();                                    //set-return shift state of WriteBox
 void WboxKeyAction(int);                                    //react on key <WBox reaction>
 void WboxCreateReturnValue();                               //Send int to Unused* struct & prepare it to return after DefaultResponse
 void SetExWriteBox(int, bool, int, int, int, int);                //Extended WBox (no DEFAULTS)
 char* CreateCheckBoxChild(char*, char**, int, int, int, int);  //create Child-Window CheckBox <most code in DrawCheckBox>
 char* CreateCheckBoxVert(char*, char**, int, int, int, int);
 char* CreateExCheckBoxVert(char*, char**, int, int, int, int, int, Button**); //you can add own buttons by last 2 params
 void DefaultCheckBox(char**, int, int);                        //DefaultCheckBox char** selections int lines int scroll
 void HugeCheckBox(char**, int, int);
 void UserCheckBox(char**, int, int, int, Button**);            //Giv new buttons by [][] table Button** and int number
 void SetUnusedCharPtr(char*);                                   //lines how much, scroll how much max on screen at the time
 void DrawCheckBox();                                            //SetUnusedCharPtr bridge beetween classes Wnd <=> Unused <=> Func(Wnd&)
 bool HaveCheckBox()  const;                                     //DrawCheckBox actualizes Buttons beyond Up-Down Button using scroll func from CheckBox*
 void UpCheckBox();
 void DownCheckBox();                                           //up-down scroll and call DrawCheckBox()
 void UpTenCheckBox();
 void DownTenCheckBox();
 char* RetSelectedButtonName() const;                            //Check all Buttons, if found selected returns its name
 void SetDefaultButton(int);
 void SetPressed(int);                                           //set that int button is pressed
 void CancelPressed();                                           //no button is pressed
 void BlackWings();                                              //this is slow func
 void FadeClip();
 void SetPressedND(int);                                           //set that int button is pressed
 void CancelPressedND();                                           //no button is pressed
 void DrawScrolls();
 void DrawScrl(int);
 void DrawFadedScrl(int);
 void FadeScroll(int);
 void SetPaletteInfo(bool);
 void ForceUnmap();
 void SelectExposeFunc(void (*)(Wnd&, void*));                         //This section is to select ResponseFunctions
 void SelectConfigureFunc(void (*)(Wnd&, int, int, void*));            //react on events, selected functions must be
 void SelectButtonPressFunc(void (*)(Wnd&, int, int, int, void*));     //on given prototype
 void SelectButtonReleaseFunc(void (*)(Wnd&, int, int, int, void*));   //Press & PressX (when as Press selected eXtendedButtonPress
 void SelectButtonPressXFunc(void (*)(Wnd&, int, int, int, void*));    //this will do default checks for Buttons, CheckBoxes & other
 void SelectButtonReleaseXFunc(void (*)(Wnd&, int, int, int, void*));  // and then call ButtonPressXFunc <this is the kind of
 void SelectMotionNotifyFunc(void (*)(Wnd&, int, int, int, void*));    //half default bridge to allow std action and user selected
 void SelectKeyPressFunc(void (*)(Wnd&, ulong, void*));               //them> Release & ReleaseX are analogical <for key and butto>
 void SelectKeyReleaseFunc(void (*)(Wnd&, ulong, void*));             //means keyboard and mouse events...
 void SelectKeyPressXFunc(void (*)(Wnd&, ulong, void*));
 void SelectKeyReleaseXFunc(void (*)(Wnd&, ulong, void*));
 void SelectOtherEventFunc(void (*)(Wnd&, void*));                    //called when event found but there is no Response for it
 void SelectExposeXFunc(void (*)(Wnd&, void*));
 void SelectConfigureXFunc(void (*)(Wnd&, int, int, void*));
 void SelectMotionNotifyXFunc(void (*)(Wnd&, int, int, int, void*));
 void DisableExposeFunc();                                     //and this for disable func
 void DisableConfigureFunc();
 void DisableButtonPressFunc();
 void DisableButtonReleaseFunc();
 void DisableButtonPressXFunc();
 void DisableButtonReleaseXFunc();
 void DisableMotionNotifyFunc();
 void DisableKeyPressFunc();
 void DisableKeyReleaseFunc();
 void DisableKeyPressXFunc();
 void DisableKeyReleaseXFunc();
 void DisableOtherEventFunc();
 void DisableExposeXFunc();
 void DisableConfigureXFunc();
 void DisableMotionNotifyXFunc();
 void SetCx(int);
 void SetCy(int);
 void SetRx(int);
 void SetRy(int);
 void SetSize(int, int);
 void SetX(int);
 void SetY(int);
 void SetEventData(int, void*);
 void AboutQuit(bool);                      //set Quit status if true is set Wnd will die after nearest event
 void SetWin(Window&);
 void SetDsp(Dsp*);
 void SetGC(GC&);
 void SetDisplay(Display*);
 void SetDrag(int);
 void EnableText(bool);
 void ImprDrag(int);
 void SetLines(int);
 void SetWndText(char**);                      //set text to be displayed on window
 void PrepareWndText(char*, int);      //from char* makes char** when meets '\n' then makes char[i][sth> char[i+1][0]
 void StdTextWrite();                          //divides char* into separated by '\n' word in char** table
 void CreateCenterOKButton();                  //quite default setting
 void CreateOKButton(int, int);                //setpofcreateYesNosition
 void CreateOtherButton(int, int, int);        //user-defined
 void CreateCenterOtherButton(int);            //-||- but in the center
 void CreateButton(int, char*, int, int, void (*)(Wnd&, void*), bool, int, int, void*);  //full-control
 void CreateScrl(int, int, int, void (*)(Wnd&, void*), bool, int, int, void*);
 void SetButtonCtrl(int, void (*)(Wnd&, void*), void*);
 void SetNumBtns(int);
 void SetBtn(Button*, int);                    //obsolete
 void SetButtonsNumber(int);                   //You should avoid function that indirectly set Big structs
 void SetScrlNumber(int);
 void SetChild(Wnd*);                          //using SomeStruct* = SomeOtherStruct* or like this
 void SetWriteBox(Wbox*);                      //this can cause SIGSEGV if You don't know what You're doing !
 void SetCheckBox(CheckBox*);
 void Invalidate();
 void SetParent(Wnd*);                         //this sets default window parent
 void ParentCallFunc(void (*)(Wnd&, void*));          //allow parent to call function, while executing child
 void EnableFrames(bool);
 void EnableGraph(bool, bool);
 void SetGraph(ulong**);
 void DrawWindow();
 void DelGraph();
 void SetWndPixel(ulong, int, int);
 void DrawWndPixel(ulong, int, int);
 void SetCurColor(ulong);
 void ModColor(int,int,int);
 void EnablePalette(bool);
 void DeleteWndText();
 void MoveDefScroll(int);
 void ScrlDispatchBPressed(int, int);
 void ScrlDispatchBReleased();
 ulong CreatePaletteChild();
 void DrawPaletteColor();
 void SetDrawPosX(int);
 void SetDrawPosY(int);
 inline void StopDrawingLines();
 inline void DrawLineTo(int, int, ulong);
 void DrawMovedLine(int, int, ulong, int, int);
 void MakeClipFromWindowText(int, int, int, int);	 //creates clip area on window (I hope with scrolles)
 void* (*ProcFunc)(Wnd&, void*);                         //functions ending on "XFunc" are called inside default
 void (*SetupFunc)(Wnd&, void*);                         //when default functions selected
 void (*ExposeFunc)(Wnd&, void*);                        //addresses of functions which are called by DefaultResponse when non-NULL
 void (*ConfigureFunc)(Wnd&, int, int, void*);
 void (*ExposeXFunc)(Wnd&, void*);                        //addresses of functions which are called by DefaultResponse when non-NULL
 void (*ConfigureXFunc)(Wnd&, int, int, void*);
 void (*ButtonPressFunc)(Wnd&, int, int, int, void*);
 void (*ButtonReleaseFunc)(Wnd&, int, int, int, void*);
 void (*ButtonPressXFunc)(Wnd&, int, int, int, void*);     //first default check for pressed button
 void (*ButtonReleaseXFunc)(Wnd&, int, int, int, void*);   //them called user function if ButtonPressFunc set to
 void (*MotionNotifyFunc)(Wnd&, int, int, int, void*);     // eXtendedButtonPress or eXtendedButtonRelease
 void (*MotionNotifyXFunc)(Wnd&, int, int, int, void*);
 void (*KeyPressFunc)(Wnd&, ulong, void*);
 void (*KeyReleaseFunc)(Wnd&, ulong, void*);
 void (*KeyPressXFunc)(Wnd&, ulong, void*);
 void (*KeyReleaseXFunc)(Wnd&, ulong, void*);
 void (*OtherEventFunc)(Wnd&, void*);
 bool operator!(void);                          //operator to check if Wnd is NULL usage: Wnd& wnd; if (!wnd) return;
                                                //                        operator there               ~~~
 private:
 int cx;
 int cy;
 int rx, ry;
 int x,y;
 int lx,ly;
 int done;
 bool want_pinfo;
 Window win;
 Dsp* odsp;
 GC gc;
 XFontStruct* font_info;
 XTextProperty window_name_property;
 int font_h;
 Display* disp;
 XEvent event;
 int drag;
 char** text;
 char* caption;
 int lines;
 int num_btns;
 int num_scrl;
 Button** btn;
 Wbox* wbox;
 CheckBox* cbox;
 Wnd* child;
 Wnd* parent;
 Unused* dta;
 bool text_disabled;
 int deleted;
 int apressed;
 void** event_ptr;
 ulong** graph;
 bool have_frames;
 bool have_graph;
 bool have_palette;
 bool have_ca;
 int gx, gy;
 ulong cur_color;
 Scroll** scrl;
 ClipArea* ca;
 void Std_Init();                               //called to init class (without will be SIGSEGV)
 void Init_event_ptr();                         //the're private! cannot be used from outside!
 void Kill_event_ptr();
};

class ClipArea
{
 public:
 ClipArea(const Wnd*);
 ClipArea(int, int);
 ClipArea(int, int, int, int);
 ClipArea(int, int, int, int, int, int);
 ClipArea(int, int, int, int, int, int, int, int, char**,int);
 ~ClipArea();
 void SetSx(int);
 void SetSy(int);
 void SetEx(int);
 void SetEy(int);
 void SetX(int);
 void SetY(int);
 void SetLx(int);
 void SetLy(int);
 void SetLines(int);
 void SetMaxLen(int);
 void SetCLines(int);
 void SetCMaxLen(int);
 void SetPos(int, int, bool);
 int Update();
 void SetClipText(char**, int);
 void SetClippedText(char**, int);
 int RetSx() const;
 int RetSy() const;
 int RetEx() const;
 int RetEy() const;
 int RetX() const;
 int RetY() const;
 int RetLx() const;
 int RetLy() const;
 int RetCharsY() const;			//about not converted text
 int RetCharsX() const;
 int RetCCharsX() const;		//about clipped text
 int RetCCharsY() const;
 char** RetClipText() const;
 char** RetClippedText() const;
 void MoveLeft(int);
 void MoveRight(int);
 void MoveUp(int);
 void MoveDown(int);
 double RetPartX();			//to draw scroll bars
 double RetPartY();
 private:
 int sx, sy, ex, ey, x, y;
 char** text;			//not converted
 char** dtext;			//converted
 int lines;			//num lines of original
 int clines;			//number of clipped lines
 int maxlen;			//maxlen of not clipped text
 int cmaxlen;			//maxlen of clipped text
 int lx, ly;
 bool re;			//is redraw needed?
};

ClipArea :: ~ClipArea()
{
 if (text)
 {
  for (int i=0;i<lines;i++) if (text[i]) { strcpy(text[i], ""); delete [] text[i]; }
  delete [] text;
 }
 if (dtext)
 {
  for (int i=0;i<clines;i++) if (dtext[i]) { strcpy(dtext[i], ""); delete [] dtext[i]; }
  delete [] dtext;
 }
}


double ClipArea :: RetPartX()
{
 double al = (double)maxlen - (double)cmaxlen;
 if (al<=1.0) return -1.0;
 return ((double)x)/al;
}


double ClipArea :: RetPartY()
{
 double al = (double)lines - (double)clines;
 if (al<=1.0) return -1.0;
 return ((double)y)/al;
}


void ClipArea :: MoveDown(int a=1)
{
 this->y += a;
 re = true;
 Update();
}


void ClipArea :: MoveUp(int a=1)
{
 this->y -= a;
 re = true;
 Update();
}


void ClipArea :: MoveRight(int a=1)
{
 this->x += a;
 re = true;
 Update();
}


void ClipArea :: MoveLeft(int a=1)
{
 this->x -= a;
 re = true;
 Update();
}


int ClipArea :: Update()
{
 if (!text) return 0;
 if (!re) return 0;
 this->cmaxlen = (ex - sx) / lx;
 this->clines = (ey - sy) / ly;
 if (cmaxlen<0) return 0;
 if (clines<0) return 0;
 maxlen=0;
 for (int i=0;i<lines;i++) if ((int)strlen(text[i])>maxlen) maxlen = strlen(text[i]);
 if (x<0) x = 0;
 if (y<0) y = 0;
 if (cmaxlen > maxlen) cmaxlen = maxlen;
 if (clines > lines)   clines = lines;
 if (x>=maxlen-cmaxlen) x = maxlen-cmaxlen;
 if (y>=lines-clines)   y = lines-clines;
 if (dtext)
  {
   for (int i=0;i<clines;i++) if (dtext[i]) delete [] dtext[i];
   delete [] dtext;
   dtext = NULL;
  }
 dtext = new char*[clines+1];
 if (!dtext) return 0;
 int j;
 bool eol;
 //printf("PTR=%p, CLIN=%d, LIN=%d X=%d, Y=%d, ML=%d, CML=%d\n", dtext, clines, lines, x, y, maxlen, cmaxlen);
 for (int i=y;i<clines+y;i++)
 {
  dtext[i-y] = new char[cmaxlen+2];
  dtext[i-y][0] = '\0';
  j = 0;
  eol = false;
  while (j<x)
    {
     if (text[i][j] == '\0')
       {
	dtext[i-y][0] = '\0';
	eol = true;
	j=x;
	break;
       }
     j++;
    }
  if (!eol)
    {
     while (text[i][j] != '\0' && j<x+cmaxlen)
       {
        dtext[i-y][j-x] = text[i][j];
        j++;
       }
     dtext[i-y][j-x] = '\0';
    }
 }
 //for (int i=0;i<clines;i++) printf("%s\n", dtext[i]);
 re = false;
 return 1;
}


char** ClipArea :: RetClippedText() const
{
 return this->dtext;
}


char** ClipArea :: RetClipText() const
{
 return this->text;
}


int ClipArea :: RetCCharsY() const
{
 return this->clines;
}


int ClipArea :: RetCCharsX() const
{
 return this->cmaxlen;
}


int ClipArea :: RetCharsY() const
{
 return this->lines;
}


int ClipArea :: RetCharsX() const
{
 return this->maxlen;
}


int ClipArea :: RetSx() const
{
 return this->sx;
}


int ClipArea :: RetSy() const
{
 return this->sy;
}


int ClipArea :: RetEx() const
{
 return this->ex;
}


int ClipArea :: RetEy() const
{
 return this->ey;
}


int ClipArea :: RetX() const
{
 return this->x;
}


int ClipArea :: RetY() const
{
 return this->y;
}


int ClipArea :: RetLx() const
{
 return this->lx;
}


int ClipArea :: RetLy() const
{
 return this->ly;
}


void ClipArea :: SetClippedText(char** t, int l)
{
 if (t)
   {
    if (dtext) { for (int i=0;i<clines;i++) if (dtext[i]) delete [] dtext[i]; delete [] dtext; }
    dtext = NULL;
    dtext = new char*[l+1];
    if (!dtext) panic("cannot create new in ClipArea::SetClippedText");
    for (int i=0;i<l;i++)
      {
       dtext[i] = new char[strlen(t[i])+3];
       strcpy(dtext[i], t[i]);
      }
   }
 if (l>0) this->clines = l;
 re = true;
}


void ClipArea :: SetClipText(char** t, int l)
{
 if (t)
   {
    if (text) { for (int i=0;i<lines;i++) if (text[i]) delete [] text[i]; delete [] text; }
    text = NULL;
    text = new char*[l+1];
    if (!text) panic("cannot create new in ClipArea::SetClippText");
    for (int i=0;i<l;i++)
      {
       text[i] = new char[strlen(t[i])+3];
       strcpy(text[i], t[i]);
      }
   }
 if (l>0) this->lines = l;
 re = true;
}


void ClipArea :: SetMaxLen(int l)
{
 if (l>0) this->maxlen = l;
 re = true;
}


void ClipArea :: SetLines(int l)
{
 if (l>0) this->lines = l;
 re = true;
}


void ClipArea :: SetCMaxLen(int l)
{
 if (l>0) this->cmaxlen = l;
 re = true;
}


void ClipArea :: SetCLines(int l)
{
 if (l>0) this->clines = l;
 re = true;
}


void ClipArea :: SetPos(int xx, int yy, bool update=false)
{
 if (xx>0) this->x = xx;
 if (yy>0) this->y = yy;
 re = true;
 if (update) Update();
}


void ClipArea :: SetSx(int p)
{
 if (p>0) this->sx = p;
 re = true;
}


void ClipArea :: SetSy(int p)
{
 if (p>0) this->sy = p;
 re = true;
}


void ClipArea :: SetEx(int p)
{
 if (p>0) this->ex = p;
 re = true;
}


void ClipArea :: SetEy(int p)
{
 if (p>0) this->ey = p;
 re = true;
}


void ClipArea :: SetX(int p)
{
 if (p>0) this->x = p;
 re = true;
}


void ClipArea :: SetY(int p)
{
 if (p>0) this->y = p;
 re = true;
}


void ClipArea :: SetLx(int p)
{
 if (p>0) this->lx = p;
 re = true;
}


void ClipArea :: SetLy(int p)
{
 if (p>0) this->ly = p;
 re = true;
}


ClipArea::ClipArea(const Wnd* w)
{
 out("ClipArea :: ClipArea(const Wnd& w)");
 sx = 5;
 sy = 5;
 lx = CAX;
 ly = CAY;
 x = 0;
 y = 0;
 ex = w->RetRx() - 5;
 ey = w->RetRy() - 5;
 clines  = lines = 0;
 cmaxlen = maxlen = 0;
 text = dtext = NULL;
 re = false;
 Update();
}


ClipArea::ClipArea(int wx, int wy)
{
 out("ClipArea :: ClipArea(2xi)");
 sx = 5;
 sy = 5;
 lx = CAX;
 ly = CAY;
 x = 0;
 y = 0;
 ex = wx;
 ey = wy;
 clines = lines = 0;
 cmaxlen = maxlen = 0;
 text = dtext = NULL;
 re = false;
 Update();
}


ClipArea::ClipArea(int wx, int wy, int _x, int _y)
{
 out("ClipArea :: ClipArea(4xi)");
 sx = wx;
 sy = wy;
 lx = CAX;
 ly = CAY;
 x = 0;
 y = 0;
 ex = _x;
 ey = _y;
 clines = lines = 0;
 cmaxlen = maxlen = 0;
 text = dtext = NULL;
 re = false;
 Update();
}


ClipArea::ClipArea(int wx, int wy, int _x, int _y, int mx, int my)
{
 out("ClipArea :: ClipArea(6xi)");
 sx = wx;
 sy = wy;
 lx = CAX;
 ly = CAY;
 x = mx;
 y = my;
 ex = _x;
 ey = _y;
 clines = lines = 0;
 cmaxlen = maxlen = 0;
 text = dtext = NULL;
 re = false;
 Update();
}


ClipArea::ClipArea(int wx, int wy, int _x, int _y, int mx, int my, int llx, int lly, char** dat, int lin)
{
 out("ClipArea :: ClipArea(8xi,ppc,i)");
 sx = wx;
 sy = wy;
 lx = llx;
 ly = lly;
 x = mx;
 y = my;
 ex = _x;
 ey = _y;
 lines = lin;
 clines = 0;
 cmaxlen = maxlen = 0;
 text = dat;
 dtext = NULL;
 re = true;
 Update();
}


bool Wnd :: WantPaletteInfo() const
{
 return this->want_pinfo;
}


void Wnd :: SetPaletteInfo(bool arg)
{
 this->want_pinfo = arg;
}


void Wnd :: DrawClipArea()
{
 out("Draw ClipArea");
 if (!have_ca) return;
 int x =ca->RetSx();
 int y =ca->RetSy();
 int xl =ca->RetEx();
 int yl =ca->RetEy();
 DrawFrame(RetDisp(), win, RGB(0XDA, 0XDA, 0XDA), gc, RGB(4,4,4), 4, x-6,xl+7,y-6,yl+5);
 ca->Update();
 char** wrt = ca->RetClippedText();
 int lin    = ca->RetCCharsY();
 if (!wrt) return;
 if (lin<=0) return;
 double xi = ca->RetPartX();
 double yi = ca->RetPartY();
 int xsc = x-6 + int(double(xl-x)*xi);
 int ysc = y-6 + int(double(yl-y)*yi);
 if (xi>=0.0) XFillA(RetDisp(), win, gc, RGB(0x90, 0, 0), xsc, yl+2, 13, 3);
 else XFillA(RetDisp(), win, gc, RGB(0x50, 0x50, 0x50), (x+xl)/2-6, yl+2, 13, 3);
 if (yi>=0.0)  XFillA(RetDisp(), win, gc, RGB(0, 0, 0x90), xl+4, ysc, 3, 11);
 else XFillA(RetDisp(), win, gc, RGB(0x50, 0x50, 0x50), xl+4, (y+yl)/2-5, 3, 11);
 XSetForeground(disp,gc,RGB(0xff,0xff,0xff));
 for (int i=0;i<lin;i++)
   {
     if (wrt[i]) XDrawString(disp, win, gc, x, y+ 6 + i*(font_h+3), wrt[i], strlen(wrt[i]));
   }
}


void Wnd :: FadeClip() // puts black rectangles onto text!
{
 out("Wnd::FadeClip() writing text in Black-RGB (0x0)");
 if (!have_ca) return;
 int x =ca->RetSx();
 int y =ca->RetSy();
// int xl =ca->RetEx();
// int yl =ca->RetEy();
 char** wrt = ca->RetClippedText();
 int lin    = ca->RetCCharsY();
 if (!wrt) return;
 if (lin<=0) return;
 XSetForeground(disp,gc,0);
 for (int i=0;i<lin;i++)
   {
     if (wrt[i]) XDrawString(disp, win, gc, x, y+ 6 + i*(font_h+3), wrt[i], strlen(wrt[i]));
   }
}

void StdExposeFunc(Wnd& wnd, void*);                     // these are forwardes of predefined standard out-class
void NoWDrawExposeFunc(Wnd&, void*);
void PalExposeFunc(Wnd& wnd, void*);
void eXtendedButtonPress(Wnd&, int, int, int, void*);    // response functions (used some-times by default)
void eXtendedButtonRelease(Wnd&, int, int, int, void*);
void StdOKFunc(Wnd&, void*);
void StdUpFunc(Wnd&, void*);
void StdDownFunc(Wnd&, void*);
void StdUpTenFunc(Wnd&, void*);
void StdDownTenFunc(Wnd&, void*);
void StdYesFunc(Wnd&, void*);
void PrepareVoidReturn(Wnd&, void*);
void StdNoFunc(Wnd&, void*);
void eXtendedKeyRelease(Wnd&, ulong, void*);
void eXtendedKeyReleaseND(Wnd&, ulong, void*);
void eXtendedKeyPress(Wnd&, ulong, void*);
void KeyPressClipControl(Wnd&, ulong, void*);
void eXtendedKeyPressQ(Wnd&, ulong, void*);
void eXtendedButtonRelease(Wnd&, int, int, int, void*);
void eXtendedButtonReleaseND(Wnd&, int, int, int, void*);
void eXtendedButtonPress(Wnd&, int, int, int, void*);
void PalKeyRelease(Wnd&, ulong, void*);
void PalButtonRelease(Wnd&, int, int, int, void*);
void PalButtonPress(Wnd&, int, int, int, void*);
void DenyResizeFunc(Wnd&, int, int, void*);
void PalDenyResizeFunc(Wnd&, int, int, void*);
void NoEDenyResizeFunc(Wnd&, int, int, void*);
void NoResizeDrawWFunc(Wnd&, int, int, void*);
void StdConfigureFunc(Wnd&, int, int, void*);
void NoWDrawConfigureFunc(Wnd&, int, int, void*);
void StdMotionNotifyFunc(Wnd&, int, int, int, void*);
void PalMotionNotifyFunc(Wnd&, int, int, int, void*);
void wBoxKeyPressFunc(Wnd&, ulong, void*);
void wBoxKeyReleaseFunc(Wnd&, ulong, void*);
void RetOwnNameFunc(Wnd&, void*);
void DrawOnWindowFunc(Wnd&, int, int, int, void*);
void PaletteExposeInfo(Wnd&, void*);

void Wnd :: Init_event_ptr()
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::Init_event_ptr");
 out(dstr);
#endif
 event_ptr = new void*[NUM_EVENTS];
 if (!event_ptr) panic("MEMORY_EXCPT_IN_operator new", __LINE__,__FILE__);
#ifdef DBG
 sprintf(dstr,"event_ptr=%p", event_ptr);
 out(dstr);
#endif
 for (int i=0;i<NUM_EVENTS;i++) event_ptr[i] = NULL;
}


ClipArea* Wnd :: RetClipArea() const
{
 if (HaveClipArea()) return ca;
 else return NULL;
}


bool Wnd :: HaveClipArea() const
{
 return have_ca;
}


void Wnd :: Kill_event_ptr()
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::Kill_event_ptr, which is now: %p", event_ptr);
 out(dstr);
#endif
 if (!event_ptr) return;
 if (event_ptr) delete [] event_ptr;
}


void Wnd :: EnableText(bool e)
{
 tout("Enable/Disable WindowText");
 text_disabled = !e;
}


void Wnd ::SetEventData(int ev_num, void* ptr_data)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SetEventData(%d,%p) ev_num, ptr_data", ev_num,ptr_data);
 out(dstr);
#endif
 if ((ev_num < 0) || (ev_num >= NUM_EVENTS)) return;
 if (!event_ptr) return;
 if (ptr_data) event_ptr[ev_num] = ptr_data;
}


void* Wnd :: RetEventData(int ev_num) const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::RetEventData(%d)", ev_num);
 out(dstr);
#endif
 if ((ev_num < 0) || (ev_num >= NUM_EVENTS)) return NULL;
 else
   {
    if (!event_ptr) return NULL;
#ifdef DBG
 sprintf(dstr,"far_ptr= %p (if NULL then unset yet)", event_ptr[ev_num]);
 out(dstr);
#endif
    return event_ptr[ev_num];
   }
}


void Wnd :: EnableGraph(bool par, bool clean = true)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::Enablegraph(%d,%d) enable,clean", par,clean);
 out(dstr);
#endif
 have_graph = par;
 if (!have_graph)
    {
     if (graph)
       {
        out("warning in wnd::enablegraph, setting to false but graph exists, deleting it!");
        DelGraph();
       }
    }
 else
    {
     if (graph)
       {
        out("warinig in wnd::enablegraph, enabling graph but it already exists");
	out("delete it first by using DelGraph!");
        return;
       }
     graph = new ulong*[RetCx()];
     if (!graph) panic("memory exception in wnd::enable graph wnd::graph failed!");
     for (int i=0;i<RetCx();i++)
       {
        graph[i] = new ulong[RetCy()];
	if (!graph[i]) panic("memory exception in wnd::enable graph wnd::graph[i] failed!");
       }
     if (clean)
       {
        for (int i=0;i<RetCx();i++) for (int j=0;j<RetCy();j++) graph[i][j] = 0x0;
       }
     gx = RetCx();
     gy = RetCy();
    }
#ifdef DBG
 sprintf(dstr,"graphisc is now set to %d", par);
 out(dstr);
#endif
}


void Wnd :: PrepareWndText(char* txt, int nolets = LEAVE)  //cut char* int parts of char**
{
#ifdef DBG
 char dstr[PREDATE];
 sprintf(dstr,"Wnd::PrepareWndtext(%p,%d) text, nolets", txt,nolets);
 out(dstr);
 if (txt)
 sprintf(dstr,"CONVERTING_THIS:\n%s", txt);
 out(dstr);
#endif
 if (!txt) return;
 if (text)
   {
    for (int i=1;i<lines;i++) if (text[i]) delete [] text[i];
    if (text) delete [] text;
   }
 lines = 1;
 int max_len = strlen(txt);
 int mxl = 0;
 int cnt = 2;
 for (int i=0;i<max_len;i++)
   {
    if (txt[i] == '\n')
     {
      lines++;
      if (cnt > mxl) mxl = cnt;
      cnt = 2;
     }
    else cnt++;
   }
 if (lines<=1) mxl = strlen(txt);
 if (cnt > mxl) mxl = cnt;
 text = new char*[lines+2];
 if (!text) panic("memory exception in wnd::text");
 for (int i=0;i<lines;i++) {text[i] = new char[mxl+1];if (!text[i]) panic("memory exception in text[]");}
 int j=0;
 int k=0;
 for (int i=0;i<max_len;i++)
    {
     while (txt[i] == '\n') { text[k][j] = '\0'; j=0; k++; i++;}
     text[k][j] = txt[i];
     if (text[k][j] == '\t') text[k][j] = ' ';
     //FIXME possible error ???  ?? ? (there is a problem with Wnd::WindowDies() maybe somewhere there is a cause
     if ((!isLetter(text[k][j])) && (nolets == REMOVE)) text[k][j] = ' ';
     j++;
    }
 text[k][j] = '\0';   //THIS IS FUCK VERY IMPORTANT, I WAS SEARCHING FOR THIS F*** BUG FOR 2 DAYS!!!
 //for (int i=0;i<lines;i++) printf("%s\n", text[i]);
 out("Wnd::preparewndtext ends. . .");
}


void Wnd :: CreateButton(int num, char* txt, int xx, int yy, void (*f)(Wnd&, void*),bool def = false, int bx = 80, int by = 25, void* add_data = NULL)
{
#ifdef DBG
 char dstr[LONG_STR*2];
 sprintf(dstr,"Wnd::CreateButton(%d,%p,%d,%d,%p,%d,%d,%d,%p) num,text,x,y,func,def,bx,dy,far_data", num,txt,xx,yy,f,def,bx,by,add_data);
 out(dstr);
 if (txt)
   {
    sprintf(dstr,"btn_name=%s", txt);
    out(dstr);
   }
#endif
 if ((num >= RetNumBtns()) || (num < 0)) {out("warning in create button, attempting to use nonexisting btn!"); return;}
 if (txt) btn[num]->SetBtnName(txt); else btn[num]->SetBtnName("unknown");
 if (add_data) btn[num]->SetVoid(add_data);
 btn[num]->SetDefault(def);
 btn[num]->SetBtnPos(xx, yy);                            //this function uses no defaults valuse
 btn[num]->SetBtnSize(bx, by);                           //all're user-defined
 if (f) btn[num]->SetBtnFunc(f);
 else out("warning in create_btn: button_func shouldn't be NULL, but currently is!");
 DrawBtn(num);
}


void Wnd :: CreateScrl(int num, int xx, int yy, void (*f)(Wnd&, void*),bool def = false,int slen = 128, int max_len = 20, void* add_data = NULL)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::CreateScrl(%d,%d,%d,%p,%d,%d,%d,%p) num,x,y,func,def,slen,max_len,far_data", num,xx,yy,f,def,slen,max_len,add_data);
 out(dstr);
#endif
 if ((num >= RetNumScrl()) || (num < 0)) {out("warning in create scroll, attempting to use nonexisting scroll!"); return;}
 if (add_data) scrl[num]->SetVoid(add_data);
 scrl[num]->SetDefault(def);
 scrl[num]->SetScrlXY(xx, yy);                            //this function uses no defaults valuse
 scrl[num]->SetScrlLen(slen);                           //all're user-defined
 scrl[num]->SetMaxPos(max_len);
 scrl[num]->SetScrlPos(0);
 if (f) scrl[num]->SetScrlFunc(f);
 else out("warning in create_scrl: scroll_func shouldn't be NULL, but currently is!");
 DrawScrl(num);
}


Wnd :: Wnd(Dsp* dsp, char* text, int btn = MB_OK)
{
#ifdef DBG
 char dstr[LONG_STR];
 if (text) sprintf(dstr,"Wnd::Wnd(%p,%p:%s,%d) dsp,txet,btn", dsp,text,text,btn);
 else sprintf(dstr,"Wnd::Wnd(%p,(char*)NULL),%d) dsp,text!,btn",dsp,btn);
 out(dstr);
#endif
 if (dsp == NULL) return;
 if (!dsp->RetDsp()) return;
 Std_Init();
 odsp = dsp;
#ifdef DBG
 sprintf(dstr,"CONSTR; cx=rx=300,cy=ry=200, will call XCreateWindow Immadiately!");
 out(dstr);
#endif
 cx = rx = 300;
 cy = ry = 200;
 ca = NULL;
 win = XCreateSimpleWindow
 (dsp->RetDsp(),
 RootWindow(dsp->RetDsp(), dsp->RetSNum())
 ,0, 0, cx, cy, 0,
 WhitePixel(dsp->RetDsp(), dsp->RetSNum()),
 BlackPixel(dsp->RetDsp(), dsp->RetSNum()));
 if (!win) panic("unable to set up window!");
 XMapWindow(dsp->RetDsp(), win);
 XFlush(dsp->RetDsp());
 disp = odsp->RetDsp();
 CreateGC();
 SetFont("*-clean-*-10-*");
 if (text)
  {
   PrepareWndText(text);
   StdTextWrite();
  }
 if (btn == MB_OK) CreateCenterOKButton();
 else CreateCenterOtherButton(btn);           //not tested yet :( [UPDATE: I think it's now tested]
}


Scroll* Wnd :: RetScroll(int par)   const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::RetScroll <<checking_conditionals>>");
 out(dstr);
#endif
 if (!HaveScroll()) return NULL;
 if (par < 0) return NULL;
 if (par >= RetNumScrl()) return NULL;
#ifdef DBG
 sprintf(dstr,"Wnd::RetScroll(%d) ==> %p", par, scrl[par]);
 out(dstr);
#endif
 return scrl[par];
}


bool Wnd :: operator!(void)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::operator! this= <cast> %p", this);
 out(dstr);
#endif
 if (this) return false;
 else return true;                     //FIXME I'm not sure operator will return true if class is NULL
}                                      //if class don't exist then operator exists ? ?? ??? (hard to say)

Wnd :: Wnd(Dsp* dsp, char* text, int btn, int xx, int yy, int bx, int by)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::wnd(%p,%p,%d,%d,%d,%d,%d) dsp,text,btn,cx,cy,btn_x,btn_y",dsp,text,btn,xx,yy,bx,by);
 out(dstr);
 if (text) sprintf(dstr,"text=%p:%s", text,text);
 out(dstr);
#endif
 if (dsp == NULL) return;
 if (!dsp->RetDsp()) return;
 Std_Init();
 odsp = dsp;
 cx = rx = xx;
 cy = ry = yy;
 ca = NULL;
 win = XCreateSimpleWindow
 (dsp->RetDsp(),
 RootWindow(dsp->RetDsp(), dsp->RetSNum())
 ,0, 0, cx, cy, 0,
 WhitePixel(dsp->RetDsp(), dsp->RetSNum()),
 BlackPixel(dsp->RetDsp(), dsp->RetSNum()));
 if (!win) panic("unable to set up window!");
 XMapWindow(dsp->RetDsp(), win);
 XFlush(dsp->RetDsp());
 disp = odsp->RetDsp();
 CreateGC();
 SetFont("*-clean-*-10-*");
 if (text)
  {
   PrepareWndText(text);
   StdTextWrite();
  }
 if ((btn == MB_OK) && (bx == DEFAULT) && (by == DEFAULT)) { out("centerok"); CreateCenterOKButton(); return; }
 if (btn == MB_OK) {out("ok");CreateOKButton(bx, by);}
 if ((!(btn == MB_OK)) && (bx == DEFAULT) && (by == DEFAULT)) { out("centerother");CreateCenterOtherButton(btn); return; }
 if (!(btn == MB_OK)) {out("other");CreateOtherButton(bx, by, btn);}
 out("constructor ends...");
}


void Wnd :: Invalidate()
{
 out("wnd::invalidate");
 if (ExposeFunc) ExposeFunc(*this,RetEventData(EV_EXPOSE));
 else out("warinig in wnd::invalidate: no expose func defined!");
 out("should be updated now.");
}


ulong Wnd :: CreatePaletteChild()
{
 out("wnd::createpalettechild");
 if (child) child->WindowDies();
 child = new Wnd(odsp, 200, 340);
 if (!child) panic("memory exception in create_win_child CreatePaletteChild");
 child->CreateGC();
 child->SetFont("*-clean-*-10-*");
 child->SetCaption("Wybierz kolor");
 child->DeclareResponseTable(MOST_RESPONSES);
 child->SelectExposeFunc(PalExposeFunc);
 child->SelectConfigureFunc(PalDenyResizeFunc);
 child->SelectButtonPressFunc(PalButtonPress);
 child->SelectKeyPressFunc(eXtendedKeyPress);
 child->SelectButtonReleaseFunc(PalButtonRelease);
 child->SelectKeyReleaseFunc(PalKeyRelease);
 child->SelectMotionNotifyFunc(PalMotionNotifyFunc);
 child->EnableFrames(0);
 child->SetScrlNumber(3);
 child->SetPaletteInfo(true);
 child->CreateScrl(0, 50,  50, NULL, true, 0xff, 0xff);
 child->CreateScrl(1, 100,  50, NULL, false, 0xff, 0xff);
 child->CreateScrl(2, 150,  50, NULL, false, 0xff, 0xff);
 child->SetButtonsNumber(1);
 child->VoidPtrReturn();
 child->CreateButton(0, "wybierz", 60, 310, PrepareVoidReturn);
 child->SetDefaultButton(0);
 child->SetParent(this);
 child->DefaultResponse();
 void* val = child->ReturnVoid();
 if (child) delete child;
 child = NULL;
 Invalidate();
 ulong* color = (ulong*)val;
 return *color;
}


void Wnd :: ScrlDispatchBPressed(int x, int y)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::ScrlDispatchBPressed(%d,%d)", x,y);
 out(dstr);
#endif
 if (!scrl) return;
 for (int i=0;i<RetNumScrl();i++)
   {
    if (scrl[i]->insideArea(x,y))
      {
       FadeScroll(i);
       scrl[i]->GetPosFromArea(y);
       scrl[i]->SetScrlFocus(true);
       DrawFadedScrl(i);
       return;
      }
   }
}


void Wnd :: ScrlDispatchBReleased()
{
 out("wnd::scrldispatchbreleased");
 if (!scrl) return;
 for (int i=0;i<RetNumScrl();i++)
   {
    if (scrl[i]->isSelected())
      {
       FadeScroll(i);
       if (scrl[i]->scrlFunc) scrl[i]->scrlFunc(*this, RetEventData(EV_BRELEASE));
       scrl[i]->SetScrlFocus(false);
       DrawFadedScrl(i);
      }
   }
}


Wnd :: Wnd(Dsp* dsp)   //this constructor do the least possible <user !MUST! continue init by (him-her)self>
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::Wnd(%p) dsp",dsp);
 out(dstr);
#endif
 if (dsp == NULL) return;
 if (!dsp->RetDsp()) return;
 Std_Init();
 odsp = dsp;
 out("using defaults... cx,rx=800, cy,ry=600");
 cx = rx =800;
 cy = ry =600;
 ca = NULL;
 win = XCreateSimpleWindow
 (dsp->RetDsp(),
 RootWindow(dsp->RetDsp(), dsp->RetSNum())
 ,0, 0, cx, cy, 1,
 WhitePixel(dsp->RetDsp(), dsp->RetSNum()),
 BlackPixel(dsp->RetDsp(), dsp->RetSNum()));
 if (!win) panic("unable to set up window in default constructor Wnd!");
 XMapWindow(dsp->RetDsp(), win);
 XFlush(dsp->RetDsp());
 disp = odsp->RetDsp();
}


char* Wnd :: RetBtnName(int num) const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::RetBtnName(%d) ==> ",num);
 out(dstr);
#endif
 if ((num < 0) || (num >= RetNumBtns())) {out("warning in wnd::retbtnname, attempted to return name of nonexisting btn"); return (char*)NULL;}
 if (!btn) return NULL;
 if (!btn[num]) return NULL;
#ifdef DBG
 sprintf(dstr,"far_ptr=%p",btn[num]->RetText());
 out(dstr);
#endif
 return btn[num]->RetText();
}


Wnd* Wnd :: ReturnParent() const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::ReturnParent() ==> %p",parent);
 out(dstr);
#endif
 if (!parent) out("warning in wnd::returnparent, returning NULL <<<THIS WINDOW DOESN'T HAVE PARENT>>>");
 return this->parent;
}


void Wnd :: ParentCallFunc(void (*external)(Wnd&, void*))
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::ParentCallFunc(far_func=%p), using EV_PARENTCALL",external);
 out(dstr);
#endif
 if (!parent) return;
 else (*external)(*this->parent, RetEventData(EV_PARENTCALL));   //maxymalny WYPAS
}


void Wnd :: MoveDefScroll(int par)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::MovedefScroll(%d)", par);
 out(dstr);
#endif
 if (!scrl) return;
 for (int i=0;i<RetNumScrl();i++)
   {
    if (scrl[i]->isDefault())
      {
       FadeScroll(i);
       scrl[i]->SetScrlPos(scrl[i]->RetPos()+par);
       if (scrl[i]->scrlFunc) scrl[i]->scrlFunc(*this, RetEventData(EV_KRELEASE));
       DrawFadedScrl(i);
       return;
      }
   }
}


bool Wnd :: HaveScroll() const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::HaveSrroll() ==> %d",((scrl) && (num_scrl)));
 out(dstr);
#endif
 if ((scrl) && (num_scrl)) return true;
 else return false;
}


int Wnd :: RetPressed() const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::RetPressed() ==> %d",apressed);
 out(dstr);
#endif
 return this->apressed;
}


void Wnd :: SetParent(Wnd* par)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SetParent(far_ptr=%p)", par);
 out(dstr);
#endif
 if (!par) {out("warning in wnd::setparent, tried to set NULL: aborted"); return ;}
 this->parent = par;
}


void Wnd :: SetPressed(int par)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SetPressed(%d)", par);
 out(dstr);
#endif
 if (par > RetNumBtns() - 1) {out("warning in Wnd::SetPressed, tried to press nonexisting, aborted!"); return;}
 if (par < -1)  return;
 if (!btn)      return;
 if (!btn[par]) return;
 this->apressed = par;
 for (int i=0;i<RetNumBtns();i++)
   {
    if (btn[i]->isDefault()) btn[i]->SetDefault(0);
   }
 btn[par]->SetDefault(1);
}


void Wnd :: CancelPressed()
{
 out("wnd::cancelpressed");
 if (!btn) return;
 for (int i=0;i<RetNumBtns();i++)
   {
    if (btn[i]->isDefault()) btn[i]->SetDefault(0);
   }
 btn[apressed]->SetDefault(1);
 this->apressed = -1;
}


void Wnd :: SetPressedND(int par)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SetPressed(%d)", par);
 out(dstr);
#endif
 if (par > RetNumBtns() - 1) {out("warning in Wnd::SetPressed, tried to press nonexisting, aborted!"); return;}
 if (par < -1)  return;
 if (!btn)      return;
 if (!btn[par]) return;
 this->apressed = par;
}


void Wnd :: CancelPressedND()
{
 out("wnd::cancelpressed");
 if (!btn) return;
 this->apressed = -1;
}


void Wnd :: CreateMBoxChild(char* text, int bbb = MB_OK, int x = 300, int y = 250, int bx = DEFAULT, int by = DEFAULT)
{
#ifdef DBG
 char dstr[LONG_STR];
 if (text) sprintf(dstr,"Wnd::CreateMBoxChild(%p:%s,%d,%d,%d,%d,%d) text,button,x,y,btn_x,btn_y",text,text,bbb,x,y,bx,by);
 else sprintf(dstr,"Wnd::CreateMBoxChild((char*)NULL,%d,%d,%d,%d,%d) text!,button,x,y,btn_x,btn_y",bbb,x,y,bx,by);
 out(dstr);
#endif
 if (child) child->WindowDies();
 child = new Wnd(odsp, text, bbb, x, y, bx, by);
 if (!child) panic("memory exception in create_win_child CreateMBoxChild");
 child->SetCaption("Informacja");
 child->DeclareResponseTable(MEDI_RESPONSES);
 child->SelectExposeFunc(StdExposeFunc);
 child->SelectConfigureFunc(DenyResizeFunc);
 child->SelectButtonPressFunc(eXtendedButtonPress);
 child->SelectKeyPressFunc(eXtendedKeyPress);
 child->SelectButtonReleaseFunc(eXtendedButtonRelease);
 child->SelectKeyReleaseFunc(eXtendedKeyRelease);
 child->SetParent(this);
 child->DefaultResponse();
 if (child) delete child;
 child = NULL;
 Invalidate();
}


char* Wnd :: CreateCheckBoxChild(char* txt, char** table, int lines, int scrl = 10, int wx = 300, int wy = 550)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::CreateCheckBoxChild(%p,%p,%d,%d,%d,%d) txt,table,lines,scrl,wx,wy", txt,table,lines,scrl,wx,wy);
 out(dstr);
 if (txt) sprintf(dstr,"txt=%p:%s",txt,txt);
 out(dstr);
 if (table) sprintf(dstr,"table=(char**)%p",table);
 out(dstr);
 for (int i=0;i<lines;i++)
   {
    sprintf(dstr,"table[%d] = (char*)%p:%s", i, table[i],table[i]);
    out(dstr);
   }
#endif
 if (!lines)
   {
    CreateMBoxChild("Brak danych wypelniajacych strukture!\nNie moge utworzyc okna przewijania\nbez elementow");
    return (char*)("NULL");
   }
 if (!table) {DoQuit(); DefaultResponse(); return NULL;}
 if (child) child->WindowDies();
 child = new Wnd(odsp, wx, wy);
 if (!child) panic("memory exception in create_win_child CreateCheckBoxChild");
 child->CreateGC();
 child->SetFont("*-clean-*-10-*");
 child->SetCaption("Wybierz");
 child->DeclareResponseTable(MEDI_RESPONSES);
 child->SelectExposeFunc(StdExposeFunc);
 child->SelectConfigureFunc(DenyResizeFunc);
 child->SelectKeyPressFunc(eXtendedKeyPress);
 child->SelectKeyReleaseFunc(eXtendedKeyRelease);
 child->SelectButtonPressFunc(eXtendedButtonPress);
 child->SelectButtonReleaseFunc(eXtendedButtonRelease);
 if (txt)
  {
   child->PrepareWndText(txt);
   child->StdTextWrite();
  }
 child->CharPtrReturn();
 child->DefaultCheckBox(table, lines, scrl);
 child->SetParent(this);
 child->DefaultResponse();
 char* val = child->ReturnCharPtr();
 if (child) delete child;
 child = NULL;
 Invalidate();
 if (!val) out("warning in CreateCheckBoxChild, returning (char*)NULL");
 return val;
 //there can be too char* val = (char*)child->DefaultResponse()
 //must convert cast because DefaultResponse returns (void*)
 //(void*) is converted from some value by class Unused at the end of DefaultResponse()
 //can de also done by:
 //char* val = (char*)child->DefaultResponse();  (char*) cast from DefaultResponse() which returns (void*)
 //This will work because child->CharPtrReturn() was called before!
}


char* Wnd :: CreateCheckBoxVert(char* txt, char** table, int lines, int scrl = 60, int wx = 700, int wy = 500)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::CreateCheckBoxVert(%p,%p,%d,%d,%d,%d) txt,table,lines,scrl,wx,wy", txt,table,lines,scrl,wx,wy);
 out(dstr);
 if (txt) sprintf(dstr,"txt=%p:%s",txt,txt);
 out(dstr);
 if (table) sprintf(dstr,"table=(char**)%p",table);
 out(dstr);
 for (int i=0;i<lines;i++)
   {
    sprintf(dstr,"table[%d] = (char*)%p:%s", i, table[i],table[i]);
    out(dstr);
   }
#endif
 if (!table) {DoQuit(); DefaultResponse(); return NULL;}
 if (!lines)
   {
    CreateMBoxChild("Brak danych wypelniajacych strukture!\nNie moge utworzyc okna przewijania\nbez elementow");
    return (char*)("NULL");
   }
 if (child) child->WindowDies();
 child = new Wnd(odsp, wx, wy);
 if (!child) panic("memory exception in create_win_child CreateCheckBoxvert");
 child->CreateGC();
 child->SetFont("*-clean-*-10-*");
 child->SetCaption("Wybierz");
 child->DeclareResponseTable(MEDI_RESPONSES);
 child->SelectExposeFunc(StdExposeFunc);
 child->SelectConfigureFunc(DenyResizeFunc);
 child->SelectKeyPressFunc(eXtendedKeyPress);
 child->SelectKeyReleaseFunc(eXtendedKeyRelease);
 child->SelectButtonPressFunc(eXtendedButtonPress);
 child->SelectButtonReleaseFunc(eXtendedButtonRelease);
 if (txt)
  {
   child->PrepareWndText(txt);
   child->StdTextWrite();
  }
 child->CharPtrReturn();
 child->HugeCheckBox(table, lines, scrl);
 child->SetParent(this);
 child->DefaultResponse();
 char* val = child->ReturnCharPtr();
 if (child) delete child;
 child = NULL;
 Invalidate();
 if (!val) out("warning in CreateCheckBoxVert, returning (char*)NULL");
 return val;
 //there can be too char* val = (char*)child->DefaultResponse()
 //must convert cast because DefaultResponse returns (void*)
 //(void*) is converted from some value by class Unused at the end of DefaultResponse()
 //can de also done by:
 //char* val = (char*)child->DefaultResponse();  (char*) cast from DefaultResponse() which returns (void*)
 //This will work because child->CharPtrReturn() was called before!
}


char* Wnd :: CreateExCheckBoxVert(char* txt, char** table, int lines, int scrl, int wx, int wy, int new_buttons, Button** btn_ptr)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::CreateExCheckBoxVert(%p,%p,%d,%d,%d,%d,%d,%p) txt,table,lines,scrl,wx,wy,add_btns, (Button**)btns", txt,table,lines,scrl,wx,wy,new_buttons,btn_ptr);
 out(dstr);
 if (txt) sprintf(dstr,"txt=%p:%s",txt,txt);
 out(dstr);
 if (table) sprintf(dstr,"table=(char**)%p",table);
 out(dstr);
 for (int i=0;i<lines;i++)
   {
    sprintf(dstr,"table[%d] = (char*)%p:%s", i, table[i],table[i]);
    out(dstr);
   }
 if (new_buttons) sprintf(dstr,"new_buttons=%d",new_buttons);
 out(dstr);
 for (int i=0;i<new_buttons;i++)
   {
    sprintf(dstr,"btn_ptr[%d] = (Button*)%p", i, btn_ptr[i]);
    out(dstr);
   }
#endif
 if (!table) {DoQuit(); DefaultResponse(); return NULL;}
 if (child) child->WindowDies();
 child = new Wnd(odsp, wx, wy);
 if (!child) panic("memory exception in create_win_child CreateCheckBoxvert");
 child->CreateGC();
 child->SetFont("*-clean-*-10-*");
 child->SetCaption("Wybierz");
 child->DeclareResponseTable(MEDI_RESPONSES);
 child->SelectExposeFunc(StdExposeFunc);
 child->SelectConfigureFunc(DenyResizeFunc);
 child->SelectKeyPressFunc(eXtendedKeyPress);
 child->SelectKeyReleaseFunc(eXtendedKeyRelease);
 child->SelectButtonPressFunc(eXtendedButtonPress);
 child->SelectButtonReleaseFunc(eXtendedButtonRelease);
 if (txt)
  {
   child->PrepareWndText(txt);
   child->StdTextWrite();
  }
 child->CharPtrReturn();
 child->UserCheckBox(table, lines, scrl, new_buttons, btn_ptr);
 child->SetParent(this);
 child->DefaultResponse();
 char* val = child->ReturnCharPtr();
 if (child) delete child;
 child = NULL;
 Invalidate();
 if (!val) out("warning in CreateCheckBoxVert, returning (char*)NULL");
 return val;
}


bool Wnd :: HaveCheckBox() const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::HaveCheckBox ==> %d",(cbox != NULL));
 out(dstr);
#endif
 if (cbox) return true; else return false;
}

void Wnd :: UpCheckBox()         { out("wnd::upcheckbox");if (cbox) cbox->UpScroll();}
void Wnd :: DownCheckBox()       { out("wnd::downcheckbox");if (cbox) cbox->DownScroll();}  //BRIDGED CLASS FUNCTIONS
void Wnd :: UpTenCheckBox()      { out("wnd::up10checkbox");if (cbox) cbox->UpTenScroll();}
void Wnd :: DownTenCheckBox()    { out("wnd::down10checkbox");if (cbox) cbox->DownTenScroll();}

void Wnd :: SetDefaultButton(int par)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SetDefaultButton(%d)",par);
 out(dstr);
#endif
 if (par<0) return;
 if (par > RetNumBtns()-1) return;
 if (!btn) return;
 for (int i=0;i<RetNumBtns();i++) if (btn[i]->isDefault()) if (btn[i]) btn[i]->SetDefault(0);
 if (btn[par]) btn[par]->SetDefault(1);
}


int Wnd :: CreateYesNoChild(char* txt, int sel = 0, int xx = 300, int yy = 250, int b1x = DEFAULT, int b1y = DEFAULT, int b2x = DEFAULT, int b2y = DEFAULT)
{
#ifdef DBG
 char dstr[LONG_STR];
 if (txt)sprintf(dstr,"Wnd::CreateYesNoChild(%p:%s,%d,%d,%d,%d,%d,%d,%d) txt,select,x,y,b1x,b1y,b2x,b2y",txt,txt,sel,xx,yy,b1x,b1y,b2x,b2y);
 else sprintf(dstr,"Wnd::CreateYesNoChild((char*)NULL,%d,%d,%d,%d,%d,%d,%d) txt,select,x,y,b1x,b1y,b2x,b2y",sel,xx,yy,b1x,b1y,b2x,b2y);
 out(dstr);
#endif
 sel %= 2;
 if (child) child->WindowDies();
 child = new Wnd(odsp, xx, yy);
 if (!child) panic("memory exception in create_win_child CreateYesNoChild");
 child->CreateGC();
 child->SetFont("*-clean-*-10-*");
 child->SetCaption("Pytanie");
 child->DeclareResponseTable(MEDI_RESPONSES);
 child->SelectExposeFunc(StdExposeFunc);
 child->SelectConfigureFunc(DenyResizeFunc);
 child->SelectButtonPressFunc(eXtendedButtonPress);
 child->SelectKeyPressFunc(eXtendedKeyPress);
 child->SelectButtonReleaseFunc(eXtendedButtonRelease);
 child->SelectKeyReleaseFunc(eXtendedKeyRelease);
 if (txt)
   {
    child->PrepareWndText(txt);
    child->StdTextWrite();
   }
 child->SetButtonsNumber(2);
 child->CreateButton(0, "Tak", xx/4-40, yy-50, StdYesFunc);
 child->CreateButton(1, "Nie", (3*xx)/4-40, yy-50, StdNoFunc);
 child->SetDefaultButton(sel);
 child->IntPtrReturn();
 child->SetParent(this);
 child->DefaultResponse();
 int val = child->ReturnInt();
 if (child) delete child;
 child = NULL;
 Invalidate();
 return val;
 //can de also done by:
 //int val = (int)child->DefaultResponse();  (int) cast from DefaultResponse() which returns (void*)
 //This will work because child->IntPtrReturn() was called before!
}


char* Wnd :: CreateLWriteBox(char* txt, bool hidden = false, int max_len = LONG_TXT, int xx = 300, int yy = 120)
{
#ifdef DBG
 char dstr[LONG_STR];
 if (txt) sprintf(dstr,"Wnd::CreateWriteBox(%p:%s,%d,%d,%d,%d) txt,hidden,max_len,x,y",txt,txt,hidden,max_len,xx,yy);
 else sprintf(dstr,"Wnd::CreateWriteBox((char*)NULL,%d,%d,%d,%d) txt,hidden,max_len,x,y",hidden,max_len,xx,yy);
 out(dstr);
#endif
 if (child) child->WindowDies();
 child = new Wnd(odsp, xx, yy);
 if (!child) panic("memory exception in create_win_child CreateWriteBox");
 child->CreateGC();
 child->SetFont("*-clean-*-10-*");
 child->SetCaption("Wpisz Text");
 child->DeclareResponseTable(KEYB_RESPONSES);
 child->SelectExposeFunc(StdExposeFunc);
 child->SelectConfigureFunc(DenyResizeFunc);
 child->SelectKeyPressFunc(wBoxKeyPressFunc);
 child->SelectKeyReleaseFunc(wBoxKeyReleaseFunc);
 if (txt)
  {
   child->PrepareWndText(txt);
   child->StdTextWrite();
  }
 child->CharPtrReturn();
 if (!hidden) child->DefaultWriteBox(max_len);
 else child->HiddenWriteBox(max_len);
 child->SetParent(this);
 child->DefaultResponse();
 char* val = child->ReturnCharPtr();
 if (child) delete child;
 child = NULL;
 Invalidate();
 if (!val) out("warning in wnd::createwritebox, returning (char*)NULL");
 return val;
 //can de also done by:
 //char* val = (char*)child->DefaultResponse();  (char*) cast from DefaultResponse() which returns (void*)
 //This will work because child->CharPtrReturn() was called before!
}


char* Wnd :: CreateWriteBox(char* txt, bool hidden = false, int max_len = TINY_TXT, int xx = 300, int yy = 120)
{
#ifdef DBG
 char dstr[LONG_STR];
 if (txt) sprintf(dstr,"Wnd::CreateWriteBox(%p:%s,%d,%d,%d,%d) txt,hidden,max_len,x,y",txt,txt,hidden,max_len,xx,yy);
 else sprintf(dstr,"Wnd::CreateWriteBox((char*)NULL,%d,%d,%d,%d) txt,hidden,max_len,x,y",hidden,max_len,xx,yy);
 out(dstr);
#endif
 if (child) child->WindowDies();
 child = new Wnd(odsp, xx, yy);
 if (!child) panic("memory exception in create_win_child CreateWriteBox");
 child->CreateGC();
 child->SetFont("*-clean-*-10-*");
 child->SetCaption("Wpisz Text");
 child->DeclareResponseTable(KEYB_RESPONSES);
 child->SelectExposeFunc(StdExposeFunc);
 child->SelectConfigureFunc(DenyResizeFunc);
 child->SelectKeyPressFunc(wBoxKeyPressFunc);
 child->SelectKeyReleaseFunc(wBoxKeyReleaseFunc);
 if (txt)
  {
   child->PrepareWndText(txt);
   child->StdTextWrite();
  }
 child->CharPtrReturn();
 if (!hidden) child->DefaultWriteBox(max_len);
 else child->HiddenWriteBox(max_len);
 child->SetParent(this);
 child->DefaultResponse();
 char* val = child->ReturnCharPtr();
 if (child) delete child;
 child = NULL;
 Invalidate();
 if (!val) out("warning in wnd::createwritebox, returning (char*)NULL");
 return val;
 //can de also done by:
 //char* val = (char*)child->DefaultResponse();  (char*) cast from DefaultResponse() which returns (void*)
 //This will work because child->CharPtrReturn() was called before!
}


void Wnd :: SetExWriteBox(int mlen, bool hid, int msx, int msy, int mx, int my)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SetExWriteBox(%d,%d,%d,%d,%d,%d) mlen,hidden,msx,msy,mx,my",mlen,hid,msx,msy,mx,my);
 out(dstr);
#endif
 if (wbox) delete wbox;
 wbox = new Wbox(mlen, msx, msy, mx, my);
 if (!wbox) panic("MEMORY_EXCPT_IN_operator new", __LINE__,__FILE__);
 wbox->SetHidden(hid);
}


char* Wnd :: CreateExWriteBox(char* txt, bool hidden, int max_len, int xx, int yy, int msx, int msy, int mx, int my)
{
#ifdef DBG
 char dstr[LONG_STR];
 if (txt) sprintf(dstr,"Wnd::CreateExWriteBox(%p:%s,%d,%d,%d,%d,%d,%d,%d,%d) txt,hidden,max_len,x,y.msx,msy,mx,my",txt,txt,hidden,max_len,xx,yy,msx,msy,mx,my);
 else sprintf(dstr,"Wnd::CreateExWriteBox((char*)NULL,%d,%d,%d,%d,%d,%d,%d,%d) txt,hidden,max_len,x,y,msx,msy,mx,my",hidden,max_len,xx,yy,msx,msy,mx,my);
 out(dstr);
#endif
 if (child) child->WindowDies();
 child = new Wnd(odsp, xx, yy);
 if (!child) panic("memory exception in create_win_child CreateExWriteBox");
 child->CreateGC();
 child->SetFont("*-clean-*-10-*");
 child->SetCaption("Wpisz Text");
 child->DeclareResponseTable(KEYB_RESPONSES);
 child->SelectExposeFunc(StdExposeFunc);
 child->SelectConfigureFunc(DenyResizeFunc);
 child->SelectKeyPressFunc(wBoxKeyPressFunc);
 child->SelectKeyReleaseFunc(wBoxKeyReleaseFunc);
 if (txt)
   {
    child->PrepareWndText(txt);
    child->StdTextWrite();
   }
 child->CharPtrReturn();
 child->SetExWriteBox(max_len, hidden, msx, msy, mx, my);  //not yet tested :(
 child->SetParent(this);
 child->DefaultResponse();
 char* val = child->ReturnCharPtr();
 if (child) delete child;
 child = NULL;
 Invalidate();
 if (!val) out("warning in CreateexWriteox, returning (char*)NULL");
 return val;
 //can de also done by:
 //char* val = (char*)child->DefaultResponse();  (char*) cast from}
 //This will work because child->CharPtrReturn() was called before!
}


void Wnd :: WboxActivateShift(bool par)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::ActivateShift(%d)",par);
 out(dstr);
#endif
 if (wbox) wbox->SetShiftState(par);
}


bool Wnd :: WboxShiftPressed()
{
#ifdef DBG
 char dstr[LONG_STR];
 if (wbox) sprintf(dstr,"Wnd::WboxShiftPressed() ==> %d", wbox->RetShiftState());
 else sprintf(dstr,"Wnd::WboxShiftPressed() ==> !NO_WBOX_SET!");
 out(dstr);
#endif
 if (wbox) return wbox->RetShiftState();
 else return false;
}


void Wnd :: WboxKeyAction(int key)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::WboxkeyAction('%c') class_bridge Wnd ==> WBox",key);
 out(dstr);
 if (wbox)
  {
   sprintf(dstr,"Wnd::wbox Ready & Listening");
   out(dstr);
  }
#endif                                          //this is an class-bridged communication example
 if (wbox) wbox->ReactOnKey(key);
 else out("warning: cannot bridge func(Wnd&) -> class Wnd -> class WriteBox, last one doesn't exists!");
}


void Wnd :: SelectDefaults(char* capt, char* txt, int quit = Q_MINUS)   //As You can see this uses !LOTS! defaults
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SelectDefaults(%p,%p,%d) caption,win_text,quit_state",capt,txt,quit);
 out(dstr);
 if (txt)  {sprintf(dstr,"win_text=%p:%s",txt,txt);  out(dstr);}
 if (capt) {sprintf(dstr,"caption=%p,%s",capt,capt); out(dstr);}
#endif
  CreateGC();
  SetFont("*-clean-*-10-*");
  if (capt) SetCaption(capt);
  DeclareResponseTable(MOST_RESPONSES);
  SelectExposeFunc(StdExposeFunc);
  SelectConfigureFunc(DenyResizeFunc);
  if (quit == Q_MINUS) SelectKeyPressFunc(eXtendedKeyPress);
  if (quit == Q_PLUS)  SelectKeyPressFunc(eXtendedKeyPressQ);
  SelectKeyReleaseFunc(eXtendedKeyRelease);
  SelectButtonPressFunc(eXtendedButtonPress);
  SelectButtonReleaseFunc(eXtendedButtonRelease);
  if (txt)
    {
     PrepareWndText(txt);
     StdTextWrite();
    }
 out("Wnd::SelectDefaults ends...");
}


void Wnd :: WinDefaults(char* capt)   //As You can see this uses !LOTS! defaults
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::WinDefaults(%p) caption",capt);
 out(dstr);
 if (capt) {sprintf(dstr,"caption=%p:%s", capt,capt); out(dstr);}
#endif
  CreateGC();
  SetFont("*-clean-*-10-*");
  if (capt) SetCaption(capt);
  DeclareResponseTable(MEDI_RESPONSES);
  SelectExposeFunc(StdExposeFunc);
  SelectConfigureFunc(DenyResizeFunc);
  SelectKeyPressFunc(eXtendedKeyPress);
  SelectKeyReleaseFunc(eXtendedKeyRelease);
  SelectButtonPressFunc(eXtendedButtonPress);
  SelectButtonReleaseFunc(eXtendedButtonRelease);
out("Wnd::WinDefaults ends...");
}


Wnd :: Wnd(Dsp* dsp, int xx, int yy)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::Wnd(%p,%d,%d) display,x,y",dsp,x,y);
 out(dstr);
#endif
 if (dsp == NULL) return;
 if (!dsp->RetDsp()) return;
 Std_Init();
 odsp = dsp;
 cx = rx = xx;
 cy = ry = yy;
 ca = NULL;
 win = XCreateSimpleWindow
 (dsp->RetDsp(),
 RootWindow(dsp->RetDsp(), dsp->RetSNum())
 ,0, 0, cx, cy, 1,
 WhitePixel(dsp->RetDsp(), dsp->RetSNum()),
 BlackPixel(dsp->RetDsp(), dsp->RetSNum()));
 if (!win) panic("unable to set up window in Wnd(Dsp*, int, int)");
 XMapWindow(dsp->RetDsp(), win);
 XFlush(dsp->RetDsp());
 disp = odsp->RetDsp();
 out("end_of_constructor!");
}

XEvent       Wnd :: RetEvent()     const  { out("Wnd::RetEvent ==>");return this->event;      }  //Std Ret functions

int          Wnd :: RetCx()        const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::RetCx() ==> %d",cx);
 out(dstr);
#endif
 return this->cx;
}


int          Wnd :: RetCy()        const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::RetCy() ==> %d",cy);
 out(dstr);
#endif
 return this->cy;
}


int          Wnd :: RetRx()        const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::RetRx() ==> %d",rx);
 out(dstr);
#endif
 return this->rx;
}


int          Wnd :: RetRy()        const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::RetRy() ==> %d",ry);
 out(dstr);
#endif
 return this->ry;
}


int          Wnd :: RetX()         const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::RetX() ==> %d",x);
 out(dstr);
#endif
 return this->x;
}


int          Wnd :: RetY()         const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::RetY() ==> %d",ry);
 out(dstr);
#endif
 return this->y;
}


bool         Wnd :: isDone()       const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::isDone() ==> %d",done);
 out(dstr);
#endif
 return this->done;
}


Dsp*         Wnd :: RetDspStr()    const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::RetDspStr() ==> %p",odsp);
 out(dstr);
#endif
 return this->odsp;
}


GC           Wnd :: RetGC()        const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::RetGC() ==>");
 out(dstr);
#endif
 return this->gc;
}


XFontStruct* Wnd :: RetFontInfo()  const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::RetFontInfo() ==> %p",font_info);
 out(dstr);
#endif
 return this->font_info;
}


int          Wnd :: RetFontH()     const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::RetFontH() ==> %d",font_h);
 out(dstr);
#endif
 return this->font_h;
}


Display*     Wnd :: RetDisp()      const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::RetDisp() ==> %p",disp);
 out(dstr);
#endif
 return this->disp;
}


long         Wnd :: WhatHappened() const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::WhatHappened() ==> %d",(int)event.type);
 out(dstr);
#endif
 return this->event.type;
}


int          Wnd :: RetDrag()      const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::RetDrag() ==> %d",drag);
 out(dstr);
#endif
 return this->drag;
}


Window       Wnd :: RetWin()       const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::RetWin() ==>");
 out(dstr);
#endif
 return this->win;
}


int          Wnd :: RetLines()     const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::Retlines() ==> %d",lines);
 out(dstr);
#endif
 return this->lines;
}


char*        Wnd :: RetCaption()   const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::RetCaption() ==> %p",caption);
 out(dstr);
 if (caption)
    {
     sprintf(dstr,"caption=%p:%s", caption,caption);
     out(dstr);
    }
#endif
 if (caption)return this->caption; else {out("warning in retcaption, returning NULL") ;return NULL;}
}


char**       Wnd :: RetWndText()   const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::RetWndText() ==> %p",text);
 out(dstr);
 if ((text) && (lines))
 for (int i=0;i<lines;i++)
   {
    sprintf(dstr,"text[%d]=%p:%s", i, text[i], text[i]);
    out(dstr);
   }
#endif
 if (text) return this->text;  else {return NULL;out("warning in wnd::rettext, returning NULL");}
}


int          Wnd :: RetNumBtns()   const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::RetNumBtns() ==> %d",num_btns);
 out(dstr);
#endif
 return this->num_btns;
}


int          Wnd :: RetNumScrl()   const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::RetNumScrl() ==> %d",num_scrl);
 out(dstr);
#endif
 return this->num_scrl;
}


Unused*      Wnd :: RetDta()       const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::RetDta() ==> %p",dta);
 out(dstr);
#endif
 return dta;
}


int          Wnd :: ReturnInt()    const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::ReturnInt() ==> dependent");
 out(dstr);
#endif
 if (dta) return dta->ReturnInt(); else return 0;
}


void*        Wnd :: ReturnVoid()   const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::ReturnVoid() ==> dependent");
 out(dstr);
#endif
 if (dta) return dta->ReturnVoid(); else return NULL;
}


char*        Wnd :: ReturnCharPtr()const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::ReturnCharPtr() ==> dependent");
 out(dstr);
#endif
 if (dta) return dta->ReturnCharPtr(); else {out("warning: Unused* struct uninitialized!"); return NULL;}
}


void Wnd :: SetButtonName(int i, char* textii)
{
 if (i<0 && i>=num_btns) return ;
 if (!textii) return;
 btn[i]->SetBtnName(textii);
}


Button*      Wnd :: RetBtn(int i)  const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::Retbtn(%d) ==> wait...",i);
 out(dstr);
 if ((i>=0) && (i<num_btns))
   {
    sprintf(dstr,"far_ptr=%p", btn[i]);
    out(dstr);
   }
#endif
 if ((i<0) || (i>=num_btns)) return NULL;
 if (btn) return this->btn[i]; else return NULL;
}


Wbox*        Wnd :: ReturnWriteBox()const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::ReturnWriteBox() ==> %p",wbox);
 out(dstr);
#endif
 if (wbox) return wbox; else return NULL;
}


CheckBox*    Wnd :: ReturnCheckBox()const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::ReturnCheckBox() ==> %p",cbox);
 out(dstr);
#endif
 if (cbox) return cbox; else return NULL;
}


Wnd*         Wnd :: ReturnChild()   const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::ReturnChild() ==> %p",child);
 out(dstr);
#endif
 if (child) return child; else return NULL;
}

                                                    //Std setFunction (do not used indirect they're dangerous)
                                                    //Unless You !REALLY! know what you're doing!
                                                    //Possible You'll see SIGSEGV sent by the kernel

void    Wnd :: SetSize(int x, int y)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SetSize(%d,%d)",x,y);
 out(dstr);
#endif
 SetRx(x); SetRy(y); SetCx(x); SetCy(y);
}


void    Wnd :: SetCx(int par)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SetCx(%d)",par);
 out(dstr);
#endif
 this->cx   = par; XResizeWindow(disp, win, cx,cy);
}


void    Wnd :: SetCy(int par)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SetCy(%d)",par);
 out(dstr);
#endif
 this->cy   = par; XResizeWindow(disp, win, cx,cy);
}


void    Wnd :: SetRx(int par)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SetRx(%d)",par);
 out(dstr);
#endif
 this->rx   = par;
}


void    Wnd :: SetRy(int par)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SetRy(%d)",par);
 out(dstr);
#endif
 this->ry   = par;
}


void    Wnd :: SetX(int par )
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SetX(%d)",par);
 out(dstr);
#endif
 this->x    = par;
}


void    Wnd :: SetY(int par )
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SetY(%d)",par);
 out(dstr);
#endif
 this->y    = par;
}


void    Wnd :: AboutQuit(bool val)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::AboutQuit(%d)",val);
 out(dstr);
#endif
 this->done = val;
}


void    Wnd :: SetWin(Window& w)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SetWin(@some_address@)");
 out(dstr);
#endif
 if (w) this->win  = w;
}


void    Wnd :: SetDsp(Dsp* d)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SetDsp(%p) DANGEROUS",d);
 out(dstr);
#endif
 this->odsp = d;
}


void    Wnd :: SetGC(GC& g)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SetGC(@some_address@)");
 out(dstr);
#endif
 this->gc   = g;
}


void    Wnd :: SetDisplay(Display* d)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SetDisplay(%p)",d);
 out(dstr);
#endif
 if (d) this->disp = d;
}


void    Wnd :: SetDrag(int d)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SetDrag(%d)",d);
 out(dstr);
#endif
 this->drag = d;
}


void    Wnd :: ImprDrag(int d)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::ImprDrag(%d)",d);
 out(dstr);
#endif
 this->drag += d;
}


void    Wnd :: SetLines(int par)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::Setlines(%d)",par);
 out(dstr);
#endif
 this->lines= par;
}


void    Wnd :: SetWndText(char** t)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SetWndText(%p) DANGEROUS_BECAUSE_LINES_REMAINS_OLD",t);
 out(dstr);
 if (t) {sprintf(dstr,"t=(char**)%p",t); out(dstr);}
 if (t) for (int i=0;i<lines;i++)
   {
    sprintf(dstr,"t[%d]=%p:%s",i,t[i],t[i]);
    out(dstr);
   }
#endif
 if (text) this->text = t;
}


void    Wnd :: SetNumBtns(int num)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SetNumBtns(%d)",num);
 out(dstr);
#endif
 this->num_btns = num;
}


void    Wnd :: SetBtn(Button* bt, int num)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SetBtn(%p,%d) btn,num",bt,num);
 out(dstr);
#endif
 if (btn) this->btn[num] = bt;
}


void    Wnd :: DoQuit()
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::DoQuit()  >>>> Set wnd::done to true, OK");
 out(dstr);
#endif
 this->done = 1;
}


void    Wnd :: SetChild(Wnd* w)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SetChild(%p) means ? adopt ?",w);
 out(dstr);
#endif
 if (w) this->child  = w;
}


void    Wnd :: SetWriteBox(Wbox* w)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SetWriteBox(%p)",w);
 out(dstr);
#endif
 if (w) this->wbox  = w;
}


void    Wnd :: SetCheckBox(CheckBox* w)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SetCheckBox(%p)",w);
 out(dstr);
#endif
 if (w) this->cbox  = w;
}


void Wnd :: Std_Init()                  //Guaranted SIGSEGV without this private function called by all constructors!
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::Std_Init() now the quest begins...");
 out(dstr);
#endif
 gx = 0;
 gy = 0;
 lx = -1;
 ly = -1;
 out("gx=0,gy=0");
 have_palette = false;
 have_ca = false;
 cur_color = RGB(0xff, 0xff, 0xff);
 ProcFunc = NULL;
 text_disabled = false;
 SetupFunc = NULL;
 ExposeFunc = NULL;
 ConfigureFunc = NULL;
 ExposeXFunc = NULL;
 ConfigureXFunc = NULL;
 ButtonPressFunc = NULL;
 ButtonReleaseFunc = NULL;
 ButtonPressXFunc = NULL;
 ButtonReleaseXFunc = NULL;
 MotionNotifyFunc = NULL;
 MotionNotifyXFunc = NULL;
 KeyPressFunc = NULL;
 KeyReleaseFunc = NULL;
 KeyPressXFunc = NULL;
 KeyReleaseXFunc = NULL;                // This eleminated posibility of use uninitialized pointers!
 OtherEventFunc = NULL;                 // The real Nightmare in C, C++
 out("all func=0, no palette, have frames no graphisc...");
 x = y = done = 0;                      // But I never wanted C, C++ to initialize Pointers
 win = 0;                               //It would slow down exe-code (I know when to initialize ptr)
 odsp = NULL;
 font_info = NULL;
 font_h = 0;
 disp = NULL;
 drag = 0;
 lines = 0;
 caption = NULL;
 text = NULL;
 dta = NULL;
 num_btns = 0;
 child = NULL;
 parent = NULL;
 btn = NULL;
 wbox = NULL;
 cbox = NULL;
 deleted = 0;
 apressed = 0;
 graph = NULL;
 have_frames = true;
 have_graph = false;
 num_scrl = 0;
 scrl = NULL;
 out("no scrolls, buttons, write-boxes, check-boxes, child & parent NULL");
 out("no text,captions, all pointers 're set to NULL, all counters to 0");
 out("Now init main_pointer_event");
 Init_event_ptr();
}


inline bool Wnd :: StartedDrawing() const
{
 if (lx>0) return true;
 else return false;
}

inline void Wnd :: StopDrawingLines() { lx = -1; }

void Wnd :: SetDrawPosX(int par)
{
 //if (par<0 && par>=RetCx()) return;   //FIXME secure version
 lx = par;
}


void MyDrawLine(Display* dsp, Window& win, GC& gc, Wnd* wnd, int sx, int sy, int ex, int ey, ulong col)
{
 int lx = (sx<=ex) ? sx : ex;
 int mx = (sx>=ex) ? sx : ex;
 int ly = (sy<=ey) ? sy : ey;
 int my = (sy>=ey) ? sy : ey;
 if (lx==mx) { for (int i=ly;i<=my;i++) wnd->DrawWndPixel(col, lx, i); return; }
 if (ly==my) { for (int i=lx;i<=mx;i++) wnd->DrawWndPixel(col, i, ly); return; }
 if ((mx-lx)>=(my-ly))
   { for (int x=lx;x<=mx;x++) wnd->DrawWndPixel(col, x, ly+(((ey-sy)*(x-lx))/(ex-sx))); return; }
 if ((mx-lx)<(my-ly))
   { for (int y=ly;y<=my;y++) wnd->DrawWndPixel(col, lx+(((ex-sx)*(y-ly))/(ey-sy)), y); return; }
}


inline void Wnd :: DrawLineTo(int nx, int ny, ulong col)
{
 //printf("draw_line! lx=%d ly=%d nx=%d ny=%d\n",lx,ly,nx,ny);
 XSetForeground(disp, gc, col);
 MyDrawLine(disp, win, gc, this, lx, ly, nx, ny, col);
}


void Wnd :: DrawMovedLine(int nx, int ny, ulong col, int mx, int my)
{
 //printf("draw_line! lx=%d ly=%d nx=%d ny=%d\n",lx,ly,nx,ny);
 XSetForeground(disp, gc, col);
 MyDrawLine(disp, win, gc, this, lx+mx, ly+my, nx+mx, ny+my, col);
}


void Wnd :: SetDrawPosY(int par)
{
 //if (par<0 && par>=RetCy()) return;   //FIXME secure version
 ly = par;
}


ulong Wnd :: RetCurColor() const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::RetCurColor ==> %d",(int)cur_color);
 out(dstr);
#endif
 return cur_color;
}


void Wnd :: EnablePalette(bool par)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::EnablePalette(%d)",par);
 out(dstr);
#endif
 have_palette = par;
}


bool Wnd :: HavePalette() const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::HavePalette() ==> %d",have_palette);
 out(dstr);
#endif
 return have_palette;
}


void Wnd :: SetCurColor(ulong col)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SetCurColor(%d)",(int)col);
 out(dstr);
#endif
#if defined BIT24DISP
 if (col > 0xffffff) return;
#elif defined BIT16DISP
 if (col > 0xffff) return;
#endif
 cur_color = col;
}


void Wnd :: ModColor(int r, int g, int b)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::ModColor(%d,%d,%d) Red, Green, Blue",r,g,b);
 out(dstr);
#endif
 int rt = ReturnRed(cur_color);
 rt += r;
 if (rt > 0xff) rt=0xff;
 if (rt < 0)    rt = 0;
 int gt = ReturnGreen(cur_color);
 gt += g;
 if (gt > 0xff) gt=0xff;
 if (gt < 0)    gt = 0;
 int bt = ReturnBlue(cur_color);
 bt += b;
 if (bt > 0xff) bt=0xff;
 if (bt < 0)    bt = 0;
 SetCurColor(RGB(rt,gt,bt));
}

void Wnd :: CharPtrReturn()          //Instrue Unused* struct to listen for char* values and
{                                    //tell it to return that value after DefaultResponse() as casted to
 out("Wnd::CharPtrReturn()");          //(void*) pointer
 if (dta) delete dta;
 dta = NULL;
 if (!dta) dta = new Unused;
 if (!dta) panic("memory exception in CharPtrReturn, wnd::dta");
 dta->type = CHAR_PTR;
}

void Wnd :: IntPtrReturn()           //analogical for Int value cast (int) -> (void*) and then user
{                                    // (void*) -> (int)
 out("Wnd::IntPtrReturn()");
 if (dta) delete dta;
 dta = NULL;
 if (!dta) dta = new Unused;
 if (!dta) panic("memory exception in IntPtrReturn, wnd::dta");
 dta->type = INT_PTR;
}

void Wnd :: VoidPtrReturn()           //analogical for Int value cast (int) -> (void*) and then user
{                                    // (void*) -> (int)
 out("Wnd::VoidPtrReturn");
 if (dta) delete dta;
 dta = NULL;
 if (!dta) dta = new Unused;
 if (!dta) panic("memory exception in VoidPtrReturn, wnd::dta");
 dta->type = VOID_PTR;
}


void Wnd :: StdTextWrite() // put char** data on Window using 'lines' variable as 'limiter' of table char**
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::StdTextWrite()");
 out(dstr);
 for (int i=0;i<lines;i++)
   {
    sprintf(dstr,"text[%d]=%p:%s",i,text[i],text[i]);
    out(dstr);
   }
#endif
 if (text_disabled) return ;
 if (!text) return;
 XSetForeground(disp,gc,RGB(0xff,0xff,0xff));
 for (int i=0;i<lines;i++)
   {
     if (text[i]) XDrawString(disp, win, gc, 20, 20 + i*(font_h+3), text[i], strlen(text[i]));
   }
}


void Wnd :: BlackWings() // puts black rectangles onto text!
{
 /*out("wnd::blackwings");
 if (!text) return;
 for (int i=0;i<lines;i++)
   {
     XFillA(disp, win, gc, 0, 15, 15+(i-1)*(font_h+3), strlen(text[i])*7+9, font_h+7);
   }*/
 out("Wnd::BlackWings() writing text in Black-RGB (0x0)");
 if (!text) return;
 XSetForeground(disp,gc,0);
 for (int i=0;i<lines;i++)
   {
     if (text[i]) XDrawString(disp, win, gc, 20, 20 + i*(font_h+3), text[i], strlen(text[i]));
   }
}


void Wnd :: CreateGC()   //if this fails PANIC
{
 out("wnd::creategc");
 gc = XCreateGC(disp, win, 0, NULL);
 if (gc < 0) {printf("GC failed to create!\n"); panic("cannot create Graphics Context") ;}
 XSetForeground(disp, gc, WhitePixel(disp, odsp->RetSNum()));
 XSetBackground(disp, gc, BlackPixel(disp, odsp->RetSNum()));
}


bool Wnd :: validBtn(int num) const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::validBtn(%d) ==> wait",num);
 out(dstr);
#endif
 if ((num <0)  || (num>=num_btns)) return false;
#ifdef DBG
 if (btn[num]) sprintf(dstr,"OK, correct button.");
 else sprintf(dstr,"warning: INVALID_BTN");
 out(dstr);
#endif
 if (!btn) return false; if (btn[num]) return true; else return false;
}


bool Wnd :: validScrl(int num) const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::validScrl(%d) ==> wait",num);
 out(dstr);
#endif
 if ((num <0)  || (num>=num_scrl)) return false;
#ifdef DBG
 if (scrl[num]) sprintf(dstr,"OK, correct scroll.");
 else sprintf(dstr,"warning: INVALID_SCRL");
 out(dstr);
#endif
 if (!scrl) return false; if (scrl[num]) return true; else return false;
}

void Wnd :: DeclareResponseTable(long int par)  //decalare which parts of DefaultResponse() activate
{                                               //user-defined or use MACROS #define *_RESPONSE
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::DeclareResponseTable(mask=%d) XSelectInput will be called immadiately!",(int)par);
 out(dstr);
#endif
 XSelectInput(disp, win, par);
}


void Wnd :: SetFont(char* font_name)            //if it fails PANIC
{
#ifdef DBG
 char dstr[LONG_STR];
 if (font_name) sprintf(dstr,"Wnd::SetFont(%p:%s)",font_name,font_name);
 else sprintf(dstr,"Wnd::SetFont((char*)NULL) FONT_NAME_INVALID >>> PANIC ? <<<");
 out(dstr);
#endif
  if (!font_name) panic("font name seems to be invalid");
  font_info = XLoadQueryFont(disp, font_name);
  if (!font_info){ printf("XLoadQueryFont: failed loading font '%s'\n", font_name);panic("cannot load font name specified") ;}
  XSetFont(disp, gc, font_info->fid);
  font_h = font_info->ascent + font_info->descent;
}


void Wnd :: SetCaption(char* window_name)  //window caption
{
#ifdef DBG
 char dstr[LONG_STR];
 if (window_name) sprintf(dstr,"Wnd::SetCaption(%p:%s)",window_name,window_name);
 else sprintf(dstr,"Wnd::SetCaption((char*NULL)), warning setting NULL");
 out(dstr);
#endif
  if (!window_name) return;
  if (caption) delete [] caption;
  caption = NULL;
  int rc = XStringListToTextProperty(&window_name,1,&window_name_property);
  if (!rc) printf("cannot set window name!\n");
  else XSetWMName(disp, win, &window_name_property);
  caption = new char[strlen(window_name)+3];
  if (!caption) panic("memory exception in wnd::caption");
  strcpy(caption, window_name);
}


void Wnd :: SetButtonsNumber(int num)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SetButtonsNumber(%d)",num);
 out(dstr);
#endif
 if (btn) for (int i=1;i<RetNumBtns();i++) if (btn[i]) delete btn[i];
 if (btn) delete [] btn;
 btn = new Button*[num];                             //FIXME should be Button*[] or (Button*)[] ? ?? ???
 if (!btn) panic("memory exception in wnd::btn");
 for (int i=0;i<num;i++) {btn[i] = new Button;if (!btn[i]) panic("memory exception in btn[]");}
 num_btns = num;
}


void Wnd :: SetScrlNumber(int num)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SetScrlNumber(%d)",num);
 out(dstr);
#endif
 if (scrl) for (int i=1;i<RetNumScrl();i++) if (scrl[i]) delete scrl[i];
 if (btn) delete [] scrl;
 scrl = new Scroll*[num];                             //FIXME should be Button*[] or (Button*)[] ? ?? ???
 if (!scrl) panic("memory exception in wnd::scrl");
 for (int i=0;i<num;i++) {scrl[i] = new Scroll;if (!scrl[i]) panic("memory exception in scrl[]");}
 num_scrl = num;
}


void Wnd :: DefaultWriteBox(int len)                    //occupies down-part of the Window
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::DefaultWriteBox(%d) len",len);
 out(dstr);
#endif
 if (wbox) delete wbox;
 wbox = new Wbox(len, 10, RetCy()-40, RetCx()-20, 30);
 if (!wbox) panic("memory exception in wnd::wbox");
}


void Wnd :: HiddenWriteBox(int len)                    //occupies down-part of the Window
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::HiddennWriteBox(%d) len",len);
 out(dstr);
#endif
 if (wbox) delete wbox;
 wbox = new Wbox(len, 10, RetCy()-40, RetCx()-20, 30);
 if (!wbox) panic("memory exception in wnd::wbox");
 wbox->SetHidden(1);
}


void Wnd :: DefaultCheckBox(char** data, int line, int scroll)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::DefaultCheckBox(%p,%d,%d) data,lines,scroll",data,line,scroll);
 out(dstr);
 for (int i=0;i<line;i++)
   {
    sprintf(dstr,"data[%d]=%p:%s",i,data[i],data[i]);
    out(dstr);
   }
#endif                          //create virtual struct and
 if (!data) {DoQuit(); DefaultResponse();return;}                        //standard Up-Down buttons, others call temporary "unknown"
 if (!line) return;
 if (!scroll) return;
 if (cbox) delete cbox;
 cbox = new CheckBox(data, line, scroll);
 if (!cbox) panic("memory exception in wnd::cbox");
 int less = (line > scroll) ? scroll : line;
 SetButtonsNumber(less+4);
 btn[less]->SetBtnName("W gore");
 btn[less]->SetBtnFunc(StdUpFunc);
 btn[less]->SetDefault(0);
 btn[less]->SetBtnSize(150, 24);
 btn[less]->SetBtnPos(RetCx()/2-75, 75);
 btn[less+1]->SetBtnName("W dol");
 btn[less+1]->SetBtnFunc(StdDownFunc);
 btn[less+1]->SetDefault(1);
 btn[less+1]->SetBtnSize(150, 24);
 btn[less+1]->SetBtnPos(RetCx()/2-75, 75+(less+1)*25);
 btn[less+2]->SetBtnName("+10");
 btn[less+2]->SetBtnFunc(StdUpTenFunc);
 btn[less+2]->SetDefault(0);
 btn[less+2]->SetBtnSize(150, 24);
 btn[less+2]->SetBtnPos(RetCx()/2-75, 50);
 btn[less+3]->SetBtnName("-10");
 btn[less+3]->SetBtnFunc(StdDownTenFunc);
 btn[less+3]->SetDefault(0);
 btn[less+3]->SetBtnSize(150, 24);
 btn[less+3]->SetBtnPos(RetCx()/2-75, 75+(less+2)*25);
 for (int i=0;i<less;i++)
   {
    btn[i]->SetBtnFunc(RetOwnNameFunc);
    btn[i]->SetDefault(0);
    btn[i]->SetBtnSize(150, 24);
    btn[i]->SetBtnName("unknown");                  //temporary to first ExposeFunc (to avoid uninitialized char* name in button)
    btn[i]->SetBtnPos(RetCx()/2-75, 75+(i+1)*25);   //because DrawButtons() can be called faster than ExposeFunc() and
   }                                                //wchich calls DrawCheckBox()
}


void Wnd :: HugeCheckBox(char** data, int line, int scroll)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::HugeCheckBox(%p,%d,%d) data,lines,scroll",data,line,scroll);
 out(dstr);
 for (int i=0;i<line;i++)
   {
    sprintf(dstr,"data[%d]=%p:%s",i,data[i],data[i]);
    out(dstr);
   }
#endif                           //create virtual struct and
 if (!data) {DoQuit(); DefaultResponse();return;}   //standard Up-Down buttons, others call temporary "unknown"
 if (!line) return;
 if (!scroll) return;
 if (cbox) delete cbox;
 cbox = new CheckBox(data, line, scroll);
 if (!cbox) panic("memory exception in wnd::cbox");
 int less = (line > scroll) ? scroll : line;
 SetButtonsNumber(less+4);
 btn[less]->SetBtnName("W gore");
 btn[less]->SetBtnFunc(StdUpFunc);
 btn[less]->SetDefault(0);
 btn[less]->SetBtnSize(100, 23);
 btn[less]->SetBtnPos(270, 25);
 btn[less+1]->SetBtnName("W dol");
 btn[less+1]->SetBtnFunc(StdDownFunc);
 btn[less+1]->SetDefault(1);
 btn[less+1]->SetBtnSize(100, 23);
 btn[less+1]->SetBtnPos(380,25);
 btn[less+2]->SetBtnName("+10");
 btn[less+2]->SetBtnFunc(StdUpTenFunc);
 btn[less+2]->SetDefault(0);
 btn[less+2]->SetBtnSize(100, 23);
 btn[less+2]->SetBtnPos(160,25);
 btn[less+3]->SetBtnName("-10");
 btn[less+3]->SetBtnFunc(StdDownTenFunc);
 btn[less+3]->SetDefault(0);
 btn[less+3]->SetBtnSize(100, 23);
 btn[less+3]->SetBtnPos(490,25);
 int k = 0;
 int j = 0;
 for (int i=0;i<less;i++)
   {
    btn[i]->SetBtnFunc(RetOwnNameFunc);
    btn[i]->SetDefault(0);
    btn[i]->SetBtnSize(150, 23);
    btn[i]->SetBtnName("unknown");
    btn[i]->SetBtnPos(25+(j*160), 70+(24*k));
    k++;
    if (k>=15) {k=0; j++;}
   }
}


void Wnd :: UserCheckBox(char** data, int line, int scroll, int num, Button** tab)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::DefaultCheckBox(%p,%d,%d,%d,%p) data,lines,scroll,num_add_btns,btns",data,line,scroll,num,tab);
 out(dstr);
 for (int i=0;i<line;i++)
   {
    sprintf(dstr,"data[%d]=%p:%s",i,data[i],data[i]);
    out(dstr);
   }
 sprintf(dstr,"Addictional buttons>>");
 out(dstr);
 for (int i=0;i<num;i++)
   {
    sprintf(dstr,"btn[%d]=%p",i,tab[i]);
    out(dstr);
   }
#endif                          //create virtual struct and
 if (!tab) panic("Cannot set NULL Buttons, destroying cache!");
 if (!data) {DoQuit(); DefaultResponse(); return;} //standard Up-Down buttons, others call temporary "unknown"
 if (!line) return;
 if (!scroll) return;
 if (cbox) delete cbox;
 cbox = new CheckBox(data, line, scroll);
 if (!cbox) panic("memory exception in wnd::cbox");
 int less = (line > scroll) ? scroll : line;
 SetButtonsNumber(less+4+num);
 for (int i=less+4;i<less+4+num;i++)
   {
    if (tab[i-(less+4)]) SetBtn(tab[i-(less+4)], i);
   }
 btn[less]->SetBtnName("W gore");
 btn[less]->SetBtnFunc(StdUpFunc);
 btn[less]->SetDefault(0);
 btn[less]->SetBtnSize(100, 23);
 btn[less]->SetBtnPos(370, 25);
 btn[less+1]->SetBtnName("W dol");
 btn[less+1]->SetBtnFunc(StdDownFunc);
 btn[less+1]->SetDefault(1);
 btn[less+1]->SetBtnSize(100, 23);
 btn[less+1]->SetBtnPos(480,25);
 btn[less+2]->SetBtnName("+10");
 btn[less+2]->SetBtnFunc(StdUpTenFunc);
 btn[less+2]->SetDefault(0);
 btn[less+2]->SetBtnSize(100, 23);
 btn[less+2]->SetBtnPos(260,25);
 btn[less+3]->SetBtnName("-10");
 btn[less+3]->SetBtnFunc(StdDownTenFunc);
 btn[less+3]->SetDefault(0);
 btn[less+3]->SetBtnSize(100, 23);
 btn[less+3]->SetBtnPos(590,25);
 int k = 0;
 int j = 0;
 for (int i=0;i<less;i++)
   {
    btn[i]->SetBtnFunc(RetOwnNameFunc);
    btn[i]->SetDefault(0);
    btn[i]->SetBtnSize(150, 23);
    btn[i]->SetBtnName("unknown");
    btn[i]->SetBtnPos(50+(j*160), 70+(24*k));
    k++;
    if (k>=12) {k=0; j++;}
   }
}


void Wnd :: CreateCenterOKButton()
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::CreatecenterOKButton()");
 out(dstr);
#endif
 SetButtonsNumber(1);       //Set to 2 & uncomment to have Cancel Button here
 btn[0]->SetBtnName("OK");
 btn[0]->SetBtnFunc(StdOKFunc);
 btn[0]->SetDefault(1);
 btn[0]->SetBtnPos(cx/2-btn[0]->RetX()/2, (3*cy)/4-btn[0]->RetY()/2);
 DrawButtons();
}


void Wnd :: CreateOKButton(int xx, int yy)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::CreateOKButton(%d,%d) x,y",xx,yy);
 out(dstr);
#endif
 SetButtonsNumber(1);       //Set to 2 & uncomment to have Cancel Button here
 btn[0]->SetBtnName("OK");
 btn[0]->SetBtnFunc(StdOKFunc);
 btn[0]->SetDefault(1);
 btn[0]->SetBtnPos(xx, yy);
 DrawButtons();
}


void Wnd :: CreateCenterOtherButton(int bbb)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::CreatecenterOtherButton(%d) Btn_ID",bbb);
 out(dstr);
#endif
 SetButtonsNumber(1);       //Set to 2 & uncomment to have Cancel Button here
 if (bbb == MB_YES) btn[0]->SetBtnName("Tak");
 else if (bbb == MB_NO) btn[0]->SetBtnName("Nie");
 else if (bbb == MB_RETRY) btn[0]->SetBtnName("Ponow");
 else if (bbb == MB_CANCEL) btn[0]->SetBtnName("Anuluj");
 else btn[0]->SetBtnName("Nieznany");
 btn[0]->SetBtnFunc(StdOKFunc);
 btn[0]->SetDefault(1);
 btn[0]->SetBtnPos(cx/2-btn[0]->RetX()/2, (3*cy)/4-btn[0]->RetY()/2);
 DrawButtons();
}


void Wnd :: CreateOtherButton(int xx, int yy, int bbb)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::CreateOtherButton(%d,%d,%d) x,y,Btn_ID",xx,yy,bbb);
 out(dstr);
#endif
 SetButtonsNumber(1);       //Set to 2 & uncomment to have Cancel Button here
 if (bbb == MB_YES) btn[0]->SetBtnName("Tak");
 else if (bbb == MB_NO) btn[0]->SetBtnName("Nie");
 else if (bbb == MB_RETRY) btn[0]->SetBtnName("Ponow");
 else if (bbb == MB_CANCEL) btn[0]->SetBtnName("Anuluj");
 else btn[0]->SetBtnName("Nieznany");
 btn[0]->SetBtnFunc(StdOKFunc);
 btn[0]->SetDefault(1);
 btn[0]->SetBtnPos(xx, yy);
 DrawButtons();
}


void Wnd :: SetButtonCtrl(int num, void (*f)(Wnd&, void*), void* ptr)  //quite funny declaration :-)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SetButtonCtrl(num=%d,func=%p,add_data=%p)",num,f,ptr);
 out(dstr);
#endif
 if ((num < 0) || (num >= RetNumBtns())) return;
 if (f) btn[num]->SetBtnFunc(f);
 if (ptr) btn[num]->SetVoid(ptr);
}


void Wnd :: DrawButtons()
{
 out("wnd::drawbuttons");
 for (int i=0;i<RetNumBtns();i++) if (validBtn(i)) DrawBtn(i);
}


void Wnd :: DrawCheckBox()
{
 out("wnd::drawcheckbox");
 if (cbox)
   {
    for (int i=cbox->RetScrollDown();i<cbox->RetScrollPeak();i++)
      {
       if (((i-cbox->RetScrollDown()) < RetNumBtns()) && ((i-cbox->RetScrollDown()) >= 0))
         btn[i-cbox->RetScrollDown()]->SetBtnName(cbox->RetName(i));
      }
    DrawButtons();                       //must be called although called by ExposeFuck
   }                                     //because every click should refresh buttons
}                                        //and StdUpFunc & StdDownFunc call DrawCheckBox
                                         //but Don't call ExposeFunc (so we must refresh it beyond ExposeFunc)

void Wnd :: DrawWBox()
{
 out("wnd::drawwbox");
 if (wbox)
  {
   DrawFrame(disp, win, RGB(0x0, 0x0, 0x0), gc, 0,
    wbox->RetY()/2, wbox->RetSx()+5, wbox->RetEx()-5, wbox->RetSy()+5, wbox->RetEy()-5);
   DrawFrame(disp, win, RGB(0xaa, 0xaa, 0xaa), gc, RGB(6,6,6),
    5, wbox->RetSx(), wbox->RetEx(), wbox->RetSy(), wbox->RetEy());
    if (wbox->HaveText())
      {
       if (!wbox->isHidden()) XDrawString(disp, win ,gc, wbox->RetSx()+7, wbox->RetSy()+5+font_h, wbox->RetText(),wbox->RetTextLen());
       else
         {
          char* tmp;
	  tmp = new char[wbox->RetTextLen()];
	  if (!tmp) return;
	  for (int i=0;i<wbox->RetTextLen();i++) tmp[i] = '*';
	  XDrawString(disp, win ,gc, wbox->RetSx()+7, wbox->RetSy()+5+font_h, tmp,wbox->RetTextLen());
	  if (tmp) delete [] tmp;
	 }
      }
  }
}


void Wnd :: EnableFrames(bool par)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::Enableframes(%d)",par);
 out(dstr);
#endif
 have_frames = par;
}


void Wnd :: DelGraph()
{
 out("wnd::delgraph");
 if (!graph) return;
 for (int i=1;i<RetCx();i++) if (graph[i]) delete [] graph[i];
 if (graph) delete [] graph;
}


bool Wnd :: HaveFrames() const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::Haveframes ==> %d",have_frames);
 out(dstr);
#endif
 return have_frames;
}


bool Wnd :: HaveGraph() const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::HaveGraph() ==> %d",have_graph);
 out(dstr);
#endif
 return have_graph;
}


void Wnd :: SetGraph(ulong** graphics)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SetGraph((ulong**)%p)",graphics);
 out(dstr);
#endif
 if (!HaveGraph())
   {
    out("warning in wnd::setgraph, window is not prepared for this!, use EnableGraph!");
    return ;
   }
 graph = graphics;
}


void Wnd :: SetWndPixel(ulong col, int x, int y)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SetWndPixel(%d,%d,%d) col,x,y",(int)col,x,y);
 out(dstr);
#endif
 if (!graph)  return;
 //if ((x<0) || (x>=RetCx())) return;   //FIXME this is for improve performance!
 //if ((y<0) || (y>=RetCy())) return;
 graph[x][y] = col;
}


ulong Wnd :: RetWndPixel(int x, int y) const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::RetWndPixel(%d,%d) x,y ==> wait",x,y);
 out(dstr);
#endif
 if (!graph)  return 0x0;
 //if ((x<0) || (x>=RetCx())) return 0x0;
 //if ((y<0) || (y>=RetCy())) return 0x0;   //FIXME this is for improve performance!
#ifdef DBG
 sprintf(dstr,"color ==> %d",(int)graph[x][y]);
 out(dstr);
#endif
 return graph[x][y];
}


void Wnd :: DrawWndPixel(ulong col, int x, int y)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::DrawWndPixel(%d,%d,%d) col,x,y",(int)col,x,y);
 out(dstr);
#endif
 if (!graph) return;
 if ((x<0) || (x>=RetCx())) return;
 if ((y<0) || (y>=RetCy())) return;    //FIXME this is for improve performance!
 graph[x][y] = col;
 XSetForeground(disp, gc, graph[x][y]);
 XDrawPoint(disp, win ,gc, x, y);
}


void Wnd :: DrawWindow()
{
 out("wnd::drawwindow");
 if (HaveFrames())
   {
    DrawHalfFrame(disp, win, RGB(0x0, 0x0, 0xf1), gc, -RGB(0x0,0x0,0x14),
    12, 0, RetCx(), 0, RetCy(), 1);
    DrawHalfFrame(disp, win, RGB(0x0, 0x0, 0xf1), gc, -RGB(0x0,0x0,0x14),
    12, 0, RetCx(), 0, RetCy(), 0);
   }
 if (HaveGraph())
   {
    int xx = RetRx();
    int yy = RetRy();
    if (xx != gx) return;
    if (yy != gy) return;
    for (register int i=0;i<xx;i++) for (register int j=0;j<yy;j++)
      {
       XSetForeground(disp, gc, graph[i][j]);
       XDrawPoint(disp, win ,gc, i, j);
      }
   }
}


void Wnd :: DrawScrolls()
{
 out("wnd::drawscrolls");
 if (!scrl) return;
 for (int i=0;i<RetNumScrl();i++) DrawScrl(i);
}


void Wnd :: FadeScroll(int num)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::FadeScroll(%d)",num);
 out(dstr);
#endif
 if (!scrl) return;
 if (!validScrl(num)) return;
 int SX = scrl[num]->RetSx();
 int SY = scrl[num]->RetSy()-5;
 int EY = SY + scrl[num]->RetLen()+5;
 int my = (EY-5)-(scrl[num]->RetPos()*scrl[num]->RetLen())/scrl[num]->RetMaxPos();
 if (my < 10) my = 10;
 if (my > RetCy()-10) my = RetCy() - 10;
 XSetForeground(disp, gc, 0);
 XDrawLine(disp, win, gc, SX-5, my-5, SX-5, my+5);
 XDrawLine(disp, win, gc, SX-4, my-5, SX-4, my+5);
 XDrawLine(disp, win, gc, SX+4, my-5, SX+4, my+5);
 XDrawLine(disp, win, gc, SX+5, my-5, SX+5, my+5);
 XSetForeground(disp, gc, RGB(0x63,0x63,0x63));
 XDrawLine(disp, win, gc, SX+3, my-5, SX+3, my+5);
 XDrawLine(disp, win, gc, SX-3, my-5, SX-3, my+5);
 XSetForeground(disp, gc, RGB(0x73,0x73,0x73));
 XDrawLine(disp, win, gc, SX+2, my-5, SX+2, my+5);
 XDrawLine(disp, win, gc, SX-2, my-5, SX-2, my+5);
 XSetForeground(disp, gc, RGB(0x83,0x83,0x83));
 XDrawLine(disp, win, gc, SX+1, my-5, SX+1, my+5);
 XDrawLine(disp, win, gc, SX-1, my-5, SX-1, my+5);
 XSetForeground(disp, gc, RGB(0x93,0x93,0x93));
 XDrawLine(disp, win, gc, SX, my-5, SX, my+5);
 BlackWings();
}


void Wnd :: DrawFadedScrl(int num)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::DrawFadedScroll(%d)",num);
 out(dstr);
#endif
 if (!scrl) return;
 if (!validScrl(num)) return;
 int SX = scrl[num]->RetSx();
 int SY = scrl[num]->RetSy()-5;
 int EY = SY + scrl[num]->RetLen()+5;
 int my = (EY-5)-(scrl[num]->RetPos()*scrl[num]->RetLen())/scrl[num]->RetMaxPos();
 if (my < 10) my = 10;
 if (my > RetCy()-10) my = RetCy() - 10;
 if (!scrl[num]->isDefault())
   {
    DrawHalfFrame(disp, win, RGB(0x83, 0x83, 0x83), gc, RGB(0xf,0xf,0xf),
    5,SX-5 ,SX+5 ,my-5 , my+5, 1);
    DrawHalfFrame(disp, win, RGB(0x83, 0x83, 0x83), gc, RGB(0xf,0xf,0xf),
    5,SX-5 ,SX+5 ,my-5 , my+5, 0);
   }
 else
   {
    DrawHalfFrame(disp, win, RGB(0x83, 0x83, 0x83), gc, -RGB(0xf,0xf,0xf),
    5,SX-5 ,SX+5 ,my-5 , my+5, 1);
    DrawHalfFrame(disp, win, RGB(0x83, 0x83, 0x83), gc, -RGB(0xf,0xf,0xf),
    5,SX-5 ,SX+5 ,my-5 , my+5, 0);
   }
 if (WantPaletteInfo()) PaletteExposeInfo(*this, NULL);
}


void Wnd ::DrawScrl(int num)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::DrawScrl(%d)",num);
 out(dstr);
#endif
 if (!scrl) return;
 if (!validScrl(num)) return;
 int SX = scrl[num]->RetSx();
 int SY = scrl[num]->RetSy()-5;
 int EY = SY + scrl[num]->RetLen()+5;
 XSetForeground(disp, gc, 0);
 XDrawLine(disp, win, gc, SX-5, SY, SX-5, EY);
 XDrawLine(disp, win, gc, SX-4, SY, SX-4, EY);
 XDrawLine(disp, win, gc, SX+4, SY, SX+4, EY);
 XDrawLine(disp, win, gc, SX+5, SY, SX+5, EY);
 DrawFrame(disp, win, RGB(0x63, 0x63, 0x63), gc, RGB(0x10,0x10,0x10),
    4,SX-3 ,SX+3 ,SY , EY);
 int my = (EY-5)-(scrl[num]->RetPos()*scrl[num]->RetLen())/scrl[num]->RetMaxPos();
 if (my < 10) my = 10;
 if (my > RetCy()-10) my = RetCy() - 10;
 if (!scrl[num]->isDefault())
   {
    DrawHalfFrame(disp, win, RGB(0x83, 0x83, 0x83), gc, RGB(0xf,0xf,0xf),
    5,SX-5 ,SX+5 ,my-5 , my+5, 1);
    DrawHalfFrame(disp, win, RGB(0x83, 0x83, 0x83), gc, RGB(0xf,0xf,0xf),
    5,SX-5 ,SX+5 ,my-5 , my+5, 0);
   }
 else
   {
    DrawHalfFrame(disp, win, RGB(0x83, 0x83, 0x83), gc, -RGB(0xf,0xf,0xf),
    5,SX-5 ,SX+5 ,my-5 , my+5, 1);
    DrawHalfFrame(disp, win, RGB(0x83, 0x83, 0x83), gc, -RGB(0xf,0xf,0xf),
    5,SX-5 ,SX+5 ,my-5 , my+5, 0);
   }
 XSetForeground(disp, gc, RGB(0x0, 0x0, 0xa));
}


void Wnd :: DrawBtn(int num)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::DrawBtn(%d)",num);
 out(dstr);
#endif
 if (!btn) return;
 if (!validBtn(num)) return;
 if (!btn[num]->isSelected())
   {
    DrawHalfFrame(disp, win, RGB(0x83, 0x83, 0x83), gc, RGB(0xf,0xf,0xf),
    4, btn[num]->RetSx(), btn[num]->RetEx(), btn[num]->RetSy(), btn[num]->RetEy(), 1);
    DrawHalfFrame(disp, win, RGB(0x83, 0x83, 0x83), gc, -RGB(0xf,0xf,0xf),
    4, btn[num]->RetSx(), btn[num]->RetEx(), btn[num]->RetSy(), btn[num]->RetEy(), 0);
    DrawFrame(disp, win, RGB(0x8a, 0x8a, 0x8a), gc, 0,
    ((btn[num]->RetX()>btn[num]->RetY()) ? (btn[num]->RetY()/2) : (btn[num]->RetX()/2)),
    btn[num]->RetSx()+4, btn[num]->RetEx()-4,btn[num]->RetSy()+4, btn[num]->RetEy()-4);
    XSetForeground(disp, gc, RGB(0x0, 0x0, 0xa));
   }
 else
   {
    DrawHalfFrame(disp, win, RGB(0x83, 0x83, 0x83), gc, -RGB(0xf,0xf,0xf),
    4, btn[num]->RetSx(), btn[num]->RetEx(), btn[num]->RetSy(), btn[num]->RetEy(), 1);
    DrawHalfFrame(disp, win, RGB(0x83, 0x83, 0x83), gc, RGB(0xf,0xf,0xf),
    4, btn[num]->RetSx(), btn[num]->RetEx(), btn[num]->RetSy(), btn[num]->RetEy(), 0);
    DrawFrame(disp, win, RGB(0x8a, 0x8a, 0x8a), gc, 0,
    ((btn[num]->RetX()>btn[num]->RetY()) ? (btn[num]->RetY()/2) : (btn[num]->RetX()/2)),
    btn[num]->RetSx()+4, btn[num]->RetEx()-4, btn[num]->RetSy()+4, btn[num]->RetEy()-4);
   }
 if (btn[num]->isDefault())
 DrawFrame(disp, win, RGB(0x6f, 0x0, 0x0), gc, 0,
   1, btn[num]->RetSx(), btn[num]->RetEx(), btn[num]->RetSy(), btn[num]->RetEy());
 char* tmp= new char[SHORT_TXT];
 if (!tmp) panic("MEMORY_EXCPT_IN_operator new", __LINE__,__FILE__);
 strcpy(tmp, btn[num]->RetText());
 int hlen = strlen(tmp)/2;
 if (!btn[num]->isSelected()) XSetForeground(disp, gc, RGB(0x0, 0x0, 0xa));
 else XSetForeground(disp, gc, RGB(0x77, 0x0, 0x0));
 XDrawString(disp, win ,gc, btn[num]->RetSx()+btn[num]->RetX()/2-6*hlen,
 btn[num]->RetSy()+btn[num]->RetY()/2+font_h-8, btn[num]->RetText(), strlen(btn[num]->RetText()));
}


void Wnd :: NextDefault()   //select next button (not CLICK but SELECT)
{
 out("wnd::nextdefault");
 int index;
 if (!btn) return;
 for (int i=0;i<RetNumBtns();i++)
  {
   if (btn[i]->isDefault())
     {
      index = i+1;
      if (index == RetNumBtns()) index = 0;
      btn[i]->SetDefault(0);
      btn[index]->SetDefault(1);
      DrawBtn(i);
      DrawBtn(index);
      return ;
     }
  }
}


void Wnd :: SelectNextScroll()   //select next button (not CLICK but SELECT)
{
 out("wnd::selectnextscroll");
 int index;
 if (!scrl) return;
 for (int i=0;i<RetNumScrl();i++)
  {
   if (scrl[i]->isDefault())
     {
      index = i+1;
      if (index == RetNumScrl()) index = 0;
      scrl[i]->SetDefault(0);
      scrl[index]->SetDefault(1);
      DrawScrl(i);
      DrawScrl(index);
      return ;
     }
  }
}


void Wnd :: PrevDefault()   //select previous button (not CLICK but SELECT)
{
 out("wnd::prevdefault");
 int index;
 if (!btn) return;
 for (int i=0;i<RetNumBtns();i++)
  {
   if (btn[i]->isDefault())
     {
      index = i-1;
      if (index == -1) index = RetNumBtns()-1;
      btn[i]->SetDefault(0);
      btn[index]->SetDefault(1);
      DrawBtn(i);
      DrawBtn(index);
      return ;
     }
  }
}


void Wnd :: DrawPaletteColor()
{
 out("wnd::drawpalettecolor");
 if (!scrl) return;
 if (!scrl[0]) return;
 if (!scrl[1]) return;
 if (!scrl[2]) return;
 int r = scrl[0]->RetPos();
 int g = scrl[1]->RetPos();
 int b = scrl[2]->RetPos();
 SetCurColor(RGB(r,g,b));
 XSetForeground(disp, gc, RetCurColor());
 for (int i=80;i<102;i++)
 for (int j=15;j<25;j++)
 XDrawPoint(disp, win, gc, i,j);
}

void* Wnd :: DefaultResponse()             //Default Proc function (must be called manually by user
{                                          //when !ALL! Setups are ready, and !ALL! responses & event declared
  out("wnd::defaultresponse");             //ResponseTable !MUST! be declared to
  if (SetupFunc) SetupFunc(*this,RetEventData(EV_SETUP));         //To have return values SetCharPtr() or SetIntPtr() !MUST! be called
  else out("no SetupFunc defined!");
  if (ExposeFunc) ExposeFunc(*this,RetEventData(EV_EXPOSE));       //See example function usage in Wnd::SelectDefaults() <BadExample>
  else out("no ExposeFunc defined!");
  if (have_ca) ca->Update();
  while (!done)                            //See also for instance: Wnd::CreateCheckBoxChild()
    {
      XNextEvent(disp, &event);
      switch (WhatHappened())
        {
         case Expose:
	     if (!ExposeFunc) out("response unhandled!");   //if no function given print it to debug
	     else ExposeFunc(*this, RetEventData(EV_EXPOSE));                         //ExposeFunc is very important see ExposeFuncX
             break;
        case ConfigureNotify:                                 //reponse functions have different prototypes
             cx = event.xconfigure.width;                     //watch it or define other <NOT_DEFAULT >
             cy = event.xconfigure.height;                    //such function're unhandled in class :(
	     if (!ConfigureFunc) out("configure unhandled!");  //the same
	     else ConfigureFunc(*this, cx, cy, RetEventData(EV_CONFIG));                //there is also ConfigureFuncX
             //repaint
             break;
        case ButtonPress:
	     x = event.xbutton.x;
             y = event.xbutton.y;
             if (!ButtonPressFunc) out("button press unhandled!");
	     else
	     {
              switch (event.xbutton.button)
	          {
                   case Button1:
		         ButtonPressFunc(*this,1,x,y,RetEventData(EV_BPRESS));
		         break;
                   case Button2:
                         ButtonPressFunc(*this,2,x,y,RetEventData(EV_BPRESS));
                         break;
		   case Button3:
			 ButtonPressFunc(*this,3,x,y,RetEventData(EV_BPRESS));
		         break;
                   default:
		         ButtonPressFunc(*this,0,x,y,RetEventData(EV_BPRESS));
                         break;
		  }
	      }
	break;
	case  ButtonRelease:
	     x = event.xbutton.x;
             y = event.xbutton.y;
	     if (!ButtonReleaseFunc) out("button release unhandled!");
	     else
	     {
             switch (event.xbutton.button)
	          {
                   case Button1:
                         ButtonReleaseFunc(*this,1,x,y,RetEventData(EV_BRELEASE));
		         break;
                   case Button2:
                         ButtonReleaseFunc(*this,2,x,y,RetEventData(EV_BRELEASE));
                         break;
		   case Button3:
                         ButtonReleaseFunc(*this,3,x,y,RetEventData(EV_BRELEASE));
		         break;
                   default:
                         ButtonReleaseFunc(*this,0,x,y,RetEventData(EV_BRELEASE));
                         break;
		  }
		break;
	     }
        break;
	case MotionNotify:
             x = event.xbutton.x;
             y = event.xbutton.y;
	     if (!MotionNotifyFunc) out("motion notify unhandled!");
	     else MotionNotifyFunc(*this, drag, x, y,RetEventData(EV_MOTION));  //this uses drag
	     break;                                                             //when 1-st key pressed
                                                                                //eXtendedButtonPress sets drag to 1
        case KeyPress:                                                          //when 2nd or 3rd sets to 2 or 3
             if (!KeyPressFunc) out("keyboard key press unhandled!");           //and when released ExtendedReleaseButton
	     else KeyPressFunc(*this, event.xkey.keycode,RetEventData(EV_KPRESS));                      //sets to 0 (this all works when response table
	     //if (event.xkey.keycode == 0x18) done=1;                          //set to MOST_RESPONSES or user defined value
             break;                                                             //which contains reaction to: buttonpress, buttonrelease
        case KeyRelease:                                                        //and motionnotify and used eXtendedButtonPress-Release
             if (!KeyReleaseFunc) out("keyboard key release unhandled!");       //functions or Functions-userdefined but these functions must
	     else KeyReleaseFunc(*this, event.xkey.keycode,RetEventData(EV_KRELEASE));                    //then set drag manually...)
             break;
        default:
             if (!OtherEventFunc) out("unknown event, no default function found!");
	     else OtherEventFunc(*this,RetEventData(EV_OTHER));
             break;
        }
   }
 out("wnd::response ends...");
 //cont_3
 if (wbox)  WboxCreateReturnValue();
 WindowDies();
 out("Wnd--DEFAULT_RESPONSE ===> after_win_dies, only return value remain");
 if (dta) return dta->DefaultReturn();   //default return cast to (void*) then user must "uncast" to correct pointer
 else return NULL;                        //example to cast void* to int do like this int val = (int)far_ptr, where far_ptr is void*
}


void Wnd :: DeleteWndText()
{
 out("Wnd::DeleteWndText()");
 for (int i=1;i<lines;i++) if (text[i]) delete [] text[i];
 if (text) delete [] text;
 text = NULL;
}


void Wnd :: WboxCreateReturnValue()
{
 out("wnd::wboxcreatereturnvalue");
 if (!dta) {out("calling charptrreturn because dta == NULL, I'm not sure it is good idea!");CharPtrReturn();}
 if ((wbox) && (dta)) dta->SetCharPtr(wbox->RetText());
}


void Wnd :: SetUnusedCharPtr(char* par)
{
#ifdef DBG
 char dstr[LONG_STR];
 if (par) sprintf(dstr,"Wnd::SetUnusedCharPtr(%p:%s)",par,par);
 else sprintf(dstr,"Wnd::SetUnusedCharPtr((char*)NULL)");
 out(dstr);
#endif
 out("wnd::setunusedcharptr (to bypass func(Wnd&) <-> Wnd <-> Unused (class comunication)");
 if (dta) dta->SetCharPtr(par);
}


void Wnd :: SelectProcFunction(void* (*procfunc)(Wnd&,void*))
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SelectProcFunction(far_func=%p)",procfunc);
 out(dstr);
#endif
 if (!procfunc) return;
 this->ProcFunc = procfunc;
 if (SetupFunc) SetupFunc(*this, RetEventData(EV_SETUP));
 (*procfunc)(*this,RetEventData(EV_PROC));                        //WYPAS, jezeli chcesz wlasny WinProc, prosze bardzo :)
 WindowDies();
}


void Wnd :: AddictionalSetup(void (*procfunc)(Wnd&,void*))
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::AddictionalSetup(far_func=%p) will be called before DefaultResponse",procfunc);
 out(dstr);
#endif
 if (!procfunc) return;
 this->SetupFunc = procfunc;
}

/* old WNDDIES()

void Wnd :: WindowDies()
{
 out("wnd::windowdies");
 if (!this) return ;
 if (odsp) odsp = NULL;
 out("disply killed");
 if (font_info) font_info = NULL;
 out("font killed");
 for (int i=1;i<lines;i++) if (text[i]) delete [] text[i];
 if (text) delete [] text;
 text = NULL;
 out("text killed");
 //FIXME possible unfreed memory address (char*)[0] text, but make an error when freeing (????)
 lines = 0;
 if (caption) delete [] caption;
 out("caption killed");
 for (int i=1;i<num_btns;i++) if (btn[i]) delete btn[i];
 //FIXME starting from 1 to delete 0 position with all [][] pointer table... (is it correct? ?? ???)
 if (btn) delete [] btn;
 btn = NULL;
 num_btns = 0;
 out("btn killed");
 if (win) XUnmapWindow(disp, win);
 if (win) XDestroyWindow(disp, win);
 disp = NULL;
 out("window unmapped & killed");
 if (child) child->WindowDies();
 if (child) delete child;
 child = NULL;
 out("children killed");
 if (wbox) delete wbox;
 wbox = NULL;
 out("wBoxes killed");
 if (cbox) delete cbox;
 cbox = NULL;
 out("cBoxes killed");
 out("wnd::killed!?");
}

*/

void Wnd :: ForceUnmap()
{
 out("wnd :: forceunmap");
 XUnmapWindow(disp, win);
 XDestroyWindow(disp, win);
 XUnmapWindow(disp, win);
 XDestroyWindow(disp, win);
}


void Wnd :: WindowDies()
{
 out("wnd::windowdies");
 if (!this) return;
 if (win) XUnmapWindow(disp, win);
 if (win) XDestroyWindow(disp, win);
 out("window unmapped & killed");
 out("NOW_PARENT send signal <<RITUAL_SUICIDE>> to its children <UNIX>!");
 if (child) child->WindowDies();                //Now Let Parent KILL_OWN_CHILDREN DAEMON >>>HIVE_666<<<
}                                               //ALTAR_FUNC (((for_the_glory_of_all_UNIX_systems)))

void Wnd :: MakeClipFromWindowText(int sx, int sy, int ex, int ey)
{
 if (sx<0) sx = 0;
 if (sy<0) sy = 0;
 if (ex>=RetRx()) ex = RetRx();
 if (ey>=RetRy()) ey = RetRy();
 ca = new ClipArea(sx, sy, ex, ey);
 ca->SetClipText(RetWndText(), RetLines());
 if (!ca) panic("Wnd::MakeClipFromWindowTextcannot create cliare");
 have_ca = true;
}


Wnd :: ~Wnd()
{
  //there is some BUG & I will find it FIXME
 out("Wnd::~Wnd ALPHA-VERSION destructor");
 if (deleted) return;
 if (odsp) odsp = NULL;
 out("display killed");
 //WindowDies();				//FIXME ??? Possible duble win deleting in some cases ??
 if (font_info) font_info = NULL;
 out("font killed");
 for (int i=1;i<lines;i++) if (text[i]) delete [] text[i];
 if (text) delete [] text;
 text = NULL;
 out("text killed");
 //FIXME possible unfreed memory address (char*)[0] text, but make an error when freeing (????)
 lines = 0;
 if (caption) delete [] caption;
 out("caption killed");
 for (int i=1;i<num_btns;i++) if (btn[i]) delete btn[i];
 //FIXME starting from 1 to delete 0 position with all [][] pointer table... (is it correct? ?? ???)
 if (btn) delete [] btn;
 btn = NULL;
 num_btns = 0;
 out("btn killed");
 for (int i=1;i<num_scrl;i++) if (scrl[i]) delete scrl[i];
 if (scrl) delete [] scrl;
 scrl = NULL;
 num_scrl = 0;
 if (ca) delete ca;
 ca = NULL;
 have_ca = false;
 out("scrl killed");
 disp = NULL;
 out("window unmapped & killed");
 //if (child) child->WindowDies();
 if (child) delete child;
 child = NULL;
 out("children killed by parent >>>IMMORAL_ALTAR_FUNC<<< called");
 if (wbox) delete wbox;
 wbox = NULL;
 out("wBoxes killed");
 if (cbox) delete cbox;
 cbox = NULL;
 out("cBoxes killed");
 if (graph) DelGraph();
 graph = NULL;
 out("graphics killed!");
 if (event_ptr) Kill_event_ptr();
 out("event_ptr(s) killed");
 out("wnd::killed!?");
 deleted = 1;
 //do not kill the parent not use !!! if (parent) delete parent
 //it wouldn't be unmapped and all other children would die prematural
}


char* Wnd :: RetSelectedButtonName() const
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::RetSelectedButtonName() ==> dependent");
 out(dstr);
#endif
 if (!btn) return NULL;
 for (int i=0;i<RetNumBtns();i++)
   {
    if (validBtn(i))
      {
       if (btn[i]->isSelected()) return btn[i]->RetText();
      }
   }
 return NULL;
}


void Wnd :: SelectExposeFunc(void (*func)(Wnd&, void*))
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SelectExposeFunc(far_func=%p)",func);
 out(dstr);
#endif
 this->ExposeFunc = func;
}


void Wnd :: SelectConfigureFunc(void (*func)(Wnd&, int, int, void*))
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SelectConfigureFunc(far_func=%p)",func);
 out(dstr);
#endif
 this->ConfigureFunc = func;
}


void Wnd :: SelectButtonPressFunc(void (*func)(Wnd&, int, int, int, void*))
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SelectButtonPressFunc(far_func=%p)",func);
 out(dstr);
#endif
 this->ButtonPressFunc = func;
}


void Wnd :: SelectButtonReleaseFunc(void (*func)(Wnd&, int, int, int, void*))
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SelectButtonReleaseFunc(far_func=%p)",func);
 out(dstr);
#endif
 this->ButtonReleaseFunc = func;
}


void Wnd :: SelectButtonPressXFunc(void (*func)(Wnd&, int, int, int, void*))
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SelectButtonPressXFunc(far_func=%p)",func);
 out(dstr);
#endif
 this->ButtonPressXFunc = func;
}


void Wnd :: SelectButtonReleaseXFunc(void (*func)(Wnd&, int, int, int, void*))
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SelectButtonReleaseXFunc(far_func=%p)",func);
 out(dstr);
#endif
 this->ButtonReleaseXFunc = func;
}


void Wnd :: SelectMotionNotifyFunc(void (*func)(Wnd&, int, int, int, void*))
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SelectMotionNotifyFunc(far_func=%p)",func);
 out(dstr);
#endif
 this->MotionNotifyFunc = func;
}


void Wnd :: SelectKeyPressFunc(void (*func)(Wnd&, ulong, void*))
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SelectKeyPressFunc(far_func=%p)",func);
 out(dstr);
#endif
 this->KeyPressFunc = func;
}


void Wnd :: SelectKeyReleaseFunc(void (*func)(Wnd&, ulong, void*))
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SelectKeyReleaseFunc(far_func=%p)",func);
 out(dstr);
#endif
 this->KeyReleaseFunc = func;
}


void Wnd :: SelectKeyPressXFunc(void (*func)(Wnd&, ulong, void*))
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SelectkeyPressXFunc(far_func=%p)",func);
 out(dstr);
#endif
 this->KeyPressXFunc = func;
}


void Wnd :: SelectKeyReleaseXFunc(void (*func)(Wnd&, ulong, void*))
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SelectKeyReleaseXFunc(far_func=%p)",func);
 out(dstr);
#endif
 this->KeyReleaseXFunc = func;
}


void Wnd :: SelectOtherEventFunc(void (*func)(Wnd&, void*))
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SelectOtherEventFunc(far_func=%p)",func);
 out(dstr);
#endif
 this->OtherEventFunc = func;
}


void Wnd :: SelectExposeXFunc(void (*func)(Wnd&, void*))
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SelectExposeXFunc(far_func=%p)",func);
 out(dstr);
#endif
 this->ExposeXFunc = func;
}


void Wnd :: SelectConfigureXFunc(void (*func)(Wnd&, int, int, void*))
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SelectConfigureXFunc(far_func=%p)",func);
 out(dstr);
#endif
 this->ConfigureXFunc = func;
}


void Wnd :: SelectMotionNotifyXFunc(void (*func)(Wnd&, int, int, int, void*))
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"Wnd::SelectMotionNotifyXFunc(far_func=%p)",func);
 out(dstr);
#endif
 this->MotionNotifyXFunc = func;
}

void Wnd :: DisableExposeFunc()         { out("wnd::disableexposefunc");this->ExposeFunc = NULL;        }
void Wnd :: DisableConfigureFunc()      { out("wnd::disableconfigurefunc");this->ConfigureFunc = NULL;     }
void Wnd :: DisableButtonPressFunc()    { out("wnd::disablebuttonpressfunc");this->ButtonPressFunc = NULL;   }
void Wnd :: DisableButtonReleaseFunc()  { out("wnd::disablebuttonreleasefunc");this->ButtonReleaseFunc = NULL; }
void Wnd :: DisableButtonPressXFunc()   { out("wnd::disablebuttonpressxfunc");this->ButtonPressXFunc = NULL;  }
void Wnd :: DisableButtonReleaseXFunc() { out("wnd::disablebuttonreleasexfunc");this->ButtonReleaseXFunc = NULL;}
void Wnd :: DisableMotionNotifyFunc()   { out("wnd::disablemotionnotifyfunc");this->MotionNotifyFunc = NULL;  }
void Wnd :: DisableKeyPressFunc()       { out("wnd::disablekeypressfunc");this->KeyPressFunc = NULL;      }
void Wnd :: DisableKeyReleaseFunc()     { out("wnd::disablekeyreleasefunc");this->KeyReleaseFunc = NULL;    }
void Wnd :: DisableKeyPressXFunc()      { out("wnd::disablekeypressxfunc");this->KeyPressXFunc = NULL;     }
void Wnd :: DisableKeyReleaseXFunc()    { out("wnd::disablekeyreleasexfunc");this->KeyReleaseXFunc = NULL;   }
void Wnd :: DisableOtherEventFunc()     { out("wnd::disableothereventfunc");this->OtherEventFunc = NULL;    }
void Wnd :: DisableExposeXFunc()        { out("wnd::disableexposexfunc");this->ExposeXFunc = NULL;        }
void Wnd :: DisableConfigureXFunc()     { out("wnd::disableconfigurexfunc");this->ConfigureXFunc = NULL;     }
void Wnd :: DisableMotionNotifyXFunc()  { out("wnd::disablemotionnotifyxfunc");this->MotionNotifyXFunc = NULL;  }

void StdExposeFunc(Wnd& wnd, void* ptr)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"StdExposeFunc(%p,%p) window,far_data",&wnd,ptr);
 out(dstr);
#endif
 if (!wnd) return;
 wnd.DrawWindow();
 wnd.DrawClipArea();
 wnd.StdTextWrite();
 wnd.DrawCheckBox();
 wnd.DrawButtons();
 wnd.DrawWBox();
 wnd.DrawScrolls();
 out("now will call ExposeXFunc");
 if (wnd.ExposeXFunc) wnd.ExposeXFunc(wnd, wnd.RetEventData(EV_EXPOSEX));
 else out("no wnd::ExposeXFunc defined!\n");
}


void PalExposeFunc(Wnd& wnd, void* ptr)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"PalExposeFunc(%p,%p) window,far_data",&wnd,ptr);
 out(dstr);
#endif
 if (!wnd) return;
 wnd.DrawWindow();
 wnd.StdTextWrite();
 wnd.DrawCheckBox();
 wnd.DrawButtons();
 wnd.DrawWBox();
 wnd.DrawScrolls();
 if (wnd.WantPaletteInfo()) wnd.DrawPaletteColor();
 out("now will call ExposeXFunc");
 if (wnd.ExposeXFunc) wnd.ExposeXFunc(wnd, wnd.RetEventData(EV_EXPOSEX));
 else out("no wnd::ExposeXFunc defined!\n");
}


void NoWDrawExposeFunc(Wnd& wnd, void* ptr)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"NoWDrawExposeFunc(%p,%p) window,far_data",&wnd,ptr);
 out(dstr);
#endif
 if (!wnd) return;
 wnd.StdTextWrite();
 wnd.DrawCheckBox();
 wnd.DrawButtons();
 wnd.DrawWBox();
 wnd.DrawScrolls();
 out("now will call ExposeXFunc");
 if (wnd.ExposeXFunc) wnd.ExposeXFunc(wnd, wnd.RetEventData(EV_EXPOSEX));
 else out("no wnd::ExposeXFunc defined!\n");
}


void StdConfigureFunc(Wnd& wnd, int x, int y, void* ptr)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"StdConfigureFunc(%p,%d,%d,%p) window,new_x,new_y,far_data",&wnd,x,y,ptr);
 out(dstr);
#endif
 if (!wnd) return;
 wnd.SetSize(x,y);
 StdExposeFunc(wnd,wnd.RetEventData(EV_EXPOSE));
 out("trying ConfigureXFunc");
 if (wnd.ConfigureXFunc) wnd.ConfigureXFunc(wnd, x, y, wnd.RetEventData(EV_CONFIGX));
 else out("no wnd::configurexfunc defined!");
}


void NoWDrawConfigureFunc(Wnd& wnd, int x, int y, void* ptr)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"NoWDrawConfigureFunc(%p,%d,%d,%p) window,new_x,new_y,far_data",&wnd,x,y,ptr);
 out(dstr);
#endif
 if (!wnd) return;
 wnd.SetSize(x,y);
 NoWDrawExposeFunc(wnd,wnd.RetEventData(EV_EXPOSE));
 out("trying ConfigureXFunc");
 if (wnd.ConfigureXFunc) wnd.ConfigureXFunc(wnd, x, y, wnd.RetEventData(EV_CONFIGX));
 else out("no wnd::configurexfunc defined!");
}


void StdMotionNotifyFunc(Wnd& wnd, int bt, int x, int y, void* ptr)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"StdMotionNotifyFunc(%p,%d,%d,%d,%p) window,button,x,y,far_data",&wnd,bt,x,y,ptr);
 out(dstr);
#endif
 if (wnd.HaveScroll()) if (bt) wnd.ScrlDispatchBPressed(x, y);
 out("trying MotionNotifyXFunc");
 if (wnd.MotionNotifyXFunc) wnd.MotionNotifyXFunc(wnd, bt, x, y, wnd.RetEventData(EV_MOTIONX));
 else out("no wnd::motionnotifyxfunc defined!");
}


void PalMotionNotifyFunc(Wnd& wnd, int bt, int x, int y, void* ptr)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"PalMotionNotifyFunc(%p,%d,%d,%d,%p) window,button,x,y,far_data",&wnd,bt,x,y,ptr);
 out(dstr);
#endif
 if ((wnd.HaveScroll()) && (bt))
   {
    wnd.ScrlDispatchBPressed(x, y);
    if (wnd.WantPaletteInfo()) wnd.DrawPaletteColor();
   }
 out("trying MotionNotifyXFunc");
 if (wnd.MotionNotifyXFunc) wnd.MotionNotifyXFunc(wnd, bt, x, y, wnd.RetEventData(EV_MOTIONX));
 else out("no wnd::motionnotifyxfunc defined!");
}


void RetOwnNameFunc(Wnd& wnd, void* ptr)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"RetOwnNameFunc(%p,%p) window,far_data",&wnd,ptr);
 out(dstr);
#endif
 if (!wnd) return;
 wnd.SetUnusedCharPtr(wnd.RetSelectedButtonName());                //this is WYPAS too class <<===>> class
 wnd.DoQuit();
}


void DenyResizeFunc(Wnd& wnd, int x, int y, void* ptr)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"DenyResizeFunc(%p,%d,%d,%p) window,new_x,new_y,far_data",&wnd,x,y,ptr);
 out(dstr);
#endif
 if (!wnd) return;
 XResizeWindow(wnd.RetDisp(), wnd.RetWin(), wnd.RetRx(), wnd.RetRy());
 StdExposeFunc(wnd, wnd.RetEventData(EV_EXPOSE));
 out("trying ConfigureXFunc");
 if (wnd.ConfigureXFunc) wnd.ConfigureXFunc(wnd, x, y, wnd.RetEventData(EV_CONFIG));
 else out("no wnd::configurexfunc defined!");
}


void PalDenyResizeFunc(Wnd& wnd, int x, int y, void* ptr)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"PaldenyResizeFunc(%p,%d,%d,%p) window,new_x,new_y,far_data",&wnd,x,y,ptr);
 out(dstr);
#endif
 if (!wnd) return;
 XResizeWindow(wnd.RetDisp(), wnd.RetWin(), wnd.RetRx(), wnd.RetRy());
 PalExposeFunc(wnd, wnd.RetEventData(EV_EXPOSE));
 out("trying ConfigureXFunc");
 if (wnd.ConfigureXFunc) wnd.ConfigureXFunc(wnd, x, y, wnd.RetEventData(EV_CONFIG));
 else out("no wnd::configurexfunc defined!");
}


void NoEDenyResizeFunc(Wnd& wnd, int x, int y, void* ptr)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"NoEDenyResizeFunc(%p,%d,%d,%p) window,new_x,new_y,far_data",&wnd,x,y,ptr);
 out(dstr);
#endif
 if (!wnd) return;
 XResizeWindow(wnd.RetDisp(), wnd.RetWin(), wnd.RetRx(), wnd.RetRy());
 //StdExposeFunc(wnd, wnd.RetEventData(EV_EXPOSE));  NoE NoExpose!
 out("trying ConfigureXFunc");
 if (wnd.ConfigureXFunc) wnd.ConfigureXFunc(wnd, x, y, wnd.RetEventData(EV_CONFIG));
 else out("no wnd::configurexfunc defined!");
}


void NoResizeDrawWFunc(Wnd& wnd, int x, int y, void* ptr)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"NoResizeDrawWFunc(%p,%d,%d,%p) window,x,y,far_data",&wnd,x,y,ptr);
 out(dstr);
#endif
 if (!wnd) return;
 XResizeWindow(wnd.RetDisp(), wnd.RetWin(), wnd.RetRx(), wnd.RetRy());
 NoWDrawExposeFunc(wnd, wnd.RetEventData(EV_EXPOSE));
 out("trying ConfigureXFunc");
 if (wnd.ConfigureXFunc) wnd.ConfigureXFunc(wnd, x, y, wnd.RetEventData(EV_CONFIG));
 else out("no wnd::configurexfunc defined!");
}


void DrawOnWindowFunc(Wnd& wnd, int drag, int x, int y, void* far_ptr)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"DrawOnWindowFunc(%p,%d,%d,%d,%p) window,drag,x,y,far_data",&wnd,drag,x,y,far_ptr);
 out(dstr);
#endif
 if (!wnd.HaveGraph())
   {
    out("warning in drawonwindow, cannot draw on wnd that haven't got graphics enabled!");
    return;
   }
 if (!drag) return;
 ulong col = wnd.RetCurColor();
 int pixsize = drag;
 wnd.DrawWndPixel(col, x,y);
 for (int i=x;i<x+pixsize;i++)
 for (int j=y;j<y+pixsize;j++) wnd.DrawWndPixel(col, i,j);
}


void DrawLinesOnWindowFunc(Wnd& wnd, int drag, int x, int y, void* far_ptr)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"DrawOnWindowFunc(%p,%d,%d,%d,%p) window,drag,x,y,far_data",&wnd,drag,x,y,far_ptr);
 out(dstr);
#endif
 if (!wnd.HaveGraph())
   {
    out("warning in drawonwindow, cannot draw on wnd that haven't got graphics enabled!");
    return;
   }
 if (!drag) return;
 ulong col = wnd.RetCurColor();
 int pixsize = drag*drag;
 wnd.DrawLineTo(x,y, col);
 for (int i=0;i<pixsize;i++)
 for (int j=0;j<pixsize;j++) wnd.DrawMovedLine(x,y,col,i,j);   //i,j to przesuniecie wg <x,y>
 wnd.SetDrawPosX(x);
 wnd.SetDrawPosY(y);
}

                              //name is not good because this is a standard react on button
                              //and called eXtened, this will try to call eXtended button press
                              //function :( but when I was writting it
                              //I hadn't yet known about this

void eXtendedButtonPress(Wnd& wnd, int key_n, int x, int y, void* ptr)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"eXtendedButtonPress(%p,%d,%d,%d,%p) window,btn,x,y,far_data",&wnd,key_n,x,y,ptr);
 out(dstr);
#endif
 if (!wnd) return;
 if (!key_n) wnd.SetDrag(0);
 wnd.ImprDrag(key_n);
 wnd.SetDrawPosX(x);
 wnd.SetDrawPosY(y);
 Button* tmp;
 for (int i=0;i<wnd.RetNumBtns();i++)
   {
    if (wnd.validBtn(i))
      {
       tmp = wnd.RetBtn(i);
       if (!tmp) panic("memory exception in Button* tmp");
       if ((tmp->btnFunc) && (tmp->insideArea(x,y))) {tmp->SetBtnFocus(1); wnd.DrawBtn(i);}
      }
   }
 if (wnd.HaveScroll()) wnd.ScrlDispatchBPressed(x,y);
 if (wnd.ButtonPressXFunc) wnd.ButtonPressXFunc(wnd, key_n, x, y, wnd.RetEventData(EV_BPRESSX));
 else out("no wnd::ButtonPressXFunc defined!");
}

                              //name is not good because this is a standard react on button
                              //and called eXtened, this will try to call eXtended button press
                              //function :(

void eXtendedButtonReleaseND(Wnd& wnd, int key_n, int x, int y, void* ptr)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"eXtendedButtonRelease(%p,%d,%d,%d,%p) window,btn,x,y,far_data",&wnd,key_n,x,y,ptr);
 out(dstr);
#endif
 if (!wnd) return;
 wnd.SetDrag(0);
 wnd.StopDrawingLines();
 Button* tmp;
 for (int i=0;i<wnd.RetNumBtns();i++)
   {
    if (wnd.validBtn(i))
      {
       tmp = wnd.RetBtn(i);
       if (!tmp) panic("memory exception in Button* tmp");
       if ((tmp->isSelected()) && (tmp->insideArea(x,y)))
         {
	  wnd.SetPressedND(i);
	  tmp->btnFunc(wnd, tmp->RetVoid());
	  wnd.CancelPressedND();
	 }
       if (tmp->isSelected()) {tmp->SetBtnFocus(0); wnd.DrawBtn(i);}
      }
   }
 if (wnd.HaveScroll()) wnd.ScrlDispatchBReleased();
 if (wnd.ButtonReleaseXFunc) wnd.ButtonReleaseXFunc(wnd, key_n, x, y, wnd.RetEventData(EV_BRELEASEX));
 else out("no wnd::ButtonReleaseXFunc defined!");
}


void eXtendedButtonRelease(Wnd& wnd, int key_n, int x, int y, void* ptr)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"eXtendedButtonRelease(%p,%d,%d,%d,%p) window,btn,x,y,far_data",&wnd,key_n,x,y,ptr);
 out(dstr);
#endif
 if (!wnd) return;
 wnd.SetDrag(0);
 wnd.StopDrawingLines();
 Button* tmp;
 for (int i=0;i<wnd.RetNumBtns();i++)
   {
    if (wnd.validBtn(i))
      {
       tmp = wnd.RetBtn(i);
       if (!tmp) panic("memory exception in Button* tmp");
       if ((tmp->isSelected()) && (tmp->insideArea(x,y)))
         {
	  wnd.SetPressed(i);
	  tmp->btnFunc(wnd, tmp->RetVoid());
	  wnd.CancelPressed();
	 }
       if (tmp->isSelected()) {tmp->SetBtnFocus(0); wnd.DrawBtn(i);}
      }
   }
 if (wnd.HaveScroll()) wnd.ScrlDispatchBReleased();
 if (wnd.ButtonReleaseXFunc) wnd.ButtonReleaseXFunc(wnd, key_n, x, y, wnd.RetEventData(EV_BRELEASEX));
 else out("no wnd::ButtonReleaseXFunc defined!");
}

                              //name is not good because this is a standard react on button
                              //and called eXtened, this will try to call eXtended button press
                              //function :(

void PaletteExposeInfo(Wnd& wnd, void* ptr)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"PaletteExposeInfo(%p,%p) window,far_data",&wnd,ptr);
 out(dstr);
#endif
 char tmp[20];
 sprintf(tmp,"%02x%02x%02x", ReturnRed(wnd.RetCurColor()),ReturnGreen(wnd.RetCurColor()),ReturnBlue(wnd.RetCurColor()));
 wnd.PrepareWndText(tmp);
 wnd.StdTextWrite();
}


void KeyPressClipControl(Wnd& wnd, ulong code, void* ptr)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"eXtendedKeyPress(%p,%d,%p) window,key_code,far_data",&wnd,(int)code,ptr);
 out(dstr);
#endif
 if (!wnd) return;
 if (code == 36)
   {
    Button* tmp;
    for (int i=0;i<wnd.RetNumBtns();i++)
      {
       if (wnd.validBtn(i))
         {
          tmp = wnd.RetBtn(i);
	  if (!tmp) panic("memory exception in Button* tmp");
          if ((tmp->btnFunc) && (tmp->isDefault())) {tmp->SetBtnFocus(1); wnd.DrawBtn(i);}
         }
      }
    }
 if (wnd.HaveClipArea())
   {
    ClipArea* tmp = wnd.RetClipArea();
    bool inv = false;
    if (code == KEY_UP)	   { wnd.FadeClip(); tmp->MoveUp();    inv = true; }
    if (code == KEY_DOWN)  { wnd.FadeClip(); tmp->MoveDown();  inv = true; }
    if (code == KEY_LEFT)  { wnd.FadeClip(); tmp->MoveLeft();  inv = true; }
    if (code == KEY_RIGHT) { wnd.FadeClip(); tmp->MoveRight(); inv = true; }
    if (code == KEY_PGUP)  { wnd.FadeClip(); tmp->MoveUp(22);    inv = true; }
    if (code == KEY_PGDOWN){ wnd.FadeClip(); tmp->MoveDown(22);  inv = true; }
    if (code == KEY_HOME)  { wnd.FadeClip(); tmp->MoveLeft(22);  inv = true; }
    if (code == KEY_END)   { wnd.FadeClip(); tmp->MoveRight(22); inv = true; }
    if (code == KEY_TAB)   wnd.NextDefault();
    if (inv) wnd.Invalidate();
   }
 if (wnd.KeyPressXFunc) wnd.KeyPressXFunc(wnd, code, wnd.RetEventData(EV_KPRESSX));
 else out("no wnd::KeyPressXFunc defined!");
}


void eXtendedKeyPress(Wnd& wnd, ulong code, void* ptr)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"eXtendedKeyPress(%p,%d,%p) window,key_code,far_data",&wnd,(int)code,ptr);
 out(dstr);
#endif
 if (!wnd) return;
 if (code == 36)
   {
    Button* tmp;
    for (int i=0;i<wnd.RetNumBtns();i++)
      {
       if (wnd.validBtn(i))
         {
          tmp = wnd.RetBtn(i);
	  if (!tmp) panic("memory exception in Button* tmp");
          if ((tmp->btnFunc) && (tmp->isDefault())) {tmp->SetBtnFocus(1); wnd.DrawBtn(i);}
         }
      }
    }
 if ((code == 23) || (code == 100) || (code == 104)) wnd.NextDefault();
 if ((code == 98) || (code == 102)) wnd.PrevDefault();
 if (wnd.KeyPressXFunc) wnd.KeyPressXFunc(wnd, code, wnd.RetEventData(EV_KPRESSX));
 else out("no wnd::KeyPressXFunc defined!");
}

                              //name is not good because this is a standard react on button
                              //and called eXtened, this will try to call eXtended button press
                              //function :(

void eXtendedKeyPressQ(Wnd& wnd, ulong code, void* ptr)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"eXtendedKeyPressQ(%p,%d,%p) window,key_code,far_data",&wnd,(int)code,ptr);
 out(dstr);
#endif
 if (!wnd) return;
 if (code == 36)
   {
    Button* tmp;
    for (int i=0;i<wnd.RetNumBtns();i++)
      {
       if (wnd.validBtn(i))
         {
          tmp = wnd.RetBtn(i);
	  if (!tmp) panic("memory exception in Button* tmp");
          if ((tmp->btnFunc) && (tmp->isDefault())) {tmp->SetBtnFocus(1); wnd.DrawBtn(i);}
         }
      }
    }
 if (code == 24)
   {
    int val = wnd.CreateYesNoChild("Na pewno zamknac okno?", 1);
    if (val) wnd.DoQuit();
   }
 if ((code == 23) || (code == 100) || (code == 104)) wnd.NextDefault();
 if ((code == 98) || (code == 102)) wnd.PrevDefault();
 if (wnd.KeyPressXFunc) wnd.KeyPressXFunc(wnd, code, wnd.RetEventData(EV_KPRESSX));
 else out("no wnd::KeyPressXFunc defined!");
}

                              //name is not good because this is a standard react on button
                              //and called eXtened, this will try to call eXtended button press
                              //function :(

void eXtendedKeyReleaseND(Wnd& wnd, ulong code, void* ptr)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"eXtendedKeyrelease(%p,%d,%p) window,key_code,far_data",&wnd,(int)code,ptr);
 out(dstr);
#endif
 if (!wnd) return;
 Button* tmp;
 for (int i=0;i<wnd.RetNumBtns();i++)
   {
    if (wnd.validBtn(i))
      {
       tmp = wnd.RetBtn(i);
       if (!tmp) panic("memory exception in Button* tmp");
       if (tmp->isSelected())
         {
	  wnd.SetPressedND(i);
	  tmp->btnFunc(wnd, tmp->RetVoid());
	  wnd.CancelPressedND();
	  tmp->SetBtnFocus(0);
	  wnd.DrawBtn(i);
	 }
       tmp->SetBtnFocus(0);
      }
   }
 if (wnd.HaveScroll())
   {
    if (code == KEY_UP) wnd.MoveDefScroll(1);
    if (code == KEY_DOWN) wnd.MoveDefScroll(-1);
    if (code == KEY_TAB) wnd.SelectNextScroll();
   }
 if (wnd.KeyReleaseXFunc) wnd.KeyReleaseXFunc(wnd, code, wnd.RetEventData(EV_KRELEASEX));
 else out("no wnd::KeyReleaseXFunc defined!");
}


void eXtendedKeyRelease(Wnd& wnd, ulong code, void* ptr)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"eXtendedKeyrelease(%p,%d,%p) window,key_code,far_data",&wnd,(int)code,ptr);
 out(dstr);
#endif
 if (!wnd) return;
 Button* tmp;
 for (int i=0;i<wnd.RetNumBtns();i++)
   {
    if (wnd.validBtn(i))
      {
       tmp = wnd.RetBtn(i);
       if (!tmp) panic("memory exception in Button* tmp");
       if (tmp->isSelected())
         {
	  wnd.SetPressed(i);
	  tmp->btnFunc(wnd, tmp->RetVoid());
	  wnd.CancelPressed();
	  tmp->SetBtnFocus(0);
	  wnd.DrawBtn(i);
	 }
       tmp->SetBtnFocus(0);
      }
   }
 if (wnd.HaveScroll())
   {
    if (code == KEY_UP) wnd.MoveDefScroll(1);
    if (code == KEY_DOWN) wnd.MoveDefScroll(-1);
    if (code == KEY_TAB) wnd.SelectNextScroll();
   }
 if (wnd.KeyReleaseXFunc) wnd.KeyReleaseXFunc(wnd, code, wnd.RetEventData(EV_KRELEASEX));
 else out("no wnd::KeyReleaseXFunc defined!");
}


void PalButtonPress(Wnd& wnd, int key_n, int x, int y, void* ptr)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"PalButtonPress(%p,%d,%d,%d,%p) window,btn,x,y,far_data",&wnd,key_n,x,y,ptr);
 out(dstr);
#endif
 if (!wnd) return;
 if (!key_n) wnd.SetDrag(0);
 wnd.ImprDrag(key_n);
 Button* tmp;
 for (int i=0;i<wnd.RetNumBtns();i++)
   {
    if (wnd.validBtn(i))
      {
       tmp = wnd.RetBtn(i);
       if (!tmp) panic("memory exception in Button* tmp");
       if ((tmp->btnFunc) && (tmp->insideArea(x,y))) {tmp->SetBtnFocus(1); wnd.DrawBtn(i);}
      }
   }
 if (wnd.HaveScroll())
   {
    wnd.ScrlDispatchBPressed(x,y);
    if (wnd.WantPaletteInfo()) wnd.DrawPaletteColor();
   }
 if (wnd.ButtonPressXFunc) wnd.ButtonPressXFunc(wnd, key_n, x, y, wnd.RetEventData(EV_BPRESSX));
 else out("no wnd::ButtonPressXFunc defined!");
}

                              //name is not good because this is a standard react on button
                              //and called eXtened, this will try to call eXtended button press
                              //function :(

void PalButtonRelease(Wnd& wnd, int key_n, int x, int y, void* ptr)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"PalButtonRelease(%p,%d,%d,%d,%p) window,btn,x,y,far_data",&wnd,key_n,x,y,ptr);
 out(dstr);
#endif
 if (!wnd) return;
 wnd.SetDrag(0);
 Button* tmp;
 for (int i=0;i<wnd.RetNumBtns();i++)
   {
    if (wnd.validBtn(i))
      {
       tmp = wnd.RetBtn(i);
       if (!tmp) panic("memory exception in Button* tmp");
       if ((tmp->isSelected()) && (tmp->insideArea(x,y)))
         {
	  wnd.SetPressed(i);
	  tmp->btnFunc(wnd, tmp->RetVoid());
	  wnd.CancelPressed();
	 }
       if (tmp->isSelected()) {tmp->SetBtnFocus(0); wnd.DrawBtn(i);}
      }
   }
 if (wnd.HaveScroll())
   {
    wnd.ScrlDispatchBReleased();
    if (wnd.WantPaletteInfo()) wnd.DrawPaletteColor();
   }
 if (wnd.ButtonReleaseXFunc) wnd.ButtonReleaseXFunc(wnd, key_n, x, y, wnd.RetEventData(EV_BRELEASEX));
 else out("no wnd::ButtonReleaseXFunc defined!");
}


void PalKeyRelease(Wnd& wnd, ulong code, void* ptr)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"PalKeyRelease(%p,%d,%p) window,key_code,far_data",&wnd,(int)code,ptr);
 out(dstr);
#endif
 if (!wnd) return;
 Button* tmp;
 for (int i=0;i<wnd.RetNumBtns();i++)
   {
    if (wnd.validBtn(i))
      {
       tmp = wnd.RetBtn(i);
       if (!tmp) panic("memory exception in Button* tmp");
       if (tmp->isSelected())
         {
	  wnd.SetPressed(i);
	  tmp->btnFunc(wnd, tmp->RetVoid());
	  wnd.CancelPressed();
	  tmp->SetBtnFocus(0);
	  wnd.DrawBtn(i);
	 }
       tmp->SetBtnFocus(0);
      }
   }
 if (wnd.HaveScroll())
   {
    if (code == KEY_UP) wnd.MoveDefScroll(1);
    if (code == KEY_DOWN) wnd.MoveDefScroll(-1);
    if (code == KEY_TAB) wnd.SelectNextScroll();
    if (wnd.WantPaletteInfo()) wnd.DrawPaletteColor();
   }
 if (wnd.KeyReleaseXFunc) wnd.KeyReleaseXFunc(wnd, code, wnd.RetEventData(EV_KRELEASEX));
 else out("no wnd::KeyReleaseXFunc defined!");
}


void StdOKFunc(Wnd& wnd, void* ptr)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"StdOKFunc(%p,%p) window,far_data",&wnd,ptr);
 out(dstr);
#endif
 if (!wnd) return;
 wnd.DoQuit();
}

void StdDownFunc(Wnd& wnd, void* ptr)     //this is strange Up func call downscroll but "up" on screen is to go down
{                              //Y coording goes from up-screen (0) to down-screen (MaxY)
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"StdDownFunc(%p,%p) window,far_data",&wnd,ptr);
 out(dstr);
#endif
 if (!wnd) return;
 if (wnd.HaveCheckBox()) wnd.UpCheckBox();
 wnd.DrawCheckBox();
}


void StdUpFunc(Wnd& wnd, void* ptr)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"StdUpFunc(%p,%p) window,far_data",&wnd,ptr);
 out(dstr);
#endif
 if (!wnd) return;
 if (wnd.HaveCheckBox()) wnd.DownCheckBox();
 wnd.DrawCheckBox();
}

void StdDownTenFunc(Wnd& wnd, void* ptr)     //this is strange Up func call downscroll but "up" on screen is to go down
{                              //Y coording goes from up-screen (0) to down-screen (MaxY)
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"StdDownTenFunc(%p,%p) window,far_data",&wnd,ptr);
 out(dstr);
#endif
 if (!wnd) return;
 if (wnd.HaveCheckBox()) wnd.UpTenCheckBox();
 wnd.DrawCheckBox();
}


void StdUpTenFunc(Wnd& wnd, void* ptr)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"StdUpTenFunc(%p,%p) window,far_data",&wnd,ptr);
 out(dstr);
#endif
 if (!wnd) return;
 if (wnd.HaveCheckBox()) wnd.DownTenCheckBox();
 wnd.DrawCheckBox();
}


void PrepareVoidReturn(Wnd& wnd, void* ptr)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"PrepareVoidReturn(%p,%p) window,far_data",&wnd,ptr);
 out(dstr);
#endif
 if (!wnd) return;
 if (!wnd.HaveScroll()) return;
 ulong* col = new ulong;
 if (!col) panic("MEMORY_EXCPT_IN_operator new", __LINE__,__FILE__);
 Scroll* tmp = wnd.RetScroll(0);
 if (!tmp) panic("scroll exception in external func(wnd&, void*) is NULL!");
 int r = tmp->RetPos();
 tmp = wnd.RetScroll(1);
 if (!tmp) panic("scroll exception in external func(wnd&, void*) is NULL!");
 int g = tmp->RetPos();
 tmp = wnd.RetScroll(2);
 if (!tmp) panic("scroll exception in external func(wnd&, void*) is NULL!");
 int b = tmp->RetPos();
 Unused* t = wnd.RetDta();
 if (!t) return;
 *col = RGB(r, g, b);
 t->SetVoid(col);                  //fuck this is smart! :)
 wnd.DoQuit();
}


void StdYesFunc(Wnd& wnd, void* ptr)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"StdYesFunc(%p,%p) window,far_data",&wnd,ptr);
 out(dstr);
#endif
 if (!wnd) return;
 Unused* tmp = wnd.RetDta();
 if (!tmp) return;
 tmp->SetInt(YES_ID);                  //fuck this is smart! :)
 wnd.DoQuit();
}


void StdNoFunc(Wnd& wnd, void* ptr)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"StdNoFunc(%p,%p) window,far_data",&wnd,ptr);
 out(dstr);
#endif
 if (!wnd) return;
 Unused* tmp = wnd.RetDta();
 if (!tmp) return;
 tmp->SetInt(NO_ID);
 wnd.DoQuit();
}


void wBoxKeyPressFunc(Wnd& wnd, ulong code, void* ptr)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"wBoxKeyPressFunc(%p,%d,%p) window,key_code,far_data",&wnd,(int)code,ptr);
 out(dstr);
#endif
 if (!wnd) return;
 if ((code == SHIFT_L) || (code == SHIFT_R)) wnd.WboxActivateShift(true);
 int key;
 if (wnd.WboxShiftPressed()) key = TransformKey(code, 1);
 else key = TransformKey(code, 0);
 wnd.WboxKeyAction(key);
 wnd.DrawWBox();
 if ((key == ENTER) || (key == ESCAPE)) wnd.DoQuit();
}


void wBoxKeyReleaseFunc(Wnd& wnd, ulong code, void* ptr)
{
#ifdef DBG
 char dstr[LONG_STR];
 sprintf(dstr,"wBoxKeyReleaseFunc(%p,%d,%p) window,key_code,far_data",&wnd,(int)code,ptr);
 out(dstr);
#endif
 if (!wnd) return;
 if ((code == SHIFT_L) || (code == SHIFT_R)) wnd.WboxActivateShift(false);
}

