/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, Fance.
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
 */

/* 
 * This file include the jMax JACK audio port.
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <math.h>
#include <jack/jack.h>
#include <fts/fts.h>
#include "jackaudio.h"

#define JACKAUDIOPORT_DEFAULT_FRAME_SIZE 128

static int nframes = 0;

/**
 * The audioport IO function calls the native audio layer to read/write a buffer
 * of samples in the native format.
 * Its argument is the audioport. Buffers are allocated by the port.
 */
static void
jackaudioport_input_fun(fts_audioport_t* port)
{
  jackaudioport_t* self = (jackaudioport_t*)port;
  /* Get JACK input port buffer pointer */
  jack_default_audio_sample_t* in;

  in = (jack_default_audio_sample_t*)jack_port_get_buffer(self->input_port, get_jack_process_nframes());  
}

static void
jackaudioport_output_fun(fts_audioport_t* port)
{
  jackaudioport_t* self = (jackaudioport_t*)port;
  /* Get JACK input port buffer pointer */
  jack_default_audio_sample_t* out;

  out = (jack_default_audio_sample_t*)jack_port_get_buffer(self->output_port, get_jack_process_nframes());
}

/** 
 * The audioport copy function copies the samples in the native format to a float buffer
 * for a given channel.
 */
static void 
jackaudioport_input_copy_fun( fts_audioport_t *port, float *buff, int buffsize, int channel)
{
  jackaudioport_t* self = (jackaudioport_t*)port;
  /* Get JACK input port buffer pointer */
  jack_default_audio_sample_t* in;
  int i;
  int nsample_consumed = get_jack_process_consumed();

  in = (jack_default_audio_sample_t*)jack_port_get_buffer(self->input_port, get_jack_process_nframes());  

  for (i = 0; i < buffsize; ++i)
  {
    buff[i] = in[i + nsample_consumed];
  }
}

static void 
jackaudioport_output_copy_fun( fts_audioport_t *port, float *buff, int buffsize, int channel)
{
  jackaudioport_t* self = (jackaudioport_t*)port;
  /* Get JACK input port buffer pointer */
  jack_default_audio_sample_t* out;
  int i;
  int nsample_consumed = get_jack_process_consumed();

  out = (jack_default_audio_sample_t*)jack_port_get_buffer(self->output_port, get_jack_process_nframes());

  for (i = 0; i < buffsize; ++i)
  {
    out[i + nsample_consumed] = buff[i];
  }

  fts_log("[jackaudioport_output_copy_fun] called  %s\n", ((jackaudioport_t*)port)->port_name);
}




static void
jackaudioport_open(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_atom_t at_m[2];

  post("[jackaudioport] symbol received : %s\n", fts_get_symbol(at));
  /* send a mesage manager to tell him that jackaudioport is opening */
  /* 
     manager need to set jack callback
     and suspend scheduler 
  */
  fts_set_object(at_m, o);
  fts_set_symbol(at_m + 1, fts_get_symbol(at));
  fts_send_message(jackaudiomanager_get_manager_object(), s, 2, at_m);
}

static void
jackaudioport_close(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_audioport_t* port = (fts_audioport_t*)o;
  fts_atom_t at_m;

  if (fts_s_close_input == s)
  {
    
    fts_audioport_unset_open(port, FTS_AUDIO_INPUT);
  }
  else
  {
    fts_audioport_unset_open(port, FTS_AUDIO_OUTPUT);
  }

  fts_set_object(&at_m, o);

  fts_send_message(jackaudiomanager_get_manager_object(), s, 1, &at_m);
}


static void
jackaudioport_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  jackaudioport_t* self = (jackaudioport_t*)o;
  jack_client_t* client = jackaudiomanager_get_jack_client();
  int flags = fts_get_int(at);
  fts_symbol_t port_name = fts_get_symbol(at + 1);

  fts_audioport_init((fts_audioport_t*)self);

  if (NULL == client)
  {
    post("[jackaudioport] cannot create jack client\n"
	 "[jackaudioport] Are you sure than jackd is running ? \n");
    fts_log("[jackaudioport] cannot create jack client \n");
    fts_object_error(o, "[jackaudioport] cannot create jack client \n");
    return;
  }
    
  self->port_name = port_name;

  /* memory allocation for input/output buffer */
  self->input_buffer = fts_malloc(JACKAUDIOPORT_DEFAULT_FRAME_SIZE * sizeof(float));
  if (NULL == self->input_buffer)
  {
    fts_log("[jackaudioport] cannot allocate memory for input buffer \n");
    fts_object_error(o, "[jackaudioport] cannot allocate memory for input buffer \n");
    return;
  }

  self->input_port = NULL;

  self->output_buffer = fts_malloc(JACKAUDIOPORT_DEFAULT_FRAME_SIZE * sizeof(float));
  if (NULL == self->output_buffer)
  {
    fts_log("[jackaudioport] cannot allocate memory for output buffer \n");
    fts_object_error(o, "[jackaudioport] cannot allocate memory for output buffer \n");
    return;
  }

  self->output_port = NULL;

  self->nframes = JACKAUDIOPORT_DEFAULT_FRAME_SIZE;

  fts_audioport_set_io_fun((fts_audioport_t*)self, FTS_AUDIO_INPUT, jackaudioport_input_fun);
  fts_audioport_set_io_fun((fts_audioport_t*)self, FTS_AUDIO_OUTPUT, jackaudioport_output_fun);
    
  fts_audioport_set_copy_fun((fts_audioport_t*)self, FTS_AUDIO_INPUT, jackaudioport_input_copy_fun);
  fts_audioport_set_copy_fun((fts_audioport_t*)self, FTS_AUDIO_OUTPUT, jackaudioport_output_copy_fun);

  fts_audioport_set_max_channels((fts_audioport_t*)self, FTS_AUDIO_INPUT, 1);
  fts_audioport_set_max_channels((fts_audioport_t*)self, FTS_AUDIO_OUTPUT, 1);


  /* 
     If port is a jack output port => this a jmax input port
     If port is a jack input port => this a jmax output port
  */
  if (JackPortIsOutput == flags)
  {
    fts_audioport_set_valid((fts_audioport_t*)self, FTS_AUDIO_INPUT);
  }
  else
  {
    fts_audioport_set_valid((fts_audioport_t*)self, FTS_AUDIO_OUTPUT);
  }
}

static void
jackaudioport_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  jackaudioport_t* self = (jackaudioport_t*)o;
  jack_client_t* client = jackaudiomanager_get_jack_client();

  fts_audioport_delete(&self->head);

  if (NULL != client)
  {
    jack_deactivate(client);
    fts_log("[jackaudioport] jack client deactivated \n");
  }

  if (0 != self->input_buffer)
  {
    fts_free(self->input_buffer);
  }

  if (0 != self->output_buffer)
  {
    fts_free(self->output_buffer);
  }
}

static void jackaudioport_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof( jackaudioport_t), jackaudioport_init, jackaudioport_delete);
    
  /* open method */
  fts_class_message_varargs(cl, fts_s_open_input, jackaudioport_open);
  fts_class_message_varargs(cl, fts_s_open_output, jackaudioport_open);

  /* close method */
  fts_class_message_varargs(cl, fts_s_close_input, jackaudioport_close);
  fts_class_message_varargs(cl, fts_s_close_output, jackaudioport_close);

  return;
}

/***********************************************************************
 *
 * Config
 *
 */
void jackaudioport_config( void)
{
  fts_symbol_t jackaudioport_symbol = fts_new_symbol("jackaudioport");
      
  jackaudioport_type = fts_class_install(jackaudioport_symbol, jackaudioport_instantiate);
}


/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
