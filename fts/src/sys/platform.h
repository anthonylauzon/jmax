/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */

#ifndef _PLATFORM_H
#define _PLATFORM_H

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif

/*********************************************************************
 *
 *  linux/gnu (gcc) standard basic platform
 *
 */
#if defined(LINUXPC)

#define FTS_ARCH_NAME "linux"
#define restrict
#define HAS_UNIX
#define HAS_DTD /* direct to disk support */
#define HAS_PTHREADS
#define HAS_OSS /* the OSS sound system */
#define VECLIB_LOOP_UNROLL NO
#define FTS_HAS_LITTLE_ENDIAN

/*********************************************************************
 *
 *  different SGI platforms
 *
 */
#elif defined(SGI)

#define FTS_ARCH_NAME "sgi"

#define HAVE_AF_VIRTUAL_PARAMETERS
#define HAS_DTD /* direct to disk support */
#define HAS_UNIX
#define HAS_PTHREADS
/* #define HAS_TTY_DEV */ /* don't commit with this on until ttydev is fixed */
#define VECLIB_LOOP_UNROLL 4
#define FTS_HAS_BIG_ENDIAN

/*********************************************************************
 *
 *  SUN workstations solaris 2
 *
 */
#elif defined(SOLARIS2)

#define FTS_ARCH_NAME "solaris2"
#define restrict /* needed for compatibility with the newer restrict keyword */
#define HAS_DTD
#define HAS_UNIX
#define FTS_HAS_BIG_ENDIAN

#else

#error "platform.h : no architecture defined"

#endif

#endif






