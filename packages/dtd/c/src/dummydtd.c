/***
 * threadsf~
 *
 */
#include <unistd.h>
#include <pthread.h>
#include <fts/fts.h>
#include <fts/thread.h>

static fts_symbol_t threadsf_symbol;
static fts_symbol_t dummy_symbol;
static fts_symbol_t foo_symbol;
static fts_symbol_t post_symbol;
fts_metaclass_t* dummy_type;

typedef struct
{
    fts_object_t head;
    int thread_id;
    fts_thread_worker_t* thread_worker;
} threadsf_t;


typedef struct
{
    fts_object_t head;
    int foo_val;
} dummy_t;


static void
dummy_foo(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
    dummy_t* self = (dummy_t*)o;
    
    self->foo_val++;
}


static void
dummy_post(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
    dummy_t* self = (dummy_t*)o;
    
    post("[dummy] foo_val = %d\n", self->foo_val);
}

static void
dummy_init(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
    dummy_t* self = (dummy_t*)o;

    self->foo_val = 0;
}

static void
dummy_delete(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
    dummy_t* self = (dummy_t*)o;

    self->foo_val = 0;
}

static fts_status_t
dummy_instantiate(fts_class_t* cl, int ac, const fts_atom_t* at)
{
    /* 0 inlet and 0 outlet */
    fts_class_init(cl, sizeof(dummy_t), 1,0, 0);
    
    fts_method_define_varargs(cl ,fts_system_inlet, fts_s_init, dummy_init);
    fts_method_define_varargs(cl, fts_system_inlet, fts_s_delete, dummy_delete);
    fts_method_define_varargs(cl, 0, foo_symbol, dummy_foo);
    fts_method_define_varargs(cl, 0, post_symbol, dummy_post);
    return fts_ok;
}


static void*
dummy_call_post_func(void* arg)
{
    int ac = 0;
    fts_atom_t* at = NULL;
    fts_object_t* o = (fts_object_t*)arg;
    fts_method_t meth = fts_class_get_method(fts_object_get_class(o),
					     0,
					     post_symbol);
    while(1)
    {
	meth(o, fts_system_inlet, post_symbol, ac, at);
	sleep(1);
    }
    return 0;
}


static void*
sleeping_func(void* arg)
{
    int i;
    while(1)
    {
	post("[threadsf] sleeping ... \n");
	sleep(1);
    }
    return 0;
}

static void
threadsf_init(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
    threadsf_t* self = (threadsf_t*)o;
    dummy_t* dummy_object = (dummy_t*)fts_object_create(dummy_type, 0, 0);

    self->thread_worker = fts_malloc(sizeof(fts_thread_worker_t));
    self->thread_worker->thread_function = dummy_call_post_func;
    post("[threadsf] create thread \n");
    fts_thread_manager_create(self->thread_worker, (void*)dummy_object, &self->thread_id);
}

static void
threadsf_delete(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
    threadsf_t* self = (threadsf_t*)o;
    /* Cancel thread */
    post("[threadsf] cancel thread\n");
    fts_thread_manager_cancel(self->thread_worker);

    /* Memory deallocation */
    fts_free(self->thread_worker);
}

static fts_status_t
threadsf_instantiate(fts_class_t* cl, int ac, const fts_atom_t* at)
{
    /* 0 inlet and 0 outlet */
    fts_class_init(cl, sizeof(threadsf_t), 0,0, 0);
    
    fts_method_define_varargs(cl ,fts_system_inlet, fts_s_init, threadsf_init);
    fts_method_define_varargs(cl, fts_system_inlet, fts_s_delete, threadsf_delete);

    return fts_ok;
}


void dummydtd_config(void)
{
    threadsf_symbol = fts_new_symbol("threadsf");
    dummy_symbol = fts_new_symbol("dummy");
    foo_symbol = fts_new_symbol("foo");
    post_symbol = fts_new_symbol("post");
    fts_class_install(threadsf_symbol, threadsf_instantiate);
    dummy_type = fts_class_install(dummy_symbol, dummy_instantiate);
}
