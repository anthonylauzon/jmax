# Microsoft Developer Studio Generated NMAKE File, Based on ftsdll.dsp
!IF "$(CFG)" == ""
CFG=ftsdll - Win32 Debug
!MESSAGE No configuration specified. Defaulting to ftsdll - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ftsdll - Win32 Release" && "$(CFG)" != "ftsdll - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ftsdll.mak" CFG="ftsdll - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ftsdll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ftsdll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "ftsdll - Win32 Release"

OUTDIR=.\..\fts\lib
INTDIR=.\Release

ALL : "..\bin\fts.dll"


CLEAN :
	-@erase "$(INTDIR)\abstraction.obj"
	-@erase "$(INTDIR)\array.obj"
	-@erase "$(INTDIR)\atom.obj"
	-@erase "$(INTDIR)\atomfile.obj"
	-@erase "$(INTDIR)\audio.obj"
	-@erase "$(INTDIR)\audioconfig.obj"
	-@erase "$(INTDIR)\audiofile.obj"
	-@erase "$(INTDIR)\autosave.obj"
	-@erase "$(INTDIR)\binaryprotocol.obj"
	-@erase "$(INTDIR)\bmaxfile.obj"
	-@erase "$(INTDIR)\bytestream.obj"
	-@erase "$(INTDIR)\class.obj"
	-@erase "$(INTDIR)\client.obj"
	-@erase "$(INTDIR)\clientmanager.obj"
	-@erase "$(INTDIR)\clipboard.obj"
	-@erase "$(INTDIR)\config.obj"
	-@erase "$(INTDIR)\connection.obj"
	-@erase "$(INTDIR)\dsp.obj"
	-@erase "$(INTDIR)\dspgraph.obj"
	-@erase "$(INTDIR)\errobj.obj"
	-@erase "$(INTDIR)\expression.obj"
	-@erase "$(INTDIR)\fifo.obj"
	-@erase "$(INTDIR)\file.obj"
	-@erase "$(INTDIR)\fpe.obj"
	-@erase "$(INTDIR)\ftl.obj"
	-@erase "$(INTDIR)\ftlmem.obj"
	-@erase "$(INTDIR)\fts.obj"
	-@erase "$(INTDIR)\hashtable.obj"
	-@erase "$(INTDIR)\label.obj"
	-@erase "$(INTDIR)\list.obj"
	-@erase "$(INTDIR)\loader.obj"
	-@erase "$(INTDIR)\mem.obj"
	-@erase "$(INTDIR)\message.obj"
	-@erase "$(INTDIR)\midi.obj"
	-@erase "$(INTDIR)\midifile.obj"
	-@erase "$(INTDIR)\object.obj"
	-@erase "$(INTDIR)\objectlist.obj"
	-@erase "$(INTDIR)\objectset.obj"
	-@erase "$(INTDIR)\package.obj"
	-@erase "$(INTDIR)\param.obj"
	-@erase "$(INTDIR)\parser.obj"
	-@erase "$(INTDIR)\patcher.obj"
	-@erase "$(INTDIR)\patfile.obj"
	-@erase "$(INTDIR)\post.obj"
	-@erase "$(INTDIR)\preset.obj"
	-@erase "$(INTDIR)\project.obj"
	-@erase "$(INTDIR)\property.obj"
	-@erase "$(INTDIR)\sched.obj"
	-@erase "$(INTDIR)\selection.obj"
	-@erase "$(INTDIR)\sigconn.obj"
	-@erase "$(INTDIR)\socketstream.obj"
	-@erase "$(INTDIR)\stack.obj"
	-@erase "$(INTDIR)\status.obj"
	-@erase "$(INTDIR)\symbol.obj"
	-@erase "$(INTDIR)\template.obj"
	-@erase "$(INTDIR)\thread.obj"
	-@erase "$(INTDIR)\time.obj"
	-@erase "$(INTDIR)\tokenizer.obj"
	-@erase "$(INTDIR)\tuple.obj"
	-@erase "$(INTDIR)\update.obj"
	-@erase "$(INTDIR)\variable.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\version.obj"
	-@erase "$(INTDIR)\win32.obj"
	-@erase "$(OUTDIR)\fts.exp"
	-@erase "$(OUTDIR)\fts.lib"
	-@erase "..\bin\fts.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FTSDLL_EXPORTS" /Fp"$(INTDIR)\ftsdll.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ftsdll.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib winmm.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\fts.pdb" /machine:I386 /out:"..\bin\fts.dll" /implib:"$(OUTDIR)\fts.lib" 
LINK32_OBJS= \
	"$(INTDIR)\abstraction.obj" \
	"$(INTDIR)\array.obj" \
	"$(INTDIR)\atom.obj" \
	"$(INTDIR)\atomfile.obj" \
	"$(INTDIR)\audio.obj" \
	"$(INTDIR)\audioconfig.obj" \
	"$(INTDIR)\audiofile.obj" \
	"$(INTDIR)\autosave.obj" \
	"$(INTDIR)\binaryprotocol.obj" \
	"$(INTDIR)\bmaxfile.obj" \
	"$(INTDIR)\bytestream.obj" \
	"$(INTDIR)\class.obj" \
	"$(INTDIR)\client.obj" \
	"$(INTDIR)\clientmanager.obj" \
	"$(INTDIR)\clipboard.obj" \
	"$(INTDIR)\config.obj" \
	"$(INTDIR)\connection.obj" \
	"$(INTDIR)\dsp.obj" \
	"$(INTDIR)\dspgraph.obj" \
	"$(INTDIR)\errobj.obj" \
	"$(INTDIR)\expression.obj" \
	"$(INTDIR)\fifo.obj" \
	"$(INTDIR)\file.obj" \
	"$(INTDIR)\fpe.obj" \
	"$(INTDIR)\ftl.obj" \
	"$(INTDIR)\ftlmem.obj" \
	"$(INTDIR)\fts.obj" \
	"$(INTDIR)\hashtable.obj" \
	"$(INTDIR)\label.obj" \
	"$(INTDIR)\list.obj" \
	"$(INTDIR)\loader.obj" \
	"$(INTDIR)\mem.obj" \
	"$(INTDIR)\message.obj" \
	"$(INTDIR)\midi.obj" \
	"$(INTDIR)\midifile.obj" \
	"$(INTDIR)\object.obj" \
	"$(INTDIR)\objectlist.obj" \
	"$(INTDIR)\objectset.obj" \
	"$(INTDIR)\package.obj" \
	"$(INTDIR)\param.obj" \
	"$(INTDIR)\parser.obj" \
	"$(INTDIR)\patcher.obj" \
	"$(INTDIR)\patfile.obj" \
	"$(INTDIR)\post.obj" \
	"$(INTDIR)\preset.obj" \
	"$(INTDIR)\project.obj" \
	"$(INTDIR)\property.obj" \
	"$(INTDIR)\sched.obj" \
	"$(INTDIR)\selection.obj" \
	"$(INTDIR)\sigconn.obj" \
	"$(INTDIR)\socketstream.obj" \
	"$(INTDIR)\stack.obj" \
	"$(INTDIR)\status.obj" \
	"$(INTDIR)\symbol.obj" \
	"$(INTDIR)\template.obj" \
	"$(INTDIR)\thread.obj" \
	"$(INTDIR)\time.obj" \
	"$(INTDIR)\tokenizer.obj" \
	"$(INTDIR)\tuple.obj" \
	"$(INTDIR)\update.obj" \
	"$(INTDIR)\variable.obj" \
	"$(INTDIR)\version.obj" \
	"$(INTDIR)\win32.obj"

"..\bin\fts.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ftsdll - Win32 Debug"

OUTDIR=.\..\fts\lib
INTDIR=.\Debug

ALL : "..\bin\fts.dll"


CLEAN :
	-@erase "$(INTDIR)\abstraction.obj"
	-@erase "$(INTDIR)\array.obj"
	-@erase "$(INTDIR)\atom.obj"
	-@erase "$(INTDIR)\atomfile.obj"
	-@erase "$(INTDIR)\audio.obj"
	-@erase "$(INTDIR)\audioconfig.obj"
	-@erase "$(INTDIR)\audiofile.obj"
	-@erase "$(INTDIR)\autosave.obj"
	-@erase "$(INTDIR)\binaryprotocol.obj"
	-@erase "$(INTDIR)\bmaxfile.obj"
	-@erase "$(INTDIR)\bytestream.obj"
	-@erase "$(INTDIR)\class.obj"
	-@erase "$(INTDIR)\client.obj"
	-@erase "$(INTDIR)\clientmanager.obj"
	-@erase "$(INTDIR)\clipboard.obj"
	-@erase "$(INTDIR)\config.obj"
	-@erase "$(INTDIR)\connection.obj"
	-@erase "$(INTDIR)\dsp.obj"
	-@erase "$(INTDIR)\dspgraph.obj"
	-@erase "$(INTDIR)\errobj.obj"
	-@erase "$(INTDIR)\expression.obj"
	-@erase "$(INTDIR)\fifo.obj"
	-@erase "$(INTDIR)\file.obj"
	-@erase "$(INTDIR)\fpe.obj"
	-@erase "$(INTDIR)\ftl.obj"
	-@erase "$(INTDIR)\ftlmem.obj"
	-@erase "$(INTDIR)\fts.obj"
	-@erase "$(INTDIR)\hashtable.obj"
	-@erase "$(INTDIR)\label.obj"
	-@erase "$(INTDIR)\list.obj"
	-@erase "$(INTDIR)\loader.obj"
	-@erase "$(INTDIR)\mem.obj"
	-@erase "$(INTDIR)\message.obj"
	-@erase "$(INTDIR)\midi.obj"
	-@erase "$(INTDIR)\midifile.obj"
	-@erase "$(INTDIR)\object.obj"
	-@erase "$(INTDIR)\objectlist.obj"
	-@erase "$(INTDIR)\objectset.obj"
	-@erase "$(INTDIR)\package.obj"
	-@erase "$(INTDIR)\param.obj"
	-@erase "$(INTDIR)\parser.obj"
	-@erase "$(INTDIR)\patcher.obj"
	-@erase "$(INTDIR)\patfile.obj"
	-@erase "$(INTDIR)\post.obj"
	-@erase "$(INTDIR)\preset.obj"
	-@erase "$(INTDIR)\project.obj"
	-@erase "$(INTDIR)\property.obj"
	-@erase "$(INTDIR)\sched.obj"
	-@erase "$(INTDIR)\selection.obj"
	-@erase "$(INTDIR)\sigconn.obj"
	-@erase "$(INTDIR)\socketstream.obj"
	-@erase "$(INTDIR)\stack.obj"
	-@erase "$(INTDIR)\status.obj"
	-@erase "$(INTDIR)\symbol.obj"
	-@erase "$(INTDIR)\template.obj"
	-@erase "$(INTDIR)\thread.obj"
	-@erase "$(INTDIR)\time.obj"
	-@erase "$(INTDIR)\tokenizer.obj"
	-@erase "$(INTDIR)\tuple.obj"
	-@erase "$(INTDIR)\update.obj"
	-@erase "$(INTDIR)\variable.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\version.obj"
	-@erase "$(INTDIR)\win32.obj"
	-@erase "$(OUTDIR)\fts.exp"
	-@erase "$(OUTDIR)\fts.lib"
	-@erase "$(OUTDIR)\fts.pdb"
	-@erase "..\bin\fts.dll"
	-@erase "..\bin\fts.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FTSDLL_EXPORTS" /Fp"$(INTDIR)\ftsdll.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ftsdll.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\fts.pdb" /debug /machine:I386 /out:"..\bin\fts.dll" /implib:"$(OUTDIR)\fts.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\abstraction.obj" \
	"$(INTDIR)\array.obj" \
	"$(INTDIR)\atom.obj" \
	"$(INTDIR)\atomfile.obj" \
	"$(INTDIR)\audio.obj" \
	"$(INTDIR)\audioconfig.obj" \
	"$(INTDIR)\audiofile.obj" \
	"$(INTDIR)\autosave.obj" \
	"$(INTDIR)\binaryprotocol.obj" \
	"$(INTDIR)\bmaxfile.obj" \
	"$(INTDIR)\bytestream.obj" \
	"$(INTDIR)\class.obj" \
	"$(INTDIR)\client.obj" \
	"$(INTDIR)\clientmanager.obj" \
	"$(INTDIR)\clipboard.obj" \
	"$(INTDIR)\config.obj" \
	"$(INTDIR)\connection.obj" \
	"$(INTDIR)\dsp.obj" \
	"$(INTDIR)\dspgraph.obj" \
	"$(INTDIR)\errobj.obj" \
	"$(INTDIR)\expression.obj" \
	"$(INTDIR)\fifo.obj" \
	"$(INTDIR)\file.obj" \
	"$(INTDIR)\fpe.obj" \
	"$(INTDIR)\ftl.obj" \
	"$(INTDIR)\ftlmem.obj" \
	"$(INTDIR)\fts.obj" \
	"$(INTDIR)\hashtable.obj" \
	"$(INTDIR)\label.obj" \
	"$(INTDIR)\list.obj" \
	"$(INTDIR)\loader.obj" \
	"$(INTDIR)\mem.obj" \
	"$(INTDIR)\message.obj" \
	"$(INTDIR)\midi.obj" \
	"$(INTDIR)\midifile.obj" \
	"$(INTDIR)\object.obj" \
	"$(INTDIR)\objectlist.obj" \
	"$(INTDIR)\objectset.obj" \
	"$(INTDIR)\package.obj" \
	"$(INTDIR)\param.obj" \
	"$(INTDIR)\parser.obj" \
	"$(INTDIR)\patcher.obj" \
	"$(INTDIR)\patfile.obj" \
	"$(INTDIR)\post.obj" \
	"$(INTDIR)\preset.obj" \
	"$(INTDIR)\project.obj" \
	"$(INTDIR)\property.obj" \
	"$(INTDIR)\sched.obj" \
	"$(INTDIR)\selection.obj" \
	"$(INTDIR)\sigconn.obj" \
	"$(INTDIR)\socketstream.obj" \
	"$(INTDIR)\stack.obj" \
	"$(INTDIR)\status.obj" \
	"$(INTDIR)\symbol.obj" \
	"$(INTDIR)\template.obj" \
	"$(INTDIR)\thread.obj" \
	"$(INTDIR)\time.obj" \
	"$(INTDIR)\tokenizer.obj" \
	"$(INTDIR)\tuple.obj" \
	"$(INTDIR)\update.obj" \
	"$(INTDIR)\variable.obj" \
	"$(INTDIR)\version.obj" \
	"$(INTDIR)\win32.obj"

"..\bin\fts.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("ftsdll.dep")
!INCLUDE "ftsdll.dep"
!ELSE 
!MESSAGE Warning: cannot find "ftsdll.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "ftsdll - Win32 Release" || "$(CFG)" == "ftsdll - Win32 Debug"
SOURCE=..\fts\abstraction.c

"$(INTDIR)\abstraction.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\array.c

"$(INTDIR)\array.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\atom.c

"$(INTDIR)\atom.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\atomfile.c

"$(INTDIR)\atomfile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\audio.c

"$(INTDIR)\audio.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\audioconfig.c

"$(INTDIR)\audioconfig.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\audiofile.c

"$(INTDIR)\audiofile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\autosave.c

"$(INTDIR)\autosave.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\binaryprotocol.c

"$(INTDIR)\binaryprotocol.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\bmaxfile.c

"$(INTDIR)\bmaxfile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\bytestream.c

"$(INTDIR)\bytestream.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\class.c

"$(INTDIR)\class.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\client.c

"$(INTDIR)\client.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\clientmanager.c

"$(INTDIR)\clientmanager.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\clipboard.c

"$(INTDIR)\clipboard.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\config.c

"$(INTDIR)\config.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\connection.c

"$(INTDIR)\connection.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\dsp.c

"$(INTDIR)\dsp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\dspgraph.c

"$(INTDIR)\dspgraph.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\errobj.c

"$(INTDIR)\errobj.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\expression.c

"$(INTDIR)\expression.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\fifo.c

"$(INTDIR)\fifo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\file.c

"$(INTDIR)\file.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\fpe.c

"$(INTDIR)\fpe.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\ftl.c

"$(INTDIR)\ftl.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\ftlmem.c

"$(INTDIR)\ftlmem.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\fts.c

"$(INTDIR)\fts.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\hashtable.c

"$(INTDIR)\hashtable.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\label.c

"$(INTDIR)\label.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\list.c

"$(INTDIR)\list.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\loader.c

"$(INTDIR)\loader.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\mem.c

"$(INTDIR)\mem.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\message.c

"$(INTDIR)\message.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\midi.c

"$(INTDIR)\midi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\midifile.c

"$(INTDIR)\midifile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\object.c

"$(INTDIR)\object.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\objectlist.c

"$(INTDIR)\objectlist.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\objectset.c

"$(INTDIR)\objectset.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\package.c

"$(INTDIR)\package.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\param.c

"$(INTDIR)\param.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\parser.c

"$(INTDIR)\parser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\patcher.c

"$(INTDIR)\patcher.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\patfile.c

"$(INTDIR)\patfile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\post.c

"$(INTDIR)\post.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\preset.c

"$(INTDIR)\preset.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\project.c

"$(INTDIR)\project.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\property.c

"$(INTDIR)\property.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\sched.c

"$(INTDIR)\sched.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\selection.c

"$(INTDIR)\selection.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\sigconn.c

"$(INTDIR)\sigconn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\socketstream.c

"$(INTDIR)\socketstream.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\stack.c

"$(INTDIR)\stack.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\status.c

"$(INTDIR)\status.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\symbol.c

"$(INTDIR)\symbol.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\template.c

"$(INTDIR)\template.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\thread.c

"$(INTDIR)\thread.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\time.c

"$(INTDIR)\time.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\tokenizer.c

"$(INTDIR)\tokenizer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\tuple.c

"$(INTDIR)\tuple.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\update.c

"$(INTDIR)\update.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\variable.c

"$(INTDIR)\variable.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\version.c

"$(INTDIR)\version.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\win32.c

"$(INTDIR)\win32.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

