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
 * Authors: Francois Dechelle, Norbert Schnell
 *
 */

#ifndef _SIGNAL_WAVE_H_
#define _SIGNAL_WAVE_H_

#define WAVE_TABLE_BITS 9
#define WAVE_TABLE_SIZE 512

#include "fts.h"
#include "floatvec.h"

struct wave_ftl_symbols
{
  fts_symbol_t outplace;
  fts_symbol_t inplace;
} sym_wave_t;

extern struct wave_ftl_symbols wave_ftl_symbols_ptr;
extern struct wave_ftl_symbols wave_ftl_symbols_fvec;

typedef struct wave_data
{ 
  /* table stored as float vector or pointer to float */
  union
  {
    float *ptr;
    float_vector_t *fvec;
  } table;
} wave_data_t;

typedef struct _wave_
{
  fts_object_t o;
  ftl_data_t data;
} wave_t;

extern void wave_declare_functions(void);

#endif




