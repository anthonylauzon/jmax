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
#include "sequence.h"
#include "eventtrk.h"
#include "messevt.h"

fts_symbol_t messevt_symbol = 0;

static void messevt_set_message(fts_object_t *this, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);

static void
messevt_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messevt_t *this = (messevt_t *)o;
  
  event_init(&this->head);
  
  this->s = 0;
  this->ac = -2;
  this->at = 0;
  this->pos = -1;

  messevt_set_message(o, 0, 0, ac - 1, at + 1);
}

/**************************************************************
 *
 *  mandatory event methods
 *
 */

void
messevt_upload(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messevt_t *this = (messevt_t *)o;
  fts_atom_t a[3];

  /*fts_set_float(this->at + 0, 1000.0 * event_get_time(&this->head));*/
  fts_set_float(this->at + 0, event_get_time(&this->head));
  fts_set_symbol(this->at + 1, messevt_symbol);

  fts_client_upload(o, event_symbol, ac + 2, this->at);
}

static void
messevt_move(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  /* generic event "super class" */
  event_t *this = (event_t *)o;
  float time = fts_get_float(at + 0);

  eventtrk_move_event(this, time);
}

static void
messevt_set_message(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messevt_t *this = (messevt_t *)o;
  int i;

  if(ac)
    {
      if(fts_is_symbol(at))
	this->s = fts_get_symbol(at);
      else if(ac == 1)
	this->s = fts_get_selector(at);
      else
	this->s = fts_s_list;

      ac--; /* skip selector */
      at++;
      
      if(ac != this->ac)
	{
	  if(this->at)
	    fts_block_free(this->at, (this->ac + 2) * sizeof(fts_atom_t)); /* first two atoms reserved for uploading */
	  
	  this->at = (fts_atom_t *)fts_block_alloc((ac + 2) * sizeof(fts_atom_t)); /* first two atoms reserved for uploading */
	}
      
      this->ac = ac;
      
      for(i=0; i<ac; i++)
	this->at[i + 2] = at[i]; /* first two atoms reserved for uploading */
    }
  else
    {
      this->s = 0;

      if(this->at)
	fts_block_free(this->at, (this->ac + 2) * sizeof(fts_atom_t));
	  
      this->ac = 0;
    }
}

static void
messevt_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messevt_t *this = (messevt_t *)o;

  messevt_set_message(o, 0, 0, ac - 1, at);
  this->pos = fts_get_int(at + ac - 1);
}

void 
messevt_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messevt_t *this = (messevt_t *)o;

  post("%s ", fts_symbol_name(this->s));
  post_atoms(this->ac, this->at);
  post("\n");
}

/**************************************************************
 *
 *  set event methods
 *
 */

static void
messevt_set_from_string(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messevt_t *this = (messevt_t *)o;

  /* &@$*&!@$%_*&!@$%&*$% 
   * for now we set the hole message string to the selector until there is a parser 
   * (the y-pos integer following is ingnored for now!)
   */

  messevt_set_message(o, 0, 0, 1, at);
}

/**************************************************************
 *
 *  class
 *
 */
static fts_status_t
messevt_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(messevt_t), 0, 0, 0); 
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, messevt_init);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("upload"), messevt_upload);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("move"), messevt_move);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("set"), messevt_set);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("print"), messevt_print);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("set_from_string"), messevt_set_from_string);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("set_message"), messevt_set_message);
  /*fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("set_position"), messevt_set_position);*/

  return fts_Success;
}

void
messevt_config(void)
{
  messevt_symbol = fts_new_symbol("messevt");

  fts_class_install(messevt_symbol, messevt_instantiate);
}
