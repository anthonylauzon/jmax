# Microsoft Developer Studio Project File - Name="sequence" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=sequence - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "sequence.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sequence.mak" CFG="sequence - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sequence - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "sequence - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "sequence - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\c\"
# PROP Intermediate_Dir ""
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SEQUENCE_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /Op /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SEQUENCE_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib fts.lib /nologo /dll /machine:I386 /libpath:"..\..\..\fts\lib"

!ELSEIF  "$(CFG)" == "sequence - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\c\"
# PROP Intermediate_Dir ""
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SEQUENCE_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SEQUENCE_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib fts.lib /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:"..\..\..\fts\lib"

!ENDIF 

# Begin Target

# Name "sequence - Win32 Release"
# Name "sequence - Win32 Debug"
# Begin Source File

SOURCE=..\c\src\event.c
# End Source File
# Begin Source File

SOURCE=..\c\src\event.h
# End Source File
# Begin Source File

SOURCE=..\c\src\getdur.c
# End Source File
# Begin Source File

SOURCE=..\c\src\locate.c
# End Source File
# Begin Source File

SOURCE=..\c\src\module.c
# End Source File
# Begin Source File

SOURCE=..\c\src\note.c
# End Source File
# Begin Source File

SOURCE=..\c\src\note.h
# End Source File
# Begin Source File

SOURCE=..\c\src\segment.c
# End Source File
# Begin Source File

SOURCE=..\c\src\segment.h
# End Source File
# Begin Source File

SOURCE=..\c\src\seqfind.c
# End Source File
# Begin Source File

SOURCE=..\c\src\seqmess.c
# End Source File
# Begin Source File

SOURCE=..\c\src\seqmidi.c
# End Source File
# Begin Source File

SOURCE=..\c\src\seqmidi.h
# End Source File
# Begin Source File

SOURCE=..\c\src\seqplay.c
# End Source File
# Begin Source File

SOURCE=..\c\src\seqrec.c
# End Source File
# Begin Source File

SOURCE=..\c\src\seqstep.c
# End Source File
# Begin Source File

SOURCE=..\c\src\seqsym.c
# End Source File
# Begin Source File

SOURCE=..\c\src\seqsym.h
# End Source File
# Begin Source File

SOURCE=..\c\src\sequence.c
# End Source File
# Begin Source File

SOURCE=..\c\src\sequence.h
# End Source File
# Begin Source File

SOURCE=..\c\src\track.c
# End Source File
# Begin Source File

SOURCE=..\c\src\track.h
# End Source File
# End Target
# End Project
