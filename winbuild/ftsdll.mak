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

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\fts.dll"


CLEAN :
	-@erase "$(INTDIR)\atomarray.obj"
	-@erase "$(INTDIR)\atomfiles.obj"
	-@erase "$(INTDIR)\atomlist.obj"
	-@erase "$(INTDIR)\atoms.obj"
	-@erase "$(INTDIR)\audio.obj"
	-@erase "$(INTDIR)\audioport.obj"
	-@erase "$(INTDIR)\autosave.obj"
	-@erase "$(INTDIR)\bytestream.obj"
	-@erase "$(INTDIR)\cfft.obj"
	-@erase "$(INTDIR)\channel.obj"
	-@erase "$(INTDIR)\classes.obj"
	-@erase "$(INTDIR)\client.obj"
	-@erase "$(INTDIR)\clipboard.obj"
	-@erase "$(INTDIR)\complex.obj"
	-@erase "$(INTDIR)\connections.obj"
	-@erase "$(INTDIR)\cubic.obj"
	-@erase "$(INTDIR)\datalib.obj"
	-@erase "$(INTDIR)\devconf.obj"
	-@erase "$(INTDIR)\devices.obj"
	-@erase "$(INTDIR)\directories.obj"
	-@erase "$(INTDIR)\doctor.obj"
	-@erase "$(INTDIR)\dsp.obj"
	-@erase "$(INTDIR)\dspcontrol.obj"
	-@erase "$(INTDIR)\dspgraph.obj"
	-@erase "$(INTDIR)\errobj.obj"
	-@erase "$(INTDIR)\expressions.obj"
	-@erase "$(INTDIR)\files.obj"
	-@erase "$(INTDIR)\floatfuns.obj"
	-@erase "$(INTDIR)\fpe.obj"
	-@erase "$(INTDIR)\ftl.obj"
	-@erase "$(INTDIR)\ftlmem.obj"
	-@erase "$(INTDIR)\ftlutils.obj"
	-@erase "$(INTDIR)\ftsdata.obj"
	-@erase "$(INTDIR)\ftsdataid.obj"
	-@erase "$(INTDIR)\gphiter.obj"
	-@erase "$(INTDIR)\hashtable.obj"
	-@erase "$(INTDIR)\incoming.obj"
	-@erase "$(INTDIR)\inout.obj"
	-@erase "$(INTDIR)\label.obj"
	-@erase "$(INTDIR)\loader.obj"
	-@erase "$(INTDIR)\mem.obj"
	-@erase "$(INTDIR)\mess.obj"
	-@erase "$(INTDIR)\messages.obj"
	-@erase "$(INTDIR)\messtile.obj"
	-@erase "$(INTDIR)\midi.obj"
	-@erase "$(INTDIR)\midifiles.obj"
	-@erase "$(INTDIR)\midiparser.obj"
	-@erase "$(INTDIR)\midiport.obj"
	-@erase "$(INTDIR)\modules.obj"
	-@erase "$(INTDIR)\nullaudioport.obj"
	-@erase "$(INTDIR)\objectlist.obj"
	-@erase "$(INTDIR)\objects.obj"
	-@erase "$(INTDIR)\objectset.obj"
	-@erase "$(INTDIR)\objtable.obj"
	-@erase "$(INTDIR)\outgoing.obj"
	-@erase "$(INTDIR)\params.obj"
	-@erase "$(INTDIR)\parser.obj"
	-@erase "$(INTDIR)\patabs.obj"
	-@erase "$(INTDIR)\patcher.obj"
	-@erase "$(INTDIR)\patcherdata.obj"
	-@erase "$(INTDIR)\patlex.obj"
	-@erase "$(INTDIR)\patparser.obj"
	-@erase "$(INTDIR)\post.obj"
	-@erase "$(INTDIR)\properties.obj"
	-@erase "$(INTDIR)\protocol.obj"
	-@erase "$(INTDIR)\ramp_vectors.obj"
	-@erase "$(INTDIR)\ramps.obj"
	-@erase "$(INTDIR)\random.obj"
	-@erase "$(INTDIR)\rfft_shuffle.obj"
	-@erase "$(INTDIR)\saver.obj"
	-@erase "$(INTDIR)\sched.obj"
	-@erase "$(INTDIR)\selection.obj"
	-@erase "$(INTDIR)\sigbus.obj"
	-@erase "$(INTDIR)\sigconn.obj"
	-@erase "$(INTDIR)\signal.obj"
	-@erase "$(INTDIR)\smem.obj"
	-@erase "$(INTDIR)\soundfiles.obj"
	-@erase "$(INTDIR)\soundformats.obj"
	-@erase "$(INTDIR)\srconv.obj"
	-@erase "$(INTDIR)\startup.obj"
	-@erase "$(INTDIR)\status.obj"
	-@erase "$(INTDIR)\symbols.obj"
	-@erase "$(INTDIR)\sync.obj"
	-@erase "$(INTDIR)\templates.obj"
	-@erase "$(INTDIR)\time.obj"
	-@erase "$(INTDIR)\ucs.obj"
	-@erase "$(INTDIR)\updates.obj"
	-@erase "$(INTDIR)\utils.obj"
	-@erase "$(INTDIR)\variables.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vec_fft.obj"
	-@erase "$(INTDIR)\version.obj"
	-@erase "$(INTDIR)\vm.obj"
	-@erase "$(INTDIR)\win32.obj"
	-@erase "$(INTDIR)\windev.obj"
	-@erase "$(OUTDIR)\fts.dll"
	-@erase "$(OUTDIR)\fts.exp"
	-@erase "$(OUTDIR)\fts.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FTSDLL_EXPORTS" /Fp"$(INTDIR)\ftsdll.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\fts.pdb" /machine:I386 /out:"$(OUTDIR)\fts.dll" /implib:"$(OUTDIR)\fts.lib" 
LINK32_OBJS= \
	"$(INTDIR)\atomarray.obj" \
	"$(INTDIR)\atomfiles.obj" \
	"$(INTDIR)\atomlist.obj" \
	"$(INTDIR)\atoms.obj" \
	"$(INTDIR)\audio.obj" \
	"$(INTDIR)\audioport.obj" \
	"$(INTDIR)\autosave.obj" \
	"$(INTDIR)\bytestream.obj" \
	"$(INTDIR)\cfft.obj" \
	"$(INTDIR)\channel.obj" \
	"$(INTDIR)\classes.obj" \
	"$(INTDIR)\client.obj" \
	"$(INTDIR)\clipboard.obj" \
	"$(INTDIR)\complex.obj" \
	"$(INTDIR)\connections.obj" \
	"$(INTDIR)\cubic.obj" \
	"$(INTDIR)\datalib.obj" \
	"$(INTDIR)\devconf.obj" \
	"$(INTDIR)\devices.obj" \
	"$(INTDIR)\directories.obj" \
	"$(INTDIR)\doctor.obj" \
	"$(INTDIR)\dsp.obj" \
	"$(INTDIR)\dspcontrol.obj" \
	"$(INTDIR)\dspgraph.obj" \
	"$(INTDIR)\errobj.obj" \
	"$(INTDIR)\expressions.obj" \
	"$(INTDIR)\files.obj" \
	"$(INTDIR)\floatfuns.obj" \
	"$(INTDIR)\fpe.obj" \
	"$(INTDIR)\ftl.obj" \
	"$(INTDIR)\ftlmem.obj" \
	"$(INTDIR)\ftlutils.obj" \
	"$(INTDIR)\ftsdata.obj" \
	"$(INTDIR)\ftsdataid.obj" \
	"$(INTDIR)\gphiter.obj" \
	"$(INTDIR)\hashtable.obj" \
	"$(INTDIR)\incoming.obj" \
	"$(INTDIR)\inout.obj" \
	"$(INTDIR)\label.obj" \
	"$(INTDIR)\loader.obj" \
	"$(INTDIR)\mem.obj" \
	"$(INTDIR)\mess.obj" \
	"$(INTDIR)\messages.obj" \
	"$(INTDIR)\messtile.obj" \
	"$(INTDIR)\midi.obj" \
	"$(INTDIR)\midifiles.obj" \
	"$(INTDIR)\midiparser.obj" \
	"$(INTDIR)\midiport.obj" \
	"$(INTDIR)\modules.obj" \
	"$(INTDIR)\nullaudioport.obj" \
	"$(INTDIR)\objectlist.obj" \
	"$(INTDIR)\objects.obj" \
	"$(INTDIR)\objectset.obj" \
	"$(INTDIR)\objtable.obj" \
	"$(INTDIR)\outgoing.obj" \
	"$(INTDIR)\params.obj" \
	"$(INTDIR)\parser.obj" \
	"$(INTDIR)\patabs.obj" \
	"$(INTDIR)\patcher.obj" \
	"$(INTDIR)\patcherdata.obj" \
	"$(INTDIR)\patlex.obj" \
	"$(INTDIR)\patparser.obj" \
	"$(INTDIR)\post.obj" \
	"$(INTDIR)\properties.obj" \
	"$(INTDIR)\protocol.obj" \
	"$(INTDIR)\ramp_vectors.obj" \
	"$(INTDIR)\ramps.obj" \
	"$(INTDIR)\random.obj" \
	"$(INTDIR)\rfft_shuffle.obj" \
	"$(INTDIR)\saver.obj" \
	"$(INTDIR)\sched.obj" \
	"$(INTDIR)\selection.obj" \
	"$(INTDIR)\sigbus.obj" \
	"$(INTDIR)\sigconn.obj" \
	"$(INTDIR)\signal.obj" \
	"$(INTDIR)\smem.obj" \
	"$(INTDIR)\soundfiles.obj" \
	"$(INTDIR)\soundformats.obj" \
	"$(INTDIR)\srconv.obj" \
	"$(INTDIR)\startup.obj" \
	"$(INTDIR)\status.obj" \
	"$(INTDIR)\symbols.obj" \
	"$(INTDIR)\sync.obj" \
	"$(INTDIR)\templates.obj" \
	"$(INTDIR)\time.obj" \
	"$(INTDIR)\ucs.obj" \
	"$(INTDIR)\updates.obj" \
	"$(INTDIR)\utils.obj" \
	"$(INTDIR)\variables.obj" \
	"$(INTDIR)\vec_fft.obj" \
	"$(INTDIR)\version.obj" \
	"$(INTDIR)\vm.obj" \
	"$(INTDIR)\win32.obj" \
	"$(INTDIR)\windev.obj"

"$(OUTDIR)\fts.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ftsdll - Win32 Debug"

OUTDIR=.\..\fts\lib\i686-win32\debug
INTDIR=.\..\fts\lib\i686-win32\debug

ALL : "..\fts\bin\i686-win32\debug\fts.dll"


CLEAN :
	-@erase "$(INTDIR)\atomarray.obj"
	-@erase "$(INTDIR)\atomfiles.obj"
	-@erase "$(INTDIR)\atomlist.obj"
	-@erase "$(INTDIR)\atoms.obj"
	-@erase "$(INTDIR)\audio.obj"
	-@erase "$(INTDIR)\audioport.obj"
	-@erase "$(INTDIR)\autosave.obj"
	-@erase "$(INTDIR)\bytestream.obj"
	-@erase "$(INTDIR)\cfft.obj"
	-@erase "$(INTDIR)\channel.obj"
	-@erase "$(INTDIR)\classes.obj"
	-@erase "$(INTDIR)\client.obj"
	-@erase "$(INTDIR)\clipboard.obj"
	-@erase "$(INTDIR)\complex.obj"
	-@erase "$(INTDIR)\connections.obj"
	-@erase "$(INTDIR)\cubic.obj"
	-@erase "$(INTDIR)\datalib.obj"
	-@erase "$(INTDIR)\devconf.obj"
	-@erase "$(INTDIR)\devices.obj"
	-@erase "$(INTDIR)\directories.obj"
	-@erase "$(INTDIR)\doctor.obj"
	-@erase "$(INTDIR)\dsp.obj"
	-@erase "$(INTDIR)\dspcontrol.obj"
	-@erase "$(INTDIR)\dspgraph.obj"
	-@erase "$(INTDIR)\errobj.obj"
	-@erase "$(INTDIR)\expressions.obj"
	-@erase "$(INTDIR)\files.obj"
	-@erase "$(INTDIR)\floatfuns.obj"
	-@erase "$(INTDIR)\fpe.obj"
	-@erase "$(INTDIR)\ftl.obj"
	-@erase "$(INTDIR)\ftlmem.obj"
	-@erase "$(INTDIR)\ftlutils.obj"
	-@erase "$(INTDIR)\ftsdata.obj"
	-@erase "$(INTDIR)\ftsdataid.obj"
	-@erase "$(INTDIR)\gphiter.obj"
	-@erase "$(INTDIR)\hashtable.obj"
	-@erase "$(INTDIR)\incoming.obj"
	-@erase "$(INTDIR)\inout.obj"
	-@erase "$(INTDIR)\label.obj"
	-@erase "$(INTDIR)\loader.obj"
	-@erase "$(INTDIR)\mem.obj"
	-@erase "$(INTDIR)\mess.obj"
	-@erase "$(INTDIR)\messages.obj"
	-@erase "$(INTDIR)\messtile.obj"
	-@erase "$(INTDIR)\midi.obj"
	-@erase "$(INTDIR)\midifiles.obj"
	-@erase "$(INTDIR)\midiparser.obj"
	-@erase "$(INTDIR)\midiport.obj"
	-@erase "$(INTDIR)\modules.obj"
	-@erase "$(INTDIR)\nullaudioport.obj"
	-@erase "$(INTDIR)\objectlist.obj"
	-@erase "$(INTDIR)\objects.obj"
	-@erase "$(INTDIR)\objectset.obj"
	-@erase "$(INTDIR)\objtable.obj"
	-@erase "$(INTDIR)\outgoing.obj"
	-@erase "$(INTDIR)\params.obj"
	-@erase "$(INTDIR)\parser.obj"
	-@erase "$(INTDIR)\patabs.obj"
	-@erase "$(INTDIR)\patcher.obj"
	-@erase "$(INTDIR)\patcherdata.obj"
	-@erase "$(INTDIR)\patlex.obj"
	-@erase "$(INTDIR)\patparser.obj"
	-@erase "$(INTDIR)\post.obj"
	-@erase "$(INTDIR)\properties.obj"
	-@erase "$(INTDIR)\protocol.obj"
	-@erase "$(INTDIR)\ramp_vectors.obj"
	-@erase "$(INTDIR)\ramps.obj"
	-@erase "$(INTDIR)\random.obj"
	-@erase "$(INTDIR)\rfft_shuffle.obj"
	-@erase "$(INTDIR)\saver.obj"
	-@erase "$(INTDIR)\sched.obj"
	-@erase "$(INTDIR)\selection.obj"
	-@erase "$(INTDIR)\sigbus.obj"
	-@erase "$(INTDIR)\sigconn.obj"
	-@erase "$(INTDIR)\signal.obj"
	-@erase "$(INTDIR)\smem.obj"
	-@erase "$(INTDIR)\soundfiles.obj"
	-@erase "$(INTDIR)\soundformats.obj"
	-@erase "$(INTDIR)\srconv.obj"
	-@erase "$(INTDIR)\startup.obj"
	-@erase "$(INTDIR)\status.obj"
	-@erase "$(INTDIR)\symbols.obj"
	-@erase "$(INTDIR)\sync.obj"
	-@erase "$(INTDIR)\templates.obj"
	-@erase "$(INTDIR)\time.obj"
	-@erase "$(INTDIR)\ucs.obj"
	-@erase "$(INTDIR)\updates.obj"
	-@erase "$(INTDIR)\utils.obj"
	-@erase "$(INTDIR)\variables.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\vec_fft.obj"
	-@erase "$(INTDIR)\version.obj"
	-@erase "$(INTDIR)\vm.obj"
	-@erase "$(INTDIR)\win32.obj"
	-@erase "$(INTDIR)\windev.obj"
	-@erase "$(OUTDIR)\fts.exp"
	-@erase "$(OUTDIR)\fts.lib"
	-@erase "$(OUTDIR)\fts.pdb"
	-@erase "..\fts\bin\i686-win32\debug\fts.dll"
	-@erase "..\fts\bin\i686-win32\debug\fts.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

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
LINK32_FLAGS=ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\fts.pdb" /debug /machine:I386 /out:"..\fts\bin\i686-win32\debug\fts.dll" /implib:"$(OUTDIR)\fts.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\atomarray.obj" \
	"$(INTDIR)\atomfiles.obj" \
	"$(INTDIR)\atomlist.obj" \
	"$(INTDIR)\atoms.obj" \
	"$(INTDIR)\audio.obj" \
	"$(INTDIR)\audioport.obj" \
	"$(INTDIR)\autosave.obj" \
	"$(INTDIR)\bytestream.obj" \
	"$(INTDIR)\cfft.obj" \
	"$(INTDIR)\channel.obj" \
	"$(INTDIR)\classes.obj" \
	"$(INTDIR)\client.obj" \
	"$(INTDIR)\clipboard.obj" \
	"$(INTDIR)\complex.obj" \
	"$(INTDIR)\connections.obj" \
	"$(INTDIR)\cubic.obj" \
	"$(INTDIR)\datalib.obj" \
	"$(INTDIR)\devconf.obj" \
	"$(INTDIR)\devices.obj" \
	"$(INTDIR)\directories.obj" \
	"$(INTDIR)\doctor.obj" \
	"$(INTDIR)\dsp.obj" \
	"$(INTDIR)\dspcontrol.obj" \
	"$(INTDIR)\dspgraph.obj" \
	"$(INTDIR)\errobj.obj" \
	"$(INTDIR)\expressions.obj" \
	"$(INTDIR)\files.obj" \
	"$(INTDIR)\floatfuns.obj" \
	"$(INTDIR)\fpe.obj" \
	"$(INTDIR)\ftl.obj" \
	"$(INTDIR)\ftlmem.obj" \
	"$(INTDIR)\ftlutils.obj" \
	"$(INTDIR)\ftsdata.obj" \
	"$(INTDIR)\ftsdataid.obj" \
	"$(INTDIR)\gphiter.obj" \
	"$(INTDIR)\hashtable.obj" \
	"$(INTDIR)\incoming.obj" \
	"$(INTDIR)\inout.obj" \
	"$(INTDIR)\label.obj" \
	"$(INTDIR)\loader.obj" \
	"$(INTDIR)\mem.obj" \
	"$(INTDIR)\mess.obj" \
	"$(INTDIR)\messages.obj" \
	"$(INTDIR)\messtile.obj" \
	"$(INTDIR)\midi.obj" \
	"$(INTDIR)\midifiles.obj" \
	"$(INTDIR)\midiparser.obj" \
	"$(INTDIR)\midiport.obj" \
	"$(INTDIR)\modules.obj" \
	"$(INTDIR)\nullaudioport.obj" \
	"$(INTDIR)\objectlist.obj" \
	"$(INTDIR)\objects.obj" \
	"$(INTDIR)\objectset.obj" \
	"$(INTDIR)\objtable.obj" \
	"$(INTDIR)\outgoing.obj" \
	"$(INTDIR)\params.obj" \
	"$(INTDIR)\parser.obj" \
	"$(INTDIR)\patabs.obj" \
	"$(INTDIR)\patcher.obj" \
	"$(INTDIR)\patcherdata.obj" \
	"$(INTDIR)\patlex.obj" \
	"$(INTDIR)\patparser.obj" \
	"$(INTDIR)\post.obj" \
	"$(INTDIR)\properties.obj" \
	"$(INTDIR)\protocol.obj" \
	"$(INTDIR)\ramp_vectors.obj" \
	"$(INTDIR)\ramps.obj" \
	"$(INTDIR)\random.obj" \
	"$(INTDIR)\rfft_shuffle.obj" \
	"$(INTDIR)\saver.obj" \
	"$(INTDIR)\sched.obj" \
	"$(INTDIR)\selection.obj" \
	"$(INTDIR)\sigbus.obj" \
	"$(INTDIR)\sigconn.obj" \
	"$(INTDIR)\signal.obj" \
	"$(INTDIR)\smem.obj" \
	"$(INTDIR)\soundfiles.obj" \
	"$(INTDIR)\soundformats.obj" \
	"$(INTDIR)\srconv.obj" \
	"$(INTDIR)\startup.obj" \
	"$(INTDIR)\status.obj" \
	"$(INTDIR)\symbols.obj" \
	"$(INTDIR)\sync.obj" \
	"$(INTDIR)\templates.obj" \
	"$(INTDIR)\time.obj" \
	"$(INTDIR)\ucs.obj" \
	"$(INTDIR)\updates.obj" \
	"$(INTDIR)\utils.obj" \
	"$(INTDIR)\variables.obj" \
	"$(INTDIR)\vec_fft.obj" \
	"$(INTDIR)\version.obj" \
	"$(INTDIR)\vm.obj" \
	"$(INTDIR)\win32.obj" \
	"$(INTDIR)\windev.obj"

"..\fts\bin\i686-win32\debug\fts.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
SOURCE=..\fts\src\lang\mess\atomarray.c

"$(INTDIR)\atomarray.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\runtime\files\atomfiles.c

"$(INTDIR)\atomfiles.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\datalib\atomlist.c

"$(INTDIR)\atomlist.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\mess\atoms.c

"$(INTDIR)\atoms.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\runtime\audio\audio.c

"$(INTDIR)\audio.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\runtime\audio\audioport.c

"$(INTDIR)\audioport.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\mess\autosave.c

"$(INTDIR)\autosave.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\runtime\devices\bytestream.c

"$(INTDIR)\bytestream.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\cfft.c

"$(INTDIR)\cfft.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\mess\channel.c

"$(INTDIR)\channel.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\mess\classes.c

"$(INTDIR)\classes.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\runtime\client\client.c

"$(INTDIR)\client.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\mess\clipboard.c

"$(INTDIR)\clipboard.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\complex.c

"$(INTDIR)\complex.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\mess\connections.c

"$(INTDIR)\connections.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\utils\cubic.c

"$(INTDIR)\cubic.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\datalib\datalib.c

"$(INTDIR)\datalib.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\runtime\devices\devconf.c

"$(INTDIR)\devconf.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\runtime\devices\devices.c

"$(INTDIR)\devices.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\runtime\files\directories.c

"$(INTDIR)\directories.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\mess\doctor.c

"$(INTDIR)\doctor.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\dsp\dsp.c

"$(INTDIR)\dsp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\dsp\dspcontrol.c

"$(INTDIR)\dspcontrol.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\dsp\dspgraph.c

"$(INTDIR)\dspgraph.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\mess\errobj.c

"$(INTDIR)\errobj.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\mess\expressions.c

"$(INTDIR)\expressions.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\runtime\files\files.c

"$(INTDIR)\files.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\utils\floatfuns.c

"$(INTDIR)\floatfuns.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\mess\fpe.c

"$(INTDIR)\fpe.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\ftl\ftl.c

"$(INTDIR)\ftl.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\ftl\ftlmem.c

"$(INTDIR)\ftlmem.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\ftl\ftlutils.c

"$(INTDIR)\ftlutils.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\mess\ftsdata.c

"$(INTDIR)\ftsdata.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\mess\ftsdataid.c

"$(INTDIR)\ftsdataid.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\dsp\gphiter.c

"$(INTDIR)\gphiter.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\utils\hashtable.c

"$(INTDIR)\hashtable.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\runtime\client\incoming.c

"$(INTDIR)\incoming.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\mess\inout.c

"$(INTDIR)\inout.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\mess\label.c

"$(INTDIR)\label.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\mess\loader.c

"$(INTDIR)\loader.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\sys\mem.c

"$(INTDIR)\mem.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\mess\mess.c

"$(INTDIR)\mess.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\mess\messages.c

"$(INTDIR)\messages.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\tiles\messtile.c

"$(INTDIR)\messtile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\runtime\midi\midi.c

"$(INTDIR)\midi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\runtime\files\midifiles.c

"$(INTDIR)\midifiles.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\runtime\midi\midiparser.c

"$(INTDIR)\midiparser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\runtime\midi\midiport.c

"$(INTDIR)\midiport.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\sys\modules.c

"$(INTDIR)\modules.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\runtime\audio\nullaudioport.c

"$(INTDIR)\nullaudioport.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\utils\objectlist.c

"$(INTDIR)\objectlist.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\mess\objects.c

"$(INTDIR)\objects.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\datalib\objectset.c

"$(INTDIR)\objectset.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\mess\objtable.c

"$(INTDIR)\objtable.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\runtime\client\outgoing.c

"$(INTDIR)\outgoing.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\mess\params.c

"$(INTDIR)\params.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\runtime\files\parser.c

"$(INTDIR)\parser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\mess\patabs.c

"$(INTDIR)\patabs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\mess\patcher.c

"$(INTDIR)\patcher.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\mess\patcherdata.c

"$(INTDIR)\patcherdata.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\mess\patlex.c

"$(INTDIR)\patlex.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\mess\patparser.c

"$(INTDIR)\patparser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\runtime\files\post.c

"$(INTDIR)\post.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\mess\properties.c

"$(INTDIR)\properties.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\runtime\client\protocol.c

"$(INTDIR)\protocol.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\utils\ramp_vectors.c

"$(INTDIR)\ramp_vectors.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\utils\ramps.c

"$(INTDIR)\ramps.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\utils\random.c

"$(INTDIR)\random.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\rfft_shuffle.c

"$(INTDIR)\rfft_shuffle.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\mess\saver.c

"$(INTDIR)\saver.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\runtime\sched\sched.c

"$(INTDIR)\sched.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\mess\selection.c

"$(INTDIR)\selection.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\dsp\sigbus.c

"$(INTDIR)\sigbus.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\dsp\sigconn.c

"$(INTDIR)\sigconn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\dsp\signal.c

"$(INTDIR)\signal.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\sys\smem.c

"$(INTDIR)\smem.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\runtime\files\soundfiles.c

"$(INTDIR)\soundfiles.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\runtime\files\soundformats.c

"$(INTDIR)\soundformats.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\utils\srconv.c

"$(INTDIR)\srconv.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\tiles\startup.c

"$(INTDIR)\startup.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\sys\status.c

"$(INTDIR)\status.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\mess\symbols.c

"$(INTDIR)\symbols.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\runtime\client\sync.c

"$(INTDIR)\sync.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\mess\templates.c

"$(INTDIR)\templates.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\runtime\time\time.c

"$(INTDIR)\time.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\tiles\ucs.c

"$(INTDIR)\ucs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\runtime\client\updates.c

"$(INTDIR)\updates.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\utils\utils.c

"$(INTDIR)\utils.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\mess\variables.c

"$(INTDIR)\variables.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\vec_fft.c

"$(INTDIR)\vec_fft.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\sys\version.c

"$(INTDIR)\version.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\mess\vm.c

"$(INTDIR)\vm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\fts\src\non-portable\win32.c"

"$(INTDIR)\win32.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\runtime\devices\windev.c

"$(INTDIR)\windev.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

