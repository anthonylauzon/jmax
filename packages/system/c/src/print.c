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

typedef struct 
{
  fts_object_t o;
  fts_bytestream_t *stream;
  fts_symbol_t prompt;
} print_t;

static void
print_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  print_t *this = (print_t *)o;

  if(ac > 0 && fts_is_symbol(at))
    this->prompt = fts_get_symbol(at);
  else
    this->prompt = fts_s_print;

  this->stream = fts_get_default_console_stream();
}

static void
print_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  print_t *this = (print_t *)o;

  fts_object_destroy((fts_object_t *)this->stream);
}

static void
print_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  print_t *this = (print_t *)o;

  fts_spost(this->stream, "%s: bang\n", this->prompt);
}

static void
print_atoms(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  print_t *this = (print_t *)o;

  fts_spost(this->stream, "%s: (", this->prompt);
  fts_spost_atoms(this->stream, ac, at);
  fts_spost(this->stream, ")\n");
}

static void
print_atom(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  print_t *this = (print_t *)o;

  fts_spost(this->stream, "%s: ", this->prompt);
  fts_spost_atoms(this->stream, ac, at);
  fts_spost(this->stream, "\n");
}

static void
print_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  print_t *this = (print_t *)o;

  fts_spost(this->stream, "%s: ", this->prompt);
  
  if(ac == 0)
    fts_spost(this->stream, "%s\n", s);
  else if(ac == 1 && s == fts_get_selector(at))
    {
      if(fts_is_object(at))
	{
	  fts_object_t *obj = fts_get_object(at);	  
	  fts_method_t meth_print = fts_class_get_method(fts_object_get_class(obj), fts_system_inlet, fts_s_print);
	  fts_atom_t a;
	  
	  if(meth_print)
	    {
	      /* print class name and let the object print its content */
	      fts_spost(this->stream, ":%s", s);
	      
	      fts_set_object(&a, this->stream);
	      meth_print(obj, fts_system_inlet, fts_s_print, 1, &a);
	      
	      return;
	    }
	}

      /* simple value or object without print method */
      fts_spost_atoms(this->stream, 1, at);
      fts_spost(this->stream, "\n");
    }
  else
    {
      /* ordinary message */
      fts_spost(this->stream, "%s ", s);
      fts_spost_atoms(this->stream, ac, at);
      fts_spost(this->stream, "\n");
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
  fts_class_init(cl, sizeof(print_t), 1, 0, 0);

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, print_init);

  fts_method_define_varargs(cl, 0, fts_s_bang, print_bang);

  fts_method_define_varargs(cl, 0, fts_s_int, print_atom);
  fts_method_define_varargs(cl, 0, fts_s_float, print_atom);
  fts_method_define_varargs(cl, 0, fts_s_symbol, print_atom);

  fts_method_define_varargs(cl, 0, fts_s_list, print_atoms);

  fts_method_define_varargs(cl, 0, fts_s_anything, print_anything);

  return fts_ok;
}

void
print_config(void)
{
  fts_class_install(fts_s_print, print_instantiate);
}
