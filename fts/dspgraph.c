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
#include <ftsprivate/object.h>
#include <ftsprivate/connection.h>
#include <ftsprivate/sigconn.h>
#include <ftsprivate/dspgraph.h>

fts_symbol_t fts_s_sig_zero;
fts_dsp_signal_t *sig_zero;

static fts_symbol_t sym_builtin_add;

/* DSP descriptors */
static fts_heap_t *dsp_descr_heap; /* heap for dsp descriptors */

/***********************************************************************
 * 
 * graph iterator
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
  new->connection = fts_object_get_outlet_connections(object, outlet);
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
      fts_method_t propagate = fts_class_get_method_varargs(fts_object_get_class(dest), fts_s_propagate_input);

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
 *  object scheduling functions
 *
 */

#define SCHEDULED -1
#define SET_SCHEDULED(o) ((o)->pred_cnt = SCHEDULED)
#define IS_SCHEDULED(o)  ((o)->pred_cnt == SCHEDULED)

typedef void (*edge_fun_t)(fts_dsp_graph_t *graph, fts_dsp_object_t *src, int woutlet, fts_dsp_object_t *dest, int winlet, fts_dsp_signal_t *sig); 

/* generic graph traversal function */
static void dsp_graph_succ_realize(fts_dsp_graph_t *graph, fts_dsp_object_t *node, edge_fun_t fun, int mark_connections);

/* edge function used to count predecessors */
static void dsp_graph_inc(fts_dsp_graph_t *graph, fts_dsp_object_t *src, int woutlet, fts_dsp_object_t *dest, int winlet, fts_dsp_signal_t *sig);

/* edge function used to decrement predecessors count and increment reference count of signals */
static void dsp_graph_dec_pred_inc_refcnt(fts_dsp_graph_t *graph, fts_dsp_object_t *src, int woutlet, fts_dsp_object_t *dest, int winlet, fts_dsp_signal_t *sig);

/* edge function used to realize a depth first sequentialization of the graph */
static void dsp_graph_schedule_depth(fts_dsp_graph_t *graph, fts_dsp_object_t *src, int woutlet, fts_dsp_object_t *dest, int winlet, fts_dsp_signal_t *sig);

static int 
gen_outputs(fts_dsp_object_t *obj, int vector_size, double sample_rate)
{
  fts_dsp_descr_t *descr = &obj->descr;
  int in_size = -1;
  int out_size = 0;
  float out_sr;
  int resamp;
  int i;
  
  for (i=0; i<descr->ninputs; i++)
    if (descr->in[i] != sig_zero)
      {
	if (in_size < 0) 
	  in_size = descr->in[i]->length;
	else if (in_size != descr->in[i]->length)
	  {
	    fts_object_error((fts_object_t *)obj, "DSP: inputs don't match");
	    return 0;
	  }
      }

  if (in_size < 0) 
    in_size = vector_size;

  resamp = fts_dsp_object_get_resampling(obj);

  /* set output vector size (no user choosen vector size yet) */
  if(resamp == 0)
    out_size = in_size;
  else if (resamp < 0)
    out_size = in_size >> -resamp;
  else if(resamp > 0)
    out_size = in_size << resamp;
    
  out_sr = (float) (sample_rate * (double)out_size / (double)vector_size);

  /* note that output signals are assigned only when the output have at least one connection to a dsp object */
  for (i=0; i<descr->noutputs; i++)
    descr->out[i] = fts_dsp_signal_new(out_size, out_sr);

  /* ??? */
  sig_zero->length = in_size;
  sig_zero->srate = out_sr;

  return 1;
}

static void 
dsp_graph_schedule_node(fts_dsp_graph_t *graph, fts_dsp_object_t *obj)
{
  fts_atom_t a;
  int i;

  /* unreference signal so that they can be reused by the outputs */
  for (i=0; i<obj->descr.ninputs; i++)
    {
      if(obj->descr.in[i])
	{
	  if (obj->descr.in[i] != sig_zero)
	    fts_dsp_signal_unreference(obj->descr.in[i]);
	}
      else
	obj->descr.in[i] = sig_zero;
    }

  if (gen_outputs(obj, graph->tick_size, graph->sample_rate))
    {
#ifdef DSP_COMPILER_VERBOSE
      post( "DSP: scheduling ");
      post_object((fts_object_t *)obj);
      post_signals( obj->descr.in, obj->descr.ninputs);
      post( " --> ");
      post_signals( obj->descr.out, obj->descr.noutputs);
      post( "\n");
#endif

      /*
	(fd) Hack to add debugging info to the FTL program.
	We keep the object that was send the message "put" and
	we pass it to ftl_..._add_call so that it stored in the
	debugging info of the DSP chain
      */

      fts_set_pointer(&a, &obj->descr);
      fts_send_message_varargs((fts_object_t *)obj, fts_s_put, 1, &a);

      /*{
	ftl_instruction_info_t *info;

	info = ftl_program_get_current_instruction_info(graph->chain);
	if (info) 
	  {
	      int i;

	    ftl_instruction_info_set_object( info, (fts_object_t *)obj);

	    ftl_instruction_info_set_ninputs( info, obj->descr.ninputs);
	    for ( i = 0; i < obj->descr.ninputs; i++)
		ftl_instruction_info_set_input( info, i, fts_dsp_get_input_name( &obj->descr, i), fts_dsp_get_input_size( &obj->descr,i));

	    ftl_instruction_info_set_noutputs( info, obj->descr.noutputs);
	    for ( i = 0; i < obj->descr.noutputs; i++)
		ftl_instruction_info_set_output( info, i, fts_dsp_get_output_name( &obj->descr, i), fts_dsp_get_output_size( &obj->descr,i));
	  }
	  }*/
    }

  SET_SCHEDULED(obj);

  dsp_graph_succ_realize(graph, obj, dsp_graph_dec_pred_inc_refcnt, 0);

  /* This is to free the unreferenced signals, after having computed
     the reference count of all the outputs signals. This happens in case
     of dsp outlets which are not connected
     */
  for (i=0; i<obj->descr.noutputs; i++)
    {
      if(obj->descr.out[i] && fts_dsp_signal_is_pending(obj->descr.out[i]))
	fts_dsp_signal_free(obj->descr.out[i]);
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
dsp_graph_succ_realize(fts_dsp_graph_t *graph, fts_dsp_object_t *obj, edge_fun_t fun, int mark_connections)
{
  int i;

  for(i=0; i<obj->descr.noutputs; i++)
    {
      graph_iterator_t iter;
      
      graph_iterator_init( &iter, (fts_object_t *)obj, i);
      
      while(!graph_iterator_end( &iter))
	{
	  fts_object_t *next;
	  int in;
	  
	  graph_iterator_get_current( &iter, &next, &in);
	  
	  if(fts_is_dsp_object(next))
	    {
	      if(mark_connections)
		{
		  fts_connection_t *conn = graph_iterator_get_current_connection( &iter);
		  
		  mark_signal_connection(conn, (void *)&(graph->signal_connection_table));
		  graph_iterator_apply_to_connection_stack(&iter, mark_signal_connection, (void *)&(graph->signal_connection_table));
		}
	      
	      (*fun)( graph, obj, i, (fts_dsp_object_t *)next, in, obj->descr.out[i]);
	    }
	  
	  graph_iterator_next( &iter);
	}
    }
}

/* Edge function used to count predecessors */
static void 
dsp_graph_inc(fts_dsp_graph_t *graph, fts_dsp_object_t *src, int woutlet, fts_dsp_object_t *dest, int winlet, fts_dsp_signal_t *sig)
{
  assert(dest != 0);

  dest->pred_cnt++;

#ifdef DSP_COMPILER_VERBOSE_DETAILS
  post("  increment ");
  post_object((fts_object_t *)dest);
  post("(%d)\n", dest->pred_cnt);
#endif
}

/* Edge function used to decrement predecessors count and increment reference count of signals */
static void 
dsp_graph_dec_pred_inc_refcnt(fts_dsp_graph_t *graph, fts_dsp_object_t *src, int woutlet, fts_dsp_object_t *dest, int winlet, fts_dsp_signal_t *sig)
{
  assert( dest != 0);

  dest->pred_cnt--;

#ifdef DSP_COMPILER_VERBOSE_DETAILS
  post("  decrement ");
  post_object((fts_object_t *)dest);
  post("(%d)\n", dest->pred_cnt);
#endif

  if (dest->descr.ninputs)
    {
      fts_connection_t *conn = fts_connection_get((fts_object_t *)src, woutlet, (fts_object_t *)dest, winlet);
      fts_dsp_signal_t *previous_sig = dest->descr.in[winlet];
      
      /* ignore order forcing connections */
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
	      
	      dest->descr.in[winlet] = new_sig;
	    }
	  else
	    {
	      if (sig)
		fts_dsp_signal_reference(sig);
	      
	      dest->descr.in[winlet] = sig;
	    }
	}
    }
}

/* Edge function used to realize a depth first sequentialization of the graph */
static void 
dsp_graph_schedule_depth(fts_dsp_graph_t *graph, fts_dsp_object_t *src, int woutlet, fts_dsp_object_t *dest, int winlet, fts_dsp_signal_t *sig)
{
  assert( dest != 0);

  if ( dest->pred_cnt == 0)
    {
      dsp_graph_schedule_node(graph, dest);
      dsp_graph_succ_realize(graph, dest, dsp_graph_schedule_depth, fts_c_audio_active);

      fts_send_message_varargs((fts_object_t *)dest, fts_new_symbol("put_after_successors"), 0, 0);
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
  fts_dsp_object_t *obj;

  for(obj = graph->objects; obj != NULL; obj = obj->next_in_dspgraph)
    {
      int i;

      obj->pred_cnt = 0;

      for (i=0; i<obj->descr.ninputs; i++)
	obj->descr.in[i] = 0;

      for (i=0; i<obj->descr.noutputs; i++)
	obj->descr.out[i] = 0;
    }
}

static void 
dsp_graph_send_message( fts_dsp_graph_t *graph, fts_symbol_t message)
{
  fts_dsp_object_t *obj;

  for( obj = graph->objects; obj != NULL; obj = obj->next_in_dspgraph)
    fts_send_message_varargs((fts_object_t *)obj, message, 0, 0);
}

static void 
dsp_graph_count_predecessors( fts_dsp_graph_t *graph)
{
  fts_dsp_object_t *obj;

  for( obj = graph->objects; obj != NULL; obj = obj->next_in_dspgraph)
    dsp_graph_succ_realize(graph, obj, dsp_graph_inc, 0);
}

static void 
dsp_graph_schedule( fts_dsp_graph_t *graph)
{
  fts_dsp_object_t *obj;

  /* schedule all nodes without predecessors */
  for( obj = graph->objects; obj != NULL; obj = obj->next_in_dspgraph)
    {
      if ( obj->pred_cnt == 0)
	dsp_graph_schedule_depth(graph, 0, 0, obj, 0, 0);
    }
}

static int
dsp_graph_check_loop( fts_dsp_graph_t *graph)
{
  fts_dsp_object_t *obj;
  int ok = 1;

  /* schedule all nodes without predecessors */
  for( obj = graph->objects; obj != NULL; obj = obj->next_in_dspgraph)
    {
      if (!IS_SCHEDULED(obj))
	{
	  fts_object_error((fts_object_t *)obj, "loop in dsp graph (object not activated)");
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
fts_dsp_graph_add_object(fts_dsp_graph_t *graph, fts_dsp_object_t *obj)
{
  obj->next_in_dspgraph = graph->objects;
  graph->objects = obj;
}

void 
fts_dsp_graph_remove_object(fts_dsp_graph_t *graph, fts_dsp_object_t *obj)
{
  fts_dsp_object_t **p;
  
  for(p=&graph->objects; *p != NULL; p=&((*p)->next_in_dspgraph))
    {
      if(*p == obj)
	{
	  *p = (*p)->next_in_dspgraph;
	  return;
	}
    }
}

void 
fts_dsp_graph_init(fts_dsp_graph_t *graph, int tick_size, double sample_rate)
{
  graph->objects = 0;
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
  dsp_descr_heap = fts_heap_new(sizeof(fts_dsp_descr_t));
  fts_s_sig_zero = fts_new_symbol("_sig_0");

  sym_builtin_add = fts_new_symbol( "__builtin_add");
  fts_dsp_declare_function( sym_builtin_add, dsp_graph_builtin_add);
}

