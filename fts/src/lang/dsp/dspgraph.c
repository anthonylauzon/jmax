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
#include <fts/lang/dsp/dspgraph.h>
#include <fts/lang/dsp/sigconn.h>
#include "signal.h"
#include "gphiter.h"

#include <fts/runtime.h>

#define ASSERT(e) if (!(e)) { fprintf( stderr, "Assertion (%s) failed file %s line %d\n",#e,__FILE__,__LINE__); *(char *)0 = 0;}

fts_symbol_t fts_s_sig_zero;
fts_symbol_t fts_s_dsp_descr;
fts_symbol_t fts_s_dsp_upsampling;
fts_symbol_t fts_s_dsp_downsampling;
fts_symbol_t fts_s_dsp_outputsize; /* give an absolute size for output vector */

fts_dsp_signal_t *sig_zero;

static fts_symbol_t sym_builtin_add;

static fts_heap_t *dsp_node_heap; /* heap for dsp nodes */
static fts_heap_t *dsp_descr_heap; /* heap for dsp descriptors */

/******************************************************************************
 *
 * The dsp_node structure
 *
 */
#define SCHEDULED -1
#define SET_SCHEDULED( node) ((node)->pred_cnt = SCHEDULED)
#define IS_SCHEDULED( node)  ( (node)->pred_cnt == SCHEDULED)

typedef void (*edge_fun_t)(fts_dsp_graph_t *graph, fts_dsp_node_t *src, int woutlet, fts_dsp_node_t *dest, int winlet, fts_dsp_signal_t *sig); 

/* generic graph traversal function */
static void dsp_graph_succ_realize(fts_dsp_graph_t *graph, fts_dsp_node_t *node, edge_fun_t fun, int mark_connections);

/* edge function used to count predecessors */
static void dsp_graph_inc(fts_dsp_graph_t *graph, fts_dsp_node_t *src, int woutlet, fts_dsp_node_t *dest, int winlet, fts_dsp_signal_t *sig);

/* edge function used to decrement predecessors count and increment reference count of signals */
static void dsp_graph_dec_pred_inc_refcnt(fts_dsp_graph_t *graph, fts_dsp_node_t *src, int woutlet, fts_dsp_node_t *dest, int winlet, fts_dsp_signal_t *sig);

/* edge function used to realize a depth first sequentialization of the graph */
static void dsp_graph_schedule_depth(fts_dsp_graph_t *graph, fts_dsp_node_t *src, int woutlet, fts_dsp_node_t *dest, int winlet, fts_dsp_signal_t *sig);

/************************************************************************
 *
 *  debugging
 *
 */

#ifdef DSP_COMPILER_VERBOSE
static void 
post_signals( fts_dsp_signal_t **sig, int n)
{
  int i;

  for ( i = 0; i < n; i++)
    {
      post( "%s[%d]", fts_symbol_name( sig->name), sig->length);
      if ( i != n-1)
	post( ",");
    }
}
#endif

/************************************************************************
 *
 *  auxiliary functions
 *
 */

static int 
dsp_input_get(fts_object_t *obj, int winlet)
{
  int i, n;

  for (i = 0, n = 0; i < winlet; i++)
    if (fts_class_has_method( fts_object_get_class( obj), i, fts_s_sig))
      n++;

  return n;
}

static int 
dsp_output_get(fts_class_t *cl, int woutlet)
{
  int i, n;
  fts_outlet_decl_t *out;

  for (i = 0, n = 0, out = cl->outlets; i < woutlet; i++, out++)
    if (out->tmess.symb == fts_s_sig || !out->tmess.symb)
      n++;

  return n;
}

static fts_dsp_node_t *
dsp_list_lookup(fts_object_t *o)
{
  fts_atom_t a;

  _fts_object_get_prop(o, fts_s_dsp_descr, &a);

  if (! fts_is_void(&a))
    return (fts_dsp_node_t *) fts_get_ptr(&a);
  else
    return 0;
}

/************************************************************************
 *
 *  object scheduling functions
 *
 */

static int 
gen_outputs(fts_object_t *o, fts_dsp_descr_t *descr, int vector_size, double sample_rate)
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
    invs = vector_size;

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

  sr = sample_rate * (double)size / (double)vector_size;

  /* note that output signals are assigned only when the output have at least one connection to a dsp object */
  for (i=0, iop=descr->out; i< descr->noutputs; i++, iop++)
    {
      *iop = fts_dsp_signal_new(size, sr);
    }

  sig_zero->length = invs;
  sig_zero->srate = sr;

  return 1;
}

static void 
dsp_graph_schedule_node(fts_dsp_graph_t *graph, fts_dsp_node_t *node)
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

  if (gen_outputs(node->o, node->descr, graph->tick_size, graph->sample_rate))
    {
#ifdef DSP_COMPILER_VERBOSE
      post( "DSP: scheduling ");
      post_object( node->o);
      post_signals( node->descr->in, node->descr->ninputs);
      post( "->");
      post_signals( node->descr->out, node->descr->noutputs);
      post( "\n");
#endif

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

	info = ftl_program_get_current_instruction_info(graph->chain);
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

  dsp_graph_succ_realize(graph, node, dsp_graph_dec_pred_inc_refcnt, 0);

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

/**************************************************************************
 *
 *  graph traversal functions
 *
 */

/* mark connections as signal (adding to table ...) */
static void
mark_signal_connection(fts_connection_t* connection, void *arg)
{
  fts_signal_connection_table_t *table = (fts_signal_connection_table_t *)arg;
  fts_object_t *source = fts_connection_get_source(connection);
  int outlet = fts_connection_get_outlet(connection);
  fts_object_t *destination = fts_connection_get_destination(connection);
  int inlet = fts_connection_get_inlet(connection);

#ifdef DSP_COMPILER_VERBOSE
  post("  mark:  ");
  post_object(source);
  post(" (%d) --> ", outlet);
  post_object(destination);
  post(" (%d)\n", inlet);
#endif

  fts_signal_connection_add(table, connection);
}

/* Generic graph traversal function */
static void 
dsp_graph_succ_realize(fts_dsp_graph_t *graph, fts_dsp_node_t *node, edge_fun_t fun, int mark_connections)
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
	      fts_dsp_node_t *succ_node;
	      int winlet;

	      graph_iterator_get_current( &iter, &succ_obj, &winlet);

	      succ_node = dsp_list_lookup( succ_obj);

	      if (succ_node)
		{
		  if(mark_connections)
		    {
		      fts_connection_t *conn = graph_iterator_get_current_connection( &iter);

#ifdef DSP_COMPILER_VERBOSE
		      post("dsp connection:  ");
		      post_object(node->o);
		      post(" (%d) --> ", woutlet);
		      post_object(succ_node->o);
		      post(" (%d)\n", winlet);
#endif
		      
		      mark_signal_connection(conn, (void *)&(graph->signal_connection_table));
		      graph_iterator_apply_to_connection_stack(&iter, mark_signal_connection, (void *)&(graph->signal_connection_table));
		    }

		  (*fun)( graph, node, woutlet, succ_node, winlet, *sig);
		}

	      graph_iterator_next( &iter);
	    }

	  sig++;
	}

      outlet++;
    }
}

/* Edge function used to count predecessors */
static void 
dsp_graph_inc(fts_dsp_graph_t *graph, fts_dsp_node_t *src, int woutlet, fts_dsp_node_t *dest, int winlet, fts_dsp_signal_t *sig)
{
  ASSERT( dest != 0);

  dest->pred_cnt++;
}

/* Edge function used to decrement predecessors count and increment reference count of signals */
static void 
dsp_graph_dec_pred_inc_refcnt(fts_dsp_graph_t *graph, fts_dsp_node_t *src, int woutlet, fts_dsp_node_t *dest, int winlet, fts_dsp_signal_t *sig)
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

	  ftl_program_add_call(graph->chain, sym_builtin_add, 4, argv);

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
dsp_graph_schedule_depth(fts_dsp_graph_t *graph, fts_dsp_node_t *src, int woutlet, fts_dsp_node_t *dest, int winlet, fts_dsp_signal_t *sig)
{
  ASSERT( dest != 0);

  if ( dest->pred_cnt == 0)
    {
      dsp_graph_schedule_node(graph, dest);
      dsp_graph_succ_realize(graph, dest, dsp_graph_schedule_depth, fts_c_signal);

      fts_message_send( dest->o, fts_SystemInlet, fts_new_symbol("put_after_successors"), 0, 0);
    }
}

/**************************************************************************
 *
 *  DSP chain creation functions
 *
 */

static void 
dsp_graph_reinit( fts_dsp_graph_t *graph)
{
  fts_dsp_node_t *nodes = graph->nodes;
  fts_dsp_node_t *node;

  for( node = nodes; node; node = node->next)
    node->pred_cnt = 0;
}

static void 
dsp_graph_send_message( fts_dsp_graph_t *graph, fts_symbol_t message)
{
  fts_dsp_node_t *nodes = graph->nodes;
  fts_dsp_node_t *node;

  for( node = nodes; node; node = node->next)
    fts_message_send( node->o, fts_SystemInlet, message, 0, 0);
}

static void 
dsp_graph_count_predecessors( fts_dsp_graph_t *graph)
{
  fts_dsp_node_t *nodes = graph->nodes;
  fts_dsp_node_t *node;

  for( node = nodes; node; node = node->next)
    dsp_graph_succ_realize(graph, node, dsp_graph_inc, 0);
}

static void 
dsp_graph_schedule( fts_dsp_graph_t *graph)
{
  fts_dsp_node_t *nodes = graph->nodes;
  fts_dsp_node_t *node;

  /* schedule all nodes without predecessors */
  for( node = nodes; node; node = node->next)
    {
      if ( node->pred_cnt == 0)
	dsp_graph_schedule_depth(graph, 0, 0, node, 0, 0);
    }
}

static void 
dsp_graph_check_loop( fts_dsp_graph_t *graph)
{
  fts_dsp_node_t *nodes = graph->nodes;
  fts_dsp_node_t *node;

  for( node = nodes; node; node = node->next)
    {
      if ( !IS_SCHEDULED(node))
	{
	  post("Loop in dsp graph: object ");
	  post_object( node->o);
	  post(" not scheduled\n");
	}
    }
}

/**************************************************************************
 *
 *  DSP graph API functions
 *
 */

void 
fts_dsp_graph_init(fts_dsp_graph_t *graph, int tick_size, double sample_rate)
{
  graph->nodes = 0;
  graph->chain = ftl_program_new();
  graph->status = status_reset;
  graph->tick_size = tick_size;
  graph->sample_rate = sample_rate;

  fts_signal_connection_table_init(&graph->signal_connection_table);
}

void
fts_dsp_graph_compile(fts_dsp_graph_t *graph)
{
  /* ask ftl to start memory relocation */
  ftl_mem_start_memory_relocation();/* should it be elsewhere ?? */

  ftl_program_init(graph->chain);
  ftl_program_set_current_subroutine(graph->chain, ftl_program_add_main(graph->chain));

  fts_dsp_signal_init();

  sig_zero = fts_dsp_signal_new(graph->tick_size, graph->sample_rate);
  fts_dsp_signal_reference( sig_zero);

  dsp_graph_reinit(graph);
  dsp_graph_send_message(graph, fts_s_put_prologue);
  dsp_graph_count_predecessors(graph);
  dsp_graph_schedule(graph);
  dsp_graph_check_loop(graph);
  dsp_graph_send_message(graph, fts_s_put_epilogue);

  ftl_mem_end_memory_relocation();
  
  ftl_program_add_return(graph->chain);
  ftl_program_compile(graph->chain);

  graph->status = status_compiled;
}

void 
fts_dsp_graph_reset(fts_dsp_graph_t *graph)
{
  graph->status = status_reset;

  ftl_program_destroy(graph->chain);
  
  fts_signal_connection_remove_all(&graph->signal_connection_table);
}


void 
fts_dsp_graph_run(fts_dsp_graph_t *graph)
{
  ftl_program_run(graph->chain);
}

/* insert object in graph */
void 
fts_dsp_graph_add_object(fts_dsp_graph_t *graph, fts_object_t *o)
{
  fts_dsp_node_t **nodes_ptr = &graph->nodes;
  fts_dsp_node_t *node;
  fts_atom_t v;

  /* fts_dsp_graph_reset(graph); */

  node = (fts_dsp_node_t *)fts_heap_zalloc(dsp_node_heap);

  node->o = o;

  fts_set_ptr(&v, (void *)node);
  _fts_object_put_prop(o, fts_s_dsp_descr, &v);

  node->descr = 0;

  node->next = *(nodes_ptr);
  *(nodes_ptr) = node;
}

/* remove object from graph */
void 
fts_dsp_graph_remove_object(fts_dsp_graph_t *graph, fts_object_t *o)
{
  fts_dsp_node_t **nodes_ptr = &graph->nodes;
  fts_dsp_node_t *node;
  fts_dsp_node_t *prev_node;
  
  fts_param_set_int(fts_s_dsp_on, 0);

  _fts_object_remove_prop(o, fts_s_dsp_descr);
  
  prev_node = 0;
  for(node=*(nodes_ptr); node; node=node->next)
    {
      if (node->o == o)
	{
	  if (prev_node)
	    prev_node->next = node->next;
	  else
	    *(nodes_ptr) = node->next;
	  
	  if (node->descr)
	    {
	      fts_block_free((char *) node->descr->in, sizeof(fts_dsp_signal_t *) * node->descr->ninputs);
	      fts_block_free((char *) node->descr->out, sizeof(fts_dsp_signal_t *) * node->descr->noutputs);
	      fts_heap_free((char *)node->descr, dsp_descr_heap);
	    }
	  
	  fts_heap_free((char *)node, dsp_node_heap);
	  return;
	}
      
      prev_node = node;
    }
}

/*********************************************************
 *
 *  config 
 *
 */

static void 
dsp_graph_builtin_add(fts_word_t *argv)
{
  float * in1 = (float *)fts_word_get_ptr(argv + 0);
  float * in2 = (float *)fts_word_get_ptr(argv + 1);
  float * out = (float *)fts_word_get_ptr(argv + 2);
  int n = fts_word_get_int(argv + 3);
  int i;

  for (i=0; i<n; i++)
    out[i] = in1[i] + in2[i];
}

void
fts_dsp_graph_config(void)
{
  dsp_node_heap = fts_heap_new(sizeof(fts_dsp_node_t));
  dsp_descr_heap = fts_heap_new(sizeof(fts_dsp_descr_t));

  fts_s_sig_zero = fts_new_symbol("_sig_0");
  fts_s_dsp_descr = fts_new_symbol("__DSP_DESCR");
  fts_s_dsp_upsampling   = fts_new_symbol("DSP_UPSAMPLING");
  fts_s_dsp_downsampling = fts_new_symbol("DSP_DOWNSAMPLING");
  fts_s_dsp_outputsize = fts_new_symbol("DSP_OUTPUTSIZE");

  sym_builtin_add = fts_new_symbol( "__builtin_add");
  dsp_declare_function( sym_builtin_add, dsp_graph_builtin_add);
}
