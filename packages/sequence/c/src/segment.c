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
#include <fts/fts.h>
#include "seqsym.h"
#include "segment.h"

fts_class_t *segment_class = 0;

static void
segment_set_duration(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  segment_t *this = (segment_t *)o;  

  if(fts_is_number(at))
    {
      double duration = fts_get_number_float(at);
      
      if(duration > 0.0)
	this->duration = duration;
      else
	this->duration = 0.0;
    }
}

static void
segment_set_attack(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  segment_t *this = (segment_t *)o;  

  if(fts_is_number(at))
    {
      double attack = fts_get_number_float(at);
      
      if(attack > 0.0)
	this->attack = attack;
      else
	this->attack = 0.0;
    }
}

static void
segment_set_release(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  segment_t *this = (segment_t *)o;  

  if(fts_is_number(at))
    {
      double release = fts_get_number_float(at);
      
      if(release > 0.0)
	this->release = release;
      else
	this->release = 0.0;
    }
}

static void
segment_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  segment_t *this = (segment_t *)o;  

  switch(ac)
    {
    default:
    case 4:
      segment_set_release(o, 0, 0, 1, at + 3);
    case 3: 
      segment_set_attack(o, 0, 0, 1, at + 2);
    case 2:
      segment_set_duration(o, 0, 0, 1, at + 1);
    case 1:
      fts_atom_assign(&this->value, at);
    case 0:
      break;
    }
}

static void
segment_get_array(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  segment_t *this = (segment_t *)o;
  fts_array_t *array = fts_get_array(at);

  fts_array_append(array, 1, &this->value);
  fts_array_append_float(array, this->duration);
  fts_array_append_float(array, this->attack);
  fts_array_append_float(array, this->release);
}

static void
segment_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  segment_t *this = (segment_t *)o;

  if(fts_is_object(&this->value))
    {
      fts_object_t *obj = fts_get_object(&this->value);
      fts_symbol_t classname = fts_object_get_class_name(obj);

      post("{<%s> duration=%g attack=%g release=%g}\n", fts_symbol_name(classname), this->duration, this->attack, this->release);
    }
  else
    {
      post("{");
      post_atoms(1, &this->value);
      post(" duration=%g attack=%g release=%g}\n", this->duration, this->attack, this->release);      
    }
}

/**************************************************************
 *
 *  class
 *
 */
static void
segment_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  segment_t *this = (segment_t *)o;  

  ac--;
  at++;

  fts_set_void(&this->value);

  this->duration = 0.0;
  this->attack = 0.0;
  this->release = 0.0;
  
  segment_set(o, 0, 0, ac, at);
}

static void
segment_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  segment_t *this = (segment_t *)o;  

  fts_atom_void(&this->value);
}

static fts_status_t
segment_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(segment_t), 1, 0, 0); 
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, segment_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, segment_delete);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_get_array, segment_get_array);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set_from_array, segment_set);

  fts_method_define_varargs(cl, 0, fts_new_symbol("duration"), segment_set_duration);
  fts_method_define_varargs(cl, 0, fts_new_symbol("attack"), segment_set_attack);
  fts_method_define_varargs(cl, 0, fts_new_symbol("release"), segment_set_release);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_print, segment_print);

  return fts_Success;
}
/*****************************************************************
 *
 *  config & install
 *
 */

void
segment_config(void)
{
  fts_class_install(seqsym_segment, segment_instantiate);

  segment_class = fts_class_get_by_name(seqsym_segment);
}
