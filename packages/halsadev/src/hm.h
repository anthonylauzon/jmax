/*
    Copyright (C) 2000 François Déchelle (dechelle@ircam.fr)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/*
  Based on ardour (http://ardour.sourceforge.net/) by Paul Barton-Davis 
*/

#ifndef _HM_H_
#define _HM_H_

#include <sys/asoundlib.h>
#include <linux/asoundid.h>

/*
  Structure: hm_channel_t
  Description:
    A structure to hold the necessary information for each channel
    of the device.
*/
typedef struct {
  /* the handle returned by snd_pcm_ope_subdevice */
  snd_pcm_t *handle;
  /* the mmap info */
  snd_pcm_mmap_control_t *mmap_control;
  /* the buffer */
  unsigned char *mmap_buffer;
} hm_channel_t;

typedef struct {
  /* number of channels, obtained by snd_ctl_pcm_info */
  int n_channels;
  /* fragment size in bytes */
  int frag_size;
  /* the channel structures */
  hm_channel_t *capture_channels;
  hm_channel_t *playback_channels;
  /* file descriptor used in select() */
  int fd;
  /* underrun counter */
  int underrun_count;
  /* the mmap control pointers for channel 0 (used for synchronization) */
  snd_pcm_mmap_control_t *capture_mc0;
  snd_pcm_mmap_control_t *playback_mc0;
  /* current capture and playback fragments */
  int capture_frag;
  int playback_frag;
} hm_t;

/*
  Function: hm_open
  Description:
    Initializes a hm_t structure, opens the ctl handle, configure the switches, initializes
    the channels.
  Arguments:
    [out] h                    a pointer to the hm_t structure
    [in]  card                 the card number
    [in]  dev                  the device number
    [in]  sample_rate          the sample rate
    [in]  fragment_samples     the fragment size in samples
  Returns:
    < 0 if error occured. The return value is the value of the ALSA call that caused
    an error
    0 if OK.
*/
extern int hm_open( hm_t *h, int card, int dev, int sample_rate, int fragment_samples);

/*
  Function: hm_close
  Description:
    Deinitializes a hm_t structure, close the handle.
  Arguments:
    [out] h                    a pointer to the hm_t structure
  Returns:
    < 0 if error occured. The return value is the value of the ALSA call that caused
    an error
    0 if OK.
*/
extern int hm_close( hm_t *h);

/*
  Function: hm_select
  Description:
    Calls select() on the file descriptor of the device. It will block until fragment
    is available.
  Arguments:
    [in]  h                    a pointer to the hm_t structure
  Returns:
    < 0 if error occured. The return value is the return value of select()
    0 if select() returned 0 (meaning no data) or underrun. If underrun,
    the underrun counter will increment.
    > 0 if OK (fragment is there)

*/
extern int hm_select( hm_t *h);

/*
  Function: hm_get_input_buffer
  Description:
    Returns a pointer to the input fragment buffer
  Arguments:
    [in]  h                    a pointer to the hm_t structure
    [in]  channel              the channel number
  Returns:
    a pointer to the input fragment buffer
*/
extern long *hm_get_input_buffer( hm_t *h, int channel);

/*
  Function: hm_get_output_buffer
  Description:
    Returns a pointer to the output fragment buffer
  Arguments:
    [in]  h                    a pointer to the hm_t structure
    [in]  channel              the channel number
  Returns:
    a pointer to the output fragment buffer
*/
extern long *hm_get_output_buffer( hm_t *h, int channel);


/*
  Function: hm_done_capture_fragment
  Description:
    Sets the data flag in the capture fragment. 
    This function must be called once the capture fragment has been copied.
  Arguments:
    [in]  h                    a pointer to the hm_t structure
    [in]  channel              the channel number
  Returns:
    none
*/
extern void hm_done_capture_fragment( hm_t *h, int channel);

/*
  Function: hm_done_playback_fragment
  Description:
    Sets the data flag in the playback fragment. 
    This function must be called once the playback fragment has been filled.
  Arguments:
    [in]  h                    a pointer to the hm_t structure
    [in]  channel              the channel number
  Returns:
    none
*/
extern void hm_done_playback_fragment( hm_t *h, int channel);

/*
  Function: hm_get_fragment_samples
  Description:
    Returns the fragment size in samples
  Arguments:
    [in]  h                    a pointer to the hm_t structure
  Returns:
    the fragment size in samples
*/
extern int hm_get_fragment_samples( hm_t *h);

/*
  Function: hm_get_n_channels
  Description:
    Returns the number of channels
  Arguments:
    [in]  h                    a pointer to the hm_t structure
  Returns:
    the number of channels
*/
extern int hm_get_n_channels( hm_t *h);

/*
  Function: hm_get_underrun_count
  Description:
    Returns the count of underruns (that always increases)
  Arguments:
    [in]  h                    a pointer to the hm_t structure
  Returns:
    the count of underruns.
*/
extern int hm_get_underrun_count( hm_t *h);

#endif
