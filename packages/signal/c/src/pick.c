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
#include "fvec.h"

fts_symbol_t pick_symbol = 0;

typedef struct _pick_
{
  fts_object_t o;
  fvec_t *fvec;
  int index;
  double conv;
  double reconv;
} pick_t;

/***************************************************************************************
 *
 *  user methods
 *
 */

static void 
pick_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pick_t *this = (pick_t *)o;

  if(fts_object_outlet_is_connected(o, 0))
    fts_outlet_object((fts_object_t *)o, 0, (fts_object_t *)this->fvec);
}

static void 
pick_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pick_t *this = (pick_t *)o;

  this->index = -(int)((fts_get_time() - fts_dsp_get_time()) * this->conv);
}

static void 
pick_set_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pick_t *this = (pick_t *)o;
  fvec_t *fvec = fvec_atom_get(at);

  if(this->fvec != fvec)
    {
      if(this->fvec != NULL)
	fts_object_release((fts_object_t *)this->fvec);
      
      this->fvec = fvec;
      fts_object_refer((fts_object_t *)fvec);
      
      this->index = fvec_get_size(fvec);
    }
}

/***************************************************************************************
 *
 *  dsp
 *
 */

static void 
pick_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pick_t *this = (pick_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  float sr = fts_dsp_get_input_srate(dsp, 0);
  int n_tick = fts_dsp_get_input_size(dsp, 0);
  fts_atom_t a[3];
  
  this->conv = 0.001 * fts_dsp_get_sample_rate();
  this->reconv = 1.0 / this->conv;

  fts_set_object(a + 0, this);
  fts_set_symbol(a + 1, fts_dsp_get_input_name(dsp, 0));
  fts_set_int(a + 2, n_tick);
  fts_dsp_add_function(pick_symbol, 3, a);
}

static void
pick_ftl(fts_word_t *argv)
{
  pick_t *this = (pick_t *)fts_word_get_pointer(argv + 0);
  float * restrict in = (float *)fts_word_get_pointer(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  float *buf = fvec_get_ptr(this->fvec);
  int size = fvec_get_size(this->fvec);
  int index = this->index;

  if(index < 0)
    {
      int onset = -index;
      int n = n_tick - onset;
      int i;

      if(n >= size)
	{
	  n = size;
	  fts_timebase_add_call(fts_get_timebase(), (fts_object_t *)this, pick_output, 0, (n - 1) * this->reconv);
	}

      for(i=0; i<n; i++)
	buf[i] = in[onset + i];

      this->index = n;
    }
  else
    {
      int n = size - index;
      int i;

      if(n > n_tick)
	n = n_tick;
      else if(n)
	fts_timebase_add_call(fts_get_timebase(), (fts_object_t *)this, pick_output, 0, (n - 1) * this->reconv);

      for(i=0; i<n; i++)
	buf[index + i] = in[i];


      this->index += n;
    }
}

/***************************************************************************************
 *
 *  class
 *
 */

static void
pick_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  pick_t *this = (pick_t *)o;
  int size = 1024;
  fvec_t *fvec;

  ac--;
  at++;

  this->fvec = NULL;
  this->index = 0;
  this->conv = 0.0;
  this->reconv = 0.0;

  if(ac > 0)
    {
      if(fts_is_number(at))
	{
	  size = fts_get_number_int(at);
	  
	  if(size < 0)
	    size = 0;
	}
      else if(fts_is_a(at, fvec_type))
	pick_set_fvec(o, 0, 0, 1, at);
      else
	{
	  fts_object_set_error(o, "Wrong argument");
	  return;
	}
    }
      
  if(this->fvec == NULL)  
    {
      this->fvec = (fvec_t *)fts_object_create(fvec_type, 0, 0);
      fvec_set_size(this->fvec, size);
      fts_object_refer((fts_object_t *)this->fvec);
    }

  this->index = fvec_get_size(this->fvec);

  fts_dsp_add_object(o);
}

static void
pick_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pick_t *this = (pick_t *)o;

  if(this->fvec)
    fts_object_release((fts_object_t *)this->fvec);    

  fts_dsp_remove_object(o);
}

static fts_status_t
pick_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{  
  fts_class_init(cl, sizeof(pick_t), 2, 1, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, pick_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, pick_delete);      
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, pick_put);
  
  fts_method_define_varargs(cl, 0, fts_s_bang, pick_bang);
  fts_method_define_varargs(cl, 1, fvec_symbol, pick_set_fvec);

  fts_dsp_declare_inlet(cl, 0);

  return fts_Success;
}

void
signal_pick_config(void)
{
  pick_symbol = fts_new_symbol("pick~");
  fts_dsp_declare_function(pick_symbol, pick_ftl);  

  fts_class_install(pick_symbol, pick_instantiate);
}
