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
 */

#include <fts/fts.h>
#include <ftsconfig.h>
#include <data/c/include/fvec.h>

fts_symbol_t paste_symbol = 0;

typedef struct _paste_
{
  fts_object_t o;
  float *buf;
  int size;
  int ring_size; /* size + tick_size */
  int index;
  double conv;
} paste_t;

/***************************************************************************************
 *
 *  user methods
 *
 */

static void 
paste_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  paste_t *this = (paste_t *)o;
  float *buf = this->buf;
  int buf_size = this->size;
  fvec_t *fvec = (fvec_t *)fts_get_object(at);
  int size = fvec_get_size(fvec);
  float *ptr = fvec_get_ptr(fvec);
  int onset = (int)((fts_get_time() - fts_dsp_get_time()) * this->conv);
  int tail;
  int index;
  int i;

  if(size > buf_size)
    size = buf_size;
  
  index = this->index + onset;
  tail = this->ring_size - index;

  if(tail > size)
    tail = size;

  /* fill fvec from ring buffer */
  for(i=0; i<tail; i++)
    buf[index + i] += ptr[i];
  
  for(i=0; i<size-tail; i++)
    buf[i] += ptr[tail + i];
}

/***************************************************************************************
 *
 *  dsp
 *
 */
static void
paste_reset(paste_t *this, double sr, int n_tick)
{
  int i;

  this->conv = 0.001 * fts_dsp_get_sample_rate();
  this->ring_size = this->size + n_tick;
  this->buf = (float *)fts_realloc(this->buf, this->ring_size * sizeof(float));

  for(i=0; i<this->ring_size; i++)
    this->buf[i] = 0.0;
}

static void 
paste_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  paste_t *this = (paste_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  float sr = fts_dsp_get_output_srate(dsp, 0);
  int n_tick = fts_dsp_get_output_size(dsp, 0);
  fts_atom_t a[3];
  
  paste_reset(this, sr, n_tick);
  
  fts_set_object(a + 0, this);
  fts_set_symbol(a + 1, fts_dsp_get_output_name(dsp, 0));
  fts_set_int(a + 2, n_tick);
  fts_dsp_add_function(paste_symbol, 3, a);
}

static void
paste_ftl(fts_word_t *argv)
{
  paste_t *this = (paste_t *)fts_word_get_pointer(argv + 0);
  float * restrict out = (float *)fts_word_get_pointer(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  float *buf = this->buf;
  int index = this->index;
  int tail = this->ring_size - index;
  int i;

  /* fill ring buffer */
  if(tail > n_tick)
    {
      for(i=0; i<n_tick; i++)
	{
	  out[i] = buf[index + i];
	  buf[index + i] = 0.0;
	}

      this->index += n_tick;
    }
  else
    {
      for(i=0; i<tail; i++)
	{
	  out[i] = buf[index + i];
	  buf[index + i] = 0.0;
	}

      for(i=0; i<n_tick-tail; i++)
	{
	  out[tail + i] = buf[i];
	  buf[i] = 0.0;
	}

      this->index = i;
    }
}

/***************************************************************************************
 *
 *  class
 *
 */

static void
paste_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  paste_t *this = (paste_t *)o;
  int size = 1024;

  this->buf = NULL;
  this->size = 0;
  this->ring_size = 0;
  this->index = 0;

  if(ac > 0)
    {
      if(fts_is_number(at))
	{
	  size = fts_get_number_int(at);
	  
	  if(size < 0)
	    size = 0;
	}
      else
	{
	  fts_object_set_error(o, "Wrong argument");
	  return;
	}
    }
      
  this->size = size;

  fts_dsp_add_object(o);
}

static void
paste_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  paste_t *this = (paste_t *)o;

  if(this->buf != NULL)
    fts_free(this->buf);

  fts_dsp_remove_object(o);
}

static void
paste_instantiate(fts_class_t *cl)
{  
  fts_class_init(cl, sizeof(paste_t), paste_init, paste_delete);      
  
  fts_class_message_varargs(cl, fts_s_put, paste_put);
  
  fts_class_inlet(cl, 0, fvec_type, paste_fvec);

  fts_dsp_declare_outlet(cl, 0);
}

void
signal_paste_config(void)
{
  paste_symbol = fts_new_symbol("paste~");
  fts_dsp_declare_function(paste_symbol, paste_ftl);  

  fts_class_install(paste_symbol, paste_instantiate);
}
