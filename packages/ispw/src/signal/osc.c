#include "fts.h"
#include <math.h>
#include <string.h>

#define TAB_BITS     9
#define TAB_NPOINTS  (1 << TAB_BITS)
#define TAB_FRACBITS 8

#define TAB_FRAC     (1 << TAB_FRACBITS)
#define TAB_K1       (TAB_NPOINTS * TAB_FRAC)
#define TAB_K2       (TAB_NPOINTS * TAB_FRAC - 1)
#define TAB_K3       (1.0f / (TAB_NPOINTS * TAB_FRAC))
#define TAB_K4       (31 - TAB_BITS-TAB_FRACBITS)

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
  wavetab_samp_t *samps;
  fts_symbol_t sym;
  int refcnt;
  int nowrap;
} wavetab_t;

static void
wavetable_load(wavetab_t *wavetab)
{
  const char *file_name = fts_symbol_name(wavetab->sym);
  float buf[TAB_NPOINTS];

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
      
      n_samples = fts_soundfile_read_float(sf, buf, TAB_NPOINTS);
      fts_soundfile_close(sf);

      if(n_samples < TAB_NPOINTS)
	{
	  post("tab1~: %s: can not read wave table\n", file_name);
	  return;
	}
      
      /* get value of first wavetable point */
      wavetab->samps[0].value = buf[0];

      /* next values and slopes */
      for(i=1; i<TAB_NPOINTS; i++)
	{
	  wavetab->samps[i].value = buf[i];
	  wavetab->samps[i-1].slope = (wavetab->samps[i].value - wavetab->samps[i-1].value) / (float) TAB_FRAC;  
	}

      /* get slope of last wavetable point in dependency of wrapping mode */
      if(strstr(file_name, "nowrap") || wavetab->nowrap)
	wavetab->samps[TAB_NPOINTS-1].slope = wavetab->samps[TAB_NPOINTS-2].slope;
      else
	wavetab->samps[TAB_NPOINTS-1].slope = (wavetab->samps[0].value - wavetab->samps[TAB_NPOINTS-1].value) / (float) TAB_FRAC;
    }    
}

static wavetab_t *
wavetable_new(fts_symbol_t name, fts_symbol_t wrap_mode)
{
  wavetab_samp_t *samps = (wavetab_samp_t *) fts_malloc(TAB_NPOINTS * sizeof(wavetab_samp_t));
  wavetab_t *wavetab;

  if(!samps)
    return 0;
  
  wavetab = (wavetab_t *)fts_malloc(sizeof(wavetab_t));
  wavetab->sym = name;
  wavetab->refcnt = 1;
  wavetab->samps = samps;

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
  fts_free((char *)wavetab->samps);
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
  long phase;
  float fconv;
  wavetab_samp_t *samps;
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
static fts_symbol_t osc_phase_function = 0;

#ifdef HI_OPT
static fts_symbol_t osc_phase_64_function = 0;
#endif

/* 
   TODO: 
   1- store the phase in the control structure as an int;
   it make no sense to convert back and forward from float at
   every tick.
   2- try to Compute four or more phase at a time, to make an unrolled
   loop with less interlocks ?
*/

static void
osc_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  osc_t *this = (osc_t *)o;
  fts_atom_t a;
  fts_atom_t argv[5];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);
  float f;
  const double zero = 0.0f;
  fts_atom_t data;

  f = 1.0f / fts_dsp_get_output_srate(dsp, 0);
  ftl_data_set(osc_control_t, this->osc_ftl_data, fconv, &f);
  ftl_data_set(osc_control_t, this->osc_ftl_data, phase, &zero);

  if (this->sym)
    {
      if (fts_hash_table_lookup(sigtab1_ht, this->sym, &data))
	{
	  wavetab_t *wavetab = (wavetab_t *) fts_get_ptr(&data);

          ftl_data_set(osc_control_t, this->osc_ftl_data, samps, &(wavetab->samps));
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
    ftl_data_set(osc_control_t, this->osc_ftl_data, samps, &sin_tab);

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
	  /* phase input, no frequency input */

#ifdef HI_OPT
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
#else
	  fts_set_symbol (argv + 0, fts_dsp_get_input_name(dsp, 1));
	  fts_set_symbol (argv + 1, fts_dsp_get_output_name(dsp, 0));
	  fts_set_ftl_data(argv+ 2, this->osc_ftl_data);
	  fts_set_long   (argv + 3, fts_dsp_get_input_size(dsp, 0));

	  dsp_add_funcall(osc_phase_function, 4, argv);
#endif
	}
    }
  else
    {
      if (fts_dsp_is_input_null(dsp, 1))
	{
	  /* no phase input, frequency input */

	  fts_set_symbol (argv + 0, fts_dsp_get_input_name(dsp, 0));
	  fts_set_symbol (argv + 1, fts_dsp_get_output_name(dsp, 0));
	  fts_set_ftl_data(argv+ 2, this->osc_ftl_data);
	  fts_set_long   (argv + 3, fts_dsp_get_input_size(dsp, 0));

	  dsp_add_funcall(osc_freq_function, 4, argv);
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

 #ifdef OLD
static void
osc_wrapper(fts_word_t *argv)
{
  float *freq  = (float *) fts_word_get_ptr(argv + 0);
  float *phase = (float *) fts_word_get_ptr(argv + 1);
  float *out   = (float *) fts_word_get_ptr(argv + 2);
  osc_control_t *this = (osc_control_t *)fts_word_get_ptr(argv + 3);
  long int n = fts_word_get_long(argv + 4);
  int     ophase;
  float   fconv;
  wavetab_samp_t *tab;

  ophase = this->phase;
  fconv  = this->fconv * 0x7fffffffL;
  tab    = this->samps;

  while (n--)
    {
      int idx;


      idx    = ((((long)(*phase * TAB_K1 + (ophase >> TAB_K4))) & TAB_K2) >> TAB_FRACBITS);
      *out   = (tab[idx].value +
		tab[idx].slope * ((((long)(*phase * TAB_K1 + (ophase >> TAB_K4))) & (TAB_FRAC-1))));
      ophase = (ophase + (int)(fconv * *freq)) & 0x7fffffffL;

      freq++;
      phase++;
      out++;
    }

  this->phase = ophase;
}


static void
osc_phase_wrapper(fts_word_t *argv)
{
  float *phase = (float *) fts_word_get_ptr(argv + 0);
  float *out   = (float *) fts_word_get_ptr(argv + 1);
  osc_control_t *this = (osc_control_t *)fts_word_get_ptr(argv + 2);
  long int n = fts_word_get_long(argv + 3);

  int     ophase;
  float   fconv;
  wavetab_samp_t *tab;

  /* This function is used when osc is used as a wave shaper; result,
     frequency and accumulated phase are always 0 */

  fconv  = this->fconv * 0x7fffffffL;
  tab    = this->samps;

  while (n--)
    {
      int idx;

      idx    = ((((long)(*phase * TAB_K1) & TAB_K2) >> TAB_FRACBITS);
      *out   = (tab[idx].value +
		tab[idx].slope * (((long)(*phase * TAB_K1 )) & (TAB_FRAC-1)));

      phase++;
      out++;
    }
}

static void
osc_freq_wrapper(fts_word_t *argv)
{
  float *freq = (float *) fts_word_get_ptr(argv + 0);
  float *out  = (float *) fts_word_get_ptr(argv + 1);
  osc_control_t *this = (osc_control_t *)fts_word_get_ptr(argv + 2);
  long int n = fts_word_get_long(argv + 3);
  int     ophase;
  float   fconv;
  wavetab_samp_t *tab;

  /* This version is used when there is no phase modulation */

  ophase = this->phase;
  fconv  = this->fconv * 0x7fffffffL;
  tab    = this->samps;

  while (n--)
    {
      int idx;


      idx    = (((ophase >> TAB_K4)) & TAB_K2) >> TAB_FRACBITS;
      *out   = (tab[idx].value +
		tab[idx].slope * ((ophase >> TAB_K4) & (TAB_FRAC - 1)))
      ophase = (ophase + (int)(fconv * *freq)) & 0x7fffffffL;

      freq++;
      out++;
    }

  this->phase = ophase;
}

#else

/* This version is faster than the previous one in pipelined machines, because using
   a little bit of loop unrolling reordering get less dependencies
   and work a little bit better in pipelined arch.
   For superscalar processor should be modified again.
*/

static void
osc_phase_wrapper(fts_word_t *argv)
{
  float *phase = (float *) fts_word_get_ptr(argv + 0);
  float *out   = (float *) fts_word_get_ptr(argv + 1);
  osc_control_t *this = (osc_control_t *)fts_word_get_ptr(argv + 2);
  long int n = fts_word_get_long(argv + 3);
  wavetab_samp_t *tab;
  int i;

  /* This function is used when osc is used as a wave shaper; result,
     frequency and accumulated phase are always 0 */

  tab = this->samps;

  for (i = 0; i < n; i+=4)
    {
      int idx0, idx1, idx2, idx3;
      float o0, o1, o2, o3;
      float ph0, ph1, ph2, ph3;

      ph0 = phase[i + 0] * TAB_K1;
      ph1 = phase[i + 1] * TAB_K1;
      ph2 = phase[i + 2] * TAB_K1;
      ph3 = phase[i + 3] * TAB_K1;

      idx0 = ((long)ph0 & TAB_K2) >> TAB_FRACBITS;
      o0 = (float)((float) tab[idx0].value + (float) tab[idx0].slope * (float) ((long)ph0 & (TAB_FRAC - 1)));

      idx1 = ((long)ph1 & TAB_K2) >> TAB_FRACBITS;
      o1 = (float)((float)tab[idx1].value + (float)tab[idx1].slope * (float)((long)ph1 & (TAB_FRAC - 1)));

      idx2 = ((long)ph2 & TAB_K2) >> TAB_FRACBITS;
      o2 = (float)((float)tab[idx2].value + (float)tab[idx2].slope * (float)((long)ph2 & (TAB_FRAC - 1)));

      idx3 = ((long)ph3 & TAB_K2) >> TAB_FRACBITS;
      o3 = (float)((float)tab[idx3].value + (float)tab[idx3].slope * (float)((long)ph3 & (TAB_FRAC - 1)));

      out[i + 0] = o0;
      out[i + 1] = o1;
      out[i + 2] = o2;
      out[i + 3] = o3;
    }
}

/* version with vs == 64 */

#ifdef HI_OPT
static void
osc_phase_64_wrapper(fts_word_t *argv)
{
  float *phase = (float *) fts_word_get_ptr(argv + 0);
  float *out   = (float *) fts_word_get_ptr(argv + 1);
  osc_control_t *this = (osc_control_t *)fts_word_get_ptr(argv + 2);
  wavetab_samp_t *tab;
  int i;

  /* This function is used when osc is used as a wave shaper; result,
     frequency and accumulated phase are always 0 */

  tab = this->samps;

  for (i = 0; i < 64; i+=4)
    {
      int idx0, idx1, idx2, idx3;
      float o0, o1, o2, o3;
      float ph0, ph1, ph2, ph3;

      ph0 = phase[i + 0] * TAB_K1;
      ph1 = phase[i + 1] * TAB_K1;
      ph2 = phase[i + 2] * TAB_K1;
      ph3 = phase[i + 3] * TAB_K1;

      idx0 = ((long)ph0 & TAB_K2) >> TAB_FRACBITS;
      o0 = (float)((float) tab[idx0].value + (float) tab[idx0].slope * (float) ((long)ph0 & (TAB_FRAC - 1)));

      idx1 = ((long)ph1 & TAB_K2) >> TAB_FRACBITS;
      o1 = (float)((float)tab[idx1].value + (float)tab[idx1].slope * (float)((long)ph1 & (TAB_FRAC - 1)));

      idx2 = ((long)ph2 & TAB_K2) >> TAB_FRACBITS;
      o2 = (float)((float)tab[idx2].value + (float)tab[idx2].slope * (float)((long)ph2 & (TAB_FRAC - 1)));

      idx3 = ((long)ph3 & TAB_K2) >> TAB_FRACBITS;
      o3 = (float)((float)tab[idx3].value + (float)tab[idx3].slope * (float)((long)ph3 & (TAB_FRAC - 1)));

      out[i + 0] = o0;
      out[i + 1] = o1;
      out[i + 2] = o2;
      out[i + 3] = o3;
    }
}
#endif

static void
osc_freq_wrapper(fts_word_t *argv)
{
  float *freq = (float *) fts_word_get_ptr(argv + 0);
  float *out  = (float *) fts_word_get_ptr(argv + 1);
  osc_control_t *this = (osc_control_t *)fts_word_get_ptr(argv + 2);
  long int n = fts_word_get_long(argv + 3);
  int     ophase;
  float   fconv;
  wavetab_samp_t *tab;
  int i;

  ophase = this->phase;
  fconv = this->fconv * 0x7fffffffL;
  tab = this->samps;

  for (i = 0; i < n; i+=4)
    {
      int idx;
      long k;
      float o0, o1, o2, o3;
      int d0, d1, d2, d3;

      d0 = (int)(fconv * freq[i + 0]);
      d1 = (int)(fconv * freq[i + 1]);
      d2 = (int)(fconv * freq[i + 2]);
      d3 = (int)(fconv * freq[i + 3]);


      k = ophase >> TAB_K4;
      idx = ((k & TAB_K2) >> TAB_FRACBITS);
      o0 = (float)((float) tab[idx].value + (float) tab[idx].slope * (float) (k & (TAB_FRAC - 1)));
      ophase = (ophase + d0) & 0x7fffffffL;


      k = ophase >> TAB_K4;
      idx = ((k & TAB_K2) >> TAB_FRACBITS);
      o1 = (float)((float)tab[idx].value + (float)tab[idx].slope * (float)(k & (TAB_FRAC - 1)));
      ophase = (ophase + d1) & 0x7fffffffL;

      k = ophase >> TAB_K4;
      idx = ((k & TAB_K2) >> TAB_FRACBITS);
      o2 = (float)((float)tab[idx].value + (float)tab[idx].slope * (float)(k & (TAB_FRAC - 1)));
      ophase = (ophase + d2) & 0x7fffffffL;

      k = ophase >> TAB_K4;
      idx = ((k & TAB_K2) >> TAB_FRACBITS);
      o3 = (float)((float)tab[idx].value + (float)tab[idx].slope * (float)(k & (TAB_FRAC - 1)));
      ophase = (ophase + d3) & 0x7fffffffL;

      out[i + 0] = o0;
      out[i + 1] = o1;
      out[i + 2] = o2;
      out[i + 3] = o3;
    }

  this->phase = ophase;
}


static void
osc_wrapper(fts_word_t *argv)
{
  float *freq  = (float *) fts_word_get_ptr(argv + 0);
  float *phase = (float *) fts_word_get_ptr(argv + 1);
  float *out   = (float *) fts_word_get_ptr(argv + 2);
  osc_control_t *this = (osc_control_t *)fts_word_get_ptr(argv + 3);
  long int n = fts_word_get_long(argv + 4);
  int     ophase;
  float   fconv;
  wavetab_samp_t *tab;
  int i;

  ophase = this->phase;
  fconv = this->fconv * 0x7fffffffL;
  tab = this->samps;

  for (i = 0; i < n; i+=4)
    {
      int idx;
      long k;
      float o0, o1, o2, o3;
      float ph0, ph1, ph2, ph3;
      int d0, d1, d2, d3;

      d0 = (int)(fconv * freq[i + 0]);
      d1 = (int)(fconv * freq[i + 1]);
      d2 = (int)(fconv * freq[i + 2]);
      d3 = (int)(fconv * freq[i + 3]);

      ph0 = phase[i + 0] * TAB_K1;
      ph1 = phase[i + 1] * TAB_K1;
      ph2 = phase[i + 2] * TAB_K1;
      ph3 = phase[i + 3] * TAB_K1;


      k = (long)(ph0 + (ophase >> TAB_K4));
      idx = ((k & TAB_K2) >> TAB_FRACBITS);
      o0 = (float)((float) tab[idx].value + (float) tab[idx].slope * (float) (k & (TAB_FRAC - 1)));
      ophase = (ophase + d0) & 0x7fffffffL;


      k = (long)(ph1 + (ophase >> TAB_K4));
      idx = ((k & TAB_K2) >> TAB_FRACBITS);
      o1 = (float)((float)tab[idx].value + (float)tab[idx].slope * (float)(k & (TAB_FRAC - 1)));
      ophase = (ophase + d1) & 0x7fffffffL;


      k = (long)(ph2 + (ophase >> TAB_K4));
      idx = ((k & TAB_K2) >> TAB_FRACBITS);
      o2 = (float)((float)tab[idx].value + (float)tab[idx].slope * (float)(k & (TAB_FRAC - 1)));
      ophase = (ophase + d2) & 0x7fffffffL;


      k = (long)(ph3 + (ophase >> TAB_K4));
      idx = ((k & TAB_K2) >> TAB_FRACBITS);
      o3 = (float)((float)tab[idx].value + (float)tab[idx].slope * (float)(k & (TAB_FRAC - 1)));
      ophase = (ophase + d3) & 0x7fffffffL;

      out[i + 0] = o0;
      out[i + 1] = o1;
      out[i + 2] = o2;
      out[i + 3] = o3;
    }

  this->phase = ophase;
}
#endif

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
  
  phase = 0x7fffffffL * ((float) fts_get_number_arg(ac, at, 0, 0));
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
	  
	  ftl_data_set(osc_control_t, this->osc_ftl_data, samps, &(wavetab->samps));
	}
      else
	post("osc1~: set %s: can not find table\n", fts_symbol_name(s));
    }
  else
    ftl_data_set(osc_control_t, this->osc_ftl_data, samps, &sin_tab);
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

  sin_tab = (wavetab_samp_t *) fts_malloc(TAB_NPOINTS * sizeof(wavetab_samp_t));

  if (! sin_tab)
    return (0);

  for (i = 0; i < TAB_NPOINTS; i++)
    sin_tab[i].value = cos(i * (2*3.141593f/TAB_NPOINTS));

  for (i = 0; i < TAB_NPOINTS; i++)
    sin_tab[i].slope = 	(sin_tab[(i + 1) & (TAB_NPOINTS - 1)].value - sin_tab[i].value) / (float) TAB_FRAC;

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

  osc_phase_function = fts_new_symbol("osc_p");
  dsp_declare_function(osc_phase_function, osc_phase_wrapper);

#ifdef HI_OPT
  osc_phase_function = fts_new_symbol("osc_p");
  dsp_declare_function(osc_phase_function, osc_phase_wrapper);

  osc_phase_64_function = fts_new_symbol("osc_p_64");
  dsp_declare_function(osc_phase_64_function, osc_phase_64_wrapper);
#endif

  osc_freq_function = fts_new_symbol("osc_f");
  dsp_declare_function(osc_freq_function, osc_freq_wrapper);

  osc_function = fts_new_symbol("osc");
  dsp_declare_function(osc_function, osc_wrapper);

  /* class declaration */

  fts_metaclass_create(fts_new_symbol("osc1~"),osc_instantiate, fts_always_equiv);
}




















