/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * See file LICENSE for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */


#ifndef _FTS_PLATFORM_H
#define _FTS_PLATFORM_H

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/*********************************************************************
 *
 *  linux/gnu (gcc) standard basic platform
 *
 */
#if defined(LINUXPC)

#define FTS_ARCH_NAME "Linux (Intel and compatible processors)"
#define restrict
#define HAS_UNIX
#define HAS_PTHREADS
#define VECLIB_LOOP_UNROLL NO
#define FTS_HAS_LITTLE_ENDIAN
#define USE_FP_ONSET
#define HAS_IEEE_FLOAT
#define HAVE_UNISTD_H 1
#define HAVE_ALLOCA_H 1
#define HAVE_SYS_TIME_H 1
#define HAVE_SYS_PARAM_H 1
#undef HAVE_DIRECT_H
#undef HAVE_PROCESS_H
#undef HAVE_IO_H 
#define FTS_API extern
#define fts_path_separator      ':'
#define fts_path_is_absolute(_p) (_p[0] == '/')

#elif defined(LINUXPPC)

#define FTS_ARCH_NAME "Linux (Power-PC processor)"
#define restrict
#define HAS_UNIX
#define HAS_PTHREADS
#define VECLIB_LOOP_UNROLL NO
#define FTS_HAS_BIG_ENDIAN
#define HAVE_UNISTD_H 1
#define HAVE_ALLOCA_H 1
#define HAVE_SYS_TIME_H 1
#define HAVE_SYS_PARAM_H 1
#undef HAVE_DIRECT_H
#undef HAVE_PROCESS_H
#undef HAVE_IO_H 
#define FTS_API extern
#define fts_path_separator      ':'
#define fts_path_is_absolute(_p) (_p[0] == '/')

#elif defined(MACOSX)

#define FTS_ARCH_NAME "MacOS-X"
#define restrict
#define HAS_UNIX
#undef HAS_PTHREADS
#define VECLIB_LOOP_UNROLL NO
#define FTS_HAS_BIG_ENDIAN
#define HAVE_UNISTD_H 1
#define HAVE_SYS_TIME_H 1
#define HAVE_SYS_PARAM_H 1
#undef HAVE_ALLOCA_H
#undef HAVE_DIRECT_H
#undef HAVE_PROCESS_H
#undef HAVE_IO_H 
#define FTS_API extern
#define fts_path_separator      ':'
#define fts_path_is_absolute(_p) (_p[0] == '/')


/*********************************************************************
 *
 *  different SGI platforms
 *
 */
#elif defined(SGI)

#define FTS_ARCH_NAME "SGI"

#define HAVE_AF_VIRTUAL_PARAMETERS
#define HAS_UNIX
#define HAS_PTHREADS
#define VECLIB_LOOP_UNROLL 4
#define FTS_HAS_BIG_ENDIAN
#define HAS_IEEE_FLOAT
#define HAVE_UNISTD_H 1
#define HAVE_ALLOCA_H 1
#define HAVE_SYS_TIME_H 1
#define HAVE_SYS_PARAM_H 1
#undef HAVE_DIRECT_H
#undef HAVE_PROCESS_H
#undef HAVE_IO_H 
#define FTS_API extern
#define fts_path_separator      ':'
#define fts_path_is_absolute(_p) (_p[0] == '/')

/*********************************************************************
 *
 *  SUN workstations solaris 2
 *
 */
#elif defined(SOLARIS2)

#define FTS_ARCH_NAME "Solaris-2"
#define restrict /* needed for compatibility with the newer restrict keyword */
#define HAS_UNIX
#define FTS_HAS_BIG_ENDIAN
#define HAVE_UNISTD_H 1
#define HAVE_ALLOCA_H 1
#define HAVE_SYS_TIME_H 1
#define HAVE_SYS_PARAM_H 1
#undef HAVE_DIRECT_H
#undef HAVE_PROCESS_H
#undef HAVE_IO_H 
#define FTS_API extern
#define fts_path_separator      ':'
#define fts_path_is_absolute(_p) (_p[0] == '/')

/*********************************************************************
 *
 *  Win32 platform
 *
 *  For compilation with the CygWin platform
 */
#elif defined(WIN32)

#define FTS_ARCH_NAME "Win32 (Intel and compatible processors)"
#define restrict
/*  #undef HAS_UNIX */
/*  #undef HAS_PTHREADS */
#define VECLIB_LOOP_UNROLL NO
#define FTS_HAS_LITTLE_ENDIAN
#define USE_FP_ONSET
#define HAS_IEEE_FLOAT

#undef HAVE_UNISTD_H 
#undef HAVE_ALLOCA_H 
#undef HAVE_SYS_TIME_H
#undef HAVE_SYS_PARAM_H 
#define HAVE_DIRECT_H 1
#define HAVE_PROCESS_H 1
#define HAVE_IO_H 1

#define COMPILATION_ARCH_STRING "i386-win32"

#ifdef _DEBUG
#define COMPILATION_MODE_STRING "debug"
#else
#define COMPILATION_MODE_STRING "opt"
#endif

#define COMPILATION_INFO_STRING "(compiled for i386-win32)"

#if defined(FTSDLL_EXPORTS)
#define FTS_API __declspec(dllexport)
#else
#define FTS_API __declspec(dllimport)
#endif


/* mapping standard Unix function names to their win32 equivalent */
#define MAXPATHLEN _MAX_PATH
#define open  _open
#define read  _read
#define write  _write
#define close  _close
#define getcwd _getcwd
#define isatty _isatty
#define alloca _alloca
#define getpid _getpid
#define snprintf _snprintf
#define bcopy(src,dst,n)  memcpy(dst,src,n)

#define realpath  win32_realpath
FTS_API char* win32_realpath(const char* path, char* resolved_path);


#define fts_path_separator      ';'

/*
 *  Valid absolute paths are:  /c/jmax, c:/jmax, c:\jmax
 */
#define fts_path_is_absolute(_p) \
 ((_p[0] == '/') || \
  ((_p[1] == ':') && (_p[2] == '/')) || \
  ((_p[1] == ':') && (_p[2] == '\\')))


#else

#error "platform.h : no architecture defined"

#endif

#endif






