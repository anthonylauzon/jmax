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

#if defined(GNU)		/* gnu (gcc) standard basic platform */

/* #define HAS_DL */
#define HAS_TIME_OF_THE_DAY
#define HAS_UNROLL_BY8
#define HAS_UNIX
/* #define HAS_frexp_AND_ldexp_AND_modf*/ /* for rough math */
#define VECLIB_LOOP_UNROLL 8

#define FTS_USE_NAMED_PIPES

#define FTS_ARCH_NAME        "gnu"

#elif defined(LINUX)		/* Linux/gnu (gcc) standard basic platform */

#define HAS_DL
#define HAS_TIME_OF_THE_DAY
#define HAS_UNROLL_BY8
#define HAS_UNIX
#define HAS_OSS			/* the OSS sound system */
/* #define HAS_frexp_AND_ldexp_AND_modf*/ /* for rough math */
#define VECLIB_LOOP_UNROLL 8

#define FTS_USE_NAMED_PIPES

#define FTS_ARCH_NAME        "linux"

#elif defined(SGI)

#define HAS_DTD			/* direct to disk support */
#define HAS_DL
#define HI_OPT			/* hyper experimental optimizations */
#define HAS_TIME_OF_THE_DAY
#define HAS_UNROLL_BY8
#define HAS_UNIX
/* #define HAS_TTY_DEV */ /* DOn't commit with this on until ttydev is fixed */
#define HAS_frexp_AND_ldexp_AND_modf /* for rough math */

#define VECLIB_LOOP_UNROLL 8

#define FTS_USE_NAMED_PIPES

#define FTS_ARCH_NAME        "sgi"

#else

#error "platform.h : no architecture defined"

#endif

#endif





