#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <fts/fts.h>
#include <fts/thread.h>

#define DEFAULT_THREAD_MANAGER_FIFO_SIZE 40

#ifndef DEFAULT_SLEEP_SEC
#define DEFAULT_SLEEP_SEC 0
#endif /* DEFAULT_SLEEP_SEC */


#ifndef DEFAULT_SLEEP_NANOSEC 
#define DEFAULT_SLEEP_NANOSEC 50000000 /* 50 ms */
#endif /* DEFAULT_SLEEP_NANOSEC */

typedef struct _thread_manager_t
{
    fts_object_t head;
    pthread_t thread;
    int id;

    /* thread creation fifo */
    fts_fifo_t fifo;
    /* fifo buffer */
    void* buffer;
    /* fifo buffer size */
    int size;
    
    /* nanosleep structure */
    struct timespec* time_req;
    struct timespec* time_rem;

} thread_manager_t;

static thread_manager_t* fts_thread_manager = NULL;
static fts_symbol_t thread_manager_s_name;
static fts_symbol_t thread_manager_s_create_thread;
static fts_symbol_t thread_manager_s_start;

static fts_class_t* thread_manager_type = NULL;
/** 
 * External function to start fts_thread_manager 
 * 
 * 
 * @return 0 if a fts_manager is running 
 */
int fts_thread_manager_start(void)
{
    if (NULL == fts_thread_manager)
    {
	fts_thread_manager = (thread_manager_t*)fts_object_create(thread_manager_type, NULL, 0, 0);	
    }
    if (0 == fts_object_get_error((fts_object_t*)fts_thread_manager))
    {
	return 0;
    }
    else
    {
	return -1;
    }
}
/** 
 * External function to create a thread
 * 
 * @param thread_worker 
 * @param thread_id 
 * 
 * @return 
 */
int fts_thread_manager_create_thread(fts_thread_worker_t* thread_worker)
{
    thread_manager_t* manager = fts_thread_manager;
    int ac = 1;
    int stat = 0;
    fts_atom_t at;
    
    fts_set_pointer(&at, thread_worker); 
    fts_send_message((fts_object_t*)manager, thread_manager_s_create_thread, ac, &at);
    
    return stat;
}


/** 
 * External function to cancel a worker thread
 * 
 * @param thread_worker 
 * 
 * @return 
 */
int fts_thread_manager_cancel_thread(fts_thread_worker_t* thread_worker)
{
    int success;
    int thread_id = thread_worker->thread;
    success = pthread_cancel(thread_worker->thread);
    if (0 != success)
    {
	fprintf(stderr, "[fts_thread_manager_cancel] no such thread found\n");
    }
    post("[thred_manager] thread (%d) cancelled \n", thread_id);
    return success;
}



static void* thread_manager_run_thread(void* arg)
{
    fts_thread_function_t* thread_func = (fts_thread_function_t*)arg;
    fts_object_t* object = thread_func->object;
    fts_method_t method = thread_func->method;
    fts_symbol_t symbol = thread_func->symbol;
    int ac = thread_func->ac;
    fts_atom_t* at = thread_func->at;
    int delay_ms = thread_func->delay_ms;
    struct timespec time_req;
    struct timespec time_rem;
    time_req.tv_sec = 0;
    time_req.tv_nsec = delay_ms * 1000 * 1000;

    while(1)
    {
	method(object, fts_system_inlet, symbol, ac, at);
	nanosleep(&time_req, &time_rem);
    }

    return 0;
}


/** 
 * Main function of the thread manager
 * 
 * @param arg 
 * 
 * @return 
 */
void* thread_manager_main(void* arg)
{
    thread_manager_t* manager = (thread_manager_t*)arg;

    while(1)
    {
	/* Is there some data in FIFO */
	if (fts_fifo_read_level(&manager->fifo) >= sizeof(fts_atom_t))
	{
	    fts_atom_t* atom = (fts_atom_t*)fts_fifo_read_pointer(&manager->fifo);	    
	    fts_thread_worker_t* worker;

	    fts_fifo_incr_read(&manager->fifo, sizeof(fts_atom_t));
	    /*
	      if there is some data
	      call fts_thread_manager_new with all data
	    */
	    post("[thread_manager] read in fifo \n");
	    /* Check if atom is an object */
	    worker = (fts_thread_worker_t*)fts_get_pointer(atom);
	    
	    worker->id = pthread_create(&worker->thread,
					NULL,
					thread_manager_run_thread,
					(void*)worker->thread_function);
	    post("[thread_manager] start a new thread (%d] \n", worker->thread);
	}
	else
	{	
	    
	    /* post("[thread_manager] thread is running \n"); */
	    nanosleep(manager->time_req, manager->time_rem);
	}
    }
    
    return NULL;
}


static void
thread_manager_create_thread(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
    thread_manager_t* self = (thread_manager_t*)o;
    fts_atom_t* write_atom;

    /* Add command in fifo */
    /* make sure that there is enough space */
    if (fts_fifo_write_level(&self->fifo) < sizeof(fts_atom_t))
    {
	post("[thread_manager] not enough space in FIFO \n");
	return;
    }

    /* But worker in fifo */
    write_atom = (fts_atom_t*)fts_fifo_write_pointer(&self->fifo);
    fts_atom_assign(write_atom, at);
    fts_fifo_incr_write(&self->fifo, sizeof(fts_atom_t));

    post("[thread_manager] message add in fifo\n");

}


static void
thread_manager_print(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
    thread_manager_t* self = (thread_manager_t*)o;

    post("[thread_manager] debug info \n");
}
/* FOR DEBUGGING */
static void
thread_manager_start(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
    thread_manager_t* self = (thread_manager_t*)o;
    /* Time to create the thread */
    int success = pthread_create(&self->thread,
			     NULL,
			     thread_manager_main,
			     (void*)self);


    if (0 != success)
    {

	post("[thread_manager] error while creating thread \n");
	fts_log("[thread_manager] error while creating thread \n");
	return;
    }
    post("thread manager created (thread id : %d) \n", self->thread);


}
/** 
 * This function create the fts_thread_manager
 * 
 * 
 * @return 
 */
static void
thread_manager_init(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
    thread_manager_t* self = (thread_manager_t*)o;
    int bytes = 0;
    int size = DEFAULT_THREAD_MANAGER_FIFO_SIZE;
    int success = 0;
    fts_atom_t* atoms;
    int i;

    bytes = size * sizeof(fts_atom_t);

    self->size = size;
    self->buffer = fts_malloc(bytes);


    fts_fifo_init(&self->fifo, self->buffer, bytes);
    atoms = (fts_atom_t*)fts_fifo_get_buffer(&self->fifo);
    for (i = 0; i < size; ++i)
    {
	fts_set_void(atoms + i);
    }

    self->time_req = fts_malloc(sizeof(struct timespec));
    self->time_req->tv_sec = DEFAULT_SLEEP_SEC;
    self->time_req->tv_nsec = DEFAULT_SLEEP_NANOSEC;

    self->time_rem = fts_malloc(sizeof(struct timespec));

    /* Time to create the thread */
    success = pthread_create(&self->thread,
			     NULL,
			     thread_manager_main,
			     (void*)self);


    if (0 != success)
    {

	post("[thread_manager] error while creating thread \n");
	fts_log("[thread_manager] error while creating thread \n");
	fts_object_set_error(o,"cannot create thread \n");
	return;
    }
    post("thread manager created (thread id : %d) \n", self->thread);
    
}

/** 
 * This function delete tje fts_thread_manager
 * 
 * 
 * @return 
 */
static void
thread_manager_delete(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
    thread_manager_t* self = (thread_manager_t*)o;

    post("[thread manager] delete\n");
    fts_free(self->buffer);
}







static void
thread_manager_instantiate(fts_class_t* cl)
{
    fts_class_init(cl, sizeof(thread_manager_t), thread_manager_init, thread_manager_delete);

    fts_class_message_varargs(cl, fts_s_print, thread_manager_print);

    fts_class_message_varargs(cl, thread_manager_s_create_thread, thread_manager_create_thread);

    fts_class_message_varargs(cl, fts_s_start, thread_manager_start);
}


/**************************************************
 *
 * Initialization of the thread_manager module
 *
 */
void 
fts_thread_manager_config(void)
{
    thread_manager_s_name = fts_new_symbol("thread_manager");
    thread_manager_s_create_thread = fts_new_symbol("create_thread");
    thread_manager_s_start = fts_new_symbol("start");
    thread_manager_type = fts_class_install(thread_manager_s_name, thread_manager_instantiate);
}
