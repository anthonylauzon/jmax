/*
 * jMax
 * Copyright (C) 1999 by IRCAM
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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
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
