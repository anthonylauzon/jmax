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
 * absolutely nothing; it is called jcomment, because the comment doctor
 * translate from old comments to new ones (i.e. comment text move from
 * arguments to the propriety).
 * The property is stored as normal property, no daemons used.
 */

#include <fts/fts.h>


static void comment_save_dotpat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  FILE *file;
  int x, y, w, font_index;
  fts_symbol_t text;
  fts_atom_t a;
  char *p;

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

  for ( p = fts_symbol_name( fts_get_symbol( &a)); *p; p++)
    {
      if ( *p == ',' || *p == ';')
	fprintf( file, "\\%c", *p);
      else
	fprintf( file, "%c", *p);
    }

  fprintf( file, ";\n");
}

static fts_status_t comment_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof( fts_object_t), 0, 0, 0);

  fts_method_define_varargs( cl, fts_SystemInlet, fts_s_save_dotpat, comment_save_dotpat); 

  return fts_Success;
}

void comment_config(void)
{
  fts_class_install(fts_new_symbol("jcomment"), comment_instantiate);
}

