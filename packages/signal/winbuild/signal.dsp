# Microsoft Developer Studio Project File - Name="signal" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=signal - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "signal.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "signal.mak" CFG="signal - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "signal - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "signal - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "signal - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\c\"
# PROP Intermediate_Dir ""
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SIGNAL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\..\include" /I "..\..\data\c\include" /I "..\..\utils\c\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SIGNAL_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib fts.lib data.lib utils.lib /nologo /dll /machine:I386 /libpath:"..\..\..\fts\lib" /libpath:"..\..\data\c" /libpath:"..\..\utils\c"

!ELSEIF  "$(CFG)" == "signal - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SIGNAL_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\data\c\include" /I "..\..\utils\c\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SIGNAL_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib fts.lib data.lib utils.lib /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:"..\..\..\fts\lib" /libpath:"..\..\data\c" /libpath:"..\..\utils\c"

!ENDIF 

# Begin Target

# Name "signal - Win32 Release"
# Name "signal - Win32 Debug"
# Begin Source File

SOURCE=..\c\src\abs.c
# End Source File
# Begin Source File

SOURCE=..\c\src\binop.c
# End Source File
# Begin Source File

SOURCE=..\c\src\biquad.c
# End Source File
# Begin Source File

SOURCE=..\c\src\biquad.h
# End Source File
# Begin Source File

SOURCE=..\c\src\bus.c
# End Source File
# Begin Source File

SOURCE=..\c\src\clip.c
# End Source File
# Begin Source File

SOURCE=..\c\src\delay.c
# End Source File
# Begin Source File

SOURCE=..\c\src\delay.h
# End Source File
# Begin Source File

SOURCE=..\c\src\env.c
# End Source File
# Begin Source File

SOURCE=..\c\src\fir.c
# End Source File
# Begin Source File

SOURCE=..\c\src\fir.h
# End Source File
# Begin Source File

SOURCE=..\c\src\harmtap.c
# End Source File
# Begin Source File

SOURCE=..\c\src\iir.c
# End Source File
# Begin Source File

SOURCE=..\c\src\iir.h
# End Source File
# Begin Source File

SOURCE=..\c\src\matrix.c
# End Source File
# Begin Source File

SOURCE=..\c\src\osc.c
# End Source File
# Begin Source File

SOURCE=..\c\src\osc.h
# End Source File
# Begin Source File

SOURCE=..\c\src\osc_ieeewrap.c
# End Source File
# Begin Source File

SOURCE=..\c\src\paste.c
# End Source File
# Begin Source File

SOURCE=..\c\src\pick.c
# End Source File
# Begin Source File

SOURCE=..\c\src\pink.c
# End Source File
# Begin Source File

SOURCE=..\c\src\playfvec.c
# End Source File
# Begin Source File

SOURCE=..\c\src\recfvec.c
# End Source File
# Begin Source File

SOURCE=..\c\src\samphold.c
# End Source File
# Begin Source File

SOURCE=..\c\src\samplepool.c
# End Source File
# Begin Source File

SOURCE=..\c\src\samplepool.h
# End Source File
# Begin Source File

SOURCE=..\c\src\signal.c
# End Source File
# Begin Source File

SOURCE=..\c\src\signal.h
# End Source File
# Begin Source File

SOURCE=..\c\src\snapshot.c
# End Source File
# Begin Source File

SOURCE=..\c\src\sogs.c
# End Source File
# Begin Source File

SOURCE=..\c\src\threshold.c
# End Source File
# Begin Source File

SOURCE=..\c\src\tilda.c
# End Source File
# Begin Source File

SOURCE=..\c\src\wahwah.c
# End Source File
# Begin Source File

SOURCE=..\c\src\wave.c
# End Source File
# Begin Source File

SOURCE=..\c\src\wave.h
# End Source File
# Begin Source File

SOURCE=..\c\src\wave_ieeewrap.c
# End Source File
# Begin Source File

SOURCE=..\c\src\white.c
# End Source File
# Begin Source File

SOURCE=..\c\src\zerocross.c
# End Source File
# End Target
# End Project
