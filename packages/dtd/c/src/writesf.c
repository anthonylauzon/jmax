/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * See file LICENSE for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * Based on Max/ISPW by Miller Puckette.
 *
 */

#include <fts/fts.h>
#include <fts/thread.h>
#include "dtd_buffer.h"
#include "dtd_thread.h"


/**************************************************
 *
 * writesf~
 *
 */
typedef struct
{
    fts_dsp_object_t head;
    int n_channels;
    fts_symbol_t filename;

    /* sound file to write */
    fts_audiofile_t* sf;

    /* communication buffer */
    dtd_buffer_t* com_buffer;

    int buffer_index;
    int write_index;
    int is_open;
    int is_started;

    fts_thread_worker_t* thread_worker;
} writesf_t;

static fts_symbol_t writesf_symbol;

static fts_symbol_t s_record;
static fts_symbol_t s_pause;



static void writesf_dsp( fts_word_t *argv)
{
  writesf_t *self = (writesf_t *)fts_word_get_pointer( argv + 0);
  int n_tick = fts_word_get_int(argv + 1);
  float* in = (float*)fts_word_get_pointer(argv + 2);
  int n;
  int channels;

  dtd_buffer_t* com_buffer;
  float** buffer;

  com_buffer = &self->com_buffer[self->buffer_index];
  buffer = com_buffer->buffer;

  if (0 == self->is_started)
  {
      return;
  }

  /* channels loop */
  for (channels = 0; channels < self->n_channels; ++channels)
  {
      in = (float*)fts_word_get_pointer(argv + 2 + channels);
      /* Write from buffer */
      for (n = 0; n < n_tick; ++n)
      {
	  buffer[channels][n + self->write_index] = in[n];
      }
  }
  self->write_index += n_tick;

  if ((self->write_index + n_tick) > com_buffer->size)
  {
      com_buffer->full = 1;
      com_buffer->end_index = self->write_index;
      /* swap buffer index */
      self->buffer_index += 1;
      self->buffer_index %= 2;

      self->write_index = 0;
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


/* forward declaration */
static void writesf_close(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);

static void writesf_open(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
    writesf_t* self = (writesf_t*)o;
    fts_audiofile_t* sf;

    /* if a file is already open we close the previous file */
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
	    dtd_thread_t* writer = (dtd_thread_t*)fts_object_create(dtd_thread_type, NULL, 0, 0);
	    fts_thread_function_t* thread_job = fts_malloc(sizeof(fts_thread_function_t));
	    writer->sf = sf;
	    writer->com_buffer = self->com_buffer;		
	    writer->buffer_index = &self->buffer_index;		
	    thread_job->object = (fts_object_t*)writer;
	    thread_job->method = fts_class_get_method(fts_object_get_class(thread_job->object),
						      fts_s_write);
	    if (0 == thread_job->method)
	    {
		fts_log("[writesf~] no such method \n");
		fts_object_set_error(o, "no such method, init failed \n");
		fts_free(thread_job);
		return;		
	    }
		
	    thread_job->ac = 0;
	    thread_job->at = NULL;
	    thread_job->symbol = fts_s_write;
		
	    thread_job->delay_ms = writer->delay_ms;
		
	    self->thread_worker = fts_malloc(sizeof(fts_thread_worker_t));
	    self->thread_worker->thread_function = thread_job;
		
	    fts_thread_manager_create_thread(self->thread_worker);
	    self->sf = sf;
	    self->is_open = 1;
	}
	else
	{
	    post("[writesf~] audiofile (%s) is not valid \n", self->filename);
	    fts_log("[writesf~] audiofile (%s) is not valid \n", self->filename);
	}
    }
    
}

static void writesf_close(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
    writesf_t* self = (writesf_t*)o;
    int i;

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
	/* set buffer to empty and set write_index, end_index to buffer begin */
	for (i = 0; i < 2; ++i)
	{
	    self->com_buffer[i].full = 0;
	    self->com_buffer[i].end_index = 0;
	}
	self->is_open = 0;
	self->is_started = 0;
	self->write_index = 0;
    }
}

static void writesf_start(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
    writesf_t* self = (writesf_t*)o;
    /* not yet implemented */
    post("[writesf~] want to start \n");
    self->is_started = 1;
}


static void writesf_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
    writesf_t* self = (writesf_t*)o;
    /* not yet implemented */
    post("[writesf~] want to stop \n");
    self->is_started = 0;
}

static void writesf_pause(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
    writesf_t* self = (writesf_t*)o;
    /* not yet implemented */
    post("[writesf~] want to pause \n");
    self->is_started += 1;
    self->is_started = self->is_started % 2;
}

static void writesf_init(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  writesf_t* self = (writesf_t*)o;
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
      com_buffer->size = 4096;
      com_buffer->n_channels = n_channels;
      com_buffer->buffer = fts_malloc(n_channels * sizeof(float*));
      for (j = 0; j < n_channels; ++j)
      {
	  com_buffer->buffer[j] = fts_malloc(com_buffer->size * sizeof(float));
      }
      com_buffer->full = 0;
      com_buffer->end_index = 0;
  }

  self->write_index = 0;
  self->buffer_index = 0;
  self->is_open = 0;
  self->is_started = 0;

  /* start the fts_thread_manager */
  fts_thread_manager_start();

  fts_dsp_object_init((fts_dsp_object_t *)o);
  fts_object_set_inlets_number(o, n_channels);
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

  fts_dsp_object_delete((fts_dsp_object_t *)o);
}


static void
writesf_instantiate(fts_class_t* cl, int ac, const fts_atom_t* at)
{
    fts_class_init(cl, sizeof(writesf_t), writesf_init, writesf_delete);

    fts_class_message_varargs(cl, fts_s_put, writesf_put);

    fts_class_message_varargs(cl, fts_s_open, writesf_open);
    fts_class_message_varargs(cl, fts_s_close, writesf_close);

    /* not yet implemented */
    fts_class_message_varargs(cl, fts_s_start, writesf_start);
    fts_class_message_varargs(cl, fts_s_stop, writesf_stop);
    fts_class_message_varargs(cl, s_pause, writesf_pause);


   fts_dsp_declare_inlet(cl, 0);

    fts_dsp_declare_function(writesf_symbol, writesf_dsp);
}



/**************************************************
 *
 * writesf configuration
 *
 */
void writesf_config(void)
{
  s_record = fts_new_symbol( "record");
  s_pause = fts_new_symbol( "pause");
  
  writesf_symbol = fts_new_symbol("writesf~");
  fts_class_install(writesf_symbol, writesf_instantiate);
}
