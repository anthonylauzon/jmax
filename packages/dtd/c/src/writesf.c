/***
 * threadsf~
 *
 */
#include <fts/fts.h>
#include <fts/thread.h>

#include "dtd_buffer.h"

#define DEFAULT_DELAY_MS 50

/* #define MY_DEBUG 1 */

fts_metaclass_t* writesf_thread_type;
static fts_symbol_t writesf_thread_symbol;

static fts_symbol_t s_write;

/**************************************************
 *
 * writesf_thread
 *
 */
typedef struct
{
    fts_object_t head;
    /* sound file to write */
    fts_audiofile_t* sf;

/*     /\* number of frames write *\/ */
/*     int nframes_write; */
    /* communication buffer */
    dtd_buffer_t* com_buffer;

    /* 
       Index of buffer write by FTS
    */
    const int* const buffer_index;  /* 0: fisrt buffer
				       1: second buffer
				    */
    int delay_ms;
} writesf_thread_t;


/* 
   !!!! No return because must be in a infinite thread !!!
*/
static void
writesf_thread_write(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
    writesf_thread_t* self = (writesf_thread_t*)o;
    int i;
    dtd_buffer_t* com_buffer;
    float** buffer;
    /* index to buffer to write */
    /* bufer_index is buffer currently write by fts */    
    int buffer_to_write;
    int size = 0;

    buffer_to_write = (*self->buffer_index + 1) % 2;
    com_buffer = &self->com_buffer[buffer_to_write];

    if (com_buffer->full == 1)
    {
	size = fts_audiofile_write(self->sf, 
				   com_buffer->buffer, 
				   com_buffer->n_channels, 
				   com_buffer->end_index);
#ifdef MY_DEBUG	
	post("[writesf_thread] empty %d buffer ... \n", buffer_to_write);
	fts_log("[writesf_thread] empty %d buffer ... \n", buffer_to_write);
#endif /* MY_DEBUG */
	com_buffer->full = 0;
    }

}


static void
writesf_thread_init(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
    writesf_thread_t* self = (writesf_thread_t*)o;

    /* sound file to write */
    self->sf = 0;
/*     /\* number of frames write *\/ */
/*     self->nframes_write = 0; */

    self->delay_ms = DEFAULT_DELAY_MS;
}

static void
writesf_thread_delete(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
    writesf_thread_t* self = (writesf_thread_t*)o;
    self->sf = 0;
}

static fts_status_t
writesf_thread_instantiate(fts_class_t* cl, int ac, const fts_atom_t* at)
{
    fts_class_init(cl, sizeof(writesf_thread_t), 1, 0, 0);

    fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, writesf_thread_init);
    fts_method_define_varargs(cl, fts_system_inlet, fts_s_delete, writesf_thread_delete);

    fts_method_define_varargs(cl, 0, s_write, writesf_thread_write);

    return fts_ok;
}

/**************************************************
 *
 * writesf~
 *
 */
typedef struct
{
    fts_object_t head;
    int n_channels;
    fts_symbol_t filename;

    /* sound file to write */
    fts_audiofile_t* sf;

    /* communication buffer */
    dtd_buffer_t* com_buffer;

    int buffer_index;
    int write_index;
    int is_open;

    fts_thread_worker_t* thread_worker;
} writesf_t;

static fts_symbol_t writesf_symbol;

static fts_symbol_t s_open;
static fts_symbol_t s_close;
static fts_symbol_t s_play;
static fts_symbol_t s_record;
static fts_symbol_t s_pause;

/* forward declaration */
static void writesf_close(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);


static void writesf_dsp( fts_word_t *argv)
{
  writesf_t *self = (writesf_t *)fts_word_get_pointer( argv + 0);
  int n_tick = fts_word_get_int(argv + 1);
  float* in = (float*)fts_word_get_pointer(argv + 2);
  int n;
  dtd_buffer_t* com_buffer;
  float** buffer;
  int* full_flag;

  int buffer_index;

  buffer_index = self->buffer_index;
  com_buffer = &self->com_buffer[buffer_index];
  buffer = com_buffer->buffer;

  /* Write from buffer */
  for (n = 0; n < n_tick; ++n)
  {
      buffer[0][n + self->write_index] = in[n];
  }
  self->write_index += n_tick;

/* #ifdef MY_DEBUG */
/*   fts_log("writeing buffer %d \n", self->buffer_index); */
/* #endif /\* MY_DEBUG *\/ */

  if ((self->write_index + n_tick) > com_buffer->size)
  {
      com_buffer->full = 1;
      com_buffer->end_index = self->write_index;
#ifdef MY_DEBUG
      post("buffer (%d) is full \n", buffer_index);
      fts_log("buffer (%d) is full \n", buffer_index);
#endif /* MY_DEBUG */
      self->buffer_index += 1;
      self->buffer_index %= 2;

      self->write_index = 0;
#ifdef MY_DEBUG
      post("buffer (%d) is going to be filled \n", self->buffer_index);
      fts_log("buffer (%d) is going to be filled \n", self->buffer_index);
#endif /* MY_DEBUG */      
  }
}

static void writesf_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  writesf_t *self = (writesf_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  fts_atom_t argv[32];
  int i;

  fts_set_pointer( argv + 0, self);
  fts_set_int( argv + 1, fts_dsp_get_input_size( dsp, 0));

  for ( i = 0; i < self->n_channels; i++)
    fts_set_symbol( argv + 2 + i, fts_dsp_get_input_name( dsp, i));

  fts_dsp_add_function( writesf_symbol, 2 + self->n_channels, argv);
}


static void writesf_open(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
    writesf_t* self = (writesf_t*)o;


    fts_audiofile_t* sf;
    int size = 0;

    if (1 == self->is_open)
    {
	/* call close */
	writesf_close(o, winlet, s, ac, at);
    }
    if (ac > 0)
    {
	self->filename = fts_get_symbol(at);
	    
	sf = fts_audiofile_open_write(self->filename, self->n_channels,
				      (int)(fts_dsp_get_sample_rate()),
				      fts_s_int16);
	if (fts_audiofile_is_valid(sf))
	{
		
	    /* create the writer thread */
	    writesf_thread_t* writer = (writesf_thread_t*)fts_object_create(writesf_thread_type, 0, 0);
	    fts_thread_function_t* thread_job = fts_malloc(sizeof(fts_thread_function_t));
	    writer->sf = sf;
	    /* Copy pointer to threaded object */
	    /* TODO: take care of race condition */
	    writer->com_buffer = self->com_buffer;
		
	    writer->buffer_index = &self->buffer_index;
		
	    thread_job->object = (fts_object_t*)writer;
	    thread_job->method = fts_class_get_method(fts_object_get_class(thread_job->object),
						      0,
						      s_write);
	    if (0 == thread_job->method)
	    {
		post("[writesf~] no such method \n");
		fts_log("[_redsf~] no such method \n");
		fts_object_set_error(o, "no such method, init failed \n");
		fts_free(thread_job);
		return;		
	    }
		
	    thread_job->ac = 0;
	    thread_job->at = NULL;
	    thread_job->symbol = s_write;
		
	    thread_job->delay_ms = writer->delay_ms;
		
	    self->thread_worker = fts_malloc(sizeof(fts_thread_worker_t));
	    self->thread_worker->thread_function = thread_job;
		
	    post("start  thread \n");
	    fts_thread_manager_create_thread(self->thread_worker);
	    self->sf = sf;
	    self->is_open = 1;
	}
	else
	{
	    post("audiofile (%s) is not valid \n", self->filename);
	}
    }
    
}

static void writesf_close(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
    writesf_t* self = (writesf_t*)o;
    int i;
    int j;
    int k;

    if (1 == self->is_open)
    {
	/* Here we stop the worker_thread */
	fts_thread_manager_cancel_thread(self->thread_worker);
	/* Delete memory allocated for the worker */
	fts_object_destroy(self->thread_worker->thread_function->object);
	fts_free(self->thread_worker->thread_function);
	fts_free(self->thread_worker);
	/* we close the soundfile */
	fts_audiofile_close(self->sf);
	/* Clear buffer */
	for (i = 0; i < 2; ++i)
	{
	    dtd_buffer_t* com_buffer = &self->com_buffer[i];
	    for (j = 0; j < self->n_channels; ++j)
	    {
		for (k = 0; k < com_buffer->size; ++k)
		{
		    com_buffer->buffer[j][k] = 0.0f;
		}
	    }
	    com_buffer->full = 1;
	}
	self->is_open = 0;
    }
}

static void writesf_start(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{

}

static void writesf_play(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{

}

static void writesf_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{

}

static void writesf_pause(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{

}

static void writesf_init(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  writesf_t* self = (writesf_t*)o;
  int n_channels;
  fts_symbol_t filename;
  int i; 
  int j;

  self->filename = 0;

  n_channels = fts_get_int_arg(ac, at, 0, 1);
  self->n_channels = (n_channels < 1) ? 1 : n_channels;
#ifdef MY_DEBUG
  post("writesf_init: n_channels = %d\n", n_channels);
#endif /* MY_DEBUG */

  if(ac == 2 && fts_is_symbol( at + 1))
  {
      self->filename = fts_get_symbol( at + 1);
  }

  /* Memory Allocation */
  self->com_buffer = fts_malloc(2 * sizeof(dtd_buffer_t));
  for (i = 0; i < 2; ++i)
  {
      dtd_buffer_t* com_buffer = &self->com_buffer[i];
      com_buffer->size = 4096;
      com_buffer->n_channels = n_channels;
      com_buffer->buffer = fts_malloc(n_channels * sizeof(float*));
      for (j = 0; j < n_channels; ++j)
      {
	  com_buffer->buffer[j] = fts_malloc(com_buffer->size * sizeof(float));
      }
      com_buffer->full = 0;
  }

  self->write_index = 0;
  self->buffer_index = 0;
  self->is_open = 0;
  /* start the fts_thread_manager */
  fts_thread_manager_start();

  fts_dsp_add_object(o);

}

static void writesf_delete(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  writesf_t* self = (writesf_t*)o;
  int i;
  int j;

  /* call close for thread */
  writesf_close(o, winlet, s, ac, at);
  /* Memory Deallocation */
  for (i = 0; i < 2; ++i)
  {
      dtd_buffer_t* com_buffer = &self->com_buffer[i];
      for (j = 0; j < self->n_channels; ++j)
      {
	  fts_free(com_buffer->buffer[j]);
      }
      fts_free(com_buffer->buffer);
  }
  fts_free(self->com_buffer);

  fts_dsp_remove_object(o);
}


static fts_status_t
writesf_instantiate(fts_class_t* cl, int ac, const fts_atom_t* at)
{
    int n_channels;
    int i;
    
    n_channels = fts_get_int_arg(ac, at, 0, 1);

    if (n_channels < 1)
    {
	n_channels = 1;
    }

    fts_class_init(cl, sizeof(writesf_t), n_channels, 0, 0);
    
    fts_method_define_varargs(cl ,fts_system_inlet, fts_s_init, writesf_init);
    fts_method_define_varargs(cl, fts_system_inlet, fts_s_delete, writesf_delete);
    fts_method_define_varargs(cl, fts_system_inlet, fts_s_put, writesf_put);

    fts_method_define_varargs(cl, 0, fts_s_open, writesf_open);
    fts_method_define_varargs(cl, 0, s_close, writesf_close);
    
    fts_method_define_varargs(cl, 0, s_play, writesf_play);
    fts_method_define_varargs(cl, 0, s_pause, writesf_pause);

    for (i = 0; i < n_channels; ++i)
    {
	fts_dsp_declare_inlet(cl, i);
    }

    fts_dsp_declare_function(writesf_symbol, writesf_dsp);

    return fts_ok;
}



/**************************************************
 *
 * dtd module configuration
 *
 */
void writesf_config(void)
{
    s_close = fts_new_symbol( "close");
    s_play = fts_new_symbol( "play");
    s_record = fts_new_symbol( "record");
    s_pause = fts_new_symbol( "pause");
    s_write = fts_new_symbol( "write");

    writesf_symbol = fts_new_symbol("writesf~");
    fts_class_install(writesf_symbol, writesf_instantiate);

    writesf_thread_symbol = fts_new_symbol("writesf_thread");
    writesf_thread_type = fts_class_install(writesf_thread_symbol, writesf_thread_instantiate);
}


