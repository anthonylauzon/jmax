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

#ifndef _FTS_GPHITER_H_
#define _FTS_GPHITER_H_

typedef struct graph_iterator graph_iterator_t;

struct graph_iterator {
  struct stack_element *top;
};

typedef void (*graph_iterator_connection_function_t)(fts_connection_t *connection, void *arg);

extern void graph_iterator_init( graph_iterator_t *iter, fts_object_t *object, int outlet);

extern void graph_iterator_next( graph_iterator_t *iter);
extern int graph_iterator_end( const graph_iterator_t *iter);
extern void graph_iterator_get_current( const graph_iterator_t *iter, fts_object_t **obj, int *inlet);

extern fts_connection_t *graph_iterator_get_current_connection(const graph_iterator_t *iter);
extern int graph_iterator_has_connection_stack(const graph_iterator_t *iter);
extern void graph_iterator_apply_to_connection_stack(const graph_iterator_t *iter, graph_iterator_connection_function_t fun, void *arg);

#endif
