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
#include <math.h>
#include <string.h>
#include "wavetab.h"
#include "osc.h"

/***************************************************************************************
 *
 *  tab1~
 *
 */

static fts_hashtable_t *sigtab1_ht;

typedef struct {
  fts_object_t _o;
  wavetab_t *wavetab;
} sigtab1_t;

static void
sigtab1_reload(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigtab1_t *this = (sigtab1_t *)o;
  if (this->wavetab)
    wavetable_load(this->wavetab);
}

static void
sigtab1_init(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fts_atom_t data, k;
  sigtab1_t *this = (sigtab1_t *)o;
  fts_symbol_t name = fts_get_symbol_arg(ac, at, 0, 0);
  fts_symbol_t wrap_mode = fts_get_symbol_arg(ac, at, 1, 0);

  if(name)
    {    
      fts_set_symbol( &k, name);
      if (fts_hashtable_get( sigtab1_ht, &k, &data))
	{
	  this->wavetab = (wavetab_t *)fts_get_pointer(&data);
	  this->wavetab->refcnt++;
	}
      else
	{
	  this->wavetab = wavetable_new(name, wrap_mode);
	  
	  fts_set_pointer(&data, this->wavetab);
	  fts_hashtable_put(sigtab1_ht, &k, &data);
	}
    }
  else
    fts_object_set_error(o, "Name argument required");
}

static void
sigtab1_delete(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sigtab1_t *this = (sigtab1_t *)o;

  if (this->wavetab && !--this->wavetab->refcnt)
    {
      fts_atom_t k;

      fts_set_symbol( &k, this->wavetab->sym);
      fts_hashtable_remove(sigtab1_ht, &k);
      wavetable_delete(this->wavetab);
    }
}

static void
sigtab1_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(sigtab1_t), sigtab1_init, sigtab1_delete);

  fts_class_method_varargs(cl, fts_s_bang, sigtab1_reload);
  
  wavetable_init();
}

/***************************************************************************************
 *
 *  osc1~
 *
 */

typedef struct 
{
  fts_object_t obj;
  fts_symbol_t sym;
  ftl_data_t ftl_data;
} osc_t;

static void
osc_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  osc_t *this = (osc_t *)o;
  fts_atom_t argv[5];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  double f;
  fts_atom_t data, k;

  osc_ftl_data_init(this->ftl_data, fts_dsp_get_output_srate(dsp, 0));

  if(this->sym)
    {
      fts_set_symbol( &k, this->sym);

      if (fts_hashtable_get(sigtab1_ht, &k, &data))
	{
	  wavetab_t *wavetab = (wavetab_t *) fts_get_pointer(&data);
	  osc_ftl_data_set_table(this->ftl_data, wavetab->table);
	}
      else
	{
	  post("osc1~: %s: can't find tab1~\n", this->sym);
	  fts_dsp_add_function_zero(fts_dsp_get_output_name(dsp, 0), fts_dsp_get_output_size(dsp, 0));
	  return;
	}
    }
  else
    osc_ftl_data_set_table(this->ftl_data, cos_table);
    
  osc_ftl_dsp_put(dsp, this->ftl_data);
}


static void
osc_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  osc_t *this = (osc_t *)o;
  double phase;
  
  phase = fts_get_float_arg(ac, at, 0, 0.0f);
  osc_ftl_data_set_phase(this->ftl_data, phase);
}


static void
osc_set(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  osc_t *this = (osc_t *)o;
  fts_symbol_t s = fts_get_symbol_arg(ac, at, 0, 0);
  fts_atom_t data, k;

  this->sym = s;
  
  if (s)
    {
      fts_set_symbol( &k, s);

      if (fts_hashtable_get(sigtab1_ht, &k, &data))
	{
	  wavetab_t *wavetab = (wavetab_t *) fts_get_pointer(&data);
	  
	  osc_ftl_data_set_table(this->ftl_data, (void *)wavetab->table);
	}
      else
	post("osc1~: set %s: can't find table\n", s);
    }
  else
    osc_ftl_data_set_table(this->ftl_data, (void *)cos_table);
}

static void
osc_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  osc_t *this = (osc_t *)o;
  fts_symbol_t sym = fts_get_symbol_arg(ac, at, 0, 0);

  this->ftl_data = osc_ftl_data_new();
  this->sym = sym;

  fts_dsp_add_object(o); /* just put object in list */
}

static void
osc_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  osc_t *this = (osc_t *)o;

  ftl_data_free(this->ftl_data);

  fts_dsp_remove_object(o);
}

static void
osc_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(osc_t), osc_init, osc_delete);

  fts_class_method_varargs(cl, fts_s_put, osc_put);

  fts_class_method_varargs(cl, fts_s_set, osc_set);

  fts_class_inlet_int(cl, 0, osc_number);
  fts_class_inlet_float(cl, 0, osc_number);

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_inlet(cl, 1);
  fts_dsp_declare_outlet(cl, 0);
  }

/******************************************************************
 *
 *  phasor~
 *
 */

typedef struct 
{
  fts_object_t _o;
  ftl_data_t state;
} phasor_t;

fts_symbol_t phasor_function = 0;
fts_symbol_t phasor_inplace_function = 0;

void
phasor_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  phasor_t *this = (phasor_t *)o;
  double phi = (double)fts_get_number_float(at);

  phasor_ftl_set_phase(this->state, phi);
}

void
phasor_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  phasor_t *this = (phasor_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  phasor_ftl_t *state = ftl_data_get_ptr(this->state);
  float sr = fts_dsp_get_input_srate(dsp, 0);
  fts_atom_t argv[4];
  double incr;

  phasor_ftl_data_init(this->state, sr);

  if (fts_dsp_get_input_name(dsp, 0) == fts_dsp_get_output_name(dsp, 0))
    {
      /* Use the inplace version */

      fts_set_symbol(argv, fts_dsp_get_input_name(dsp, 0));
      fts_set_ftl_data(argv+1, this->state);
      fts_set_int(argv+2, fts_dsp_get_input_size(dsp, 0));

      fts_dsp_add_function(phasor_inplace_function, 3, argv);
    }
  else
    {
      /* standard code */
      fts_set_symbol( argv, fts_dsp_get_input_name(dsp, 0));
      fts_set_symbol( argv+1, fts_dsp_get_output_name(dsp, 0));
      fts_set_ftl_data( argv+2, this->state);
      fts_set_int( argv+3, fts_dsp_get_input_size(dsp, 0));

      fts_dsp_add_function(phasor_function, 4, argv);
    }
}

static void
phasor_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  phasor_t *this = (phasor_t *)o;

  this->state = phasor_ftl_data_new();

  fts_dsp_add_object(o);
}

static void
phasor_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  phasor_t *this = (phasor_t *)o;

  ftl_data_free(this->state);
  fts_dsp_remove_object(o);
}

static void
phasor_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(phasor_t), phasor_init, phasor_delete);

  fts_class_method_varargs(cl, fts_s_put, phasor_put);

  fts_class_inlet_int(cl, 0, phasor_set);
  fts_class_inlet_float(cl, 0, phasor_set);

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0);
}

void
osc_config(void)
{
  fts_class_install(fts_new_symbol("tab1~"),sigtab1_instantiate);

  sigtab1_ht = (fts_hashtable_t *)fts_malloc( sizeof( fts_hashtable_t));
  fts_hashtable_init(sigtab1_ht, 0, FTS_HASHTABLE_MEDIUM);

  if (!wavetable_make_cos())
    {
      post("osc~: out of memory\n");
      return;
    }

  osc_ftl_declare_functions();

  phasor_function = fts_new_symbol("phasor");
  phasor_inplace_function = fts_new_symbol("phasor_inplace");

  fts_dsp_declare_function(phasor_function, phasor_ftl);
  fts_dsp_declare_function(phasor_inplace_function, phasor_ftl_inplace);

  fts_class_install(fts_new_symbol("osc1~"),osc_instantiate);
  fts_class_install(fts_new_symbol("phasor~"), phasor_instantiate);
}
