# Microsoft Developer Studio Generated NMAKE File, Based on ftsclient.dsp
!IF "$(CFG)" == ""
CFG=ftsclient - Win32 Debug
!MESSAGE No configuration specified. Defaulting to ftsclient - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ftsclient - Win32 Release" && "$(CFG)" != "ftsclient - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ftsclient.mak" CFG="ftsclient - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ftsclient - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ftsclient - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "ftsclient - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\windows\ftsclient.dll"


CLEAN :
	-@erase "$(INTDIR)\ftsclient.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\ftsclient.exp"
	-@erase "$(OUTDIR)\ftsclient.lib"
	-@erase "..\..\..\windows\ftsclient.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FTSCLIENT_EXPORTS" /Fp"$(INTDIR)\ftsclient.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ftsclient.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  ws2_32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\ftsclient.pdb" /machine:I386 /out:"c:\windows\ftsclient.dll" /implib:"$(OUTDIR)\ftsclient.lib" 
LINK32_OBJS= \
	"$(INTDIR)\ftsclient.obj"

"..\..\..\windows\ftsclient.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ftsclient - Win32 Debug"

OUTDIR=.
INTDIR=.\Debug

ALL : "..\..\..\windows\ftsclient.dll"


CLEAN :
	-@erase "$(INTDIR)\ftsclient.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ftsclient.exp"
	-@erase "$(OUTDIR)\ftsclient.lib"
	-@erase "$(OUTDIR)\ftsclient.pdb"
	-@erase "..\..\..\windows\ftsclient.dll"
	-@erase "..\..\..\windows\ftsclient.ilk"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FTSCLIENT_EXPORTS" /Fp"$(INTDIR)\ftsclient.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ftsclient.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\ftsclient.pdb" /debug /machine:I386 /out:"c:\windows\ftsclient.dll" /implib:"$(OUTDIR)\ftsclient.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\ftsclient.obj"

"..\..\..\windows\ftsclient.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("ftsclient.dep")
!INCLUDE "ftsclient.dep"
!ELSE 
!MESSAGE Warning: cannot find "ftsclient.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "ftsclient - Win32 Release" || "$(CFG)" == "ftsclient - Win32 Debug"
SOURCE=..\cpp\ftsclient.cpp

"$(INTDIR)\ftsclient.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

