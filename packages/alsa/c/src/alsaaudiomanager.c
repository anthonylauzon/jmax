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


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <alsa/asoundlib.h>

#include <fts/fts.h>
#include "alsaaudio.h"

#define FTS_MAX_CHANNELS 32



/* 
   this function set default device if "default" is not already use,
   so we need to call this function after plugins scanning 
*/
void
alsaaudiomanager_scan_devices()
{
  int card;
  int err;
  int dev;
  char device_name[32];
  snd_ctl_t* ctl_handle;
  snd_ctl_card_info_t* card_info;
  snd_pcm_info_t* pcminfo;

  fts_audioport_t* port;
  fts_audioport_t* default_port;

  fts_atom_t at;
  fts_symbol_t s_devicename = 0;
  
  snd_ctl_card_info_alloca(&card_info);
  snd_pcm_info_alloca(&pcminfo);
  card = -1; 
  
  /* if card = -&, snd_card_next return the first available card */
  if (snd_card_next(&card) < 0 || card < 0)
  {
    fts_log("[alsaaudiomanager] No soundcards found \n");    
    return;
  }
  
  while (card >= 0)
  {
    sprintf(device_name, "hw:%d", card);
    err = snd_ctl_open(&ctl_handle, device_name, 0);
    if (err < 0)
    {
      fts_log("[alsaudiomanager] Cannot open a ctl: %s\n", snd_strerror(err));
      return;
    }
    err = snd_ctl_card_info(ctl_handle, card_info);
    if (err < 0)
    {
      fts_log("[alsaaudiomanager] Cannot get card info : %s\n", snd_strerror(err));
      snd_ctl_close(ctl_handle);
      return;
    }
    /* Here same output as snd_card_get_name(card_id, name) */
    fts_log("[alsaaudiomanager] ctl info card name :%s\n", snd_ctl_card_info_get_name(card_info));
    
    /* check for devices */
    dev = -1;
    while(1)
    {
      unsigned int count;
      if (snd_ctl_pcm_next_device(ctl_handle, &dev)< 0)
      {
	fts_log("[alsaaudiomanager] snd_ctl_pcm_get_next_device error \n");
      }
      if (dev < 0)
      {
	break;
      }
      snd_pcm_info_set_device(pcminfo, dev);
      snd_pcm_info_set_subdevice(pcminfo, 0);
/*       snd_pcm_info_set_stream(pcminfo, SND_PCM_STREAM_PLAYBACK); */
      err = snd_ctl_pcm_info(ctl_handle, pcminfo);
      if (err < 0)
      {
	if (err != -ENOENT)
	{
	  fts_log("[alsaaudiomanager] control digital audio info (%i): %s\n", card, snd_strerror(err));		    
	}
	continue;
      }
      fts_log("[alsaaudiomanager] Device Name : %s (hw:%d,%d)\n", snd_pcm_info_get_name(pcminfo), card, dev);
      
      snprintf(device_name, 32, "hw:%d,%d", card, dev);
      s_devicename = fts_new_symbol(device_name);
      fts_set_symbol(&at, s_devicename);
      port = (fts_audioport_t*)fts_object_create(alsaaudioport_type, 1, &at);
      if (NULL != port)
      {
	fts_object_refer((fts_object_t*)port);
	fts_audiomanager_put_port(fts_new_symbol(snd_pcm_info_get_name(pcminfo)), port);
	fts_log("[alsaaudiomanager] fts_audiomanager_put_port: %s\n", snd_pcm_info_get_name(pcminfo));

	/* check if default port exists */
	default_port = fts_audiomanager_get_port(fts_s_default);
	if (NULL != default_port)
	{
	  if (!fts_audioport_is_input(default_port)
	      || !fts_audioport_is_output(default_port))
	  {
	    fts_audiomanager_remove_port(fts_s_default);
	    fts_object_release(default_port);	    
	    if (fts_audioport_is_input(port)
		&&  fts_audioport_is_output(port))
	    {
	      fts_object_refer((fts_object_t*) port);
	      fts_audiomanager_put_port(fts_s_default, port);
	      post("[alsaaudiomanager] default is %s\n", snd_pcm_info_get_name(pcminfo));
	      fts_log("[alsaaudiomanager] default is %s\n", snd_pcm_info_get_name(pcminfo));
	    }	    
	  }
	}
	else
	{	  
	  if (fts_audioport_is_input(port)
	      &&  fts_audioport_is_output(port))
	  {
	    fts_object_refer((fts_object_t*) port);
	    fts_audiomanager_put_port(fts_s_default, port);
	    post("[alsaaudiomanager] default is %s\n", snd_pcm_info_get_name(pcminfo));
	    fts_log("[alsaaudiomanager] default is %s\n", snd_pcm_info_get_name(pcminfo));
	  }	  
	}	
      }
    }
    snd_card_next(&card);   
    snd_ctl_close(ctl_handle);
  }
}


void
alsaaudiomanager_scan_plugins(void)
{
  snd_config_t* top_conf;
  snd_config_t* cur_conf;
  snd_config_iterator_t conf_it;
  snd_config_iterator_t next_conf_it;
  fts_symbol_t s_device_name;

  int err = snd_config_update();
  if (err < 0)
  {
    fts_log("[alsaaudiomanager] error while getting alsa configuration: %s\n", snd_strerror(err));
    return;
  }

  err = snd_config_search(snd_config, "pcm", &top_conf);
  /* snd_config_for_each is a macro .... */
  snd_config_for_each(conf_it, next_conf_it, top_conf)
  {
    const char* id_configuration_node;
    snd_config_type_t config_type;
    cur_conf = snd_config_iterator_entry(conf_it);
    
    config_type = snd_config_get_type(cur_conf);
    err = snd_config_get_id(cur_conf, &id_configuration_node);
    if (err < 0)
    {
      fts_log("[alsaaudiomanager] Error while getting id configuration node : %s\n", snd_strerror(err));
    }
    else
    {
      fts_atom_t at;
      fts_audioport_t* port;
      fts_log("[alsaaudiomanager] Id Configuration Node: %s\n", id_configuration_node);
      s_device_name = fts_new_symbol(id_configuration_node);
      fts_set_symbol(&at, s_device_name);
      port = (fts_audioport_t*)fts_object_create(alsaaudioport_type, 1, &at);
      if (NULL != port)
      {
	fts_object_refer((fts_object_t*)port);
	fts_audiomanager_put_port(s_device_name, port);
	fts_log("[alsaaudiomanager] fts_audiomanager_put_port: %s\n", s_device_name);
      }
    }
    conf_it = snd_config_iterator_next(conf_it);
  }
  
}


snd_pcm_access_t 
alsaaudiomanager_convert_jmax_symbol_to_alsa_access(fts_symbol_t s_access)
{
  snd_pcm_access_t access;

  if (s_access == s_mmap_noninterleaved)
  {
    access = SND_PCM_ACCESS_MMAP_NONINTERLEAVED;
  }
  else if (s_access == s_mmap_interleaved)
  {
    access = SND_PCM_ACCESS_MMAP_INTERLEAVED;
  }
  else if (s_access == s_rw_noninterleaved)
  {
    access = SND_PCM_ACCESS_RW_NONINTERLEAVED;
  }
  else
  {
    access = SND_PCM_ACCESS_RW_INTERLEAVED;
  }

  return access;
}



int
alsaaudiomanager_get_channels_max(const char* device_name, int stream_mode)
{
  snd_pcm_t* handle;
  snd_pcm_hw_params_t* hwparams;
  int err;
  int max_channels;

  snd_pcm_hw_params_alloca(&hwparams);
  
  err = snd_pcm_open(&handle, device_name, stream_mode, SND_PCM_NONBLOCK);
  if (err < 0)
  {
    fts_log("[alsaaudiomanger] cannot open ALSA PCM device %s (%s)\n", device_name, snd_strerror(err));
    return err;
  }
  
  err = snd_pcm_hw_params_any(handle, hwparams);
  if (err < 0)
  {
    fts_log("[alsaaudiomanager] cannot get hardware configuration for device %s (%s)\n", device_name, snd_strerror(err));
    return err;
  }

  max_channels = snd_pcm_hw_params_get_channels_max(hwparams);
  if (FTS_MAX_CHANNELS < max_channels)
  {
    max_channels = FTS_MAX_CHANNELS;
  }

  fts_log("[alsaaudiomanager] device %s opened with max channels = %d \n", device_name, max_channels);
  snd_pcm_close(handle);
  
  return max_channels;
}

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
