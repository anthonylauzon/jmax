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

#include <fts/fts.h>
#include <ftsconfig.h>

#include <ftsprivate/config.h>
#include <ftsprivate/audio.h>

fts_class_t* audiolabel_type = NULL;

static fts_symbol_t audiolabel_s_name;

static fts_symbol_t audiolabel_s_label_type;

static fts_symbol_t audiolabel_s_in_channel;
static fts_symbol_t audioconfig_s_input_channel;

static fts_symbol_t audiolabel_s_out_channel;
static fts_symbol_t audioconfig_s_output_channel;


void
audiolabel_set_stereo(audiolabel_t* label, int stereo_flag)
{
#warning NOT YET IMPLEMENTED (audiolabel_set_stereo)
  /* check if stereo ccan be done with current channel, and current input/output audioport */

  label->stereo_flag = stereo_flag;
}

void
audiolabel_set_input_channel(audiolabel_t* label, int channel)
{
#warning NOT YET IMPLEMENTED (audiolabel_set_input_channel)
  /* check if input channel is available on input audioport, and if stereo can be done */
  if (1 == label->stereo_flag)
  {

  }

  label->input_channel = channel;
}

void
audiolabel_set_input_port(audiolabel_t* label, fts_audioport_t* port, fts_symbol_t name)
{
#warning NOT YET IMPLEMENTED (audiolabel_set_input_port)
  label->input_device = name;
}

void
audiolabel_set_output_channel(audiolabel_t* label, int channel)
{
#warning NOT YET IMPLEMENTED (audiolabel_set_output_channel)
  /* check if output channel is available on input audioport, and if stereo can be done */
  if (1 == label->stereo_flag)
  {

  }

  label->output_channel = channel;
}

void
audiolabel_set_output_port(audiolabel_t* label, fts_audioport_t* port, fts_symbol_t name)
{
#warning NOT YET IMPLEMENTED (audiolabel_set_output_port)
  label->output_device = name;
}



void
audiolabel_client_send_message(fts_object_t* o, fts_symbol_t selector, audiolabel_t* label, int index)
{
  fts_atom_t args[7];
  
  fts_set_int(args, index);
  fts_set_symbol(args + 1, label->name);
  fts_set_int(args + 2, label->stereo_flag);
  fts_set_symbol(args + 3, label->input_device);
  fts_set_int(args + 4, label->input_channel);
  fts_set_symbol(args + 5, label->output_device);
  fts_set_int(args + 6, label->output_channel);
  
  fts_client_send_message(o, fts_s_insert, 7, args);
}


static void
audiolabel_input(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
#warning NOT YET IMPLEMENTED (audiolabel_input), only test for communication
  audiolabel_t* self = (audiolabel_t*)o;
  fts_symbol_t name = fts_get_symbol(at);

  post("[audiolabel:] audiolabel_input, label name: %s, output device: %s\n", self->name, name);

  /* Make a query on audiomanager to retreive corresponding fts_audioport_t* */
}

static void
audiolabel_output(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
#warning NOT YET IMPLEMENTED (audiolabel_output), only test for communication
  audiolabel_t* self = (audiolabel_t*)o;
  fts_symbol_t name = fts_get_symbol(at);

  post("[audiolabel:] audiolabel_output, label name: %s, output device: %s\n", self->name, name);
  /* Make a query on audiomanager to retreive corresponding fts_audioport_t* */

}

static void
audiolabel_in_channel(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  audiolabel_t* self = (audiolabel_t*)o;
  int in_channel = fts_get_int(at);

  post("[audiolabel] label name: %s, in channel: %d\n", self->name, in_channel);

  audiolabel_set_input_channel(self, in_channel);
}


static void
audiolabel_out_channel(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  audiolabel_t* self = (audiolabel_t*)o;
  int out_channel = fts_get_int(at);

  post("[audiolabel] label name: %s, out channel: %d\n", self->name, out_channel);

  audiolabel_set_output_channel(self, out_channel);
}

static void
audiolabel_label_type(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  audiolabel_t* self = (audiolabel_t*)o;
  int label_type = fts_get_int(at);

  post("[audiolabel] label name: %s, label type: %d\n", self->name, label_type);

  audiolabel_set_stereo(self, label_type);
}

static void
audiolabel_init(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  audiolabel_t* self = (audiolabel_t*)o;
  fts_symbol_t name;
  int stereo_flag = 1;
  fts_symbol_t input_device = NULL;
  fts_audioport_t* input_audioport = NULL;
  int input_channel = 0;

  fts_symbol_t output_device = NULL;
  fts_audioport_t* output_audioport = NULL;
  int output_channel = 0;

  self->name = name;
  self->stereo_flag = stereo_flag;
  self->input_device = input_device;
  self->input_audioport = input_audioport;
  self->input_channel = input_channel;
  self->output_device = output_device;
  self->output_audioport = output_audioport;
  self->output_channel = output_channel;
}

static void
audiolabel_delete(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  audiolabel_t* self = (audiolabel_t*)o;

  if (NULL != self->input_audioport)
  {
    fts_object_release(self->input_audioport);
  }
  
  if (NULL != self->output_audioport)
  {
    fts_object_release(self->output_audioport);
  }

}

static void
audiolabel_instantiate(fts_class_t* cl)
{
  fts_class_init(cl, sizeof(audiolabel_t), audiolabel_init, audiolabel_delete);

  fts_class_message_varargs(cl, fts_s_input, audiolabel_input);
  fts_class_message_varargs(cl, fts_s_output, audiolabel_output);

  fts_class_message_varargs(cl, audiolabel_s_in_channel, audiolabel_in_channel);
  fts_class_message_varargs(cl, audiolabel_s_out_channel, audiolabel_out_channel);

  fts_class_message_varargs(cl, audiolabel_s_label_type, audiolabel_label_type);
}

void fts_audiolabel_config(void)
{    
  audiolabel_s_name = fts_new_symbol("__audiolabel");
  audiolabel_s_in_channel = fts_new_symbol("in_channel");  
  audiolabel_s_out_channel = fts_new_symbol("out_channel");
  audiolabel_s_label_type = fts_new_symbol("label_type");

  audiolabel_type = fts_class_install(audiolabel_s_name, audiolabel_instantiate);

}

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
