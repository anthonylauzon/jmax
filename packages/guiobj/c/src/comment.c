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

fts_symbol_t sym_setComment = 0;

typedef struct 
{
  fts_object_t o;
  fts_symbol_t text;
  int color;
} comment_t;

static fts_memorystream_t *comment_memory_stream ;

static fts_memorystream_t *comment_get_memory_stream()
{
  if(!comment_memory_stream)
    comment_memory_stream = (fts_memorystream_t *)fts_object_create( fts_memorystream_class, 0, 0);

  return comment_memory_stream;
}

static void 
comment_update_gui(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  comment_t *this = (comment_t *)o;
  fts_atom_t a;
  
  if(this->text != NULL)
    {
      fts_set_symbol(&a, this->text);
      fts_client_send_message(o, sym_setComment, 1, &a);
    }

  if(this->color != 0)
    {
      fts_set_int(&a, this->color);
      fts_client_send_message(o, fts_s_color, 1, &a);
    }
}

static void
comment_set_text(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  comment_t *this = (comment_t *)o;
  
  this->text = fts_get_symbol(at);
}

static void
comment_set_text_from_array(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  comment_t *this = (comment_t *)o;  
  fts_memorystream_t *stream = comment_get_memory_stream();

  fts_memorystream_reset(stream);
  fts_spost_object_description_args( (fts_bytestream_t *)stream, ac, (fts_atom_t *)at);
  fts_bytestream_output_char((fts_bytestream_t *)stream,'\0');

  this->text = fts_new_symbol(fts_memorystream_get_bytes( stream));
}

static void
comment_set_color(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  comment_t *this = (comment_t *)o;
  fts_atom_t a;  

  if( this->color != fts_get_int(at))
  {
    this->color = fts_get_int(at);
    
    fts_set_int( &a, this->color);
    fts_client_send_message( o, fts_s_color, 1, &a);
    
    fts_object_set_dirty(o);
  }
}

static void
comment_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  comment_t * this = (comment_t *)o;

  this->text = NULL;
  this->color = 0;
}

static void
comment_dump_gui(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  comment_t * this = (comment_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  fts_atom_t a;  

  if(this->text != NULL)
    {
      fts_set_symbol(&a, this->text);
      fts_dumper_send(dumper, fts_s_comment, 1, &a);
    }

  if(this->color != 0)
    {
      fts_set_int(&a, this->color);
      fts_dumper_send(dumper, fts_s_color, 1, &a);
    }
}

static void 
comment_save_dotpat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  comment_t * this = (comment_t *)o;
  FILE *file = (FILE *)fts_get_pointer(at);
  int font_index = 1;
  fts_atom_t a;
  const char *p;
  int x, y, w;

  fts_object_get_prop( o, fts_s_x, &a);
  x = fts_get_int( &a);

  fts_object_get_prop( o, fts_s_y, &a);
  y = fts_get_int( &a);

  fts_object_get_prop( o, fts_s_width, &a);
  w = fts_get_int( &a);

  font_index = 1;

  fprintf( file, "#P comment %d %d %d %d ", x, y, w, font_index);

  for ( p = this->text; *p; p++)
    {
      if ( *p == ',' || *p == ';')
	fprintf( file, "\\%c", *p);
      else
	fprintf( file, "%c", *p);
    }

  fprintf( file, ";\n");
}

static void 
comment_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof( comment_t), comment_init, NULL);

  fts_class_message_varargs(cl, fts_s_dump_gui, comment_dump_gui);

  fts_class_message_varargs(cl, fts_s_save_dotpat, comment_save_dotpat); 
  fts_class_message_varargs(cl, fts_s_update_gui, comment_update_gui); 

  fts_class_message_varargs(cl, fts_s_set, comment_set_text_from_array);

  fts_class_message_varargs(cl, fts_s_comment, comment_set_text);
  fts_class_message_varargs(cl, fts_s_color, comment_set_color); 
}

void 
comment_config(void)
{
  fts_class_install(fts_s_jcomment, comment_instantiate);
  sym_setComment = fts_new_symbol("setComment");
}
