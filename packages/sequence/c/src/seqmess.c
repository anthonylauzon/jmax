/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#include <fts/fts.h>
#include <fts/packages/sequence/sequence.h>
#include <fts/packages/sequence/track.h>
#include <fts/packages/sequence/seqsym.h>
#include "seqmess.h"

/**************************************************************
 *
 *  set event methods
 *
 */

static fts_method_status_t
seqmess_set(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  seqmess_t *this = (seqmess_t *)o;

  switch(ac)
    {
    default:
      if(fts_is_number(at + ac - 1))
	this->position = fts_get_number_int(at + ac - 1);
    case 1:
      if(fts_is_symbol(at + 0))
	this->s = fts_get_symbol(at + 0);
    case 0:
      break;
    }
  
  return fts_ok;
}

static fts_method_status_t
seqmess_dump_state(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  seqmess_t *this = (seqmess_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  fts_message_t *mess = (fts_message_t *)fts_object_create(fts_message_class, 0, 0);

  fts_object_refer((fts_object_t *)mess);    
  
  /* send set message with pitch and duration */
  fts_message_set(mess, fts_s_set, 0, 0);
  fts_message_append_symbol(mess, this->s);
  fts_message_append_int(mess, this->position);
  fts_dumper_message_send(dumper, mess);
  
  fts_object_release((fts_object_t *)mess);  
  
  return fts_ok;
}

/**************************************************************
 *
 *  class
 *
 */

static fts_method_status_t
seqmess_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  seqmess_t *this = (seqmess_t *)o;

  this->s = NULL;
  this->position = 0;

  seqmess_set(o, NULL, ac, at, fts_nix);
  
  return fts_ok;
}

static void
seqmess_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(seqmess_t), seqmess_init, 0);

  fts_class_message_varargs(cl, fts_s_dump_state, seqmess_dump_state);

  fts_class_message_varargs(cl, fts_s_set, seqmess_set);
}

FTS_MODULE_INIT(seqmess)
{
  fts_class_install(seqsym_seqmess, seqmess_instantiate);
}
