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
print_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  print_t *this = (print_t *)o;

  if(this->prompt)
    fts_spost(this->stream, "%s: ", this->prompt);
  
  if(s != NULL)
    {
      /* ordinary message */
      fts_spost_symbol(this->stream, s);
      fts_spost(this->stream, " ");
    }

  if(ac == 1 && !fts_is_symbol(at))
    {
      /* single argument */
      if(fts_is_object(at))
	{
	  fts_object_t *obj = fts_get_object(at);	  
	  fts_method_t meth = fts_class_get_method_varargs(fts_object_get_class(obj), fts_s_print);
	  fts_atom_t a;
	  
	  if(meth)
	    {
	      fts_set_object(&a, this->stream);
	      meth(obj, 0, 0, 1, &a);
	      return;
	    }
	  
	  meth = fts_class_get_method_varargs(fts_object_get_class(obj), fts_s_post);
	  
	  if(meth)
	    {
	      fts_set_object(&a, this->stream);
	      meth(obj, 0, 0, 1, &a);
	      fts_spost(this->stream, "\n");
	      return;
	    }
	}
      
      /* simple value or object without print method */
      fts_spost_atoms(this->stream, 1, at);
      fts_spost(this->stream, "\n");
    }
  else if(ac > 0)
  {
    /* multiple args */
    if(s == NULL)
      fts_spost(this->stream, "(", this->prompt);

    fts_spost_atoms(this->stream, ac, at);

    if(s == NULL)
      fts_spost(this->stream, ")\n");
  }
  else if(s == NULL)
    fts_spost(this->stream, "<bang>\n");
}

/**********************************************************************
 *
 *  class
 *
 */

static void
print_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  print_t *this = (print_t *)o;

  this->stream = fts_get_default_console_stream();
  this->prompt = NULL;

  if(ac > 0 && fts_is_symbol(at))
    this->prompt = fts_get_symbol(at);
}

static void
print_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(print_t), print_init, 0);

  fts_class_input_handler(cl, print_input);
}

void
print_config(void)
{
  fts_class_install(fts_s_print, print_instantiate);
}
