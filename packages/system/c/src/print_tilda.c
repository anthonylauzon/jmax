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

#include <fts/fts.h>

typedef struct print_tilda_t 
{
  fts_dsp_object_t _o;
  fts_bytestream_t *stream;
  fts_symbol_t prompt;
  int cols;
  float *buf;
  int size;
  int alloc;
  int index;  
} print_tilda_t;

static void
print_tilda_post(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  print_tilda_t *this = (print_tilda_t *)o;

  if(fts_object_get_patcher(o) != NULL)  
    {
      float *buf = this->buf;
      int size = this->size;
      int cols = this->cols;
      int i, j;
      
      if(this->prompt)
	post("%s:\n", this->prompt);
      
      for(i=0; i<size-cols; i+=cols)
	{
	  for(j=0; j<cols; j++)
	    {
	      fts_spost_float(this->stream, buf[i + j]);
	      fts_spost(this->stream, " ");
	    }
	  
	  fts_spost(this->stream, "\n");      
	}
      
      if(i < size)
	{
	  for (; i<size; i++)
	    {
	      fts_spost_float(this->stream, buf[i]);
	      fts_spost(this->stream, " ");
	    }
	  
	  fts_spost(this->stream, "\n");      
	}
    }
}

static void
print_tilda_set_size(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  print_tilda_t *this = (print_tilda_t *)o;
  int size = fts_get_number_int(at);

  if(size < 1)
    size = 1;
  
  if(size > this->alloc)
    {
      this->buf = fts_realloc(this->buf, size * sizeof(float));
      this->alloc = size;
    }
  
  this->size = size;
}

static void
print_tilda_set_cols(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  print_tilda_t *this = (print_tilda_t *)o;
  int cols = fts_get_number_int(at);

  if(cols < 1)
    cols = 1;
  
  this->cols = cols;
}

static void
print_tilda_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  print_tilda_t *this = ((print_tilda_t *)o);

  switch(ac)
    {
    default:
    case 2:
      if(fts_is_number(at + 1))
	print_tilda_set_cols(o, 0, 0, 1, at + 1);
    case 1:
      if(fts_is_number(at))
	print_tilda_set_size(o, 0, 0, 1, at);
    case 0:
      break;
    }
}

static void
print_tilda_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  print_tilda_t *this = ((print_tilda_t *)o);

  print_tilda_set(o, 0, 0, ac, at);
  this->index = 0;
}

static void 
ftl_print_tilda(fts_word_t *argv)
{
  print_tilda_t *this = (print_tilda_t *)fts_word_get_pointer(argv + 0);
  float *in = (float *)fts_word_get_pointer(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  int index = this->index;
  int n_left = this->size - index;

  if(n_left > 0)
    {
      int i;
      
      if(n_left > n_tick)
	n_left = n_tick;
      
      for(i=0; i<n_left; i++)
	this->buf[index + i] = in[i];
      
      index = index + n_left;
      
      if(index >= this->size)
	fts_timebase_add_call(fts_get_timebase(), (fts_object_t *)this, print_tilda_post, 0, 0.0);

      this->index = index;
    }
}

static void
print_tilda_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  print_tilda_t *this = ((print_tilda_t *)o);
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  int n_tick = fts_dsp_get_input_size(dsp, 0);
  fts_atom_t argv[3];
  
  fts_set_object(argv + 0, o);
  fts_set_symbol(argv + 1, fts_dsp_get_input_name(dsp, 0));
  fts_set_int(argv + 2, n_tick);
  fts_dsp_add_function(fts_s_print, 3, argv);
}

static void
print_tilda_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  print_tilda_t *this = ((print_tilda_t *)o);
  int size = fts_dsp_get_tick_size();

  this->stream = fts_get_default_console_stream();
  this->prompt = NULL;

  this->buf = fts_malloc(size * sizeof(float));
  this->size = size;
  this->alloc = size;
  this->cols = 4;

  if(ac > 0 && fts_is_symbol(at))
    {
      this->prompt = fts_get_symbol(at);
      ac--;
      at++;
    }

  print_tilda_set(o, 0, 0, ac, at);
    
  fts_dsp_object_init((fts_dsp_object_t *)o);
}

static void
print_tilda_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  print_tilda_t *this = ((print_tilda_t *)o);

  if(this->buf) 
    fts_free(this->buf);

  fts_dsp_object_delete((fts_dsp_object_t *)o);
}

static void
print_tilda_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(print_tilda_t), print_tilda_init, print_tilda_delete);

  fts_class_message_varargs(cl, fts_s_put, print_tilda_put);

  fts_class_message_varargs(cl, fts_s_bang, print_tilda_print);
  fts_class_message_varargs(cl, fts_s_print, print_tilda_print);

  fts_class_message_varargs(cl, fts_s_set, print_tilda_set);
  fts_class_message_varargs(cl, fts_new_symbol("cols"), print_tilda_set_cols);
  fts_class_message_varargs(cl, fts_new_symbol("size"), print_tilda_set_size);

  fts_dsp_declare_inlet(cl, 0);

  fts_dsp_declare_function(fts_s_print, ftl_print_tilda);
}

void
print_tilda_config(void)
{
  fts_class_install(fts_new_symbol("print~"), print_tilda_instantiate);
}
