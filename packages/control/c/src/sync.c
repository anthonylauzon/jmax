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

static fts_symbol_t sym_all = 0;
static fts_symbol_t sym_any = 0;

typedef struct 
{
  fts_object_t o;
  int n;
  unsigned long bits;
  fts_atom_t a[32];
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
    {
      if(!fts_is_void(this->a + i))
	fts_outlet_send((fts_object_t *)this, i, fts_get_selector(this->a + i), 1, this->a + i);
    }
}

static void
sync_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sync_t *this = (sync_t *)o;

  fts_atom_assign(this->a + winlet, at);

  if(this->mode == mode_all)
    {
      this->bits &= ~(1 << winlet);

      if(this->bits == 0)
	{
	  sync_output(this);
	  this->bits = (1 << this->n) - 1;
	}
    }
  else if(this->bits & (1 << winlet))
    sync_output(this);
}

static void
sync_set_mode(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sync_t *this = (sync_t *)o;

  if(fts_is_symbol(at))
    {
      fts_symbol_t mode = fts_get_symbol(at);
      
      if(mode == sym_all)
	{
	  this->mode = mode_all;
	  this->bits = 0;
	}
      else if(mode == sym_any)
	{
	  this->mode = mode_select;
	  this->bits = 0xffffffff;
	}
    }
  else if(fts_is_number(at))
    {
      int in = fts_get_number_int(at);

      if(in < 0)
	in = 0;
      else if(in > this->n)
	in = this->n;

      this->mode = mode_select;
      this->bits = 1 << in;
    }
  else if(fts_is_list(at))
    {
      fts_list_t *l = fts_get_list(at);
      fts_atom_t *a = fts_list_get_ptr(l);
      int size = fts_list_get_size(l);
      int i;

      this->mode = mode_select;
      this->bits = 0;

      for(i=0; i<size; i++)
	{
	  if(fts_is_number(a + i))
	    {
	      int in = fts_get_number_int(a + i);
	      
	      if(in < 0)
		in = 0;
	      else if(i > this->n)
		in = this->n;
	      
	      this->bits += 1 << in;
	    }  
	}
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

  if(ac <= 1)
    {
      if(ac == 1 && fts_is_number(at))
	n = fts_get_number_int(at);
      
      if(n < 2) 
	n = 2;
      else if(n > 32)
	n = 32;

      for(i=0; i<n; i++)
	fts_set_void(this->a + i);
    }
  else if(ac > 1)
    {
      n = ac;

      for(i=0; i<n; i++)
	{
	  fts_set_void(this->a + i);
	  fts_atom_assign(this->a + i, at + i);
	}
    }
  else
    fts_object_set_error(o, "Wrong arguments");

  this->n = n;
  this->bits = (1 << n) - 1;
  this->mode = mode_all;
}

static fts_status_t
sync_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int n = 0;
  int i;

  ac--;
  at++;

  if(ac > 0 && fts_is_number(at))
    n = fts_get_number_int(at);

  if(n < 2) 
    n = 2;
  else if(n > 32)
    n = 32;

  fts_class_init(cl, sizeof(sync_t), n, n, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, sync_init);

  for(i=0; i<n; i++)
    fts_method_define_varargs(cl, i, fts_s_anything, sync_input);
  
  return fts_Success;
}

void
sync_config(void)
{
  sym_all = fts_new_symbol("all");
  sym_any = fts_new_symbol("any");

  fts_metaclass_install(fts_new_symbol("sync"), sync_instantiate, fts_first_arg_equiv);
}
