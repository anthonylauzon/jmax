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

#define SYNC_MAX_SIZE (sizeof(unsigned int))

static fts_symbol_t sym_all = 0;
static fts_symbol_t sym_none = 0;

static fts_symbol_t sym_any = 0;
static fts_symbol_t sym_left = 0;
static fts_symbol_t sym_right = 0;

typedef struct 
{
  fts_object_t o;
  int n;
  unsigned int trigger; /* control bits: trigger on input at given inlets */
  unsigned int require; /* control bits: require input on given inlets */
  unsigned int reset; /* control bits: reset memory of given inputs after on each input */
  unsigned int wait; /* status bits: wait for input at given inlet before output */
  fts_atom_t a[SYNC_MAX_SIZE];
  enum {mode_all, mode_select} mode;
} sync_t;

/************************************************************
 *
 *  user methods
 *
 */

static void
sync_output(sync_t *this)
{
  int i;

  for(i=this->n-1; i>=0; i--)
    if(!fts_is_void(this->a + i))
      fts_outlet_send((fts_object_t *)this, i, fts_get_selector(this->a + i), 1, this->a + i);
}

static void
sync_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sync_t *this = (sync_t *)o;
  
  if(ac)
    {
      unsigned int bit = 1 << winlet;
      
      fts_atom_assign(this->a + winlet, at);
      
      this->wait &= ~bit;
      
      if(!this->wait && (this->trigger & bit))
	{
	  sync_output(this);
	  this->wait |= this->reset & this->require;
	}
    }
}

static void
sync_set_bits(unsigned int *bits, int n, const fts_atom_t *at, int sign)
{
  if(fts_is_symbol(at))
    {
      fts_symbol_t mode = fts_get_symbol(at);
      
      if(mode == sym_all)
	*bits = (1 << n) - 1;
      else if(mode == sym_none)
	*bits = 0;
    }
  else if(fts_is_number(at))
    {
      int in = fts_get_number_int(at) * sign;

      if(in >= 0 && in < n)
	*bits = 1 << in;
    }
  else if(fts_is_list(at))
    {
      fts_array_t *l = fts_get_array(at);
      fts_atom_t *a = fts_array_get_atoms(l);
      int size = fts_array_get_size(l);
      int i;

      *bits = 0;

      for(i=0; i<size; i++)
	{
	  if(fts_is_number(a + i))
	    {
	      int in = fts_get_number_int(a + i) * sign;
	      
	      if(in >= 0 && in < n)
		*bits |= 1 << in;
	    }  
	}
    }
}

static void
sync_set_trigger(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sync_t *this = (sync_t *)o;

  sync_set_bits(&this->trigger, this->n, at, 1);
}

static void
sync_set_trigger_prop(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  sync_set_trigger(o, 0, 0, 1, value);
}

static void
sync_set_require(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sync_t *this = (sync_t *)o;
  unsigned int once = 0;

  sync_set_bits(&this->require, this->n, at, 1);
  sync_set_bits(&once, this->n, at, -1);

  this->reset = ~once;
  this->wait = this->require | once;
}

static void
sync_set_require_prop(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  sync_set_require(o, 0, 0, 1, value);
}

static void
sync_set_mode(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sync_t *this = (sync_t *)o;

  if(fts_is_symbol(at))
    {
      fts_symbol_t mode = fts_get_symbol(at);
      
      if(mode == sym_any)
	{
	  this->trigger = (1 << this->n) - 1;
	  this->reset = 0;
	  this->require = 0;
	}
      else if(mode == sym_left)
	{
	  this->trigger = 1;
	  this->reset = 0;
	  this->require = 0;
	}
      else if(mode == sym_right)
	{
	  this->trigger = (1 << (this->n - 1));
	  this->reset = 0;
	  this->require = 0;
	}

      this->wait = this->require;
    }
}

static void
sync_set_mode_prop(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  sync_set_mode(o, 0, 0, 1, value);
}

/************************************************************
 *
 *  class
 *
 */
static void
sync_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  sync_t *this = (sync_t *)o;
  int n = 0;
  int i;

  ac--;
  at++;

  /* void state */
  for(i=0; i<SYNC_MAX_SIZE; i++)
    fts_set_void(this->a + i);

  if(ac == 1)
    {
      if(fts_is_number(at))
	{
	  n = fts_get_number_int(at);
	  
	  if(n < 2) 
	    n = 2;
	  else if(n > SYNC_MAX_SIZE)
	    n = SYNC_MAX_SIZE;
	}
      else
	{
	  fts_object_set_error(o, "Wrong argument");
	  return;
	}
    }
  else if(ac > 1)
    {
      if(ac > SYNC_MAX_SIZE)
	ac = SYNC_MAX_SIZE;

      n = ac;

      for(i=0; i<n; i++)
	fts_atom_assign(this->a + i, at + i);
    }

  this->n = n;
  this->trigger = this->require = this->reset = this->wait = (1 << n) - 1;
}

static fts_status_t
sync_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int n = 0;
  int i;

  ac--;
  at++;

  if(ac == 1 && fts_is_number(at))
    n = fts_get_number_int(at);
  else if(ac > 1)
    n = ac;

  if(n < 2) 
    n = 2;
  else if(n > SYNC_MAX_SIZE)
    n = SYNC_MAX_SIZE;

  fts_class_init(cl, sizeof(sync_t), n, n, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, sync_init);
  fts_class_add_daemon(cl, obj_property_put, fts_new_symbol("trigger"), sync_set_trigger_prop);
  fts_class_add_daemon(cl, obj_property_put, fts_new_symbol("require"), sync_set_require_prop);
  fts_class_add_daemon(cl, obj_property_put, fts_new_symbol("mode"), sync_set_mode_prop);
      
  for(i=0; i<n; i++)
    fts_method_define_varargs(cl, i, fts_s_anything, sync_input);
  
  return fts_Success;
}

void
sync_config(void)
{
  sym_all = fts_new_symbol("all");
  sym_none = fts_new_symbol("none");
  sym_any = fts_new_symbol("any");
  sym_left = fts_new_symbol("left");
  sym_right = fts_new_symbol("right");

  fts_metaclass_install(fts_new_symbol("sync"), sync_instantiate, fts_narg_equiv);
}