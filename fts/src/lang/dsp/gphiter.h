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

typedef struct graph_iterator_t graph_iterator_t;

struct graph_iterator_t {
  struct stack_element_t *top;
};

void graph_iterator_init( graph_iterator_t *iter, fts_object_t *object, int outlet);

void graph_iterator_next( graph_iterator_t *iter);
int graph_iterator_end( const graph_iterator_t *iter);
void graph_iterator_get_current( const graph_iterator_t *iter, fts_object_t **obj, int *inlet);

#endif

