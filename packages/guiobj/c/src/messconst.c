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

#define MESSCONST_FLASH_TIME 125.0f

typedef struct {
  fts_object_t o;
  int value; /* for blinking */
  fts_expression_t *expression;
  fts_array_t tmp;
  /* inlet values */
  int ac;
  fts_atom_t *at;
} messconst_t;

/************************************************
 *
 *  user methods
 *
 */
 
static void 
messconst_off(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messconst_t *this = (messconst_t *)o;

  this->value = 0;

  fts_update_request((fts_object_t *)this);
}

static fts_status_t
messconst_expression_callback( int ac, const fts_atom_t *at, void *data)
{
  if (ac > 0)
    {
      if (fts_is_symbol( at))
	fts_outlet_send( (fts_object_t *)data, 0, fts_get_symbol(at), ac-1, at+1);
      else
	fts_outlet_varargs( (fts_object_t *)data, 0, ac, at);
    }

  return fts_ok;
}

static void
messconst_eval( messconst_t *this)
{
  fts_patcher_t *patcher = fts_object_get_patcher( (fts_object_t *)this);
  fts_status_t status;

  status = fts_expression_reduce( this->expression, patcher, this->ac, this->at, messconst_expression_callback, this);

  if (status != fts_ok)
    fts_object_signal_runtime_error( (fts_object_t *)this, "%s", fts_status_get_description( status));
}


static void
messconst_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messconst_t *this = (messconst_t *)o;

  if(winlet < 0 && this->value == 0)
    {
      /* messbox on */
      this->value = 1;
      fts_update_request(o);
  
      fts_timebase_add_call(fts_get_timebase(), o, messconst_off, 0, MESSCONST_FLASH_TIME);
    }

  messconst_eval( this);
}

static void
messconst_spost_description(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messconst_t *this = (messconst_t *) o;

  fts_spost_object_description_args( (fts_bytestream_t *)fts_get_object(at), fts_array_get_size( &this->tmp), fts_array_get_atoms( &this->tmp));
}

static void
messconst_update_real_time(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messconst_t *this = (messconst_t *) o;
  fts_atom_t a;

  fts_set_int( &a, this->value);  
  fts_client_send_message_real_time(o, fts_s_value, 1, &a);
}
 
static void 
messconst_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messconst_t *this = (messconst_t *) o;
  int ninlets;
  fts_status_t status;

  status = fts_expression_set( this->expression, ac, at);
  if (status != fts_ok)
    {
      fts_object_signal_runtime_error( o, "%s", fts_status_get_description( status));
      return;
    }

  fts_array_init( &this->tmp, ac, at);

  ninlets = fts_expression_get_env_count( this->expression);
  if (ninlets == 0)
    ninlets = 1;

  fts_object_set_inlets_number(o, ninlets);

  if (ninlets != this->ac)
    {
      int i;

      fts_free( this->at);

      this->ac = ninlets;
      this->at = (fts_atom_t *)fts_malloc( sizeof( fts_atom_t) * this->ac);

      for ( i = 0; i < this->ac; i++)
	fts_set_int( this->at + i, 0);
    }

  if (fts_object_has_id((fts_object_t *)this))
    {
      fts_atom_t a;

      fts_set_int(&a, ninlets);
      fts_client_send_message(o, fts_s_n_inlets, 1, &a);
    }
}

static void
messconst_dump(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messconst_t *this = (messconst_t *) o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  fts_message_t *mess;

  mess = fts_dumper_message_new( dumper, fts_s_set);
  fts_message_append( mess, fts_array_get_size( &this->tmp), fts_array_get_atoms( &this->tmp));
  fts_dumper_message_send( dumper, mess);
}

static void
messconst_varargs(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messconst_t *this = (messconst_t *) o;

  if(ac == 1)
    fts_atom_assign(this->at + winlet, at);
  else
    {
      fts_object_t *tuple = fts_object_create(fts_tuple_class, NULL, ac, at);
      fts_atom_t a;
      
      fts_set_object(&a, tuple);
      fts_atom_assign(this->at + winlet, &a);
    }

  if (winlet == 0)
    messconst_eval( this);
}

/************************************************
 *
 *    class
 *
 */

static void
messconst_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messconst_t *this = (messconst_t *)o;
  int ninlets = 1;
  int noutlets = 1;
  int new = 0;
  int i;
  fts_status_t status;
  
  /* Do we have a new object description (i.e. "ins <INT> outs <INT>") or an old one ? */
  if (ac == 4 
      && fts_is_symbol( at) && fts_get_symbol( at) == fts_s_n_inlets
      && fts_is_int( at+1)
      && fts_is_symbol( at+2) && fts_get_symbol( at+2) == fts_s_n_outlets
      && fts_is_int( at+3))
    {
      /* If new one, then it gives the number of inlets and outlets */
      ninlets = fts_get_int( at+1);
      noutlets = fts_get_int( at+3);

      fts_object_set_inlets_number(o, ninlets);
      fts_object_set_outlets_number(o, noutlets);

      new = 1;
    }

  status = fts_expression_new( 0, 0, &this->expression);
  if (status != fts_ok)
    {
      fts_object_set_error( o, "%s", fts_status_get_description( status));
      return;
    }

  fts_array_init( &this->tmp, 0, 0);

  this->ac = ninlets;
  this->at = (fts_atom_t *)fts_malloc( sizeof( fts_atom_t) * this->ac);

  for ( i = 0; i < this->ac; i++)
    fts_set_int( this->at + i, 0);

  /* if old one, then we must call the set method by hand, giving as argument the description */
  if(!new)
    messconst_set( (fts_object_t *)this, fts_system_inlet, fts_s_set, ac, at);
}

static void
messconst_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messconst_t *this = (messconst_t *)o;

  fts_expression_delete( this->expression);
  fts_array_destroy( &this->tmp);
  fts_free( this->at);
}

static void
messconst_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(messconst_t), messconst_init, messconst_delete);

  fts_class_message_varargs(cl, fts_s_set, messconst_set);
  fts_class_message_varargs(cl, fts_s_dump, messconst_dump);

  fts_class_message_varargs(cl, fts_s_update_real_time, messconst_update_real_time); 
  fts_class_message_varargs(cl, fts_s_spost_description, messconst_spost_description); 
  
  fts_class_message_varargs(cl, fts_s_bang, messconst_send);
  
  fts_class_inlet_varargs(cl, 0, messconst_varargs);
  fts_class_outlet_anything(cl, 0);
}

void
messconst_config(void)
{
  fts_class_install( fts_new_symbol("messconst"), messconst_instantiate);
}
