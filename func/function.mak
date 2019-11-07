#
# Borland C++ IDE generated makefile
# Generated 02-07-05 at 13:55:24 
#
.AUTODEPEND


#
# Borland C++ tools
#
IMPLIB  = Implib
BCC32   = Bcc32 +BccW32.cfg 
BCC32I  = Bcc32i +BccW32.cfg 
TLINK32 = TLink32
ILINK32 = Ilink32
TLIB    = TLib
BRC32   = Brc32
TASM32  = Tasm32
#
# IDE macros
#


#
# Options
#
IDE_LinkFLAGS32 =  -LC:\BC5\LIB
IDE_ResFLAGS32 = 
LinkerLocalOptsAtW32_functiondexe =  -Tpe -aa -V4.0 -c
ResLocalOptsAtW32_functiondexe = 
BLocalOptsAtW32_functiondexe = 
CompInheritOptsAt_functiondexe = -IC:\BC5\INCLUDE -D_RTLDLL;
LinkerInheritOptsAt_functiondexe = -x
LinkerOptsAt_functiondexe = $(LinkerLocalOptsAtW32_functiondexe)
ResOptsAt_functiondexe = $(ResLocalOptsAtW32_functiondexe)
BOptsAt_functiondexe = $(BLocalOptsAtW32_functiondexe)

#
# Dependency List
#
Dep_function = \
   function.exe

function : BccW32.cfg $(Dep_function)
  echo MakeNode

Dep_functiondexe = \
   function.obj\
   function.h\
   function.res

function.exe : $(Dep_functiondexe)
  $(ILINK32) @&&|
 /v $(IDE_LinkFLAGS32) $(LinkerOptsAt_functiondexe) $(LinkerInheritOptsAt_functiondexe) +
C:\BC5\LIB\c0w32.obj+
function.obj
$<,$*
C:\BC5\LIB\import32.lib+
C:\BC5\LIB\cw32i.lib

function.res

|
function.obj :  function.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_functiondexe) $(CompInheritOptsAt_functiondexe) -o$@ function.cpp
|

function.res :  function.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_functiondexe) $(CompInheritOptsAt_functiondexe)  -FO$@ function.rc
|
# Compiler configuration file
BccW32.cfg : 
   Copy &&|
-w
-R
-v
-WM-
-vi
-H
-H=function.csm
-W
| $@


