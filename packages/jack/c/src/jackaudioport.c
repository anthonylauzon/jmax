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

typedef struct
{
    fts_audioport_t head;
/*     jack_client_t* client; */
    /* TODO: 
       Change structure if we want several input and output .
       But maybe it should be several jackaudioport, in this case we have to choose
       unique jack_client name */
    jack_port_t* input_port;
    jack_port_t* output_port;
    /* TODO:
       If we add several input/output  port, we should want several input/output buffer
    */
    float* input_buffer;
    float* output_buffer;
    /* TODO:
       Frame size could be changed by jack, we need to add a callback for buffer size 
       change */
    int nframes;
    /* TODO:
       sampling rate could be changed by jack, we need to add a callback for sampling 
       rate change */
    unsigned long samplingRate;

} jackaudioport_t;
/* Do we want connection set by jMax or by an external program ? */
   


#define JACKAUDIOPORT_DEFAULT_FRAME_SIZE 128

/* Input function of fts_audioport */
/* TODO:
   Change this function to be able to deal with several channels 
*/
static void
jackaudioport_input(fts_word_t* argv)
{
    jackaudioport_t* this;
    int n;
    int channels;
    float* out;

    this = (jackaudioport_t*)fts_word_get_pointer(argv + 0);
    /* get number of sample in buffer */
    n = fts_word_get_int(argv + 1);
    /* get number of channels */
    channels = fts_audioport_get_input_channels((fts_audioport_t*)this);
    /* get object output buffer */
    out = (float*)fts_word_get_pointer(argv + 2);
    /* only a copy from jack buffer to object output */
    /* TODO:
       - What happen if n is greater than input_buffer allocated space
       - Check what can happen if jack try to write in buffer, when we are doing 
       the copy 
    */
    memcpy(out, this->input_buffer, n * sizeof(float));
}

/* Output function of fts_audioport */
/* TODO:
   Change this function to be able to deal with several channels 
*/
static void
jackaudioport_output(fts_word_t* argv)
{
    jackaudioport_t* this;
    int n;
    int channels;

    float* in;
    this = (jackaudioport_t*)fts_word_get_pointer(argv + 0);

    /* get number of sample in buffer */  
    n = fts_word_get_int(argv + 1);
    /* get number of channels */
    channels = fts_audioport_get_output_channels((fts_audioport_t*)this);
    
    /* get object input buffer */
    in = (float*)fts_word_get_pointer(argv + 2 );

    /* only a copy from object input to jack buffer */
    /* TODO:
       - What happen if n is greater than input_buffer allocated space
       - Check what can happen if jack try to read in buffer, when we are doing 
       the copy 
    */
    memcpy(this->output_buffer, in, n * sizeof(float));    
}

/* JACK callback */
/* TODO:
   Change this function if we want more JACK port 
*/
static
int jackaudioport_process(jack_nframes_t nframes, void* arg)
{

    jackaudioport_t* this = (jackaudioport_t*)arg;
    /* Get JACK output port buffer pointer */
    jack_default_audio_sample_t* out = (jack_default_audio_sample_t*)jack_port_get_buffer(this->output_port, nframes);
    /* Get JACK input port buffer pointer */
    jack_default_audio_sample_t* in = (jack_default_audio_sample_t*)jack_port_get_buffer(this->input_port, nframes);
    
    int n = 0;
    /* get number of samples of a FTS tick */
    int samples_per_tick = fts_dsp_get_tick_size();

    /* TODO: 
       Check if in/out are valid pointer 
    */
    this->input_buffer = in;
    this->output_buffer = out;
    
    /* TODO: 
       Need to be fix if (nframes % samples_per_tick != 0) 

       Case 1: nframes < samples_per_tick
       
       Case 2: nframes > samples_per_tick

    */
    for (n = 0; n < nframes; n += samples_per_tick)
    {
	/*	fts_sched_run_one_tick_without_select(); */
	/* Run scheduler */
	fts_sched_run_one_tick();  
	/* Step forward in input/output buffer */
	this->input_buffer += samples_per_tick;
	this->output_buffer += samples_per_tick;
    }
    return 0;
}

/* This function is used to remove object from scheduler and to activate JACK client */
static void
jackaudioport_halt(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
    fd_set rfds;
    jackaudioport_t* this = (jackaudioport_t*)o;
    jack_client_t* client = jackaudiomanager_get_jack_client();
    /* Remove object of FTS scheduler */
    fts_sched_remove(o);
    fts_log("[jackaudioport] jackaudioport removed from scheduler \n");

    
    /* Activate jack client */    
    if (jack_activate(client) == -1)
    {
	fts_log("[jackaudioport] cannot activate JACK client \n");
	fts_object_set_error(o, "cannot activate JACK client \n");
	return;
    }


    fts_log("[jackaudioport] jack client activated \n");

    /* DEBUG */
    /* connect to alsa_pcm:playback_1 ... */
    if (jack_connect(client, jack_port_name(this->output_port), "alsa_pcm:playback_1"))
    {
      fts_log("[jackaudioport] cannot connect to alsa_pcm:plyback_1\n");
    }

    /* Stop FTS scheduler */
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);
    /* check return value of select */
    if (select(1, &rfds, NULL, NULL, NULL) < 0)
    {
	fts_log("[jackaudioport] select falied \n");
    }

    fts_log("[jackaudioport] FTS scheduler stopped \n");

}

static void
jackaudioport_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
    jackaudioport_t* this = (jackaudioport_t*)o;
    jack_client_t* client = jackaudiomanager_get_jack_client();
    fts_audioport_init(&this->head);

    if (NULL == client)
    {
	post("[jackaudioport] cannot create jack client\n"
	     "[jackaudioport] Are you sure than jackd is running ? \n");
	fts_log("[jackaudioport] cannot create jack client \n");
	fts_object_set_error(o, "[jackaudioport] cannot create jack client \n");
	return;
    }

    /* JACK client process callback setting */
    jack_set_process_callback(client,
			      jackaudioport_process, /* callback function */
			      (void*)this);          /* we need to have our object
							in our callback function
						     */
    fts_log("[jackaudioport] set jackaudioport process callback \n");
    
    /* JACK input/output port registering */
    this->input_port = jack_port_register(client,                   /* client structure */
					  "input",                  /* port name */
					  JACK_DEFAULT_AUDIO_TYPE,  /* port type */ 
					  JackPortIsInput,          /* flags */
					  0);                       /* we can pass 0 for buffersize,
								       because we use the 
								       JACK_DEFAULT_AUDIO_TYPE 
								       builtin type
								    */
    fts_log("[jackaudioport] input port registered \n");
    
    /* see above for jack_port_register parameters */
    this->output_port = jack_port_register(client, 
					   "output", 
					   JACK_DEFAULT_AUDIO_TYPE, 
					   JackPortIsOutput, 
					   0);
    fts_log("[jackaudioport] output port registered \n");


    /* memory allocation for input/output buffer */
    this->input_buffer = fts_malloc(JACKAUDIOPORT_DEFAULT_FRAME_SIZE * sizeof(float));
    if (NULL == this->input_buffer)
    {
	fts_log("[jackaudioport] cannot allocate memory for input buffer \n");
	fts_object_set_error(o, "[jackaudioport] cannot allocate memory for input buffer \n");
	return;
    }
    this->output_buffer = fts_malloc(JACKAUDIOPORT_DEFAULT_FRAME_SIZE * sizeof(float));
    if (NULL == this->output_buffer)
    {
	fts_log("[jackaudioport] cannot allocate memory for output buffer \n");
	fts_object_set_error(o, "[jackaudioport] cannot allocate memory for output buffer \n");
	return;
    }

    this->nframes = JACKAUDIOPORT_DEFAULT_FRAME_SIZE;


    fts_audioport_set_input_channels((fts_audioport_t*)this, 1);
    fts_audioport_set_input_function((fts_audioport_t*)this, jackaudioport_input);
    fts_audioport_set_output_channels((fts_audioport_t*)this, 1);
    fts_audioport_set_output_function((fts_audioport_t*)this, jackaudioport_output);

    fts_sched_add(o, FTS_SCHED_ALWAYS);
    /* jackaudioport_halt(o, winlet, s, ac, at); */
}

static void
jackaudioport_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
    jackaudioport_t* this = (jackaudioport_t*)o;
    jack_client_t* client = jackaudiomanager_get_jack_client();

    fts_audioport_delete(&this->head);

    if (NULL != client)
    {
	jack_deactivate(client);
	fts_log("[jackaudioport] jack client deactivated \n");
    }

    if (0 != this->input_buffer)
    {
	fts_free(this->input_buffer);
    }

    if (0 != this->output_buffer)
    {
	fts_free(this->output_buffer);
    }

    /* go back to fts scheduling */
/*     fts_sched_unsuspend(); */
}

static void jackaudioport_instantiate(fts_class_t *cl)
{
    fts_class_init(cl, sizeof( jackaudioport_t), jackaudioport_init, jackaudioport_delete);
    
    fts_class_message_varargs(cl, fts_s_sched_ready, jackaudioport_halt);
    
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


    fts_class_install(jackaudioport_symbol, jackaudioport_instantiate);

    if (!fts_audioport_get_default_class())
	fts_audioport_set_default_class(jackaudioport_symbol);
}


/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
