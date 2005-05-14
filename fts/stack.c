/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#include <fts/fts.h>

#define FTS_STACK_BLOCK_SIZE 256

void 
__fts_stack_init( fts_stack_t *s, int element_size)
{
  s->alloc = FTS_STACK_BLOCK_SIZE;
  s->element_size = element_size;
  s->buffer = fts_malloc( s->alloc * s->element_size);
  s->size = 0;
}

void 
fts_stack_destroy( fts_stack_t *stack)
{
  fts_free( stack->buffer);
}

int 
__fts_stack_realloc( fts_stack_t *stack, int alloc)
{
  if(alloc > stack->alloc)
  {
    stack->buffer = fts_realloc( stack->buffer, alloc * stack->element_size);
    stack->alloc = alloc;
    
    return 1;
  }
  
  return 0;
}

int
__fts_stack_append(fts_stack_t *stack, void *p, int n)
{
  int alloc = stack->alloc;
  int size = stack->size + n;
  
  if(alloc < size)
  {
    do 
      alloc += FTS_STACK_BLOCK_SIZE;
    while(alloc < size);

    stack->buffer = fts_realloc( stack->buffer, alloc * stack->element_size);
    stack->alloc = alloc;
  }

  memcpy(stack->buffer + stack->size * stack->element_size, p, n * stack->element_size);
  
  stack->size = size;
  
  return size;
}

void 
fts_string_init(fts_stack_t *stack, char *str)
{
  stack->alloc = FTS_STACK_BLOCK_SIZE;
  stack->element_size = sizeof(char);
  stack->buffer = fts_malloc(stack->alloc);
  
  if(str != NULL)
    __fts_stack_append(stack, str, strlen(str) + 1);
  else
  {
    stack->size = 1;
    ((char *)stack->buffer)[0] = '\0';
  }
}
