#include <pthread.h>
#include <fts/fts.h>

/**************************************************
 *
 * Thread Manager
 *
 */

/**
 * Thread job function
 */
typedef void* (*fts_thread_function_t)(void*);


/**
 * thread structure
 */
typedef struct fts_thread_worker
{
    fts_fifo_t fifo;
    pthread_t thread;
    fts_thread_function_t thread_function;
} fts_thread_worker_t;

/** 
 * Create a thread with the given 
 */
FTS_API int fts_thread_manager_create(fts_thread_worker_t* thread_worker,
				      void* arg,
				      int* thread_id);

/**
 * Stop this thread
 * Need a thread id
 */
FTS_API int fts_thread_manager_cancel(fts_thread_worker_t* thread_worker);
