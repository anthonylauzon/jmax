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

#include <fts/fts.h>
#include "fft_ftl.h"

/* temporary here */
void complete_symetric_spectrum_inplc_after_rfft(complex *buf, int n_points);
void shuffle_after_cfft_for_tandem(complex *in, complex *out0, complex *out1, int n_points);
void shuffle_before_cifft_for_tandem(complex *in0, complex *in1, complex *out, int n_points);
void shuffle_after_cfft_for_tandem_half(complex *in, complex *out0, complex *out1, int n_points);
void shuffle_before_cifft_for_tandem_half(complex *in0, complex *in1, complex *out, int n_points);

/**************************************************************
 *
 *    all the symbols
 *
 */
 
extern void ftl_fft_complex(fts_word_t *argv);
extern void ftl_ifft_complex(fts_word_t *argv);
extern void ftl_fft_real(fts_word_t *argv);
extern void ftl_ifft_real(fts_word_t *argv);
extern void ftl_fft_real_half(fts_word_t *argv);
extern void ftl_ifft_real_half(fts_word_t *argv);
extern void ftl_fft_tandem(fts_word_t *argv);
extern void ftl_ifft_tandem(fts_word_t *argv);
extern void ftl_fft_tandem_half(fts_word_t *argv);
extern void ftl_ifft_tandem_half(fts_word_t *argv);

extern void ftl_fft_real_miller(fts_word_t *argv);
extern void ftl_ifft_real_miller(fts_word_t *argv);
extern void ftl_fft_tandem_miller(fts_word_t *argv);
extern void ftl_ifft_tandem_miller(fts_word_t *argv);

fts_symbol_t
dsp_sym_fft_complex = 0,
  dsp_sym_ifft_complex = 0,
  dsp_sym_fft_real = 0,
  dsp_sym_ifft_real = 0,
  dsp_sym_fft_real_half = 0,
  dsp_sym_ifft_real_half = 0,
  dsp_sym_fft_tandem = 0,
  dsp_sym_ifft_tandem = 0,
  dsp_sym_fft_tandem_half = 0,
  dsp_sym_ifft_tandem_half = 0,

  dsp_sym_fft_real_miller = 0,
  dsp_sym_ifft_real_miller = 0,
  dsp_sym_fft_tandem_miller = 0,
  dsp_sym_ifft_tandem_miller = 0;
  
void
ftl_fft_init(void)
{
  dsp_sym_fft_complex = fts_new_symbol("fft_complex");
  dsp_sym_ifft_complex = fts_new_symbol("ifft_complex");
  dsp_sym_fft_real = fts_new_symbol("fft_real");
  dsp_sym_ifft_real = fts_new_symbol("ifft_real");
  dsp_sym_fft_real_half = fts_new_symbol("fft_real_half");
  dsp_sym_ifft_real_half = fts_new_symbol("ifft_real_half");
  dsp_sym_fft_tandem = fts_new_symbol("fft_tandem");
  dsp_sym_ifft_tandem = fts_new_symbol("ifft_tandem");
  dsp_sym_fft_tandem_half = fts_new_symbol("fft_tandem_half");
  dsp_sym_ifft_tandem_half = fts_new_symbol("ifft_tandem_half");
  
  dsp_sym_fft_real_miller = fts_new_symbol("fft_real_miller");
  dsp_sym_ifft_real_miller = fts_new_symbol("ifft_real_miller");
  dsp_sym_fft_tandem_miller = fts_new_symbol("fft_tandem_miller");
  dsp_sym_ifft_tandem_miller = fts_new_symbol("ifft_tandem_miller");

  fts_dsp_declare_function(dsp_sym_fft_complex, ftl_fft_complex);
  fts_dsp_declare_function(dsp_sym_ifft_complex, ftl_ifft_complex);
  fts_dsp_declare_function(dsp_sym_fft_real, ftl_fft_real);
  fts_dsp_declare_function(dsp_sym_ifft_real, ftl_ifft_real);
  fts_dsp_declare_function(dsp_sym_fft_real_half, ftl_fft_real_half);
  fts_dsp_declare_function(dsp_sym_ifft_real_half, ftl_ifft_real_half);
  fts_dsp_declare_function(dsp_sym_fft_tandem, ftl_fft_tandem);
  fts_dsp_declare_function(dsp_sym_ifft_tandem, ftl_ifft_tandem);
  fts_dsp_declare_function(dsp_sym_fft_tandem_half, ftl_fft_tandem_half);
  fts_dsp_declare_function(dsp_sym_ifft_tandem_half, ftl_ifft_tandem_half);

  fts_dsp_declare_function(dsp_sym_fft_real_miller, ftl_fft_real_miller);
  fts_dsp_declare_function(dsp_sym_ifft_real_miller, ftl_ifft_real_miller);
  fts_dsp_declare_function(dsp_sym_fft_tandem_miller, ftl_fft_tandem_miller);
  fts_dsp_declare_function(dsp_sym_ifft_tandem_miller, ftl_ifft_tandem_miller);
}


/**************************************************************
 *
 *    the routines with lots of scrap (copy & paste)
 *
 */

/*** complex ***/
 
void
ftl_fft_complex(fts_word_t *argv)
{
  fft_ctl_t *ctl = (fft_ctl_t *)fts_word_get_pointer(argv + 0);
  int n_tick = (int)fts_word_get_int(argv + 1);
  float *in0 = (float *)fts_word_get_pointer(argv + 2);
  float *in1 = (float *)fts_word_get_pointer(argv + 3);
  float *out0 = (float *)fts_word_get_pointer(argv + 4);
  float *out1 = (float *)fts_word_get_pointer(argv + 5);
  complex *buf = ctl->buf;
  complex *spec = ctl->spec;
  int out_idx = ctl->out_idx;
  int in_idx = ctl->in_idx;
  int size = ctl->size;
  int i;

  ctl->gap_count -= n_tick;
  if(ctl->gap_count < 0)
    {
      for(i=0; i<n_tick; i++)
	{
	  buf[in_idx + i].re = in0[i];
	  buf[in_idx + i].im = in1[i];
	}

      in_idx += n_tick;
    }
  
  if(out_idx < size)
    {
      for(i=0; i<n_tick; i++)
	{
	  out0[i] = spec[out_idx + i].re;
	  out1[i] = spec[out_idx + i].im;
	}

      out_idx += n_tick;
    }
  else
    {
      for(i=0; i<n_tick; i++)
	out0[i] = out1[i] = 0.0;
    }

  if(in_idx >= size)
    {
      fts_cfft(buf, spec, size);
      in_idx = out_idx = 0;
      ctl->gap_count = ctl->gap_size;

      fts_timebase_add_call(fts_get_timebase(), ctl->object, fft_output_bang, 0, 0.0);
    }
  
  ctl->out_idx = out_idx;
  ctl->in_idx = in_idx;
}

void
ftl_ifft_complex(fts_word_t *argv)
{
  fft_ctl_t *ctl = (fft_ctl_t *)fts_word_get_pointer(argv + 0);
  int n_tick = (int)fts_word_get_int(argv + 1);
  float *in0 = (float *)fts_word_get_pointer(argv + 2);
  float *in1 = (float *)fts_word_get_pointer(argv + 3);
  float *out0 = (float *)fts_word_get_pointer(argv + 4);
  float *out1 = (float *)fts_word_get_pointer(argv + 5);
  complex *buf = ctl->buf;
  complex *spec = ctl->spec;
  int out_idx = ctl->out_idx;
  int in_idx = ctl->in_idx;
  int size = ctl->size;
  int i;

  ctl->gap_count -= n_tick;
  if(ctl->gap_count < 0)
    {
      for(i=0; i<n_tick; i++)
	{
	  spec[in_idx + i].re = in0[i];
	  spec[in_idx + i].im = in1[i];
	}

      in_idx += n_tick;
    }
  
  if(out_idx < size)
    {
      for(i=0; i<n_tick; i++)
	{
	  out0[i] = buf[out_idx + i].re;
	  out1[i] = buf[out_idx + i].im;
	}

      out_idx += n_tick;
    }
  else
    {
      for(i=0; i<n_tick; i++)
	out0[i] = out1[i] = 0.0;
    }

  if(in_idx >= size)
    {
      fts_cifft(spec, buf, size);
      in_idx = out_idx = 0;
      ctl->gap_count = ctl->gap_size;
      fts_timebase_add_call(fts_get_timebase(), ctl->object, fft_output_bang, 0, 0.0);
    }
  
  ctl->out_idx = out_idx;
  ctl->in_idx = in_idx;
}

/*** real ***/
 
void
ftl_fft_real(fts_word_t *argv)
{
  fft_ctl_t *ctl = (fft_ctl_t *)fts_word_get_pointer(argv + 0);
  int n_tick = (int)fts_word_get_int(argv + 1);
  float *in0 = (float *)fts_word_get_pointer(argv + 2);
  float *out0 = (float *)fts_word_get_pointer(argv + 3);
  float *out1 = (float *)fts_word_get_pointer(argv + 4);
  float *buf = (float *)(ctl->buf);
  complex *spec = ctl->spec;
  int out_idx = ctl->out_idx;
  int in_idx = ctl->in_idx;
  int size = ctl->size;
  int i;

  ctl->gap_count -= n_tick;
  if(ctl->gap_count < 0)
    {
      for(i=0; i<n_tick; i++)
	buf[in_idx + i] = in0[i];

      in_idx += n_tick;
    }
  
  if(out_idx < size)
    {
      for(i=0; i<n_tick; i++)
	{
	  out0[i] = spec[out_idx + i].re;
	  out1[i] = spec[out_idx + i].im;
	}

      out_idx += n_tick;
    }
  else
    {
      for(i=0; i<n_tick; i++)
	out0[i] = out1[i] = 0.0;
    }

  if(in_idx >= size)
    {
      fts_rfft(buf, spec, size);
      complete_symetric_spectrum_inplc_after_rfft(spec, size);
      in_idx = out_idx = 0;
      ctl->gap_count = ctl->gap_size;
      fts_timebase_add_call(fts_get_timebase(), ctl->object, fft_output_bang, 0, 0.0);
    }
  
  ctl->out_idx = out_idx;
  ctl->in_idx = in_idx;
}

void
ftl_ifft_real(fts_word_t *argv)
{
  fft_ctl_t *ctl = (fft_ctl_t *)fts_word_get_pointer(argv + 0);
  int n_tick = (int)fts_word_get_int(argv + 1);
  float *in0 = (float *)fts_word_get_pointer(argv + 2);
  float *in1 = (float *)fts_word_get_pointer(argv + 3);
  float *out0 = (float *)fts_word_get_pointer(argv + 4);
  float *buf = (float *)ctl->buf;
  complex *spec = ctl->spec;
  int out_idx = ctl->out_idx;
  int in_idx = ctl->in_idx;
  int size = ctl->size;
  int i;

  ctl->gap_count -= n_tick;
  if(ctl->gap_count < 0)
    {
      if(in_idx >= size)
	{
	  fts_rifft(spec, buf, size);
	  in_idx = out_idx = 0;
	  ctl->gap_count = ctl->gap_size;
	  fts_timebase_add_call(fts_get_timebase(), ctl->object, fft_output_bang, 0, 0.0);
	}

      for(i=0; i<n_tick; i++)
	{
	  spec[in_idx + i].re = in0[i];
	  spec[in_idx + i].im = in1[i];
	}

      in_idx += n_tick;
    }

  if(out_idx < size)
    {
      for(i=0; i<n_tick; i++)
	out0[i] = buf[out_idx + i];

      out_idx += n_tick;
    }
  else
    {
      for(i=0; i<n_tick; i++)
	out0[i] = 0.0;
    }
  
  ctl->out_idx = out_idx;
  ctl->in_idx = in_idx;
}

void
ftl_fft_real_half(fts_word_t *argv)
{
  fft_ctl_t *ctl = (fft_ctl_t *)fts_word_get_pointer(argv + 0);
  int n_tick = (int)fts_word_get_int(argv + 1);
  float *in0 = (float *)fts_word_get_pointer(argv + 2);
  float *out0 = (float *)fts_word_get_pointer(argv + 3);
  float *out1 = (float *)fts_word_get_pointer(argv + 4);
  float *buf = (float *)ctl->buf;
  complex *spec = ctl->spec;
  int out_idx = ctl->out_idx;
  int in_idx = ctl->in_idx;
  int size = ctl->size;
  int spec_size = size >> 1; /* half of spectrum */
  int n_out_tick = n_tick >> 1; /* down sampling */
  int i;

  ctl->gap_count -= n_tick;
  if(ctl->gap_count < 0)
    {
      for(i=0; i<n_tick; i++)
	buf[in_idx + i] = in0[i];

      in_idx += n_tick;
    }

  if(out_idx < spec_size)
    {
      /* down sampled output */
      for(i=0; i<n_out_tick; i++)
	{
	  out0[i] = spec[out_idx + i].re;
	  out1[i] = spec[out_idx + i].im;
	}

      out_idx += n_out_tick;
    }
  else
    {
      for(i=0; i<n_out_tick; i++)
	out0[i] = out1[i] = 0.0;
    }

  if(in_idx >= size)
    {
      fts_rfft(buf, spec, size);
      in_idx = out_idx = 0;
      ctl->gap_count = ctl->gap_size;
      fts_timebase_add_call(fts_get_timebase(), ctl->object, fft_output_bang, 0, 0.0);
    }
  
  ctl->out_idx = out_idx;
  ctl->in_idx = in_idx;
}

void
ftl_ifft_real_half(fts_word_t *argv)
{
  fft_ctl_t *ctl = (fft_ctl_t *)fts_word_get_pointer(argv + 0);
  int n_tick = (int)fts_word_get_int(argv + 1);
  float *in0 = (float *)fts_word_get_pointer(argv + 2);
  float *in1 = (float *)fts_word_get_pointer(argv + 3);
  float *out0 = (float *)fts_word_get_pointer(argv + 4);
  float *buf = (float *)ctl->buf;
  complex *spec = ctl->spec;
  int out_idx = ctl->out_idx;
  int in_idx = ctl->in_idx;
  int size = ctl->size;
  int spec_size = size >> 1; /* half of spectrum */
  int n_out_tick = n_tick << 1; /* up sampling */
  int i;

  ctl->gap_count -= n_tick;
  if(ctl->gap_count < 0)
    {
      for(i=0; i<n_tick; i++)
	{
	  spec[in_idx + i].re = in0[i];
	  spec[in_idx + i].im = in1[i];
	} 

      /* down sampled input */
      in_idx += n_tick;
    }

  if(out_idx < size)
    {
      for(i=0; i<n_out_tick; i++)
	out0[i] = buf[out_idx + i];

      out_idx += n_out_tick;
    }
  else
    {
      for(i=0; i<n_out_tick; i++)
	out0[i] = 0.0;
    }

  if(in_idx >= spec_size)
    {
      fts_rifft(spec, buf, size);
      in_idx = out_idx = 0;
      ctl->gap_count = ctl->gap_size;
      fts_timebase_add_call(fts_get_timebase(), ctl->object, fft_output_bang, 0, 0.0);
    }
  
  ctl->out_idx = out_idx;
  ctl->in_idx = in_idx;
}

/*** tandem ***/
 
void
ftl_fft_tandem(fts_word_t *argv)
{
  fft_ctl_t *ctl = (fft_ctl_t *)fts_word_get_pointer(argv + 0);
  int n_tick = (int)fts_word_get_int(argv + 1);
  float *in0 = (float *)fts_word_get_pointer(argv + 2);
  float *in1 = (float *)fts_word_get_pointer(argv + 3);
  float *out0 = (float *)fts_word_get_pointer(argv + 4);
  float *out1 = (float *)fts_word_get_pointer(argv + 5);
  float *out2 = (float *)fts_word_get_pointer(argv + 6);
  float *out3 = (float *)fts_word_get_pointer(argv + 7);
  complex *buf = ctl->buf;
  complex *spec = ctl->spec;
  int out_idx = ctl->out_idx;
  int in_idx = ctl->in_idx;
  int size = ctl->size;
  int i;
  
  ctl->gap_count -= n_tick;
  if(ctl->gap_count < 0)
    {
      for(i=0; i<n_tick; i++)
	{
	  buf[in_idx + i].re = in0[i];
	  buf[in_idx + i].im = in1[i];
	}

      in_idx += n_tick;
    }
  
  if(out_idx < size)
    {
      for(i=0; i<n_tick; i++)
	{
	  out0[i] = spec[out_idx + i].re;
	  out1[i] = spec[out_idx + i].im;
	  out2[i] = spec[size + out_idx + i].re;
	  out3[i] = spec[size + out_idx + i].im;
	}

      out_idx += n_tick;
    }
  else
    {
      for(i=0; i<n_tick; i++)
	out0[i] = out1[i] = out2[i] = out3[i] = 0.0;
    }

  if(in_idx >= size)
    {
      fts_cfft_inplc(buf, size);
      shuffle_after_cfft_for_tandem(buf, spec, spec + size, size);
      in_idx = out_idx = 0;
      ctl->gap_count = ctl->gap_size;
      fts_timebase_add_call(fts_get_timebase(), ctl->object, fft_output_bang, 0, 0.0);
    }
  
  ctl->out_idx = out_idx;
  ctl->in_idx = in_idx;
}

void
ftl_ifft_tandem(fts_word_t *argv)
{
  fft_ctl_t *ctl = (fft_ctl_t *)fts_word_get_pointer(argv + 0);
  int n_tick = (int)fts_word_get_int(argv + 1);
  float *in0 = (float *)fts_word_get_pointer(argv + 2);
  float *in1 = (float *)fts_word_get_pointer(argv + 3);
  float *in2 = (float *)fts_word_get_pointer(argv + 4);
  float *in3 = (float *)fts_word_get_pointer(argv + 5);
  float *out0 = (float *)fts_word_get_pointer(argv + 6);
  float *out1 = (float *)fts_word_get_pointer(argv + 7);
  complex *buf = ctl->buf;
  complex *spec = ctl->spec;
  int out_idx = ctl->out_idx;
  int in_idx = ctl->in_idx;
  int size = ctl->size;
  int i;
  
  ctl->gap_count -= n_tick;
  if(ctl->gap_count < 0)
    {
      for(i=0; i<n_tick; i++)
	{
	  spec[in_idx + i].re = in0[i];
	  spec[in_idx + i].im = in1[i];
	  spec[size + in_idx + i].re = in2[i];
	  spec[size + in_idx + i].im = in3[i];
	}

      in_idx += n_tick;
    }

  if(out_idx < size)
    {
      for(i=0; i<n_tick; i++)
	{
	  out0[i] = buf[out_idx + i].re;
	  out1[i] = buf[out_idx + i].im;
	}

      out_idx += n_tick;
    }
  else
    {
      for(i=0; i<n_tick; i++)
	out0[i] = out1[i] = 0.0;
    }

  if(in_idx >= size)
    {
      shuffle_before_cifft_for_tandem(spec, spec + size, buf, size);
      fts_cifft_inplc(buf, size);
      in_idx = out_idx = 0;
      ctl->gap_count = ctl->gap_size;
      fts_timebase_add_call(fts_get_timebase(), ctl->object, fft_output_bang, 0, 0.0);
    }
  
  ctl->out_idx = out_idx;
  ctl->in_idx = in_idx;
}

void
ftl_fft_tandem_half(fts_word_t *argv)
{
  fft_ctl_t *ctl = (fft_ctl_t *)fts_word_get_pointer(argv + 0);
  int n_tick = (int)fts_word_get_int(argv + 1);
  float *in0 = (float *)fts_word_get_pointer(argv + 2);
  float *in1 = (float *)fts_word_get_pointer(argv + 3);
  float *out0 = (float *)fts_word_get_pointer(argv + 4);
  float *out1 = (float *)fts_word_get_pointer(argv + 5);
  float *out2 = (float *)fts_word_get_pointer(argv + 6);
  float *out3 = (float *)fts_word_get_pointer(argv + 7);
  complex *buf = ctl->buf;
  complex *spec = ctl->spec;
  int out_idx = ctl->out_idx;
  int in_idx = ctl->in_idx;
  int size = ctl->size;
  int spec_size = size >> 1; /* half of spectrum */
  int n_out_tick = n_tick >> 1; /* down sampling */
  int i;

  ctl->gap_count -= n_tick;
  if(ctl->gap_count < 0)
    {
      for(i=0; i<n_tick; i++)
	{
	  buf[in_idx + i].re = in0[i];
	  buf[in_idx + i].im = in1[i];
	}
      
      in_idx += n_tick;
    }

  if(out_idx < spec_size)
    {
      for(i=0; i<n_out_tick; i++)
	{
	  out0[i] = spec[out_idx + i].re;
	  out1[i] = spec[out_idx + i].im;
	  out2[i] = spec[spec_size + out_idx + i].re;
	  out3[i] = spec[spec_size + out_idx + i].im;
	}

      out_idx += n_out_tick;
    }
  else
    {
      for(i=0; i<n_out_tick; i++)
	out0[i] = out1[i] = out2[i] = out3[i] = 0.0;
    }

  if(in_idx >= size)
    {
      fts_cfft_inplc(buf, size);
      shuffle_after_cfft_for_tandem_half(buf, spec, spec + spec_size, size);
      in_idx = out_idx = 0;
      ctl->gap_count = ctl->gap_size;
      fts_timebase_add_call(fts_get_timebase(), ctl->object, fft_output_bang, 0, 0.0);
    }
  
  ctl->out_idx = out_idx;
  ctl->in_idx = in_idx;
}

void
ftl_ifft_tandem_half(fts_word_t *argv)
{
  fft_ctl_t *ctl = (fft_ctl_t *)fts_word_get_pointer(argv + 0);
  int n_tick = (int)fts_word_get_int(argv + 1);
  float *in0 = (float *)fts_word_get_pointer(argv + 2);
  float *in1 = (float *)fts_word_get_pointer(argv + 3);
  float *in2 = (float *)fts_word_get_pointer(argv + 4);
  float *in3 = (float *)fts_word_get_pointer(argv + 5);
  float *out0 = (float *)fts_word_get_pointer(argv + 6);
  float *out1 = (float *)fts_word_get_pointer(argv + 7);
  complex *buf = ctl->buf;
  complex *spec = ctl->spec;
  int out_idx = ctl->out_idx;
  int in_idx = ctl->in_idx;
  int size = ctl->size;
  int spec_size = size >> 1;
  int n_out_tick = n_tick << 1; /* up sampling */
  int i;
  
  ctl->gap_count -= n_tick; 
  if(ctl->gap_count < 0)
    {
      for(i=0; i<n_tick; i++)
	{
	  spec[in_idx + i].re = in0[i];
	  spec[in_idx + i].im = in1[i];
	  spec[spec_size + in_idx + i].re = in2[i];
	  spec[spec_size + in_idx + i].im = in3[i];
	} 

      /* down sampled input */
      in_idx += n_tick;
    }

  if(out_idx < size)
    {
      for(i=0; i<n_out_tick; i++)
	{
	  out0[i] = buf[out_idx + i].re;
	  out1[i] = buf[out_idx + i].im;
	}

      out_idx += n_out_tick;
    }
  else
    {
      for(i=0; i<n_out_tick; i++)
	out0[i] = out1[i] = 0.0;
    }

  if(in_idx >= spec_size)
    {
      shuffle_before_cifft_for_tandem_half(spec, spec + spec_size, buf, size);
      fts_cifft_inplc(buf, size);
      in_idx = out_idx = 0;
      ctl->gap_count = ctl->gap_size;
      fts_timebase_add_call(fts_get_timebase(), ctl->object, fft_output_bang, 0, 0.0);
    }
  
  ctl->out_idx = out_idx;
  ctl->in_idx = in_idx;
}


/*** miller ***/
 
void
ftl_fft_real_miller(fts_word_t *argv)
{
  fft_ctl_t *ctl = (fft_ctl_t *)fts_word_get_pointer(argv + 0);
  int n_tick = (int)fts_word_get_int(argv + 1);
  float *in0 = (float *)fts_word_get_pointer(argv + 2);
  float *out0 = (float *)fts_word_get_pointer(argv + 3);
  float *out1 = (float *)fts_word_get_pointer(argv + 4);
  float *buf = (float *)ctl->buf;
  complex *spec = ctl->spec;
  int out_idx = ctl->out_idx;
  int in_idx = ctl->in_idx;
  int size = ctl->size;
  int spec_size = size >> 1; /* half of spectrum */
  int i;

  ctl->gap_count -= n_tick;
  if(ctl->gap_count < 0)
    {
      for(i=0; i<n_tick; i++)
	buf[in_idx + i] = in0[i];

      in_idx += n_tick;
    }

  if(out_idx < spec_size)
    {
      for(i=0; i<n_tick; i++)
	{
	  out0[i] = spec[out_idx + i].re;
	  out1[i] = spec[out_idx + i].im;
	}

      out_idx += n_tick;
    }
  else
    {
      for(i=0; i<n_tick; i++)
	out0[i] = out1[i] = 0.0;
    }

  if(in_idx >= size)
    {
      fts_rfft(buf, spec, size);
      in_idx = out_idx = 0;
      ctl->gap_count = ctl->gap_size;
      fts_timebase_add_call(fts_get_timebase(), ctl->object, fft_output_bang, 0, 0.0);
    }
  
  ctl->out_idx = out_idx;
  ctl->in_idx = in_idx;
}

void
ftl_ifft_real_miller(fts_word_t *argv)
{
  fft_ctl_t *ctl = (fft_ctl_t *)fts_word_get_pointer(argv + 0);
  int n_tick = (int)fts_word_get_int(argv + 1);
  float *in0 = (float *)fts_word_get_pointer(argv + 2);
  float *in1 = (float *)fts_word_get_pointer(argv + 3);
  float *out0 = (float *)fts_word_get_pointer(argv + 4);
  float *buf = (float *)ctl->buf;
  complex *spec = ctl->spec;
  int out_idx = ctl->out_idx;
  int in_idx = ctl->in_idx;
  int size = ctl->size;
  int spec_size = size >> 1; /* half of spectrum */
  int i;

  ctl->gap_count -= n_tick;
  if(ctl->gap_count < 0)
    {
      if(in_idx < spec_size)
	{
	  for(i=0; i<n_tick; i++)
	    {
	      spec[in_idx + i].re = in0[i];
	      spec[in_idx + i].im = in1[i];
	    }
	}

      in_idx += n_tick;
    }

  if(out_idx < size)
    {
      for(i=0; i<n_tick; i++)
	out0[i] = buf[out_idx + i];

      out_idx += n_tick;
    }
  else
    {
      for(i=0; i<n_tick; i++)
	out0[i] = 0.0;
    }

  if(in_idx >= size)
    {
      fts_rifft(spec, buf, size);
      in_idx = out_idx = 0;
      ctl->gap_count = ctl->gap_size;
      fts_timebase_add_call(fts_get_timebase(), ctl->object, fft_output_bang, 0, 0.0);
    }
  
  ctl->out_idx = out_idx;
  ctl->in_idx = in_idx;
}

void
ftl_fft_tandem_miller(fts_word_t *argv)
{
  fft_ctl_t *ctl = (fft_ctl_t *)fts_word_get_pointer(argv + 0);
  int n_tick = (int)fts_word_get_int(argv + 1);
  float *in0 = (float *)fts_word_get_pointer(argv + 2);
  float *in1 = (float *)fts_word_get_pointer(argv + 3);
  float *out0 = (float *)fts_word_get_pointer(argv + 4);
  float *out1 = (float *)fts_word_get_pointer(argv + 5);
  float *out2 = (float *)fts_word_get_pointer(argv + 6);
  float *out3 = (float *)fts_word_get_pointer(argv + 7);
  complex *buf = ctl->buf;
  complex *spec = ctl->spec;
  int out_idx = ctl->out_idx;
  int in_idx = ctl->in_idx;
  int size = ctl->size;
  int spec_size = size >> 1; /* half of spectrum */
  int i;

  ctl->gap_count -= n_tick;
  if(ctl->gap_count < 0)
    {
      for(i=0; i<n_tick; i++)
	{
	  buf[in_idx + i].re = in0[i];
	  buf[in_idx + i].im = in1[i];
	}

      in_idx += n_tick;
    }

  if(out_idx < spec_size)
    {
      for(i=0; i<n_tick; i++)
	{
	  out0[i] = spec[out_idx + i].re;
	  out1[i] = spec[out_idx + i].im;
	  out2[i] = spec[spec_size + out_idx + i].re;
	  out3[i] = spec[spec_size + out_idx + i].im;
	}

      out_idx += n_tick;
    }
  else
    {
      for(i=0; i<n_tick; i++)
	out0[i] = out1[i] = out2[i] = out3[i] = 0.0;
    }

  if(in_idx >= size)
    {
      fts_cfft_inplc(buf, size);
      shuffle_after_cfft_for_tandem_half(buf, spec, spec + spec_size, size);
      in_idx = out_idx = 0;
      ctl->gap_count = ctl->gap_size;
      fts_timebase_add_call(fts_get_timebase(), ctl->object, fft_output_bang, 0, 0.0);
    }
  
  ctl->out_idx = out_idx;
  ctl->in_idx = in_idx;
}

void
ftl_ifft_tandem_miller(fts_word_t *argv)
{
  fft_ctl_t *ctl = (fft_ctl_t *)fts_word_get_pointer(argv + 0);
  int n_tick = (int)fts_word_get_int(argv + 1);
  float *in0 = (float *)fts_word_get_pointer(argv + 2);
  float *in1 = (float *)fts_word_get_pointer(argv + 3);
  float *in2 = (float *)fts_word_get_pointer(argv + 4);
  float *in3 = (float *)fts_word_get_pointer(argv + 5);
  float *out0 = (float *)fts_word_get_pointer(argv + 6);
  float *out1 = (float *)fts_word_get_pointer(argv + 7);
  complex *buf = ctl->buf;
  complex *spec = ctl->spec;
  int out_idx = ctl->out_idx;
  int in_idx = ctl->in_idx;
  int size = ctl->size;
  int spec_size = size >> 1;
  int i;
  
  ctl->gap_count -= n_tick; 
  if(ctl->gap_count < 0)
    {
      if(in_idx < spec_size)
	{
	  for(i=0; i<n_tick; i++)
	    {
	      spec[in_idx + i].re = in0[i];
	      spec[in_idx + i].im = in1[i];
	      spec[spec_size + in_idx + i].re = in2[i];
	      spec[spec_size + in_idx + i].im = in3[i];
	    }
	}
      in_idx += n_tick;
    }

  if(out_idx < size)
    {
      for(i=0; i<n_tick; i++)
	{
	  out0[i] = buf[out_idx + i].re;
	  out1[i] = buf[out_idx + i].im;
	}

      out_idx += n_tick;
    }
  else
    {
      for(i=0; i<n_tick; i++)
	out0[i] = out1[i] = 0.0;
    }

  if(in_idx >= size)
    {
      shuffle_before_cifft_for_tandem_half(spec, spec + spec_size, buf, size);
      fts_cifft_inplc(buf, size);
      in_idx = out_idx = 0;
      ctl->gap_count = ctl->gap_size;
      fts_timebase_add_call(fts_get_timebase(), ctl->object, fft_output_bang, 0, 0.0);
    }
  
  ctl->out_idx = out_idx;
  ctl->in_idx = in_idx;
}


/**************************************************************
 *
 *    this will be moved to the veclib... first lets test it...
 *
 */
 
void shuffle_after_cfft_for_tandem(complex *in, complex *out0, complex *out1, int n_points)
{
  int index, xedni;
  complex out0_index, out1_index;
    
  out0[0].re = in[0].re;
  out0[0].im = 0;
  out1[0].re = in[0].im;
  out1[0].im = 0;

  for(index=1, xedni=n_points-1; index<n_points>>1; index++, xedni--)
    {
      out0_index.re = 0.5f * (in[index].re + in[xedni].re);
      out0_index.im = 0.5f * (in[index].im - in[xedni].im);
      out1_index.re = 0.5f * (in[xedni].im + in[index].im);
      out1_index.im = 0.5f * (in[xedni].re - in[index].re);
      out0[index].re = out0_index.re;
      out0[xedni].re = out0_index.re;
      out1[index].re = out1_index.re;
      out1[xedni].re = out1_index.re;
      out0[index].im = out0_index.im;
      out0[xedni].im = -out0_index.im;
      out1[index].im = out1_index.im;
      out1[xedni].im = -out1_index.im;
    }
  out0[index].re = in[index].re;
  out0[index].im = 0.0f;
  out1[index].re = in[index].im;
  out1[index].im = 0.0f;
}

void shuffle_before_cifft_for_tandem(complex *in0, complex *in1, complex *out, int n_points)
{
  int index;
  complex out_0, out_1;

  out_0.re = in0[0].re;
  out_0.im = in1[0].re;
  out_1.re = in0[1].re - in1[1].im;
  out_1.im = in0[1].im + in1[1].re;
  out[0].re = out_0.re;
  out[0].im = out_0.im;
  out[1].re = out_1.re;
  out[1].im = out_1.im;

  for(index=2; index<n_points; index+=2)
    {
      out_0.re = in0[index].re - in1[index].im;
      out_0.im = in0[index].im + in1[index].re;
      out_1.re = in0[index + 1].re - in1[index + 1].im;
      out_1.im = in0[index + 1].im + in1[index + 1].re;
      out[index].re = out_0.re;
      out[index].im = out_0.im;
      out[index + 1].re = out_1.re;
      out[index + 1].im = out_1.im;
    }
}

void shuffle_after_cfft_for_tandem_half(complex *in, complex *out0, complex *out1, int n_points)
{
  int index, xedni;
  complex out0_index, out1_index;
    
  out0[0].re = in[0].re;
  out0[0].im = 0;
  out1[0].re = in[0].im;
  out1[0].im = 0;

  for(index=1, xedni=n_points-1; index<n_points>>1; index++, xedni--)
    {
      out0_index.re = 0.5f * (in[index].re + in[xedni].re);
      out0_index.im = 0.5f * (in[index].im - in[xedni].im);
      out1_index.re = 0.5f * (in[xedni].im + in[index].im);
      out1_index.im = 0.5f * (in[xedni].re - in[index].re);
      out0[index].re = out0_index.re;
      out1[index].re = out1_index.re;
      out0[index].im = out0_index.im;
      out1[index].im = out1_index.im;
    }
}

void shuffle_before_cifft_for_tandem_half(complex *in0, complex *in1, complex *out, int n_points)
{
  int index, xedni;
  complex out_index, out_xedni;

  out[0].re = in0[0].re;
  out[0].im = in1[0].re;

  for(index=1, xedni=n_points-1; index<n_points>>1; index++, xedni--)
    {
      out_index.re = in0[index].re - in1[index].im;
      out_index.im = in0[index].im + in1[index].re;
      out_xedni.re = in1[index].im + in0[index].re;
      out_xedni.im = in1[index].re - in0[index].im;
      out[index].re = out_index.re;
      out[index].im = out_index.im;
      out[xedni].re = out_xedni.re;
      out[xedni].im = out_xedni.im;
    }
  out[index].re = 0;
  out[index].im = 0;
}

void complete_symetric_spectrum_inplc_after_rfft(complex *buf, int n_points)
{
  int index, xedni;
  
  buf[n_points - 1].re = buf[1].re;
  buf[n_points - 1].im = -buf[1].im;
  buf[n_points - 2].re = buf[2].re;
  buf[n_points - 2].im = -buf[2].im;
  buf[n_points - 3].re = buf[3].re;
  buf[n_points - 3].im = -buf[3].im;

  for(index=4, xedni=n_points-4; index<n_points>>1; index+=4, xedni-=4)
    {
      buf[xedni].re = buf[index].re;
      buf[xedni].im = -buf[index].im;
      buf[xedni - 1].re = buf[index + 1].re;
      buf[xedni - 1].im = -buf[index + 1].im;
      buf[xedni - 2].re = buf[index + 2].re;
      buf[xedni - 2].im = -buf[index + 2].im;
      buf[xedni - 3].re = buf[index + 3].re;
      buf[xedni - 3].im = -buf[index + 3].im;
    }
  buf[index].re = 0;
}
