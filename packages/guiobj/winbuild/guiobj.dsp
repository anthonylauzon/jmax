# Microsoft Developer Studio Project File - Name="guiobj" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=guiobj - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "guiobj.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "guiobj.mak" CFG="guiobj - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "guiobj - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "guiobj - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "guiobj - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\c\"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GUIOBJ_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\..\include" /I "..\..\data\c\src" /I "..\..\mess\c\src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GUIOBJ_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib data.lib fts.lib /nologo /dll /machine:I386 /libpath:"..\..\data\c\lib" /libpath:"..\..\..\fts\lib"

!ELSEIF  "$(CFG)" == "guiobj - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\c\"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GUIOBJ_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\data\c\src" /I "..\..\mess\c\src" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GUIOBJ_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib data.lib fts.lib /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:"..\..\data\c\\" /libpath:"..\..\..\fts\lib"

!ENDIF 

# Begin Target

# Name "guiobj - Win32 Release"
# Name "guiobj - Win32 Debug"
# Begin Source File

SOURCE=..\c\src\button.c
# End Source File
# Begin Source File

SOURCE=..\c\src\comment.c
# End Source File
# Begin Source File

SOURCE=..\c\src\comment_doctor.c
# End Source File
# Begin Source File

SOURCE=..\c\src\display.c
# End Source File
# Begin Source File

SOURCE=..\c\src\dspcontrol.c
# End Source File
# Begin Source File

SOURCE=..\c\src\fork.c
# End Source File
# Begin Source File

SOURCE=..\c\src\gfloat.c
# End Source File
# Begin Source File

SOURCE=..\c\src\gint.c
# End Source File
# Begin Source File

SOURCE=..\c\src\guiobj.c
# End Source File
# Begin Source File

SOURCE=..\c\src\guiobj.h
# End Source File
# Begin Source File

SOURCE=..\c\src\key.c
# End Source File
# Begin Source File

SOURCE=..\c\src\keyserver.c
# End Source File
# Begin Source File

SOURCE=..\c\src\keyserver.h
# End Source File
# Begin Source File

SOURCE=..\c\src\messconst.c
# End Source File
# Begin Source File

SOURCE=..\c\src\panel.c
# End Source File
# Begin Source File

SOURCE=..\c\src\scope.c
# End Source File
# Begin Source File

SOURCE=..\c\src\slider.c
# End Source File
# Begin Source File

SOURCE=..\c\src\toggle.c
# End Source File
# Begin Source File

SOURCE=..\c\src\vecdisplay.c
# End Source File
# End Target
# End Project
