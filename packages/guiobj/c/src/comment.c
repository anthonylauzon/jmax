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


/*
 * The comment object is just a place holder for a comment property; it does
 * absolutely nothing.
 * The property is stored as normal property, no daemons used.
 */

#include <fts/fts.h>

fts_symbol_t sym_setComment = 0;

typedef struct {
  fts_object_t o;
  int color;
} comment_t;


static void comment_save_dotpat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  FILE *file;
  int x, y, w, font_index;
  fts_symbol_t text;
  fts_atom_t a;
  const char *p;

  file = (FILE *)fts_get_pointer( at);

  fts_object_get_prop( o, fts_s_x, &a);
  x = fts_get_int( &a);
  fts_object_get_prop( o, fts_s_y, &a);
  y = fts_get_int( &a);
  fts_object_get_prop( o, fts_s_width, &a);
  w = fts_get_int( &a);
  font_index = 1;

  fprintf( file, "#P comment %d %d %d %d ", x, y, w, font_index);

  fts_object_get_prop( o, fts_s_comment, &a);

  for ( p = fts_get_symbol( &a); *p; p++)
    {
      if ( *p == ',' || *p == ';')
	fprintf( file, "\\%c", *p);
      else
	fprintf( file, "%c", *p);
    }

  fprintf( file, ";\n");
}

static void comment_send_properties(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_t a[1];
  
  fts_object_get_prop(o, fts_s_comment, a);
  if( fts_get_symbol(a))
    fts_client_send_message(o, sym_setComment, 1, a);

  if( ((comment_t *)o)->color != 0)
    {
      fts_set_int(a, ((comment_t *)o)->color);
      fts_client_send_message(o, fts_s_color, 1, a);
    }
}

static void
comment_set_color(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  comment_t *this = (comment_t *)o;
  fts_atom_t a[1];  

  this->color = fts_get_int(at);
 
  if( fts_object_has_id(o))
    {
      fts_set_int( a, this->color);
      fts_client_send_message( o, fts_s_color, 1, a);
    }

  fts_patcher_set_dirty((fts_patcher_t *)o->patcher, 1);
}

static void
comment_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  comment_t * this = (comment_t *)o;

  this->color = 0;
}

static void
comment_dump(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  comment_t * this = (comment_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  fts_atom_t a[1];  

  if( this->color != 0)
    {
      fts_set_int(a, this->color);
      fts_dumper_send(dumper, fts_s_color, 1, a);
    }
}

static fts_status_t comment_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof( comment_t), 0, 0, 0);

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, comment_init);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_dump, comment_dump);  

  fts_method_define_varargs( cl, fts_system_inlet, fts_s_save_dotpat, comment_save_dotpat); 
  fts_method_define_varargs( cl, fts_system_inlet, fts_s_send_properties, comment_send_properties); 
  fts_method_define_varargs( cl, fts_system_inlet, fts_s_color, comment_set_color); 

  return fts_ok;
}

void comment_config(void)
{
  fts_class_install(fts_new_symbol("jcomment"), comment_instantiate);
  sym_setComment = fts_new_symbol("setComment");
}

