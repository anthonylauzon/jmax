#include "sys.h"
#include "lang/mess.h"
#include "lang/ftl.h"
#include "lang/dsp.h"

#include <stdio.h>

#define DEBUG_DSP_COMPILER

#define ASSERT(e) if (!(e)) { fprintf( stderr, "assertion (%s) failed file %s line %d\n",#e,__FILE__,__LINE__); *(char *)0 = 0;}


/* clocks for use with timer primitives.
   we provide (see the dsp_timer_install function below)
   two timers (dsp_msec and dsp_tick) that measure
   the elapsed time after the dac start.
   Their values is zero after a delete 
*/

static int  please_zero_dsp_time = 0; /* time must be zeroed in the same place 
					 it is incremented, otherwise it will
					 never be zero in fts_alarm_sched.
				       */
static double dsp_ms_clock = 0;
static double dsp_tick_clock = 0;



/* System property symbol and functions used by the dead code elimination subsystem */

static fts_symbol_t fts_s_dsp_descr;

static fts_heap_t dsp_graph_heap;
static fts_heap_t dsp_descr_heap;

/* For switch hack, temporaly made global */
ftl_program_t *dsp_chain_on = 0;
static ftl_program_t *dsp_chain_off = 0;
static ftl_program_t *dsp_chain = 0;

static dsp_signal *sig_zero;
static int graph_obj_count;


/* The dsp_node structure */
typedef struct dsp_node_t dsp_node_t;

#define IS_SCHEDULED -1

struct dsp_node_t {
  fts_object_t *o;
  int pred_cnt;
  fts_dsp_descr_t *descr;
  dsp_node_t *next;
};

typedef void (*edge_fun_t)(dsp_node_t *src, int woutlet, 
			    dsp_node_t *dest, int winlet, 
			    dsp_signal *sig); 

static dsp_node_t *dsp_graph;

static void dsp_gen_outputs(fts_object_t *o, fts_dsp_descr_t *descr);


static int
dsp_input_get(fts_object_t *obj, int winlet)
{
  int i, n;

  for (i = 0, n = 0; i < winlet; i++)
    if (fts_object_handle_message(obj, i, fts_s_sig))
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

/* insert object in graph; call from init method object */
void
dsp_list_insert(fts_object_t *o)
{
  dsp_node_t *g;
  fts_atom_t v;

  g = (dsp_node_t *)fts_heap_zalloc(&dsp_graph_heap);

  g->o = o;

  fts_set_ptr(&v, (void *)g);
  _fts_object_put_prop(o, fts_s_dsp_descr, &v);

  g->descr = 0;

  graph_obj_count++;

  g->next = dsp_graph;
  dsp_graph = g;
}

/* remove object from graph; call from delet method object */
void
dsp_list_remove(fts_object_t *o)
{
  dsp_node_t *g, *pg;

  /* We stop the dsp chain, because for sure is not more
   consistent with the object network */

  dsp_chain_delete();

  _fts_object_remove_prop(o, fts_s_dsp_descr);

  pg = 0;
  for( g = dsp_graph; g; g = g->next)
    {
      if (g->o == o)
	{
	  if (pg)
	    pg->next = g->next;
	  else
	    dsp_graph = g->next;

	  if (g->descr)
	    {
	      fts_block_free((char *) g->descr->in, sizeof(dsp_signal *) * g->descr->ninputs);
	      fts_block_free((char *) g->descr->out, sizeof(dsp_signal *) * g->descr->noutputs);
	      fts_heap_free((char *)g->descr, &dsp_descr_heap);
	    }

	  fts_heap_free((char *)g, &dsp_graph_heap);
	  graph_obj_count--;
	  return;
	}

      pg = g;
    }
}

/* static deleted because used from the optimizer */

dsp_node_t *
dsp_list_lookup(fts_object_t *o)
{
  /* old stuff
  dsp_node_t *g;

  for( g = dsp_graph; g; g = g->next)
    if (g->o == o)
      return g;
      */

  fts_atom_t a;

  _fts_object_get_prop(o, fts_s_dsp_descr, &a);

  if (! fts_is_void(&a))
    return (dsp_node_t *) fts_get_ptr(&a);
  else
    return 0;
}

static int
is_connected_to_dsp_objects( fts_object_t *object)
{
  fts_connection_t **conn;
  fts_outlet_decl_t *outlet;
  int n, noutlets, connected;

  if (fts_object_is_outlet( object))
    {
      fts_object_t *patcher;
      int pos;

      pos = ((fts_outlet_t *)object)->position;
      patcher = (fts_object_t *) fts_object_get_patcher( object);
      outlet = &(patcher->cl->outlets[pos]);
      conn = &(patcher->out_conn[pos]);
      noutlets = 1;
    }
  else
    {
      outlet = object->cl->outlets;
      conn = object->out_conn;
      noutlets = fts_object_get_outlets_number(object);
    }

  for (n = 0; n < noutlets; n++)
    {
      if (outlet->tmess.symb == fts_s_sig || !outlet->tmess.symb)
	{
	  fts_connection_t *c;

	  for ( c = *conn; c; c = c->next_same_src)
	    {
	      if (fts_object_is_patcher( c->dst))
		{
		  fts_object_t *inlet;

		  inlet = fts_patcher_get_inlet( c->dst, c->winlet);

		  if (inlet && is_connected_to_dsp_objects(inlet))
		    return 1;
		}
	      else if (fts_object_is_outlet( c->dst))
		{
		  if (is_connected_to_dsp_objects(c->dst))
		    return 1;
		}
	      else if ( dsp_list_lookup( c->dst))
		return 1;
	    }
	}
      outlet++;
      conn++;
    }

  return 0;
}


static void
dsp_succ_realize( dsp_node_t *node, edge_fun_t fun)
{
  fts_connection_t **conn;
  fts_outlet_decl_t *outlet;
  int n;
  int noutlets;
  dsp_signal **sig;
  static void *zero = 0;

  if ( node->descr)
    sig = node->descr->out;
  else
    sig = (dsp_signal **)(&zero);

  /* for the outlet objects, we take the patcher out connections */
  /* @@@ This is wrong: we should take only
     the outlet of the patcher the outlet object represent !!! 
     */

  if (fts_object_is_outlet( node->o))
    {
      fts_object_t *patcher;
      int pos;

      pos = ((fts_outlet_t *)node->o)->position;

      patcher = (fts_object_t *) fts_object_get_patcher( node->o);
      outlet = &(patcher->cl->outlets[pos]);
      conn = &(patcher->out_conn[pos]);
      noutlets = 1;
    }
  else
    {
      outlet = node->o->cl->outlets;
      conn = node->o->out_conn;
      noutlets = fts_object_get_outlets_number(node->o);
    }

  for (n = 0; n < noutlets; n++)
    {
      if (outlet->tmess.symb == fts_s_sig || !outlet->tmess.symb)
	{
	  fts_connection_t *c;

	  for ( c = *conn; c; c = c->next_same_src)
	    {
	      dsp_node_t *successor_node;
	      int winlet;

	      if (fts_object_is_patcher( c->dst))
		{
		  fts_object_t *inlet;

		  inlet = fts_patcher_get_inlet( c->dst, c->winlet);

		  if (inlet)
		    {
		      successor_node = dsp_list_lookup(inlet);
		    }
		  else
		    {
		      successor_node = 0;
		    }

		  winlet = 0;
		}
	      else
		{
		  successor_node = dsp_list_lookup( c->dst);
		  winlet = c->winlet;
		}

	      if (successor_node)
		{
		  (*fun)( node, n, successor_node, winlet, *sig);
		}
	    }
	  sig++;
	}
      outlet++;
      conn++;
    }
}

static void
inc(dsp_node_t *src, int woutlet, dsp_node_t *dest, int winlet, dsp_signal *sig)
{
  ASSERT( dest != 0);

  dest->pred_cnt++;
}

static void
dec_pred_inc_refcnt(dsp_node_t *src, int woutlet, dsp_node_t *dest, int winlet, dsp_signal *sig)
{
  int ninputs;

  ASSERT( dest != 0);

  dest->pred_cnt--;

  ninputs = dsp_input_get(dest->o, fts_object_get_inlets_number(dest->o));

  if (! dest->descr)
    {
      dest->descr = (fts_dsp_descr_t *)fts_heap_zalloc(&dsp_descr_heap);
      dest->descr->ninputs = ninputs;
      dest->descr->noutputs = dsp_output_get(dest->o->cl, fts_object_get_outlets_number(dest->o));
    }

  if (ninputs)
    {
      int nin;
      dsp_signal *previous_sig;

      if (! dest->descr->in)
		  dest->descr->in = (dsp_signal **)fts_block_zalloc(sizeof(dsp_signal *) * ninputs);

      nin = dsp_input_get(dest->o, winlet);

      ASSERT( nin < ninputs);

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
	  post( "Multiple signal connections between outlet %d of object %s and inlet %d of object %s (connection ignored)\n",
		woutlet, fts_symbol_name(fts_object_get_class_name(src->o)),
		winlet, fts_symbol_name(fts_object_get_class_name(dest->o)));
#endif
	}
    }
}

static void
dsp_graph_reinit( dsp_node_t *graph)
{
  dsp_node_t *g;

  for( g = graph; g; g = g->next)
    g->pred_cnt = 0;
}

static void
dsp_pred_count( dsp_node_t *graph)
{
  dsp_node_t *node;

  for( node = graph; node; node = node->next)
    dsp_succ_realize( node, inc);
}

static void
dsp_chain_create_start(int vs)
{
  extern void ftl_mem_start_memory_relocation(void);

  /* ask ftl to start memory relocation */

  ftl_mem_start_memory_relocation();/* should it be elsewhere ?? */

  /*ask to  zero the dsp timers next dsp_chain_poll cycle */

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

static void
dsp_chain_create_end(void)
{
  extern void ftl_mem_end_memory_relocation(void);

  ftl_mem_end_memory_relocation();

  ftl_program_add_return( dsp_chain_on);
  
  ftl_program_compile(dsp_chain_on);

  dsp_chain = dsp_chain_on;

  fts_audio_activate_devices();
}


static void dsp_object_schedule(dsp_node_t *g)
{
  dsp_signal **sig;
  fts_atom_t a;
  int i, dead_code;

  dead_code = 0;
  /* (FD): 
     must not schedule objects which (are inlets or outlets) and (are not connected to DSP objects) 
     */
  if ( (fts_object_is_inlet( g->o) || fts_object_is_outlet( g->o)) && !is_connected_to_dsp_objects( g->o))
    {
      dead_code = 1;
    }

  if (! g->descr)
    {
      g->descr = (fts_dsp_descr_t *)fts_heap_zalloc(&dsp_descr_heap);
      g->descr->ninputs  = dsp_input_get(g->o, fts_object_get_inlets_number(g->o));
      if ( g->descr->ninputs)
		  g->descr->in = (dsp_signal **)fts_block_zalloc(sizeof(dsp_signal *) * g->descr->ninputs);
      g->descr->noutputs = dsp_output_get(g->o->cl, fts_object_get_outlets_number(g->o));
    }

  /* For IRIX 6.2 MipsPro 7.x, we don't reuse signals
     between in and outs, but we guarantee that all the arguments
     point to different buffers, so that we can use advanced 
     optimization without fearing pointer aliasing.
   */

  for (i = 0, sig = g->descr->in; i < g->descr->ninputs; i++, sig++)
    if (*sig)
      {
	if (*sig != sig_zero)
	  Sig_unreference(*sig);
      }
    else
      *sig = sig_zero;

  if (g->descr->noutputs)
    g->descr->out = (dsp_signal **)fts_block_zalloc(sizeof(dsp_signal *) * g->descr->noutputs);


  /* Now that dsp_gen_outputs compute the downsampling using
     object properties, we don't need to call it in the
     object put method; it become a private function,
     and is not part of the API !!
   */
  if (!dead_code)
    {
      dsp_gen_outputs(g->o, g->descr);

      fts_set_ptr(&a, g->descr);
      fts_message_send(g->o, fts_SystemInlet, fts_s_put, 1, &a);
    }

  g->pred_cnt = IS_SCHEDULED;

  dsp_succ_realize(g, dec_pred_inc_refcnt);

  /* This is to free the unreferenced signals, after having computed the reference count 
	  of all the outputs signals. This happens in case of dsp outlets which are not connected */
  for (i = 0, sig = g->descr->out; i < g->descr->noutputs; i++, sig++)
    if ( *sig)
      if ( (*sig)->refcnt == 0)
	Sig_free( *sig);

  if (g->descr->ninputs)
    fts_block_free((char *) g->descr->in, sizeof(dsp_signal *) * g->descr->ninputs);

  if (g->descr->noutputs)
    fts_block_free((char *) g->descr->out, sizeof(dsp_signal *) * g->descr->noutputs);

  fts_heap_free((char *)g->descr, &dsp_descr_heap);
  g->descr = 0;
}

static void
dsp_schedule_depth(dsp_node_t *src, int woutlet, dsp_node_t *dest, int winlet, dsp_signal *sig)
{
  ASSERT( dest != 0);

  if ( dest->pred_cnt == 0)
    {
      dsp_object_schedule( dest);

      dsp_succ_realize( dest, dsp_schedule_depth);

      fts_message_send( dest->o, fts_SystemInlet, fts_new_symbol("put_after_successors"), 0, 0);
    }
}


void 
dsp_chain_create(int vs)
{
  dsp_node_t *node;

  if (dsp_is_running())
    {
      post( "DSP is running, cannot create DSP chain\n");
      return;
    }

  dsp_chain_create_start(vs);

  /* schedule all nodes without predecessors */
  for( node = dsp_graph; node; node = node->next)
    if ( node->pred_cnt == 0)
      dsp_schedule_depth( 0, 0, node, 0, 0);

  /* Add a dac syncronization/zero call for each out device not used by an object */

  fts_audio_add_unused_zero_fun();

  /* looks for loop */
  for( node = dsp_graph; node; node = node->next)
    if ( node->pred_cnt != IS_SCHEDULED)
      {
	post( "Loop in dsp graph: object %s not scheduled\n", 
	      fts_symbol_name( node->o->cl->mcl->name));
      }

  dsp_chain_create_end();
}

void 
dsp_chain_delete(void)
{
  /* First we send remote messages, then we delete
     the local ones */

  if ( !dsp_is_running())
    return;

  if ( dsp_chain_on)
    ftl_program_destroy( dsp_chain_on);

  dsp_chain = dsp_chain_off;

  fts_audio_deactivate_devices();
}



static void
dsp_gen_outputs(fts_object_t *o, fts_dsp_descr_t *descr)
{
  int i, invs = -1;
  dsp_signal **iop;
  fts_atom_t a;
  int size;

  /* Compute the size using object properties,
     starting from the input size (or default VS if not present), 
     and using the upsampling/downsampling object property to 
     compute the actual size.
     If both are present, we take down.
    */

  for (i = 0, iop = descr->in; i < descr->ninputs; i++, iop++)
    if (*iop != sig_zero)
      {
	if (invs < 0) 
	  invs = (*iop)->length;
	else if (invs != (*iop)->length)
	  {
	  
/* nos: 
  "shouldn't it be the object 
  to decide if it accepts different 
  vector sizes at different inputs
  or not and what to do??"
*/
	    post("inputs don't match (guess where...;-)\n");
	    return;
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

  /* output signals are assigned only when the output have at least
     one connection to a dsp object.
     
   */

  for (i = 0, iop = descr->out; i< descr->noutputs; i++, iop++)
    {
      /* TO BE CHANGED USING Inlet and outlet properties !!! */

      *iop = Sig_new(size);
    }

/* BAD! (nos) */
/*  if (sig_zero->length < invs) */
    sig_zero->length = invs;

  sig_zero->srate = fts_dsp_get_sampling_rate() / (double)(DEFAULTVS/size);
}

void
dsp_sig_inlet(fts_class_t *cl, int num)
{
  fts_method_define(cl, num, fts_s_sig, 0, 0, 0);
}

void
dsp_sig_outlet(fts_class_t *cl, int num)
{
  fts_outlet_type_define(cl, num, fts_s_sig, 0, 0);
}

/* exported, must be redone each time a new out device is installed */

void
dsp_make_dsp_off_chain(void)
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
     

void
dsp_init(void)
{
  fts_heap_init(&dsp_graph_heap, sizeof(dsp_node_t), 32);
  fts_heap_init(&dsp_descr_heap, sizeof(fts_dsp_descr_t), 32);

  Sig_init();

  fts_s_dsp_descr = fts_new_symbol("__DSP_DESCR");

  /*  optimizer_init(); */

  /* Make the dsp off program  */

  dsp_make_dsp_off_chain();
}


/* This function should be moved to a tile, or to the runtime */

void
dsp_chain_poll(void)
{
  if (dsp_chain == dsp_chain_on)
    {
      if (please_zero_dsp_time)
	{
	  dsp_ms_clock = 0.0;
	  dsp_tick_clock = 0.0;

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

void
dsp_add_funcall(fts_symbol_t symb, int ac, fts_atom_t *av)
{
  ftl_program_add_call(dsp_chain_on, symb, ac, av);
}

void
dsp_add_signal(fts_symbol_t name, int vs)
{
  ftl_program_add_signal(dsp_chain_on, name, vs);
}

void
dsp_declare_function(fts_symbol_t name, void (*w)(fts_word_t *))
{
  ftl_declare_function( name, w);
}

void
dsp_chain_print(void)
{
  post("printing dsp chain:\n");
  ftl_program_print(dsp_chain);
}

void
dsp_chain_print_signals(void)
{
  post("printing signals:\n");
  ftl_program_print_signals_count(dsp_chain);
}

void
dsp_chain_generate_C_code(void)
{
  ftl_program_generate_C_code(dsp_chain);
}

void
dsp_chain_print_bytecode( void)
{
  ftl_program_print_bytecode( dsp_chain);
}

void
dsp_install_clocks(void)
{
  /* Install the timer */

  fts_clock_define_protected(fts_new_symbol("dsp_msec"),   &dsp_ms_clock);
  fts_clock_define_protected(fts_new_symbol("dsp_tick"), &dsp_tick_clock);
}


int
dsp_is_running( void)
{
  return dsp_chain == dsp_chain_on;
}



/* Never called ??? */

#ifdef DEBUG
static void
dsp_print_realize(dsp_signal **s, int n, const char *msg)
{
  int i;

  for (i = 0; i < n; i++, s++)
    {
      Sig_print(*s);
    }
}

static void
dsp_print(fts_object_t *o, fts_dsp_descr_t *descr)
{
  dsp_print_realize(descr->in, descr->ninputs, "input");
  dsp_print_realize(descr->out, descr->noutputs, "output");
}
#endif



