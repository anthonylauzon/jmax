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

#include "sys.h"
#include "lang/mess.h"
#include "gphiter.h"

extern fts_symbol_t fts_s_propagate_input;

#define ASSERT(e) if (!(e)) { fprintf( stderr, "Assertion (%s) failed file %s line %d\n",#e,__FILE__,__LINE__); *(char *)0 = 0;}

typedef struct stack_element stack_element_t;

struct stack_element {
  fts_object_t *object;
  fts_connection_t *remember;
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

static void 
dump_stack( stack_element_t *elem, int depth)
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

static void 
dump_iter( graph_iterator_t *iter, char *msg)
{
  fprintf( stderr, "(%s) dumping stack\n", msg);

  dump_stack( iter->top, 0);
}


/* --------------------------------------------------------------------------- */
/*                                                                             */
/* Local functions for stack handling and graph recursive traversal            */
/*                                                                             */
/* --------------------------------------------------------------------------- */

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
  new->remember = 0;

  new->next = iter->top;

  iter->top = new;
}

static void 
graph_iterator_pop( graph_iterator_t *iter)
{
  stack_element_t *next;

  next = iter->top->next;

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
      stack_element_t *oldtop = iter->top;
      fts_object_t *dest = iter->top->connection->dst;
      fts_atom_t a[3];
      fts_status_t stat;

      /* try to get successor of eventual "thru" object */
      fts_set_fun(a + 0, (fts_fun_t)graph_iterator_push);
      fts_set_ptr(a + 1, iter);
      fts_set_int(a + 2, iter->top->connection->winlet);
      stat = fts_message_send(dest, fts_SystemInlet, fts_s_propagate_input, 2, a);

      if(stat == fts_Success)
	{
	  /* skip "thru" object */
	  oldtop->remember = oldtop->connection;
	  oldtop->connection = oldtop->connection->next_same_src;	  
	  graph_iterator_step( iter);
	}
    }
}

/* --------------------------------------------------------------------------- */
/*                                                                             */
/* Exported functions                                                          */
/*                                                                             */
/* --------------------------------------------------------------------------- */

/* Initialize a graph iterator on the connections of outlet number `outlet' of object `obj' */
void 
graph_iterator_init( graph_iterator_t *iter, fts_object_t *object, int outlet)
{
  iter->top = 0;

  graph_iterator_push( iter, object, outlet);
  graph_iterator_step( iter);
}

/* Advance to next object */
void 
graph_iterator_next( graph_iterator_t *iter)
{
  ASSERT( iter->top != 0);

  if (iter->top->connection)
    iter->top->connection = iter->top->connection->next_same_src;

  graph_iterator_step( iter);
}


/* Is it finished ? */
int 
graph_iterator_end( const graph_iterator_t *iter)
{
  return (iter->top == 0);
}

/* Returns the destination object and the inlet number of current connection */
void 
graph_iterator_get_current( const graph_iterator_t *iter, fts_object_t **obj, int *inlet)
{
  if (iter->top->connection)
    {
      *obj = iter->top->connection->dst;
      *inlet = iter->top->connection->winlet;
    }
}

int
graph_iterator_has_connection_stack(const graph_iterator_t *iter)
{
  return (iter->top != 0);
}

fts_connection_t *
graph_iterator_get_current_connection(const graph_iterator_t *iter)
{
  return iter->top->connection;
}

void 
graph_iterator_apply_to_connection_stack(const graph_iterator_t *iter, graph_iterator_connection_function_t fun, void *arg)
{
  stack_element_t *elem = iter->top;

  while(elem)
    {
      if(elem->remember)
	fun(elem->remember, arg);

      elem = elem->next;
    }
}
