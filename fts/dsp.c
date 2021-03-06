/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#include <fts/fts.h>
#include <ftsprivate/class.h>
#include <ftsprivate/patcherobject.h>
#include <ftsprivate/object.h>
#include <ftsprivate/connection.h>
#include <ftsprivate/sigconn.h>
#include <ftsprivate/dspgraph.h>
#include <ftsprivate/audio.h>
#include <ftsprivate/timebase.h>

#define FTS_DSP_DEFAULT_SAMPLE_RATE 44100

#define FTS_DSP_MIN_TICK_SIZE 8
#define FTS_DSP_MAX_TICK_SIZE 512
#define FTS_DSP_DEFAULT_TICK_SIZE 64

static fts_dsp_graph_t main_dsp_graph;

/* main DSP graph parameters */
static double dsp_sample_rate;
static int dsp_tick_size;
static double dsp_tick_duration;

static double dsp_time = 0.0;

static fts_symbol_t dsp_zero_fun_symbol = 0;
static fts_symbol_t dsp_copy_fun_symbol = 0;

static fts_timebase_t *dsp_timebase;
static fts_param_t *dsp_active_param = 0;
static fts_param_t* dsp_sample_rate_param = 0;
static fts_param_t* dsp_buffer_size_param = 0;

fts_class_t *fts_dsp_edge_class = 0;
fts_class_t *fts_dsp_signal_class = 0;

static void 
fts_dsp_default_method(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
}


/*********************************************************
 *
 *  DSP timebase
 *
 */

void
fts_dsp_run_tick( void)
{
  fts_timebase_advance( dsp_timebase);

  /* run DSP chain (or idle) */
  if (fts_dsp_graph_is_compiled( &main_dsp_graph))
    fts_dsp_graph_run( &main_dsp_graph);
}

static void
dsp_timebase_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{ 
  fts_timebase_t *self = (fts_timebase_t *)o;

  fts_timebase_init( self);
  fts_timebase_set_step( self, dsp_tick_duration);
}

static void
dsp_timebase_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{ 
  fts_timebase_t *self = (fts_timebase_t *)o;

  fts_timebase_reset( self);
}

static void
dsp_timebase_instantiate( fts_class_t *cl)
{
  fts_class_init( cl, sizeof(fts_timebase_t), dsp_timebase_init, dsp_timebase_delete);
}

/**************************************************************************
 *
 *  general DSP API functions
 *
 */

void
fts_dsp_restart( void)
{
  if( fts_dsp_graph_is_compiled( &main_dsp_graph))
    {
      fts_dsp_graph_reset( &main_dsp_graph);      
      fts_dsp_graph_compile( &main_dsp_graph);
    }
}

int
fts_dsp_is_running( void)
{
  return fts_dsp_graph_is_compiled( &main_dsp_graph);
}

int
fts_dsp_get_tick_size( void)
{
  return fts_dsp_graph_get_tick_size( &main_dsp_graph);
}

/***************************************************
 * DSP sample rate support
 */
double
fts_dsp_set_sample_rate( double dsp_sample_rate)
{
  fts_param_set_float( dsp_sample_rate_param, dsp_sample_rate);
  return fts_dsp_graph_set_sample_rate( &main_dsp_graph, dsp_sample_rate);
}

double
fts_dsp_get_sample_rate(void)
{
  fts_atom_t* value = fts_param_get_value( dsp_sample_rate_param);
  
  return fts_dsp_graph_get_sample_rate( &main_dsp_graph);
}

void
fts_dsp_sample_rate_add_listener( fts_object_t* object, fts_method_t method)
{
  fts_param_add_listener( dsp_sample_rate_param, object, method);
}

void
fts_dsp_sample_rate_remove_listener( fts_object_t* object)
{
  fts_param_remove_listener( dsp_sample_rate_param, object);
}



/***************************************************
 * Audio configuration buffer size change support
 */
void
fts_dsp_set_buffer_size(int dsp_buffer_size)
{
  fts_param_set_int( dsp_buffer_size_param, dsp_buffer_size);
}

int
fts_dsp_get_buffer_size()
{
  fts_atom_t* value = fts_param_get_value( dsp_buffer_size_param);
  return fts_get_int(value);
}

void
fts_dsp_buffer_size_add_listener( fts_object_t* object, fts_method_t method)
{
  fts_param_add_listener( dsp_buffer_size_param, object, method);
}

void
fts_dsp_buffer_size_remove_listener( fts_object_t* object)
{
  fts_param_remove_listener( dsp_buffer_size_param, object);
}

/***************************************************
 *
 */
double
fts_dsp_get_time()
{
  return fts_timebase_get_tick_time( dsp_timebase);
}

void 
fts_dsp_declare_function( fts_symbol_t name, void (*w)(fts_word_t *))
{
  ftl_declare_function( name, w);
}

void 
fts_dsp_declare_inlet( fts_class_t *cl, int num)
{
  fts_class_inlet( cl, num, fts_dsp_signal_class, fts_dsp_default_method);
}

void 
fts_dsp_declare_outlet( fts_class_t *cl, int num)
{
  fts_class_outlet( cl, num, fts_dsp_signal_class);
}

static int
dsp_object_get_n_inlets( fts_dsp_object_t *obj)
{
  fts_object_t *o = (fts_object_t *)obj;
  fts_class_t *cl = fts_object_get_class( o);
  int i;

  for( i=0; i<fts_object_get_inlets_number( o); i++)
    {
      if( fts_class_get_inlet_method( cl, i, fts_dsp_signal_class) == NULL)
	break;
    }

  return i;
}

static int
dsp_object_get_n_outlets( fts_dsp_object_t *obj)
{
  fts_object_t *o = (fts_object_t *)obj;
  fts_class_t *cl = fts_object_get_class( o);
  int i;

  for( i=0; i<fts_object_get_outlets_number( o); i++)
    {
      if( !fts_class_outlet_has_type( cl, i, fts_dsp_signal_class))
	break;
    }

  return i;
}

void
fts_dsp_object_init( fts_dsp_object_t *obj)
{
  fts_class_t *cl = fts_object_get_class( (fts_object_t *)obj);
  int ninputs, noutputs;

  fts_object_get_patcher_data( (fts_object_t *)obj);

  ninputs = dsp_object_get_n_inlets( obj);
  noutputs = dsp_object_get_n_outlets( obj);

  /* make sure that class has a put method */
  if( fts_class_get_method_varargs( cl, fts_s_put) == NULL)
    fts_class_message_varargs( cl, fts_s_put, fts_dsp_default_method);

  obj->descr.ninputs = ninputs;
  obj->descr.noutputs = noutputs;
  
  if( obj->descr.ninputs > 0)
    obj->descr.in = (fts_dsp_signal_t **)fts_zalloc( sizeof( fts_dsp_signal_t *) * ninputs);
  
  if( obj->descr.noutputs > 0)
    obj->descr.out = (fts_dsp_signal_t **)fts_zalloc( sizeof(fts_dsp_signal_t *) * noutputs);

  fts_dsp_graph_add_object( &main_dsp_graph, obj);
}

void 
fts_dsp_object_delete( fts_dsp_object_t *obj)
{
  fts_dsp_graph_remove_object( &main_dsp_graph, obj);

  fts_free( obj->descr.in);
  fts_free( obj->descr.out);  
}

int
fts_is_dsp_object( fts_object_t *o)
{
  return (fts_class_get_method_varargs( fts_object_get_class( o), fts_s_put) != NULL);
}

void 
fts_dsp_add_function( fts_symbol_t symb, int ac, fts_atom_t *av)
{
  ftl_program_add_call( main_dsp_graph.chain, symb, ac, av);
}

int 
fts_dsp_is_sig_inlet( fts_object_t *o, int num)
{
  return fts_class_get_inlet_method( fts_object_get_class( o), num, fts_dsp_signal_class) != NULL;
}

int 
fts_dsp_is_input_null( fts_dsp_descr_t *descr, int in)
{
  return descr->in[in] == sig_zero;
}

fts_object_t *
dsp_get_current_object( void )
{
  if( main_dsp_graph.chain)
    return ftl_program_get_current_object( main_dsp_graph.chain);
  else
    return 0;
}

void 
dsp_add_signal( fts_symbol_t name, int size)
{
  ftl_program_add_signal( main_dsp_graph.chain, name, size);
}

void 
dsp_chain_post( void)
{
  fts_post( "printing dsp chain:\n");
  ftl_program_post( main_dsp_graph.chain);
}

void 
dsp_chain_post_signals( void)
{
  fts_post( "printing signals:\n");
  ftl_program_post_signals_count( main_dsp_graph.chain);
}

void 
dsp_chain_fprint( FILE *f)
{
  fprintf( f, "printing dsp chain:\n");
  ftl_program_fprint( f, main_dsp_graph.chain);
}

void 
dsp_chain_fprint_signals( FILE *f)
{
  fprintf( f, "printing signals:\n");
  ftl_program_fprint_signals_count( f, main_dsp_graph.chain);
}

ftl_program_t *
dsp_get_current_dsp_chain( void)
{
  return main_dsp_graph.chain;
}

/**************************************************************************
 *
 *  DSP edge
 *
 *    The DSP edge is a dummy object to assure the order of input and outputs
 *    of objects such as busses and delays. All inputs are scheduled BEFORE the
 *    edge, all outputs AFTER. See fts_dsp_after_edge() and fts_dsp_before_edge(). 
 *
 */

static void
dsp_edge_put(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{ 
  fts_dsp_edge_t *self = (fts_dsp_edge_t *)o;
  fts_dsp_descr_t* dsp = (fts_dsp_descr_t *)fts_get_pointer( at);

  self->n_tick = fts_dsp_get_input_size( dsp, 0);
  self->sr = fts_dsp_get_input_srate( dsp, 0);
}

static void
dsp_edge_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{ 
  fts_dsp_edge_t *self = (fts_dsp_edge_t *)o;

  self->n_tick = fts_dsp_get_tick_size();
  self->sr = fts_dsp_get_sample_rate();

  fts_dsp_object_init( (fts_dsp_object_t *)o);
}

static void
dsp_edge_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{ 
  fts_dsp_object_delete( (fts_dsp_object_t *)o);
}

static void
dsp_edge_instantiate( fts_class_t *cl)
{
  fts_class_init( cl, sizeof( fts_dsp_edge_t), dsp_edge_init, dsp_edge_delete);

  fts_class_message_varargs(cl, fts_s_name, fts_object_name);
  fts_class_message_varargs(cl, fts_s_put, dsp_edge_put);

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0);
}

void
fts_dsp_after_edge(fts_object_t *o, fts_dsp_edge_t *edge)
{
  /* create hidden order forcing connection */
  fts_connection_new((fts_object_t *)edge, 0, o, 0, fts_c_order_forcing);
}

void
fts_dsp_before_edge(fts_object_t *o, fts_dsp_edge_t *edge)
{
  /* create hidden order forcing connection */
  fts_connection_new(o, 0, (fts_object_t *)edge, 0, fts_c_order_forcing);
}

/**************************************************************************
 *
 *  kernel ftl functions
 *
 */

/* DSP signal dummy class */
static void
dsp_signal_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_object_t), 0, 0);
}

static void
dsp_zero_fun(fts_word_t *argv)
{
  float *out = (float *)fts_word_get_pointer(argv + 0);
  int n = fts_word_get_int(argv + 1);
  int i;

  for(i=0; i<n; i++)
    out[i] = 0.0;
}

static void
dsp_copy_fun(fts_word_t *argv)
{
  float *in = (float *)fts_word_get_pointer(argv + 0);
  float *out = (float *)fts_word_get_pointer(argv + 1);
  int n = fts_word_get_int(argv + 2);
  int i;

  for(i=0; i<n; i++)
    out[i] = in[i];
}

static void
dsp_fill_fun(fts_word_t *argv)
{
  float c = *((float *)fts_word_get_pointer(argv + 0));
  float *out = (float *)fts_word_get_pointer(argv + 1);
  int n = fts_word_get_int(argv + 2);
  int i;

  for(i=0; i<n; i++)
    out[i] = c;
}

void
fts_dsp_add_function_zero(fts_symbol_t out, int size)
{
  fts_atom_t a[2];

  fts_set_symbol(a + 0, out);
  fts_set_int(a + 1, size);
  fts_dsp_add_function(dsp_zero_fun_symbol, 2, a);
}

void
fts_dsp_add_function_fill(ftl_data_t value, fts_symbol_t out, int size)
{
  fts_atom_t a[3];

  fts_set_ftl_data(a + 0, value);
  fts_set_symbol(a + 1, out);
  fts_set_int(a + 2, size);
  fts_dsp_add_function(dsp_copy_fun_symbol, 3, a);
}

void
fts_dsp_add_function_copy(fts_symbol_t in, fts_symbol_t out, int size)
{
  fts_atom_t a[3];

  fts_set_symbol(a + 0, in);
  fts_set_symbol(a + 1, out);
  fts_set_int(a + 2, size);
  fts_dsp_add_function(dsp_copy_fun_symbol, 3, a);
}

int
fts_dsp_is_active(void)
{
  if(dsp_active_param != NULL)
    {
      fts_atom_t *value = fts_param_get_value(dsp_active_param);
      
      if(fts_is_number(value) && fts_get_number_int(value) != 0)
	return 1;
    }
  
  return 0;
}

void
fts_dsp_activate(void)
{
  fts_param_set_int(dsp_active_param, 1);
}

void
fts_dsp_desactivate(void)
{
  fts_param_set_int(dsp_active_param, 0);
}

void
fts_dsp_active_add_listener(fts_object_t *object, fts_method_t method)
{
  fts_param_add_listener(dsp_active_param, object, method);
}

void
fts_dsp_active_remove_listener(fts_object_t *object)
{
  fts_param_remove_listener(dsp_active_param, object);
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
  
  if (fts_dsp_graph_is_compiled(&main_dsp_graph))
  {
    fts_dsp_graph_reset(&main_dsp_graph);
  }
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
dsp_set_sample_rate(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  if (fts_is_number(at))
  {
    float sr = (float)fts_get_number_float(at);
    
    dsp_sample_rate = sr;
    dsp_reset();
  }
  if(fts_dsp_is_active())
    fts_dsp_graph_compile(&main_dsp_graph);
}


static void 
dsp_active(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  int active = 0;

  if(fts_is_number(at))
    active = fts_get_number_int(at);

  if(active)
    fts_dsp_graph_compile(&main_dsp_graph);
  else if(fts_dsp_graph_is_compiled(&main_dsp_graph))
    fts_dsp_graph_reset(&main_dsp_graph);
}

/**************************************************************************
 *
 * Initialization and shutdown
 *
 */

FTS_MODULE_INIT(dsp)
{
  fts_class_t *dsp_timebase_class = 0;

  /* init sample rate */
  dsp_sample_rate = FTS_DSP_DEFAULT_SAMPLE_RATE;
  dsp_tick_size = FTS_DSP_DEFAULT_TICK_SIZE;
  dsp_tick_duration = 1000.0 * (double)FTS_DSP_DEFAULT_TICK_SIZE / FTS_DSP_DEFAULT_SAMPLE_RATE;

  /* create dsp timebase in root patcher (will be cleaned up with root patcher) */
  dsp_timebase_class = fts_class_install(NULL, dsp_timebase_instantiate);
  dsp_timebase = (fts_timebase_t *)fts_object_create(dsp_timebase_class, 0, 0);
  fts_set_timebase( dsp_timebase);

  /* DSP edge class */
  fts_dsp_edge_class = fts_class_install(fts_new_symbol("edge~"), dsp_edge_instantiate);

  /* DSP signal dummy class */
  fts_dsp_signal_class = fts_class_install(NULL, dsp_signal_instantiate);

  /* create DSP parameter */
  dsp_active_param = (fts_param_t *)fts_object_create(fts_param_class, 0, 0);
  fts_param_add_listener(dsp_active_param, NULL, dsp_active);

  dsp_sample_rate_param = (fts_param_t*)fts_object_create(fts_param_class, 0, 0);
  fts_param_add_listener(dsp_sample_rate_param, NULL, dsp_set_sample_rate);

  dsp_buffer_size_param = (fts_param_t*)fts_object_create(fts_param_class, 0, 0);

  /* create main DSP graph */
  fts_dsp_graph_init(&main_dsp_graph, dsp_tick_size, dsp_sample_rate);

  dsp_zero_fun_symbol = fts_new_symbol("dsp_zero_fun_symbol");
  fts_dsp_declare_function( dsp_zero_fun_symbol, dsp_zero_fun);
  
  dsp_copy_fun_symbol = fts_new_symbol("dsp_copy_fun_symbol");
  fts_dsp_declare_function( dsp_copy_fun_symbol, dsp_copy_fun);
}

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
