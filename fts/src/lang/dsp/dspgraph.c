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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include <string.h>
#include <stdlib.h>

#include <fts/sys.h>
#include <fts/lang/mess.h>
#include <fts/lang/ftl.h>
#include <fts/lang/dsp.h>
#include "signal.h"
#include "gphiter.h"
#include "sigconn.h"

#include <fts/runtime.h>

#define ASSERT(e) if (!(e)) { fprintf( stderr, "Assertion (%s) failed file %s line %d\n",#e,__FILE__,__LINE__); *(char *)0 = 0;}

/* 
 * Time must be zeroed in the same place it is incremented, 
 * otherwise it will never be zero in fts_alarm_sched.
 */
static int  please_zero_dsp_time = 0;

/* 
 * Clocks for use with timer primitives.
 * We provide (see the dsp_timer_install function below), two timers
 * (dsp_msec and dsp_tick) that measure the elapsed time after the dac start.
 * Their values is zero after a delete 
 */
static double dsp_ms_clock = 0;
static double dsp_tick_clock = 0;

/* Heaps for graph and descriptors */
static fts_heap_t *dsp_graph_heap;
static fts_heap_t *dsp_descr_heap;

static ftl_program_t *dsp_chain = 0;

static int dsp_is_running = 0;

static fts_dsp_signal_t *sig_zero;

static int verbose = 0;

static fts_signal_connection_table_t signal_connection_table;

static fts_symbol_t sym_builtin_add;

/******************************************************************************
 *
 * The dsp_node structure
 *
 */
typedef struct _dsp_node_ dsp_node_t;

struct _dsp_node_ {
  fts_object_t *o;
  int pred_cnt;
  fts_dsp_descr_t *descr;
  dsp_node_t *next;
};

#define SCHEDULED -1
#define SET_SCHEDULED( node) ((node)->pred_cnt = SCHEDULED)
#define IS_SCHEDULED( node)  ( (node)->pred_cnt == SCHEDULED)

static dsp_node_t *dsp_graph = 0;

typedef void (*edge_fun_t)(dsp_node_t *src, int woutlet, dsp_node_t *dest, int winlet, fts_dsp_signal_t *sig); 

/* Generic graph traversal function */
static void dsp_succ_realize( dsp_node_t *node, edge_fun_t fun, int mark_connections);
/* Edge function used to count predecessors */
static void inc(dsp_node_t *src, int woutlet, dsp_node_t *dest, int winlet, fts_dsp_signal_t *sig);
/* Edge function used to decrement predecessors count and increment reference count of signals */
static void dec_pred_inc_refcnt(dsp_node_t *src, int woutlet, dsp_node_t *dest, int winlet, fts_dsp_signal_t *sig);
/* Edge function used to realize a depth first sequentialization of the graph */
static void dsp_schedule_depth(dsp_node_t *src, int woutlet, dsp_node_t *dest, int winlet, fts_dsp_signal_t *sig);

/* --------------------------------------------------------------------------- */
/*                                                                             */
/* Prints the objects with full patcher hierarchy                              */
/* Used for verbose compilation and error signaling                            */
/*                                                                             */
/* --------------------------------------------------------------------------- */

static void cat_sig_name( char *s, fts_dsp_signal_t *sig)
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

static void append_sigs( char *s, fts_dsp_signal_t **sig, int n)
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
    if (fts_object_has_method(obj, i, fts_s_sig))
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

#define DSP_AUTO_COMPILE_DEFAULT 0

void
fts_dsp_auto_stop()
{
  int auto_compile = ((fts_param_get_int(fts_s_dsp_on, 0) != 0) && (fts_param_get_int(fts_s_dsp_auto_compile, DSP_AUTO_COMPILE_DEFAULT) != 0));

  if(auto_compile)
    dsp_chain_delete();
  else if(fts_param_get_int(fts_s_dsp_on, 0) != 0)
    fts_param_set_int(fts_s_dsp_on, 0);
}

void
fts_dsp_auto_restart()
{
  int auto_compile = ((fts_param_get_int(fts_s_dsp_on, 0) != 0) && (fts_param_get_int(fts_s_dsp_auto_compile, DSP_AUTO_COMPILE_DEFAULT) != 0));

  if(auto_compile)
    dsp_chain_create(fts_get_tick_size());
}

void
fts_dsp_auto_update()
{
  int auto_compile = ((fts_param_get_int(fts_s_dsp_on, 0) != 0) && (fts_param_get_int(fts_s_dsp_auto_compile, DSP_AUTO_COMPILE_DEFAULT) != 0));

  if(auto_compile)
    {
      dsp_chain_delete();
      dsp_chain_create(fts_get_tick_size());
    }
}

/* --------------------------------------------------------------------------- */
/*                                                                             */
/* Object scheduling functions                                                 */
/*                                                                             */
/* --------------------------------------------------------------------------- */

static int 
dsp_gen_outputs(fts_object_t *o, fts_dsp_descr_t *descr)
{
  int i, invs = -1;
  fts_dsp_signal_t **iop;
  fts_atom_t a;
  float sr;
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
	    post("DSP: inputs don't match for object ");
	    post_object( o);
	    post("\n");
	    return 0;
	  }
      }

  if (invs < 0) 
    invs = fts_get_tick_size();

  fts_object_get_prop(o, fts_s_dsp_downsampling, &a);

  if (!fts_is_void(&a))
    size = invs >> fts_get_int(&a);
  else
    {
      fts_object_get_prop(o, fts_s_dsp_upsampling, &a);
    
      if (!fts_is_void(&a))
        size = invs << fts_get_int(&a);
      else
        {
          fts_object_get_prop(o, fts_s_dsp_outputsize, &a);

          if (! fts_is_void(&a))
	    size = fts_get_int(&a);
          else
	    size = invs;
        }
    }

  sr = fts_param_get_float(fts_s_sampling_rate, 44100.) * (double)size / fts_get_tick_size();

  /* note that output signals are assigned only when the output have at least one connection to a dsp object */
  for (i=0, iop=descr->out; i< descr->noutputs; i++, iop++)
    *iop = fts_dsp_signal_new(size, sr);

  sig_zero->length = invs;
  sig_zero->srate = sr;

  return 1;
}

static void 
dsp_object_schedule(dsp_node_t *node)
{
  fts_dsp_signal_t **sig;
  fts_atom_t a;
  int i;

  if (! node->descr)
    {
      node->descr = (fts_dsp_descr_t *)fts_heap_zalloc(dsp_descr_heap);

      node->descr->ninputs  = dsp_input_get(node->o, fts_object_get_inlets_number(node->o));

      if ( node->descr->ninputs)
	{
	  node->descr->in = (fts_dsp_signal_t **)fts_block_zalloc(sizeof(fts_dsp_signal_t *) * node->descr->ninputs); 
	}
      node->descr->noutputs = dsp_output_get(fts_object_get_class(node->o), fts_object_get_outlets_number(node->o));
      node->descr->out = 0;	/* safe initialization */
    }

  /* unreference signal so that they can be reused by the outputs */
  for (i = 0, sig = node->descr->in; i < node->descr->ninputs; i++, sig++)
    if (*sig)
      {
	if (*sig != sig_zero)
	  fts_dsp_signal_unreference(*sig);
      }
    else
      *sig = sig_zero;

  if (node->descr->noutputs)
    {
      node->descr->out = (fts_dsp_signal_t **)fts_block_zalloc(sizeof(fts_dsp_signal_t *) * node->descr->noutputs);
    }

  if (dsp_gen_outputs(node->o, node->descr))
    {
      if(verbose)
	{
	  post( "DSP: scheduling ");
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

	info = ftl_program_get_current_instruction_info( dsp_chain);
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

  dsp_succ_realize(node, dec_pred_inc_refcnt, 0);

  /* This is to free the unreferenced signals, after having computed
     the reference count of all the outputs signals. This happens in case
     of dsp outlets which are not connected
     */
  for (i = 0, sig = node->descr->out; i < node->descr->noutputs; i++, sig++)
    {
      if(*sig && fts_dsp_signal_is_pending( *sig))
	{
	  fts_dsp_signal_free( *sig);
	}
    }

  if (node->descr->ninputs)
    fts_block_free((char *) node->descr->in, sizeof(fts_dsp_signal_t *) * node->descr->ninputs);

  if (node->descr->noutputs)
    fts_block_free((char *) node->descr->out, sizeof(fts_dsp_signal_t *) * node->descr->noutputs);

  fts_heap_free((char *)node->descr, dsp_descr_heap);
  node->descr = 0;
}

/* --------------------------------------------------------------------------- */
/*                                                                             */
/* Graph traversal functions                                                   */
/*                                                                             */
/* --------------------------------------------------------------------------- */

/* mark connections as signal (adding to table ...) */
static void
mark_signal_connection(fts_connection_t* connection, void *arg)
{
  fts_signal_connection_table_t *table = (fts_signal_connection_table_t *)arg;
  fts_object_t *source = fts_connection_get_source(connection);
  int outlet = fts_connection_get_outlet(connection);
  fts_object_t *destination = fts_connection_get_destination(connection);
  int inlet = fts_connection_get_inlet(connection);

  if ( verbose)
    {
      post("  mark:  ");
      post_object(source);
      post(" (%d) --> ", outlet);
      post_object(destination);
      post(" (%d)\n", inlet);
    }

  fts_signal_connection_add(table, connection);
}

/* Generic graph traversal function */
static void dsp_succ_realize( dsp_node_t *node, edge_fun_t fun, int mark_connections)
{
  fts_outlet_decl_t *outlet;
  int woutlet;
  fts_dsp_signal_t **sig;
  static void *zero = 0;

  if ( node->descr)
    sig = node->descr->out;
  else
    sig = (fts_dsp_signal_t **)(&zero);

  outlet = fts_object_get_class(node->o)->outlets;

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
		{
		  if(mark_connections)
		    {
		      fts_connection_t *conn = graph_iterator_get_current_connection( &iter);

		      if (verbose)
			{
			  post("dsp connection:  ");
			  post_object(node->o);
			  post(" (%d) --> ", woutlet);
			  post_object(succ_node->o);
			  post(" (%d)\n", winlet);
			}
		      
		      mark_signal_connection(conn, (void *)&signal_connection_table);
		      graph_iterator_apply_to_connection_stack(&iter, mark_signal_connection, (void *)&signal_connection_table);
		    }

		  (*fun)( node, woutlet, succ_node, winlet, *sig);
		}

	      graph_iterator_next( &iter);
	    }

	  sig++;
	}

      outlet++;
    }
}

/* Edge function used to count predecessors */
static void inc(dsp_node_t *src, int woutlet, dsp_node_t *dest, int winlet, fts_dsp_signal_t *sig)
{
  ASSERT( dest != 0);

  dest->pred_cnt++;
}

/* Edge function used to decrement predecessors count and increment reference count of signals */
static void dec_pred_inc_refcnt(dsp_node_t *src, int woutlet, dsp_node_t *dest, int winlet, fts_dsp_signal_t *sig)
{
  int ninputs;

  ASSERT( dest != 0);

  dest->pred_cnt--;

  ninputs = dsp_input_get(dest->o, fts_object_get_inlets_number(dest->o));

  if (! dest->descr)
    {
      dest->descr = (fts_dsp_descr_t *)fts_heap_zalloc(dsp_descr_heap);
      dest->descr->ninputs = ninputs;
      dest->descr->noutputs = dsp_output_get(fts_object_get_class(dest->o), fts_object_get_outlets_number(dest->o));
      dest->descr->in = 0;
      dest->descr->out = 0;
    }

  if (ninputs)
    {
      int nin;
      fts_dsp_signal_t *previous_sig;

      if (! dest->descr->in)
	{
	  /* (fd) to avoid writing past the end of the dsp_descr... */
	  dest->descr->in = (fts_dsp_signal_t **)fts_block_zalloc(sizeof(fts_dsp_signal_t *) * fts_object_get_inlets_number(dest->o));
	}

      nin = dsp_input_get(dest->o, winlet);

      previous_sig = dest->descr->in[nin];

      if ( previous_sig && previous_sig != sig_zero /*&& fts_dsp_is_order_forced( dest->o)*/)
	{
	  fts_atom_t argv[4];
	  fts_dsp_signal_t *new_sig;

	  fts_dsp_signal_unreference( previous_sig);
	  fts_dsp_signal_reference( sig);

	  /* Should verify that inputs length match */
	  new_sig = fts_dsp_signal_new( sig->length, sig->srate);

	  fts_set_symbol(argv + 0, previous_sig->name);
	  fts_set_symbol(argv + 1, sig->name);
	  fts_set_symbol(argv + 2, new_sig->name);
	  fts_set_int(argv + 3, new_sig->length);

	  dsp_add_funcall( sym_builtin_add, 4, argv);

	  fts_dsp_signal_unreference( sig);
	  fts_dsp_signal_reference( new_sig);

	  dest->descr->in[nin] = new_sig;
	}
      else
	{
	  if (sig)
	    fts_dsp_signal_reference(sig);
	  dest->descr->in[nin] = sig;
	}
    }
}

/* Edge function used to realize a depth first sequentialization of the graph */
static void 
dsp_schedule_depth(dsp_node_t *src, int woutlet, dsp_node_t *dest, int winlet, fts_dsp_signal_t *sig)
{
  ASSERT( dest != 0);

  if ( dest->pred_cnt == 0)
    {
      dsp_object_schedule( dest);
      
      dsp_succ_realize( dest, dsp_schedule_depth, fts_c_signal);

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

static void dsp_graph_send_message( dsp_node_t *graph, fts_symbol_t message)
{
  dsp_node_t *node;

  for( node = graph; node; node = node->next)
    fts_message_send( node->o, fts_SystemInlet, message, 0, 0);
}

static void dsp_graph_count_predecessors( dsp_node_t *graph)
{
  dsp_node_t *node;

  for( node = graph; node; node = node->next)
    dsp_succ_realize( node, inc, 0);
}

static void dsp_graph_schedule( dsp_node_t *graph)
{
  dsp_node_t *node;

  /* schedule all nodes without predecessors */
  for( node = graph; node; node = node->next)
    if ( node->pred_cnt == 0)
      dsp_schedule_depth( 0, 0, node, 0, 0);
}

static void dsp_graph_check_loop( dsp_node_t *graph)
{
  dsp_node_t *node;

  for( node = graph; node; node = node->next)
    if ( !IS_SCHEDULED(node))
      {
	post("Loop in dsp graph: object ");
	post_object( node->o);
	post(" not scheduled\n");
      }
}

void dsp_chain_create(int vs)
{
  if (dsp_is_running)
    dsp_chain_delete();

  /* ask ftl to start memory relocation */
  ftl_mem_start_memory_relocation();/* should it be elsewhere ?? */

  /* ask to  zero the dsp timers next dsp_chain_poll cycle */
  please_zero_dsp_time = 1;

  ftl_program_init( dsp_chain);
  ftl_program_set_current_subroutine( dsp_chain, ftl_program_add_main( dsp_chain));

  fts_dsp_signal_init();

  sig_zero = fts_dsp_signal_new( fts_get_tick_size(), fts_get_sample_rate());
  fts_dsp_signal_reference( sig_zero);

  dsp_graph_reinit( dsp_graph);

  dsp_graph_send_message( dsp_graph, fts_s_put_prologue);

  dsp_graph_count_predecessors( dsp_graph);

  dsp_graph_schedule(dsp_graph);

  dsp_graph_check_loop(dsp_graph);

  dsp_graph_send_message( dsp_graph, fts_s_put_epilogue);

  ftl_mem_end_memory_relocation();
  
  ftl_program_add_return( dsp_chain);
  ftl_program_compile(dsp_chain);

  dsp_is_running = 1;
}

void dsp_chain_delete(void)
{
  if (!dsp_is_running)
    return;

  dsp_is_running = 0;
  
  ftl_program_destroy( dsp_chain);

  fts_signal_connection_remove_all(&signal_connection_table);
}

/* --------------------------------------------------------------------------- */
/*                                                                             */
/* DSP API functions                                                           */
/* Most are just wrapper around FTL API, to keep dsp_chain local               */
/*                                                                             */
/* --------------------------------------------------------------------------- */

fts_object_t *dsp_get_current_object()
{
  if (dsp_chain)
    return ftl_program_get_current_object(dsp_chain);
  else
    return 0;
}

void fts_dsp_declare_function(fts_symbol_t name, void (*w)(fts_word_t *))
{
  ftl_declare_function( name, w);
}

void fts_dsp_declare_inlet(fts_class_t *cl, int num)
{
  fts_method_define(cl, num, fts_s_sig, 0, 0, 0);
}

void fts_dsp_declare_outlet(fts_class_t *cl, int num)
{
  fts_outlet_type_define(cl, num, fts_s_sig, 0, 0);
}

/* insert object in graph */
static void dsp_graph_add_object(dsp_node_t **graph, fts_object_t *o)
{
  dsp_node_t *node;
  fts_atom_t v;

  node = (dsp_node_t *)fts_heap_zalloc(dsp_graph_heap);

  node->o = o;

  fts_set_ptr(&v, (void *)node);
  _fts_object_put_prop(o, fts_s_dsp_descr, &v);

  node->descr = 0;

  node->next = *graph;
  *graph = node;
}

/* remove object from graph */
static void dsp_graph_remove_object(dsp_node_t **graph, fts_object_t *o)
{
  dsp_node_t *node, *prev_node;

  /* We stop the dsp chain, because for sure is not more consistent with the object network; use the param to propagate */
  fts_dsp_auto_stop();

  _fts_object_remove_prop(o, fts_s_dsp_descr);
  
  prev_node = 0;
  for(node=*graph; node; node=node->next)
    {
      if (node->o == o)
	{
	  if (prev_node)
	    prev_node->next = node->next;
	  else
	    *graph = node->next;
	  
	  if (node->descr)
	    {
	      fts_block_free((char *) node->descr->in, sizeof(fts_dsp_signal_t *) * node->descr->ninputs);
	      fts_block_free((char *) node->descr->out, sizeof(fts_dsp_signal_t *) * node->descr->noutputs);
	      fts_heap_free((char *)node->descr, dsp_descr_heap);
	    }
	  
	  fts_heap_free((char *)node, dsp_graph_heap);
	  return;
	}
      
      prev_node = node;
    }

  fts_dsp_auto_restart();
}

void fts_dsp_add_object(fts_object_t *o)
{
  dsp_graph_add_object(&dsp_graph, o);
}

void fts_dsp_remove_object(fts_object_t *o)
{
  dsp_graph_remove_object(&dsp_graph, o);
}

void fts_dsp_add_function(fts_symbol_t symb, int ac, fts_atom_t *av)
{
  ftl_program_add_call(dsp_chain, symb, ac, av);
}

void dsp_add_signal(fts_symbol_t name, int vs)
{
  ftl_program_add_signal(dsp_chain, name, vs);
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

int fts_dsp_is_running( void)
{
  return dsp_is_running;
}

ftl_program_t *dsp_get_current_dsp_chain( void)
{
  return dsp_chain;
}

int fts_dsp_is_sig_inlet(fts_object_t *o, int num)
{
  return fts_method_exists(fts_object_get_class(o), num, fts_s_sig);
}

int fts_dsp_is_input_null( fts_dsp_descr_t *descr, int in)
{
  return descr->in[in] == sig_zero;
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

void fts_dsp_chain_poll( void)
{
  if (dsp_is_running)
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
	  dsp_tick_clock += (double)1.0;
	  dsp_ms_clock = dsp_tick_clock * fts_get_tick_duration();
	}

      ftl_program_run(dsp_chain);
    }
  else
    fts_audioport_idle();
}

/* --------------------------------------------------------------------------- */
/*                                                                             */
/* DSP compiler control                                                        */
/*                                                                             */
/* --------------------------------------------------------------------------- */
static void dspcompiler_builtin_add(fts_word_t *argv)
{
  float * in1 = (float *)fts_word_get_ptr(argv + 0);
  float * in2 = (float *)fts_word_get_ptr(argv + 1);
  float * out = (float *)fts_word_get_ptr(argv + 2);
  int n = fts_word_get_int(argv + 3);
  int i;

  for (i=0; i<n; i++)
    out[i] = in1[i] + in2[i];
}

/* DSP ON parameter listener */
static void dsp_on_listener(void *listener, fts_symbol_t name,  const fts_atom_t *value)
{
  if (fts_is_int(value))
    {
      int on;
      on = fts_get_int(value);

      if (on)
	dsp_chain_create(fts_get_tick_size());
      else
	dsp_chain_delete();
    }
}

void dsp_compiler_init(void)
{
  sym_builtin_add = fts_new_symbol( "__builtin_add");
  dsp_declare_function( sym_builtin_add, dspcompiler_builtin_add);

  dsp_graph_heap = fts_heap_new(sizeof(dsp_node_t));
  dsp_descr_heap = fts_heap_new(sizeof(fts_dsp_descr_t));

  fts_signal_connection_table_init(&signal_connection_table);

  fts_param_add_listener(fts_s_dsp_on, 0, dsp_on_listener);

  dsp_chain = ftl_program_new();
}
