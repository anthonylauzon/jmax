#include <pthread.h>

/**************************************************
 *
 * Thread Manager
 *
 */
typedef struct _thread_manager_t
{
    fts_object_t head;
    unsigned long int thread_manager_ID;
    int id;

    /* thread creation fifo */
    fts_fifo_t create_fifo;
    /* fifo buffer */
    void* create_buffer;
    /* fifo buffer size */
    int create_size;

    /* thread cancellation fifo */
    fts_fifo_t cancel_fifo;
    /* fifo buffer */
    void* cancel_buffer;
    /* fifo buffer size */
    int cancel_size;
    
    int delay_ms; /* sleep delay in millisecond */
} thread_manager_t;


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
    int is_dead; /* flag to know if this thread will die */
} fts_thread_function_t;


/**
 * thread structure
 */
typedef struct fts_thread_worker
{
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


/* ************************************************** */
/*                                                    */
/* Platform dependant function                        */
/*                                                    */
/* ************************************************** */
FTS_API int thread_manager_start(thread_manager_t* self);
FTS_API int thread_manager_cancel_thread(fts_thread_worker_t* thread_worker);
FTS_API void* thread_manager_run_thread(void* arg);
FTS_API void* thread_manager_main(void* arg);

