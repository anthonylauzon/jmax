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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell, Zack Settel.
 *
 */

/* 
   %	sigcoef_bpass2   Jean Marc Jot and Zack Settel 1994   
   %                     Ported to the new fts by MDC.
   %
   %       calculate coefficients for '2p2z~' in boost/cut bandpass implementation 
   
   %	K  : filter gain in dB
   %	F  : center frequency in octaves (F < -1, 0 = sampling rate)
   %	W  : bandwidth (for K/2 dB) in octaves (preferably less than |F|-1)
   %	pt : to plot the amplitude frequency response (see PLOT)
   %	     (character string specifying line type, color...)
   %
   %	This is a Regalia/Mitra-type filter, modified for obtaining
   %	the following poperties:
   %	- the bandwidth is for 50% of the total dB gain variation
   %	- boost and cut characteristic are symetric about the 0 dB axis
   %	  (replacing K by -K yelds the inverse filter, including phase)
   %	- the dB-responses obtained when K varies form a homothetic family
   %	  => associating two filters in series is approximately equivalent
   %	  to adding their gains in dB (for gains below roughly 12 dB)
   */

#include <math.h>
#include "fts.h"


#define InRange(v,lo,hi) ((v)<=(hi)&&(v)>=(lo))

#define PI 3.141593f
#define PIOVER2 (PI/2.0f)
#define PIOVER2I (1/PIOVER2)
#define LOG2I 1.442695040f
#define TINY .0001f
#define MAXSRATE 44100.0f
#define MAXOCTAVE 10

#define VERSION "sigcoef_bpass2  v1.0  -j.m.jot/z.settel 1994"

typedef struct 
{
  fts_object_t obj;

  float srate;
  float gain0;
  float gain1;
  float gain2;
  float cfradians;
  float cfreq;
  float bwidth;
  float bwdb;
	
} sigcoef_bpass2_t;


/* utility functions. */

static void
sigcoef_bpass2_verify_freq(sigcoef_bpass2_t *this)
{
  double lfreq, cfreq;

  cfreq = this->cfreq;
    
  if (cfreq < TINY)
    cfreq = TINY;
  else if (cfreq > .5f * this->srate - TINY)
    cfreq = .5f * this->srate - TINY;
    
  lfreq = log(2*cfreq/this->srate) * LOG2I; /* get log FQ */

  if (.5f * this->bwdb > -lfreq)	/* bound center FQ such that CF + BW/2 < SR/2  */
    {
      lfreq = -.5f * this->bwdb + TINY;
      cfreq = pow(2, lfreq) * this->srate*.5f;
    }
  this->cfreq = cfreq;
  this->cfradians = PI*this->cfreq/this->srate;
}

static void
sigcoef_bpass2_verify_bw(sigcoef_bpass2_t *this)
{
  double lfreq;
  double bwdb;

  bwdb = this->bwdb;
    
  if (bwdb < TINY)
    bwdb = TINY;
  else if (bwdb > MAXOCTAVE)
    bwdb = MAXOCTAVE;

  lfreq = log((double)2*this->cfreq/this->srate) * LOG2I; /* get log FQ */

  if (.5f * bwdb > -lfreq)		/* bound center FQ such that CF + BW/2 < SR/2  */
    {
      bwdb = -2*lfreq - TINY;
    }
  this->bwdb = (float) bwdb;
  this->bwidth = (float)((pow(2.0f, bwdb/2.0f)- pow(2.0f, -bwdb/2.0f))); /* convert to linear */	
}



/* reevaluate and output coeficents  */

static void
sigcoef_bpass2_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigcoef_bpass2_t *this = (sigcoef_bpass2_t *)o;
  float c0,c1,c2,d0,d1,d2;	/* output coefs */
  float a1,a2,b1,b2,g0;		/* temp coefs */
  float k,f,w,c,r;	

  k = (float)pow(10.0f, this->gain0 / 20.0f);
        
  f = this->cfradians;	
  w = f*this->bwidth;

  if (w >= PIOVER2)		
    {
      /* just in case there is a bug */ /* Shadock ^ 2 */

      f = log((double)this->cfradians*PIOVER2I)*LOG2I;
      post("coef_bpass2~_float: bug found:  f= %f\n", f);

      w = PIOVER2 - TINY;
    }

  c = (float) cos(2*f);
  r = (float)(tan(w)/sqrt((double)k));
    
  a1 = 2 * c / (r + 1);		
  a2 = (r - 1) / (r + 1);
  b1 = 2 * c / (k*r + 1);
  b2 = (k*r - 1) / (k*r + 1);		
  g0 = (k*r + 1) / (r + 1);
    
  /* form product */

  c0 = g0;
  c1 = -a1;
  c2 = -a2;
  d0 =  1.0f;
  d1 = -b1;
  d2 = -b2;

  fts_outlet_float(o, 5,  d2);   
  fts_outlet_float(o, 4,  d1);   
  fts_outlet_float(o, 3,  d0);   
  fts_outlet_float(o, 2, -c2); /* negate term for 2p20~ */
  fts_outlet_float(o, 1, -c1); /* negate term for 2p20~ */
  fts_outlet_float(o, 0,  c0);   
}


/* K0:  filter low shelf output gain  (db) */

static void
sigcoef_bpass2_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigcoef_bpass2_t *this = (sigcoef_bpass2_t *)o;

  this->gain0 = fts_get_float(at);

  sigcoef_bpass2_bang(o, 0, 0, 0, 0);
}

/*  F: center fq. <hz> */

static void
sigcoef_bpass2_float_1(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigcoef_bpass2_t *this = (sigcoef_bpass2_t *)o;

  this->cfreq = fts_get_float(at);
  sigcoef_bpass2_verify_freq(this);
}

/*  W: bandwidth <octaves> */

static void
sigcoef_bpass2_float_2(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigcoef_bpass2_t *this = (sigcoef_bpass2_t *)o;

  this->bwdb = fts_get_float(at);

  sigcoef_bpass2_verify_bw(this);
}



/* System inlet  methods */ 

/* note:  f1 and f2 constrained: f1<f2  */

static void
sigcoef_bpass2_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigcoef_bpass2_t *this = (sigcoef_bpass2_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);

  this->srate = fts_dsp_get_input_srate(dsp, 0); /* get current srate at DSP 'on' time */

  sigcoef_bpass2_verify_freq(this);
}


/* takes 3 optional floating point numbers: gain<db>  centerFq<hz> bandwidth <octaves> */

static void
sigcoef_bpass2_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigcoef_bpass2_t *this = (sigcoef_bpass2_t *)o;
  float g = fts_get_float_arg(ac, at, 1, 0.0f);
  float f = fts_get_float_arg(ac, at, 2, 500);
  float w = fts_get_float_arg(ac, at, 3, 0.5f);

  if (!InRange(f,0, MAXSRATE/2) || !InRange(w,0, MAXOCTAVE))
    {
      post("coef_bpass2_new: Fq and/or BW argument - out of range, put in the range\n");
    }
 
  this->srate = MAXSRATE;	/* srate default  */
  this->cfreq = (float)f;	/* set-up state for constraint test in fq and bw methods */
  this->bwdb  = (float)w;		/* same */
  this->gain0 = (float) g;

  sigcoef_bpass2_verify_freq(this);
  sigcoef_bpass2_verify_bw(this);

  dsp_list_insert(o); /* just put object in list */
}


static void
sigcoef_bpass2_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dsp_list_remove(o);
}


static fts_status_t
sigcoef_bpass2_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[4];

  /* initialize the class */

  fts_class_init(cl, sizeof(sigcoef_bpass2_t), 3, 6, 0); 

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_float;
  a[2] = fts_s_float;
  a[3] = fts_s_float;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, sigcoef_bpass2_init, 4, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, sigcoef_bpass2_delete, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, sigcoef_bpass2_put);

  /* Sigcoef_Bpass2 args */

  fts_method_define(cl, 0, fts_s_bang, sigcoef_bpass2_bang, 0, 0);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, sigcoef_bpass2_float, 1, a);
  fts_method_define(cl, 1, fts_s_float, sigcoef_bpass2_float_1, 1, a);
  fts_method_define(cl, 2, fts_s_float, sigcoef_bpass2_float_2, 1, a);

  /* Type the outlet */

  a[0] = fts_s_float;
  fts_outlet_type_define(cl, 0,	fts_s_float, 1, a);
  fts_outlet_type_define(cl, 1,	fts_s_float, 1, a);
  fts_outlet_type_define(cl, 2,	fts_s_float, 1, a);
  fts_outlet_type_define(cl, 3,	fts_s_float, 1, a);
  fts_outlet_type_define(cl, 4,	fts_s_float, 1, a);
  fts_outlet_type_define(cl, 5,	fts_s_float, 1, a);

  /* define the first inlet as dsp */

  dsp_sig_inlet(cl, 0);

  return fts_Success;
}

void
sigcoef_bpass2_config(void)
{
  fts_class_install(fts_new_symbol("coef_bpass2~"),sigcoef_bpass2_instantiate);
}





