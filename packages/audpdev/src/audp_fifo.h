
typedef struct audp_fifo {
  char *buf;
  unsigned long size;
  unsigned long low, high;
  unsigned long begin, end;
} audp_fifo_t;


int audp_fifo_init(audp_fifo_t *fifo, unsigned long size, unsigned long low, unsigned long end);

int audp_fifo_write(audp_fifo_t *fifo, unsigned long size, void *buf);

int audp_fifo_read(audp_fifo_t *fifo, unsigned long size, void *buf);

