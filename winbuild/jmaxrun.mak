# Microsoft Developer Studio Generated NMAKE File, Based on jmaxrun.dsp
!IF "$(CFG)" == ""
CFG=jmaxrun - Win32 Debug
!MESSAGE No configuration specified. Defaulting to jmaxrun - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "jmaxrun - Win32 Release" && "$(CFG)" != "jmaxrun - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "jmaxrun.mak" CFG="jmaxrun - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "jmaxrun - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "jmaxrun - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "jmaxrun - Win32 Release"

OUTDIR=.\jMaxRunRelease
INTDIR=.\jMaxRunRelease

ALL : "..\bin\jmax.exe"


CLEAN :
	-@erase "$(INTDIR)\jmax.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\bin\jmax.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "$(JAVA_HOME)\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\jmaxrun.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\jmax.pdb" /machine:I386 /out:"..\bin\jmax.exe" 
LINK32_OBJS= \
	"$(INTDIR)\jmax.obj"

"..\bin\jmax.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "jmaxrun - Win32 Debug"

OUTDIR=.\jMaxRunDebug
INTDIR=.\jMaxRunDebug

ALL : "..\bin\jmax.exe"


CLEAN :
	-@erase "$(INTDIR)\jmax.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\jmax.pdb"
	-@erase "..\bin\jmax.exe"
	-@erase "..\bin\jmax.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "$(JAVA_HOME)\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\jmaxrun.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\jmax.pdb" /debug /machine:I386 /out:"..\bin\jmax.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\jmax.obj"

"..\bin\jmax.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("jmaxrun.dep")
!INCLUDE "jmaxrun.dep"
!ELSE 
!MESSAGE Warning: cannot find "jmaxrun.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "jmaxrun - Win32 Release" || "$(CFG)" == "jmaxrun - Win32 Debug"
SOURCE=..\fts\jmax.c

"$(INTDIR)\jmax.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

