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


#ifndef _PLATFORM_H
#define _PLATFORM_H

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
#define HAS_DTD /* direct to disk support */
#define HAS_PTHREADS
#define VECLIB_LOOP_UNROLL NO
#define FTS_HAS_LITTLE_ENDIAN
#define USE_FP_ONSET
#define USE_IEEE_WRAP

/* This macro is defined here because not all platforms have a isnanf macro (or function */

#define fts_isnanf(x) (((*(long *)&(x) & 0x7f800000L)==0x7f800000L)&& \
			 ((*(long *)&(x) & 0x007fffffL)!=0x00000000L) )


#elif defined(LINUXPPC)

#define FTS_ARCH_NAME "Linux (Power-PC processor)"
#define restrict
#define HAS_UNIX
#define HAS_DTD /* direct to disk support */
#define HAS_PTHREADS
#define VECLIB_LOOP_UNROLL NO
#define FTS_HAS_BIG_ENDIAN

/* This macro is defined here because not all platforms have a isnanf macro (or function */

#define fts_isnanf(x) (((*(long *)&(x) & 0x7f800000L)==0x7f800000L)&& \
			 ((*(long *)&(x) & 0x007fffffL)!=0x00000000L) )


#elif defined(MACOSX)

#define FTS_ARCH_NAME "MacOS-X"
#define restrict
#define HAS_UNIX
#undef HAS_DTD /* direct to disk support */
#undef HAS_PTHREADS
#define VECLIB_LOOP_UNROLL NO
#define FTS_HAS_BIG_ENDIAN

/* This macro is defined here because not all platforms have a isnanf macro (or function */

#define fts_isnanf(x) (((*(long *)&(x) & 0x7f800000L)==0x7f800000L)&& \
			 ((*(long *)&(x) & 0x007fffffL)!=0x00000000L) )


/*********************************************************************
 *
 *  different SGI platforms
 *
 */
#elif defined(SGI)

#define FTS_ARCH_NAME "SGI"

#define HAVE_AF_VIRTUAL_PARAMETERS
#define HAS_DTD /* direct to disk support */
#define HAS_UNIX
#define HAS_PTHREADS
/* #define HAS_TTY_DEV */ /* don't commit with this on until ttydev is fixed */
#define VECLIB_LOOP_UNROLL 4
#define FTS_HAS_BIG_ENDIAN

/* This macro is defined here because not all platforms have a isnanf macro (or function */

#define fts_isnanf(x) (((*(long *)&(x) & 0x7f800000L)==0x7f800000L)&& \
			 ((*(long *)&(x) & 0x007fffffL)!=0x00000000L) )


/*********************************************************************
 *
 *  SUN workstations solaris 2
 *
 */
#elif defined(SOLARIS2)

#define FTS_ARCH_NAME "Solaris-2"
#define restrict /* needed for compatibility with the newer restrict keyword */
#define HAS_DTD
#define HAS_UNIX
#define FTS_HAS_BIG_ENDIAN

/* This macro is defined here because not all platforms have a isnanf macro (or function */

#define fts_isnanf(x) (((*(long *)&(x) & 0x7f800000L)==0x7f800000L)&& \
			 ((*(long *)&(x) & 0x007fffffL)!=0x00000000L) )

/*********************************************************************
 *
 *  Win32 platform
 *
 *  For compilation with the CygWin platform
 */
#elif defined(WIN32)

#define FTS_ARCH_NAME "Win32 (Intel and compatible processors)"
#define restrict
#define HAS_UNIX
/*define HAS_DTD direct to disk support */
#define HAS_PTHREADS
#define VECLIB_LOOP_UNROLL NO
#define FTS_HAS_LITTLE_ENDIAN
#define USE_FP_ONSET
#define USE_IEEE_WRAP

/* This macro is defined here because not all platforms have a isnanf macro (or function */

#define fts_isnanf(x) (((*(long *)&(x) & 0x7f800000L)==0x7f800000L)&& \
			 ((*(long *)&(x) & 0x007fffffL)!=0x00000000L) )

/* This is included for the null device only (null.c) */
struct timespec {
  long tv_sec;
  long tv_nsec;
};
#define nanosleep  win32_nanosleep
int win32_nanosleep(struct timespec *, int);

#else

#error "platform.h : no architecture defined"

#endif

#endif






