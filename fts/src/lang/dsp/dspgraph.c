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
#include <string.h>

#include "sys.h"
#include "lang/mess.h"
#include "lang/ftl.h"
#include "lang/dsp.h"
#include "gphiter.h"
/* (fd) For post */
#include "runtime/files.h"


extern void fts_audio_add_unused_zero_fun(void);
extern void fts_audio_activate_devices(void);
extern void fts_audio_deactivate_devices(void);
extern void fts_audio_add_all_zero_fun(void);
extern float fts_sched_get_tick_length(void);

#define ASSERT(e) if (!(e)) { fprintf( stderr, "Assertion (%s) failed file %s line %d\n",#e,__FILE__,__LINE__); *(char *)0 = 0;}

/* 
   Time must be zeroed in the same place it is incremented, 
   otherwise it will never be zero in fts_alarm_sched.
*/
static int  please_zero_dsp_time = 0;

/* 
   Clocks for use with timer primitives.
   We provide (see the dsp_timer_install function below), two timers
   (dsp_msec and dsp_tick) that measure the elapsed time after the dac start.
   Their values is zero after a delete 
*/
static double dsp_ms_clock = 0;
static double dsp_tick_clock = 0;


/* Heaps for graph and descriptors */

static fts_heap_t *dsp_graph_heap;
static fts_heap_t *dsp_descr_heap;

/* For switch hack, this variable is temporaly made global */
ftl_program_t *dsp_chain_on = 0;
static ftl_program_t *dsp_chain_off = 0;
static ftl_program_t *dsp_chain = 0;

static dsp_signal *sig_zero;

static int verbose = 0;
static int depth = 0;


/* --------------------------------------------------------------------------- */
/*                                                                             */
/* The dsp_node structure                                                      */
/*                                                                             */
/* --------------------------------------------------------------------------- */

typedef struct dsp_node_t dsp_node_t;

struct dsp_node_t {
  fts_object_t *o;
  int pred_cnt;
  fts_dsp_descr_t *descr;
  dsp_node_t *next;
};

#define SCHEDULED -1
#define SET_SCHEDULED( node) ((node)->pred_cnt = SCHEDULED)
#define IS_SCHEDULED( node)  ( (node)->pred_cnt == SCHEDULED)

static dsp_node_t *dsp_graph;


typedef void (*edge_fun_t)(dsp_node_t *src, int woutlet, dsp_node_t *dest, int winlet, dsp_signal *sig); 

/* Generic graph traversal function */
static void dsp_succ_realize( dsp_node_t *node, edge_fun_t fun);
/* Edge function used to count predecessors */
static void inc(dsp_node_t *src, int woutlet, dsp_node_t *dest, int winlet, dsp_signal *sig);
/* Edge function used to decrement predecessors count and increment reference count of signals */
static void dec_pred_inc_refcnt(dsp_node_t *src, int woutlet, dsp_node_t *dest, int winlet, dsp_signal *sig);
/* Edge function used to realize a depth first sequentialization of the graph */
static void dsp_schedule_depth(dsp_node_t *src, int woutlet, dsp_node_t *dest, int winlet, dsp_signal *sig);

/* --------------------------------------------------------------------------- */
/*                                                                             */
/* Prints the objects with full patcher hierarchy                              */
/* Used for verbose compilation and error signaling                            */
/*                                                                             */
/* --------------------------------------------------------------------------- */

static void cat_sig_name( char *s, dsp_signal *sig)
{
  char tmp[64];

  if (sig)
    {
      strcat( s, fts_symbol_name( sig->name));
      sprintf( tmp, "[%d]", sig->length);
      strcat( s, tmp);
    }
  else
    strcat( s, "nil");
}

static void append_sigs( char *s, dsp_signal **sig, int n)
{
  int i;

  s[0] = '\0';

  if (n > 0)
    cat_sig_name( s, sig[0]);

  for ( i = 1; i < n; i++)
    {
      strcat( s, ",");
      cat_sig_name( s, sig[i]);
    }
}

static char *inputs_name( fts_dsp_descr_t *descr)
{
  static char buffer[256];

  append_sigs( buffer, descr->in, descr->ninputs);

  return buffer;
}

static char *outputs_name( fts_dsp_descr_t *descr)
{
  static char buffer[256];

  append_sigs( buffer, descr->out, descr->noutputs);

  return buffer;
}


/* --------------------------------------------------------------------------- */
/*                                                                             */
/* Auxiliary functions                                                         */
/*                                                                             */
/* --------------------------------------------------------------------------- */

static int dsp_input_get(fts_object_t *obj, int winlet)
{
  int i, n;

  for (i = 0, n = 0; i < winlet; i++)
    if (fts_object_handle_message(obj, i, fts_s_sig))
      n++;

  return n;
}

static int dsp_output_get(fts_class_t *cl, int woutlet)
{
  int i, n;
  fts_outlet_decl_t *out;

  for (i = 0, n = 0, out = cl->outlets; i < woutlet; i++, out++)
    if (out->tmess.symb == fts_s_sig || !out->tmess.symb)
      n++;

  return n;
}

static dsp_node_t *dsp_list_lookup(fts_object_t *o)
{
  fts_atom_t a;

  _fts_object_get_prop(o, fts_s_dsp_descr, &a);

  if (! fts_is_void(&a))
    return (dsp_node_t *) fts_get_ptr(&a);
  else
    return 0;
}


/* --------------------------------------------------------------------------- */
/*                                                                             */
/* Object scheduling functions                                                 */
/*                                                                             */
/* --------------------------------------------------------------------------- */

static int dsp_gen_outputs(fts_object_t *o, fts_dsp_descr_t *descr)
{
  int i, invs = -1;
  dsp_signal **iop;
  fts_atom_t a;
  int size;

  /*
    Compute the size using object properties, starting from the input
    size (or default VS if not present), and using the
    upsampling/downsampling object property to compute the actual size.
    If both are present, we take down.
    */
  for (i = 0, iop = descr->in; i < descr->ninputs; i++, iop++)
    if (*iop != sig_zero)
      {
	if (invs < 0) 
	  invs = (*iop)->length;
	else if (invs != (*iop)->length)
	  {
	    post("DSP [%d] inputs don't match for object ", depth);
	    post_object( o);
	    post("\n");
	    return 0;
	  }
      }

  if (invs < 0) 
    invs = DEFAULTVS;

  fts_object_get_prop(o, fts_s_dsp_downsampling, &a);

  if (! fts_is_void(&a))
    size = invs >> fts_get_long(&a);
  else
    {
      fts_object_get_prop(o, fts_s_dsp_upsampling, &a);
    
      if (! fts_is_void(&a))
        size = invs << fts_get_long(&a);
      else
        {
          fts_object_get_prop(o, fts_s_dsp_outputsize, &a);

          if (! fts_is_void(&a))
	    size = fts_get_long(&a);
          else
	    size = invs;
        }
    }

  /* Output signals are assigned only when the output have at least
     one connection to a dsp object.
   */
  for (i = 0, iop = descr->out; i< descr->noutputs; i++, iop++)
    {
      /* TO BE CHANGED USING Inlet and outlet properties !!! */
      *iop = Sig_new(size);
    }

  sig_zero->length = invs;

  sig_zero->srate = fts_param_get_float(fts_s_sampling_rate, 44100.) / ((double)DEFAULTVS/(double)size);

  return 1;
}

static void dsp_object_schedule(dsp_node_t *node)
{
  dsp_signal **sig;
  fts_atom_t a;
  int i;

  if (! node->descr)
    {
      node->descr = (fts_dsp_descr_t *)fts_heap_zalloc(dsp_descr_heap);
      node->descr->ninputs  = dsp_input_get(node->o, fts_object_get_inlets_number(node->o));
      if ( node->descr->ninputs)
	node->descr->in = (dsp_signal **)fts_block_zalloc(sizeof(dsp_signal *) * node->descr->ninputs);
      node->descr->noutputs = dsp_output_get(node->o->cl, fts_object_get_outlets_number(node->o));
    }

  /* For IRIX 6.2 MipsPro 7.x, we don't reuse signals between in and
     outs, but we guarantee that all the arguments point to different
     buffers, so that we can use advanced optimization without fearing
     pointer aliasing.
   */

  for (i = 0, sig = node->descr->in; i < node->descr->ninputs; i++, sig++)
    if (*sig)
      {
	if (*sig != sig_zero)
	  Sig_unreference(*sig);
      }
    else
      *sig = sig_zero;

  if (node->descr->noutputs)
    node->descr->out = (dsp_signal **)fts_block_zalloc(sizeof(dsp_signal *) * node->descr->noutputs);

  /* Now that dsp_gen_outputs compute the downsampling using object
     properties, we don't need to call it in the object put method; it
     become a private function, and is not part of the API !!
     */
  if ( dsp_gen_outputs(node->o, node->descr))
    {
      if ( verbose)
	{
	  post( "DSP [%d] scheduling ", depth);
	  post_object( node->o);
	  post( "%s->%s\n", inputs_name( node->descr), outputs_name( node->descr));
	}

      /*
	(fd) Hack to add debugging info to the FTL program.
	We keep the object that was send the message "put" and
	we pass it to ftl_..._add_call so that it stored in the
	debugging info of the DSP chain
	*/
      fts_set_ptr(&a, node->descr);
      fts_message_send(node->o, fts_SystemInlet, fts_s_put, 1, &a);

      {
	ftl_instruction_info_t *info;

	info = ftl_program_get_current_instruction_info( dsp_chain_on);
	if (info)
	  {
	      int i;

	    ftl_instruction_info_set_object( info, node->o);

	    ftl_instruction_info_set_ninputs( info, node->descr->ninputs);
	    for ( i = 0; i < node->descr->ninputs; i++)
		ftl_instruction_info_set_input( info, i, fts_dsp_get_input_name( node->descr, i), fts_dsp_get_input_size( node->descr,i));

	    ftl_instruction_info_set_noutputs( info, node->descr->noutputs);
	    for ( i = 0; i < node->descr->noutputs; i++)
		ftl_instruction_info_set_output( info, i, fts_dsp_get_output_name( node->descr, i), fts_dsp_get_output_size( node->descr,i));
	  }
      }
    }

  SET_SCHEDULED( node);

  dsp_succ_realize(node, dec_pred_inc_refcnt);

  /* This is to free the unreferenced signals, after having computed
     the reference count of all the outputs signals. This happens in case
     of dsp outlets which are not connected
     */
  for (i = 0, sig = node->descr->out; i < node->descr->noutputs; i++, sig++)
    if ( *sig)
      if ( (*sig)->refcnt == 0)
	Sig_free( *sig);

  if (node->descr->ninputs)
    fts_block_free((char *) node->descr->in, sizeof(dsp_signal *) * node->descr->ninputs);

  if (node->descr->noutputs)
    fts_block_free((char *) node->descr->out, sizeof(dsp_signal *) * node->descr->noutputs);

  fts_heap_free((char *)node->descr, dsp_descr_heap);
  node->descr = 0;
}

/* --------------------------------------------------------------------------- */
/*                                                                             */
/* Graph traversal functions                                                   */
/*                                                                             */
/* --------------------------------------------------------------------------- */

/* Generic graph traversal function */
static void dsp_succ_realize( dsp_node_t *node, edge_fun_t fun)
{
  fts_outlet_decl_t *outlet;
  int woutlet;
  dsp_signal **sig;
  static void *zero = 0;

  if ( node->descr)
    sig = node->descr->out;
  else
    sig = (dsp_signal **)(&zero);

  outlet = node->o->cl->outlets;

  for (woutlet = 0; woutlet < fts_object_get_outlets_number(node->o); woutlet++)
    {
      if (outlet->tmess.symb == fts_s_sig || !outlet->tmess.symb)
	{
	  graph_iterator_t iter;

	  graph_iterator_init( &iter, node->o, woutlet);

	  while ( !graph_iterator_end( &iter))
	    {
	      fts_object_t *succ_obj;
	      dsp_node_t *succ_node;
	      int winlet;

	      graph_iterator_get_current( &iter, &succ_obj, &winlet);

	      succ_node = dsp_list_lookup( succ_obj);

	      if (succ_node)
		(*fun)( node, woutlet, succ_node, winlet, *sig);
	  
	      graph_iterator_next( &iter);
	    }

	  sig++;
	}

      outlet++;
    }
}

/* Edge function used to count predecessors */
static void inc(dsp_node_t *src, int woutlet, dsp_node_t *dest, int winlet, dsp_signal *sig)
{
  ASSERT( dest != 0);

  dest->pred_cnt++;
}

/* Edge function used to decrement predecessors count and increment reference count of signals */
static void dec_pred_inc_refcnt(dsp_node_t *src, int woutlet, dsp_node_t *dest, int winlet, dsp_signal *sig)
{
  int ninputs;

  ASSERT( dest != 0);

  dest->pred_cnt--;

  ninputs = dsp_input_get(dest->o, fts_object_get_inlets_number(dest->o));

  if (! dest->descr)
    {
      dest->descr = (fts_dsp_descr_t *)fts_heap_zalloc(dsp_descr_heap);
      dest->descr->ninputs = ninputs;
      dest->descr->noutputs = dsp_output_get(dest->o->cl, fts_object_get_outlets_number(dest->o));
    }

  if (ninputs)
    {
      int nin;
      dsp_signal *previous_sig;

#if 0
      if (! dest->descr->in)
	dest->descr->in = (dsp_signal **)fts_block_zalloc(sizeof(dsp_signal *) * ninputs);
#else
      if (! dest->descr->in)
	/* (fd) to avoid writing past the end of the dsp_descr... */
	dest->descr->in = (dsp_signal **)fts_block_zalloc(sizeof(dsp_signal *) * fts_object_get_inlets_number(dest->o));
#endif

      nin = dsp_input_get(dest->o, winlet);

#if 0
      if (nin >= ninputs)
	{
	  return;
	}
#endif

      previous_sig = dest->descr->in[nin];
      if ((! previous_sig) || (previous_sig == sig_zero))
	{
	  if (sig)
	    Sig_reference(sig);
	  dest->descr->in[nin] = sig;
	}
      else
	{
#if DSP_MULTIPLE_CONN_WARN
	  post("Multiple signal connections between outlet %d of object %s and inlet %d of object %s (connection ignored)\n",
		woutlet, fts_symbol_name(fts_object_get_class_name(src->o)),
		winlet, fts_symbol_name(fts_object_get_class_name(dest->o)));
#endif
	}
    }
}

/* Edge function used to realize a depth first sequentialization of the graph */
static void dsp_schedule_depth(dsp_node_t *src, int woutlet, dsp_node_t *dest, int winlet, dsp_signal *sig)
{
  ASSERT( dest != 0);

  if ( dest->pred_cnt == 0)
    {
      dsp_object_schedule( dest);

      depth++;
      dsp_succ_realize( dest, dsp_schedule_depth);
      depth--;

      fts_message_send( dest->o, fts_SystemInlet, fts_new_symbol("put_after_successors"), 0, 0);
    }
}


/* --------------------------------------------------------------------------- */
/*                                                                             */
/* DSP chain creation functions                                                */
/*                                                                             */
/* --------------------------------------------------------------------------- */

static void dsp_graph_reinit( dsp_node_t *graph)
{
  dsp_node_t *node;

  for( node = graph; node; node = node->next)
    node->pred_cnt = 0;
}

static void dsp_pred_count( dsp_node_t *graph)
{
  dsp_node_t *node;

  for( node = graph; node; node = node->next)
    dsp_succ_realize( node, inc);
}

static void dsp_chain_create_start(int vs)
{
  /* ask ftl to start memory relocation */
  ftl_mem_start_memory_relocation();/* should it be elsewhere ?? */

  /* ask to  zero the dsp timers next dsp_chain_poll cycle */
  please_zero_dsp_time = 1;

  if ( !dsp_chain_on)
    dsp_chain_on = ftl_program_new();
  else
    ftl_program_init( dsp_chain_on);

  ftl_program_set_current_subroutine( dsp_chain_on, ftl_program_add_main( dsp_chain_on));

  Sig_setup(vs);

  sig_zero = Sig_new(DEFAULTVS);

  dsp_graph_reinit( dsp_graph);

  dsp_pred_count( dsp_graph);
}

static void dsp_graph_schedule( void)
{
  dsp_node_t *node;

  depth = 0;

  /* schedule all nodes without predecessors */
  for( node = dsp_graph; node; node = node->next)
    if ( node->pred_cnt == 0)
      dsp_schedule_depth( 0, 0, node, 0, 0);
}

static void dsp_graph_check_loop( void)
{
  dsp_node_t *node;

  for( node = dsp_graph; node; node = node->next)
    if ( !IS_SCHEDULED(node))
      {
	post("Loop in dsp graph: object ");
	post_object( node->o);
	post(" not scheduled\n");
      }
}

static void dsp_chain_create_end( void)
{
  ftl_program_add_return( dsp_chain_on);

  ftl_mem_end_memory_relocation();
  
  ftl_program_compile(dsp_chain_on);

  dsp_chain = dsp_chain_on;

  fts_audio_activate_devices();
}

void dsp_chain_create(int vs)
{
  if (dsp_is_running())
    {
      post( "DSP is running, cannot create DSP chain\n");
      return;
    }

  dsp_chain_create_start(vs);

  dsp_graph_schedule();

  /* Add a dac syncronization/zero call for each out device not used by an object */
  fts_audio_add_unused_zero_fun();

  /* Looks for loop */
  dsp_graph_check_loop();

  dsp_chain_create_end();
}

void dsp_chain_delete(void)
{
  if ( !dsp_is_running())
    return;

  if ( dsp_chain_on)
    ftl_program_destroy( dsp_chain_on);

  dsp_chain = dsp_chain_off;

  fts_audio_deactivate_devices();
}

/* DSP ON parameter listener */

static void dsp_on_listener(void *listener, fts_symbol_t name,  const fts_atom_t *value)
{
  if (fts_is_int(value))
    {
      int on;
      on = fts_get_int(value);

      if (on)
	dsp_chain_create(fts_param_get_float(fts_s_vector_size, DEFAULTVS));
      else
	dsp_chain_delete();
    }
}


fts_object_t *dsp_get_current_object()
{
  if (dsp_chain_on)
    return ftl_program_get_current_object(dsp_chain_on);
  else
    return 0;
}

/* exported, must be redone each time a new out device is installed */
void dsp_make_dsp_off_chain(void)
{
  ftl_program_t *tmp;

  if (dsp_chain_off)
    {
      /* delete the old off dsp chain */

      ftl_program_destroy( dsp_chain_off);
    }

  tmp = dsp_chain_on;

  dsp_chain_on = ftl_program_new();

  ftl_program_set_current_subroutine( dsp_chain_on, ftl_program_add_main( dsp_chain_on));

  Sig_setup(DEFAULTVS);

  sig_zero = Sig_new(DEFAULTVS);

  /* add the zero calls for all the installed out device */

  fts_audio_add_all_zero_fun();
  ftl_program_add_return( dsp_chain_on);
  ftl_program_compile(dsp_chain_on);

  dsp_chain_off = dsp_chain_on;
  dsp_chain_on = tmp;
  dsp_chain = dsp_chain_off;
}


/* --------------------------------------------------------------------------- */
/*                                                                             */
/* DSP API functions                                                           */
/* Most are just wrapper around FTL API, to keep dsp_chain_on local            */
/*                                                                             */
/* --------------------------------------------------------------------------- */

void dsp_declare_function(fts_symbol_t name, void (*w)(fts_word_t *))
{
  ftl_declare_function( name, w);
}

void dsp_sig_inlet(fts_class_t *cl, int num)
{
  fts_method_define(cl, num, fts_s_sig, 0, 0, 0);
}

void dsp_sig_outlet(fts_class_t *cl, int num)
{
  fts_outlet_type_define(cl, num, fts_s_sig, 0, 0);
}

/* insert object in graph; call from init method object */
void dsp_list_insert(fts_object_t *o)
{
  dsp_node_t *node;
  fts_atom_t v;

  node = (dsp_node_t *)fts_heap_zalloc(dsp_graph_heap);

  node->o = o;

  fts_set_ptr(&v, (void *)node);
  _fts_object_put_prop(o, fts_s_dsp_descr, &v);

  node->descr = 0;

  node->next = dsp_graph;
  dsp_graph = node;
}

/* remove object from graph; call from delet method object */
void dsp_list_remove(fts_object_t *o)
{
  dsp_node_t *node, *prev_node;

  /* We stop the dsp chain, because for sure is not more
   consistent with the object network; use the param to propagate */

  fts_param_set_int(fts_s_dsp_on, 0);

  _fts_object_remove_prop(o, fts_s_dsp_descr);

  prev_node = 0;
  for( node = dsp_graph; node; node = node->next)
    {
      if (node->o == o)
	{
	  if (prev_node)
	    prev_node->next = node->next;
	  else
	    dsp_graph = node->next;

	  if (node->descr)
	    {
	      fts_block_free((char *) node->descr->in, sizeof(dsp_signal *) * node->descr->ninputs);
	      fts_block_free((char *) node->descr->out, sizeof(dsp_signal *) * node->descr->noutputs);
	      fts_heap_free((char *)node->descr, dsp_descr_heap);
	    }

	  fts_heap_free((char *)node, dsp_graph_heap);
	  return;
	}

      prev_node = node;
    }
}

void dsp_add_signal(fts_symbol_t name, int vs)
{
  ftl_program_add_signal(dsp_chain_on, name, vs);
}

void dsp_add_funcall(fts_symbol_t symb, int ac, fts_atom_t *av)
{
  ftl_program_add_call(dsp_chain_on, symb, ac, av);
}

void dsp_chain_post(void)
{
  post("printing dsp chain:\n");
  ftl_program_post(dsp_chain);
}

void dsp_chain_post_signals(void)
{
  post("printing signals:\n");
  ftl_program_post_signals_count(dsp_chain);
}

void dsp_chain_fprint(FILE *f)
{
  fprintf(f, "printing dsp chain:\n");
  ftl_program_fprint(f, dsp_chain);
}

void dsp_chain_fprint_signals(FILE *f)
{
  fprintf(f, "printing signals:\n");
  ftl_program_fprint_signals_count(f, dsp_chain);
}


int dsp_is_running( void)
{
  return dsp_chain == dsp_chain_on;
}

ftl_program_t *dsp_get_current_dsp_chain( void)
{
  return dsp_chain;
}

/* --------------------------------------------------------------------------- */
/*                                                                             */
/* DSP clocks management                                                       */
/* DSP chain execution function                                                */
/*                                                                             */
/* --------------------------------------------------------------------------- */

extern void fts_clock_define_protected(fts_symbol_t name, double *clock);

void dsp_install_clocks(void)
{
  /* Install the timer */

  fts_clock_define_protected(fts_new_symbol("dsp_msec"),   &dsp_ms_clock);
  fts_clock_define_protected(fts_new_symbol("dsp_tick"), &dsp_tick_clock);
}

/* This function should be moved to a tile, or to the runtime */

void dsp_chain_poll(void)
{
  if (dsp_chain == dsp_chain_on)
    {
      if (please_zero_dsp_time)
	{
	  dsp_ms_clock = 0.0;
	  dsp_tick_clock = 0.0;

	  fts_clock_reset(fts_new_symbol("dsp_msec"));
	  fts_clock_reset(fts_new_symbol("dsp_tick"));

	  please_zero_dsp_time = 0;
	}
      else
	{
	  dsp_tick_clock += 1.0;
	  dsp_ms_clock = dsp_tick_clock * fts_sched_get_tick_length();
	}
    }

  ftl_program_run(dsp_chain);
}


/* --------------------------------------------------------------------------- */
/*                                                                             */
/* DSP compiler control                                                        */
/*                                                                             */
/* --------------------------------------------------------------------------- */

void dsp_compiler_init(void)
{
  dsp_graph_heap = fts_heap_new(sizeof(dsp_node_t));
  dsp_descr_heap = fts_heap_new(sizeof(fts_dsp_descr_t));

  /* Install the dsp_on parameter listener */
  
  fts_param_add_listener(fts_s_dsp_on, 0, dsp_on_listener);
}

