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

#include <stdlib.h>
#include <string.h>
#if HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#include <ftsprivate/bmaxfile.h>
#include <ftsprivate/audio.h>
#include <ftsprivate/audioconfig.h>
#include <ftsprivate/midi.h>
#include <ftsprivate/config.h>
#include <ftsprivate/client.h>


#define AUDIO_CONFIG_DEFAULT_SAMPLE_RATE 44100.
#define AUDIO_CONFIG_DEFAULT_BUFFER_SIZE 1024

/****************************************************
 *
 * audio config
 *
 */

fts_class_t *fts_audioconfig_class;

/* array of sampling rates */
static fts_array_t audioconfig_sample_rates_array;

/* array of buffer size */
static fts_array_t audioconfig_buffer_sizes_array;


static fts_symbol_t audioconfig_s_unconnected;
static fts_symbol_t audioconfig_s_name;
static fts_symbol_t audioconfig_s_buffer_sizes;
static fts_symbol_t audioconfig_s_sampling_rates;

static fts_symbol_t audioconfig_s_buffer_size;
static fts_symbol_t audioconfig_s_sampling_rate;

static fts_symbol_t audioconfig_s_inputs;
static fts_symbol_t audioconfig_s_outputs;


static void
audioconfig_restore(fts_audioconfig_t* config)
{
  fts_atom_t a;

  fts_set_object(&a, config);
  fts_name_set_value(fts_get_root_patcher(), audioconfig_s_name, fts_null);
  fts_name_set_value(fts_get_root_patcher(), audioconfig_s_name, &a);
}


static void
audiomanagers_get_sample_rates(fts_array_t* sample_rates_array)
{
#warning NOT YET IMPLEMENTED (audiomanagers_get_sample_rates), dummy implementation to test communication between fts server and client
  /*
    loop on registered audio managers:
      foreach audiomanagers check which sample rate are available on audioport
      check if sample rate already in samples_rates_array
        if not add sample rate in array
  */
  fts_array_append_int(sample_rates_array,22050);
  fts_array_append_int(sample_rates_array,44100);
  fts_array_append_int(sample_rates_array,48000);
  fts_array_append_int(sample_rates_array,96000);
}

static void
audiomanagers_get_buffer_sizes(fts_array_t* buffer_sizes_array)
{
#warning NOT YET IMPLEMENTED (audiomanagers_get_buffer_sizes), dummy implementation to test communication between fts server and client
  /*
    loop on registered audio managers:
      foreach audiomanagers check which buffer size are available on audioport
      check if buffer size already in buffer_sizes_array
        if not add buffer size in array
  */

  fts_array_append_int(buffer_sizes_array, 128);
  fts_array_append_int(buffer_sizes_array, 256);
  fts_array_append_int(buffer_sizes_array, 512);
  fts_array_append_int(buffer_sizes_array, 1024);
}



static fts_symbol_t
audiomanagers_get_default_input(void)
{
#warning NOT YET IMPLEMENTED (audiomanagers_get_default_input)
  fts_symbol_t name = NULL;
/*   fts_audiomanager_t *mm; */
/*   fts_atom_t arg; */
  
/*   fts_set_pointer(&arg, &name); */

/*   for(mm = audiomanagers; mm != NULL && name == NULL; mm = mm->next) */
/*     fts_send_message((fts_object_t *)mm, fts_audiomanager_s_get_default_input, 1, &arg); */

  return name;
}

static fts_symbol_t
audiomanagers_get_default_output(void)
{
#warning NOT YET IMPLEMENTED (audiomanagers_get_default_output)
  fts_symbol_t name = NULL;
/*   fts_audiomanager_t *mm;  */
/*   fts_atom_t arg; */
  
/*   fts_set_pointer(&arg, &name); */

/*   for(mm = audiomanagers; mm != NULL && name == NULL; mm = mm->next) */
/*     fts_send_message((fts_object_t *)mm, fts_audiomanager_s_get_default_output, 1, &arg);  */

  return name;
}

static void 
audioconfig_update_devices(fts_audioconfig_t* config)
{
  int ac;
  fts_atom_t* at;
  fts_symbol_t *names;
  fts_audioport_t* port;
  fts_array_clear(&audioconfig_sample_rates_array);
  fts_array_clear(&audioconfig_buffer_sizes_array);

  /* get audio port names */
  names = fts_audiomanager_get_input_names();
  fts_client_start_message( (fts_object_t *)config, audioconfig_s_inputs);
  while (*names)
    {
      fts_client_add_symbol( (fts_object_t *)config, *names);
      port = fts_audiomanager_get_port(*names);
      fts_client_add_int((fts_object_t*)config, fts_audioport_get_max_channels(port, FTS_AUDIO_INPUT));
      names++;
    }
  fts_client_done_message( (fts_object_t *)config);

  names = fts_audiomanager_get_output_names();
  fts_client_start_message( (fts_object_t *)config, audioconfig_s_outputs);
  while (*names)
    {
      fts_client_add_symbol( (fts_object_t *)config, *names);
      port = fts_audiomanager_get_port(*names);
      fts_client_add_int((fts_object_t*)config, fts_audioport_get_max_channels(port, FTS_AUDIO_OUTPUT));
      names++;
    }
  fts_client_done_message( (fts_object_t *)config);
}

static void 
audioconfig_update_sample_rates(fts_audioconfig_t* config)
{
  int ac;
  fts_atom_t* at;

  /* get available sample rates from audiomanagers */
  audiomanagers_get_sample_rates(&audioconfig_sample_rates_array);

  ac = fts_array_get_size(&audioconfig_sample_rates_array);
  at = fts_array_get_atoms(&audioconfig_sample_rates_array);
  fts_client_send_message((fts_object_t*)config, audioconfig_s_sampling_rates, ac, at);  
}

static void
audioconfig_update_buffer_sizes(fts_audioconfig_t* config)
{
  int ac;
  fts_atom_t* at;

  /* get available buffer sizes from audiomanagers */
  audiomanagers_get_buffer_sizes(&audioconfig_buffer_sizes_array);

  ac = fts_array_get_size(&audioconfig_buffer_sizes_array);
  at = fts_array_get_atoms(&audioconfig_buffer_sizes_array);
  fts_client_send_message((fts_object_t*)config, audioconfig_s_buffer_sizes, ac, at);
}

static void 
audioconfig_update_labels(fts_audioconfig_t* config)
{
#warning NOT YET IMPLEMENTED (audioconfig_update_labels)
  fts_audiolabel_t* label = config->labels;
  int n = config->n_labels;
  int i;
  /* Make a query on audiomanagers to know which input and output audioport are available */
  /* check input and output audioports */
/*   for ( i = 0; i < n; ++i) */
/*   { */
/*     if (label->input_audioport */
/*   } */
}

static void 
audioconfig_erase_labels(fts_audioconfig_t* config)
{
  fts_audiolabel_t* label = config->labels;
  
  while (NULL != label)
  {
    fts_audiolabel_t* next = label->next;
    fts_object_release((fts_object_t*)label);
    label = next;
  }
  
  config->labels = NULL;
  config->n_labels = 0;
}

/* Return NULL if no such index */
static fts_audiolabel_t*
audioconfig_label_get_by_index(fts_audioconfig_t* config, int index)
{
  fts_audiolabel_t* label = config->labels;
  while(label && index--)
    label = label->next;

  return label;
}

/* Return NULL if no such label name */
fts_audiolabel_t*
fts_audioconfig_label_get_by_name(fts_audioconfig_t* config, fts_symbol_t name)
{
  fts_audiolabel_t* label = config->labels;
  while(label && label->name != name)
    label = label->next;

  return label;
}

static fts_audiolabel_t* 
audioconfig_label_insert(fts_audioconfig_t* config, int index, fts_symbol_t name)
{
  fts_audiolabel_t** p = &config->labels;
  fts_audiolabel_t* label;
  int n = index;
  fts_atom_t a[1];

  fts_set_symbol( a, name);
  label = (fts_audiolabel_t*)fts_object_create(fts_audiolabel_class, 1, a);

  fts_object_refer((fts_object_t*)label);

  /* insert label to list */
  while ((*p) && n--)
    p = &(*p)->next;

  label->next = (*p);
  *p = label;
  
  config->n_labels++;
  
  fts_config_set_dirty((fts_config_t*)fts_config_get(), 1);

  return label;
}


static void
audioconfig_label_remove(fts_audioconfig_t* config, int index)
{
  fts_audiolabel_t** p = &config->labels;
  int n = index;
  
  /* remove label and send to client */
  while((*p) && n--)
    p = &(*p)->next;

  if (*p)
  {
    fts_audiolabel_t* label = *p;

    *p = (*p)->next;
    config->n_labels--;
    fts_object_release((fts_object_t*)label);
  }

  /* send reomve to client */
  if(fts_object_has_id((fts_object_t *)config)) 
  {
    fts_atom_t arg;
      
    fts_set_int(&arg, index);
    fts_client_send_message((fts_object_t *)config, fts_s_remove, 1, &arg);
  }

  fts_config_set_dirty((fts_config_t*)fts_config_get(), 1);
}

void 
fts_audioconfig_set_defaults(fts_audioconfig_t* audioconfig)
{
  if(audioconfig != NULL)
  {
    fts_audiolabel_t *label = audioconfig_label_get_by_index(audioconfig, 0);
      
    if(label == NULL)
      label = audioconfig_label_insert(audioconfig, 0, fts_s_default);

#if 0
    if(label->input_audioport == NULL)
    {
      fts_symbol_t name = audiomanagers_get_default_input();
      fts_audioport_t *port = audiomanagers_get_input(name, fts_s_default);
	  
      audioconfig_label_set_input_port(audioconfig, label, 0, port, name);
    }

    if(label->output_audioport == NULL)
    {
      fts_symbol_t name = audiomanagers_get_default_output();
      fts_audioport_t *port = audiomanagers_get_output(name, fts_s_default);
	  
      audioconfig_label_set_output_port(audioconfig, label, 0, port, name);
    }
#endif

    fts_config_set_dirty((fts_config_t*)fts_config_get(), 0);
  }
}


/****************************************************
 *
 *  AUDIO configuration class
 *
 */

#define AUDIO_CONFIG_DEBUG

static void
audioconfig_clear(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_audioconfig_t* self = (fts_audioconfig_t*)o;

  audioconfig_erase_labels(self);
  audioconfig_label_insert(self, 0, fts_s_default);

  if (fts_object_has_id(o))
    fts_client_send_message(o, fts_s_clear, 0, 0);
}

/* name utility */
fts_symbol_t
fts_audioconfig_get_fresh_label_name(fts_audioconfig_t *config, fts_symbol_t name)
{
  const char *str = name;
  int len = strlen(str);
  char *new_str = alloca((len + 10) * sizeof(char));
  int num = 0;
  int dec = 1;
  int i;

  /* separate base name and index */
  for(i=len-1; i>=0; i--) 
    {
    if(len == (i + 1) && str[i] >= '0' && str[i] <= '9')
      num += (str[len = i] - '0') * dec;
    else
      new_str[i] = str[i];
      
    dec *= 10;
  }
  
  /* generate new label name */
  while(fts_audioconfig_label_get_by_name(config, name) != NULL) 
  {
    sprintf(new_str + len, "%d", ++num);
    name = fts_new_symbol(new_str);
  }

  return name;
}

static void
audioconfig_insert_label(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_audioconfig_t* self = (fts_audioconfig_t*)o;
  int index = fts_get_int(at);
  fts_symbol_t name = fts_get_symbol(at + 1);
  int n = 0;
  fts_audiolabel_t* label;

  if( name == fts_s_default)
    {
      label = fts_audioconfig_label_get_by_name( self, name);

      if( label == NULL)
	label = audioconfig_label_insert( self, 0, name);
    }
  else
    {
      /* check if name is not already used */
      if (fts_audioconfig_label_get_by_name(self, name) != NULL)
	name = fts_audioconfig_get_fresh_label_name(self, name);
  
      label = audioconfig_label_insert(self, index, name);
    }

  /* It seems that this method is never called with 6 arguments */
/*   if( ac == 6) */
/*     { */
/*       label->input_device = fts_get_symbol(at + 2); */
/*       label->input_channel = fts_get_int(at + 3); */
/*       label->output_device = fts_get_symbol(at + 4); */
/*       label->output_channel = fts_get_int(at + 5); */
/*     } */
  
  if (fts_object_has_id(o))
    {
      fts_atom_t args[7];
      fts_client_register_object((fts_object_t *)label, fts_get_client_id(o));
      /* send new label to client */
      
      fts_set_int(args, index);
      fts_set_int(args + 1, fts_get_object_id((fts_object_t*)label)); 
      fts_set_symbol(args + 2, fts_audiolabel_get_name(label));
      fts_set_symbol(args + 3, fts_audiolabel_get_port_name( label, FTS_AUDIO_INPUT));
      fts_set_int(args + 4, fts_audiolabel_get_channel( label, FTS_AUDIO_INPUT));
      fts_set_symbol(args + 5, fts_audiolabel_get_port_name( label, FTS_AUDIO_OUTPUT));
      fts_set_int(args + 6, fts_audiolabel_get_channel( label, FTS_AUDIO_OUTPUT));
      fts_client_send_message(o, fts_s_insert, 7, args);    
    }
}


static void
audioconfig_remove_label(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_audioconfig_t* self = (fts_audioconfig_t*)o;
  int index = fts_get_int(at);

  audioconfig_label_remove(self, index);
  audioconfig_update_devices(self);
  audioconfig_restore(self);
}


int audioconfig_check_sample_rate(fts_audioconfig_t* config, double sample_rate)
{
#warning NOT YET IMPLEMENTED (audioconfig_check_sample_rate)
  int success = 1;
  /* foreach used audioport:
       check if sample rate is available for selected channels
       if not:
          try to find another sample rate OR return error ?
       end if 
     end foreach
  */
  return success;
}

int audioconfig_check_buffer_size(fts_audioconfig_t* config, int buffer_size)
{
#warning NOT YET IMPLEMENTED (audioconfig_check_buffer_size)
  int success = 1;
  /* foreach used audioport:
       check if buffer size is available for selected channels
       if not:
          try to find another buffer size OR return error ?
       end if 
     end foreach
  */
  return success;

}


int 
fts_audioconfig_get_buffer_size(fts_audioconfig_t* self)
{
#warning TODO: Convert this function into a macro
  return self->buffer_size;
}

int 
fts_audioconfig_set_buffer_size(fts_audioconfig_t* config, int buffer_size)
{
#warning NOT YET IMPLEMENTED (fts_audioconfig_set_buffer_size)
  /* check if buffer size is available with current use audioport */
  if (audioconfig_check_buffer_size(config, buffer_size))
  {
    config->buffer_size = buffer_size;
    /* change buffer size for used audioport */
    return buffer_size;
  }
  else
  {
    /* error */
    return buffer_size;
  }
}

int
fts_audioconfig_get_sample_rate(fts_audioconfig_t* config)
{
#warning TODO: Convert this function into a macro
  return config->sample_rate;
}

int
fts_audioconfig_set_sample_rate(fts_audioconfig_t* config, int sample_rate)
{
#warning NOT YET IMPLEMENTED , need to change dsp code to set sample rate
  int set_sample_rate = 0;
  fts_atom_t arg;
  /* check if sample rate is available with current use audioport */
  if (audioconfig_check_sample_rate(config, sample_rate))
  {
    config->sample_rate = sample_rate;
/*     /\* change dsp sample rate *\/ */
     fts_dsp_set_sample_rate((double)sample_rate);
    /* change sample rate for used audioport */
    set_sample_rate = sample_rate;
  }
  else
  {
    /* error */
    set_sample_rate =  sample_rate;
  }
  
  fts_set_int(&arg, 48000);
  fts_client_send_message((fts_object_t*)config, audioconfig_s_sampling_rate, 1, &arg);

  return set_sample_rate;
}


void
fts_audioconfig_dump( fts_audioconfig_t *this, fts_bmax_file_t *f)
{
  fts_audiolabel_t* label = this->labels;
  int i = 0;
  fts_atom_t a[7];

  /* save buffer_size */
  fts_set_symbol(a, audioconfig_s_buffer_size);
  fts_set_int(a+1, this->buffer_size);
  fts_bmax_code_push_atoms(f, 2, a);
  fts_bmax_code_obj_mess(f, fts_s_audio_config, 2);
  fts_bmax_code_pop_args(f, 2);

  /* save sampling_rate */
  fts_set_symbol(a, audioconfig_s_sampling_rate);
  fts_set_int(a+1, this->sample_rate);
  fts_bmax_code_push_atoms(f, 2, a);
  fts_bmax_code_obj_mess(f, fts_s_audio_config, 2);
  fts_bmax_code_pop_args(f, 2);

  while(label)
    {
      fts_set_symbol(a, fts_s_insert);
      fts_set_int(a+1, i);
      fts_set_symbol(a+2, fts_audiolabel_get_name(label));

      fts_set_symbol(a+3, fts_audiolabel_get_port_name( label, FTS_AUDIO_INPUT));
      fts_set_int(a+4, fts_audiolabel_get_channel( label, FTS_AUDIO_INPUT));

      fts_set_symbol(a+5, fts_audiolabel_get_port_name( label, FTS_AUDIO_OUTPUT));
      fts_set_int(a+6, fts_audiolabel_get_channel( label, FTS_AUDIO_OUTPUT));

      fts_bmax_code_push_atoms(f, 7, a);
      fts_bmax_code_obj_mess(f, fts_s_audio_config, 7);
      fts_bmax_code_pop_args(f, 7);
      i++;

      label = label->next;
    }
}

static void 
audioconfig_buffer_size(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_audioconfig_t* self = (fts_audioconfig_t*)o;
  int buffer_size = fts_get_int(at);
  fts_atom_t arg;
      
  post("[audioconfig:] new buffer size: %d\n", fts_audioconfig_set_buffer_size(self, buffer_size));  

  fts_set_int(&arg, self->buffer_size);
  fts_client_send_message( o, audioconfig_s_buffer_size, 1, &arg);

  fts_config_set_dirty((fts_config_t*)fts_config_get(), 1);
}

static void
audioconfig_sample_rate(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
#warning NOT YET IMPLEMENTED (audioconfig_sample_rate)
  fts_audioconfig_t* self = (fts_audioconfig_t*)o;
  int sample_rate = fts_get_int(at);
  fts_atom_t arg;

  post("[audioconfig:] new sample rate: %d\n", fts_audioconfig_set_sample_rate(self, sample_rate));  
 
  fts_set_int(&arg, self->sample_rate);
  fts_client_send_message( o, audioconfig_s_sampling_rate, 1, &arg); 

  fts_config_set_dirty((fts_config_t*)fts_config_get(), 1);
}

static void
audioconfig_upload( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_audioconfig_t *self = (fts_audioconfig_t *)o;
  fts_audiolabel_t* label = self->labels;
  int index;
  fts_atom_t args[7];

  for (index = 0; index < self->n_labels; ++index)
  {
    if (fts_object_has_id(o))
    {
      if (!fts_object_has_id((fts_object_t*)label))
      {
	fts_client_register_object((fts_object_t *)label, fts_get_client_id(o));
      }

      /* send new label to client */
      fts_set_int(args, index);
      fts_set_int(args + 1, fts_get_object_id((fts_object_t*)label)); 
      fts_set_symbol(args + 2, fts_audiolabel_get_name(label));
      fts_set_symbol(args + 3, fts_audiolabel_get_port_name( label, FTS_AUDIO_INPUT));
      fts_set_int(args + 4, fts_audiolabel_get_channel( label, FTS_AUDIO_INPUT));
      fts_set_symbol(args + 5, fts_audiolabel_get_port_name( label, FTS_AUDIO_OUTPUT));
      fts_set_int(args + 6, fts_audiolabel_get_channel( label, FTS_AUDIO_OUTPUT));

      fts_client_send_message(o, fts_s_insert, 7, args);      
    }
    label = label->next;
  }

  audioconfig_update_labels(self);
  audioconfig_update_devices(self);

#ifdef AUDIO_CONFIG_DEBUG
  fts_log("[audioconfig] upload done \n");
#endif /* AUDIO_CONFIG_DEBUG */
  
  audioconfig_update_sample_rates(self);
  audioconfig_update_buffer_sizes(self);

  fts_set_int(args, self->sample_rate);
  fts_client_send_message(o, audioconfig_s_sampling_rate, 1, args); 
  fts_set_int(args, self->buffer_size);
  fts_client_send_message(o, audioconfig_s_buffer_size, 1, args); 
}


static void
audioconfig_set_to_defaults(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_audioconfig_t* self = (fts_audioconfig_t*)o;
  audioconfig_clear(o, winlet, fts_s_clear, 0, 0);
  audioconfig_upload(o, winlet, fts_s_upload, 0, 0);
  fts_config_set_dirty((fts_config_t*)fts_config_get(), 0);
}

static void
audioconfig_print(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_audioconfig_t* self = (fts_audioconfig_t*)o;
  fts_bytestream_t* stream = fts_post_get_stream(ac, at);
  fts_audiolabel_t* label = self->labels;

  fts_spost(stream, "[audioconfig] sampling rate: %d\t buffer size: %d\n", 
	    self->sample_rate, 
	    self->buffer_size);
  fts_spost(stream, "labels\n");

  while(NULL != label)
  {
    fts_spost(stream, "\t%s: input: %s channel: %d output: %s channel: %d\n",
	      fts_audiolabel_get_name( label), 
	      fts_audiolabel_get_port_name( label, FTS_AUDIO_INPUT), 
	      fts_audiolabel_get_channel( label, FTS_AUDIO_INPUT),
	      fts_audiolabel_get_port_name( label, FTS_AUDIO_OUTPUT), 
	      fts_audiolabel_get_channel( label, FTS_AUDIO_OUTPUT));
    label = label->next;
  }
}

static void
audioconfig_init(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_audioconfig_t* self = (fts_audioconfig_t*)o;

  self->sample_rate = AUDIO_CONFIG_DEFAULT_SAMPLE_RATE;
  self->buffer_size = AUDIO_CONFIG_DEFAULT_BUFFER_SIZE;
}

static void
audioconfig_delete(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_audioconfig_t* self = (fts_audioconfig_t*)o;
}

static void
audioconfig_instantiate(fts_class_t* cl)
{
  fts_class_init(cl, sizeof(fts_audioconfig_t), audioconfig_init, audioconfig_delete);

  fts_class_message_varargs(cl, fts_s_clear, audioconfig_clear);
  fts_class_message_varargs(cl, fts_s_default, audioconfig_set_to_defaults);

  fts_class_message_varargs(cl, fts_s_insert, audioconfig_insert_label);
  fts_class_message_varargs(cl, fts_s_remove, audioconfig_remove_label);

  fts_class_message_varargs(cl, fts_s_upload, audioconfig_upload);

  fts_class_message_varargs(cl, audioconfig_s_buffer_size, audioconfig_buffer_size);
  fts_class_message_varargs(cl, audioconfig_s_sampling_rate, audioconfig_sample_rate);
}


void fts_audioconfig_config(void)
{
  audioconfig_s_name = fts_new_symbol("__audioconfig");  
  audioconfig_s_unconnected = fts_new_symbol("-");

  audioconfig_s_buffer_sizes = fts_new_symbol("buffer_sizes");
  audioconfig_s_sampling_rates = fts_new_symbol("sampling_rates");

  audioconfig_s_buffer_size = fts_new_symbol("buffer_size");
  audioconfig_s_sampling_rate = fts_new_symbol("sampling_rate");

  audioconfig_s_inputs = fts_new_symbol("inputs");
  audioconfig_s_outputs = fts_new_symbol("outputs");

  fts_array_init(&audioconfig_sample_rates_array, 0, 0);
  fts_array_init(&audioconfig_buffer_sizes_array, 0, 0);

  fts_audioconfig_class = fts_class_install(audioconfig_s_name, audioconfig_instantiate);
}



/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
