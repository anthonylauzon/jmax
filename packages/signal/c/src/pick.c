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
#include <ftsconfig.h>
#include <fts/packages/data/data.h>

fts_symbol_t pick_symbol = 0;

typedef struct _pick_
{
  fts_dsp_object_t o;
  fmat_t *fmat;
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
pick_output(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  pick_t *this = (pick_t *)o;

  if(fts_object_outlet_is_connected(o, 0))
    fts_outlet_object((fts_object_t *)o, 0, (fts_object_t *)this->fmat);
}

static void 
pick_bang(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  pick_t *this = (pick_t *)o;

  this->index = -(int)((fts_get_time() - fts_dsp_get_time()) * this->conv);
}

static void 
pick_set_fmat(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  pick_t *this = (pick_t *)o;
  fmat_t *fmat = (fmat_t *)fts_get_object(at);
  
  if(this->fmat != fmat)
  {
    if(this->fmat != NULL)
      fts_object_release((fts_object_t *)this->fmat);
    
    this->fmat = fmat;
    fts_object_refer((fts_object_t *)fmat);
    
    this->index = fmat_get_m(fmat);
  }
}

/***************************************************************************************
 *
 *  dsp
 *
 */

static void 
pick_put(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  pick_t *this = (pick_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
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
  float *buf = fmat_get_ptr(this->fmat);
  int size = fmat_get_m(this->fmat);
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
pick_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{ 
  pick_t *this = (pick_t *)o;
  int size = 1024;

  this->fmat = NULL;
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
      else if(fts_is_a(at, fmat_class))
	pick_set_fmat(o, 0, 1, at, fts_nix);
      else
	{
	  fts_object_error(o, "bad argument");
	  return;
	}
    }
      
  if(this->fmat == NULL)  
    {
      this->fmat = (fmat_t *)fts_object_create(fmat_class, 0, 0);
      fmat_set_size(this->fmat, size, 1);
      fts_object_refer((fts_object_t *)this->fmat);
    }

  this->index = fmat_get_m(this->fmat);

  fts_dsp_object_init((fts_dsp_object_t *)o);
}

static void
pick_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  pick_t *this = (pick_t *)o;

  if(this->fmat)
    fts_object_release((fts_object_t *)this->fmat);    

  fts_dsp_object_delete((fts_dsp_object_t *)o);
}

static void
pick_instantiate(fts_class_t *cl)
{  
  fts_class_init(cl, sizeof(pick_t), pick_init, pick_delete);      
  
  fts_class_message_varargs(cl, fts_s_put, pick_put);
  
  fts_class_inlet_bang(cl, 0, pick_bang);
  fts_class_inlet(cl, 1, fmat_class, pick_set_fmat);

  fts_dsp_declare_inlet(cl, 0);
  fts_class_outlet(cl, 0, fmat_class);
}

void
signal_pick_config(void)
{
  pick_symbol = fts_new_symbol("pick~");
  fts_dsp_declare_function(pick_symbol, pick_ftl);  

  fts_class_install(pick_symbol, pick_instantiate);
}
