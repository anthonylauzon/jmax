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
#include "reftype.h"

#define MESSCONST_FLASH_TIME 125.0f

static fts_symbol_t sym_semi = 0;
static fts_symbol_t sym_comma = 0;

#define messconst_symbol_is_separator(s) ((s) == sym_semi || (s) == sym_comma)

#define messconst_symbol_is_atom_type_selector(s) \
( \
  (s) == fts_s_int || \
  (s) == fts_s_float || \
  (s) == fts_s_symbol || \
  (s) == fts_s_object || \
  (s) == fts_s_ptr || \
  (s) == fts_s_atom_array || \
  reftype_get_by_symbol(s) != 0 \
)

/************************************************
 *
 *  single message
 *
 */
 
typedef struct _mess_
{
  fts_symbol_t s;
  int ac;
  fts_atom_t *at;
  struct _mess_ *next;
} mess_t;

static mess_t *
mess_new(fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mess_t *mess = fts_malloc(sizeof(mess_t));
  int i;
  
  mess->s = s;
  
  if(ac > 0)
    {
      mess->ac = ac;
      mess->at = fts_malloc(sizeof(fts_atom_t) * ac);
    }
  else
    {
      mess->ac = 0;
      mess->at = 0;
    }

  for(i=0; i<ac; i++)
    mess->at[i] = at[i];

  mess->next = 0;

  return mess;
}

static void
mess_delete(mess_t * mess)
{
  if(mess->ac)
    fts_free(mess->at);

  fts_free(mess);
}

/************************************************
 *
 *  messconst
 *
 */
 
typedef struct 
{
  fts_object_t o;

  mess_t *mess; /* list of messages */
  
  /* blink when click */
  int value; 
  fts_alarm_t alarm;
} messconst_t;

/************************************************
 *
 *  user methods
 *
 */
 
static void
messconst_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messconst_t *this = (messconst_t *)o;
  mess_t *mess = this->mess;

  this->value = 1;
  fts_object_ui_property_changed(o, fts_s_value);

  fts_alarm_set_delay(&this->alarm, MESSCONST_FLASH_TIME);
  fts_alarm_arm(&this->alarm);

  while(mess)
    {
      fts_outlet_send(o, 0, mess->s, mess->ac, mess->at);
      mess = mess->next;
    }
}

static void
messconst_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messconst_t *this = (messconst_t *)o;
  mess_t *mess = this->mess;

  while(mess)
    {
      fts_outlet_send(o, 0, mess->s, mess->ac, mess->at);
      mess = mess->next;
    }
}

/************************************************
 *
 *  timer
 *
 */
 
static void 
messconst_tick(fts_alarm_t *alarm, void *calldata)
{
  messconst_t *this = (messconst_t *)calldata;

  this->value = 0;
  fts_object_ui_property_changed((fts_object_t *)this, fts_s_value);
}

/************************************************
 *
 *  deamons 
 *
 */
 
/* daemon to get the "value" property */
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

  fts_object_ui_property_changed(obj, fts_s_value);

  fts_alarm_arm(&(this->alarm));
}

static void
messconst_send_properties(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_property_changed(o, fts_s_value);
}

static void
messconst_send_ui_properties(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_ui_property_changed(o, fts_s_value);
}
 
/************************************************
 *
 *    class
 *
 */

static int
messconst_check_message(int ac, const fts_atom_t *at, fts_symbol_t *mess_s, int *mess_ac, const fts_atom_t **mess_at)
{
  if(ac > 0)
    {
      fts_symbol_t s;

      if(ac == 1)
	{
	  if(fts_is_atom_array(at))
	    {
	      /* list format: "{" [<value> ...] "}" (mandatory braces!) */
	      fts_atom_array_t *aa = fts_get_atom_array(at);

	      *mess_s = fts_s_list;
	      *mess_ac = fts_atom_array_get_size(aa);
	      *mess_at = fts_atom_array_get_ptr(aa);

	      return 1;
	    }
	  else if(!fts_is_symbol(at))
	    {
	      /* value format: <value> (without type specifyer - selector is added from atom type) */
	      
	      *mess_s = fts_get_selector(at);
	      *mess_ac = 1;
	      *mess_at = at;
	      
	      return 1;
	    }
	}
      else if(ac == 2 && fts_is_symbol(at))
	{
	  /* value format: <type specifyer> <value> (type specifyer must match atom type) */
	  s = fts_get_symbol(at);
	  
	  if(s == fts_s_atom_array || s == fts_s_object || s == fts_s_ptr) /* forbiddden type indentifiers */
	    return 0;
	  else if(s == fts_get_selector(at + 1))
	    {
	      *mess_s = s;
	      *mess_ac = 1;
	      *mess_at = at + 1;

	      return 1;
	    }
	}
      else if(!fts_is_symbol(at))
	return 0;

      /* message format: <selector> [<value> ...] (any message - type specifyers are not allowed as selectors) */
      s = fts_get_symbol(at);

      if(messconst_symbol_is_atom_type_selector(s) || s == fts_s_list)
	return 0;
      else
	{
	  *mess_s = s;
	  *mess_ac = ac - 1;
	  *mess_at = at + 1;
	  
	  return 1;
	}
    }
  
  return 0;
}

static int
messconst_check(int ac, const fts_atom_t *at)
{
  if(ac > 0)
    {
      fts_symbol_t mess_s;
      int mess_ac;
      const fts_atom_t *mess_at;
      int begin = 0;
      int i;
      
      for(i=0; i<ac; i++)
	{
	  if(fts_is_symbol(at + i) && messconst_symbol_is_separator(fts_get_symbol(at + i)))
	    {
	      if(!messconst_check_message(i - begin, at + begin, &mess_s, &mess_ac, &mess_at))
		return 0;
	      
	      begin = i + 1;
	    }
	}
      
      if(i > begin)
	{
	  if(!messconst_check_message(i - begin, at + begin, &mess_s, &mess_ac, &mess_at))
	    return 0;
	}
      
      return 1;
    }
  
  return 0;
}

static void
messconst_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messconst_t *this = (messconst_t *)o;
  mess_t **append = &this->mess;
  mess_t *mess;
  fts_symbol_t mess_s;
  int mess_ac;
  const fts_atom_t *mess_at;
  int begin = 1;
  int i;

  this->mess = 0;
  this->value = 0;
  fts_alarm_init(&(this->alarm), 0, messconst_tick, this);

  for(i=1; i<ac; i++)
    {
      if(fts_is_symbol(at + i) && messconst_symbol_is_separator(fts_get_symbol(at + i)))
	{
	  messconst_check_message(i - begin, at + begin, &mess_s, &mess_ac, &mess_at);

	  *append = mess_new(mess_s, mess_ac, mess_at);
	  append = &((*append)->next);

	  begin = i + 1;
	}
    }

  if(i > begin)
    {
      messconst_check_message(i - begin, at + begin, &mess_s, &mess_ac, &mess_at);
      *append = mess_new(mess_s, mess_ac, mess_at);
    }  
}

static void
messconst_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messconst_t *this = (messconst_t *)o;
  mess_t *mess = this->mess;

  while(mess)
    {
      mess_t *next = mess->next;

      mess_delete(mess);

      mess = next;
    }

  fts_alarm_unarm(&(this->alarm));
}

static fts_status_t
messconst_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(messconst_check(ac - 1, at + 1))
    {
      fts_class_init(cl, sizeof(messconst_t), 1, 1, 0);
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, messconst_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, messconst_delete);
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_send_properties, messconst_send_properties); 
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_send_ui_properties, messconst_send_ui_properties); 
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_bang, messconst_send);
      
      fts_method_define_varargs(cl, 0, fts_s_bang, messconst_bang);
      
      /* value daemons */
      fts_class_add_daemon(cl, obj_property_get, fts_s_value, messconst_get_value);
      fts_class_add_daemon(cl, obj_property_put, fts_s_value, messconst_put_value);

      return fts_Success;
  }
  
  return &fts_CannotInstantiate;;
}

int
messconst_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  return messconst_check(ac1 - 1, at1 + 1);
}

void
messconst_config(void)
{
  sym_semi = fts_new_symbol(";");
  sym_comma = fts_new_symbol(",");
  
  fts_metaclass_install(fts_new_symbol("messconst"), messconst_instantiate, messconst_equiv);
}
