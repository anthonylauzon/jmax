#include <pthread.h>

/**************************************************
 *
 * Thread Manager
 *
 */

/**
 * Thread job function
 */
typedef struct fts_thread_function
{
    fts_object_t* object;
    fts_method_t method;
    fts_symbol_t symbol;
    int ac;
    fts_atom_t* at;
    int delay_ms; /* sleep delay in millisecond */

} fts_thread_function_t;


/**
 * thread structure
 */
typedef struct fts_thread_worker
{
    pthread_t thread;
    fts_thread_function_t* thread_function;
    int id;
} fts_thread_worker_t;

/** 
 * Create a thread with the given 
 */
FTS_API int fts_thread_manager_create_thread(fts_thread_worker_t* thread_worker);

/**
 * Stop this thread
 * Need a thread id
 */
FTS_API int fts_thread_manager_cancel_thread(fts_thread_worker_t* thread_worker);

/** 
 * @func fts_thread_mananger_start
 * Start the fts_thread_manager 
 * 
 * 
 * @return 0 if a fts_manager is running 
 * @return -1 if an error occured
 */

FTS_API int fts_thread_manager_start(void);


