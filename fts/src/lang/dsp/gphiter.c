#include "sys.h"
#include "lang/mess.h"
#include "gphiter.h"


#define ASSERT(e) if (!(e)) { fprintf( stderr, "Assertion (%s) failed file %s line %d\n",#e,__FILE__,__LINE__); *(char *)0 = 0;}


typedef struct stack_element_t stack_element_t;

struct stack_element_t {
  fts_object_t *object;
  fts_connection_t *connection;
  stack_element_t *next;
};

static fts_heap_t *stack_element_heap;

static int inited = 0;


/* --------------------------------------------------------------------------- */
/*                                                                             */
/* Debug code                                                                  */
/*                                                                             */
/* --------------------------------------------------------------------------- */

static void dump_stack( stack_element_t *elem, int depth)
{
  if (elem)
    {
      fprintf( stderr, "[%3d] %s -> (%s,%d)\n",
	       depth,
	       fts_symbol_name(fts_object_get_class_name( elem->object)),
	       elem->connection ? fts_symbol_name(fts_object_get_class_name(elem->connection->dst)) : "null",
	       elem->connection ? elem->connection->winlet : -1);
      dump_stack( elem->next, depth+1);
    }
}

static void dump_iter( graph_iterator_t *iter, char *msg)
{
  fprintf( stderr, "(%s) dumping stack\n", msg);

  dump_stack( iter->top, 0);
}


/* --------------------------------------------------------------------------- */
/*                                                                             */
/* Local functions for stack handling and graph recursive traversal            */
/*                                                                             */
/* --------------------------------------------------------------------------- */

static void graph_iterator_push( graph_iterator_t *iter, fts_object_t *object, int outlet)
{
  stack_element_t *new_elem;

  if (!inited)
    {
      stack_element_heap =  fts_heap_new(sizeof( stack_element_t));
      inited = 1;
    }

  new_elem = (stack_element_t *)fts_heap_zalloc(stack_element_heap);

  new_elem->object = object;
  new_elem->connection = object->out_conn[outlet];
  new_elem->next = iter->top;

  iter->top = new_elem;
}

static int graph_iterator_is_object_already_in_stack( graph_iterator_t *iter, fts_object_t *object)
{
  stack_element_t *elem;

  for ( elem = iter->top; elem; elem = elem->next)
    if (object == elem->object)
      return 1;

  return 0;
}

static void handle_connection_to_system_object( graph_iterator_t *iter, fts_connection_t *connection)
{
  fts_object_t *system_object, *newobject;

  system_object = connection->dst;

  if (fts_object_is_patcher( system_object))
    {
      newobject = fts_patcher_get_inlet( system_object, connection->winlet);

      while (newobject)
	{
	  if (!graph_iterator_is_object_already_in_stack( iter, newobject))
	    graph_iterator_push( iter, newobject, 0);

	  newobject = fts_inlet_get_next_inlet(newobject);
	}
    }
  else if (fts_object_is_outlet( system_object))
    {
      newobject = (fts_object_t *)fts_object_get_patcher( system_object);

      if (!graph_iterator_is_object_already_in_stack( iter, newobject))
	graph_iterator_push( iter, newobject, ((fts_outlet_t *)system_object)->position);
    }
  else if (fts_object_is_send( system_object))
    {
      fts_object_t *rcvobject;

      rcvobject = fts_send_get_first_receive( system_object);
      while ( rcvobject)
	{
	  if (!graph_iterator_is_object_already_in_stack( iter, rcvobject))
	    graph_iterator_push( iter, rcvobject, 0);

	  rcvobject = fts_receive_get_next_receive( rcvobject);
	}
    }
}

static void graph_iterator_pop( graph_iterator_t *iter)
{
  stack_element_t *next;

  next = iter->top->next;

  fts_heap_free( (char *)iter->top, stack_element_heap);

  iter->top = next;
}

static int is_system_object( fts_object_t *obj)
{
  return fts_object_is_patcher(obj) 
    || fts_object_is_outlet(obj)
    || fts_object_is_send(obj);
}

static void graph_iterator_step( graph_iterator_t *iter)
{
  if (!iter->top)
    return;

  if ( !iter->top->connection)
    {
      graph_iterator_pop( iter);

      graph_iterator_step( iter);
    }
  else if ( is_system_object( iter->top->connection->dst))
    {
      stack_element_t *oldtop;

      oldtop = iter->top;

      handle_connection_to_system_object( iter, iter->top->connection);

      /* Advance to skip system object */
      oldtop->connection = oldtop->connection->next_same_src;

      graph_iterator_step( iter);
    }
}


/* --------------------------------------------------------------------------- */
/*                                                                             */
/* Exported functions                                                          */
/*                                                                             */
/* --------------------------------------------------------------------------- */

/* Initialize a graph iterator on the connections of outlet number `outlet' of object `obj' */
void graph_iterator_init( graph_iterator_t *iter, fts_object_t *object, int outlet)
{
  iter->top = 0;

  graph_iterator_push( iter, object, outlet);

  graph_iterator_step( iter);
}

/* Advance to next object */
void graph_iterator_next( graph_iterator_t *iter)
{
  ASSERT( iter->top != 0);

  if (iter->top->connection)
    iter->top->connection = iter->top->connection->next_same_src;

  graph_iterator_step( iter);
}


/* Is it finished ? */
int graph_iterator_end( const graph_iterator_t *iter)
{
  return (iter->top == 0);
}

/* Returns the destination object and the inlet number of current connection */
void graph_iterator_get_current( const graph_iterator_t *iter, fts_object_t **obj, int *inlet)
{
  if (iter->top->connection)
    {
      *obj = iter->top->connection->dst;
      *inlet = iter->top->connection->winlet;
    }
}


