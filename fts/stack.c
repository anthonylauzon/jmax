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

#include <fts/fts.h>

void __fts_stack_init( fts_stack_t *s, int element_size)
{
  s->alloc = 256;
  s->element_size = element_size;
  s->buffer = fts_malloc( s->alloc * s->element_size);
  s->top = 0;
}

void fts_stack_destroy( fts_stack_t *s)
{
  fts_free( s->buffer);
}

int __fts_stack_realloc( fts_stack_t *s)
{
  s->alloc *= 2;
  s->buffer = fts_realloc( s->buffer, s->alloc * s->element_size);

  return 0;
}
