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

#include <fts/fts.h>
#include "sampbuf.h"
 
static fts_hashtable_t the_sampbuf_hashtable;

/* names and hash tables */

void
sampbuf_table_init(void)
{
  fts_hashtable_init(&the_sampbuf_hashtable, 0, FTS_HASHTABLE_MEDIUM);
}

sampbuf_t *
sampbuf_get(fts_symbol_t name)
{
  fts_atom_t data, k;
  
  if (! name)
    return(0);
  
  fts_set_symbol( &k, name);
  if(fts_hashtable_get( &the_sampbuf_hashtable, &k, &data))
    return (sampbuf_t *) fts_get_pointer(&data);
  else
    return 0;
}

int
sampbuf_name_already_registered(fts_symbol_t name)
{
  fts_atom_t data, k;

  fts_set_symbol( &k, name);
  return(fts_hashtable_get( &the_sampbuf_hashtable, &k, &data));
}

void
sampbuf_add(fts_symbol_t name, sampbuf_t *buf)
{
  fts_atom_t data, k;

  fts_set_symbol( &k, name);
  fts_set_pointer(&data, buf);
  fts_hashtable_put(&the_sampbuf_hashtable, &k, &data);
}

void
sampbuf_remove(fts_symbol_t name)
{
  fts_atom_t k;

  fts_set_symbol( &k, name);
  fts_hashtable_remove( &the_sampbuf_hashtable, &k);
}

/* data */

void
sampbuf_init(sampbuf_t *buf, long size)
{
  if(buf == 0)
    return;

  buf->samples = 0;
  buf->size = 0;
  buf->alloc_size = 0;

  sampbuf_realloc(buf, size);
}

void
sampbuf_realloc(sampbuf_t *buf, long size)
{
  int i;

  /* very lazy */
  if(size && size <= buf->alloc_size)
    buf->size = size;
  else
    {
      float *samples;

      samples = (float *)fts_malloc((size + GUARDPTS) * sizeof(float));

      for(i=0; i<size + GUARDPTS; i++)
	samples[i] = 0.0;
      
      if(buf->samples)
	{
	  for(i=0; i<buf->size; i++)
	    samples[i] = buf->samples[i];
	  
	  fts_free((void *)buf->samples);
	}
      
      buf->samples = samples;
      buf->size = size;
      buf->alloc_size = size;
    }
}

void
sampbuf_erase(sampbuf_t *buf)
{
  if(buf == 0)
    return;

  if(buf->samples)
      fts_free((void *)buf->samples);
      
  buf->samples = 0;
  buf->size = 0;
  buf->alloc_size = 0;
}

void
sampbuf_zero(sampbuf_t *buf)
{
  if(buf == 0)
    return;

  if(buf->samples)
      fts_free((void *)buf->samples);
      
  buf->samples = 0;
  buf->size = 0;
  buf->alloc_size = 0;
}

void
gensampname(char *buf, const char *base, long int n)
{
  char b2[30];
  char *s = b2+29;
  *s = 0;

  if (n < 0)
    n = 0;

  while ((*buf = *base++))
    buf++;

  if (!n)
    *(--s) = '0';
  else while (n)
    {
      *(--s) = '0' + (n%10);
      n /= 10;
    }
  while ((*buf++ = *s++))
    ;
}
