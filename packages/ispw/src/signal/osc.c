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

#include "fts.h"
#include <math.h>
#include <string.h>
#include "wavetab.h"
#include "osc_ftl.h"

/***************************************************************************************
 *
 *  tab1~
 *
 */

static fts_hash_table_t *sigtab1_ht;

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
  fts_atom_t data;
  sigtab1_t *this = (sigtab1_t *)o;
  fts_symbol_t name = fts_get_symbol_arg(ac, at, 1, 0);
  fts_symbol_t wrap_mode = fts_get_symbol_arg(ac, at, 2, 0);

  if(!name)
    return;

  if (fts_hash_table_lookup(sigtab1_ht, name, &data))
    {
      this->wavetab = (wavetab_t *)fts_get_ptr(&data);
      this->wavetab->refcnt++;
    }
  else
    {
      this->wavetab = wavetable_new(name, wrap_mode);
      fts_set_ptr(&data, this->wavetab);
      fts_hash_table_insert(sigtab1_ht, name, &data);
    }
}

static void
sigtab1_delete(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sigtab1_t *this = (sigtab1_t *)o;

  if (this->wavetab && !--this->wavetab->refcnt)
    {
      fts_hash_table_remove(sigtab1_ht, this->wavetab->sym);
      wavetable_delete(this->wavetab);
    }
}

static fts_status_t
sigtab1_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  fts_class_init(cl, sizeof(sigtab1_t), 1, 0, 0);

  wavetable_init();

  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  a[2] = fts_s_symbol;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, sigtab1_init, 3, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, sigtab1_delete, 0, a);

  fts_method_define(cl, 0, fts_s_bang, sigtab1_reload, 0, a);
  
  return fts_Success;
}

void
sigtab1_config(void)
{
  fts_class_install(fts_new_symbol("tab1~"),sigtab1_instantiate);

  sigtab1_ht = fts_hash_table_new();
  fts_hash_table_init(sigtab1_ht);
}


/***************************************************************************************
 *
 *  osc1~ obj
 *
 */

typedef struct 
{
  fts_object_t obj;
  fts_symbol_t sym;
  ftl_data_t ftl_data;
} osc_t;

static void
osc_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  osc_t *this = (osc_t *)o;
  fts_symbol_t sym = fts_get_symbol_arg(ac, at, 1, 0);

  this->ftl_data = osc_ftl_data_new();
  this->sym = sym;

  dsp_list_insert(o); /* just put object in list */
}

static void
osc_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  osc_t *this = (osc_t *)o;

  ftl_data_free(this->ftl_data);

  dsp_list_remove(o);
}


/***************************************************************************************
 *
 *  osc1~ dsp
 *
 */

static void
osc_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  osc_t *this = (osc_t *)o;
  fts_atom_t argv[5];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);
  double f;
  fts_atom_t data;

  osc_ftl_data_init(this->ftl_data, fts_dsp_get_output_srate(dsp, 0));

  if(this->sym)
    {
      if (fts_hash_table_lookup(sigtab1_ht, this->sym, &data))
	{
	  wavetab_t *wavetab = (wavetab_t *) fts_get_ptr(&data);
	  osc_ftl_data_set_table(this->ftl_data, wavetab->table);
	}
      else
	{
	  post("osc1~: %s: can not find tab1~\n", fts_symbol_name(this->sym));
	  fts_set_symbol(argv + 0, fts_s_sig_zero);
	  fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 0));
	  fts_set_long  (argv + 2, fts_dsp_get_output_size(dsp, 0));
	  dsp_add_funcall(ftl_sym.cpy.f, 3, argv);
	  return;
	}
    }
  else
    osc_ftl_data_set_table(this->ftl_data, cos_table);
    
  osc_ftl_dsp_put(dsp, this->ftl_data);
}


/***************************************************************************************
 *
 *  osc1~ user methods
 *
 */

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
  fts_atom_t data;

  this->sym = s;
  
  if (s)
    {
      if (fts_hash_table_lookup(sigtab1_ht, s, &data))
	{
	  wavetab_t *wavetab = (wavetab_t *) fts_get_ptr(&data);
	  
	  osc_ftl_data_set_table(this->ftl_data, (void *)wavetab->table);
	}
      else
	post("osc1~: set %s: can not find table\n", fts_symbol_name(s));
    }
  else
    osc_ftl_data_set_table(this->ftl_data, (void *)cos_table);
}

/***************************************************************************************
 *
 *  osc1~ class
 *
 */

static fts_status_t
osc_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  fts_class_init(cl, sizeof(osc_t), 2, 1, 0);

  /* System methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, osc_init, 2, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, osc_delete, 0, 0);

  a[0] = fts_s_ptr;
  fts_method_define(cl, fts_SystemInlet, fts_s_put, osc_put, 1, a);

  /* User method */

  a[0] = fts_s_symbol;
  fts_method_define_optargs(cl, 0, fts_new_symbol("set"), osc_set, 1, a, 0);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, osc_number, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, osc_number, 1, a);

  /* DSP declarations */

  dsp_sig_inlet(cl, 0);
  dsp_sig_inlet(cl, 1);
  dsp_sig_outlet(cl, 0);
  
  return fts_Success;
}

void
osc_config(void)
{
  if (!wavetable_make_cos())
    {
      post("osc~: out of memory\n");
      return;
    }

  /* Declare the oscillator related FTL functions (platform dependent) */
  osc_ftl_declare_functions();

  /* class declaration */
  fts_class_install(fts_new_symbol("osc1~"),osc_instantiate);
}
