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
# PROP Output_Dir "..\fts\lib"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FTSDLL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FTSDLL_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib winmm.lib /nologo /dll /machine:I386 /out:"..\bin\fts.dll"

!ELSEIF  "$(CFG)" == "ftsdll - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\fts\lib"
# PROP Intermediate_Dir "Debug"
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
# ADD LINK32 ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /dll /debug /machine:I386 /out:"..\bin\fts.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "ftsdll - Win32 Release"
# Name "ftsdll - Win32 Debug"
# Begin Source File

SOURCE=..\fts\abstraction.c
# End Source File
# Begin Source File

SOURCE=..\fts\array.c
# End Source File
# Begin Source File

SOURCE=..\fts\atom.c
# End Source File
# Begin Source File

SOURCE=..\fts\atomfile.c
# End Source File
# Begin Source File

SOURCE=..\fts\audio.c
# End Source File
# Begin Source File

SOURCE=..\fts\autosave.c
# End Source File
# Begin Source File

SOURCE=..\fts\bytestream.c
# End Source File
# Begin Source File

SOURCE=..\fts\class.c
# End Source File
# Begin Source File

SOURCE=..\fts\client.c
# End Source File
# Begin Source File

SOURCE=..\fts\clipboard.c
# End Source File
# Begin Source File

SOURCE=..\fts\connection.c
# End Source File
# Begin Source File

SOURCE=..\fts\doctor.c
# End Source File
# Begin Source File

SOURCE=..\fts\dsp.c
# End Source File
# Begin Source File

SOURCE=..\fts\dspgraph.c
# End Source File
# Begin Source File

SOURCE=..\fts\errobj.c
# End Source File
# Begin Source File

SOURCE=..\fts\expression.c
# End Source File
# Begin Source File

SOURCE=..\fts\file.c
# End Source File
# Begin Source File

SOURCE=..\fts\fpe.c
# End Source File
# Begin Source File

SOURCE=..\fts\ftl.c
# End Source File
# Begin Source File

SOURCE=..\fts\ftlmem.c
# End Source File
# Begin Source File

SOURCE=..\fts\fts.c
# End Source File
# Begin Source File

SOURCE=..\fts\hashtable.c
# End Source File
# Begin Source File

SOURCE=..\fts\label.c
# End Source File
# Begin Source File

SOURCE=..\fts\list.c
# End Source File
# Begin Source File

SOURCE=..\fts\loader.c
# End Source File
# Begin Source File

SOURCE=..\fts\mem.c
# End Source File
# Begin Source File

SOURCE=..\fts\message.c
# End Source File
# Begin Source File

SOURCE=..\fts\midi.c
# End Source File
# Begin Source File

SOURCE=..\fts\midifile.c
# End Source File
# Begin Source File

SOURCE=..\fts\object.c
# End Source File
# Begin Source File

SOURCE=..\fts\objectlist.c
# End Source File
# Begin Source File

SOURCE=..\fts\objectset.c
# End Source File
# Begin Source File

SOURCE=..\fts\objtable.c
# End Source File
# Begin Source File

SOURCE=..\fts\OLDclient.c
# End Source File
# Begin Source File

SOURCE=..\fts\package.c
# End Source File
# Begin Source File

SOURCE=..\fts\param.c
# End Source File
# Begin Source File

SOURCE=..\fts\patcher.c
# End Source File
# Begin Source File

SOURCE=..\fts\patparser.c
# End Source File
# Begin Source File

SOURCE=..\fts\post.c
# End Source File
# Begin Source File

SOURCE=..\fts\project.c
# End Source File
# Begin Source File

SOURCE=..\fts\property.c
# End Source File
# Begin Source File

SOURCE=..\fts\saver.c
# End Source File
# Begin Source File

SOURCE=..\fts\sched.c
# End Source File
# Begin Source File

SOURCE=..\fts\selection.c
# End Source File
# Begin Source File

SOURCE=..\fts\sigconn.c
# End Source File
# Begin Source File

SOURCE=..\fts\soundfile.c
# End Source File
# Begin Source File

SOURCE=..\fts\soundfile_def.c
# End Source File
# Begin Source File

SOURCE=..\fts\srconv.c
# End Source File
# Begin Source File

SOURCE=..\fts\stack.c
# End Source File
# Begin Source File

SOURCE=..\fts\symbol.c
# End Source File
# Begin Source File

SOURCE=..\fts\template.c
# End Source File
# Begin Source File

SOURCE=..\fts\time.c
# End Source File
# Begin Source File

SOURCE=..\fts\variable.c
# End Source File
# Begin Source File

SOURCE=..\fts\version.c
# End Source File
# Begin Source File

SOURCE=..\fts\vm.c
# End Source File
# Begin Source File

SOURCE=..\fts\win32.c
# End Source File
# End Target
# End Project
