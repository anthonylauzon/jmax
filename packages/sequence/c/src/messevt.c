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

static void messevt_set_message(fts_object_t *this, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);

typedef struct _messevt_
{
  event_t head;
  fts_symbol_t s;
  int ac;
  fts_atom_t *at;
  int pos;
} messevt_t;

static void
messevt_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messevt_t *this = (messevt_t *)o;
  
  event_init(&this->head);
  
  this->s = 0;
  this->ac = -4;
  this->at = 0;
  this->pos = fts_get_int(at + 1);

  messevt_set_message(o, 0, 0, ac - 2, at + 2);
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

  fts_set_float(this->at + 0, event_get_time(&this->head));
  fts_set_symbol(this->at + 1, seqsym_messevt);
  fts_set_int(this->at + 2, this->pos);
  fts_set_symbol(this->at + 3, this->s);
  
  fts_client_upload(o, seqsym_event, this->ac + 4, this->at);
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
	    fts_block_free(this->at, (this->ac + 4) * sizeof(fts_atom_t)); /* first two atoms reserved for uploading */
	  
	  this->at = (fts_atom_t *)fts_block_alloc((ac + 4) * sizeof(fts_atom_t)); /* first two atoms reserved for uploading */
	}
      
      this->ac = ac;
      
      for(i=0; i<ac; i++)
	this->at[i + 4] = at[i]; /* first two atoms reserved for uploading */
    }
  else
    {
      this->s = 0;

      if(this->at)
	fts_block_free(this->at, (this->ac + 4) * sizeof(fts_atom_t));
	  
      this->ac = 0;
    }
}

static void
messevt_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messevt_t *this = (messevt_t *)o;

  this->pos = fts_get_int(at);
  messevt_set_message(o, 0, 0, ac - 1, at + 1);
}

void 
messevt_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messevt_t *this = (messevt_t *)o;

  post("(%d) %s", this->pos, fts_symbol_name(this->s));
  post_atoms(this->ac, this->at + 4);
  post("\n");
}

static void
messevt_save_bmax(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messevt_t *this = (messevt_t *)o;
  fts_bmax_file_t *file = (fts_bmax_file_t *) fts_get_ptr(at);  

  fts_bmax_code_push_symbol(file, this->s);
  fts_bmax_code_push_int(file, this->pos);
  fts_bmax_code_push_symbol(file, seqsym_messevt);
  fts_bmax_code_push_float(file, event_get_time(&this->head));

  fts_bmax_code_obj_mess(file, fts_SystemInlet, seqsym_bmax_add_event, 4);
  fts_bmax_code_pop_args(file, 4);
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

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_save_bmax, messevt_save_bmax);

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
  fts_class_install(seqsym_messevt, messevt_instantiate);
}



