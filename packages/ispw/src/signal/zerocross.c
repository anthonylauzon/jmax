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
/* a zero crossing counter */
/* By : Francois Dechelle; ported to the new FTS by mdc */

#include "fts.h"


typedef struct S_zerocross
{
  fts_object_t obj;
  int count;			/* zero crossing counter*/
  float lastsample;		/* last stored samples */
} zerocross_t;

static fts_symbol_t zerocross_function = 0;

/* ---------- DSP method ---------- */

/* Args:
     float *in1		 input vector 
     zerocrossctl *x	 control 
     long n		  number of points 
*/

static void
ftl_zerocross(fts_word_t *argv)
{
  float *in = (float *)fts_word_get_ptr(argv + 0);
  zerocross_t *this = (zerocross_t *)fts_word_get_ptr(argv + 1);
  long n = (long)fts_word_get_long(argv + 2);
  float lastsample, current;
  unsigned int lastsamplesign, currentsign, count;
  int i;

  lastsample = this->lastsample;
  lastsamplesign = (lastsample <= 0.0f);
  count = this->count;

  for (i = 0; i < n; i++)
    {
      current = in[i];
      currentsign = (current <= 0.0f);
      count += lastsamplesign ^ currentsign;
      lastsamplesign = currentsign;
    }

  this->lastsample = current;
  this->count = count;
}

/* ---------- method put : put in or out the DSP chain ---------- */

static void
zerocross_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  zerocross_t *this = (zerocross_t *)o;
  fts_atom_t argv[3];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);

  fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
  fts_set_ptr   (argv + 1, this);
  fts_set_long  (argv + 2, fts_dsp_get_input_size(dsp, 0));

  dsp_add_funcall(zerocross_function, 3, argv);
}


/* ---------- method bang : reset the count ---------- */

static void
zerocross_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  zerocross_t *this = (zerocross_t *)o;

  fts_outlet_int(o, 0, this->count);
  this->count = 0;
}


/* ---------- method new : instance creation ---------- */

static void
zerocross_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  zerocross_t *this = (zerocross_t *)o;

  /* initialise control */

  this->count = 0;
  this->lastsample = 0.0f;

  /* just put object in the dsp list */

  dsp_list_insert(o); 
}


static void
zerocross_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dsp_list_remove(o);
}


/* ---------- method setup : class creation ---------- */

static fts_status_t
zerocross_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  /* initialize the class */

  fts_class_init(cl, sizeof(zerocross_t), 1, 1, 0); 

  /* define the system methods */

  a[0] = fts_s_symbol;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, zerocross_init, 1, a);
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, zerocross_delete, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, zerocross_put);

  /* Zerocross user methods */

  fts_method_define(cl, 0, fts_s_bang, zerocross_bang, 0, 0);

  /* Type the outlet */

  dsp_sig_inlet(cl, 0);

  a[0] = fts_s_int;
  fts_outlet_type_define(cl, 0,	fts_s_int, 1, a);

  zerocross_function = fts_new_symbol("zerocross");
  dsp_declare_function(zerocross_function, ftl_zerocross);

  return fts_Success;
}

void
zerocross_config(void)
{
  fts_class_install(fts_new_symbol("zerocross~"),zerocross_instantiate);
  fts_class_alias(fts_new_symbol("zerocross"), fts_new_symbol("zerocross~"));
}
