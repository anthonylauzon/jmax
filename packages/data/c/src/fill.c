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
 */

#include <fts/fts.h>
#include <ftsconfig.h>

#include "vec.h"
#include "ivec.h"
#include "fvec.h"
#include "mat.h"

static void
fillfrom_number_to_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fill_t *this = (fill_t *)o;
  ivec_t *vec = ivec_atom_get(at);
  int n = fts_get_number_int(&this->a);

  ivec_set_const(vec, n);  
  fts_outlet_send(o, 0, ivec_symbol, 1, at);
}

static void
fillfrom_number_to_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fill_t *this = (fill_t *)o;
  fvec_t *vec = fvec_atom_get(at);
  float f = fts_get_number_float(&this->a);

  fvec_set_const(vec, f);
  fts_outlet_send(o, 0, fvec_symbol, 1, at);
}

/* int vector */

static void
fillto_ivec_from_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fill_t *this = (fill_t *)o;
  ivec_t *vec = ivec_atom_get(&this->a);
  int size = ivec_get_size(vec);
  int n = fts_get_number_int(at);
  int i;

  for(i=0; i<size; i++)
    ivec_set_element(vec, i, n);
  
  fts_outlet_send(o, 0, ivec_symbol, 1, &this->a);
}

static void
fillto_ivec_from_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fill_t *this = (fill_t *)o;
  ivec_t *this_vec = ivec_atom_get(&this->a);
  ivec_t *in_vec = ivec_atom_get(at);
  int * restrict this_ptr = ivec_get_ptr(this_vec);
  int * restrict in_ptr = ivec_get_ptr(in_vec);
  int this_size = ivec_get_size(this_vec);
  int in_size = ivec_get_size(in_vec);
  int size = (this_size <= in_size)? this_size: in_size;
  int i;

  for(i=0; i<size; i++)
    this_ptr[i] = in_ptr[i];
  
  fts_outlet_send(o, 0, ivec_symbol, 1, &this->a);
}

static void
fillto_ivec_from_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fill_t *this = (fill_t *)o;
  ivec_t *this_vec = ivec_atom_get(&this->a);
  fvec_t *in_vec = fvec_atom_get(at);
  int * restrict this_ptr = ivec_get_ptr(this_vec);
  float * restrict in_ptr = fvec_get_ptr(in_vec);
  int this_size = ivec_get_size(this_vec);
  int in_size = fvec_get_size(in_vec);
  int size = (this_size <= in_size)? this_size: in_size;
  int i;

  for(i=0; i<size; i++)
    this_ptr[i] = (int)in_ptr[i];
  
  fts_outlet_send(o, 0, ivec_symbol, 1, &this->a);
}

static void
fillfrom_ivec_to_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fill_t *this = (fill_t *)o;
  ivec_t *this_vec = ivec_atom_get(&this->a);
  ivec_t *in_vec = ivec_atom_get(at);
  int * restrict this_ptr = ivec_get_ptr(this_vec);
  int * restrict in_ptr = ivec_get_ptr(in_vec);
  int this_size = ivec_get_size(this_vec);
  int in_size = ivec_get_size(in_vec);
  int size = (this_size <= in_size)? this_size: in_size;
  int i;

  for(i=0; i<size; i++)
    in_ptr[i] = this_ptr[i];
  
  fts_outlet_send(o, 0, ivec_symbol, 1, at);
}

static void
fillfrom_ivec_to_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fill_t *this = (fill_t *)o;
  ivec_t *this_vec = ivec_atom_get(&this->a);
  fvec_t *in_vec = fvec_atom_get(at);
  int * restrict this_ptr = ivec_get_ptr(this_vec);
  float * restrict in_ptr = fvec_get_ptr(in_vec);
  int this_size = ivec_get_size(this_vec);
  int in_size = fvec_get_size(in_vec);
  int size = (this_size <= in_size)? this_size: in_size;
  int i;

  for(i=0; i<size; i++)
    in_ptr[i] = (float)this_ptr[i];
  
  fts_outlet_send(o, 0, ivec_symbol, 1, at);
}

/* float vector */

static void
fillto_fvec_from_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fill_t *this = (fill_t *)o;
  fvec_t *vec = fvec_atom_get(&this->a);
  int size = fvec_get_size(vec);
  float f = fts_get_number_int(at);
  int i;

  for(i=0; i<size; i++)
    fvec_set_element(vec, i, f);
  
  fts_outlet_send(o, 0, fvec_symbol, 1, &this->a);
}

static void
fillto_fvec_from_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fill_t *this = (fill_t *)o;
  fvec_t *this_vec = fvec_atom_get(&this->a);
  ivec_t *in_vec = ivec_atom_get(at);
  float * restrict this_ptr = fvec_get_ptr(this_vec);
  int * restrict in_ptr = ivec_get_ptr(in_vec);
  int this_size = fvec_get_size(this_vec);
  int in_size = ivec_get_size(in_vec);
  int size = (this_size <= in_size)? this_size: in_size;
  int i;

  for(i=0; i<size; i++)
    this_ptr[i] = (float)in_ptr[i];
  
  fts_outlet_send(o, 0, fvec_symbol, 1, &this->a);
}

static void
fillto_fvec_from_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fill_t *this = (fill_t *)o;
  fvec_t *this_vec = fvec_atom_get(&this->a);
  fvec_t *in_vec = fvec_atom_get(at);
  float * restrict this_ptr = fvec_get_ptr(this_vec);
  float * restrict in_ptr = fvec_get_ptr(in_vec);
  int this_size = fvec_get_size(this_vec);
  int in_size = fvec_get_size(in_vec);
  int size = (this_size <= in_size)? this_size: in_size;
  int i;

  for(i=0; i<size; i++)
    this_ptr[i] = in_ptr[i];
  
  fts_outlet_send(o, 0, fvec_symbol, 1, &this->a);
}

static void
fillfrom_fvec_to_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fill_t *this = (fill_t *)o;
  fvec_t *this_vec = fvec_atom_get(&this->a);
  ivec_t *in_vec = ivec_atom_get(at);
  float * restrict this_ptr = fvec_get_ptr(this_vec);
  int * restrict in_ptr = ivec_get_ptr(in_vec);
  int this_size = fvec_get_size(this_vec);
  int in_size = ivec_get_size(in_vec);
  int size = (this_size <= in_size)? this_size: in_size;
  int i;

  for(i=0; i<size; i++)
    in_ptr[i] = (int)this_ptr[i];
  
  fts_outlet_send(o, 0, fvec_symbol, 1, at);
}

static void
fillfrom_fvec_to_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fill_t *this = (fill_t *)o;
  fvec_t *this_vec = fvec_atom_get(&this->a);
  fvec_t *in_vec = fvec_atom_get(at);
  float * restrict this_ptr = fvec_get_ptr(this_vec);
  float * restrict in_ptr = fvec_get_ptr(in_vec);
  int this_size = fvec_get_size(this_vec);
  int in_size = fvec_get_size(in_vec);
  int size = (this_size <= in_size)? this_size: in_size;
  int i;

  for(i=0; i<size; i++)
    in_ptr[i] = this_ptr[i];
  
  fts_outlet_send(o, 0, fvec_symbol, 1, at);
}
