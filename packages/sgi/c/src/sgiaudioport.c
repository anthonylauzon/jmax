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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

/*
 * This file's authors:
 * François Déchelle (dechelle@ircam.fr)
 */

#include <unistd.h>
#include <dmedia/audio.h>
#include <dmedia/audiofile.h>

#include <fts/fts.h>

#define DEFAULT_SAMPLING_RATE (44100.)
#define DEFAULT_FIFO_SIZE 256
#define DEFAULT_CHANNELS 2

typedef struct {
  fts_audioport_t head;
  ALport input_alport;
  ALport output_alport;
  stamp_t frames;
  int fifo_size;
  float *dac_fmtbuf;
  float *adc_fmtbuf;
} sgiaudioport_t;

static fts_symbol_t s_analog_in;
static fts_symbol_t s_analog_out;

static void sgiaudioport_input( fts_word_t *argv)
{
  sgiaudioport_t *port;
  int n, channels, ch, i, j;

  port = (sgiaudioport_t *)fts_word_get_pointer( argv+0);
  n = fts_word_get_int(argv + 1);
  channels = fts_audioport_get_input_channels( port);

  alReadFrames( port->input_alport, port->adc_fmtbuf, n);

  for ( ch = 0; ch < channels; ch++)
    {
      float *out = (float *) fts_word_get_pointer( argv + 2 + ch);

      j = ch;
      for ( i = 0; i < n; i++)
	{
	  out[i] = port->adc_fmtbuf[j];
	  j += channels;
	}
    }
}

static void sgiaudioport_output( fts_word_t *argv)
{
  sgiaudioport_t *port;
  int n, channels, ch, i, j;

  port = (sgiaudioport_t *)fts_word_get_pointer( argv+0);
  n = fts_word_get_int(argv + 1);
  channels = fts_audioport_get_output_channels( port);

  port->frames += n;

  for ( ch = 0; ch < channels; ch++)
    {
      float *in = (float *) fts_word_get_pointer( argv + 2 + ch);

      j = ch;
      for ( i = 0; i < n; i++)
	{
	  port->dac_fmtbuf[j] = in[i];
	  j += channels;
	}
    }

  alWriteFrames( port->output_alport, port->dac_fmtbuf, n);
}

static int sgiaudioport_xrun( fts_audioport_t *port)
{
  sgiaudioport_t *sgiport = (sgiaudioport_t *)port;
  stamp_t al_frames, al_time;

  alGetFrameTime( sgiport->output_alport, &al_frames, &al_time);

  if (al_frames > (sgiport->frames + sgiport->fifo_size))
    {
      int ret; 

      ret = al_frames - (sgiport->frames + sgiport->fifo_size);
      sgiport->frames = al_frames;

      return ret;
    }

  return 0;
}

static char *sgiaudioport_get_error_message( int err)
{
  switch (err) {
  case AL_BAD_CONFIG:
    return "Invalid config";

  case AL_BAD_DIRECTION:
    return "Invalid direction (neither \"r\" nor \"w\")";

  case AL_BAD_OUT_OF_MEM:
    return "Not enough memory";

  case AL_BAD_DEVICE_ACCESS:
    return "Audio hardware is not available or is improperly configured";

  case AL_BAD_DEVICE:
    return "Invalid device";

  case AL_BAD_NO_PORTS:
    return "No audio ports available";

  case AL_BAD_QSIZE:
    return "Invalid fifo size";

  case AL_BAD_SAMPFMT:
    return "Invalid sample format";

  case AL_BAD_FLOATMAX:
    return "Invalid float maximum";

  case AL_BAD_WIDTH:
    return "Invalid sample width";

  default:
    return "Unknown error";
  }
}

static int sgiaudioport_open( ALport *alport, char *device_name, char *alport_name, const char *dir, int channels, int fifo_size, int *perr)
{
  ALconfig config;
  int device_id;

  config = alNewConfig();

  if ( (*perr = alSetSampFmt( config, AL_SAMPFMT_FLOAT)) != 0)
    return 0;
  
  if ( (*perr = alSetFloatMax( config, 1.1f)) != 0)
    return 0;

  if ( (*perr = alSetWidth(config, AL_SAMPLE_24)) != 0)
    return 0;

  if ( (*perr = alSetChannels( config, channels)) != 0)
    return 0;

  if ( (*perr = alSetQueueSize( config, fifo_size)) != 0)
    return 0;

  if ( ! (device_id = alGetResourceByName( AL_SYSTEM, device_name, AL_DEVICE_TYPE)) )
    {
      *perr = oserror();
      return 0;
    }

  alSetDevice( config, device_id);

  if ( ! (*alport = alOpenPort( alport_name, dir, config)) )
    {
      *perr = oserror();
      return 0;
    }

  alFreeConfig( config);

  return 1;
}

static void sgiaudioport_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sgiaudioport_t *this = (sgiaudioport_t *)o;
  float sr;
  char input_device_name[256];
  char output_device_name[256];
  int err, input_channels, output_channels, sample_rate;
  stamp_t al_time;

  fts_audioport_init( &this->head);

  sr = fts_dsp_get_sample_rate();
  sample_rate = (int)sr ;
  this->fifo_size = DEFAULT_FIFO_SIZE;

  strcpy( input_device_name, fts_get_symbol_arg( ac, at, 0, s_analog_in));
  input_channels = fts_get_int_arg( ac, at, 1, DEFAULT_CHANNELS);
  strcpy( output_device_name, fts_get_symbol_arg( ac, at, 2, s_analog_out));
  output_channels = fts_get_int_arg( ac, at, 3, DEFAULT_CHANNELS);

  if (input_channels)
    {
      if ( !sgiaudioport_open( &this->input_alport, input_device_name, "jMax audioport input", "r", input_channels, this->fifo_size, &err))
	{
	  fts_object_set_error( o, sgiaudioport_get_error_message( err));
	  return;
	}

      fts_audioport_set_input_channels( (fts_audioport_t *)this, input_channels);
      fts_audioport_set_input_function( (fts_audioport_t *)this, sgiaudioport_input);
      this->adc_fmtbuf = (float *) fts_malloc(fts_dsp_get_tick_size() * input_channels * sizeof(float));
    }

  if (output_channels)
    {
      if ( !sgiaudioport_open( &this->output_alport, output_device_name, "jMax audioport output", "w", output_channels, this->fifo_size, &err))
	{
	  fts_object_set_error( o, sgiaudioport_get_error_message( err));
	  return;
	}

      fts_audioport_set_output_channels( (fts_audioport_t *)this, output_channels);
      fts_audioport_set_output_function( (fts_audioport_t *)this, sgiaudioport_output);
      this->dac_fmtbuf = (float *) fts_malloc(fts_dsp_get_tick_size() * output_channels * sizeof(float));

      alGetFrameTime( this->output_alport, &(this->frames), &al_time);

      fts_audioport_set_xrun_function( (fts_audioport_t *)this, sgiaudioport_xrun);
    }
}

static void sgiaudioport_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sgiaudioport_t *this = (sgiaudioport_t *)o;

  fts_audioport_delete( &this->head);

  if (this->adc_fmtbuf)
    fts_free( this->adc_fmtbuf);
  if (this->dac_fmtbuf)
    fts_free( this->dac_fmtbuf);

  if (this->input_alport)
    alCloseThis( this->input_alport);
  if (this->output_alport)
    alClosePort( this->output_alport);
}

static void sgiaudioport_get_state( fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  fts_set_object( value, o);
}

static void sgiaudioport_instantiate(fts_class_t *cl)
{
  fts_class_init( cl, sizeof( sgiaudioport_t), sgiaudioport_init, sgiaudioport_delete);

  fts_class_add_daemon( cl, obj_property_get, fts_s_state, sgiaudioport_get_state);
}

void sgiaudioport_config( void)
{
  fts_class_install( fts_new_symbol("sgiaudioport"), sgiaudioport_instantiate);

  s_analog_in = fts_new_symbol( "AnalogIn");
  s_analog_out = fts_new_symbol( "AnalogOut");
}

