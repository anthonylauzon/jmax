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

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ftsdll - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\ftsdll.dll"


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
	-@erase "$(INTDIR)\ftlfun.obj"
	-@erase "$(INTDIR)\ftlmem.obj"
	-@erase "$(INTDIR)\ftlutils.obj"
	-@erase "$(INTDIR)\ftsdata.obj"
	-@erase "$(INTDIR)\ftsdataid.obj"
	-@erase "$(INTDIR)\gphiter.obj"
	-@erase "$(INTDIR)\hashtab.obj"
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
	-@erase "$(INTDIR)\vec_bit.obj"
	-@erase "$(INTDIR)\vec_carith.obj"
	-@erase "$(INTDIR)\vec_cfmul.obj"
	-@erase "$(INTDIR)\vec_cmerge.obj"
	-@erase "$(INTDIR)\vec_cplx.obj"
	-@erase "$(INTDIR)\vec_cpy.obj"
	-@erase "$(INTDIR)\vec_csplit.obj"
	-@erase "$(INTDIR)\vec_csum.obj"
	-@erase "$(INTDIR)\vec_explog.obj"
	-@erase "$(INTDIR)\vec_fabs.obj"
	-@erase "$(INTDIR)\vec_farith.obj"
	-@erase "$(INTDIR)\vec_fclip.obj"
	-@erase "$(INTDIR)\vec_fcmp.obj"
	-@erase "$(INTDIR)\vec_fdot.obj"
	-@erase "$(INTDIR)\vec_fft.obj"
	-@erase "$(INTDIR)\vec_fmax.obj"
	-@erase "$(INTDIR)\vec_fmin.obj"
	-@erase "$(INTDIR)\vec_fsum.obj"
	-@erase "$(INTDIR)\vec_iabs.obj"
	-@erase "$(INTDIR)\vec_iarith.obj"
	-@erase "$(INTDIR)\vec_iclip.obj"
	-@erase "$(INTDIR)\vec_icmp.obj"
	-@erase "$(INTDIR)\vec_imax.obj"
	-@erase "$(INTDIR)\vec_imin.obj"
	-@erase "$(INTDIR)\vec_isum.obj"
	-@erase "$(INTDIR)\vec_round.obj"
	-@erase "$(INTDIR)\vec_rsqrt.obj"
	-@erase "$(INTDIR)\vec_scl_bit.obj"
	-@erase "$(INTDIR)\vec_scl_carith.obj"
	-@erase "$(INTDIR)\vec_scl_cfmul.obj"
	-@erase "$(INTDIR)\vec_scl_farith.obj"
	-@erase "$(INTDIR)\vec_scl_fcmp.obj"
	-@erase "$(INTDIR)\vec_scl_iarith.obj"
	-@erase "$(INTDIR)\vec_scl_icmp.obj"
	-@erase "$(INTDIR)\vec_sqrt.obj"
	-@erase "$(INTDIR)\vec_trigon.obj"
	-@erase "$(INTDIR)\vecmod.obj"
	-@erase "$(INTDIR)\vecstat.obj"
	-@erase "$(INTDIR)\vecx_bit.obj"
	-@erase "$(INTDIR)\vecx_carith.obj"
	-@erase "$(INTDIR)\vecx_cfmul.obj"
	-@erase "$(INTDIR)\vecx_cmerge.obj"
	-@erase "$(INTDIR)\vecx_cplx.obj"
	-@erase "$(INTDIR)\vecx_cpy.obj"
	-@erase "$(INTDIR)\vecx_csplit.obj"
	-@erase "$(INTDIR)\vecx_csum.obj"
	-@erase "$(INTDIR)\vecx_explog.obj"
	-@erase "$(INTDIR)\vecx_fabs.obj"
	-@erase "$(INTDIR)\vecx_farith.obj"
	-@erase "$(INTDIR)\vecx_fclip.obj"
	-@erase "$(INTDIR)\vecx_fcmp.obj"
	-@erase "$(INTDIR)\vecx_fdot.obj"
	-@erase "$(INTDIR)\vecx_fmax.obj"
	-@erase "$(INTDIR)\vecx_fmin.obj"
	-@erase "$(INTDIR)\vecx_fsum.obj"
	-@erase "$(INTDIR)\vecx_iabs.obj"
	-@erase "$(INTDIR)\vecx_iarith.obj"
	-@erase "$(INTDIR)\vecx_iclip.obj"
	-@erase "$(INTDIR)\vecx_icmp.obj"
	-@erase "$(INTDIR)\vecx_imax.obj"
	-@erase "$(INTDIR)\vecx_imin.obj"
	-@erase "$(INTDIR)\vecx_isum.obj"
	-@erase "$(INTDIR)\vecx_round.obj"
	-@erase "$(INTDIR)\vecx_rsqrt.obj"
	-@erase "$(INTDIR)\vecx_scl_bit.obj"
	-@erase "$(INTDIR)\vecx_scl_carith.obj"
	-@erase "$(INTDIR)\vecx_scl_cfmul.obj"
	-@erase "$(INTDIR)\vecx_scl_farith.obj"
	-@erase "$(INTDIR)\vecx_scl_fcmp.obj"
	-@erase "$(INTDIR)\vecx_scl_iarith.obj"
	-@erase "$(INTDIR)\vecx_scl_icmp.obj"
	-@erase "$(INTDIR)\vecx_sqrt.obj"
	-@erase "$(INTDIR)\vecx_trigon.obj"
	-@erase "$(INTDIR)\version.obj"
	-@erase "$(INTDIR)\vm.obj"
	-@erase "$(INTDIR)\win32.obj"
	-@erase "$(OUTDIR)\ftsdll.dll"
	-@erase "$(OUTDIR)\ftsdll.exp"
	-@erase "$(OUTDIR)\ftsdll.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FTSDLL_EXPORTS" /Fp"$(INTDIR)\ftsdll.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ftsdll.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\ftsdll.pdb" /machine:I386 /out:"$(OUTDIR)\ftsdll.dll" /implib:"$(OUTDIR)\ftsdll.lib" 
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
	"$(INTDIR)\ftlfun.obj" \
	"$(INTDIR)\ftlmem.obj" \
	"$(INTDIR)\ftlutils.obj" \
	"$(INTDIR)\ftsdata.obj" \
	"$(INTDIR)\ftsdataid.obj" \
	"$(INTDIR)\gphiter.obj" \
	"$(INTDIR)\hashtab.obj" \
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
	"$(INTDIR)\status.obj" \
	"$(INTDIR)\symbols.obj" \
	"$(INTDIR)\sync.obj" \
	"$(INTDIR)\templates.obj" \
	"$(INTDIR)\time.obj" \
	"$(INTDIR)\ucs.obj" \
	"$(INTDIR)\updates.obj" \
	"$(INTDIR)\utils.obj" \
	"$(INTDIR)\variables.obj" \
	"$(INTDIR)\vec_bit.obj" \
	"$(INTDIR)\vec_carith.obj" \
	"$(INTDIR)\vec_cfmul.obj" \
	"$(INTDIR)\vec_cmerge.obj" \
	"$(INTDIR)\vec_cplx.obj" \
	"$(INTDIR)\vec_cpy.obj" \
	"$(INTDIR)\vec_csplit.obj" \
	"$(INTDIR)\vec_csum.obj" \
	"$(INTDIR)\vec_explog.obj" \
	"$(INTDIR)\vec_fabs.obj" \
	"$(INTDIR)\vec_farith.obj" \
	"$(INTDIR)\vec_fclip.obj" \
	"$(INTDIR)\vec_fcmp.obj" \
	"$(INTDIR)\vec_fdot.obj" \
	"$(INTDIR)\vec_fft.obj" \
	"$(INTDIR)\vec_fmax.obj" \
	"$(INTDIR)\vec_fmin.obj" \
	"$(INTDIR)\vec_fsum.obj" \
	"$(INTDIR)\vec_iabs.obj" \
	"$(INTDIR)\vec_iarith.obj" \
	"$(INTDIR)\vec_iclip.obj" \
	"$(INTDIR)\vec_icmp.obj" \
	"$(INTDIR)\vec_imax.obj" \
	"$(INTDIR)\vec_imin.obj" \
	"$(INTDIR)\vec_isum.obj" \
	"$(INTDIR)\vec_round.obj" \
	"$(INTDIR)\vec_rsqrt.obj" \
	"$(INTDIR)\vec_scl_bit.obj" \
	"$(INTDIR)\vec_scl_carith.obj" \
	"$(INTDIR)\vec_scl_cfmul.obj" \
	"$(INTDIR)\vec_scl_farith.obj" \
	"$(INTDIR)\vec_scl_fcmp.obj" \
	"$(INTDIR)\vec_scl_iarith.obj" \
	"$(INTDIR)\vec_scl_icmp.obj" \
	"$(INTDIR)\vec_sqrt.obj" \
	"$(INTDIR)\vec_trigon.obj" \
	"$(INTDIR)\vecmod.obj" \
	"$(INTDIR)\vecstat.obj" \
	"$(INTDIR)\vecx_bit.obj" \
	"$(INTDIR)\vecx_carith.obj" \
	"$(INTDIR)\vecx_cfmul.obj" \
	"$(INTDIR)\vecx_cmerge.obj" \
	"$(INTDIR)\vecx_cplx.obj" \
	"$(INTDIR)\vecx_cpy.obj" \
	"$(INTDIR)\vecx_csplit.obj" \
	"$(INTDIR)\vecx_csum.obj" \
	"$(INTDIR)\vecx_explog.obj" \
	"$(INTDIR)\vecx_fabs.obj" \
	"$(INTDIR)\vecx_farith.obj" \
	"$(INTDIR)\vecx_fclip.obj" \
	"$(INTDIR)\vecx_fcmp.obj" \
	"$(INTDIR)\vecx_fdot.obj" \
	"$(INTDIR)\vecx_fmax.obj" \
	"$(INTDIR)\vecx_fmin.obj" \
	"$(INTDIR)\vecx_fsum.obj" \
	"$(INTDIR)\vecx_iabs.obj" \
	"$(INTDIR)\vecx_iarith.obj" \
	"$(INTDIR)\vecx_iclip.obj" \
	"$(INTDIR)\vecx_icmp.obj" \
	"$(INTDIR)\vecx_imax.obj" \
	"$(INTDIR)\vecx_imin.obj" \
	"$(INTDIR)\vecx_isum.obj" \
	"$(INTDIR)\vecx_round.obj" \
	"$(INTDIR)\vecx_rsqrt.obj" \
	"$(INTDIR)\vecx_scl_bit.obj" \
	"$(INTDIR)\vecx_scl_carith.obj" \
	"$(INTDIR)\vecx_scl_cfmul.obj" \
	"$(INTDIR)\vecx_scl_farith.obj" \
	"$(INTDIR)\vecx_scl_fcmp.obj" \
	"$(INTDIR)\vecx_scl_iarith.obj" \
	"$(INTDIR)\vecx_scl_icmp.obj" \
	"$(INTDIR)\vecx_sqrt.obj" \
	"$(INTDIR)\vecx_trigon.obj" \
	"$(INTDIR)\version.obj" \
	"$(INTDIR)\vm.obj" \
	"$(INTDIR)\win32.obj" \
	"$(INTDIR)\cubic.obj"

"$(OUTDIR)\ftsdll.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ftsdll - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\ftsdll.dll"


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
	-@erase "$(INTDIR)\ftlfun.obj"
	-@erase "$(INTDIR)\ftlmem.obj"
	-@erase "$(INTDIR)\ftlutils.obj"
	-@erase "$(INTDIR)\ftsdata.obj"
	-@erase "$(INTDIR)\ftsdataid.obj"
	-@erase "$(INTDIR)\gphiter.obj"
	-@erase "$(INTDIR)\hashtab.obj"
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
	-@erase "$(INTDIR)\vec_bit.obj"
	-@erase "$(INTDIR)\vec_carith.obj"
	-@erase "$(INTDIR)\vec_cfmul.obj"
	-@erase "$(INTDIR)\vec_cmerge.obj"
	-@erase "$(INTDIR)\vec_cplx.obj"
	-@erase "$(INTDIR)\vec_cpy.obj"
	-@erase "$(INTDIR)\vec_csplit.obj"
	-@erase "$(INTDIR)\vec_csum.obj"
	-@erase "$(INTDIR)\vec_explog.obj"
	-@erase "$(INTDIR)\vec_fabs.obj"
	-@erase "$(INTDIR)\vec_farith.obj"
	-@erase "$(INTDIR)\vec_fclip.obj"
	-@erase "$(INTDIR)\vec_fcmp.obj"
	-@erase "$(INTDIR)\vec_fdot.obj"
	-@erase "$(INTDIR)\vec_fft.obj"
	-@erase "$(INTDIR)\vec_fmax.obj"
	-@erase "$(INTDIR)\vec_fmin.obj"
	-@erase "$(INTDIR)\vec_fsum.obj"
	-@erase "$(INTDIR)\vec_iabs.obj"
	-@erase "$(INTDIR)\vec_iarith.obj"
	-@erase "$(INTDIR)\vec_iclip.obj"
	-@erase "$(INTDIR)\vec_icmp.obj"
	-@erase "$(INTDIR)\vec_imax.obj"
	-@erase "$(INTDIR)\vec_imin.obj"
	-@erase "$(INTDIR)\vec_isum.obj"
	-@erase "$(INTDIR)\vec_round.obj"
	-@erase "$(INTDIR)\vec_rsqrt.obj"
	-@erase "$(INTDIR)\vec_scl_bit.obj"
	-@erase "$(INTDIR)\vec_scl_carith.obj"
	-@erase "$(INTDIR)\vec_scl_cfmul.obj"
	-@erase "$(INTDIR)\vec_scl_farith.obj"
	-@erase "$(INTDIR)\vec_scl_fcmp.obj"
	-@erase "$(INTDIR)\vec_scl_iarith.obj"
	-@erase "$(INTDIR)\vec_scl_icmp.obj"
	-@erase "$(INTDIR)\vec_sqrt.obj"
	-@erase "$(INTDIR)\vec_trigon.obj"
	-@erase "$(INTDIR)\vecmod.obj"
	-@erase "$(INTDIR)\vecstat.obj"
	-@erase "$(INTDIR)\vecx_bit.obj"
	-@erase "$(INTDIR)\vecx_carith.obj"
	-@erase "$(INTDIR)\vecx_cfmul.obj"
	-@erase "$(INTDIR)\vecx_cmerge.obj"
	-@erase "$(INTDIR)\vecx_cplx.obj"
	-@erase "$(INTDIR)\vecx_cpy.obj"
	-@erase "$(INTDIR)\vecx_csplit.obj"
	-@erase "$(INTDIR)\vecx_csum.obj"
	-@erase "$(INTDIR)\vecx_explog.obj"
	-@erase "$(INTDIR)\vecx_fabs.obj"
	-@erase "$(INTDIR)\vecx_farith.obj"
	-@erase "$(INTDIR)\vecx_fclip.obj"
	-@erase "$(INTDIR)\vecx_fcmp.obj"
	-@erase "$(INTDIR)\vecx_fdot.obj"
	-@erase "$(INTDIR)\vecx_fmax.obj"
	-@erase "$(INTDIR)\vecx_fmin.obj"
	-@erase "$(INTDIR)\vecx_fsum.obj"
	-@erase "$(INTDIR)\vecx_iabs.obj"
	-@erase "$(INTDIR)\vecx_iarith.obj"
	-@erase "$(INTDIR)\vecx_iclip.obj"
	-@erase "$(INTDIR)\vecx_icmp.obj"
	-@erase "$(INTDIR)\vecx_imax.obj"
	-@erase "$(INTDIR)\vecx_imin.obj"
	-@erase "$(INTDIR)\vecx_isum.obj"
	-@erase "$(INTDIR)\vecx_round.obj"
	-@erase "$(INTDIR)\vecx_rsqrt.obj"
	-@erase "$(INTDIR)\vecx_scl_bit.obj"
	-@erase "$(INTDIR)\vecx_scl_carith.obj"
	-@erase "$(INTDIR)\vecx_scl_cfmul.obj"
	-@erase "$(INTDIR)\vecx_scl_farith.obj"
	-@erase "$(INTDIR)\vecx_scl_fcmp.obj"
	-@erase "$(INTDIR)\vecx_scl_iarith.obj"
	-@erase "$(INTDIR)\vecx_scl_icmp.obj"
	-@erase "$(INTDIR)\vecx_sqrt.obj"
	-@erase "$(INTDIR)\vecx_trigon.obj"
	-@erase "$(INTDIR)\version.obj"
	-@erase "$(INTDIR)\vm.obj"
	-@erase "$(INTDIR)\win32.obj"
	-@erase "$(OUTDIR)\ftsdll.dll"
	-@erase "$(OUTDIR)\ftsdll.exp"
	-@erase "$(OUTDIR)\ftsdll.ilk"
	-@erase "$(OUTDIR)\ftsdll.lib"
	-@erase "$(OUTDIR)\ftsdll.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FTSDLL_EXPORTS" /Fp"$(INTDIR)\ftsdll.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ftsdll.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\ftsdll.pdb" /debug /machine:I386 /out:"$(OUTDIR)\ftsdll.dll" /implib:"$(OUTDIR)\ftsdll.lib" /pdbtype:sept 
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
	"$(INTDIR)\ftlfun.obj" \
	"$(INTDIR)\ftlmem.obj" \
	"$(INTDIR)\ftlutils.obj" \
	"$(INTDIR)\ftsdata.obj" \
	"$(INTDIR)\ftsdataid.obj" \
	"$(INTDIR)\gphiter.obj" \
	"$(INTDIR)\hashtab.obj" \
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
	"$(INTDIR)\status.obj" \
	"$(INTDIR)\symbols.obj" \
	"$(INTDIR)\sync.obj" \
	"$(INTDIR)\templates.obj" \
	"$(INTDIR)\time.obj" \
	"$(INTDIR)\ucs.obj" \
	"$(INTDIR)\updates.obj" \
	"$(INTDIR)\utils.obj" \
	"$(INTDIR)\variables.obj" \
	"$(INTDIR)\vec_bit.obj" \
	"$(INTDIR)\vec_carith.obj" \
	"$(INTDIR)\vec_cfmul.obj" \
	"$(INTDIR)\vec_cmerge.obj" \
	"$(INTDIR)\vec_cplx.obj" \
	"$(INTDIR)\vec_cpy.obj" \
	"$(INTDIR)\vec_csplit.obj" \
	"$(INTDIR)\vec_csum.obj" \
	"$(INTDIR)\vec_explog.obj" \
	"$(INTDIR)\vec_fabs.obj" \
	"$(INTDIR)\vec_farith.obj" \
	"$(INTDIR)\vec_fclip.obj" \
	"$(INTDIR)\vec_fcmp.obj" \
	"$(INTDIR)\vec_fdot.obj" \
	"$(INTDIR)\vec_fft.obj" \
	"$(INTDIR)\vec_fmax.obj" \
	"$(INTDIR)\vec_fmin.obj" \
	"$(INTDIR)\vec_fsum.obj" \
	"$(INTDIR)\vec_iabs.obj" \
	"$(INTDIR)\vec_iarith.obj" \
	"$(INTDIR)\vec_iclip.obj" \
	"$(INTDIR)\vec_icmp.obj" \
	"$(INTDIR)\vec_imax.obj" \
	"$(INTDIR)\vec_imin.obj" \
	"$(INTDIR)\vec_isum.obj" \
	"$(INTDIR)\vec_round.obj" \
	"$(INTDIR)\vec_rsqrt.obj" \
	"$(INTDIR)\vec_scl_bit.obj" \
	"$(INTDIR)\vec_scl_carith.obj" \
	"$(INTDIR)\vec_scl_cfmul.obj" \
	"$(INTDIR)\vec_scl_farith.obj" \
	"$(INTDIR)\vec_scl_fcmp.obj" \
	"$(INTDIR)\vec_scl_iarith.obj" \
	"$(INTDIR)\vec_scl_icmp.obj" \
	"$(INTDIR)\vec_sqrt.obj" \
	"$(INTDIR)\vec_trigon.obj" \
	"$(INTDIR)\vecmod.obj" \
	"$(INTDIR)\vecstat.obj" \
	"$(INTDIR)\vecx_bit.obj" \
	"$(INTDIR)\vecx_carith.obj" \
	"$(INTDIR)\vecx_cfmul.obj" \
	"$(INTDIR)\vecx_cmerge.obj" \
	"$(INTDIR)\vecx_cplx.obj" \
	"$(INTDIR)\vecx_cpy.obj" \
	"$(INTDIR)\vecx_csplit.obj" \
	"$(INTDIR)\vecx_csum.obj" \
	"$(INTDIR)\vecx_explog.obj" \
	"$(INTDIR)\vecx_fabs.obj" \
	"$(INTDIR)\vecx_farith.obj" \
	"$(INTDIR)\vecx_fclip.obj" \
	"$(INTDIR)\vecx_fcmp.obj" \
	"$(INTDIR)\vecx_fdot.obj" \
	"$(INTDIR)\vecx_fmax.obj" \
	"$(INTDIR)\vecx_fmin.obj" \
	"$(INTDIR)\vecx_fsum.obj" \
	"$(INTDIR)\vecx_iabs.obj" \
	"$(INTDIR)\vecx_iarith.obj" \
	"$(INTDIR)\vecx_iclip.obj" \
	"$(INTDIR)\vecx_icmp.obj" \
	"$(INTDIR)\vecx_imax.obj" \
	"$(INTDIR)\vecx_imin.obj" \
	"$(INTDIR)\vecx_isum.obj" \
	"$(INTDIR)\vecx_round.obj" \
	"$(INTDIR)\vecx_rsqrt.obj" \
	"$(INTDIR)\vecx_scl_bit.obj" \
	"$(INTDIR)\vecx_scl_carith.obj" \
	"$(INTDIR)\vecx_scl_cfmul.obj" \
	"$(INTDIR)\vecx_scl_farith.obj" \
	"$(INTDIR)\vecx_scl_fcmp.obj" \
	"$(INTDIR)\vecx_scl_iarith.obj" \
	"$(INTDIR)\vecx_scl_icmp.obj" \
	"$(INTDIR)\vecx_sqrt.obj" \
	"$(INTDIR)\vecx_trigon.obj" \
	"$(INTDIR)\version.obj" \
	"$(INTDIR)\vm.obj" \
	"$(INTDIR)\win32.obj" \
	"$(INTDIR)\cubic.obj"

"$(OUTDIR)\ftsdll.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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


SOURCE=..\fts\src\lang\veclib\portable\cfft.c

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


SOURCE=..\fts\src\lang\veclib\ftlfun.c

"$(INTDIR)\ftlfun.obj" : $(SOURCE) "$(INTDIR)"
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


SOURCE=..\fts\src\lang\utils\hashtab.c

"$(INTDIR)\hashtab.obj" : $(SOURCE) "$(INTDIR)"
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


SOURCE=..\fts\src\lang\veclib\portable\rfft_shuffle.c

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


SOURCE=..\fts\src\lang\veclib\portable\vec_bit.c

"$(INTDIR)\vec_bit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vec_carith.c

"$(INTDIR)\vec_carith.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vec_cfmul.c

"$(INTDIR)\vec_cfmul.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vec_cmerge.c

"$(INTDIR)\vec_cmerge.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vec_cplx.c

"$(INTDIR)\vec_cplx.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vec_cpy.c

"$(INTDIR)\vec_cpy.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vec_csplit.c

"$(INTDIR)\vec_csplit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vec_csum.c

"$(INTDIR)\vec_csum.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vec_explog.c

"$(INTDIR)\vec_explog.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vec_fabs.c

"$(INTDIR)\vec_fabs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vec_farith.c

"$(INTDIR)\vec_farith.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vec_fclip.c

"$(INTDIR)\vec_fclip.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vec_fcmp.c

"$(INTDIR)\vec_fcmp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vec_fdot.c

"$(INTDIR)\vec_fdot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vec_fft.c

"$(INTDIR)\vec_fft.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vec_fmax.c

"$(INTDIR)\vec_fmax.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vec_fmin.c

"$(INTDIR)\vec_fmin.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vec_fsum.c

"$(INTDIR)\vec_fsum.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vec_iabs.c

"$(INTDIR)\vec_iabs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vec_iarith.c

"$(INTDIR)\vec_iarith.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vec_iclip.c

"$(INTDIR)\vec_iclip.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vec_icmp.c

"$(INTDIR)\vec_icmp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vec_imax.c

"$(INTDIR)\vec_imax.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vec_imin.c

"$(INTDIR)\vec_imin.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vec_isum.c

"$(INTDIR)\vec_isum.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vec_round.c

"$(INTDIR)\vec_round.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vec_rsqrt.c

"$(INTDIR)\vec_rsqrt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vec_scl_bit.c

"$(INTDIR)\vec_scl_bit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vec_scl_carith.c

"$(INTDIR)\vec_scl_carith.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vec_scl_cfmul.c

"$(INTDIR)\vec_scl_cfmul.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vec_scl_farith.c

"$(INTDIR)\vec_scl_farith.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vec_scl_fcmp.c

"$(INTDIR)\vec_scl_fcmp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vec_scl_iarith.c

"$(INTDIR)\vec_scl_iarith.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vec_scl_icmp.c

"$(INTDIR)\vec_scl_icmp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vec_sqrt.c

"$(INTDIR)\vec_sqrt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vec_trigon.c

"$(INTDIR)\vec_trigon.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\vecmod.c

"$(INTDIR)\vecmod.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\vecstat.c

"$(INTDIR)\vecstat.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vecx_bit.c

"$(INTDIR)\vecx_bit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vecx_carith.c

"$(INTDIR)\vecx_carith.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vecx_cfmul.c

"$(INTDIR)\vecx_cfmul.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vecx_cmerge.c

"$(INTDIR)\vecx_cmerge.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vecx_cplx.c

"$(INTDIR)\vecx_cplx.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vecx_cpy.c

"$(INTDIR)\vecx_cpy.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vecx_csplit.c

"$(INTDIR)\vecx_csplit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vecx_csum.c

"$(INTDIR)\vecx_csum.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vecx_explog.c

"$(INTDIR)\vecx_explog.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vecx_fabs.c

"$(INTDIR)\vecx_fabs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vecx_farith.c

"$(INTDIR)\vecx_farith.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vecx_fclip.c

"$(INTDIR)\vecx_fclip.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vecx_fcmp.c

"$(INTDIR)\vecx_fcmp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vecx_fdot.c

"$(INTDIR)\vecx_fdot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vecx_fmax.c

"$(INTDIR)\vecx_fmax.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vecx_fmin.c

"$(INTDIR)\vecx_fmin.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vecx_fsum.c

"$(INTDIR)\vecx_fsum.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vecx_iabs.c

"$(INTDIR)\vecx_iabs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vecx_iarith.c

"$(INTDIR)\vecx_iarith.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vecx_iclip.c

"$(INTDIR)\vecx_iclip.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vecx_icmp.c

"$(INTDIR)\vecx_icmp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vecx_imax.c

"$(INTDIR)\vecx_imax.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vecx_imin.c

"$(INTDIR)\vecx_imin.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vecx_isum.c

"$(INTDIR)\vecx_isum.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vecx_round.c

"$(INTDIR)\vecx_round.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vecx_rsqrt.c

"$(INTDIR)\vecx_rsqrt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vecx_scl_bit.c

"$(INTDIR)\vecx_scl_bit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vecx_scl_carith.c

"$(INTDIR)\vecx_scl_carith.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vecx_scl_cfmul.c

"$(INTDIR)\vecx_scl_cfmul.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vecx_scl_farith.c

"$(INTDIR)\vecx_scl_farith.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vecx_scl_fcmp.c

"$(INTDIR)\vecx_scl_fcmp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vecx_scl_iarith.c

"$(INTDIR)\vecx_scl_iarith.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vecx_scl_icmp.c

"$(INTDIR)\vecx_scl_icmp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vecx_sqrt.c

"$(INTDIR)\vecx_sqrt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fts\src\lang\veclib\portable\vecx_trigon.c

"$(INTDIR)\vecx_trigon.obj" : $(SOURCE) "$(INTDIR)"
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



!ENDIF 

