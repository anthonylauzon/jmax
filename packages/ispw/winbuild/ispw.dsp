# Microsoft Developer Studio Project File - Name="ispw" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ispw - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ispw.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ispw.mak" CFG="ispw - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ispw - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ispw - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ispw - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ISPW_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\..\include" /I "..\..\data\c\src" /I "..\..\utils\c\src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ISPW_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib fts.lib data.lib utils.lib /nologo /dll /machine:I386 /libpath:"..\..\..\fts\lib" /libpath:"..\..\data\c" /libpath:"..\..\utils\c"

!ELSEIF  "$(CFG)" == "ispw - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ispw___Win32_Debug"
# PROP BASE Intermediate_Dir "ispw___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\c\"
# PROP Intermediate_Dir ""
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ISPW_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\data\c\src" /I "..\..\utils\c\src" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ISPW_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib fts.lib data.lib utils.lib /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:"..\..\..\fts\lib" /libpath:"..\..\data\c\\" /libpath:"..\..\utils\c\\"

!ENDIF 

# Begin Target

# Name "ispw - Win32 Release"
# Name "ispw - Win32 Debug"
# Begin Source File

SOURCE=..\c\src\abs.c
# End Source File
# Begin Source File

SOURCE=..\c\src\accum.c
# End Source File
# Begin Source File

SOURCE=..\c\src\adc_tilda.c
# End Source File
# Begin Source File

SOURCE=..\c\src\atomlist.c
# End Source File
# Begin Source File

SOURCE=..\c\src\atomlist.h
# End Source File
# Begin Source File

SOURCE=..\c\src\bag.c
# End Source File
# Begin Source File

SOURCE=..\c\src\bangbang.c
# End Source File
# Begin Source File

SOURCE=..\c\src\biquad.c
# End Source File
# Begin Source File

SOURCE=..\c\src\biquad.h
# End Source File
# Begin Source File

SOURCE=..\c\src\cfbpass2.c
# End Source File
# Begin Source File

SOURCE=..\c\src\change.c
# End Source File
# Begin Source File

SOURCE=..\c\src\clip.c
# End Source File
# Begin Source File

SOURCE=..\c\src\coef_h1.c
# End Source File
# Begin Source File

SOURCE=..\c\src\dac_tilda.c
# End Source File
# Begin Source File

SOURCE=..\c\src\delay.c
# End Source File
# Begin Source File

SOURCE=..\c\src\delbuf.c
# End Source File
# Begin Source File

SOURCE=..\c\src\delbuf.h
# End Source File
# Begin Source File

SOURCE=..\c\src\delread.c
# End Source File
# Begin Source File

SOURCE=..\c\src\deltable.c
# End Source File
# Begin Source File

SOURCE=..\c\src\deltable.h
# End Source File
# Begin Source File

SOURCE=..\c\src\delwrite.c
# End Source File
# Begin Source File

SOURCE=..\c\src\drunk.c
# End Source File
# Begin Source File

SOURCE=..\c\src\expr_doctor.c
# End Source File
# Begin Source File

SOURCE=..\c\src\exscale.c
# End Source File
# Begin Source File

SOURCE=..\c\src\fft.c
# End Source File
# Begin Source File

SOURCE=..\c\src\fft_ftl.c
# End Source File
# Begin Source File

SOURCE=..\c\src\fft_ftl.h
# End Source File
# Begin Source File

SOURCE=..\c\src\filters.c
# End Source File
# Begin Source File

SOURCE=..\c\src\filters.h
# End Source File
# Begin Source File

SOURCE=..\c\src\fir.h
# End Source File
# Begin Source File

SOURCE=..\c\src\float.c
# End Source File
# Begin Source File

SOURCE=..\c\src\fourpoint.c
# End Source File
# Begin Source File

SOURCE=..\c\src\fourpoint.h
# End Source File
# Begin Source File

SOURCE=..\c\src\ftl_delay.c
# End Source File
# Begin Source File

SOURCE=..\c\src\ftl_fir.c
# End Source File
# Begin Source File

SOURCE=..\c\src\ftl_iir.c
# End Source File
# Begin Source File

SOURCE=..\c\src\ftl_samprw.c
# End Source File
# Begin Source File

SOURCE=..\c\src\ftom.c
# End Source File
# Begin Source File

SOURCE=..\c\src\funbuff.c
# End Source File
# Begin Source File

SOURCE=..\c\src\gate.c
# End Source File
# Begin Source File

SOURCE=..\c\src\iir.c
# End Source File
# Begin Source File

SOURCE=..\c\src\iir.h
# End Source File
# Begin Source File

SOURCE=..\c\src\integer.c
# End Source File
# Begin Source File

SOURCE=..\c\src\ispw.c
# End Source File
# Begin Source File

SOURCE=..\c\src\ispwcomp.c
# End Source File
# Begin Source File

SOURCE=..\c\src\line.c
# End Source File
# Begin Source File

SOURCE=..\c\src\loadbang.c
# End Source File
# Begin Source File

SOURCE=..\c\src\logscale.c
# End Source File
# Begin Source File

SOURCE=..\c\src\makenote.c
# End Source File
# Begin Source File

SOURCE=..\c\src\message.c
# End Source File
# Begin Source File

SOURCE=..\c\src\message_doctor.c
# End Source File
# Begin Source File

SOURCE=..\c\src\metro.c
# End Source File
# Begin Source File

SOURCE=..\c\src\midicd.h
# End Source File
# Begin Source File

SOURCE=..\c\src\mididecrypt.c
# End Source File
# Begin Source File

SOURCE=..\c\src\mtof.c
# End Source File
# Begin Source File

SOURCE=..\c\src\naming.c
# End Source File
# Begin Source File

SOURCE=..\c\src\naming.h
# End Source File
# Begin Source File

SOURCE=..\c\src\nbangs.c
# End Source File
# Begin Source File

SOURCE=..\c\src\noise.c
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

SOURCE=..\c\src\pack.c
# End Source File
# Begin Source File

SOURCE=..\c\src\pbank.c
# End Source File
# Begin Source File

SOURCE=..\c\src\phasor.c
# End Source File
# Begin Source File

SOURCE=..\c\src\phasor.h
# End Source File
# Begin Source File

SOURCE=..\c\src\phasor_ieeewrap.c
# End Source File
# Begin Source File

SOURCE=..\c\src\pipe.c
# End Source File
# Begin Source File

SOURCE=..\c\src\pitch.c
# End Source File
# Begin Source File

SOURCE=..\c\src\poly.c
# End Source File
# Begin Source File

SOURCE=..\c\src\prepend.c
# End Source File
# Begin Source File

SOURCE=..\c\src\pt.c
# End Source File
# Begin Source File

SOURCE=..\c\src\pt_meth.c
# End Source File
# Begin Source File

SOURCE=..\c\src\pt_meth.h
# End Source File
# Begin Source File

SOURCE=..\c\src\pt_obj.h
# End Source File
# Begin Source File

SOURCE=..\c\src\pt_tools.c
# End Source File
# Begin Source File

SOURCE=..\c\src\pt_tools.h
# End Source File
# Begin Source File

SOURCE=..\c\src\random.c
# End Source File
# Begin Source File

SOURCE=..\c\src\resample.c
# End Source File
# Begin Source File

SOURCE=..\c\src\route.c
# End Source File
# Begin Source File

SOURCE=..\c\src\Rreceive.c
# End Source File
# Begin Source File

SOURCE=..\c\src\Rsend.c
# End Source File
# Begin Source File

SOURCE=..\c\src\Rsend.h
# End Source File
# Begin Source File

SOURCE=..\c\src\sampbuf.c
# End Source File
# Begin Source File

SOURCE=..\c\src\sampbuf.h
# End Source File
# Begin Source File

SOURCE=..\c\src\samphold.c
# End Source File
# Begin Source File

SOURCE=..\c\src\samppeek.c
# End Source File
# Begin Source File

SOURCE=..\c\src\sampread.c
# End Source File
# Begin Source File

SOURCE=..\c\src\sampread.h
# End Source File
# Begin Source File

SOURCE=..\c\src\sampunit.c
# End Source File
# Begin Source File

SOURCE=..\c\src\sampunit.h
# End Source File
# Begin Source File

SOURCE=..\c\src\sampwrite.c
# End Source File
# Begin Source File

SOURCE=..\c\src\sampwrite.h
# End Source File
# Begin Source File

SOURCE=..\c\src\scale.c
# End Source File
# Begin Source File

SOURCE=..\c\src\select.c
# End Source File
# Begin Source File

SOURCE=..\c\src\sig1.c
# End Source File
# Begin Source File

SOURCE=..\c\src\sigabs.c
# End Source File
# Begin Source File

SOURCE=..\c\src\sigapass3.c
# End Source File
# Begin Source File

SOURCE=..\c\src\sigarctan2.c
# End Source File
# Begin Source File

SOURCE=..\c\src\sigline.c
# End Source File
# Begin Source File

SOURCE=..\c\src\sigparam.c
# End Source File
# Begin Source File

SOURCE=..\c\src\sigprint.c
# End Source File
# Begin Source File

SOURCE=..\c\src\sigsnap.c
# End Source File
# Begin Source File

SOURCE=..\c\src\sigswitch.c
# End Source File
# Begin Source File

SOURCE=..\c\src\sigtable.c
# End Source File
# Begin Source File

SOURCE=..\c\src\sigthres.c
# End Source File
# Begin Source File

SOURCE=..\c\src\sigthrow.c
# End Source File
# Begin Source File

SOURCE=..\c\src\sigthru.c
# End Source File
# Begin Source File

SOURCE=..\c\src\speedlim.c
# End Source File
# Begin Source File

SOURCE=..\c\src\split.c
# End Source File
# Begin Source File

SOURCE=..\c\src\stripnote.c
# End Source File
# Begin Source File

SOURCE=..\c\src\sustain.c
# End Source File
# Begin Source File

SOURCE=..\c\src\symbol.c
# End Source File
# Begin Source File

SOURCE=..\c\src\tabcycle.c
# End Source File
# Begin Source File

SOURCE=..\c\src\table.c
# End Source File
# Begin Source File

SOURCE=..\c\src\tabpeek.c
# End Source File
# Begin Source File

SOURCE=..\c\src\tabpoke.c
# End Source File
# Begin Source File

SOURCE=..\c\src\time.c
# End Source File
# Begin Source File

SOURCE=..\c\src\timer.c
# End Source File
# Begin Source File

SOURCE=..\c\src\trigger.c
# End Source File
# Begin Source File

SOURCE=..\c\src\unpack.c
# End Source File
# Begin Source File

SOURCE=..\c\src\value.c
# End Source File
# Begin Source File

SOURCE=..\c\src\vd.c
# End Source File
# Begin Source File

SOURCE=..\c\src\vd.h
# End Source File
# Begin Source File

SOURCE=..\c\src\vecclip.c
# End Source File
# Begin Source File

SOURCE=..\c\src\veclog.c
# End Source File
# Begin Source File

SOURCE=..\c\src\vecsqrt.c
# End Source File
# Begin Source File

SOURCE=..\c\src\vectrigon.c
# End Source File
# Begin Source File

SOURCE=..\c\src\vexp.c
# End Source File
# Begin Source File

SOURCE=..\c\src\vexp.h
# End Source File
# Begin Source File

SOURCE=..\c\src\vexp_fun.c
# End Source File
# Begin Source File

SOURCE=..\c\src\vexp_if.c
# End Source File
# Begin Source File

SOURCE=..\c\src\vexp_util.c
# End Source File
# Begin Source File

SOURCE=..\c\src\vexp_util.h
# End Source File
# Begin Source File

SOURCE=..\c\src\voxalloc.c
# End Source File
# Begin Source File

SOURCE=..\c\src\wahwah.c
# End Source File
# Begin Source File

SOURCE=..\c\src\wavetab.c
# End Source File
# Begin Source File

SOURCE=..\c\src\wavetab.h
# End Source File
# Begin Source File

SOURCE=..\c\src\zerocross.c
# End Source File
# End Target
# End Project
