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

#include "pt_obj.h"
#include "pt_tools.h"
#include "pt_meth.h"

#include <math.h>

#define HALF_TONES(x) (17.312340491f * log(x))
#define FREQ(ht) exp(0.057762265f * (ht))
#define LD(x) (1.442695041f * log(x))

#define CLASS_NAME "pitch track"

#define pt_common_LOWINDEX 4

/* defaults */
#define DEF_n_points 256
static float def_amp_coeffs[pt_common_N_AMP_COEFFS] = {1.0f, 0.9f, 0.8f, 0.7f, 0.7f, 0.6f, 0.6f, 0.6f};

/*************************************************
 *
 *      usefull
 */
 
static void fix_pitch_range(pt_common_obj_t *x)
{
   long index_low, index_high;

   index_low = (int)(2.0f * (x->ctl.pitch_low - x->stat.pitch_base) + 0.5f);
   if(index_low < pt_common_LOWINDEX) index_low = pt_common_LOWINDEX;
   else if(index_low >= x->n_channels) index_low = x->n_channels - 3;
   x->stat.index_low = index_low;
   
   index_high = (int)(2.0f * (x->ctl.pitch_high - x->stat.pitch_base) + 0.5f);
   if(index_high < index_low + 2) index_high = index_low + 2;
   else if(index_high >= x->n_channels) index_high = x->n_channels - 1;
   x->stat.index_high = index_high;
}

static void fix_pitch_base(pt_common_obj_t *x)
{
   x->stat.freq_base = 2.0f * x->srate / x->n_points;
   x->stat.pitch_base = 69.0f + HALF_TONES(x->stat.freq_base / x->ctl.freq_ref);
   fix_pitch_range(x);
}
   
/*************************************************
 *
 *   common pt methods
 */
 
static void pt_common_meth_ref_pitch(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
   pt_common_obj_t *x = (pt_common_obj_t *)o;
   float p = fts_get_float_arg(ac, at, 0, 69.0f);
   
   if(p >= 57.0f && p <= 81) x->ctl.freq_ref = 440.0f * FREQ(p - 69.0f);
   else fts_post("%s: reference-pitch out of range of +/- one octave around note 69\n", CLASS_NAME);
   fix_pitch_base(x);
}

static void pt_common_meth_ref_freq(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
   pt_common_obj_t *x = (pt_common_obj_t *)o;
   float f = fts_get_float_arg(ac, at, 0, 440.0f);
   
   if(f >= 220.0f && f <= 880.0f) x->ctl.freq_ref = f;
   else fts_post("%s: reference-freq out of range of +/- one octave around 440Hz\n", CLASS_NAME);
   fix_pitch_base(x);
}

static void pt_common_meth_rough_pitch(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
   pt_common_obj_t *x = (pt_common_obj_t *)o;
   long n = fts_get_int_arg(ac, at, 0, 0);
   
   x->ctl.pitch_rough = n? 1: 0;
}

static void pt_common_meth_pitch_range(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
   pt_common_obj_t *x = (pt_common_obj_t *)o;
   long pitch_low = fts_get_int_arg(ac, at, 0, 0);
   long pitch_high = fts_get_int_arg(ac, at, 1, 127);

   if(pitch_low < 0) pitch_low = 0;
   else if(pitch_low > 127) pitch_low = 127;
   if(pitch_high < pitch_low) pitch_high = pitch_low;
   else if(pitch_high > 127) pitch_high = 127;
   
   x->ctl.pitch_low = pitch_low;
   x->ctl.pitch_high = pitch_high;
   
   fix_pitch_range(x);
}

static void pt_common_meth_stick(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
   pt_common_obj_t *x = (pt_common_obj_t *)o;
   float f = fts_get_float_arg(ac, at, 0, 0.3f);
   
   if(f < 0.0f) f = 0.0f;
   else if(f > 1.0f) f = 1.0f;
   x->ctl.pitch_stick = f;
}

static void pt_common_meth_amp_range(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
   pt_common_obj_t *x = (pt_common_obj_t *)o;
   float f1 = fts_get_float_arg(ac, at, 0, 0.001f);
   float f2 = fts_get_float_arg(ac, at, 1, 0.003f);
   float scale = x->n_points * x->n_points;
   
   x->ctl.power_off = f1 * f1 * scale;
   x->ctl.power_on = f2 * f2 * scale;
}

static void pt_common_meth_quality_range(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
   pt_common_obj_t *x = (pt_common_obj_t *)o;
   float f1 = fts_get_float_arg(ac, at, 0, 0.25f);
   float f2 = fts_get_float_arg(ac, at, 1, 0.5f);
   
   x->ctl.quality_off = f1 * f1;
   x->ctl.quality_on = f2 * f2;
}

static void pt_common_meth_debouncetime(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
   pt_common_obj_t *x = (pt_common_obj_t *)o;
   long n = fts_get_int_arg(ac, at, 0, 150);

   x->ctl.debounce_time = (n > 0)? n: 0;
}

static void pt_common_meth_stretch(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
   pt_common_obj_t *x = (pt_common_obj_t *)o;
   float f = fts_get_float_arg(ac, at, 0, 1.0f);
   
   if (f < 0.01f) f = 1.0f;
   x->ctl.pitch_stretch = f;
}

static void pt_common_meth_coefs(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
   pt_common_obj_t *x = (pt_common_obj_t *)o;
   int i;

   if(!ac){
      for(i=0; i<pt_common_N_AMP_COEFFS; i++)
         x->ctl.coeffs[i] = def_amp_coeffs[i];
   }else if(ac == pt_common_N_AMP_COEFFS){
      for(i=0; i<ac; i++){
         float coef = (fts_is_number(at+i))? fts_get_number_float(at+i): 0.0f;
         if(coef < 0) coef = 0;
         else if(coef > 1) coef = 1;
         x->ctl.coeffs[i] = coef;
      }
   }else{
      fts_post("%s: need %d coefficients\n", CLASS_NAME, pt_common_N_AMP_COEFFS);
   }
}

static void pt_common_meth_obsolete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
   fts_post("%s:\n", CLASS_NAME);
   fts_post("   The messages:\n");
   fts_post("   'pt_common_tune', 'ampl_threshold' and 'quality_threshold'\n");
   fts_post("   are obsolete since a long long time...\n");
   fts_post("   Use 'reference-pitch', 'amp-range' and 'quality-range' instead!\n");
}

static void pt_common_meth_print_kernel(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
   long channel = fts_get_int_arg(ac, at, 0, -1);
   pt_common_print_millers_kernels(channel);
}

/*************************************************
 *
 *      pt init & instatiate & set_sampling_rate
 */

int pt_common_init(pt_common_obj_t *x, long n_points, long n_period)
{
   fts_object_t* o = (fts_object_t *)x;
   int n_channels, n_overlap;
      
   if(n_points <= 0) n_points = DEF_n_points;
   if(
      n_points > pt_common_MAX_n_points ||
      !fts_fft_declaresize(n_points)
   ){
      fts_post("%s: frame size must be a power of 2 between %d and %d\n", CLASS_NAME, FTS_FFT_MIN_SIZE, pt_common_MAX_n_points);
      return(0);
   }      
   
   if(n_period <= 0) n_period = n_points;
   if(n_period > n_points){
      fts_post("%s: period can't exceed period\n", CLASS_NAME);
      return(0);
   }
   
   n_channels = (pt_common_CHANNELS_PER_OCTAVE * (LD(n_points) - 2));
   n_overlap = n_points - n_period;
   
   x->n_points = n_points;
   x->n_overlap = n_overlap;
   x->n_channels = n_channels;
   x->srate = 1.0f;
   
   x->buf.main = (float *)fts_zalloc(n_points * sizeof(float));
   if( !x->buf.main)
     {
       fts_post("%s: can't allocate buffers\n", CLASS_NAME);
       return(0);
     }

   x->buf.for_fft = (complex *)fts_zalloc(n_points * sizeof(complex));
   if( !x->buf.for_fft)
     {
       fts_post("%s: can't allocate buffers\n", CLASS_NAME);
       return(0);
     }
   
   pt_common_init_millers_kernels();

   x->buf.end = x->buf.main + n_points;
   
   pt_common_meth_ref_freq(o, 0, 0, 0, 0);
   pt_common_meth_rough_pitch(o, 0, 0, 0, 0);
   pt_common_meth_pitch_range(o, 0, 0, 0, 0);
   pt_common_meth_stick(o, 0, 0, 0, 0);
   pt_common_meth_stretch(o, 0, 0, 0, 0);
   pt_common_meth_coefs(o, 0, 0, 0, 0);
   pt_common_meth_amp_range(o, 0, 0, 0, 0);
   pt_common_meth_quality_range(o, 0, 0, 0, 0);
   pt_common_meth_debouncetime(o, 0, 0, 0, 0);
   
   return(1);
}

void pt_common_delete(pt_common_obj_t *x)
{
   fts_free(x->buf.main);
   fts_free(x->buf.for_fft);
}

void pt_common_instantiate(fts_class_t *cl)
{
   fts_class_message_varargs(cl, fts_new_symbol("reference-pitch"), pt_common_meth_ref_pitch);
   fts_class_message_varargs(cl, fts_new_symbol("reference-freq"), pt_common_meth_ref_freq);
   fts_class_message_varargs(cl, fts_new_symbol("rough-pitch"), pt_common_meth_rough_pitch);
   fts_class_message_varargs(cl, fts_new_symbol("pitch-range"), pt_common_meth_pitch_range);
   fts_class_message_varargs(cl, fts_new_symbol("stick"), pt_common_meth_stick);
   fts_class_message_varargs(cl, fts_new_symbol("stretch"), pt_common_meth_stretch);
   fts_class_message_varargs(cl, fts_new_symbol("coefs"), pt_common_meth_coefs);
   fts_class_message_varargs(cl, fts_new_symbol("amp-range"), pt_common_meth_amp_range);
   fts_class_message_varargs(cl, fts_new_symbol("quality-range"), pt_common_meth_quality_range);
   fts_class_message_varargs(cl, fts_new_symbol("debounce-time"), pt_common_meth_debouncetime);
   fts_class_message_varargs(cl, fts_new_symbol("pt_common_tune"), pt_common_meth_obsolete);
   fts_class_message_varargs(cl, fts_new_symbol("ampl_threshold"), pt_common_meth_obsolete);
   fts_class_message_varargs(cl, fts_new_symbol("quality_threshold"), pt_common_meth_obsolete);

   fts_class_message_varargs(cl, fts_new_symbol("print-kernel"), pt_common_meth_print_kernel);

   pt_common_init_millers_dumsqrt();
}

/*************************************************
 *
 *      pt's dsp
 */
 
void pt_common_dsp_fun_put(pt_common_obj_t *x, fts_dsp_descr_t *dsp) /* to set proper sampling rate */
{
  int i;

  if(x->n_overlap & (fts_dsp_get_input_size(dsp, 0)-1))
    fts_post("%s: period must be multiple of vector size: %d\n", CLASS_NAME, fts_dsp_get_input_size(dsp, 0));
		
  x->srate = fts_dsp_get_input_srate(dsp, 0);
  fix_pitch_base(x);
      
  for(i=0; i<x->n_points; i++)
    x->buf.main[i] = 0.0;
  
  x->buf.fill = x->buf.main + x->n_overlap;
}

void pt_common_dsp_function(fts_word_t *a)
{
  pt_common_obj_t *x = (pt_common_obj_t *)fts_word_get_pointer(a);
  analysis_t analysis = (analysis_t)fts_word_get_pointer(a+1);
  float *in = (float *)fts_word_get_pointer(a+2);
  long n = fts_word_get_int(a+3);
  int i;

  for(i=0; i<n; i++)
    x->buf.fill[i] = in[i];
  
  x->buf.fill += n;   

  if(x->buf.fill >= x->buf.end)
    {
      analysis((fts_object_t *)x);

      for(i=0; i<x->n_overlap; i++)
	x->buf.main[i] = x->buf.main[x->n_points - x->n_overlap + i];
      x->buf.fill = x->buf.main + x->n_overlap;
    }
}

/*************************************************
 *
 *   object & control print out
 */
 
void pt_common_print_obj(pt_common_obj_t *x)
{
   fts_post("  fft points: %d\n", x->n_points);
   fts_post("  frame overlap: %d\n", x->n_overlap);
   fts_post("  filter bands: %d\n", x->n_channels);
   fts_post("  sample rate: %f\n", x->srate);
}

void pt_common_print_ctl(pt_common_obj_t *x)
{
   fts_post("  reference pitch: %f\n", (69.0f + HALF_TONES(x->ctl.freq_ref / 440)));
   fts_post("  reference freq: %f\n", x->ctl.freq_ref);
   fts_post("  rough pitch: %s\n", (x->ctl.pitch_rough)? ("rough"): ("fine"));
   fts_post("  pitch range: %d...%d\n", x->ctl.pitch_low, x->ctl.pitch_high);
   fts_post("    reported min pitch: %f\n", pt_common_candidate_midi_pitch(x, x->stat.index_low));
   fts_post("    reported max pitch: %f\n", pt_common_candidate_midi_pitch(x, x->stat.index_high));
   fts_post("    reported min freq: %f\n", pt_common_candidate_frequency(x, x->stat.index_low));
   fts_post("    reported max freq: %f\n", pt_common_candidate_frequency(x, x->stat.index_high));
   fts_post("  stick: %f\n", x->ctl.pitch_stick);
   fts_post("  stretch: %f\n", x->ctl.pitch_stretch);
   fts_post("  amplitude range:\n");
   fts_post("    min: %f\n", sqrt(x->ctl.power_off)/x->n_points);
   fts_post("    max: %f\n", sqrt(x->ctl.power_on)/x->n_points);
   fts_post("  quality range: \n");
   fts_post("    min: %f\n", sqrt(x->ctl.quality_off));
   fts_post("    max: %f\n", sqrt(x->ctl.quality_on));
   fts_post("  coefs:\n");
   {
     int i; 
     for(i=0; i<pt_common_N_AMP_COEFFS; i++) 
       fts_post("    #%d: %f\n", i, x->ctl.coeffs[i]);
   }
   fts_post("  debounce time: %d\n", x->ctl.debounce_time);
}

/*************************************************
 *
 *   ask for debounce time
 */
 
int pt_common_debounce_time_is_up(pt_common_obj_t *x, float *time)
{
  float now = fts_get_time();
  int times_up = now > *time;
  
  *time = now + x->ctl.debounce_time;

  return(times_up);
}

