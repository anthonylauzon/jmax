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

fts_symbol_t cut_symbol = 0;

typedef struct _cut_
{
  fts_dsp_object_t o;
  float *buf;
  int size;
  int index;
  fvec_t *fvec;
} cut_t;

/***************************************************************************************
 *
 *  user methods
 *
 */

static void 
cut_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cut_t *this = (cut_t *)o;
  float *buf = this->buf;
  int ring_size = this->size;
  int size = fvec_get_size(this->fvec);
  float *ptr = fvec_get_ptr(this->fvec);
  int onset = 0;
  int tail;
  int index;
  int i;

  if(size > ring_size)
    {
      onset = size - ring_size;
      size = ring_size;

      for(i=0; i<onset; i++)
	ptr[i] = 0.0;
    }
  
  index = this->index - size;

  if(index < 0)
    {
      tail = -index;
      index += ring_size;
    }
  else
    tail = ring_size - index;

  if(tail > size)
    tail = size;

  /* fill fvec from ring buffer */
  for(i=0; i<tail; i++)
    ptr[onset + i] = buf[index + i];
  
  for(i=0; i<size-tail; i++)
    ptr[onset + tail + i] = buf[i];

  fts_outlet_object(o, 0, (fts_object_t *)this->fvec);
}

static void 
cut_set_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cut_t *this = (cut_t *)o;
  fvec_t *fvec = (fvec_t *)fts_get_object(at);

  if(this->fvec != fvec)
    {
      if(this->fvec != NULL)
	fts_object_release((fts_object_t *)this->fvec);
      
      this->fvec = fvec;
      fts_object_refer((fts_object_t *)fvec);
    }
}

/***************************************************************************************
 *
 *  dsp
 *
 */

static void 
cut_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cut_t *this = (cut_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  int n_tick = fts_dsp_get_input_size(dsp, 0);
  fts_atom_t a[3];
  
  fts_set_object(a + 0, this);
  fts_set_symbol(a + 1, fts_dsp_get_input_name(dsp, 0));
  fts_set_int(a + 2, n_tick);
  fts_dsp_add_function(cut_symbol, 3, a);
}

static void
cut_ftl(fts_word_t *argv)
{
  cut_t *this = (cut_t *)fts_word_get_pointer(argv + 0);
  float * restrict in = (float *)fts_word_get_pointer(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  float *buf = this->buf;
  int size = this->size;
  int index = this->index;
  int tail = size - index;
  int i;

  /* fill ring buffer */
  if(tail > n_tick)
    {
      for(i=0; i<n_tick; i++)
	buf[index + i] = in[i];

      this->index += n_tick;
    }
  else
    {
      for(i=0; i<tail; i++)
	buf[index + i] = in[i];

      for(i=0; i<n_tick-tail; i++)
	buf[i] = in[tail + i];

      this->index = i;
    }
}

/***************************************************************************************
 *
 *  class
 *
 */

static void
cut_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  cut_t *this = (cut_t *)o;
  int size = 1024;
  fvec_t *fvec = NULL;

  this->buf = NULL;
  this->index = 0;
  this->size = 0;
  this->fvec = NULL;

  if(ac > 0)
    {
      if(fts_is_number(at))
	{
	  size = fts_get_number_int(at);
	  
	  if(size < 0)
	    size = 0;
	}
      else if(fts_is_a(at, fvec_type))
	{
	  fvec = (fvec_t *)fts_get_object(at);
	  size = fvec_get_size(fvec);
	}
      else
	{
	  fts_object_error(o, "bad argument");
	  return;
	}
    }
      
  if(fvec == NULL)  
    {
      fvec = (fvec_t *)fts_object_create(fvec_type, 0, 0);
      fvec_set_size(fvec, size);
    }

  this->buf = (float *)fts_malloc(size * sizeof(float));
  this->size = size;

  this->fvec = fvec;
  fts_object_refer((fts_object_t *)fvec);

  fts_dsp_object_init((fts_dsp_object_t *)o);
}

static void
cut_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cut_t *this = (cut_t *)o;

  if(this->buf != NULL)
    fts_free(this->buf);

  if(this->fvec != NULL)
    fts_object_release((fts_object_t *)this->fvec);    

  fts_dsp_object_delete((fts_dsp_object_t *)o);
}

static void
cut_instantiate(fts_class_t *cl)
{  
  fts_class_init(cl, sizeof(cut_t), cut_init, cut_delete);      
  
  fts_class_message_varargs(cl, fts_s_put, cut_put);
  
  fts_class_inlet_bang(cl, 0, cut_bang);
  fts_class_inlet(cl, 1, fvec_type, cut_set_fvec);

  fts_dsp_declare_inlet(cl, 0);
  fts_class_outlet(cl, 0, fvec_type);
}

void
signal_cut_config(void)
{
  cut_symbol = fts_new_symbol("cut~");
  fts_dsp_declare_function(cut_symbol, cut_ftl);  

  fts_class_install(cut_symbol, cut_instantiate);
}
