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

typedef struct _alsaaudiomanager
{
    fts_object_t o;
  /* Need to store array of device name and corresponding alsa audioport */
} alsaaudiomanager_t;

/* From aplay source code by Jaroslav Kysela */
static void alsaaudiomanager_device_list(snd_pcm_stream_t stream, fts_bytestream_t* bytestream)
{
  snd_ctl_t *handle;
  int card, err, dev, idx;
  snd_ctl_card_info_t *info;
  snd_pcm_info_t *pcminfo;



  snd_ctl_card_info_alloca(&info);
  snd_pcm_info_alloca(&pcminfo);

  card = -1;
  if (snd_card_next(&card) < 0 || card < 0) {
    fts_spost(bytestream,"no soundcards found...");
    return;
  }
  while (card >= 0) {
    char name[32];
    sprintf(name, "hw:%d", card);
    if ((err = snd_ctl_open(&handle, name, 0)) < 0) {
      fts_spost(bytestream,"control open (%i): %s", card, snd_strerror(err));
      continue;
    }
    if ((err = snd_ctl_card_info(handle, info)) < 0) {
      fts_spost(bytestream,"control hardware info (%i): %s", card, snd_strerror(err));
      snd_ctl_close(handle);
      continue;
    }
    dev = -1;
    while (1) {
      unsigned int count;
      if (snd_ctl_pcm_next_device(handle, &dev)<0)
	fts_spost(bytestream,"snd_ctl_pcm_next_device");
      if (dev < 0)
	break;
      snd_pcm_info_set_device(pcminfo, dev);
      snd_pcm_info_set_subdevice(pcminfo, 0);
      snd_pcm_info_set_stream(pcminfo, stream);
      if ((err = snd_ctl_pcm_info(handle, pcminfo)) < 0) {
	if (err != -ENOENT)
	  fts_spost(bytestream,"control digital audio info (%i): %s", card, snd_strerror(err));
	continue;
      }
      fts_spost(bytestream, "card %i: %s [%s], device %i: %s [%s]\n",
	      card, snd_ctl_card_info_get_id(info), snd_ctl_card_info_get_name(info),
	      dev,
	      snd_pcm_info_get_id(pcminfo),
	      snd_pcm_info_get_name(pcminfo));
      count = snd_pcm_info_get_subdevices_count(pcminfo);
      fts_spost(bytestream, "  Subdevices: %i/%i\n", snd_pcm_info_get_subdevices_avail(pcminfo), count);
      for (idx = 0; idx < count; idx++) {
	snd_pcm_info_set_subdevice(pcminfo, idx);
	if ((err = snd_ctl_pcm_info(handle, pcminfo)) < 0) {
	  fts_spost(bytestream,"control digital audio playback info (%i): %s", card, snd_strerror(err));
	} else {
	  fts_spost(bytestream, "  Subdevice #%i: %s\n", idx, snd_pcm_info_get_subdevice_name(pcminfo));
	}
      }
    }
    snd_ctl_close(handle);
    if (snd_card_next(&card) < 0) {
      fts_spost(bytestream,"snd_card_next");
      break;
    }
  }
}




static void
alsaaudiomanager_print(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
/*   alsaaudiomanager_t* this = (alsaaudiomanager_t*)o; */

  fts_bytestream_t* bytestream = fts_post_get_stream(ac, at);  
  fts_spost(bytestream, "[alsaaudiomanager] All PCM PLAYBACK device \n");

  alsaaudiomanager_device_list(SND_PCM_STREAM_PLAYBACK, bytestream);

  fts_spost(bytestream, "[alsaaudiomanager] All PCM CAPTURE device \n");

  alsaaudiomanager_device_list(SND_PCM_STREAM_CAPTURE, bytestream);

/*
  post("Current PCM capture stream:\n");
  alsaaudiomanager_print_stream(&this->capture);
  post("Current PCM playback stream:\n");
  alsaaudiomanager_print_stream(&this->playback);
*/
}

static void
alsaaudiomanager_init(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  /* For debug call print method .... */
  

}


static void
alsaaudiomanager_delete(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{


}

static void alsaaudiomanager_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof( alsaaudiomanager_t), alsaaudiomanager_init, alsaaudiomanager_delete);

  /* debug print */
  fts_class_message_varargs(cl, fts_s_print, alsaaudiomanager_print);
}



/***********************************************************************
 *
 * Config
 *
 */
void alsaaudiomanager_config( void)
{
  fts_symbol_t s = fts_new_symbol("alsaaudiomanager");

  fts_class_install( s, alsaaudiomanager_instantiate);
}


/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
