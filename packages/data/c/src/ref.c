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

#include "fts.h"

static fts_symbol_t sym_ref = 0;

/********************************************************************
 *
 *   object
 *
 */

typedef struct
{
  fts_object_t o;
  fts_atom_t a;		
} ref_t;

static void
ref_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ref_t *this = (ref_t *)o;

  this->a = at[1];
  
  fts_refer(&this->a);
}

static void
ref_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ref_t *this = (ref_t *) o;

  fts_release(&this->a);
}

static void
ref_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ref_t *this = (ref_t *) o;

  fts_outlet_send(o, 0, fts_get_selector(&this->a), 1, &this->a);
}

static void
ref_primitive_store(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ref_t *this = (ref_t *) o;
  
  this->a = at[0];
}

static void
ref_primitive_store_and_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ref_t *this = (ref_t *) o;
  
  this->a = at[0];

  fts_outlet_send(o, 0, fts_get_selector(&this->a), 1, &this->a);
}

static void
ref_store(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ref_t *this = (ref_t *) o;
  
  fts_release(&this->a);
  this->a = at[0];
  fts_refer(at);
}

static void
ref_store_and_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ref_t *this = (ref_t *) o;
  
  fts_release(&this->a);
  this->a = at[0];
  fts_refer(at);

  fts_outlet_send(o, 0, fts_get_selector(&this->a), 1, &this->a);
}

/********************************************************************
 *
 *   class
 *
 */

static fts_status_t
ref_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_type_t t;

  if(ac == 2 && fts_is_object(at + 1))
    {
      fts_symbol_t selector = fts_get_selector(at + 1);

      fts_class_init(cl, sizeof(ref_t), 2, 1, 0);
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, ref_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, ref_delete);
      
      fts_method_define_varargs(cl, 0, fts_s_bang, ref_bang);
      
      fts_method_define_varargs(cl, 0, selector, ref_store_and_output);
      fts_method_define_varargs(cl, 1, selector, ref_store);

      t = fts_get_type(at + 1);
      fts_outlet_type_define(cl, 0, selector, 1, &t);
      
      return fts_Success;
    }
  else
    return &fts_CannotInstantiate;
}

void
ref_config(void)
{
  sym_ref = fts_new_symbol("ref");

  fts_metaclass_install(sym_ref, ref_instantiate, fts_arg_type_equiv);
}
