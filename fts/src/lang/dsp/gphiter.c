#include "sys.h"
#include "lang/mess.h"
#include "gphiter.h"
#include <stdio.h>

#define ASSERT(e) if (!(e)) { fprintf( stderr, "Assertion (%s) failed file %s line %d\n",#e,__FILE__,__LINE__); *(char *)0 = 0;}


typedef struct stack_element_t stack_element_t;

struct stack_element_t {
  fts_object_t *object;
  fts_connection_t *connection;
  stack_element_t *next;
};

static fts_heap_t stack_element_heap;

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
      fts_heap_init( &stack_element_heap, sizeof( stack_element_t), 64);
      inited = 1;
    }

  new_elem = (stack_element_t *)fts_heap_zalloc( &stack_element_heap);

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

static void graph_iterator_handle_special_object( fts_connection_t *connection, fts_object_t **new_object, int *outlet)
{
  fts_object_t *special_object;

  special_object = connection->dst;
  if (fts_object_is_patcher( special_object))
    {
      *new_object = fts_patcher_get_inlet( special_object, connection->winlet);
      *outlet = 0;
    }
  else if (fts_object_is_outlet( special_object))
    {
      *new_object = (fts_object_t *)fts_object_get_patcher( special_object);
      *outlet = ((fts_outlet_t *)special_object)->position;
    }
}

static void graph_iterator_pop( graph_iterator_t *iter)
{
  stack_element_t *next;

  next = iter->top->next;

  fts_heap_free( (char *)iter->top, &stack_element_heap);

  iter->top = next;
}

static int is_special_object( fts_object_t *obj)
{
  return fts_object_is_patcher(obj) 
    || fts_object_is_outlet(obj);
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
  else if ( is_special_object( iter->top->connection->dst))
    {
      fts_object_t *replacement_object;
      int outlet;

      graph_iterator_handle_special_object( iter->top->connection, &replacement_object, &outlet);

      if ( !replacement_object)
	return;

      iter->top->connection = iter->top->connection->next_same_src;

      if (!graph_iterator_is_object_already_in_stack( iter, replacement_object))
	graph_iterator_push( iter, replacement_object, outlet);

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


