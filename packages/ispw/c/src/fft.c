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
#include "fft_ftl.h"


static fts_symbol_t sym_tandem = 0, sym_real = 0, sym_half = 0, sym_miller = 0;
static int check_args(int ac, const fts_atom_t *at, fts_symbol_t *type,  fts_symbol_t *real_spec);

/* the class names */
fts_symbol_t sym_fft = 0;
fts_symbol_t sym_ifft = 0;

typedef struct{
  fts_object_t head;
  fts_symbol_t name;
  fft_ctl_t ctl; /* pointer to control block */
  long hop; /* hop in samples of FFT (at least size) */
  long phase; /* onset of fft frame within the hop size */
  long spec_size;
  fts_symbol_t type; /* sym_real or sym_tandem */
  fts_symbol_t real_spec; /* sym_half or sym_miller */
  int bang_out; /* index of bang outlet */
} fft_t;

enum
{
  FFT_ARG_class,
  FFT_ARG_args,
  N_FFT_ARG
};

#define DEF_size 512

/************************************************
 *
 *    object
 *
 */

void
fft_output_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_bang((fts_object_t *)o, ((fft_t *)o)->bang_out);
}

static void
buffers_init(fft_t *x, fts_symbol_t type, fts_symbol_t real_spec, int ac, const fts_atom_t *at)
{
  long size, hop, phase;
  complex *buf = 0;
  complex *spec = 0;
  long spec_size;

  while(ac && fts_is_symbol(at)) /* skip type declaration */
    {
      at++;
      ac--;
    }

  size = fts_get_int_arg(ac, at, 0, DEF_size);
  hop = fts_get_int_arg(ac, at, 1, size);
  phase = fts_get_int_arg(ac, at, 2, 0);
  
  if(!fts_fft_declaresize(size))
    {
      post("error: %s: no fft size: %d\n", x->name, size);
      return;
    }

  if(hop % size || hop <= 0)
    {
      post("error: %s: hop must be a multiple of the FFT size\n", x->name);
      return;
    }

  if(real_spec)
    spec_size = size >> 1;
  else
    spec_size = size;

  if(type == sym_real)
    buf = (complex *)fts_malloc(sizeof(float) * size);
  else /* tandem or complex */
    buf = (complex *)fts_malloc(sizeof(complex) * size);

  if(!buf)
    {
      post("error: %s: can't allocate buffers\n", x->name);
      return;
    }
  
  if(type == sym_tandem)
    spec = (complex *)fts_malloc(sizeof(complex) * (2 * spec_size));
  else
    spec = (complex *)fts_malloc(sizeof(complex) * spec_size);
  if (!spec)
    {
      post("error: %s: can't allocate buffers\n", x->name);
      return;
    }

  x->ctl.object = (fts_object_t *)x;
  x->ctl.buf = buf;
  x->ctl.spec = spec;
  x->ctl.size = size;
  x->spec_size = spec_size;
  x->hop = hop;  
  x->phase = phase % hop;

  x->type = type; /* default */
  x->real_spec = real_spec;
}  

static void
fft_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fft_t *x = (fft_t *)o;
  fts_symbol_t type = 0;
  fts_symbol_t real_spec = 0;

  if(!check_args(ac, at, &type, &real_spec))
    {
      fts_object_set_error(o, "Wrong  arguments\n");
      return;
    }

  buffers_init(x, type, real_spec, ac, at);

  if(real_spec == sym_half)
    {
      fts_atom_t av;
      
      fts_set_int(&av, 1);
      fts_object_put_prop(o, fts_s_dsp_downsampling, &av);
    }
  
  if(type == sym_real)
    {
      fts_object_set_outlets_number(o, 3);

      x->bang_out = 2;
    }
  else if(type == sym_tandem)
    {
      fts_object_set_inlets_number(o, 2);
      fts_object_set_outlets_number(o, 5);

      x->bang_out = 4;
    }
  else /* complex */
    {
      fts_object_set_inlets_number(o, 2);
      fts_object_set_outlets_number(o, 3);

      x->bang_out = 2;
    }

  fts_dsp_add_object(o);
}

static void
ifft_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fft_t *x = (fft_t *)o;
  fts_symbol_t type = 0;
  fts_symbol_t real_spec = 0;

  if(!check_args(ac, at, &type, &real_spec))
    {
      fts_object_set_error(o, "Wrong arguments\n", x->name);
      return;
    }

  buffers_init(x, type, real_spec, ac, at);

  if(real_spec == sym_half)
    {
      fts_atom_t av;
      
      fts_set_int(&av, 1);
      fts_object_put_prop(o, fts_s_dsp_upsampling, &av);
    }
  
  if(type == sym_real)
    {
      fts_object_set_inlets_number(o, 2);
      fts_object_set_outlets_number(o, 2);

      x->bang_out = 1;
    }
  else if(type == sym_tandem)
    {
      fts_object_set_inlets_number(o, 4);
      fts_object_set_outlets_number(o, 3);

      x->bang_out = 2;
    }
  else /* complex */
    {
      fts_object_set_inlets_number(o, 2);
      fts_object_set_outlets_number(o, 3);

      x->bang_out = 2;
    }

  fts_dsp_add_object(o);
}

static void
fft_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fft_t *x = (fft_t *)o;

  if(x->ctl.buf) 
    fts_free((void *)x->ctl.buf);

  if(x->ctl.spec) 
    fts_free((void *)x->ctl.spec);

  fts_dsp_remove_object(o);
}

/************************************************
 *
 *    dsp
 *
 */

static void
fft_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fft_t *x = (fft_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  fts_symbol_t type = x->type;
  fts_symbol_t real_spec = x->real_spec;
  fts_atom_t dsp_arg[8]; /* maximum # of dsp args */
  int n_tick = fts_dsp_get_input_size(dsp, 0);
  long phase = x->phase;
  long hop = x->hop;
  long size = x->ctl.size;
  long spec_size = x->spec_size;
  int sig_tick; /* tick size in time domain */
  int spec_tick; /* tick size in spectral domain (is sig_tick/2 for real_spec = sym_half) */
  int i, n;

  if(real_spec == sym_half)
    {
      sig_tick = n_tick;
      spec_tick = n_tick >> 1; /* down sample outputs */
    }
  else
    {
      sig_tick = spec_tick = n_tick;
    }

  if(sig_tick > size){
    post("%s: FFT size to small (< tick size)\n", x->name);
    return;
  }
  if(phase % sig_tick){
    post("%s: phase must be multiple of tick size: %d\n", x->name, sig_tick);
    return;
  }

  x->ctl.gap_size = hop - size;

  /* zero buffers */

  if(type == sym_real)
    n = size / 2;
  else /* complex or tandem */
    n = size;

  for(i=0; i<n; i++)
    {
      x->ctl.buf[i].re = 0.0;
      x->ctl.buf[i].im = 0.0;
    }
 
  if(type == sym_tandem)
    n = 2 * spec_size;
  else
    n = spec_size;

  for(i=0; i<n; i++)
    {
      x->ctl.spec[i].re = 0.0;
      x->ctl.spec[i].im = 0.0;
    }
 
  x->ctl.in_idx = 0;
  x->ctl.out_idx = size;
  x->ctl.gap_count = phase;
  
  /* put it ... */
  fts_set_pointer(dsp_arg + 0, &(x->ctl));
  
  fts_set_int(dsp_arg + 1, sig_tick);
  
  if(type == sym_real)
    {
      fts_set_symbol(dsp_arg + 2, fts_dsp_get_input_name(dsp, 0));
      fts_set_symbol(dsp_arg + 3, fts_dsp_get_output_name(dsp, 0));
      fts_set_symbol(dsp_arg + 4, fts_dsp_get_output_name(dsp, 1));
      if(real_spec == sym_half)
	fts_dsp_add_function(dsp_sym_fft_real_half, 5, dsp_arg);
      else if(real_spec == sym_miller)
	fts_dsp_add_function(dsp_sym_fft_real_miller, 5, dsp_arg);
      else
	fts_dsp_add_function(dsp_sym_fft_real, 5, dsp_arg);
    }
  else if(type == sym_tandem)
    {
      fts_set_symbol(dsp_arg + 2, fts_dsp_get_input_name(dsp, 0));
      fts_set_symbol(dsp_arg + 3, fts_dsp_get_input_name(dsp, 1));
      fts_set_symbol(dsp_arg + 4, fts_dsp_get_output_name(dsp, 0));
      fts_set_symbol(dsp_arg + 5, fts_dsp_get_output_name(dsp, 1));
      fts_set_symbol(dsp_arg + 6, fts_dsp_get_output_name(dsp, 2));
      fts_set_symbol(dsp_arg + 7, fts_dsp_get_output_name(dsp, 3));
      if(real_spec == sym_half)
	fts_dsp_add_function(dsp_sym_fft_tandem_half, 8, dsp_arg);
      else if(real_spec == sym_miller)
	fts_dsp_add_function(dsp_sym_fft_tandem_miller, 8, dsp_arg);
      else
	fts_dsp_add_function(dsp_sym_fft_tandem, 8, dsp_arg);
    }
  else /* complex */
    {
      fts_set_symbol(dsp_arg + 2, fts_dsp_get_input_name(dsp, 0));
      fts_set_symbol(dsp_arg + 3, fts_dsp_get_input_name(dsp, 1));
      fts_set_symbol(dsp_arg + 4, fts_dsp_get_output_name(dsp, 0));
      fts_set_symbol(dsp_arg + 5, fts_dsp_get_output_name(dsp, 1));
      fts_dsp_add_function(dsp_sym_fft_complex, 6, dsp_arg);
    }
}

static void
ifft_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fft_t *x = (fft_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  fts_symbol_t type = x->type;
  fts_symbol_t real_spec = x->real_spec;
  fts_atom_t dsp_arg[8]; /* maximum # of dsp args */
  int n_tick = fts_dsp_get_input_size(dsp, 0);
  long phase = x->phase;
  long hop = x->hop;
  long size = x->ctl.size;
  long spec_size = x->spec_size;
  int sig_tick; /* tick size in time domain */
  int spec_tick; /* tick size in spectral domain (is sig_tick/2 for real_spec = sym_half) */
  int i, n;

  if(real_spec == sym_half)
    {
      sig_tick = n_tick << 1; /* up sample outputs */
      spec_tick = n_tick;
    }
  else
    {
      sig_tick = spec_tick = n_tick;
    }

  if(sig_tick > size){
    post("%s: FFT size to small (< tick size)\n", x->name);
    return;
  }
  if(phase % sig_tick){
    post("%s: phase must be multiple of tick size: %d\n", x->name, sig_tick);
    return;
  }

  x->ctl.gap_size = hop - size;

  /* zero buffers */

  if(type == sym_real)
    n = size / 2;
  else /* complex or tandem */
    n = size;

  for(i=0; i<n; i++)
    {
      x->ctl.buf[i].re = 0.0;
      x->ctl.buf[i].im = 0.0;
    }
 
  if(type == sym_tandem)
    n = 2 * spec_size;
  else
    n = spec_size;

  for(i=0; i<n; i++)
    {
      x->ctl.spec[i].re = 0.0;
      x->ctl.spec[i].im = 0.0;
    }
 
  x->ctl.in_idx = 0;
  x->ctl.out_idx = size;
  x->ctl.gap_count = phase;
  
  /* put it ... */
  fts_set_pointer(dsp_arg + 0, &(x->ctl));
  
  fts_set_int(dsp_arg + 1, spec_tick);
  
  if(type == sym_real)
    {
      fts_set_symbol(dsp_arg + 2, fts_dsp_get_input_name(dsp, 0));
      fts_set_symbol(dsp_arg + 3, fts_dsp_get_input_name(dsp, 1));
      fts_set_symbol(dsp_arg + 4, fts_dsp_get_output_name(dsp, 0));
      if(real_spec == sym_half)
	fts_dsp_add_function(dsp_sym_ifft_real_half, 5, dsp_arg);
      else if(real_spec == sym_miller)
	fts_dsp_add_function(dsp_sym_ifft_real_miller, 5, dsp_arg);
      else
	fts_dsp_add_function(dsp_sym_ifft_real, 5, dsp_arg);
    }    
  
  else if(type == sym_tandem)
    {
      fts_set_symbol(dsp_arg + 2, fts_dsp_get_input_name(dsp, 0));
      fts_set_symbol(dsp_arg + 3, fts_dsp_get_input_name(dsp, 1));
      fts_set_symbol(dsp_arg + 4, fts_dsp_get_input_name(dsp, 2));
      fts_set_symbol(dsp_arg + 5, fts_dsp_get_input_name(dsp, 3));
      fts_set_symbol(dsp_arg + 6, fts_dsp_get_output_name(dsp, 0));
      fts_set_symbol(dsp_arg + 7, fts_dsp_get_output_name(dsp, 1));
      if(real_spec == sym_half)
	fts_dsp_add_function(dsp_sym_ifft_tandem_half, 8, dsp_arg);
      else if(real_spec == sym_miller)
	fts_dsp_add_function(dsp_sym_ifft_tandem_miller, 8, dsp_arg);
      else
	fts_dsp_add_function(dsp_sym_ifft_tandem, 8, dsp_arg);
    }    
  else /* complex */
    {
      fts_set_symbol(dsp_arg + 2, fts_dsp_get_input_name(dsp, 0));
      fts_set_symbol(dsp_arg + 3, fts_dsp_get_input_name(dsp, 1));
      fts_set_symbol(dsp_arg + 4, fts_dsp_get_output_name(dsp, 0));
      fts_set_symbol(dsp_arg + 5, fts_dsp_get_output_name(dsp, 1));
      fts_dsp_add_function(dsp_sym_ifft_complex, 6, dsp_arg);
    }
}

/************************************************
 *
 *    methods
 *
 */

static void
fft_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fft_t *x = (fft_t *)o;
  x->ctl.in_idx = 0;
  x->ctl.out_idx = 0;
  x->ctl.gap_count = 0;
}

static void
fft_setphase(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fft_t *x = (fft_t *)o;
  long phase = fts_get_int_arg(ac, at, 0, 0);

  x->phase = phase;
  x->ctl.gap_count = phase;
}

/************************************************
 *
 *    class
 *
 */

static void
fft_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fft_t), fft_init, fft_delete);

  fts_class_method_varargs(cl, fts_s_put, fft_put);
  
  fts_class_method_varargs(cl, fts_s_bang, fft_bang);
  fts_class_method_varargs(cl, fts_new_symbol("setphase"), fft_setphase);

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0);
  }

static void
ifft_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fft_t), ifft_init, fft_delete);

  fts_class_method_varargs(cl, fts_s_put, ifft_put);

  fts_class_method_varargs(cl, fts_s_bang, fft_bang);
  fts_class_method_varargs(cl, fts_new_symbol("setphase"), fft_setphase);

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0);
  }

void
fft_ispw_config(void)
{
  sym_fft = fts_new_symbol("fft~");
  sym_ifft = fts_new_symbol("ifft~");
  sym_tandem = fts_new_symbol("tandem");
  sym_real = fts_new_symbol("real");
  sym_half = fts_new_symbol("half");
  sym_miller = fts_new_symbol("miller");
  
  fts_class_install(sym_fft, fft_instantiate);
  fts_class_install(sym_ifft, ifft_instantiate);
  
  if(!dsp_sym_fft_complex) 
    ftl_fft_init(); /* init the fft functions dsp symbol table */
}

/************************************************
 *
 *    utillity
 *
 */

static int 
check_args(int ac, const fts_atom_t *at, fts_symbol_t *type, fts_symbol_t *real_spec)
{
  fts_symbol_t s ;
  
  while(ac && fts_is_symbol(at))
    {
      s = fts_get_symbol(at++); 
      ac--;
      if(s == sym_real || s == sym_tandem)
	{
	  if(*type != 0 && *type != s) 
	    return(0); /* check failed: wrong type */
	  else 
	    *type = s;
	}
      else if(s == sym_half || s == sym_miller)
	{
	  if(*real_spec != 0 && *real_spec != s) 
	    return(0); /* check failed: wrong real_spec */
	  else 
	    *real_spec = s;
	}
      else
	return(0); /* check failed: wrong arg */
    }

  if((*real_spec) && !(*type) ) 
    return(0); /* real_spec just possible with real or tandem */

  return(1); /* check o.k. */
}

