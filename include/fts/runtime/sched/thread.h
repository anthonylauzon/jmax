
typedef struct {
  fts_object_t head;
  fts_fifo_t fifo;

#ifdef HAS_POSIX_THREADS
  pthread_t thread;
#endif
} fts_thread_t;


