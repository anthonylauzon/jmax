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

#include <ftsprivate/object.h>
#include <ftsprivate/patcher.h>
#include <ftsprivate/variable.h>

/****************************************************************************
 *
 * "define" object
 *
 */

typedef struct
{
  fts_object_t o;
  fts_symbol_t name;
} define_t;

static void 
define_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  define_t *this = (define_t *) o;  

  if(ac > 1 && fts_is_symbol(at))
    {
      fts_patcher_t *patcher = fts_object_get_patcher(o);

      this->name = fts_get_symbol(at);
      
      /* if the variable already exists in this local context, make a double definition error object  */
      if (0)
	{
	  fts_object_set_error( o, "variable %s doubly defined", this->name);
	  return;
	}
      
      fts_name_define(patcher, this->name, (fts_atom_t *)(at + 1));
    }
  else
    fts_object_set_error(o, "bad arguments");
}

static void 
define_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  define_t *this = (define_t *) o;  

  fts_name_undefine(fts_object_get_patcher( o), this->name);
}

static void
define_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_object_t), define_init, define_delete);
}

void
fts_name_define(fts_patcher_t *patcher, fts_symbol_t name, fts_atom_t *value)
{
  fts_patcher_t *scope = fts_patcher_get_top_level(patcher);
}

void
fts_name_undefine(fts_patcher_t *patcher, fts_symbol_t name)
{
}

void
fts_name_add_listener(fts_patcher_t *patcher, fts_symbol_t name, fts_object_t *obj)
{
}

void
fts_name_remove_listener(fts_patcher_t *patcher, fts_symbol_t name, fts_object_t *obj)
{
}

fts_atom_t *
fts_name_get_value(fts_patcher_t *patcher, fts_symbol_t name)
{
  return NULL;
}

void 
fts_kernel_variable_init(void)
{
  fts_class_install( fts_new_symbol( "define"), define_instantiate);
}
