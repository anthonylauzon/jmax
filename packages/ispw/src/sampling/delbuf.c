#include "fts.h"
#include "delbuf.h"


/* clear delayline (fill with 0.0) */
void 
delbuf_clear_delayline(del_buf_t *buf)
{
  fts_vecx_fzero(buf->delay_line, buf->ring_size + buf->n_tick);
  buf->phase = buf->ring_size;
}

/* free delayline */
void 
delbuf_delete_delayline(del_buf_t *buf)
{
  if(buf->delay_line)
    fts_free((void *)(buf->delay_line)); 

  buf->delay_line = 0;
  buf->phase = 0;
  buf->ring_size = 0;
  buf->size = 0;
  buf->malloc_size = 0;
  buf->n_tick = 0;
}

del_buf_t * 
delbuf_new(float raw_size, fts_symbol_t unit)
{
  del_buf_t *buf;

  buf = (del_buf_t *)fts_malloc(sizeof(del_buf_t));

  if(!buf) return (0);

  buf->delay_line = 0;
  buf->phase = 0;
  buf->ring_size = 0;
  buf->size = 0;
  buf->raw_size = raw_size;
  buf->unit = unit;
  buf->malloc_size = 0;
  buf->n_tick = 0;
  buf->is_init = 0;

  return (buf);
}

/* init delbuf object (allocate delbuf if necsssary) */
int 
delbuf_init(del_buf_t *buf, float sr, long n_tick)
{
  long size, ring_size;

  if(!buf) return(0);
  
  size = (long)fts_unit_convert_to_base(buf->unit, buf->raw_size, &sr);
  if(size < n_tick) size = n_tick;
  
  /* check if new size matches old size */
  if(size > buf->malloc_size) /* need to reallocate */
    {
      if(buf->malloc_size != 0) 
	delbuf_delete_delayline(buf); /* delete to reallocate */  
      
      /* allocate delayline */

      ring_size = size + n_tick + VD_TAIL;
      ring_size += (-ring_size) & (n_tick - 1);
      buf->delay_line = (float *)fts_malloc((ring_size + n_tick) * sizeof(float));

      /* init delbuf with new delayline or exit */
      if(!buf->delay_line){
	buf->phase = 0;
	buf->ring_size = 0;
	buf->size = 0;
	buf->malloc_size = 0;
	buf->n_tick = 0;
	post("error: delay line: out of memory\n");
	return(0);
      }
      else{
	buf->ring_size = ring_size;
	buf->size = size;
	buf->malloc_size = size;
      }
    }
  else if(size < buf->malloc_size) /* no allocation */
    {
      ring_size = size + n_tick + VD_TAIL;
      ring_size += (-ring_size) & (n_tick - 1);

      buf->ring_size = ring_size;
      buf->size = size;
    }
    
  buf->n_tick = n_tick;
  delbuf_clear_delayline(buf);
  buf->is_init = 1;
  return(1);
}

int 
delbuf_is_init(del_buf_t *buf)
{
  return (buf->is_init);
}

void
delbuf_clear_is_init_flag(del_buf_t *buf)
{
  buf->is_init = 0;
}
