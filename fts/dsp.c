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
 */

#include <fts/fts.h>
#include <ftsprivate/class.h>
#include <ftsprivate/connection.h>
#include <ftsprivate/sigconn.h>
#include <ftsprivate/dspgraph.h>
#include <ftsprivate/audio.h>
#include <ftsprivate/time.h>

#define FTS_DSP_DEFAULT_SAMPLE_RATE 44100

#define FTS_DSP_MIN_TICK_SIZE 8
#define FTS_DSP_MAX_TICK_SIZE 512
#define FTS_DSP_DEFAULT_TICK_SIZE 64

fts_dsp_graph_t main_dsp_graph;

fts_symbol_t fts_s_dsp_on = 0;
fts_symbol_t fts_s_sample_rate = 0;

/* main DSP graph parameters */
static double dsp_sample_rate;
static int dsp_tick_size;
static double dsp_tick_duration;

static double dsp_time = 0.0;

static fts_symbol_t dsp_zero_fun_symbol = 0;
static fts_symbol_t dsp_copy_fun_symbol = 0;

static fts_symbol_t dsp_timebase_symbol = 0;
static fts_timebase_t *dsp_timebase = 0;

/*********************************************************
 *
 *  DSP timebase
 *
 */
static void 
dsp_timebase_advance( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_timebase_advance(dsp_timebase);

  /* run DSP chain (or idle) */
  if (fts_dsp_graph_is_compiled(&main_dsp_graph))
    fts_dsp_graph_run(&main_dsp_graph);
  else
    fts_audio_idle();
}

static void
dsp_timebase_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  fts_timebase_t *this = (fts_timebase_t *)o;

  fts_timebase_init(this);
  fts_timebase_set_step(this, dsp_tick_duration);
}

static void
dsp_timebase_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  fts_timebase_t *this = (fts_timebase_t *)o;

  fts_timebase_reset(this);
}

static fts_status_t
dsp_timebase_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(fts_timebase_t), 0, 0, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, dsp_timebase_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, dsp_timebase_delete);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_sched_ready, dsp_timebase_advance);
  
  return fts_Success;
}

void
fts_dsp_timebase_configure(void)
{
  fts_atom_t a;

  /* create dsp timebase in root patcher (will be cleaned up with root patcher) */
  fts_class_install(dsp_timebase_symbol, dsp_timebase_instantiate);
  fts_set_symbol(&a, dsp_timebase_symbol);
  fts_object_new_to_patcher( fts_get_root_patcher(), 1, &a, (fts_object_t **)&dsp_timebase);

  /* set DSP time base as FTS master and add add it to the scheduler */  
  fts_set_timebase(dsp_timebase);
  fts_sched_add((fts_object_t *)dsp_timebase, FTS_SCHED_ALWAYS);
}

/*********************************************************************
 *
 *  dsp parameter call backs
 *
 */

static void
dsp_reset(void)
{
  dsp_tick_duration = dsp_tick_size * 1000.0 / dsp_sample_rate;
  
  fts_dsp_graph_reset(&main_dsp_graph);
  fts_dsp_graph_set_tick_size(&main_dsp_graph, dsp_tick_size);
  fts_dsp_graph_set_sample_rate(&main_dsp_graph, dsp_sample_rate);  

  /* reset audio ports !!! */
  /* redefine global system variable $SampleRate (todo!) */
}

static void 
dsp_set_tick_size(void *listener, fts_symbol_t name, const fts_atom_t *value)
{
  if (fts_is_number(value))
    {
      int n = fts_get_number_int(value);

      dsp_tick_size = n;
      dsp_reset();
    }
}

static void 
dsp_set_sample_rate(void *listener, fts_symbol_t name, const fts_atom_t *value)
{
  if (fts_is_number(value))
    {
      float sr = fts_get_number_float(value);

      dsp_sample_rate = sr;
      dsp_reset();
    }
}

static void 
dsp_set_on(void *listener, fts_symbol_t name, const fts_atom_t *value)
{
  if (fts_is_int(value))
    {
      int on = fts_get_int(value);

      if(on)
	fts_dsp_graph_compile(&main_dsp_graph);
      else
	fts_dsp_graph_reset(&main_dsp_graph);
    }
}

/**************************************************************************
 *
 *  general DSP API functions
 *
 */

void
fts_dsp_restart(void)
{
  if(fts_dsp_graph_is_compiled(&main_dsp_graph))
    {
      fts_dsp_graph_reset(&main_dsp_graph);      
      fts_dsp_graph_compile(&main_dsp_graph);
    }
}

int
fts_dsp_is_running(void)
{
  return fts_dsp_graph_is_compiled(&main_dsp_graph);
}

int
fts_dsp_get_tick_size(void)
{
  return fts_dsp_graph_get_tick_size(&main_dsp_graph);
}

double
fts_dsp_get_sample_rate()
{
  return fts_dsp_graph_get_sample_rate(&main_dsp_graph);
}

void 
fts_dsp_declare_function(fts_symbol_t name, void (*w)(fts_word_t *))
{
  ftl_declare_function( name, w);
}

static void 
sig_dummy( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
}

void 
fts_dsp_declare_inlet(fts_class_t *cl, int num)
{
  fts_method_define_varargs(cl, num, fts_s_sig, sig_dummy);
}

void 
fts_dsp_declare_outlet(fts_class_t *cl, int num)
{
  fts_outlet_type_define(cl, num, fts_s_sig, 0, 0);
}

void 
fts_dsp_add_object(fts_object_t *o)
{
  fts_dsp_graph_add_object(&main_dsp_graph, o);
}

void 
fts_dsp_remove_object(fts_object_t *o)
{
  fts_dsp_graph_remove_object(&main_dsp_graph, o);
}

void 
fts_dsp_add_function(fts_symbol_t symb, int ac, fts_atom_t *av)
{
  ftl_program_add_call(main_dsp_graph.chain, symb, ac, av);
}

int 
fts_dsp_is_sig_inlet(fts_object_t *o, int num)
{
  return fts_class_has_method( fts_object_get_class(o), num, fts_s_sig);
}

int 
fts_dsp_is_input_null( fts_dsp_descr_t *descr, int in)
{
  return descr->in[in] == sig_zero;
}

fts_object_t *
dsp_get_current_object()
{
  if(main_dsp_graph.chain)
    return ftl_program_get_current_object(main_dsp_graph.chain);
  else
    return 0;
}

void 
dsp_add_signal(fts_symbol_t name, int size)
{
  ftl_program_add_signal(main_dsp_graph.chain, name, size);
}

void 
dsp_chain_post(void)
{
  post("printing dsp chain:\n");
  ftl_program_post(main_dsp_graph.chain);
}

void 
dsp_chain_post_signals(void)
{
  post("printing signals:\n");
  ftl_program_post_signals_count(main_dsp_graph.chain);
}

void 
dsp_chain_fprint(FILE *f)
{
  fprintf(f, "printing dsp chain:\n");
  ftl_program_fprint(f, main_dsp_graph.chain);
}

void 
dsp_chain_fprint_signals(FILE *f)
{
  fprintf(f, "printing signals:\n");
  ftl_program_fprint_signals_count(f, main_dsp_graph.chain);
}

ftl_program_t *
dsp_get_current_dsp_chain( void)
{
  return main_dsp_graph.chain;
}

/**************************************************************************
 *
 *  kernel ftl functions
 *
 */

static void
dsp_zero_fun(fts_word_t *argv)
{
  float *out = (float *)fts_word_get_ptr(argv + 0);
  int n = fts_word_get_int(argv + 1);
  int i;

  for(i=0; i<n; i++)
    out[i] = 0.0;
}

static void
dsp_copy_fun(fts_word_t *argv)
{
  float *in = (float *)fts_word_get_ptr(argv + 0);
  float *out = (float *)fts_word_get_ptr(argv + 1);
  int n = fts_word_get_int(argv + 2);
  int i;

  for(i=0; i<n; i++)
    out[i] = 0.0;
}

void
fts_dsp_add_function_zero(fts_symbol_t signal, int size)
{
  fts_atom_t a[2];

  fts_set_symbol(a + 0, signal);
  fts_set_int(a + 1, size);
  dsp_add_funcall(dsp_zero_fun_symbol, 2, a);
}

void
fts_dsp_add_function_copy(fts_symbol_t in, fts_symbol_t out, int size)
{
  fts_atom_t a[3];

  fts_set_symbol(a + 0, in);
  fts_set_symbol(a + 1, out);
  fts_set_int(a + 2, size);
  dsp_add_funcall(dsp_copy_fun_symbol, 3, a);
}

/**************************************************************************
 *
 * Initialization and shutdown
 *
 */

void fts_kernel_dsp_init(void)
{
  fts_s_dsp_on = fts_new_symbol("dsp_on");
  fts_s_sample_rate = fts_new_symbol("sample_rate");
  dsp_timebase_symbol = fts_new_symbol("dsp_timebase");
  
  /* init sample rate */
  dsp_sample_rate = FTS_DSP_DEFAULT_SAMPLE_RATE;
  dsp_tick_size = FTS_DSP_DEFAULT_TICK_SIZE;
  dsp_tick_duration = 1000.0 * (double)FTS_DSP_DEFAULT_TICK_SIZE / FTS_DSP_DEFAULT_SAMPLE_RATE;
    
  /* init DSP parameters */
  fts_param_add_listener(fts_s_sample_rate, 0, dsp_set_sample_rate);
  fts_param_add_listener(fts_s_dsp_on, 0, dsp_set_on);

  /* create main DSP graph */
  fts_dsp_graph_init(&main_dsp_graph, dsp_tick_size, dsp_sample_rate);

  dsp_zero_fun_symbol = fts_new_symbol("dsp_zero_fun_symbol");
  fts_dsp_declare_function( dsp_zero_fun_symbol, dsp_zero_fun);
  
  dsp_copy_fun_symbol = fts_new_symbol("dsp_copy_fun_symbol");
  fts_dsp_declare_function( dsp_copy_fun_symbol, dsp_copy_fun);
}
