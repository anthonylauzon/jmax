# Microsoft Developer Studio Project File - Name="ftsdll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ftsdll - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ftsdll.mak".
!MESSAGE 
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

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ftsdll - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FTSDLL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FTSDLL_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"Release/fts.dll"

!ELSEIF  "$(CFG)" == "ftsdll - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\fts\lib\i686-win32\debug"
# PROP Intermediate_Dir "..\fts\lib\i686-win32\debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FTSDLL_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FTSDLL_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /dll /debug /machine:I386 /out:"..\fts\bin\i686-win32\debug\fts.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "ftsdll - Win32 Release"
# Name "ftsdll - Win32 Debug"
# Begin Source File

SOURCE=..\fts\src\lang\mess\atomarray.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\runtime\files\atomfiles.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\datalib\atomlist.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\mess\atoms.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\runtime\audio\audio.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\runtime\audio\audioport.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\mess\autosave.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\runtime\devices\bytestream.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\veclib\cfft.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\mess\channel.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\mess\classes.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\runtime\client\client.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\mess\clipboard.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\mess\clipboard.h
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\veclib\complex.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\mess\connections.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\utils\cubic.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\datalib\datalib.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\runtime\devices\devconf.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\runtime\devices\devconf.h
# End Source File
# Begin Source File

SOURCE=..\fts\src\runtime\devices\devices.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\runtime\files\directories.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\mess\doctor.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\dsp\dsp.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\dsp\dspcontrol.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\dsp\dspgraph.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\mess\errobj.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\mess\expressions.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\runtime\files\files.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\utils\floatfuns.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\mess\fpe.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\mess\fpe.h
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\ftl\ftl.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\ftl\ftlmem.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\ftl\ftlutils.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\mess\ftsdata.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\mess\ftsdataid.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\dsp\gphiter.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\dsp\gphiter.h
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\utils\hashtable.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\runtime\client\incoming.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\mess\inout.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\mess\label.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\mess\loader.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\sys\mem.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\mess\mess.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\mess\messages.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\mess\messP.h
# End Source File
# Begin Source File

SOURCE=..\fts\src\tiles\messtile.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\tiles\messtile.h
# End Source File
# Begin Source File

SOURCE=..\fts\src\runtime\midi\midi.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\runtime\files\midifiles.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\runtime\midi\midiparser.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\runtime\midi\midiport.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\sys\modules.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\runtime\audio\nullaudioport.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\utils\objectlist.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\mess\objects.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\datalib\objectset.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\mess\objtable.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\runtime\client\outgoing.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\mess\params.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\runtime\files\parser.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\mess\patabs.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\mess\patcher.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\mess\patcherdata.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\mess\patlex.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\mess\patparser.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\runtime\files\post.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\mess\properties.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\runtime\client\protocol.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\utils\ramp_vectors.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\utils\ramps.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\utils\random.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\veclib\rfft_shuffle.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\mess\saver.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\runtime\sched\sched.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\mess\selection.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\dsp\sigbus.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\dsp\sigconn.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\dsp\sigconn.h
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\dsp\signal.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\sys\smem.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\sys\smem.h
# End Source File
# Begin Source File

SOURCE=..\fts\src\runtime\files\soundfiles.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\runtime\files\soundformats.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\utils\srconv.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\tiles\startup.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\sys\status.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\mess\symbols.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\runtime\client\sync.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\mess\templates.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\runtime\time\time.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\tiles\ucs.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\tiles\ucs.h
# End Source File
# Begin Source File

SOURCE=..\fts\src\runtime\client\updates.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\utils\utils.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\mess\variables.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\veclib\vec_fft.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\sys\version.c
# End Source File
# Begin Source File

SOURCE=..\fts\src\lang\mess\vm.c
# End Source File
# Begin Source File

SOURCE="..\fts\src\non-portable\win32.c"
# End Source File
# Begin Source File

SOURCE=..\fts\src\runtime\devices\windev.c
# End Source File
# End Target
# End Project
