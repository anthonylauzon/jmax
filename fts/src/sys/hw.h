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

#ifndef _HW_H_
#define _HW_H_

/* generic include with the prototypes of HW dependent functions. */

extern void *fts_dl_open( const char *filename, char *error);
extern int fts_dl_lookup( void *handle, const char *symbol, void **address, char *error);

extern void fts_enable_fpe_traps( void);
extern unsigned int fts_check_fpe( void);
extern void fts_disable_fpe_traps( void);
/* 
 * Bit masks returned with fts_check_fpe;
 */
#define FTS_INVALID_FPE  0x01
#define FTS_DIVIDE0_FPE  0x02
#define FTS_OVERFLOW_FPE 0x04
#define FTS_INEXACT_FPE  0x08
#define FTS_UNDERFLOW_FPE 0x10
#define FTS_DENORMALIZED_FPE 0x20

extern int fts_memory_is_locked( void);
extern int fts_lock_memory( void);
extern void fts_unlock_memory( void);

extern void fts_platform_init( void);

extern void fts_real_time_on( void);
extern void fts_real_time_off( void);

extern void fts_pause( void);

#endif
