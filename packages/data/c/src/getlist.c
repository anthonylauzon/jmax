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
#include "preset.h"
#include "messtab.h"

typedef struct 
{
  fts_object_t o;
  fts_array_t list; /* ouput buffer */
} getlist_t;

/************************************************
 *
 *    object
 *
 */
 
static void
getlist_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getlist_t *this = (getlist_t *)o;

  fts_array_init(&this->list, 0, 0);
}

static void
getlist_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getlist_t *this = (getlist_t *)o;

  fts_array_destroy(&this->list);
}

/************************************************
 *
 *  user methods
 *
 */

static void
getlist_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if(fts_is_object(at))
    {
      getlist_t *this = (getlist_t *)o;
      fts_object_t *input = fts_get_object(at);
      fts_class_t *class = fts_object_get_class(input);
      fts_method_t method = fts_class_get_method(class, fts_SystemInlet, fts_s_append_state_to_array);

      if(method)
	{
	  fts_atom_t a;
	  
	  fts_array_set_size(&this->list, 0);
	  
	  /* get object state as array */
	  fts_set_array(&a, &this->list);
	  fts_message_send(input, fts_SystemInlet, fts_s_append_state_to_array, 1, &a);
	  
	  fts_outlet_send(o, 0, fts_s_list, fts_array_get_size(&this->list), fts_array_get_atoms(&this->list));
	}
    }
}

static void
getlist_preset(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getlist_t *this = (getlist_t *)o;
  preset_t *preset = preset_atom_get(at);
  int size = 0;
  int i;

  fts_array_set_size(&this->list, 0);
  preset_get_keys(preset, &this->list);

  fts_outlet_send(o, 0, fts_s_list, fts_array_get_size(&this->list), fts_array_get_atoms(&this->list));
}

static void
getlist_messtab(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getlist_t *this = (getlist_t *)o;
  messtab_t *messtab = messtab_atom_get(at);
  int size = 0;
  int i;

  fts_array_set_size(&this->list, 0);
  messtab_get_keys(messtab, &this->list);

  fts_outlet_send(o, 0, fts_s_list, fts_array_get_size(&this->list), fts_array_get_atoms(&this->list));
}

/************************************************
 *
 *    class
 *
 */

static fts_status_t
getlist_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(getlist_t), 1, 1, 0); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, getlist_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, getlist_delete);
  
  fts_method_define_varargs(cl, 0, fts_s_anything, getlist_input);
  fts_method_define_varargs(cl, 0, preset_type, getlist_preset);
  fts_method_define_varargs(cl, 0, messtab_type, getlist_messtab);
  
  fts_outlet_type_define_varargs(cl, 0, fts_s_list);
  
  return fts_Success;
}

void
getlist_config(void)
{
  fts_class_install(fts_new_symbol("getlist"), getlist_instantiate);
}
