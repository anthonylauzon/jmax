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

#include <fts/sys.h>
#include <fts/lang.h>
#include "messP.h"

fts_class_t *inlet_class = 0;
fts_class_t *outlet_class = 0;

#define fts_object_is_outlet(o) (fts_object_get_class(o) == outlet_class)
#define fts_object_is_inlet(o) (fts_object_get_class(o) == inlet_class)

extern void fts_patcher_remove_inlet(fts_patcher_t *patcher, fts_inlet_t *this);
extern void fts_patcher_remove_outlet(fts_patcher_t *patcher, fts_outlet_t *this);
extern void fts_patcher_trim_number_of_inlets(fts_patcher_t *patcher);
extern void fts_patcher_trim_number_of_outlets(fts_patcher_t *patcher);
extern void fts_patcher_inlet_reposition(fts_object_t *o, int pos);
extern void fts_patcher_outlet_reposition(fts_object_t *o, int pos);

typedef struct _send_
{
  fts_object_t head;
  fts_channel_t *channel;
} send_t;

typedef struct _receive_
{
  fts_object_t head;
  fts_channel_t *channel;
} receive_t;

/***************************************************************************
 *
 *  send
 *
 */

static void
send_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  send_t *this = (send_t *) o;

  fts_channel_output_message_from_targets(this->channel, 0, s, ac, at);
}

static void
send_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  send_t *this = (send_t *) o;
  fts_label_t *label = 0;

  if(fts_is_symbol(at + 1))
    {
      fts_symbol_t name = fts_get_symbol(at + 1);
      
      label = fts_label_get(fts_object_get_patcher(o), name);
      fts_variable_add_user(fts_object_get_patcher(o), name, o);
    }
  else if(fts_is_a(at + 1, fts_s_label))
    label = (fts_label_t *)fts_get_object(at + 1);
  else
    {
      fts_object_set_error(o, "Wrong argument");
      return;
    }

  fts_channel_add_origin(fts_label_get_channel(label), (fts_object_t *)this);  
  this->channel = fts_label_get_channel(label);
}

static void
send_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  send_t *this = (send_t *) o;

  fts_channel_remove_origin(this->channel, (fts_object_t *)this);
}

static void
send_find_friends(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  send_t *this = (send_t *)o;

  fts_channel_find_friends(this->channel, ac, at);
}

static void
send_propagate_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  send_t *this = (send_t *)o;
  fts_propagate_fun_t propagate_fun = (fts_propagate_fun_t)fts_get_fun(at + 0);
  void *propagate_context = fts_get_ptr(at + 1);

  fts_channel_propagate_input( this->channel, propagate_fun, propagate_context, 0);
}

static fts_status_t
send_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(send_t), 1, 0, 0); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, send_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, send_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_find_friends, send_find_friends);

  fts_method_define_varargs(cl, 0, fts_s_anything, send_anything);
  fts_class_define_thru(cl, send_propagate_input);

  return fts_Success;
}

/***************************************************************************
 *
 *  receive
 *
 */

static void
receive_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  receive_t *this = (receive_t *)o;
  fts_label_t *label = 0;

  if(fts_is_symbol(at + 1))
    {
      fts_symbol_t name = fts_get_symbol(at + 1);
      
      label = fts_label_get(fts_object_get_patcher(o), name);
      fts_variable_add_user(fts_object_get_patcher(o), name, o);
    }
  else if(fts_is_a(at + 1, fts_s_label))
    label = (fts_label_t *)fts_get_object(at + 1);
  else
    {
      fts_object_set_error(o, "Wrong argument");
      return;
    }

  fts_channel_add_target(fts_label_get_channel(label), o);
  this->channel = fts_label_get_channel(label);
}
  
static void
receive_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  receive_t *this = (receive_t *)o;

  fts_channel_remove_target(this->channel, (fts_object_t *)this);
}

static fts_status_t
receive_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(receive_t), 0, 1, 0); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, receive_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, receive_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_find_friends, send_find_friends);

  return fts_Success;
}

/*************************************************************
 *
 *  inlet class
 *
 *  the inlet has one mandatory argument, the position (inlet number):
 *    -2: the inlet is not assigned (compatibility with the .pat parser) it will be assigned later.
 *    -1: the inlet is assigned to the next unused number in the existing inlet list.
 */

static int 
inlet_get_next_position(fts_patcher_t *patcher, fts_inlet_t *this)
{
  int pos;

  pos = 0;

  while (1)
    {
      int found;
      fts_object_t *p;

      found = 1;

      for (p = patcher->objects; p ; p = p->next_in_patcher)
	if (fts_object_is_inlet(p) && ((fts_inlet_t *)p) != this)
	  if (((fts_inlet_t *) p)->position == pos)
	    found = 0;

      if (found)
	return pos;
      else
	pos++;
    }
}

static void 
inlet_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_inlet_t *this  = (fts_inlet_t *) o;
  fts_patcher_t *patcher = fts_object_get_patcher(o);
  int pos = fts_get_int_arg(ac, at, 1, -1);

  /* Initialize to a non valid value */
  this->position = -1;
  this->next = 0;

  if (pos == -2)
    {
      /* OFF inlets: inlet will be redefined later (.pat parsing) */
      return;
    }
  else if (pos < 0)
    fts_patcher_inlet_reposition(o, inlet_get_next_position(patcher, this));
  else
    fts_patcher_inlet_reposition(o, pos);
}

static void 
inlet_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_inlet_t *this   = (fts_inlet_t *) o;
  fts_patcher_t  *patcher = fts_object_get_patcher(o);

  fts_patcher_remove_inlet(patcher, this);

  if (! patcher->deleted)
    fts_patcher_trim_number_of_inlets(patcher);
}

static void 
inlet_save_dotpat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  FILE *file;
  fts_atom_t xa, ya, wa;

  file = (FILE *)fts_get_ptr( at);

  fts_object_get_prop( o, fts_s_x, &xa);
  fts_object_get_prop( o, fts_s_y, &ya);
  fts_object_get_prop( o, fts_s_width, &wa);

  fprintf( file, "#P inlet %d %d %d;\n", fts_get_int( &xa), fts_get_int( &ya), fts_get_int( &wa));
}

/*************************************************************
 *
 *  inlet/outlet utilities
 *
 */

static int
inout_check(int ac, const fts_atom_t *at)
{
  return (ac == 0 || (ac == 1 && (fts_is_int(at) || fts_is_symbol(at) || fts_is_a(at, fts_s_label))));
}

static fts_status_t
inlet_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  ac--;
  at++;

  if(inout_check(ac, at))
    {
      if(ac == 0 || fts_is_int(at))
	{
	  /* initialize the class */
	  fts_class_init(cl, sizeof(fts_inlet_t),  1, 1, 0);
	  
	  /* define the system methods */
	  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, inlet_init);
	  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, inlet_delete);
	  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_save_dotpat, inlet_save_dotpat); 
	  
	  fts_class_define_thru(cl, 0);

	  return fts_Success;
	}
      else
	return receive_instantiate(cl, ac, at);
    }
      
  return &fts_CannotInstantiate;
}

/*************************************************************
 *
 *  outlet class
 *
 */

/* if outlets are assigned, send the message they receive directly thru the patcher outlet */
static void 
outlet_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac,  const fts_atom_t *at)
{
  fts_outlet_t *this  = (fts_outlet_t *) o;
  fts_patcher_t  *patcher = fts_object_get_patcher(o);

  fts_outlet_send((fts_object_t *)patcher, this->position, s, ac, at);
}

static int 
outlet_get_next_position(fts_patcher_t *patcher, fts_outlet_t *this)
{
  int pos;
  pos = 0;

  while (1)
    {
      int found;
      fts_object_t *p;

      found = 1;

      for (p = patcher->objects; p ; p = p->next_in_patcher)
	if (fts_object_is_outlet(p) && ((fts_outlet_t *) p) != this)
	  if (((fts_outlet_t *) p)->position == pos)
	    found = 0;

      if (found)
	return pos;
      else
	pos++;
    }
}

static void
outlet_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_t *this = (fts_outlet_t *) o;
  fts_patcher_t  *patcher = fts_object_get_patcher(o);
  int pos = fts_get_int_arg(ac, at, 1, -1);

  this->position = -1;      
  this->next = 0;      

  if (pos == -2)
    {
      /* OFF inlets: inlet will be redefined later (.pat parsing) */
      return;
    }
  else if (pos < 0)
    fts_patcher_outlet_reposition(o, outlet_get_next_position(patcher, this));
  else
    fts_patcher_outlet_reposition(o, pos);
}

static void
outlet_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_t *this   = (fts_outlet_t *) o;
  fts_patcher_t  *patcher = fts_object_get_patcher(o);

  fts_patcher_remove_outlet(patcher, this);

  if (! patcher->deleted)
    fts_patcher_trim_number_of_outlets(patcher);
}

static void 
outlet_save_dotpat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  FILE *file;
  fts_atom_t xa, ya, wa;

  file = (FILE *)fts_get_ptr( at);

  fts_object_get_prop( o, fts_s_x, &xa);
  fts_object_get_prop( o, fts_s_y, &ya);
  fts_object_get_prop( o, fts_s_width, &wa);

  fprintf( file, "#P outlet %d %d %d;\n", fts_get_int( &xa), fts_get_int( &ya), fts_get_int( &wa));
}

static void
outlet_propagate_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_t *this  = (fts_outlet_t *)o;
  fts_propagate_fun_t propagate_fun = (fts_propagate_fun_t)fts_get_fun(at + 0);
  void *propagate_context = fts_get_ptr(at + 1);
  fts_patcher_t *patcher;

  patcher = fts_object_get_patcher(this);

  propagate_fun(propagate_context, (fts_object_t *)patcher, this->position);
}

static fts_status_t 
outlet_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  ac--;
  at++;

  if(inout_check(ac, at))
    {
      if(ac == 0 || fts_is_int(at))
	{
	  fts_class_init(cl, sizeof(fts_outlet_t), 1, 1, 0);
	  
	  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, outlet_init);
	  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, outlet_delete);
	  
	  fts_method_define_varargs( cl, fts_SystemInlet, fts_s_save_dotpat, outlet_save_dotpat); 

	  fts_class_define_thru(cl, outlet_propagate_input);
	  
	  fts_method_define_varargs(cl, 0, fts_s_anything, outlet_anything);
	  
	  return fts_Success;
	}
      else
	return send_instantiate(cl, ac, at);
    }
  
  return &fts_CannotInstantiate;
}

static int
inout_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  if((ac0 == 1 || fts_is_int(at0 + 1)) && (ac1 == 1 || fts_is_int(at1 + 1)))
    return 1;
  else if(ac0 == 2 && ac1 == 2 && fts_get_type(at0 + 1) == fts_get_type(at1 + 1))
    return 1;
  else
    return 0;
}

void
fts_inout_config(void)
{
  fts_metaclass_install(fts_s_inlet, inlet_instantiate, inout_equiv);
  fts_metaclass_install(fts_s_outlet, outlet_instantiate, inout_equiv);

  inlet_class = fts_class_get_by_name(fts_s_inlet);
  outlet_class = fts_class_get_by_name(fts_s_outlet);
}
