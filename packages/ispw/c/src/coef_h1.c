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

/*
   %	Compute coefficients and display the amplitude 
   %	frequency response of a double shelving filter
   %	(product of low-shelving and high-shelving filters).
   %	Fi : transition frequencies in octaves
   %	     (stricly negative, 0 = sampling rate, F1 < F2)
   %	Ki : filter gains in dB (0=low, 1=mid, 2=high)
   %	pt : to plot the amplitude frequency response (see PLOT)
   %	     (character string specifying line type, color...)
   %
   %	This is a Regalia/Mitra-type filter, modified for obtaining
   %	the following poperties:
   %	- the TRANSITION FREQUENCY is for 50% of the total dB gain variation
   %	- low-shelving and high-shelving responses are symetric about the 
   %	  transition frequency and their product is an all-pass filter
   %	- boost and cut characteristic are symetric about the 0 dB axis
   %	  (replacing K by -K yelds the inverse filter, including phase)
   %	- the dB-responses obtained when K varies form a homothetic family
   %	  => associating two filters in series is approximately equivalent to
   %	  adding their shelving gains in dB (for gains below roughly 12 dB) 
   */

#include <math.h>
#include <fts/fts.h>



#define PI 3.141593f
#define MAXSRATE 44100.0f
#define MAX_GAIN 120.0f

static void
sigcoef_hlshelf1_in3(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);

static void
sigcoef_hlshelf1_in4(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);

typedef struct
{
  fts_dsp_object_t _o;
  float s_rate;
  float s_gain0;
  float s_gain1;
  float s_gain2;
  float s_ltransfq;
  float s_htransfq;
  float s_lradians;
  float s_hradians;
} sigcoef_hlshelf1_t;

static void
sigcoef_hlshelf1_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_t a;
  sigcoef_hlshelf1_t* this = (sigcoef_hlshelf1_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);


  this->s_rate = fts_dsp_get_input_srate(dsp, 0);

  fts_set_float(&a, this->s_ltransfq);
  sigcoef_hlshelf1_in3(o, 3, s, 1, &a);

  fts_set_float(&a, this->s_htransfq);
  sigcoef_hlshelf1_in4(o, 4, s, 1, &a);
}

static void
sigcoef_hlshelf1_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigcoef_hlshelf1_t* this = (sigcoef_hlshelf1_t *)o;
  fts_atom_t a;
  float k0, k1, k2, f1, f2;
/*  int i, z = 0, b; */


  k0 = fts_get_float_arg(ac, at, 0, 0.0f);   
  k1 = fts_get_float_arg(ac, at, 1, 0.0f);   
  k2 = fts_get_float_arg(ac, at, 2, 0.0f);   
  f1 = fts_get_float_arg(ac, at, 3, 0.0f);   
  f2 = fts_get_float_arg(ac, at, 4, 0.0f);   

  if (f1 == 0.0f && f2 == 0.0f)
    { /* all gains = 0db */
      f1 = 150.0f;	
      f2 = 5000.0f;
    }

  if (f1 < 0) 
    f1 = 0.0f;

  if (f2 > MAXSRATE) 
    f2 = .5f*MAXSRATE;

  if (f1 >= f2)
    {
      post("sigcoef_hlshelf1_new: low shelf Fq. >=  high shelf Fq. - can't continue\n");
      return;
    }
 
  this->s_rate = MAXSRATE;		/* srate default  */
  this->s_gain0 = k0;
  this->s_gain1 = k1;
  this->s_gain2 = k2;

  this->s_ltransfq = 0.0f;
  this->s_htransfq = MAXSRATE/2;

  fts_set_float(&a, f1);
  sigcoef_hlshelf1_in3(o, 3, s, 1, &a);

  fts_set_float(&a, f2);
  sigcoef_hlshelf1_in4(o, 4, s, 1, &a);

  fts_dsp_object_init((fts_dsp_object_t *)o);
}

static void
sigcoef_hlshelf1_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigcoef_hlshelf1_t* this = (sigcoef_hlshelf1_t *)o;
  float c0, c1, c2, d0, d1, d2;	/* output coefs */
  float a1, a2, b1, b2, g1, g2;	/* temp coefs */
  double x;

  /* low shelf */
  x = 0.5 * 0.115129255 * (double)(this->s_gain0 - this->s_gain1); /* ln(10) / 20 = 0.115129255 */
  if(x < -200.) /* exp(x) -> 0 */
    {
      a1 = 1.0f;
      b1 = -1.0f;
      g1 = 0.0f;
    }
  else
    {
      double t = tan(this->s_lradians);
      double e = exp(x);
      double r = t / e;
      double kr = t * e;

      a1 = (r - 1) / (r + 1);		
      b1 = (kr - 1) / (kr + 1);
      g1 = (kr + 1) / (r + 1);
    }

  /* high shelf */
  x = 0.5 * 0.115129255 * (double)(this->s_gain2 - this->s_gain1); /* ln(10) / 20 = 0.115129255 */
  if(x < -200.) /* exp(x) -> 0 */
    {
      a2 = -1.0f;
      b2 = 1.0f;
      g2 = 0.0f;
    }
  else
    {
      double t = tan(this->s_hradians);
      double e = exp(x);
      double r = t / e;
      double kr = t * e;

      a2 = (1 - r) / (1 + r);
      b2 = (1 - kr) / (1 + kr);
      g2 = (1 + kr) / (1 + r);
    }

  /* form product */
  c0 = g1 * g2 * (float)(exp((double)(this->s_gain1) * 0.05f * 2.302585093f));  ;
  c1 = a1 + a2;
  c2 = a1 * a2;
  d0 =  1.0f;
  d1 = b1 + b2;
  d2 = b1 * b2;
 
  fts_outlet_float(o, 5, d2);
  fts_outlet_float(o, 4, d1);  
  fts_outlet_float(o, 3, d0);   
  fts_outlet_float(o, 2, -c2); /* negate term for 2p20~ */
  fts_outlet_float(o, 1, -c1); /* negate term for 2p20~ */
  fts_outlet_float(o, 0, c0);   
}

static void
sigcoef_hlshelf1_in0(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigcoef_hlshelf1_t* this = (sigcoef_hlshelf1_t *)o;
  float f = fts_get_float_arg(ac, at, 0, 0.0f);

  if(f - this->s_gain1 > MAX_GAIN)
    f = this->s_gain1 + MAX_GAIN; 

  this->s_gain0 = f;
  sigcoef_hlshelf1_bang(o, 0, 0, 0, 0);
}

static void
sigcoef_hlshelf1_in1(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigcoef_hlshelf1_t* this = (sigcoef_hlshelf1_t *)o;
  float f = fts_get_float_arg(ac, at, 0, 0.0f);

  if(f > MAX_GAIN)
    f = MAX_GAIN;

  this->s_gain1 = fts_get_float_arg(ac, at, 0, 0.0f);
}

static void
sigcoef_hlshelf1_in2(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigcoef_hlshelf1_t* this = (sigcoef_hlshelf1_t *)o;
  float f = fts_get_float_arg(ac, at, 0, 0.0f);

  if(f - this->s_gain1 > MAX_GAIN)
    f = this->s_gain1 + MAX_GAIN; 

  this->s_gain2 = f;
}

static void
sigcoef_hlshelf1_in3(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigcoef_hlshelf1_t* this = (sigcoef_hlshelf1_t *)o;
  float n = fts_get_float_arg(ac, at, 0, 0.0f);

  /* constrain: 0 <= this->s_ltransfq < this->s_htransfq. */
  this->s_ltransfq = (n < this->s_htransfq) ? n : this->s_htransfq - 0.5f;
  if (this->s_ltransfq < 0) this->s_ltransfq = 0.0f;
  this->s_lradians = PI * this->s_ltransfq / this->s_rate;
}

static void
sigcoef_hlshelf1_in4(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigcoef_hlshelf1_t* this = (sigcoef_hlshelf1_t *)o;
  float n = fts_get_float_arg(ac, at, 0, 0.0f);

  /* constrain: this->s_htransfq. < this->s_ltransfq <=  0.5f * this->s_rate */
  this->s_htransfq = (n > this->s_ltransfq) ? n : this->s_ltransfq + 0.5f;
  if (this->s_htransfq > 0.5f * this->s_rate) this->s_htransfq = 0.5f * this->s_rate; /* clip FQ range */
  this->s_hradians= PI * (0.5f - (this->s_htransfq / this->s_rate));	
}

static void
sigcoef_hlshelf1_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_object_delete((fts_dsp_object_t *)o);
}

static void
sigcoef_hlshelf1_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(sigcoef_hlshelf1_t), sigcoef_hlshelf1_init, sigcoef_hlshelf1_delete);

  fts_class_message_varargs(cl, fts_s_put, sigcoef_hlshelf1_put);

  fts_class_inlet_bang(cl, 0, sigcoef_hlshelf1_bang);

  fts_class_inlet_float(cl, 0, sigcoef_hlshelf1_in0);
  fts_class_inlet_float(cl, 1, sigcoef_hlshelf1_in1);
  fts_class_inlet_float(cl, 2, sigcoef_hlshelf1_in2);
  fts_class_inlet_float(cl, 3, sigcoef_hlshelf1_in3);
  fts_class_inlet_float(cl, 4, sigcoef_hlshelf1_in4);

  fts_class_inlet_int(cl, 0, sigcoef_hlshelf1_in0);
  fts_class_inlet_int(cl, 1, sigcoef_hlshelf1_in1);
  fts_class_inlet_int(cl, 2, sigcoef_hlshelf1_in2);
  fts_class_inlet_int(cl, 3, sigcoef_hlshelf1_in3);
  fts_class_inlet_int(cl, 4, sigcoef_hlshelf1_in4);

  fts_dsp_declare_inlet(cl, 0);

  fts_class_outlet_float(cl, 0);
  fts_class_outlet_float(cl, 1);
  fts_class_outlet_float(cl, 2);
  fts_class_outlet_float(cl, 3);
  fts_class_outlet_float(cl, 4);
  fts_class_outlet_float(cl, 5);
}

void
sigcoef_hlshelf1_config(void)
{
  fts_class_t *cl = fts_class_install(fts_new_symbol("coef_hlshelf1~"), sigcoef_hlshelf1_instantiate);
  fts_class_alias(cl, fts_new_symbol("coef_h~"));
}
