/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#include <fts/fts.h>
#include <fts/thread.h>
#include "dtd_buffer.h"
#include "dtd_thread.h"

/**************************************************
 *
 * readsf~
 *
 */
typedef struct
{
  fts_dsp_object_t head;
  int n_channels;
  fts_symbol_t filename;

  /* sound file to read */
  fts_audiofile_t* sf;

  /* communication buffer */
  dtd_buffer_t* com_buffer;

  int buffer_index;
  int read_index;
  int is_open;
  int is_started;
  int is_eof;

  fts_thread_worker_t* thread_worker;
} readsf_t;

static fts_symbol_t readsf_symbol;

static fts_symbol_t s_play;
static fts_symbol_t s_pause;
static fts_symbol_t s_open_file;


void reader_set_state(readsf_t* self,
		      fts_audiofile_t* sf, 
		      dtd_buffer_t* com_buffer, 
		      const int* const buffer_index,
		      const int* const is_eof)
{
  dtd_thread_t* reader = (dtd_thread_t*)self->thread_worker->thread_function->object;
  dtd_thread_set_state(reader, sf, com_buffer, buffer_index, is_eof);
}

static void create_reader_thread(readsf_t* self)
{
  fts_thread_worker_t* thread_worker = fts_malloc(sizeof(fts_thread_worker_t));
  dtd_thread_t* reader = (dtd_thread_t*)fts_object_create(dtd_thread_type, 0, 0);
  fts_thread_function_t* thread_job = fts_malloc(sizeof(fts_thread_function_t));
    
  thread_job->object = (fts_object_t*)reader;
  thread_job->method = fts_class_get_method_varargs(fts_object_get_class(thread_job->object),
						    fts_s_read);
  if (0 == thread_job->method)
  {
    fts_log("[readsf~] no such method \n");
    fts_object_error((fts_object_t*)self, "no such method, init failed \n");
    fts_free(thread_job);
    return;
  }
    
  thread_job->ac = 0;
  thread_job->at = NULL;
  thread_job->symbol = fts_s_read;
    
  thread_job->delay_ms = reader->delay_ms;
  thread_worker = fts_malloc(sizeof(fts_thread_worker_t));
  thread_worker->thread_function = thread_job;
  self->thread_worker = thread_worker;
}

static void delete_reader_thread(readsf_t* self)
{
  fts_object_destroy(self->thread_worker->thread_function->object);
  fts_free(self->thread_worker->thread_function);
  fts_free(self->thread_worker);
}

static void readsf_eof_alarm(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  /* set bang to last outlet */
  fts_outlet_bang(o, fts_object_get_outlets_number(o) - 1);
}

static void readsf_dsp( fts_word_t *argv)
{
  readsf_t *self = (readsf_t *)fts_word_get_pointer( argv + 0);
  int n_tick = fts_word_get_int(argv + 1);
  float* out;
  int n;
  int channels;
  int n_channels = self->n_channels;
  dtd_buffer_t* com_buffer;
  float** buffer;
  int buffer_index;

  buffer_index = self->buffer_index;


  if (0 == self->is_started)
  {
    /* now fill output with zero */
    for (channels = 0; channels < n_channels; ++channels)
    {
      out = (float*)fts_word_get_pointer(argv + 2 + channels);
      /* fill output with zero */
      for (n = 0; n < n_tick; ++n)
      {		  
	  out[n] = 0.0f;
      }
    }
    return;
  }


  /* check if it is eof and buffers are empty */
  if ((1 == self->is_eof)
      && (self->com_buffer[0].full == 0)
      && (self->com_buffer[1].full == 0)
    )
  {
    fts_post ("[readsf~] end of file\n");
    self->is_started = 0;
    /* add callback  eof file method */
    fts_timebase_add_call(fts_get_timebase(),(fts_object_t*)self, readsf_eof_alarm, NULL, 0.0);
    return;
  }



  /* check if there is enough data available in com_buffer */
  if (!(self->com_buffer[buffer_index].end_index >= (self->read_index + n_tick)))
  {
    /* set buffer to empty */
    self->com_buffer[buffer_index].full = 0;
    /* swap buffer */      
    buffer_index = (buffer_index + 1) % 2;
    /* check again */
    if (!(self->com_buffer[buffer_index].end_index >= (self->read_index + n_tick)))
    {
      /* set buffer to empty */
      self->com_buffer[buffer_index].full = 0;
      /* now fill output with zero */
      for (channels = 0; channels < n_channels; ++channels)
      {
	out = (float*)fts_word_get_pointer(argv + 2 + channels);
	/* fill output with zero */
	for (n = 0; n < n_tick; ++n)
	{		  
	  out[n] = 0.0f;
	}
      }
      /* job done, go away ... */
      return;
    }
    /* set new value for buffer_index */
    self->buffer_index = buffer_index;
  }

  com_buffer = &self->com_buffer[buffer_index];
  buffer = com_buffer->buffer;

  /* Read from buffer */
  for (channels = 0; channels < n_channels; ++channels)
  {
    out = (float*)fts_word_get_pointer(argv + 2 + channels);
    for (n = 0; n < n_tick; ++n)
    {
      out[n] = buffer[channels][n + self->read_index];
    }
  }
  self->read_index += n_tick;

  /* check if there is enough place in buffer for next run */
  if ((self->read_index + n_tick) > com_buffer->size)
  {
    /* set empty flag */
    com_buffer->full = 0;
    com_buffer->end_index = 0;
    /* swap index */
    self->buffer_index += 1;
    self->buffer_index %= 2;
    /* reset read_index */
    self->read_index = 0;
  }
  

}

static void readsf_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t *self = (readsf_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  fts_atom_t argv[32];
  int i;

  fts_set_pointer( argv + 0, self);
  fts_set_int( argv + 1, fts_dsp_get_output_size( dsp, 0));

  for ( i = 0; i < self->n_channels; i++)
  {
    fts_set_symbol( argv + 2 + i, fts_dsp_get_output_name( dsp, i));
  }
  
  fts_dsp_add_function( readsf_symbol, 2 + self->n_channels, argv);
}

/* forward declaration */
static void readsf_close(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);



static void readsf_open(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t* self = (readsf_t*)o;
  fts_audiofile_t* sf;

  if (1 == self->is_open)
  {
    /* call close */
    readsf_close(o, winlet, s, ac, at);
  }

  if (ac > 0 && fts_is_symbol(at))
  {
    self->filename = fts_get_symbol(at);

    sf = fts_audiofile_open_read(self->filename);
    if (fts_audiofile_is_valid(sf))
    {	    
      /* set reader thread state */
      reader_set_state(self, sf, self->com_buffer, &self->buffer_index, &self->is_eof);

      /* start reader thread */
      fts_thread_manager_create_thread(self->thread_worker);
      self->sf = sf;
      self->is_open = 1; 
    }
    else
    {
      fts_post("[readsf~] audiofile (%s) is not valid \n", self->filename);
      fts_log("[readsf~] audiofile (%s) is not valid \n", self->filename);
    }
  }
  else
  {
    fts_atom_t a[4];
      
    fts_set_symbol(a, fts_s_open);
    fts_set_symbol(a + 1, s_open_file);
    fts_set_symbol(a + 2, fts_project_get_dir());
    if (NULL != self->filename)
    {
      fts_set_symbol(a + 3, self->filename);
    }
    else
    {
      fts_set_symbol(a + 3, fts_new_symbol(" "));
    }
    fts_client_send_message(o, fts_s_openFileDialog, 4, a);
  }
}

static void readsf_close(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t* self = (readsf_t*)o;
  int i;
  int j;
  int k;

  if (1 == self->is_open)
  {
    /* Here we stop the worker_thread */
    fts_thread_manager_cancel_thread(self->thread_worker);	
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
      com_buffer->full = 0;
    }
    self->is_open = 0;
    self->is_started = 0;
  }
}

static void readsf_start(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t* self = (readsf_t*)o;

  fts_post("[readsf~] start \n");
  self->is_started = 1;
}


static void readsf_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t* self = (readsf_t*)o;

  fts_post("[readsf~] stop \n");
  self->is_started = 0;
  self->read_index = 0;

}

static void readsf_pause(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t* self = (readsf_t*)o;

  fts_post("[readsf~] pause \n");
  self->is_started += 1;
  self->is_started = self->is_started % 2;

}

static void readsf_init(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  readsf_t* self = (readsf_t*)o;
  int n_channels;
  int i; 
  int j;
  
  self->filename = 0;
  
  n_channels = fts_get_int_arg(ac, at, 0, 1);
  self->n_channels = (n_channels < 1) ? 1 : n_channels;

  if(ac == 2 && fts_is_symbol( at + 1))
  {
    self->filename = fts_get_symbol( at + 1);
  }

  /* Memory Allocation */
  self->com_buffer = fts_malloc(2 * sizeof(dtd_buffer_t));
  for (i = 0; i < 2; ++i)
  {
    dtd_buffer_t* com_buffer = &self->com_buffer[i];
    com_buffer->size = DTD_COM_BUF_DEFAULT_SIZE;
    com_buffer->n_channels = n_channels;
    com_buffer->buffer = fts_malloc(n_channels * sizeof(float*));
    for (j = 0; j < n_channels; ++j)
    {
      com_buffer->buffer[j] = fts_malloc(com_buffer->size * sizeof(float));
    }
    com_buffer->full = 0;
    com_buffer->end_index = 0;
  }

  self->read_index = 0;
  self->buffer_index = 0;
  self->is_open = 0;
  self->is_started = 0;
  self->is_eof = 0;

  /* create the reader thread */
  create_reader_thread(self);
  
  /* start the fts_thread_manager */
  fts_thread_manager_start();

  fts_dsp_object_init((fts_dsp_object_t *)o);
  /* add an outlet for end of file alarm bang */
  fts_object_set_outlets_number(o, n_channels + 1);
  fts_class_outlet_bang(fts_object_get_class(o), n_channels);
}

static void readsf_delete(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  readsf_t* self = (readsf_t*)o;
  int i;
  int j;

  /* call close for thread and file */
  readsf_close(o, winlet, s, ac, at);

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

  /* delete the reader thread */
  delete_reader_thread(self);

  fts_dsp_object_delete((fts_dsp_object_t *)o);
}


static void
readsf_instantiate(fts_class_t* cl)
{
  fts_class_init(cl, sizeof(readsf_t), readsf_init, readsf_delete);
  fts_class_message_varargs(cl, fts_s_put, readsf_put);
  
  fts_class_message_varargs(cl, fts_s_open, readsf_open);
  fts_class_message_varargs(cl, fts_s_close, readsf_close);
  
  fts_class_message_varargs(cl, fts_s_start, readsf_start);
  fts_class_message_varargs(cl, fts_s_stop, readsf_stop);
  fts_class_message_varargs(cl, s_pause, readsf_pause);

  fts_class_inlet_thru(cl, 0);
  
  fts_dsp_declare_outlet(cl, 0);
  fts_dsp_declare_outlet(cl, 1);
  fts_dsp_declare_outlet(cl, 2);

  fts_dsp_declare_function(readsf_symbol, readsf_dsp);
}



/**************************************************
 *
 * readsf configuration
 *
 */
void readsf_config(void)
{
  s_play = fts_new_symbol( "play");
  s_pause = fts_new_symbol( "pause");
  s_open_file = fts_new_symbol("open file");
  
  readsf_symbol = fts_new_symbol("readsf~");
  fts_class_install(readsf_symbol, readsf_instantiate);
}

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
