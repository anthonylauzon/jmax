/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#include "fts.h"
#include <math.h>
#include <string.h>

#define OSC_TAB_BITS 9
#define OSC_TAB_SIZE (1 << OSC_TAB_BITS)

typedef struct
{
  float value;
  float slope;
} wavetab_samp_t;

/***************************************************************************************
 *
 *  wave table data
 *
 */

static fts_symbol_t sym_nowrap;
static fts_hash_table_t *sigtab1_ht;

typedef struct {
  wavetab_samp_t *table;
  fts_symbol_t sym;
  int refcnt;
  int nowrap;
} wavetab_t;

static void
wavetable_load(wavetab_t *wavetab)
{
  const char *file_name = fts_symbol_name(wavetab->sym);
  float buf[OSC_TAB_SIZE];

    if(file_name)
    {
      fts_soundfile_t *sf = fts_soundfile_open_read_float(wavetab->sym, 0, 0.0f, 0);
      int n_samples;
      int i;

      if(!sf)
	{
	  post("tab1~: %s: can not open wave table file\n", file_name);
	  return;
	}
      
      n_samples = fts_soundfile_read_float(sf, buf, OSC_TAB_SIZE);
      fts_soundfile_close(sf);

      if(n_samples < OSC_TAB_SIZE)
	{
	  post("tab1~: %s: can not read wave table\n", file_name);
	  return;
	}
      
      /* get value of first wavetable point */
      wavetab->table[0].value = buf[0];

      /* next values and slopes */
      for(i=1; i<OSC_TAB_SIZE; i++)
	{
	  wavetab->table[i].value = buf[i];
	  wavetab->table[i-1].slope = (wavetab->table[i].value - wavetab->table[i-1].value);  
	}

      /* get slope of last wavetable point in dependency of wrapping mode */
      if(strstr(file_name, "nowrap") || wavetab->nowrap)
	wavetab->table[OSC_TAB_SIZE-1].slope = wavetab->table[OSC_TAB_SIZE-2].slope;
      else
	wavetab->table[OSC_TAB_SIZE-1].slope = (wavetab->table[0].value - wavetab->table[OSC_TAB_SIZE-1].value);
    }    
}

static wavetab_t *
wavetable_new(fts_symbol_t name, fts_symbol_t wrap_mode)
{
  wavetab_samp_t *table = (wavetab_samp_t *) fts_malloc(OSC_TAB_SIZE * sizeof(wavetab_samp_t));
  wavetab_t *wavetab;

  if(!table)
    return 0;
  
  wavetab = (wavetab_t *)fts_malloc(sizeof(wavetab_t));
  wavetab->sym = name;
  wavetab->refcnt = 1;
  wavetab->table = table;

  if(wrap_mode == sym_nowrap)
    wavetab->nowrap = 1;
  else
    wavetab->nowrap = 0;

  wavetable_load(wavetab);
  return wavetab;
}

static void
wavetable_delete(wavetab_t *wavetab)
{
  fts_free((char *)wavetab->table);
  fts_free(wavetab);
}

/***************************************************************************************
 *
 *  tab1~
 *
 */

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

  sym_nowrap = fts_new_symbol("nowrap");

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
  fts_metaclass_create(fts_new_symbol("tab1~"),sigtab1_instantiate, fts_always_equiv);

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
  wavetab_samp_t *table;
  double phase;
  double incr;
} osc_control_t;

typedef struct 
{
  fts_object_t obj;
  fts_symbol_t sym;
  ftl_data_t osc_ftl_data;
} osc_t;

static void
osc_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  osc_t *this = (osc_t *)o;
  fts_symbol_t sym = fts_get_symbol_arg(ac, at, 1, 0);

  this->osc_ftl_data = ftl_data_new(osc_control_t);
  this->sym = sym;

  dsp_list_insert(o); /* just put object in list */
}

static void
osc_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  osc_t *this = (osc_t *)o;

  ftl_data_free(this->osc_ftl_data);

  dsp_list_remove(o);
}


/***************************************************************************************
 *
 *  osc1~ dsp
 *
 */

static wavetab_samp_t *sin_tab;
static fts_symbol_t osc_function = 0;
static fts_symbol_t osc_freq_function = 0;
static fts_symbol_t osc_freq_inplace_function = 0;
static fts_symbol_t osc_phase_function = 0;
static fts_symbol_t osc_phase_inplace_function = 0;
static fts_symbol_t osc_phase_64_function = 0;
static fts_symbol_t osc_phase_64_inplace_function = 0;



static void
osc_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  osc_t *this = (osc_t *)o;
  fts_atom_t a;
  fts_atom_t argv[5];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);
  double f;
  fts_atom_t data;

  f = (double)OSC_TAB_SIZE / (double)fts_dsp_get_output_srate(dsp, 0);
  ftl_data_set(osc_control_t, this->osc_ftl_data, incr, &f);
  f = 0.0;
  ftl_data_set(osc_control_t, this->osc_ftl_data, phase, &f);

  if (this->sym)
    {
      if (fts_hash_table_lookup(sigtab1_ht, this->sym, &data))
	{
	  wavetab_t *wavetab = (wavetab_t *) fts_get_ptr(&data);

          ftl_data_set(osc_control_t, this->osc_ftl_data, table, &(wavetab->table));
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
    ftl_data_set(osc_control_t, this->osc_ftl_data, table, &sin_tab);

  /* The C version of the oscillator include now three version,
      one with frequency modulation only, one with phase modulation
      only, and one with both */

  if (fts_dsp_is_input_null(dsp, 0))
    {
      /* no frequency input */

      if (fts_dsp_is_input_null(dsp, 1))
	{
	  /* no phase input, no frequency input */

	  /* Special case: always output zero */

	  fts_set_symbol (argv + 0,   fts_dsp_get_input_name(dsp, 0)); /* input is zero, anyway */
	  fts_set_symbol (argv + 1, fts_dsp_get_output_name(dsp, 0));
	  fts_set_long   (argv + 2, fts_dsp_get_input_size(dsp, 0));

	  dsp_add_funcall(ftl_sym.cpy.f, 3, argv);
	}
      else
	{

	  /* Different code generated for inplace and not in place operations */

	  if (fts_dsp_get_input_name(dsp, 1) == fts_dsp_get_output_name(dsp, 0))
	    {
	      /* Inplace phase input, no frequency input */

	      if (fts_dsp_get_input_size(dsp, 0) == 64)
		{
		  fts_set_symbol (argv + 0, fts_dsp_get_input_name(dsp, 1));
		  fts_set_ftl_data(argv+ 1, this->osc_ftl_data);

		  dsp_add_funcall(osc_phase_64_inplace_function, 2, argv);
		}
	      else
		{
		  fts_set_symbol (argv + 0, fts_dsp_get_input_name(dsp, 1));
		  fts_set_ftl_data(argv+ 1, this->osc_ftl_data);
		  fts_set_long   (argv + 2, fts_dsp_get_input_size(dsp, 0));
		  dsp_add_funcall(osc_phase_inplace_function, 3, argv);
		}
	    }
	  else
	    {
	      /* Non inplace phase input, no frequency input */

	      if (fts_dsp_get_input_size(dsp, 0) == 64)
		{
		  fts_set_symbol (argv + 0, fts_dsp_get_input_name(dsp, 1));
		  fts_set_symbol (argv + 1, fts_dsp_get_output_name(dsp, 0));
		  fts_set_ftl_data(argv+ 2, this->osc_ftl_data);

		  dsp_add_funcall(osc_phase_64_function, 3, argv);
		}
	      else
		{
		  fts_set_symbol (argv + 0, fts_dsp_get_input_name(dsp, 1));
		  fts_set_symbol (argv + 1, fts_dsp_get_output_name(dsp, 0));
		  fts_set_ftl_data(argv+ 2, this->osc_ftl_data);
		  fts_set_long   (argv + 3, fts_dsp_get_input_size(dsp, 0));
		  dsp_add_funcall(osc_phase_function, 4, argv);
		}

	    }
	}
    }
  else
    {
      if (fts_dsp_is_input_null(dsp, 1))
	{
	  /* no phase input, frequency input */

	  if (fts_dsp_get_input_name(dsp, 0) == fts_dsp_get_output_name(dsp, 0))
	    {
	      /* In place */

	      fts_set_symbol (argv + 0, fts_dsp_get_input_name(dsp, 0));
	      fts_set_ftl_data(argv+ 1, this->osc_ftl_data);
	      fts_set_long   (argv + 2, fts_dsp_get_input_size(dsp, 0));

	      dsp_add_funcall(osc_freq_inplace_function, 3, argv);
	    }
	  else
	    {
	      /* Not In place */

	      fts_set_symbol (argv + 0, fts_dsp_get_input_name(dsp, 0));
	      fts_set_symbol (argv + 1, fts_dsp_get_output_name(dsp, 0));
	      fts_set_ftl_data(argv+ 2, this->osc_ftl_data);
	      fts_set_long   (argv + 3, fts_dsp_get_input_size(dsp, 0));

	      dsp_add_funcall(osc_freq_function, 4, argv);
	    }


	}
      else
	{
	  /* phase input, frequency input */
	      
	  fts_set_symbol (argv + 0, fts_dsp_get_input_name(dsp, 0));
	  fts_set_symbol (argv + 1, fts_dsp_get_input_name(dsp, 1));
	  fts_set_symbol (argv + 2, fts_dsp_get_output_name(dsp, 0));
	  fts_set_ftl_data(argv+ 3, this->osc_ftl_data);
	  fts_set_long   (argv + 4, fts_dsp_get_input_size(dsp, 0));

	  dsp_add_funcall(osc_function, 5, argv);
	}
    }
}


static void
ftl_osc_phase(fts_word_t *argv)
{
  float * restrict phase = (float *) fts_word_get_ptr(argv + 0);
  float * restrict out   = (float *) fts_word_get_ptr(argv + 1);
  osc_control_t * restrict this = (osc_control_t *)fts_word_get_ptr(argv + 2);
  long int n = fts_word_get_long(argv + 3);
  wavetab_samp_t * restrict tab;
  int i;

  /* This function is used when osc is used as a wave shaper; result,
     frequency and accumulated phase are always 0 */

  tab = this->table;

  for(i=0; i<n; i++)
    {
      fts_wrapper_t frac;
      int idx;
      float f;

      fts_wrapper_frac_set(&frac, phase[i] * (double)OSC_TAB_SIZE);
      idx = fts_wrapper_get_int(&frac, OSC_TAB_BITS);
      f = fts_wrapper_frac_get_wrap(&frac);
      
      out[i] = tab[idx].value + f * tab[idx].slope;
    }
}


static void
ftl_osc_phase_inplace(fts_word_t *argv)
{
  float * restrict sig = (float *) fts_word_get_ptr(argv + 0);
  osc_control_t * restrict this = (osc_control_t *)fts_word_get_ptr(argv + 1);
  long int n = fts_word_get_long(argv + 2);
  wavetab_samp_t * restrict tab;
  int i;

  /* This function is used when osc is used as a wave shaper; result,
     frequency and accumulated phase are always 0 */

  tab = this->table;

  for(i=0; i<n; i++)
    {
      fts_wrapper_t frac;
      int idx;
      float f;

      fts_wrapper_frac_set(&frac, sig[i] * (double)OSC_TAB_SIZE);
      idx = fts_wrapper_get_int(&frac, OSC_TAB_BITS);
      f = fts_wrapper_frac_get_wrap(&frac);
      
      sig[i] = tab[idx].value + f * tab[idx].slope;
    }
}


/* version with vs == 64 */

static void
ftl_osc_phase_64(fts_word_t *argv)
{
  float * restrict phase = (float *) fts_word_get_ptr(argv + 0);
  float * restrict out   = (float *) fts_word_get_ptr(argv + 1);
  osc_control_t * restrict this = (osc_control_t *)fts_word_get_ptr(argv + 2);
  wavetab_samp_t * restrict tab;
  int i;

  /* This function is used when osc is used as a wave shaper; result,
     frequency and accumulated phase are always 0 */

  tab = this->table;

  for(i=0; i<64; i++)
    {
      fts_wrapper_t frac;
      int idx;
      float f;

      fts_wrapper_frac_set(&frac, phase[i] * (double)OSC_TAB_SIZE);
      idx = fts_wrapper_get_int(&frac, OSC_TAB_BITS);
      f = fts_wrapper_frac_get_wrap(&frac);
      
      out[i] = tab[idx].value + f * tab[idx].slope;
    }
}


static void
ftl_osc_phase_64_inplace(fts_word_t *argv)
{
  float * restrict sig = (float *) fts_word_get_ptr(argv + 0);
  osc_control_t * restrict this = (osc_control_t *)fts_word_get_ptr(argv + 1);
  wavetab_samp_t * restrict tab;
  int i;

  /* This function is used when osc is used as a wave shaper; result,
     frequency and accumulated phase are always 0 */

  tab = this->table;

  for(i=0; i<64; i++)
    {
      fts_wrapper_t frac;
      int idx;
      float f;

      fts_wrapper_frac_set(&frac, sig[i] * (double)OSC_TAB_SIZE);
      idx = fts_wrapper_get_int(&frac, OSC_TAB_BITS);
      f = fts_wrapper_frac_get_wrap(&frac);
      
      sig[i] = tab[idx].value + f * tab[idx].slope;
    }
}

static void
ftl_osc_freq(fts_word_t *argv)
{
  float * restrict freq = (float *) fts_word_get_ptr(argv + 0);
  float * restrict out  = (float *) fts_word_get_ptr(argv + 1);
  osc_control_t * restrict this = (osc_control_t *)fts_word_get_ptr(argv + 2);
  long int n = fts_word_get_long(argv + 3);
  wavetab_samp_t * restrict tab = this->table;
  fts_wrapper_t phi;
  double incr = this->incr;
  int i;

  fts_wrapper_frac_set(&phi, this->phase);

  for(i=0; i<n; i++)
    {
      fts_wrapper_t frac;
      int idx;
      double f;

      fts_wrapper_copy(&phi, &frac);
      idx = fts_wrapper_get_int(&frac, OSC_TAB_BITS);
      fts_wrapper_incr(&phi, freq[i] * incr);
      f = fts_wrapper_frac_get_wrap(&frac);
      
      out[i] = tab[idx].value + f * tab[idx].slope;
    }

  /* use wrapper phi to wrap phase into oscillator table range */
  this->phase = fts_wrap(&phi, fts_wrapper_frac_get(&phi), OSC_TAB_BITS);
}

static void
ftl_osc_freq_inplace(fts_word_t *argv)
{
  float * restrict inout = (float *) fts_word_get_ptr(argv + 0);
  osc_control_t * restrict this = (osc_control_t *)fts_word_get_ptr(argv + 1);
  long int n = fts_word_get_long(argv + 2);
  wavetab_samp_t * restrict  tab = this->table;
  fts_wrapper_t phi;
  double incr = this->incr;
  int i;

  fts_wrapper_frac_set(&phi, this->phase);

  for(i=0; i<n; i++)
    {
      fts_wrapper_t frac;
      int idx;
      double f;

      fts_wrapper_copy(&phi, &frac);
      idx = fts_wrapper_get_int(&frac, OSC_TAB_BITS);
      fts_wrapper_incr(&phi, inout[i] * incr);
      f = fts_wrapper_frac_get_wrap(&frac);
      
      inout[i] = tab[idx].value + f * tab[idx].slope;
    }

  /* use wrapper phi to wrap phase into oscillator table range */
  this->phase = fts_wrap(&phi, fts_wrapper_frac_get(&phi), OSC_TAB_BITS);
}


static void
ftl_osc(fts_word_t *argv)
{
  float *freq  = (float *) fts_word_get_ptr(argv + 0);
  float *phase = (float *) fts_word_get_ptr(argv + 1);
  float *out   = (float *) fts_word_get_ptr(argv + 2);
  osc_control_t *this = (osc_control_t *)fts_word_get_ptr(argv + 3);
  long int n = fts_word_get_long(argv + 4);
  wavetab_samp_t *tab = this->table;
  fts_wrapper_t phi;
  double incr = this->incr;
  int i;

  /* get running phase from last time */
  fts_wrapper_frac_set(&phi, this->phase);

  for(i=0; i<n; i++)
    {
      fts_wrapper_t frac;
      int idx;
      double f;
      double a, b;

      /* copy running phase into fractional wrapper */
      fts_wrapper_copy(&phi, &frac);

      /* add offset phase to running phase */
      fts_wrapper_incr(&frac, phase[i]);

      /* increment running phase */
      fts_wrapper_incr(&phi, freq[i] * incr);

      /* get integer and (after wrapping) fractional part */
      idx = fts_wrapper_get_int(&frac, OSC_TAB_BITS);
      f = fts_wrapper_frac_get_wrap(&frac);
      
      /* interpolate */
      out[i] = tab[idx].value + f * tab[idx].slope;
    }

  /* use wrapper phi to wrap running phase into oscillator table range */
  this->phase = fts_wrap(&phi, fts_wrapper_frac_get(&phi), OSC_TAB_BITS);

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
  ftl_data_set(osc_control_t, this->osc_ftl_data, phase, &phase);
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
	  
	  ftl_data_set(osc_control_t, this->osc_ftl_data, table, &(wavetab->table));
	}
      else
	post("osc1~: set %s: can not find table\n", fts_symbol_name(s));
    }
  else
    ftl_data_set(osc_control_t, this->osc_ftl_data, table, &sin_tab);
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

static int
make_sin_tab(void)
{
  int i;

  sin_tab = (wavetab_samp_t *) fts_malloc(OSC_TAB_SIZE * sizeof(wavetab_samp_t));

  if (! sin_tab)
    return (0);

  for (i = 0; i < OSC_TAB_SIZE; i++)
    sin_tab[i].value = cos(i * (2*3.141593f/OSC_TAB_SIZE));

  for (i = 0; i < OSC_TAB_SIZE; i++)
    sin_tab[i].slope = 	(sin_tab[(i + 1) & (OSC_TAB_SIZE - 1)].value - sin_tab[i].value);

  return (1);
}

void
osc_config(void)
{
  if (! make_sin_tab())
    {
      post("osc~: out of memory\n");
      return;
    }

  /* Declare the oscillator related FTL functions */

  osc_phase_function = fts_new_symbol("ftl_osc_phase");
  dsp_declare_function(osc_phase_function, ftl_osc_phase);

  osc_phase_64_function = fts_new_symbol("ftl_osc_phase_64");
  dsp_declare_function(osc_phase_64_function, ftl_osc_phase_64);

  osc_phase_inplace_function = fts_new_symbol("ftl_osc_phase_inplace");
  dsp_declare_function(osc_phase_inplace_function, ftl_osc_phase_inplace);

  osc_phase_64_inplace_function = fts_new_symbol("ftl_osc_phase_64_inplace");
  dsp_declare_function(osc_phase_64_inplace_function, ftl_osc_phase_64_inplace);

  osc_freq_function = fts_new_symbol("ftl_osc_freq");
  dsp_declare_function(osc_freq_function, ftl_osc_freq);

  osc_freq_inplace_function = fts_new_symbol("ftl_osc_freq_inplace");
  dsp_declare_function(osc_freq_inplace_function, ftl_osc_freq_inplace);

  osc_function = fts_new_symbol("ftl_osc");
  dsp_declare_function(osc_function, ftl_osc);

  /* class declaration */

  fts_metaclass_create(fts_new_symbol("osc1~"),osc_instantiate, fts_always_equiv);
}




















