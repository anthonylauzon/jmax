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

#include <string.h>
#include <stdlib.h>
#include <assert.h>

/*#define DSP_COMPILER_VERBOSE*/
/*#define DSP_COMPILER_VERBOSE_DETAILS*/

#include <fts/fts.h>
#include <ftsprivate/class.h>
#include <ftsprivate/connection.h>
#include <ftsprivate/sigconn.h>
#include <ftsprivate/dspgraph.h>

fts_symbol_t fts_s_sig_zero;
fts_symbol_t fts_s_dsp_descr;
fts_symbol_t fts_s_dsp_upsampling;
fts_symbol_t fts_s_dsp_downsampling;
fts_symbol_t fts_s_dsp_outputsize; /* give an absolute size for output vector */

fts_dsp_signal_t *sig_zero;

static fts_symbol_t sym_builtin_add;

static fts_heap_t *dsp_node_heap; /* heap for dsp nodes */
static fts_heap_t *dsp_descr_heap; /* heap for dsp descriptors */

/***********************************************************************
 * 
 * The graph iterator
 *
 */

typedef struct graph_iterator {
  struct stack_element *top;
} graph_iterator_t;

typedef void (*graph_iterator_connection_function_t)(fts_connection_t *connection, void *arg);

extern fts_symbol_t fts_s_propagate_input;

typedef struct stack_element stack_element_t;

struct stack_element {
  fts_object_t *object;
  fts_connection_t *connection_to_thru;
  fts_connection_t *connection;
  stack_element_t *next;
};

static fts_heap_t *stack_element_heap;
static int inited = 0;

#ifdef DEBUG
static void dump_stack( stack_element_t *elem, int depth)
{
  if (elem)
    {
      fprintf( stderr, "[%3d] %s -> (%s,%d)\n",
	       depth,
	       fts_object_get_class_name( elem->object),
	       elem->connection ? fts_object_get_class_name(elem->connection->dst) : "null",
	       elem->connection ? elem->connection->winlet : -1);
      dump_stack( elem->next, depth+1);
    }
}

static void dump_iter( graph_iterator_t *iter, char *msg)
{
  fprintf( stderr, "(%s) dumping stack\n", msg);

  dump_stack( iter->top, 0);
}
#endif

static void 
graph_iterator_push(void *ptr, fts_object_t *object, int outlet)
{
  graph_iterator_t *iter = (graph_iterator_t *)ptr;
  stack_element_t *new, *elem;
  fts_connection_t* connection;

  if (!inited)
    {
      stack_element_heap = fts_heap_new(sizeof(stack_element_t));
      inited = 1;
    }

  /* return if object is already in stack */
  for(elem=iter->top; elem; elem=elem->next)
    if (object == elem->object)
      return;

  new = (stack_element_t *)fts_heap_zalloc(stack_element_heap);
  
  new->object = object;

  if (object->out_conn)
    connection = object->out_conn[outlet];
  else
    connection = 0;

  new->connection = connection;
  new->connection_to_thru = 0;

  new->next = iter->top;
  iter->top = new;
}

static void 
graph_iterator_pop( graph_iterator_t *iter)
{
  stack_element_t *next = iter->top->next;

  fts_heap_free( (char *)iter->top, stack_element_heap);

  iter->top = next;
}

static void 
graph_iterator_step( graph_iterator_t *iter)
{
  if (!iter->top)
    return;

  if (!iter->top->connection)
    {
      graph_iterator_pop( iter);
      graph_iterator_step( iter);
    }
  else 
    {
      /* try to replace object by */
      fts_object_t *dest = iter->top->connection->dst;
      fts_method_t propagate = fts_class_get_method(fts_object_get_class(dest), fts_s_propagate_input);

      if(propagate != NULL)
	{
	  stack_element_t *oldtop = iter->top;
	  fts_atom_t a[3];

	  /* try to get successor of eventual "thru" object */
	  fts_set_pointer(a + 0, graph_iterator_push);
	  fts_set_pointer(a + 1, iter);
	  fts_set_int(a + 2, iter->top->connection->winlet);
	  propagate(dest, fts_system_inlet, fts_s_propagate_input, 3, a);

	  /* skip "thru" object */
	  oldtop->connection_to_thru = oldtop->connection;
	  oldtop->connection = oldtop->connection->next_same_src;	  
	  graph_iterator_step( iter);
	}
    }
}

/* Initialize a graph iterator on the connections of outlet number `outlet' of object `obj' */
static void 
graph_iterator_init( graph_iterator_t *iter, fts_object_t *object, int outlet)
{
  iter->top = 0;

  graph_iterator_push( iter, object, outlet);
  graph_iterator_step( iter);
}

/* Advance to next object */
static void 
graph_iterator_next( graph_iterator_t *iter)
{
  assert( iter->top != 0);

  if (iter->top->connection)
    iter->top->connection = iter->top->connection->next_same_src;

  graph_iterator_step( iter);
}


/* Is it finished ? */
static int 
graph_iterator_end( const graph_iterator_t *iter)
{
  return (iter->top == 0);
}

/* Returns the destination object and the inlet number of current connection */
static void 
graph_iterator_get_current( const graph_iterator_t *iter, fts_object_t **obj, int *inlet)
{
  if (iter->top->connection)
    {
      *obj = iter->top->connection->dst;
      *inlet = iter->top->connection->winlet;
    }
}

static fts_connection_t *
graph_iterator_get_current_connection(const graph_iterator_t *iter)
{
  return iter->top->connection;
}

static void 
graph_iterator_apply_to_connection_stack(const graph_iterator_t *iter, graph_iterator_connection_function_t fun, void *arg)
{
  stack_element_t *elem = iter->top;

  while(elem)
    {
      if(elem->connection_to_thru)
	fun(elem->connection_to_thru, arg);

      elem = elem->next;
    }
}

/***********************************************************************
 * 
 * The signal allocator 
 *
 */

typedef struct _fts_dsp_signal_list_t 
{
  fts_dsp_signal_t *signal;
  struct _fts_dsp_signal_list_t *next;
} fts_dsp_signal_list_t;

static void fts_dsp_signal_list_insert( fts_dsp_signal_list_t **list, fts_dsp_signal_t *sig)
{
  fts_dsp_signal_list_t *tmp;

  tmp = (fts_dsp_signal_list_t *)fts_malloc( sizeof( fts_dsp_signal_list_t) );
  tmp->signal = sig;
  tmp->next = *list;
  *list = tmp;
}

static void fts_dsp_signal_list_delete( fts_dsp_signal_list_t *list)
{
  fts_dsp_signal_list_t *current, *next;

  for ( current = list; current; current = next )
    {
      next = current->next;
      fts_free( current);
    }
}

static int signal_count = 0;
static fts_dsp_signal_list_t *signal_list = 0;

#define SIGNAL_PENDING -1

fts_dsp_signal_t *fts_dsp_signal_new( int vector_size, float sample_rate)
{
  fts_dsp_signal_list_t *current;
  fts_dsp_signal_t *sig;
  char buffer[16];

  for ( current = signal_list; current; current = current->next )
    {
      if ( current->signal->refcnt == 0 && current->signal->length == vector_size)
	{
	  current->signal->refcnt = SIGNAL_PENDING;
	  return current->signal;
	}
    }

  sig = (fts_dsp_signal_t *)fts_zalloc( sizeof(fts_dsp_signal_t));

  sprintf( buffer, "_sig_%d", signal_count++);
  sig->name = fts_new_symbol( buffer);
  sig->refcnt = SIGNAL_PENDING;
  sig->length = vector_size;
  sig->srate = sample_rate;

  dsp_add_signal( sig->name, vector_size);

  fts_dsp_signal_list_insert( &signal_list, sig);

  return sig;
}

void fts_dsp_signal_free( fts_dsp_signal_t *sig)
{
  sig->refcnt = 0;
}

int fts_dsp_signal_is_pending( fts_dsp_signal_t *sig)
{
  return sig->refcnt == SIGNAL_PENDING;
}

void fts_dsp_signal_unreference( fts_dsp_signal_t *sig)
{
  if (sig->refcnt != SIGNAL_PENDING)
    sig->refcnt--;
}

void fts_dsp_signal_reference(fts_dsp_signal_t *sig)
{
  if (sig->refcnt == SIGNAL_PENDING)
    sig->refcnt = 1;
  else
    sig->refcnt++;
}

void fts_dsp_signal_print( fts_dsp_signal_t *s)
{
  post( "dsp_signal *%p{ name=\"%s\" refcnt=%p vs=%d}\n", s, s->name, s->refcnt, s->length);
}

void fts_dsp_signal_init( void)
{
  fts_dsp_signal_list_t *current;

  /* free all the signal currently in signal list */
  for ( current = signal_list; current; current = current->next )
    {
      fts_free( current->signal);
    }

  fts_dsp_signal_list_delete( signal_list);

  signal_count = 0;
  signal_list = 0;
}


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

  if(n > 0)
    {
      for ( i = 0; i < n; i++)
	{
	  post( "%s[%d]", sig[i]->name, sig[i]->length);
	  if ( i != n-1)
	    post( ", ");
	}
    }
  else
    post("NONE");
}

static void 
post_object( fts_object_t *obj)
{
  int ac = fts_object_get_description_size( obj);
  const fts_atom_t *at = fts_object_get_description_atoms( obj);

  if(ac > 0)
    {
      post("(:");
      post_atoms(ac, at);
      post(") ");
    }
  else
    post("(:\?\?\?) ");
}
#endif

/************************************************************************
 *
 *  auxiliary functions
 *
 */

static int
dsp_class_is_outlet(fts_class_t *cl, int out)
{
  return fts_class_outlet_has_type(cl, out, fts_dsp_signal_metaclass);
}

static int 
dsp_input_get(fts_object_t *obj, int winlet)
{
  int i, n;

  for (i = 0, n = 0; i < winlet; i++)
    if (fts_class_inlet_get_method( fts_object_get_class( obj), i, fts_dsp_signal_metaclass) != NULL)
      n++;

  return n;
}

static int 
dsp_output_get(fts_object_t *obj, int woutlet)
{
  fts_class_t *cl = fts_object_get_class(obj);
  int n = 0;
  int i;

  for (i=0; i<woutlet; i++)
    if (dsp_class_is_outlet(cl, i))
      n++;

  return n;
}

static fts_dsp_node_t *
dsp_list_lookup(fts_object_t *o)
{
  fts_atom_t a;

  _fts_object_get_prop(o, fts_s_dsp_descr, &a);

  if (! fts_is_void(&a))
    return (fts_dsp_node_t *) fts_get_pointer(&a);
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
	    fts_object_signal_runtime_error( o, "DSP: inputs don't match");
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

  sr = (float) (sample_rate * (double)size / (double)vector_size);

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

  /* unreference signal so that they can be reused by the outputs */
  for (i=0; i<node->descr.ninputs; i++)
    {
      if(node->descr.in[i])
	{
	  if (node->descr.in[i] != sig_zero)
	    fts_dsp_signal_unreference(node->descr.in[i]);
	}
      else
	node->descr.in[i] = sig_zero;
    }

  if (gen_outputs(node->o, &node->descr, graph->tick_size, graph->sample_rate))
    {
#ifdef DSP_COMPILER_VERBOSE
      post( "DSP: scheduling ");
      post_object( node->o);
      post_signals( node->descr.in, node->descr.ninputs);
      post( " --> ");
      post_signals( node->descr.out, node->descr.noutputs);
      post( "\n");
#endif

      /*
	(fd) Hack to add debugging info to the FTL program.
	We keep the object that was send the message "put" and
	we pass it to ftl_..._add_call so that it stored in the
	debugging info of the DSP chain
      */

      fts_set_pointer(&a, &node->descr);
      fts_send_message(node->o, fts_s_put, 1, &a);

      /*{
	ftl_instruction_info_t *info;

	info = ftl_program_get_current_instruction_info(graph->chain);
	if (info) 
	  {
	      int i;

	    ftl_instruction_info_set_object( info, node->o);

	    ftl_instruction_info_set_ninputs( info, node->descr.ninputs);
	    for ( i = 0; i < node->descr.ninputs; i++)
		ftl_instruction_info_set_input( info, i, fts_dsp_get_input_name( &node->descr, i), fts_dsp_get_input_size( &node->descr,i));

	    ftl_instruction_info_set_noutputs( info, node->descr.noutputs);
	    for ( i = 0; i < node->descr.noutputs; i++)
		ftl_instruction_info_set_output( info, i, fts_dsp_get_output_name( &node->descr, i), fts_dsp_get_output_size( &node->descr,i));
	  }
	  }*/
    }

  SET_SCHEDULED( node);

  dsp_graph_succ_realize(graph, node, dsp_graph_dec_pred_inc_refcnt, 0);

  /* This is to free the unreferenced signals, after having computed
     the reference count of all the outputs signals. This happens in case
     of dsp outlets which are not connected
     */
  for (i=0; i<node->descr.noutputs; i++)
    {
      if(node->descr.out[i] && fts_dsp_signal_is_pending(node->descr.out[i]))
	fts_dsp_signal_free(node->descr.out[i]);
    }
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

  fts_signal_connection_add(table, connection);
}

/* Generic graph traversal function */
static void 
dsp_graph_succ_realize(fts_dsp_graph_t *graph, fts_dsp_node_t *node, edge_fun_t fun, int mark_connections)
{
  fts_object_t *obj = node->o;
  fts_class_t *cl = fts_object_get_class(obj);
  int i;

  for(i=0; i<fts_object_get_outlets_number(obj); i++)
    {
      int sig_out = 0;

      /* for each signal outlet */
      if(dsp_class_is_outlet(cl, i))
	{
	  graph_iterator_t iter;

	  graph_iterator_init( &iter, obj, i);

	  while(!graph_iterator_end( &iter))
	    {
	      fts_object_t *succ_obj;
	      fts_dsp_node_t *succ_node;
	      int in;

	      graph_iterator_get_current( &iter, &succ_obj, &in);

	      succ_node = dsp_list_lookup( succ_obj);

	      if(succ_node)
		{
		  if(mark_connections)
		    {
		      fts_connection_t *conn = graph_iterator_get_current_connection( &iter);

		      mark_signal_connection(conn, (void *)&(graph->signal_connection_table));
		      graph_iterator_apply_to_connection_stack(&iter, mark_signal_connection, (void *)&(graph->signal_connection_table));
		    }

		  (*fun)( graph, node, i, succ_node, in, node->descr.out[sig_out]);
		}

	      graph_iterator_next( &iter);
	    }

	  sig_out++;
	}
    }
}

/* Edge function used to count predecessors */
static void 
dsp_graph_inc(fts_dsp_graph_t *graph, fts_dsp_node_t *src, int woutlet, fts_dsp_node_t *dest, int winlet, fts_dsp_signal_t *sig)
{
  assert( dest != 0);

  dest->pred_cnt++;

#ifdef DSP_COMPILER_VERBOSE_DETAILS
  post("  increment ");
  post_object(dest->o);
  post("(%d)\n", dest->pred_cnt);
#endif
}

/* Edge function used to decrement predecessors count and increment reference count of signals */
static void 
dsp_graph_dec_pred_inc_refcnt(fts_dsp_graph_t *graph, fts_dsp_node_t *src, int woutlet, fts_dsp_node_t *dest, int winlet, fts_dsp_signal_t *sig)
{
  int ninputs;

  assert( dest != 0);

  dest->pred_cnt--;

#ifdef DSP_COMPILER_VERBOSE_DETAILS
  post("  decrement ");
  post_object(dest->o);
  post("(%d)\n", dest->pred_cnt);
#endif

  if (dest->descr.ninputs)
    {
      fts_connection_t *conn = fts_connection_get(src->o, woutlet, dest->o, winlet);
      fts_dsp_signal_t *previous_sig;
      int nin;
      
      nin = dsp_input_get(dest->o, winlet);
      previous_sig = dest->descr.in[nin];
      
      /* ignore hidden connections */
      if(conn == NULL || fts_connection_get_type(conn) != fts_c_order_forcing)
	{
	  /* insert builtin add for double connections */
	  if(previous_sig && previous_sig != sig_zero)
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
	      
	      dest->descr.in[nin] = new_sig;
	    }
	  else
	    {
	      if (sig)
		fts_dsp_signal_reference(sig);
	      
	      dest->descr.in[nin] = sig;
	    }
	}
    }
}

/* Edge function used to realize a depth first sequentialization of the graph */
static void 
dsp_graph_schedule_depth(fts_dsp_graph_t *graph, fts_dsp_node_t *src, int woutlet, fts_dsp_node_t *dest, int winlet, fts_dsp_signal_t *sig)
{
  assert( dest != 0);

  if ( dest->pred_cnt == 0)
    {
      dsp_graph_schedule_node(graph, dest);
      dsp_graph_succ_realize(graph, dest, dsp_graph_schedule_depth, fts_c_signal_active);

      fts_send_message( dest->o, fts_new_symbol("put_after_successors"), 0, 0);
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
  fts_dsp_node_t *node;

  for( node = graph->nodes; node; node = node->next)
    {
      int i;

      node->pred_cnt = 0;

      for (i=0; i<node->descr.ninputs; i++)
	node->descr.in[i] = 0;

      for (i=0; i<node->descr.noutputs; i++)
	node->descr.out[i] = 0;
    }
}

static void 
dsp_graph_send_message( fts_dsp_graph_t *graph, fts_symbol_t message)
{
  fts_dsp_node_t *node;

  for( node = graph->nodes; node; node = node->next)
    fts_send_message( node->o, message, 0, 0);
}

static void 
dsp_graph_count_predecessors( fts_dsp_graph_t *graph)
{
  fts_dsp_node_t *node;

  for( node = graph->nodes; node; node = node->next)
    dsp_graph_succ_realize(graph, node, dsp_graph_inc, 0);
}

static void 
dsp_graph_schedule( fts_dsp_graph_t *graph)
{
  fts_dsp_node_t *node;

  /* schedule all nodes without predecessors */
  for( node = graph->nodes; node; node = node->next)
    {
      if ( node->pred_cnt == 0)
	dsp_graph_schedule_depth(graph, 0, 0, node, 0, 0);
    }
}

static int
dsp_graph_check_loop( fts_dsp_graph_t *graph)
{
  fts_dsp_node_t *node;
  int ok = 1;

  for( node = graph->nodes; node; node = node->next)
    {
      if ( !IS_SCHEDULED(node))
	{
	  fts_object_signal_runtime_error( node->o, "Loop in dsp graph (object not activated)");
	  ok = 0;
	}
    }

  return ok;
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
  fts_dsp_node_t *node = (fts_dsp_node_t *)fts_heap_zalloc(dsp_node_heap);
  fts_atom_t v;

  node->o = o;

  fts_set_pointer(&v, (void *)node);
  _fts_object_put_prop(o, fts_s_dsp_descr, &v);

  node->descr.ninputs = dsp_input_get(o, fts_object_get_inlets_number(o));
  node->descr.noutputs = dsp_output_get(o, fts_object_get_outlets_number(o));
  
  if(node->descr.ninputs)
    node->descr.in = (fts_dsp_signal_t **)fts_zalloc(sizeof(fts_dsp_signal_t *) * node->descr.ninputs);
  
  if(node->descr.noutputs)
    node->descr.out = (fts_dsp_signal_t **)fts_zalloc(sizeof(fts_dsp_signal_t *) * node->descr.noutputs);

  /* insert to list */
  node->next = graph->nodes;
  graph->nodes = node;
}

/* remove object from graph */
void 
fts_dsp_graph_remove_object(fts_dsp_graph_t *graph, fts_object_t *o)
{
  fts_dsp_node_t *node;
  fts_dsp_node_t *prev_node;
  
  fts_dsp_desactivate();

  _fts_object_remove_prop(o, fts_s_dsp_descr);
  
  prev_node = 0;
  for(node=graph->nodes; node; node=node->next)
    {
      if(node->o == o)
	{
	  fts_free(node->descr.in);
	  fts_free(node->descr.out);

	  if(prev_node)
	    prev_node->next = node->next;
	  else
	    graph->nodes = node->next;
	  
	  return;
	}
      else
	prev_node = node;
    }
}

/*********************************************************
 *
 * Initialization
 *
 */

static void 
dsp_graph_builtin_add(fts_word_t *argv)
{
  float * in1 = (float *)fts_word_get_pointer(argv + 0);
  float * in2 = (float *)fts_word_get_pointer(argv + 1);
  float * out = (float *)fts_word_get_pointer(argv + 2);
  int n = fts_word_get_int(argv + 3);
  int i;

  for (i=0; i<n; i++)
    out[i] = in1[i] + in2[i];
}

void fts_kernel_dsp_graph_init(void)
{
  dsp_node_heap = fts_heap_new(sizeof(fts_dsp_node_t));
  dsp_descr_heap = fts_heap_new(sizeof(fts_dsp_descr_t));

  fts_s_sig_zero = fts_new_symbol("_sig_0");
  fts_s_dsp_descr = fts_new_symbol("__DSP_DESCR");
  fts_s_dsp_upsampling   = fts_new_symbol("DSP_UPSAMPLING");
  fts_s_dsp_downsampling = fts_new_symbol("DSP_DOWNSAMPLING");
  fts_s_dsp_outputsize = fts_new_symbol("DSP_OUTPUTSIZE");

  sym_builtin_add = fts_new_symbol( "__builtin_add");
  fts_dsp_declare_function( sym_builtin_add, dsp_graph_builtin_add);
}

