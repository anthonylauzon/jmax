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
#include "seqsym.h"
#include "sequence.h"
#include "eventtrk.h"

typedef struct _message_
{
  fts_object_t _obj;
  fts_symbol_t s;
  int ac;
  fts_atom_t *at;
  int pos; /* property */
} message_t;

static void
message_set_message(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  message_t *this = (message_t *)o;
  int i;

  if(ac)
    {
      /* implement conventions of conversion from atom list to message */
      if(fts_is_symbol(at))
	{
	  this->s = fts_get_symbol(at);

	  /* skip selector */
	  ac--;
	  at++;
	}
      else if(ac == 1)
	this->s = fts_get_selector(at);
      else
	this->s = fts_s_list;

      if(ac != this->ac)
	{
	  if(this->at)
	    fts_block_free(this->at, this->ac * sizeof(fts_atom_t));
	  
	  this->at = (fts_atom_t *)fts_block_alloc(ac * sizeof(fts_atom_t));
	}
      
      this->ac = ac;
      
      for(i=0; i<ac; i++)
	this->at[i] = at[i];
    }
  else
    {
      this->s = 0;

      if(this->at)
	fts_block_free(this->at, this->ac * sizeof(fts_atom_t));
	  
      this->ac = 0;
    }
}

/**************************************************************
 *
 *  set event methods
 *
 */

static void
message_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  message_t *this = (message_t *)o;
  
  this->s = fts_get_symbol(at + 0);
  this->pos = fts_get_int(at + ac - 1);
}

void 
message_get_atoms(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  message_t *this = (message_t *)o;
  int *n = fts_get_ptr(at);
  fts_atom_t *a = fts_get_ptr(at + 1);

  fts_set_symbol(a + 0, this->s);
  fts_set_int(a + 1, this->pos);

  *n = 2;
}

/**************************************************************
 *
 *  class
 *
 */

static void
message_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  message_t *this = (message_t *)o;
  
  this->s = 0;
  this->ac = 0;
  this->at = 0;

  message_set_message(o, 0, 0, 1, at + 1);

  if(ac > 1)
    this->pos = fts_get_number_int(at + 2);
}

static fts_status_t
message_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(message_t), 0, 0, 0); 
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, message_init);

  fts_method_define_varargs(cl, fts_SystemInlet, seqsym_set, message_set);
  fts_method_define_varargs(cl, fts_SystemInlet, seqsym_get_atoms, message_get_atoms);

  return fts_Success;
}

void
message_config(void)
{
  fts_class_install(seqsym_message, message_instantiate);
}
