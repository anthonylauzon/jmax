#include <stdlib.h>
#include <fts/fts.h>
#include "audp_fifo.h"


int audp_fifo_init(audp_fifo_t *fifo, unsigned long size, unsigned long low, unsigned long high) {
  fifo->size = size;
  if ((fifo->buf = fts_malloc(size)) == NULL)
    return -1;

  fifo->begin = fifo->end = 0;
  fifo->low = low;
  fifo->high = high;

  return 0;
}


int audp_fifo_write(audp_fifo_t *fifo, unsigned long size, void *buf) {
  unsigned long fifo_fill = fifo->end - fifo->begin;

  if (fifo_fill + size > fifo->size) {
    /* not enough space */
    return -1;
  }
  
  if (fifo->end + size > fifo->size) {
    /* we have to relocate fifo data at begining */
    unsigned long i,j;

    for (i=fifo->begin,j=0; j<fifo_fill; i++, j++)
      fifo->buf[j] = fifo->buf[i];
    
    fifo->begin = 0;
    fifo->end = fifo_fill;
  }

  memcpy(fifo->buf + fifo->end, buf, size);
  fifo->end += size;

  return size;
}

int audp_fifo_read(audp_fifo_t *fifo, unsigned long size, void *buf) {
  if (fifo->end - fifo->begin < size) 
    return -1;
  
  memcpy(buf, fifo->buf + fifo->begin, size);
  fifo->begin += size;
  return size;
}
  


    
