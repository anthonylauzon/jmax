/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#include <fts/fts.h>

typedef struct
{
  fts_object_t o;
  int value;
} toggle_t;


static void 
toggle_update_real_time(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  toggle_t *this = (toggle_t *) o;
  fts_atom_t a;

  fts_set_int( &a, this->value);  
  fts_client_send_message_real_time(o, fts_s_value, 1, &a);
}

static void 
toggle_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  toggle_t *this = (toggle_t *) o;

  if(ac > 0 && fts_is_number(at))
    {
      int n = (fts_get_number_int(at) != 0);

      if(this->value != n)
	{
	  this->value = n;
	  
	  fts_update_request(o);
	}
    }
}

static void 
toggle_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  toggle_t *this = (toggle_t *) o;

  toggle_set(o, 0, 0, 1, at);

  fts_outlet_int(o, 0, this->value);
}

static void 
toggle_toggle(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  toggle_t *this = (toggle_t *) o;

  this->value = !this->value;

  fts_update_request(o);

  fts_outlet_int(o, 0, this->value);
}

static void 
toggle_save_dotpat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  FILE *file;
  fts_atom_t xa, ya, wa;

  file = (FILE *)fts_get_pointer( at);

  fts_object_get_prop( o, fts_s_x, &xa);
  fts_object_get_prop( o, fts_s_y, &ya);
  fts_object_get_prop( o, fts_s_width, &wa);

  fprintf( file, "#P toggle %d %d %d;\n", fts_get_int( &xa), fts_get_int( &ya), fts_get_int( &wa) );
}

static void 
toggle_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(toggle_t), 0, 0);

  fts_class_message_varargs(cl, fts_s_update_real_time, toggle_update_real_time); 
  fts_class_message_varargs(cl, fts_s_save_dotpat, toggle_save_dotpat); 

  fts_class_message_varargs(cl, fts_s_set, toggle_toggle);

  fts_class_message_varargs(cl, fts_new_symbol("click"), toggle_toggle);

  fts_class_inlet_bang(cl, 0, toggle_toggle);
  fts_class_inlet_number(cl, 0, toggle_number);
  fts_class_inlet_varargs(cl, 0, toggle_number);

  fts_class_outlet_int(cl, 0);
}

void 
toggle_config(void)
{
  fts_class_install(fts_new_symbol("toggle"),toggle_instantiate);
}
