/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#ifndef _HW_H_
#define _HW_H_

/* generic include with the prototypes of HW dependent functions. */

extern void fts_platform_init(void);
extern void fts_set_no_real_time(void);
extern void fts_pause(void);
extern int fts_memory_is_locked(void);
extern void fts_unlock_memory(void);
extern unsigned int fts_check_fpe(void);

typedef void (* fts_fpe_handler)(int which);
extern void fts_set_fpe_handler(fts_fpe_handler fh);
extern void fts_reset_fpe_handler(void);

/* bit masks to be used with fts_check_fpe;
 * 
 */

#define FTS_INVALID_FPE  0x01
#define FTS_DIVIDE0_FPE  0x02
#define FTS_OVERFLOW_FPE 0x04
#define FTS_INEXACT_FPE  0x08
#define FTS_UNDERFLOW_FPE 0x10

#endif
