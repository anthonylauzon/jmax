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
  sampbuf_t *buf;
  
  if(!name) return(0);
  
  if(fts_hash_table_lookup(&the_sampbuf_hashtable, name, (void **)&buf)){
    return(buf);
  }else{
    /* no error message here, it may be called just to be informed !! */
    return(0);
  }
}

int
sampbuf_name_already_registered(fts_symbol_t name)
{
  sampbuf_t *buf;
  return(fts_hash_table_lookup(&the_sampbuf_hashtable, name, (void **)&buf));
}

void
sampbuf_add(fts_symbol_t name, sampbuf_t *buf)
{
  fts_hash_table_insert(&the_sampbuf_hashtable, name, (void *)buf);
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
