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
 */

/* 
 * This file include the jMax JACK audio port.
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <math.h>
#include <jack/jack.h>
#include <fts/fts.h>


#define MY_DEFAULT_FREQ 8.

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif /* M_PI */

#define DEFAULT_FRAME_SIZE 128

typedef struct
{
    fts_audioport_t head;
    jack_client_t* client;
    jack_port_t* input_port;
    jack_port_t* output_port;
    float* input_buffer;
    float* output_buffer;
    int nframes;
    unsigned long samplingRate;
} jackaudioport_t;

static void
jackaudioport_input(fts_word_t* argv)
{
    jackaudioport_t* this;
    int n, channels, ch, i, j;
    float* out;

    this = (jackaudioport_t*)fts_word_get_pointer(argv + 0);
    n = fts_word_get_int(argv + 1);
    channels = fts_audioport_get_input_channels((fts_audioport_t*)this);
    out = (float*)fts_word_get_pointer(argv + 2);

    for (i = 0; i < n; ++i)
    {
	out[i] = this->input_buffer[i];
    }
}

static void
jackaudioport_output(fts_word_t* argv)
{
    jackaudioport_t* this;
    int n, channels, ch, i, j;
    float* in;
    this = (jackaudioport_t*)fts_word_get_pointer(argv + 0);
    n = fts_word_get_int(argv + 1);
    channels = fts_audioport_get_output_channels((fts_audioport_t*)this);
    in = (float*)fts_word_get_pointer(argv + 2 );

    for (i = 0; i < n; ++i)
    {
	this->output_buffer[i] = in[i];
    }
    
}

static
int jackaudioport_process(jack_nframes_t nframes, void* arg)
{

    jackaudioport_t* this = (jackaudioport_t*)arg;
    jack_default_audio_sample_t* out = (jack_default_audio_sample_t*)jack_port_get_buffer(this->output_port, nframes);
    jack_default_audio_sample_t* in = (jack_default_audio_sample_t*)jack_port_get_buffer(this->input_port, nframes);
    
    int n = 0;
    int samples_per_tick = fts_dsp_get_tick_size();

    this->input_buffer = in;
    this->output_buffer = out;
    
    for (n = 0; n < nframes; n += samples_per_tick)
    {
/*    fts_sched_run_one_tick_without_select(); */
	fts_sched_run_one_tick();
	this->input_buffer += samples_per_tick;
	this->output_buffer += samples_per_tick;
    }
    return 0;
}

static void
jackaudioport_halt(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
    fd_set rfds;
    jackaudioport_t* this = (jackaudioport_t*)o;
    int err;



    post("[jackaudioport] remove scheduler ready daemon \n");

    fts_sched_remove(o);
    /* Halt scheduler main loop */
    post("[jackaudioport] halt scheduler \n");
    fts_sched_suspend();
    fts_log("[jackaudioport] client activated \n");

/*
  fts_log("[jackaudioport] sched_run_one_tick_without_select \n");
  fts_sched_run_one_tick_without_select(); 
*/
    post("[jackaudioport] scheduler ready daemon removed \n");

    post("[jackaudioport] activate jack client \n");
    jack_activate(this->client);

}

static void
jackaudioport_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
    jackaudioport_t* this = (jackaudioport_t*)o;

    fts_audioport_init(&this->head);

    fts_audioport_set_input_channels((fts_audioport_t*)this, 1);
    fts_audioport_set_input_function((fts_audioport_t*)this, jackaudioport_input);
    fts_audioport_set_output_channels((fts_audioport_t*)this, 1);
    fts_audioport_set_output_function((fts_audioport_t*)this, jackaudioport_output);

    /* JACK structure member initialisation */
    fts_log("[jackaudioport] init jackaudioport \n");    
    this->client = jack_client_new("jmax_jackaudioport");
    fts_log("[jackaudioport] client created \n");
    this->input_port = jack_port_register(this->client, "input", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    fts_log("[jackaudioport] input port registered \n");
    this->output_port = jack_port_register(this->client, "output", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    fts_log("[jackaudioport] output port registered \n");
    jack_set_process_callback(this->client, jackaudioport_process, (void*)this);
/*
    fts_sched_add(o, FTS_SCHED_ALWAYS);
*/
    /* memory allocation for output_buffer */
    /* 
       ARRRGGHHH 
       MEMORY LEAK HERE
    */
    this->input_buffer = fts_malloc(DEFAULT_FRAME_SIZE * sizeof(float));
    this->output_buffer = fts_malloc(DEFAULT_FRAME_SIZE * sizeof(float));
    this->nframes = DEFAULT_FRAME_SIZE;

    jackaudioport_halt(o, winlet, s, ac, at);
}

static void
jackaudioport_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
    jackaudioport_t* this = (jackaudioport_t*)o;

    fts_audioport_delete(&this->head);
    fts_log("[jackaudioport] deactivate jack client \n");
    jack_deactivate(this->client);
    fts_log("[jackaudioport] close jack client \n");
    jack_client_close(this->client);

    if (0 != this->input_buffer)
    {
	fts_free(this->input_buffer);
    }

    if (0 != this->output_buffer)
    {
	fts_free(this->output_buffer);
    }

    /* go back to fts scheduling */
    fts_sched_unsuspend();
}

static void jackaudioport_get_state( fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
    fts_set_object( value, o);
}

static fts_status_t jackaudioport_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
    fts_class_init( cl, sizeof( jackaudioport_t), 0, 0, 0);

    fts_method_define_varargs( cl, fts_SystemInlet, fts_s_init, jackaudioport_init);
    fts_method_define_varargs( cl, fts_SystemInlet, fts_s_delete, jackaudioport_delete);

/*
  fts_method_define_varargs( cl, fts_SystemInlet, fts_s_sched_ready, jackaudioport_halt);
*/
    fts_class_add_daemon( cl, obj_property_get, fts_s_state, jackaudioport_get_state);
    
    return fts_Success;
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
    {
	fts_audioport_set_default_class(jackaudioport_symbol);
    }

}
