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
#ifndef _GPHITER_H_
#define _GPHITER_H_

typedef struct graph_iterator_t graph_iterator_t;

struct graph_iterator_t {
  struct stack_element_t *top;
};

void graph_iterator_init( graph_iterator_t *iter, fts_object_t *object, int outlet);

void graph_iterator_next( graph_iterator_t *iter);
int graph_iterator_end( const graph_iterator_t *iter);
void graph_iterator_get_current( const graph_iterator_t *iter, fts_object_t **obj, int *inlet);

#endif

