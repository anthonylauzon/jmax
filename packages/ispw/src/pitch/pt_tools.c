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
#include <math.h>
#include "pt_obj.h"
#include "pt_tools.h"

#define FTS_PI (3.141592654f)
#define FTS_TWO_PI (6.283185307f)

typedef struct{
  kernel_t *kernels;    /* the filter kernels */
  int       n_channels;  /* # of filter channels */
  float    *all_coeffs;  /* vector of all filter coefficients */
  int       n_all_coeffs;  /* total number of filter coefficients */
  int       channels_per_octave;
  float     q_factor;
  float     coeff_cut;
} filter_bank_t;

/* only local function */
static filter_bank_t *filter_bank_init(int channels_per_octave, float coeff_cut, int n_points);
 
/*************************************************
 *
 *    the common tables for the pt classes and all that shit
 *    and their init funs
 */
 
static filter_bank_t *filter_bank_max = (filter_bank_t *)0;
static int millers_dumbtabs_are_allready_initialized = 0;
static float millers_dumbtab1[256];
static float millers_dumbtab2[1024];

/*************************************************
 *
 *    creating Miller's pt filter bank kernels
 *    and print them
 */

void 
pt_common_init_millers_kernels(void)
{
  if(!filter_bank_max)
    filter_bank_max = filter_bank_init(pt_common_CHANNELS_PER_OCTAVE, -33.0f, pt_common_MAX_n_points);
}

void 
pt_common_print_millers_kernels(int channel)
{
  if(channel < 0 || channel >= filter_bank_max->n_channels){
    post("filter bank:\n");
    post("# of filters: %d\n", filter_bank_max->n_channels);
    post("coeffs in total: %d\n", filter_bank_max->n_all_coeffs);
    post("channels per octave: %d\n", filter_bank_max->channels_per_octave);
    post("q-factor: %f\n", filter_bank_max->q_factor);
    post("coefficient cutoff: %f\n", filter_bank_max->coeff_cut);
  }else{
    int j;
    post("___________________\n");
    post("kernel #%d\n", channel);
    post("  onset: %d\n",filter_bank_max->kernels[channel].onset);
    post("  correct: %f\n", filter_bank_max->kernels[channel].pow_corr);
    post("  %d coeffs:\n", filter_bank_max->kernels[channel].n_filter_coeffs);
    for(j=0; j<filter_bank_max->kernels[channel].n_filter_coeffs; j++)
      post("\t%f\n", filter_bank_max->kernels[channel].filter_coeffs[j]);
  }
}

/*************************************************
 *
 *    Miller's "bounded Q"
 */


void 
pt_common_millers_bounded_q(complex *fft_spectrum, kernel_t *filter, float *q_pow_spec, int n_channels)
{
  int i, j;
  for(i=0; i<n_channels; i++){
    float re = 0;
    float im = 0;
    complex *band = fft_spectrum + filter[i].onset/2; /* onset made for float buffer */
    
    for(j=0; j<filter[i].n_filter_coeffs; j++){
      float coef = filter[i].filter_coeffs[j];
      re += band[j].re * coef;
      im += band[j].im * coef;
    }
    q_pow_spec[i] = re * re + im * im;
  }
}


/*************************************************
 *
 *    Miller's mean power
 */

float
pt_common_millers_mean_power(float *sig, float *temp, float *wind, int n_points)
{
  int i;
  float pow = 0.0f;
	
  for(i=0; i<n_points; i++)
    pow += sig[i] * sig[i] * wind[i];

  return(pow);
}

  
/*************************************************
 *
 *    Miller's dum sqrt (ask him what it does)
 */
#if 0
void 
pt_common_init_millers_dumsqrt(void)
{
  int i;
  
  if(millers_dumbtabs_are_allready_initialized) return;
  
  for (i=0; i<256; i++){
    float f;
    long l = (i ? (i == 256-1 ? 256-2 : i) : 1)<< 23;
    *(long *)(&f) = l;
    millers_dumbtab1[i] = sqrt(f);  
  }
  for (i=0; i<1024; i++){
    float f = 1 + (1.0f/1024) * i;
    millers_dumbtab2[i] = sqrt(f);  
  }
  millers_dumbtabs_are_allready_initialized = 1;
}

double pt_common_millers_dumsqrt(double d)
{
  float f = d;
  long l = *(long *)(&f);
  if (f < 0) return (0.0f);
  else return (millers_dumbtab1[(l >> 23) & 0xff] * millers_dumbtab2[(l >> 13) & 0x3ff]);
}
#endif

/*************************************************
 *
 *    Miller's fundamental pitch algorithm
 */
int 
pt_common_find_pitch_candidate(pt_common_obj_t *x, float *candidate, float *pitch_power, float *total_power, int print_out)
{
  complex *fft_buf = x->buf.for_fft;
  int n_points = x->n_points;
  int n_channels = x->n_channels;
  float q_pow_spec[pt_common_MAX_n_channels + pt_common_EXTRA_CHANNELS];
  float q_amp_spec[pt_common_MAX_n_channels + pt_common_EXTRA_CHANNELS];
  float tone_weights[pt_common_MAX_n_channels];
  float *found;
  int index;
  float correct;
  int i;

  fts_vecx_fcpyre(x->buf.main, x->buf.for_fft, x->n_points);
  fts_vecx_ffillim(0.0f, x->buf.for_fft, x->n_points);

  /* get a bounded-Q power spectrum, quarter-tone spaced */
     
  fts_cfft_inplc(fft_buf, n_points); 
/*     
  if(print_out >= 2){
    post("  raw fft output:\n");
    for(i=0; i<n_points; i+=2)
      post("    #%d: %f %f, %f %f\n", i, fft_buf[i].re, fft_buf[i].im, fft_buf[i+1].re, fft_buf[i+1].im);
  }
*/
  pt_common_millers_bounded_q((complex *)fft_buf, filter_bank_max->kernels, q_pow_spec, n_channels);
  for(i=n_channels; i<n_channels+pt_common_EXTRA_CHANNELS; i++) q_pow_spec[i] = 0; /* zero end of buffer */
/*
  if(print_out >= 2){
    post("  raw filter output:\n");
    for(i=0; i<n_channels; i+=4)
      post("    #%d: %f %f %f %f\n", i, q_pow_spec[i], q_pow_spec[i+1], q_pow_spec[i+2], q_pow_spec[i+3]);
  }
*/
  /* compute amplitudes (????... ask Miller why and why like this!) */
  for(i=0; i<n_channels; i++) q_amp_spec[i] = pt_common_millers_dumsqrt(q_pow_spec[i]);
  for( ; i<n_channels+pt_common_EXTRA_CHANNELS; i++) q_amp_spec[i] = 0;
  
/*
  if(print_out >= 2){
    post("  filter bands:\n");
    for(i=0; i<n_channels; i+=4)
      post("    #%d: %f %f %f %f\n", i, q_amp_spec[i], q_amp_spec[i+1], q_amp_spec[i+2], q_amp_spec[i+3]);
  }
*/

  /* find tone (index) with maximal power */
  {
    float best_weight = 0;
    float *coeffs = x->ctl.coeffs;
    int index_low = x->stat.index_low;
    int index_high = x->stat.index_high;
    int last_index = x->stat.last_index;

    /* look for bin with max. power in itself and first few partials */
    index = 0;
    for(i=0; i<n_channels; i++){ /* for each filter band */
      float *bin = q_amp_spec + i;
      float tone_weight =
        bin[PB_0] * coeffs[0] + bin[PB_1] * coeffs[1] +
        bin[PB_2] * coeffs[2] + bin[PB_3] * coeffs[3] +
        bin[PB_4] * coeffs[4] + bin[PB_5] * coeffs[5] +
        bin[PB_6] * coeffs[6] + bin[PB_7] * coeffs[7];
      tone_weights[i] = tone_weight;
      if(
        tone_weight > best_weight &&
        i >= index_low &&
        i <= index_high
      ){
        best_weight = tone_weight;
        index = i;
      }
    }

    if(index < index_low) return(0);

    /* possibly stick to previous pitch */
    if(last_index){
      if(tone_weights[last_index-1] > tone_weights[last_index] && last_index > index_low) last_index--;
      else if(tone_weights[last_index+1] > tone_weights[last_index] && last_index < index_high) last_index++;
      if(tone_weights[last_index] > (1.0f - x->ctl.pitch_stick) * best_weight) index = last_index;
    }
    found = q_pow_spec + index;
  }
  
/*
  if(print_out){
    post("  candidate\n");
    post("    index: %d\n", index);
  }
*/
  
  /* total and harmonic power */
  {
    kernel_t *band = filter_bank_max->kernels;
    
    /* get the total power (or at least something proportional) */  
    *total_power = 0.0f;
    for(i=0; i<n_channels; i++) *total_power += q_pow_spec[i] * band[i].pow_corr;

    /* get the "harmonic" power */
    *pitch_power =
      found[PB_0] + found[PB_1] + found[PB_2] + found[PB_3] +
      found[PB_4] + found[PB_5] + found[PB_6] + found[PB_7];
  
    /* get the total power from DC up to the 8th harmonic.
     * The pow_corr factor is to correct for the fact that the
     * filterbank overlap is greater in the bands and so
     * merely summing them would weight them too highly.
     */
  }

/*
  if(print_out){
    float quality = (*total_power > 0.0f)? (100.0f * sqrt(*pitch_power / *total_power)): (0.0f);
    post("    power: %f pitched, %f total (%f%%)\n",
      sqrt(*pitch_power)/n_points,
      sqrt(*total_power)/n_points,
      quality
    );
  }
*/

  /* do a parabolic correction to increase precision */
  if(!x->ctl.pitch_rough){
    float lower = (
      found[PB_0-1] + 2.0f * found[PB_1-1] + 3.0f * found[PB_2-1] + 4.0f * found[PB_3-1] +
      5.0f * found[PB_4-1] + 6.0f * found[PB_5-1] + 7.0f * found[PB_6-1] + 8.0f * found[PB_7-1]
    );
    float this = (
      found[PB_0] + 2.0f * found[PB_1] + 3.0f * found[PB_2] + 4.0f * found[PB_3] +
      5.0f * found[PB_4] + 6.0f * found[PB_5] + 7.0f * found[PB_6] + 8.0f * found[PB_7]
    );  
    float higher = (
      found[PB_0+1] + 2.0f * found[PB_1+1] + 3.0f * found[PB_2+1] + 4.0f * found[PB_3+1] +
      5.0f * found[PB_4+1] + 6.0f * found[PB_5+1] + 7.0f * found[PB_6+1] + 8.0f * found[PB_7+1]
    );  
  
    if(lower >= this) correct = -0.5f;
    else if(higher >= this) correct = 0.5f;
    else{
      float a = 1.0f - lower / this;
      float b = 1.0f - higher / this;
      correct = (a - b) / (2.0f * (a + b) - 3.0f * a * b);
    }
    *candidate = index + correct;
  }else{
    *candidate = index;
  }
  
  /* pitch o.k.? */
  if(
    *pitch_power < x->ctl.power_off ||
    *pitch_power < x->ctl.quality_off * *total_power
  ){
    x->stat.last_index = 0;
    return(0);
  }
  
  /* remember the index for pitch-stick test next time if note is still really on */
  if(*pitch_power <= x->ctl.power_on)
    x->stat.last_index = 0; /* there is a candidate, but forget it */
  else
    x->stat.last_index = index;

  return(1);
}

float pt_common_candidate_frequency(pt_common_obj_t *x, float candidate)
{
  return(x->stat.freq_base * exp(0.028881133f * candidate)); /*  0.028881133 = ln(2.0f) / 24 */
}

float pt_common_candidate_midi_pitch(pt_common_obj_t *x, float candidate)
{
  return(x->stat.pitch_base + x->ctl.pitch_stretch * 0.5f * candidate);
}

/*************************************************
 *
 *    the filter bank initialisation (maybe sometimes usefull)
 *    ... derived from Millers code
 */
 
static filter_bank_t *
filter_bank_init(int channels_per_octave, float coeff_cut, int n_points)
{
  int i;
  int n_channels = (channels_per_octave * (log(n_points) * 1.442695041f - 2)); /* 1.0f/ln(2) = 1.442695041 */
    /* - 2: drops negative frequencies and lowest two points */
  float q_factor = exp(0.693147181f / (float) channels_per_octave) - 1.0f; /* ln(2) = 0.693147181 */
  int coeff_space = (4 * n_channels + 2 * n_points);
    /* estimates total size of kernels: cover the whole spectrum 4 times and overlap 4 points apiece */
  float cutoff = exp(2.302585093f * coeff_cut/20); /* ln(10) = 2.302585093 */
  int n_all_coeffs = 0;
  complex *fft_buf = (complex *)0;
  filter_bank_t *filter_bank = (filter_bank_t *)0;
  kernel_t *kernels = (kernel_t *)0;
  float *all_coeffs = (float *)0;
  float freq;
  
  fft_buf = (complex *)fts_malloc(n_points * sizeof(complex));
  if ( !fft_buf)
    { 
      post("pt: filter bank init: can not allocate FFT buffer!\n");
      return(0);
    }

  filter_bank = (filter_bank_t *)fts_malloc(sizeof(filter_bank_t));
  if ( !filter_bank)
    {
      fts_free(fft_buf);
      post("pt: filter bank init: can not allocate structure!\n");
      return(0);
    }

  kernels = (kernel_t *)fts_malloc(n_channels * sizeof(kernel_t));
  if ( !kernels)
    {
      fts_free(fft_buf);
      fts_free(filter_bank);
      post("pt: filter bank init: can not allocate kernels!\n");
      return(0);
    }

  all_coeffs = (float *)fts_malloc(coeff_space * sizeof(float));
  if ( !all_coeffs)
    {
      fts_free(fft_buf);
      fts_free(filter_bank);
      fts_free(kernels);
      post("pt: filter bank init: can not allocate coefficients!\n");
      return(0);
    }

  /* lowest octave starts at point #2 -> freq=2./n_points */
  freq = 2.0f/n_points;
  for(i=0; i<n_channels; i++){
    float bandwidth = q_factor * freq;
    float filter_length = 1.0f / bandwidth; /* fixed filter cross over at 0.5f amplitude of hanning fr. */
    float wind_length = (filter_length > (float)n_points)? ((float)n_points): (filter_length);
    float wind_norm = 1.0f / wind_length;    /* normalize window */
    float correct = wind_length / filter_length;
    float d_wind_phase = FTS_TWO_PI / wind_length;
    float d_sine_phase = freq * FTS_TWO_PI;
    int down, up, j;
    int n_filter_coeffs;
    
    fts_vecx_czero(fft_buf, n_points);

    fft_buf[n_points/2].re = wind_norm;
    fft_buf[n_points/2].im = 0;
    for(down=n_points/2-1, up=n_points/2+1, j=1; j<wind_length/2; j++, up++, down--){
      double wind = wind_norm * (0.5f + 0.5f * cos(j * d_wind_phase));
      fft_buf[down].re = fft_buf[up].re = cos(j * d_sine_phase) * wind;
      fft_buf[down].im = -(fft_buf[up].im = sin(j * d_sine_phase) * wind);
    }
    
    fts_fft_declaresize(n_points);
    fts_cfft_inplc(fft_buf, n_points);
    
    for(down = n_points/2-1; down >= 0; down--){
      float f = fft_buf[down].re;
      if(f > cutoff || f < -cutoff) break;
    }
    for(up = 0; up < n_points/2; up++){
      float f = fft_buf[up].re;
      if(f > cutoff || f < -cutoff) break;
    }
    if(up>=down){
      static int bugged = 0;
      if(!bugged){
        post("filter bank initialisation failed\n");
        bugged = 1;
      }
      up = down;
    }
      
    n_filter_coeffs = down - up + 1;
    
    kernels[i].filter_coeffs = all_coeffs + n_all_coeffs; /* here the coeffs are found then */
    kernels[i].n_filter_coeffs = n_filter_coeffs;
    kernels[i].onset = up * 2; /* * 2: onset for float instead of complex */
    kernels[i].pow_corr = correct;
    /* copy coeffs to the *all_coeffs vector */

    for(j=n_all_coeffs; j<n_all_coeffs+n_filter_coeffs; j++, up++) all_coeffs[j] = fft_buf[up].re;
    n_all_coeffs += n_filter_coeffs;

    freq += freq * q_factor;
  }
  
  filter_bank->kernels = kernels;
  filter_bank->n_channels = n_channels;
  filter_bank->all_coeffs = all_coeffs;
  filter_bank->n_all_coeffs = n_all_coeffs;
  filter_bank->channels_per_octave = channels_per_octave;
  filter_bank->q_factor = q_factor;
  filter_bank->coeff_cut = coeff_cut;
  fts_free(fft_buf);
  
  return(filter_bank);
}
