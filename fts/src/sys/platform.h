/*
 *                      Copyright (c) 1993 by IRCAM
 *                          All rights reserved.
 *
 *  For any information regarding this and other IRCAM software, please
 *  send email to:
 *                              manager@ircam.fr
 *
 *
 *
 * FTS by Miller Puckette
 * New features by Maurizio De Cecco and Francois Dechelle
 *
 */

#ifndef _PLATFORM_H
#define _PLATFORM_H

/*********************************************************************
 *
 *  gnu (gcc) standard basic platform
 *
 */
#if defined(GNU)

#define FTS_ARCH_NAME        "gnu"
#define restrict
/* #define HAS_DL */
#define HAS_TIME_OF_THE_DAY
#define HAS_UNROLL_BY8
#define HAS_UNIX
#define VECLIB_LOOP_UNROLL 8
#define FTS_USE_NAMED_PIPES

/*********************************************************************
 *
 *  linux/gnu (gcc) standard basic platform
 *
 */
#elif defined(LINUX)

#define FTS_ARCH_NAME "linux"
#define restrict
#undef HI_OPT /* hyper experimental optimizations */
#define HAS_DL
#define HAS_TIME_OF_THE_DAY
#define HAS_UNROLL_BY8
#define HAS_UNIX
#define HAS_OSS /* the OSS sound system */
#define VECLIB_LOOP_UNROLL 4
#define FTS_USE_NAMED_PIPES
#define FTS_HAS_LITTLE_ENDIAN

/*********************************************************************
 *
 *  different SGI platforms
 *
 */
#elif defined(SGI)

#define FTS_ARCH_NAME "sgi"

#define HAS_DTD /* direct to disk support */
#define HAS_DL
#define HI_OPT /* hyper experimental optimizations */
#define HAS_TIME_OF_THE_DAY
#define HAS_UNROLL_BY8
#define HAS_UNIX
/* #define HAS_TTY_DEV */ /* don't commit with this on until ttydev is fixed */
#define VECLIB_LOOP_UNROLL 4
#define FTS_USE_NAMED_PIPES
#define FTS_HAS_BIG_ENDIAN

/*********************************************************************
 *
 *  SUN workstations solaris 2
 *
 */
#elif defined(SOLARIS2)

#define FTS_ARCH_NAME "solaris2"
#define restrict /* needed for compatibility with the newer restrict keyword */
#define HAS_DL
#define HAS_DTD
#define HAS_TIME_OF_THE_DAY
#define HAS_UNROLL_BY8
#define HAS_UNIX
#define FTS_HAS_BIG_ENDIAN

#else

#error "platform.h : no architecture defined"

#endif

#endif






