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
#include "channel.h"

static fts_class_t *label_class = 0;

fts_metaclass_t *inlet_metaclass = 0;
fts_metaclass_t *outlet_metaclass = 0;

#define fts_object_is_outlet(o) ((o)->head.cl->mcl == outlet_metaclass)
#define fts_object_is_inlet(o) ((o)->head.cl->mcl == inlet_metaclass)

extern void fts_patcher_remove_inlet(fts_patcher_t *patcher, fts_inlet_t *this);
extern void fts_patcher_remove_outlet(fts_patcher_t *patcher, fts_outlet_t *this);
extern void fts_patcher_trim_number_of_inlets(fts_patcher_t *patcher);
extern void fts_patcher_trim_number_of_outlets(fts_patcher_t *patcher);
extern void fts_patcher_inlet_reposition(fts_object_t *o, int pos);
extern void fts_patcher_outlet_reposition(fts_object_t *o, int pos);

/*************************************************************
 *
 *  label
 *
 */

typedef fts_channel_t label_t;

static void
label_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_channel_init((fts_channel_t *)o);
}

static void
label_get_state(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  fts_set_object_with_type(value, obj, fts_s_label);
}

static void
label_find_friends(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_channel_find_friends((fts_channel_t *)o, ac, at);
}

static fts_status_t
label_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(label_t), 0, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, label_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_find_friends, label_find_friends);
  fts_class_add_daemon(cl, obj_property_get, fts_s_state, label_get_state);

  return fts_Success;
}

/***************************************************************************
 *
 *  send
 *
 */

static void
send_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_access_t *this = (fts_access_t *) o;

  fts_channel_output_message_from_targets(fts_access_get_channel(this), s, ac, at);
}

static void
send_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_access_t *this = (fts_access_t *) o;
  label_t *label = 0;

  if(fts_is_symbol(at + 1))
    {
      fts_symbol_t name = fts_get_symbol(at + 1);
      
      label = (label_t *)fts_variable_get_object_always(fts_object_get_patcher(o), name, label_class);
      
      fts_variable_add_user(fts_object_get_patcher(o), name, o);
    }
  else if(fts_is_a(at + 1, fts_s_label))
    label = (label_t *)fts_get_object(at + 1);

  fts_object_refer((fts_object_t *)label);
  fts_channel_add_origin((fts_channel_t *)label, this);
}

static void
send_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_access_t *this = (fts_access_t *) o;

  fts_channel_remove_origin(fts_access_get_channel(this), this);
  fts_object_release((fts_object_t *)fts_access_get_channel(this));
}

static void
send_find_friends(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_access_t *this = (fts_access_t *)o;

  fts_channel_find_friends(fts_access_get_channel(this), ac, at);
}

static fts_status_t
send_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(fts_access_t), 1, 0, 0); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, send_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, send_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_find_friends, send_find_friends);

  fts_method_define_varargs(cl, 0, fts_s_anything, send_anything);

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
  fts_access_t *this = (fts_access_t *) o;
  label_t *label = 0;

  if(fts_is_symbol(at + 1))
    {
      fts_symbol_t name = fts_get_symbol(at + 1);
      
      label = (label_t *)fts_variable_get_object_always(fts_object_get_patcher(o), name, label_class);
      
      fts_variable_add_user(fts_object_get_patcher(o), name, o);
    }
  else if(fts_is_a(at + 1, fts_s_label))
    label = (label_t *)fts_get_object(at + 1);

  fts_object_refer((fts_object_t *)label);
  fts_channel_add_target(label, this);
}
  
static void
receive_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_access_t *this = (fts_access_t *) o;

  fts_channel_remove_target(fts_access_get_channel(this), this);

  fts_object_release((fts_object_t *)fts_access_get_channel(this));
}

static fts_status_t
receive_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(fts_access_t), 0, 1, 0); 

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
  int pos = fts_get_int_arg(ac, at, 1, 0);

  /* Initialize to a non valid value */
  this->position = -1;
  this->next = 0;

  if (pos == -2)
    {
      /* OFF inlets: inlet will be redefined later (.pat parsing) */
      return;
    }
  else if (pos == -1)
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
  return (ac == 1 && (fts_is_int(at) || fts_is_symbol(at) || fts_is_a(at, fts_s_label)));
}

static fts_status_t
inlet_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  ac--;
  at++;

  if(inout_check(ac, at))
    {
      if(fts_is_int(at))
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
  int pos = fts_get_int_arg(ac, at, 1, 0);

  this->position = -1;      
  this->next = 0;      

  if (pos == -2)
    {
      /* OFF inlets: inlet will be redefined later (.pat parsing) */
      return;
    }
  else if (pos == -1)
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
      if(fts_is_int(at))
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
  if(inout_check(ac1 - 1, at1 + 1))
    return (fts_get_type(at0 + 1) == fts_get_type(at1 + 1));
  else
    return 0;
}

void
fts_inout_config(void)
{
  fts_metaclass_install(fts_s_inlet, inlet_instantiate, inout_equiv);
  fts_metaclass_install(fts_s_outlet, outlet_instantiate, inout_equiv);

  inlet_metaclass = fts_metaclass_get_by_name(fts_s_inlet);
  outlet_metaclass = fts_metaclass_get_by_name(fts_s_outlet);

  fts_class_install(fts_s_label, label_instantiate);

  label_class = fts_class_get_by_name(fts_s_label);
}
