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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell, Miller Puckette.
 *
 */

#include <string.h>
#include <fts/fts.h>

typedef struct {
  fts_object_t o;
  fts_symbol_t prompt;
} print_t;

static fts_symbol_t sym_print = 0;

/**********************************************************************
 *
 *  object
 *
 */

static void
print_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  print_t *this = (print_t *)o;

  this->prompt = fts_get_symbol_arg(ac, at, 1, fts_new_symbol("print"));
}


/**********************************************************************
 *
 *  user methods
 *
 */

static void
print_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  print_t *this = (print_t *)o;

  post("%s: bang\n", fts_symbol_name(this->prompt));
}


static void
print_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  print_t *this = (print_t *)o;

  post("%s: {", fts_symbol_name(this->prompt));
  post_atoms(ac, at);
  post("}\n");
}

static void
print_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  print_t *this = (print_t *)o;

  post("%s: ", fts_symbol_name(this->prompt));
  
  if(ac == 0)
    {
      post_symbol(s);
      post("\n");
    }
  else if(ac == 1 && fts_is_object(at))
    {
      fts_object_t *obj = fts_get_object(at);

      if(s == fts_object_get_class_name(obj))
	{
	  post_symbol(s);
	  post(" ");

	  if(fts_send_message(obj, fts_SystemInlet, fts_s_print, 0, 0) != fts_Success)
	    post("???");
      
	  post("\n");
	}
    }
  else if(ac == 1 && s == fts_get_selector(at))
    {
      post_atoms(1, at);
      post("\n");
    }
  else
    {
      post_symbol(s);
      post(" ");
      post_atoms(ac, at);
      post("\n");
    }
}

/**********************************************************************
 *
 *  class
 *
 */

static fts_status_t
print_instantiate(fts_class_t *cl, int ac, const fts_atom_t *aat)
{
  fts_symbol_t a[2];

  fts_class_init(cl, sizeof(print_t), 1, 0, 0);

  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, print_init, 2, a, 1);

  fts_method_define_varargs(cl, 0, fts_s_bang, print_bang);
  fts_method_define_varargs(cl, 0, fts_s_list, print_list);
  fts_method_define_varargs(cl, 0, fts_s_anything, print_anything);

  return fts_Success;
}

void
print_config(void)
{
  fts_class_install(fts_new_symbol("print"), print_instantiate);
}
