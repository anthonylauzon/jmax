#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <fts/fts.h>
#include <fts/thread.h>

#define DEFAULT_THREAD_MANAGER_FIFO_SIZE 40

#ifndef DEFAULT_SLEEP_MS
#define DEFAULT_SLEEP_MS 50 /* 50 ms */
#endif /* DEFAULT_SLEEP_MS */


static thread_manager_t* fts_thread_manager = NULL;
static fts_symbol_t thread_manager_s_name;

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
      fts_thread_manager = (thread_manager_t*)fts_object_create(thread_manager_type, NULL, 0, 0);	

    if (fts_thread_manager != NULL)
      return 0;
    else
      return -1;
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
    thread_manager_t* self = fts_thread_manager;
    int stat = 0;
    fts_atom_t at;
    fts_atom_t* write_atom;

    /* tell thread that it is not dead */
    thread_worker->thread_function->is_dead = 0;
    fts_set_pointer(&at, thread_worker); 

    /* Add command in fifo */
    /* make sure that there is enough space */
    if (fts_fifo_write_level(&self->create_fifo) < sizeof(fts_atom_t))
    {
	post("[thread_manager] not enough space in create FIFO \n");
	return -1;
    }

    /* Put worker in fifo */
    write_atom = (fts_atom_t*)fts_fifo_write_pointer(&self->create_fifo);
    fts_atom_assign(write_atom, &at);
    fts_fifo_incr_write(&self->create_fifo, sizeof(fts_atom_t));

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
    thread_manager_t* self = fts_thread_manager;
    int stat = 0;
    fts_atom_t at;
    fts_atom_t* write_atom;

    /* tell thread that it is not dead */
    thread_worker->thread_function->is_dead = 1;
    fts_set_pointer(&at, thread_worker); 

    /* Add command in fifo */
    /* make sure that there is enough space */
    if (fts_fifo_write_level(&self->cancel_fifo) < sizeof(fts_atom_t))
    {
	post("[thread_manager] not enough space in cancel FIFO \n");
	return -1;
    }

    /* Put worker in fifo */
    write_atom = (fts_atom_t*)fts_fifo_write_pointer(&self->cancel_fifo);
    fts_atom_assign(write_atom, &at);
    fts_fifo_incr_write(&self->cancel_fifo, sizeof(fts_atom_t));

    return stat;

}


/**
 * FOR DEBUGGING 
 */
static void
thread_manager_print(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
    post("[thread_manager] debug info \n");
}


/** 
 * This function create the fts_thread_manager
 * 
 * 
 * @return 
 */
static void
fts_thread_manager_init(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
    thread_manager_t* self = (thread_manager_t*)o;
    int bytes = 0;
    int size = DEFAULT_THREAD_MANAGER_FIFO_SIZE;
    int success = 0;
    fts_atom_t* atoms;
    int i;

    bytes = size * sizeof(fts_atom_t);


    /* create thread FIFO */
    self->create_size = size;
    self->create_buffer = fts_malloc(bytes);
    
    fts_fifo_init(&self->create_fifo, self->create_buffer, bytes);
    atoms = (fts_atom_t*)fts_fifo_get_buffer(&self->create_fifo);
    for (i = 0; i < size; ++i)
    {
	fts_set_void(atoms + i);
    }

    /* cancel thread FIFO */
    self->cancel_size = size;
    self->cancel_buffer = fts_malloc(bytes);


    fts_fifo_init(&self->cancel_fifo, self->cancel_buffer, bytes);
    atoms = (fts_atom_t*)fts_fifo_get_buffer(&self->cancel_fifo);
    for (i = 0; i < size; ++i)
    {
	fts_set_void(atoms + i);
    }


    self->delay_ms = DEFAULT_SLEEP_MS;

    success = thread_manager_start(self);

    if (0 != success)
    {
	post("[thread_manager] error while creating thread manager \n");
	fts_log("[thread_manager] error while creating thread manager \n");
	fts_object_set_error(o,"cannot create thread manager \n");
	return;
    }
}

/** 
 * fts_thread_manager destructor
 * 
 * 
 * @return 
 */
static void
fts_thread_manager_delete(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
    thread_manager_t* self = (thread_manager_t*)o;

    fts_free(self->create_buffer);
    fts_free(self->cancel_buffer);
}


static void
thread_manager_instantiate(fts_class_t* cl)
{
    fts_class_init(cl, sizeof(thread_manager_t), fts_thread_manager_init, fts_thread_manager_delete);

    fts_class_message_varargs(cl, fts_s_print, thread_manager_print);
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
    thread_manager_type = fts_class_install(thread_manager_s_name, thread_manager_instantiate);
}
