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
#include <fts/private/class.h>
#include <fts/private/connection.h>
#include <fts/private/sigconn.h>
#include <fts/private/dspgraph.h>
#include <fts/private/audio.h>

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
static double dsp_tick_duration_minus_one_sample;

/* DSP time */
double fts_dsp_time;

void 
fts_dsp_run_tick(void)
{
  /* advance clock to end of the tick (fire alarms) */
  fts_sched_advance_clock( fts_dsp_get_time() + dsp_tick_duration_minus_one_sample);
  
  /* increment dsp time to next tick */
  fts_dsp_time += dsp_tick_duration;

  /* set scheduler time (cheating a little for the alarms set in the dsp code) */
  fts_sched_set_time( fts_dsp_get_time());

  /* run DSP chain (or idle) */
  if (fts_dsp_graph_is_compiled(&main_dsp_graph))
    fts_dsp_graph_run(&main_dsp_graph);
  else
    fts_audio_idle();
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
  dsp_tick_duration_minus_one_sample = (double)(dsp_tick_size - 1) * 1000.0 / dsp_sample_rate;
  
  fts_dsp_graph_reset(&main_dsp_graph);
  fts_dsp_graph_init(&main_dsp_graph, dsp_tick_size, dsp_sample_rate);

  /* reset audio ports !!! */

  /* redefine global system variable $SampleRate (todo!) */
}

static void 
dsp_set_tick_size(void *listener, fts_symbol_t name, const fts_atom_t *value)
{
  if (fts_is_number(value))
    {
      float n = fts_get_number_int(value);

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

double
fts_dsp_get_time(void)
{
  return fts_dsp_time;
}

void 
fts_dsp_declare_function(fts_symbol_t name, void (*w)(fts_word_t *))
{
  ftl_declare_function( name, w);
}

static void sig_dummy( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
}

void 
fts_dsp_declare_inlet(fts_class_t *cl, int num)
{
  fts_method_define(cl, num, fts_s_sig, sig_dummy, 0, 0);
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
 * Initialization and shutdown
 *
 */

void fts_kernel_dsp_init(void)
{
  fts_s_dsp_on = fts_new_symbol("dsp_on");
  fts_s_sample_rate = fts_new_symbol("sample_rate");

  /* create main DSP graph */
  fts_dsp_graph_init(&main_dsp_graph, dsp_tick_size, dsp_sample_rate);

  /* init sample rate */
  dsp_sample_rate = FTS_DSP_DEFAULT_SAMPLE_RATE;
  dsp_tick_size = FTS_DSP_DEFAULT_TICK_SIZE;

  dsp_tick_duration = (double)dsp_tick_size * 1000.0 / 44100.0;
  dsp_tick_duration_minus_one_sample = (double)(FTS_DSP_DEFAULT_TICK_SIZE - 1) * 1000.0 / 44100.0;

  /* init DSP time */
  fts_dsp_time = 0.0;

  fts_param_add_listener(fts_s_sample_rate, 0, dsp_set_sample_rate);
  fts_param_add_listener(fts_s_dsp_on, 0, dsp_set_on);
}

void fts_kernel_dsp_shutdown(void)
{
  fts_param_set_int(fts_s_dsp_on, 0);
}


