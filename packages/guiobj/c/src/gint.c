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

typedef struct 
{
  fts_object_t o;
  int value;
} gint_t;


static void
gint_update_real_time(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  gint_t *this = (gint_t *)o;
  fts_atom_t a;

  fts_set_int( &a, this->value);  
  fts_client_send_message_real_time(o, fts_s_value, 1, &a);
}

static void
gint_update(gint_t *this, int n)
{
  if (this->value != n)
    {
      this->value = n;
      fts_update_request((fts_object_t *)this);
    }
}

static void
gint_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  gint_t *this = (gint_t *)o;

  fts_outlet_int(o, 0, this->value);
}

static void
gint_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  gint_t *this = (gint_t *)o;
  int n = fts_get_number_int(at);

  gint_update(this, n);
  fts_outlet_int(o, 0, n);
}

static void
gint_varargs(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if(ac && fts_is_number(at))
    gint_number(o, 0, 0, 1, at);
}

static void
gint_incr(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  gint_t *this = (gint_t *)o;
  int n;
  
  if(ac > 0 && fts_is_number(at))
    n = this->value + fts_get_number_int(at);
  else
    n = this->value + 1;

  gint_update(this, n);
  fts_outlet_int(o, 0, n);
}

static void
gint_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  gint_t *this = (gint_t *)o;
  int n = fts_get_number_int(at);

  gint_update(this, n);
}

static void 
gint_save_dotpat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  FILE *file;
  int x, y, w, font_index;
  fts_atom_t a;

  file = (FILE *)fts_get_pointer( at);

  fts_object_get_prop( o, fts_s_x, &a);
  x = fts_get_int( &a);
  fts_object_get_prop( o, fts_s_y, &a);
  y = fts_get_int( &a);
  fts_object_get_prop( o, fts_s_width, &a);
  w = fts_get_int( &a);
  font_index = 1;

  fprintf( file, "#P number %d %d %d %d;\n", x, y, w, font_index);
}

static void gint_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(gint_t), 0, 0);

  fts_class_message_varargs(cl, fts_s_save_dotpat, gint_save_dotpat);

  fts_class_message_varargs(cl, fts_s_update_real_time, gint_update_real_time); 
  fts_class_message_varargs(cl, fts_s_value, gint_number); 

  fts_class_message_varargs(cl, fts_s_set, gint_set);
  fts_class_message_varargs(cl, fts_new_symbol("incr"), gint_incr);

  fts_class_inlet_bang(cl, 0, gint_bang);
  fts_class_inlet_number(cl, 0, gint_number);
  fts_class_inlet_varargs(cl, 0, gint_varargs);

  fts_class_outlet_int(cl, 0);
}

void
gint_config(void)
{
  fts_class_install(fts_new_symbol("intbox"),gint_instantiate);
}

