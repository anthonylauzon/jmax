/*
 *                      Copyright (c) 1993 by IRCAM
 *                          All rights reserved.
 *
 *  For any information regarding this and other IRCAM software, please
 *  send email to:
 *                              manager@ircam.fr
 *
 *      $Revision: 1.1 $ IRCAM $Date: 1998/09/19 14:37:02 $
 *
 * Oscillator.
 * Based on the old version by Miller Puckette.
 *
 */

/* In this non i860 version */

#include "fts.h"



#include <math.h>
#include <string.h>

#define SIN_BITS     9
#define SIN_NPOINTS  (1 << SIN_BITS)
#define SIN_FRACBITS 8

#define SIN_FRAC     (1 << SIN_FRACBITS)
#define SIN_K1       (SIN_NPOINTS * SIN_FRAC)
#define SIN_K2       (SIN_NPOINTS * SIN_FRAC - 1)
#define SIN_K3       (1.0f / (SIN_NPOINTS * SIN_FRAC))
#define SIN_K4       (31 - SIN_BITS-SIN_FRACBITS)

typedef struct
{
  float value;
  float slope;
} sinsamp_t;

#define SINTABSIZE (SIN_NPOINTS * sizeof(sinsamp_t))

/* ---------------------------- tab1 ----------------------------------- */

static fts_hash_table_t *sigtab1_ht;

typedef struct {
  sinsamp_t *samps;
  fts_symbol_t sym;
  fts_symbol_t dir;
  int refcnt;
} wavetab_t;

typedef struct {
  fts_object_t _o;
  wavetab_t *wavetab;
} sigtab1_t;

#define HEADERSIZE 28
#define NINTERP    2
#define TABLESIZE  0x1000

#define NEEDBYTES (HEADERSIZE + TABLESIZE*sizeof(short)/sizeof(sinsamp_t))

static void
common_bang(wavetab_t *wavetab)
{
  const char *name = fts_symbol_name(wavetab->sym);
  int fd = fts_file_open(name, wavetab->dir, "r");
  const char *basename, *s2;
  char *tempbuf, *rats;
  long  n;
  sinsamp_t *sptr;

  if (fd < 0)
    {
      post("tab1~ %s: can't open\n", name);
      return;
    }

  tempbuf = fts_malloc(NEEDBYTES);

  if (!tempbuf)
    {
      post("tab1: can't get buffer space for read\n");
      fts_file_close(fd);
      return;
    }

  n = read(fd, tempbuf, NEEDBYTES);

  if (n < NEEDBYTES)
    {
      if (n < 0)
	post("tab1~: %s: read failed\n", name);
      else
	post("tab1~: %s: too short\n", name);

      fts_file_close(fd);
      fts_free(tempbuf);

      return;
    }

  for (n = 0, rats = tempbuf + HEADERSIZE, sptr = wavetab->samps;
       n < SIN_NPOINTS; rats += sizeof(short), sptr++, n++)
    {
      sptr->value = (1.0f/32767.0f) * *(short *)rats;

      if (n)
	(sptr-1)->slope = (sptr->value - (sptr-1)->value) / (float) SIN_FRAC; /* mdc: change the scaling */
    }

  for (s2 = basename = name; *s2; s2++) 
    if (*s2 == '/')
      basename = s2 + 1;

  if (!strncmp(basename, "nowrap", 6))
    {
      /* post("tab1: reading %s (non-wraparound)\n", name);	*/
      wavetab->samps[SIN_NPOINTS-1].slope = wavetab->samps[SIN_NPOINTS-2].slope;
    }
  else
    {
      /* post("tab1: reading %s\n", name);	*/
      wavetab->samps[SIN_NPOINTS-1].slope = (wavetab->samps[0].value - wavetab->samps[SIN_NPOINTS-1].value) / (float) SIN_FRAC;
    }

/*  print_samples(wavetab->samps);*/
  fts_file_close(fd);

  fts_free(tempbuf);
}

static wavetab_t *
common_new(fts_symbol_t s)
{
  sinsamp_t *samps = (sinsamp_t *) fts_malloc(SINTABSIZE);
  wavetab_t *wavetab;

  if (!samps)
    return 0;
  
  wavetab = (wavetab_t *)fts_zalloc(sizeof(wavetab_t));
  wavetab->sym = s;
  wavetab->refcnt = 1;
  wavetab->samps = samps;
  wavetab->dir = fts_get_default_directory();
  common_bang(wavetab);
  return wavetab;
}

static void
common_delete(wavetab_t *wavetab)
{
  fts_hash_table_remove(sigtab1_ht, wavetab->sym);

  fts_free((char *)wavetab->samps);
  fts_free(wavetab);
}

static void
sigtab1_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigtab1_t *this = (sigtab1_t *)o;
  if (this->wavetab)
    common_bang(this->wavetab);
}

static void
sigtab1_init(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fts_atom_t data;
  sigtab1_t *this = (sigtab1_t *)o;
  fts_symbol_t s = fts_get_symbol_arg(ac, at, 1, 0);
  int r;

  if (!s)
    return;

  if (fts_hash_table_lookup(sigtab1_ht, s, &data))
    {
      this->wavetab = (wavetab_t *)fts_get_ptr(&data);
      this->wavetab->refcnt++;
    }
  else
    {
      this->wavetab = common_new(s);
      r = fts_hash_table_insert(sigtab1_ht, s, (void *)this->wavetab);
    }
}

static void
sigtab1_delete(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sigtab1_t *this = (sigtab1_t *)o;

  if (this->wavetab && !--this->wavetab->refcnt)
    common_delete(this->wavetab);
}

static fts_status_t
sigtab1_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  fts_class_init(cl, sizeof(sigtab1_t), 1, 0, 0);

  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, sigtab1_init, 2, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, sigtab1_delete, 0, a);

  fts_method_define(cl, 0, fts_s_bang, sigtab1_bang, 0, a);
  
  return fts_Success;
}

void
sigtab1_config(void)
{
  fts_metaclass_create(fts_new_symbol("tab1~"),sigtab1_instantiate, fts_always_equiv);

  sigtab1_ht = fts_hash_table_new();
  fts_hash_table_init(sigtab1_ht);
}

/* ---------------------------- osc1 ----------------------------------- */

typedef struct 
{
  long phase;
  float fconv;
  sinsamp_t *samps;
} osc_control_t;

typedef struct 
{
  fts_object_t obj;
  fts_symbol_t sym;
  ftl_data_t osc_ftl_data;
} osc_t;

static sinsamp_t *sin_tab;
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
  sinsamp_t *tab;

  ophase = this->phase;
  fconv  = this->fconv * 0x7fffffffL;
  tab    = this->samps;

  while (n--)
    {
      int idx;


      idx    = ((((long)(*phase * SIN_K1 + (ophase >> SIN_K4))) & SIN_K2) >> SIN_FRACBITS);
      *out   = (tab[idx].value +
		tab[idx].slope * ((((long)(*phase * SIN_K1 + (ophase >> SIN_K4))) & (SIN_FRAC-1))));
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
  sinsamp_t *tab;

  /* This function is used when osc is used as a wave shaper; result,
     frequency and accumulated phase are always 0 */

  fconv  = this->fconv * 0x7fffffffL;
  tab    = this->samps;

  while (n--)
    {
      int idx;

      idx    = ((((long)(*phase * SIN_K1) & SIN_K2) >> SIN_FRACBITS);
      *out   = (tab[idx].value +
		tab[idx].slope * (((long)(*phase * SIN_K1 )) & (SIN_FRAC-1)));

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
  sinsamp_t *tab;

  /* This version is used when there is no phase modulation */

  ophase = this->phase;
  fconv  = this->fconv * 0x7fffffffL;
  tab    = this->samps;

  while (n--)
    {
      int idx;


      idx    = (((ophase >> SIN_K4)) & SIN_K2) >> SIN_FRACBITS;
      *out   = (tab[idx].value +
		tab[idx].slope * ((ophase >> SIN_K4) & (SIN_FRAC - 1)))
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
  sinsamp_t *tab;
  int i;

  /* This function is used when osc is used as a wave shaper; result,
     frequency and accumulated phase are always 0 */

  tab = this->samps;

  for (i = 0; i < n; i+=4)
    {
      int idx0, idx1, idx2, idx3;
      float o0, o1, o2, o3;
      float ph0, ph1, ph2, ph3;

      ph0 = phase[i + 0] * SIN_K1;
      ph1 = phase[i + 1] * SIN_K1;
      ph2 = phase[i + 2] * SIN_K1;
      ph3 = phase[i + 3] * SIN_K1;

      idx0 = ((long)ph0 & SIN_K2) >> SIN_FRACBITS;
      o0 = (float)((float) tab[idx0].value + (float) tab[idx0].slope * (float) ((long)ph0 & (SIN_FRAC - 1)));

      idx1 = ((long)ph1 & SIN_K2) >> SIN_FRACBITS;
      o1 = (float)((float)tab[idx1].value + (float)tab[idx1].slope * (float)((long)ph1 & (SIN_FRAC - 1)));

      idx2 = ((long)ph2 & SIN_K2) >> SIN_FRACBITS;
      o2 = (float)((float)tab[idx2].value + (float)tab[idx2].slope * (float)((long)ph2 & (SIN_FRAC - 1)));

      idx3 = ((long)ph3 & SIN_K2) >> SIN_FRACBITS;
      o3 = (float)((float)tab[idx3].value + (float)tab[idx3].slope * (float)((long)ph3 & (SIN_FRAC - 1)));

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
  sinsamp_t *tab;
  int i;

  /* This function is used when osc is used as a wave shaper; result,
     frequency and accumulated phase are always 0 */

  tab = this->samps;

  for (i = 0; i < 64; i+=4)
    {
      int idx0, idx1, idx2, idx3;
      float o0, o1, o2, o3;
      float ph0, ph1, ph2, ph3;

      ph0 = phase[i + 0] * SIN_K1;
      ph1 = phase[i + 1] * SIN_K1;
      ph2 = phase[i + 2] * SIN_K1;
      ph3 = phase[i + 3] * SIN_K1;

      idx0 = ((long)ph0 & SIN_K2) >> SIN_FRACBITS;
      o0 = (float)((float) tab[idx0].value + (float) tab[idx0].slope * (float) ((long)ph0 & (SIN_FRAC - 1)));

      idx1 = ((long)ph1 & SIN_K2) >> SIN_FRACBITS;
      o1 = (float)((float)tab[idx1].value + (float)tab[idx1].slope * (float)((long)ph1 & (SIN_FRAC - 1)));

      idx2 = ((long)ph2 & SIN_K2) >> SIN_FRACBITS;
      o2 = (float)((float)tab[idx2].value + (float)tab[idx2].slope * (float)((long)ph2 & (SIN_FRAC - 1)));

      idx3 = ((long)ph3 & SIN_K2) >> SIN_FRACBITS;
      o3 = (float)((float)tab[idx3].value + (float)tab[idx3].slope * (float)((long)ph3 & (SIN_FRAC - 1)));

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
  sinsamp_t *tab;
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


      k = ophase >> SIN_K4;
      idx = ((k & SIN_K2) >> SIN_FRACBITS);
      o0 = (float)((float) tab[idx].value + (float) tab[idx].slope * (float) (k & (SIN_FRAC - 1)));
      ophase = (ophase + d0) & 0x7fffffffL;


      k = ophase >> SIN_K4;
      idx = ((k & SIN_K2) >> SIN_FRACBITS);
      o1 = (float)((float)tab[idx].value + (float)tab[idx].slope * (float)(k & (SIN_FRAC - 1)));
      ophase = (ophase + d1) & 0x7fffffffL;

      k = ophase >> SIN_K4;
      idx = ((k & SIN_K2) >> SIN_FRACBITS);
      o2 = (float)((float)tab[idx].value + (float)tab[idx].slope * (float)(k & (SIN_FRAC - 1)));
      ophase = (ophase + d2) & 0x7fffffffL;

      k = ophase >> SIN_K4;
      idx = ((k & SIN_K2) >> SIN_FRACBITS);
      o3 = (float)((float)tab[idx].value + (float)tab[idx].slope * (float)(k & (SIN_FRAC - 1)));
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
  sinsamp_t *tab;
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

      ph0 = phase[i + 0] * SIN_K1;
      ph1 = phase[i + 1] * SIN_K1;
      ph2 = phase[i + 2] * SIN_K1;
      ph3 = phase[i + 3] * SIN_K1;


      k = (long)(ph0 + (ophase >> SIN_K4));
      idx = ((k & SIN_K2) >> SIN_FRACBITS);
      o0 = (float)((float) tab[idx].value + (float) tab[idx].slope * (float) (k & (SIN_FRAC - 1)));
      ophase = (ophase + d0) & 0x7fffffffL;


      k = (long)(ph1 + (ophase >> SIN_K4));
      idx = ((k & SIN_K2) >> SIN_FRACBITS);
      o1 = (float)((float)tab[idx].value + (float)tab[idx].slope * (float)(k & (SIN_FRAC - 1)));
      ophase = (ophase + d1) & 0x7fffffffL;


      k = (long)(ph2 + (ophase >> SIN_K4));
      idx = ((k & SIN_K2) >> SIN_FRACBITS);
      o2 = (float)((float)tab[idx].value + (float)tab[idx].slope * (float)(k & (SIN_FRAC - 1)));
      ophase = (ophase + d2) & 0x7fffffffL;


      k = (long)(ph3 + (ophase >> SIN_K4));
      idx = ((k & SIN_K2) >> SIN_FRACBITS);
      o3 = (float)((float)tab[idx].value + (float)tab[idx].slope * (float)(k & (SIN_FRAC - 1)));
      ophase = (ophase + d3) & 0x7fffffffL;

      out[i + 0] = o0;
      out[i + 1] = o1;
      out[i + 2] = o2;
      out[i + 3] = o3;
    }

  this->phase = ophase;
}
#endif



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


static int
make_sin_tab(void)
{
  int i;

  sin_tab = (sinsamp_t *) fts_malloc(SINTABSIZE);

  if (! sin_tab)
    return (0);

  for (i = 0; i < SIN_NPOINTS; i++)
    sin_tab[i].value = cos(i * (2*3.141593f/SIN_NPOINTS));

  for (i = 0; i < SIN_NPOINTS; i++)
    sin_tab[i].slope = 	(sin_tab[(i + 1) & (SIN_NPOINTS - 1)].value - sin_tab[i].value)/ (float) SIN_FRAC;

  return (1);
}

/* Args: an optional symbol */

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




















