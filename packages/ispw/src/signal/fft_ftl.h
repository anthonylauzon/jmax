/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */


/* the control structure */
typedef struct{
  complex *buf; /* input buffer */
  complex *spec; /* buffer for spectrum (has double size in tandem mode) */
  long in_idx;
  long out_idx;
  int gap_size; /* number of samples to wait before collecting new input */
  int gap_count; /* counts down gap_ticks */
  long size; /* number of complex points in buffer */
  fts_alarm_t alarm;
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
