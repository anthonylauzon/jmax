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

#include <utils.h>

/* the control structure */
typedef struct
{
  fts_object_t *object;
  complex *buf; /* input buffer */
  complex *spec; /* buffer for spectrum (has double size in tandem mode) */
  long in_idx;
  long out_idx;
  int gap_size; /* number of samples to wait before collecting new input */
  int gap_count; /* counts down gap_ticks */
  long size; /* number of complex points in buffer */
} fft_ctl_t;

/* the dsp symbols */

extern fts_symbol_t
  dsp_sym_fft_complex,
  dsp_sym_ifft_complex,
  dsp_sym_fft_real,
  dsp_sym_ifft_real,
  dsp_sym_fft_real_half,
  dsp_sym_ifft_real_half,
  dsp_sym_fft_tandem,
  dsp_sym_ifft_tandem,
  dsp_sym_fft_tandem_half,
  dsp_sym_ifft_tandem_half,

  dsp_sym_fft_real_miller,
  dsp_sym_ifft_real_miller,
  dsp_sym_fft_tandem_miller,
  dsp_sym_ifft_tandem_miller;

extern void ftl_fft_init(void);

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

extern void fft_output_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
