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

/************************************************
 *
 *  messconst
 *
 */
fts_symbol_t s_messconst = 0;
 
typedef struct {
  fts_object_t o;
  int value; /* for blinking */
  fts_expression_t *expression;
  fts_array_t tmp;
  /* inlet values */
  int ac;
  fts_atom_t *at;
} messconst_t;

static fts_metaclass_t *messconst_metaclass;

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

static void
messconst_expression_callback( int ac, const fts_atom_t *at, void *data)
{
  if (fts_is_symbol( at))
    fts_outlet_send( (fts_object_t *)data, 0, fts_get_symbol(at), ac-1, at+1);
  else
    fts_outlet_atoms( (fts_object_t *)data, 0, ac, at);
}

void
messconst_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messconst_t *this = (messconst_t *)o;

  if(this->value == 0)
    {
      /* messbox on */
      this->value = 1;
      fts_update_request(o);
  
      fts_timebase_add_call(fts_get_timebase(), o, messconst_off, 0, MESSCONST_FLASH_TIME);
    }

  fts_expression_reduce( this->expression, this->ac, this->at, messconst_expression_callback, this);
}

static void
messconst_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messconst_t *this = (messconst_t *)o;

  fts_expression_reduce( this->expression, this->ac, this->at, messconst_expression_callback, this);
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

  fts_expression_set( this->expression, ac, at, fts_object_get_patcher( (fts_object_t *)this));
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
messconst_tuple(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messconst_t *this = (messconst_t *)o;
  fts_tuple_t *tuple = (fts_tuple_t *)fts_object_create(fts_tuple_metaclass, ac, at);
  fts_atom_t a;
  
  fts_set_object(&a, (fts_object_t *)tuple);
  fts_atom_assign(this->at + winlet, at);

  if (winlet == 0)
    fts_expression_reduce( this->expression, this->ac, this->at, messconst_expression_callback, this);
}

static void
messconst_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messconst_t *this = (messconst_t *) o;

  if (ac == 1 && s == fts_get_selector( at))
    {
      fts_atom_assign(this->at + winlet, at);

      if (winlet == 0)
	fts_expression_reduce( this->expression, this->ac, this->at, messconst_expression_callback, this);
    }
  else
    fts_object_signal_runtime_error(o, "Don't understand message %s", s);

}

/************************************************
 *
 *  daemons 
 *
 */
 
static void
messconst_get_value(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  messconst_t *this = (messconst_t *)obj;

  fts_set_int(value, this->value);
}

static void
messconst_put_value(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  messconst_t *this = (messconst_t *)obj;

  fts_outlet_bang(obj, 0);

  fts_update_request(obj);
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
  int i;
  
  /* Do we have a new object description (i.e. "ins <INT> outs <INT>") or an old one ? */
  if (ac == 4 
      && fts_is_symbol( at) && fts_get_symbol( at) == fts_s_ninlets
      && fts_is_int( at+1)
      && fts_is_symbol( at+2) && fts_get_symbol( at+2) == fts_s_noutlets
      && fts_is_int( at+3))
    {
      /* If new one, then it gives the number of inlets and outlets */
      ninlets = fts_get_int( at+1);
      noutlets = fts_get_int( at+3);

      fts_object_set_inlets_number(o, ninlets);
      fts_object_set_outlets_number(o, noutlets);
    }
  else
    {
      /* if old one, then we must call the set method by hand, giving as argument the description */
      messconst_set( (fts_object_t *)this, fts_system_inlet, fts_s_set, ac, at);
    }

  this->expression = fts_expression_new( 0, 0, fts_object_get_patcher( (fts_object_t *)this));
  fts_array_init( &this->tmp, 0, 0);

  this->ac = ninlets;
  this->at = (fts_atom_t *)fts_malloc( sizeof( fts_atom_t) * this->ac);

  for ( i = 0; i < this->ac; i++)
    fts_set_int( this->at + i, 0);
}

static void
messconst_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messconst_t *this = (messconst_t *)o;

  fts_expression_delete( this->expression);
  fts_array_destroy( &this->tmp);
  fts_free( this->at);
}

static fts_status_t
messconst_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(messconst_t), 1, 1, 0);
  
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, messconst_init);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_delete, messconst_delete);

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_set, messconst_set);

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_dump, messconst_dump);

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_update_real_time, messconst_update_real_time); 
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_spost_description, messconst_spost_description); 
  
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_bang, messconst_send);
  
  fts_method_define_varargs(cl, 0, fts_s_bang, messconst_bang);
  
  fts_method_define_varargs(cl, 0, fts_s_list, messconst_tuple);
  fts_method_define_varargs(cl, 0, fts_s_anything, messconst_anything);

  return fts_ok;
}

void
messconst_config(void)
{
  s_messconst = fts_new_symbol("messconst");
  messconst_metaclass = fts_class_install(s_messconst, messconst_instantiate);
}
