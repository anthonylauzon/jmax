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
#include "fts.h"
#include "sampbuf.h"
 
static fts_hash_table_t the_sampbuf_hashtable;

/* names and hash tables */

void
sampbuf_table_init(void)
{
  fts_hash_table_init(&the_sampbuf_hashtable);
}

sampbuf_t *
sampbuf_get(fts_symbol_t name)
{
  fts_atom_t data;
  
  if (! name)
    return(0);
  
  if(fts_hash_table_lookup(&the_sampbuf_hashtable, name, &data))
    return (sampbuf_t *) fts_get_ptr(&data);
  else
    return 0;
}

int
sampbuf_name_already_registered(fts_symbol_t name)
{
  fts_atom_t data;
  return(fts_hash_table_lookup(&the_sampbuf_hashtable, name, &data));
}

void
sampbuf_add(fts_symbol_t name, sampbuf_t *buf)
{
  fts_atom_t data;

  fts_set_ptr(&data, buf);
  fts_hash_table_insert(&the_sampbuf_hashtable, name, &data);
}

void
sampbuf_remove(fts_symbol_t name)
{
  fts_hash_table_remove(&the_sampbuf_hashtable, name);
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
  /* very lazy */
  if(size <= buf->alloc_size)
    buf->size = size;
  else
    {
      float *samples;

      samples = (float *)fts_malloc((size + GUARDPTS) * sizeof(float));
      fts_vec_fzero(samples, size + GUARDPTS);
      
      if(buf->samples)
	{
	  fts_vec_fcpy(buf->samples, samples, buf->size);
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
