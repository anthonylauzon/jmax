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
   The audio io  module.

   Include all the run-time and installation time handling of audio input output streams,
   i.e. real DACs, files, and audio streams in general (implemented as FTS devices).

*/

/* Include files */


#include <fts/sys.h>
#include <fts/lang.h>
#include <fts/runtime.h>
#include <fts/runtime/audio/audio.h>



/******************************************************************************/
/*                                                                            */
/*             Basic types                                                    */
/*                                                                            */
/******************************************************************************/

struct fts_audio_input_logical_device
{
  int active;

  fts_symbol_t name;

  fts_dev_t *device;

  int nchans;
  int vecsize;			/* the number of sample for each tick for this device */

  /* support for the adc object */

  float *buffer;		/* tmp buffer used in the multiple adc objject case */
  int scheduled_object_count;
  int object_count;
};


struct fts_audio_output_logical_device
{
  int active;

  fts_symbol_t name;

  fts_dev_t *device;

  int nchans;
  int vecsize;			/* the number of sample for each tick for this device */


  /* support for the dac object */

  float *buffer;		/* tmp buffer used in the multiple dac objject case */

  int scheduled_object_count;
  int object_count;

  int *channel_set;		/* used during the dspchain construction:
				   is 1 if an operation to set the channel content
				   as already been put in the dspchain;
				   reset by fts_audio_deactivate_devices.
				   */
};



/******************************************************************************/
/*                                                                            */
/*             Audio IO Module Declaration                                    */
/*                                                                            */
/******************************************************************************/

/* logical device declarations */

static fts_status_t fts_audio_dev_set_input_ldev(fts_dev_t *dev, int ac, const fts_atom_t *at);
static fts_dev_t   *fts_audio_dev_get_input_ldev(int ac, const fts_atom_t *at);
static fts_status_t fts_audio_unset_input_logical_dev(int ac, const fts_atom_t *at);
static fts_status_t fts_audio_reset_input_logical_dev(void);

static fts_status_t fts_audio_dev_set_output_ldev(fts_dev_t *dev, int ac, const fts_atom_t *at);
static fts_dev_t   *fts_audio_dev_get_output_ldev(int ac, const fts_atom_t *at);
static fts_status_t fts_audio_unset_output_logical_dev(int ac, const fts_atom_t *at);
static fts_status_t fts_audio_reset_output_logical_dev(void);


static void fts_audio_init(void);
static void fts_audio_shutdown(void);

static void fts_audio_null_dac(fts_word_t *args);

static fts_symbol_t default_input_name;	/* initted to _default_input_ */
static fts_symbol_t default_output_name;	/* initted to _default_output_ */

static fts_hash_table_t fts_audio_input_logical_device_table;
static fts_hash_table_t fts_audio_output_logical_device_table;


static int fts_audio_pending_close = 0; 

fts_module_t fts_audio_module = {"FTS Audio", "FTS Audio Input Output system ",
				    fts_audio_init, fts_audio_shutdown, 0};





static void
fts_audio_init(void)
{
  /* Install audio input and output logical devs */

  fts_declare_logical_dev(fts_new_symbol("in~"),
			  fts_sig_dev,
			  fts_audio_dev_set_input_ldev,
			  fts_audio_dev_get_input_ldev,
			  fts_audio_unset_input_logical_dev,
			  fts_audio_reset_input_logical_dev
			  );

  fts_declare_logical_dev(fts_new_symbol("out~"), 
			  fts_sig_dev,
			  fts_audio_dev_set_output_ldev,
			  fts_audio_dev_get_output_ldev,
			  fts_audio_unset_output_logical_dev,
			  fts_audio_reset_output_logical_dev
			  );

  /* init the audio logical device hash tables  */

  fts_hash_table_init(&fts_audio_input_logical_device_table);
  fts_hash_table_init(&fts_audio_output_logical_device_table);
}

void fts_audio_set_pending_close()
{
  fts_audio_pending_close = 1;
}

static void fts_audio_close_an_input_device(fts_symbol_t name, fts_atom_t *data, void *user_data)
{
  fts_atom_t a;
  fts_audio_input_logical_device_t *ldev = (fts_audio_input_logical_device_t *) fts_get_ptr(data);

  if (ldev->active)
    {
      fts_dev_t *dev;

      dev = ldev->device;
      fts_set_symbol(&a, ldev->name);
      fts_audio_unset_input_logical_dev(1, &a);
      fts_dev_close(dev);
    }
}


static void fts_audio_close_an_output_device(fts_symbol_t name, fts_atom_t *data, void *user_data)
{
  fts_atom_t a;
  fts_audio_output_logical_device_t *ldev = (fts_audio_output_logical_device_t *) fts_get_ptr(data);

  if (ldev->active)
    {
      fts_dev_t *dev;

      dev = ldev->device;
      fts_set_symbol(&a, ldev->name);
      fts_audio_unset_output_logical_dev(1, &a);
      fts_dev_close(dev);
    }
}


static void
fts_audio_shutdown(void)
{
  fts_hash_table_apply(&fts_audio_input_logical_device_table, fts_audio_close_an_input_device, 0);
  fts_hash_table_apply(&fts_audio_output_logical_device_table, fts_audio_close_an_output_device, 0);
}


/******************************************************************************/
/*                                                                            */
/*             Audio Stream  Installation                                     */
/*                                                                            */
/******************************************************************************/

/*
   The current implementation accept multiple logical devices for in
   and out; each logical device is identified by name; the first logical
   device installed is the default logical device, used when no name 
   is specified.

   A audio devuce is always active, i.e. the fts_audio system do i/o
   operation on all the installed logical device; the devices should be
   opened on the same tick they are installed; from the FTS side, this
   is not a problem in general,i.e.  the fts_audio system is active
   also if the dsp chain is off; this may be changed in the future,
   but would require adding a activate/deactivate operation to the
   devices (also needed for standby operations).

   For each audio logical device the handling sub system provide a
   number of access and control functions
   */


/* These functions create the logical device by need; so you can refer
   to a logical device also when is not created/set; of course, it will
   "work" only if installed; this solve creation order between Dacs/Adc and 
   logical devices.

   Created logical devices are never removed; the "active" flag is set to
   1 to say they are active.
 */

fts_audio_input_logical_device_t *fts_audio_get_input_logical_device(fts_symbol_t name)
{
  fts_atom_t data;
  int ret;

  if (name == (fts_symbol_t )0)
    name = default_input_name;

  ret = fts_hash_table_lookup(&fts_audio_input_logical_device_table, name, &data);

  if (ret)
    return (fts_audio_input_logical_device_t *)fts_get_ptr(&data);
  else
    {
      /* build an inactive logical device and put it in the
       table, and return it */

      fts_audio_input_logical_device_t *ldev;
      
      ldev = (fts_audio_input_logical_device_t *)fts_malloc(sizeof(fts_audio_input_logical_device_t));
      ldev->active = 0;
      ldev->object_count = 0;	/* object count can be valid for an non active logical device */
      ldev->name = name;
      ldev->buffer = 0;
      fts_set_ptr(&data, ldev);
      fts_hash_table_insert(&fts_audio_input_logical_device_table, name, &data);

      return ldev;
    }
}


fts_audio_output_logical_device_t *fts_audio_get_output_logical_device(fts_symbol_t name)
{
  fts_atom_t data;
  int ret;

  if (name == (fts_symbol_t )0)
    name = default_output_name;
  
  ret = fts_hash_table_lookup(&fts_audio_output_logical_device_table, name, &data);

  if (ret)
    return (fts_audio_output_logical_device_t *)fts_get_ptr(&data);
  else
    {
      /* build an inactive logical device and put it in the
       table, and return it */

      fts_audio_output_logical_device_t *ldev;

      ldev = (fts_audio_output_logical_device_t *)fts_malloc(sizeof(fts_audio_output_logical_device_t));
      ldev->active = 0;
      ldev->object_count = 0;	/* object count is significative also for not active ldev */
      ldev->name = name;
      ldev->buffer = 0;
      ldev->channel_set = 0;

      fts_set_ptr(&data, ldev);
      fts_hash_table_insert(&fts_audio_output_logical_device_table, name, &data);

      return ldev;
    }
}


void
fts_audio_set_default_in(fts_symbol_t in)
{
  default_input_name = in;
}


void fts_audio_set_default_out(fts_symbol_t out)
{
  default_output_name = out;
}

/* I/O buffer handling */

fts_dev_t *fts_audio_get_input_device(fts_audio_input_logical_device_t *ldev)
{
  return ldev->device;
}


fts_dev_t **fts_audio_get_input_device_pointer(fts_audio_input_logical_device_t *ldev)
{
  return &(ldev->device);
}


int fts_audio_get_input_channels(fts_audio_input_logical_device_t *ldev)
{
  return ldev->nchans;
}


int fts_audio_input_device_is_active(fts_audio_input_logical_device_t *ldev)
{
  return ldev->active;
}

int fts_audio_output_device_is_active(fts_audio_output_logical_device_t *ldev)
{
  return ldev->active;
}


fts_dev_t *fts_audio_get_output_device(fts_audio_output_logical_device_t *ldev)
{
  return ldev->device;
}

fts_dev_t **fts_audio_get_output_device_pointer(fts_audio_output_logical_device_t *ldev)
{
  return &(ldev->device);
}

int fts_audio_get_output_channels(fts_audio_output_logical_device_t *ldev)
{
  return ldev->nchans;
}


fts_symbol_t fts_audio_get_input_ftl_function(fts_audio_input_logical_device_t *ldev)
{
  return fts_dev_class_get_sig_get_fun_name(fts_dev_get_class(ldev->device));
}

fts_symbol_t fts_audio_get_output_ftl_function(fts_audio_output_logical_device_t *ldev)
{
  return fts_dev_class_get_sig_put_fun_name(fts_dev_get_class(ldev->device));
}
/*
   Install the device as input device, allocate the needed buffers and
   initialize the input_nchans  variable.
*/


static fts_status_t
fts_audio_dev_set_input_ldev(fts_dev_t *dev, int ac, const fts_atom_t *at)
{
  fts_symbol_t name;
  fts_audio_input_logical_device_t *ldev;

  /* We stop the dsp chain, because the FTL program
     can be inconsistent, now (changing buffers !!!)
     */

  fts_param_set_int(fts_s_dsp_on, 0);

  /* 
     Also, if the current devices are in "pending close" conditions,
     close all input and output audio devices; this currently happen 
     only at startup, but this mechanism should be used to change devices
     to avoid spikes in load */

  if (fts_audio_pending_close)
    {
      fts_audio_reset_output_logical_dev();
      fts_audio_reset_input_logical_dev();
      fts_audio_pending_close = 0;
    }
      
  /* we get the logical device name */

  if ((ac >= 1) && fts_is_symbol(at))
    name = fts_get_symbol(at);
  else
    name = fts_new_symbol("_default_input_ldev_");

  /* the dev system guarantee that we don't re-open a device that is active
     without closing it first.
   */

  ldev = fts_audio_get_input_logical_device(name);
  ldev->active = 1;
  ldev->device = dev;
  ldev->nchans = fts_sig_dev_get_nchans(dev);
  ldev->vecsize = fts_get_tick_size();

  ldev->buffer = (float *) fts_malloc(sizeof(float) * ldev->vecsize * ldev->nchans);

  fts_vecx_fzero(ldev->buffer, ldev->vecsize * ldev->nchans);

  /* house keeping preparation*/

  ldev->scheduled_object_count = 0;

  /* recompute the off dsp chain */

  dsp_make_dsp_off_chain();	


  return fts_Success;
}


static fts_dev_t  *
fts_audio_dev_get_input_ldev(int ac, const fts_atom_t *at)
{
  fts_symbol_t name;
  fts_audio_input_logical_device_t *ldev;

  if ((ac > 0) && fts_is_symbol(at))
    name = fts_get_symbol(at);
  else
    name = 0;

  ldev = fts_audio_get_input_logical_device(name);

  if (ldev && ldev->active)
    return ldev->device;
  else
    return 0;
}


static fts_status_t
fts_audio_unset_input_logical_dev(int ac, const fts_atom_t *at)
{
  fts_symbol_t name;
  fts_audio_input_logical_device_t *ldev;

  /* We stop the dsp chain, because the FTL program
     can be inconsistent, now (changing buffers !!!)
     */

  fts_param_set_int(fts_s_dsp_on, 0);

  if ((ac > 0) && fts_is_symbol(at))
    name = fts_get_symbol(at);
  else
    name = 0;

  ldev = fts_audio_get_input_logical_device(name);

  ldev->active = 0;

  if (ldev->buffer)
    {
      fts_free(ldev->buffer);
      ldev->buffer = 0;		/* security cleanup */
    }

  /* recompute the off dsp chain */

  dsp_make_dsp_off_chain();	

  return fts_Success;
}


static fts_status_t fts_audio_reset_input_logical_dev(void)
{
  fts_hash_table_apply(&fts_audio_input_logical_device_table, fts_audio_close_an_input_device, 0);

  return fts_Success;
}

/*
   Install the device as output device, allocate the needed buffers and
   initialize the output_nchans  variable.
*/


static fts_status_t
fts_audio_dev_set_output_ldev(fts_dev_t *dev, int ac, const fts_atom_t *at)
{
  int i;
  fts_symbol_t name;
  fts_audio_output_logical_device_t *ldev;

  /* We stop the dsp chain, because the FTL program
     can be inconsistent, now (changing buffers !!!)
     */

  fts_param_set_int(fts_s_dsp_on, 0);  

  /* 
     Also, if the current devices are in "pending close" conditions,
     close all input and output audio devices; this currently happen 
     only at startup, but this mechanism should be used to change devices
     to avoid spikes in load */

  if (fts_audio_pending_close)
    {
      fts_audio_reset_output_logical_dev();
      fts_audio_reset_input_logical_dev();
      fts_audio_pending_close = 0;
    }

  /* we get the logical device name */

  if ((ac >= 1) && fts_is_symbol(at))
    name = fts_get_symbol(at);
  else
    name = fts_new_symbol("_default_output_ldev_");

  /* the dev system guarantee that we don't re-open a device that is active
     without closing it first.
   */

  ldev = fts_audio_get_output_logical_device(name);

  ldev->active = 1;
  ldev->device = dev;
  ldev->nchans = fts_sig_dev_get_nchans(dev);
  ldev->vecsize = fts_get_tick_size();

  ldev->buffer = (float *) fts_malloc(sizeof(float) * ldev->vecsize * ldev->nchans);

  fts_vecx_fzero(ldev->buffer, ldev->vecsize * ldev->nchans);

  /* house keeping preparation*/

  ldev->scheduled_object_count = 0;

  ldev->channel_set = (int *)fts_malloc(sizeof(int) * ldev->nchans);

  for (i = 0; i < ldev->nchans; i++)
    ldev->channel_set[i] = 0; /* used during the dspchain construction */

  /* recompute the off dsp chain */

  dsp_make_dsp_off_chain();	

  return fts_Success;
}

static fts_dev_t  *
fts_audio_dev_get_output_ldev(int ac, const fts_atom_t *at)
{
  fts_symbol_t name;
  fts_audio_output_logical_device_t *ldev;

  if ((ac > 0) && fts_is_symbol(at))
    name = fts_get_symbol(at);
  else
    name = 0;

  ldev = fts_audio_get_output_logical_device(name);

  if (ldev && ldev->active)
    return ldev->device;
  else
    return 0;
}


static fts_status_t
fts_audio_unset_output_logical_dev(int ac, const fts_atom_t *at)
{
  fts_symbol_t name;
  fts_audio_output_logical_device_t *ldev;

  /* We stop the dsp chain, because the FTL program
     can be inconsistent, now (changing buffers !!!)
     */

  fts_param_set_int(fts_s_dsp_on, 0);  

  if ((ac > 0) && fts_is_symbol(at))
    name = fts_get_symbol(at);
  else
    name = 0;

  ldev = fts_audio_get_output_logical_device(name);

  ldev->active = 0;

  if (ldev->channel_set)
    {
      fts_free(ldev->channel_set);
      ldev->channel_set = 0;	/* security clean up */
    }

  if (ldev->buffer)
    {
      fts_free(ldev->buffer);
      ldev->buffer = 0;		/* security clean up */
    }

  dsp_make_dsp_off_chain();	/* recompute the off dsp chain */

  return fts_Success;
}


static fts_status_t fts_audio_reset_output_logical_dev(void)
{
  fts_hash_table_apply(&fts_audio_output_logical_device_table, fts_audio_close_an_output_device, 0);

  return fts_Success;
}


int
fts_audio_input_stream_is_installed(fts_audio_input_logical_device_t *ldev, int index)
{
  return ldev && ldev->active && (index < ldev->nchans);
}

int
fts_audio_output_stream_is_installed(fts_audio_output_logical_device_t *ldev, int index)
{
  return ldev && ldev->active && (index < ldev->nchans);
}


float *
fts_audio_get_input_buffer(fts_audio_input_logical_device_t *ldev, int index)
{
  return ldev->buffer + index * ldev->vecsize;
}


float *
fts_audio_get_output_buffer(fts_audio_output_logical_device_t *ldev, int index)
{
  return ldev->buffer + index * ldev->vecsize;
}


void
fts_audio_set_output_buffer_used(fts_audio_output_logical_device_t *ldev, int index)
{
  ldev->channel_set[index] = 1;
}

int
fts_audio_is_output_buffer_used(fts_audio_output_logical_device_t *ldev, int index)
{
  return  (ldev->channel_set[index] == 1);
}


/* output object housekeeping: 
   we count the output objects
   and the scheduled objects.
 */



void
fts_audio_add_output_object(fts_audio_output_logical_device_t *ldev)
{
  ldev->object_count++;
}

void
fts_audio_remove_output_object(fts_audio_output_logical_device_t *ldev)
{
  ldev->object_count--;
}

int
fts_audio_get_output_object_count(fts_audio_output_logical_device_t *ldev)
{
  return ldev->object_count;
}

void
fts_audio_add_scheduled_output_object(fts_audio_output_logical_device_t *ldev)
{
  ldev->scheduled_object_count++;
}


int
fts_audio_get_scheduled_output_object_count(fts_audio_output_logical_device_t *ldev)
{
  return ldev->scheduled_object_count;
}

void
fts_audio_add_input_object(fts_audio_input_logical_device_t *ldev)
{
  ldev->object_count++;
}

void
fts_audio_remove_input_object(fts_audio_input_logical_device_t *ldev)
{
  ldev->object_count--;
}

int
fts_audio_get_input_object_count(fts_audio_input_logical_device_t *ldev)
{
  return ldev->object_count;
}

void
fts_audio_add_scheduled_input_object(fts_audio_input_logical_device_t *ldev)
{
  ldev->scheduled_object_count++;
}


int
fts_audio_get_scheduled_input_object_count(fts_audio_input_logical_device_t *ldev)
{
  return ldev->scheduled_object_count;
}

/* call this before starting dsp */


static void fts_audio_activate_input_logical_device(fts_symbol_t name, fts_atom_t *data, void *user_data)
{
  fts_audio_input_logical_device_t *ldev = (fts_audio_input_logical_device_t *) fts_get_ptr(data);

  if (ldev->active)
    {
      ldev->scheduled_object_count = 0;
      fts_sig_dev_activate(ldev->device);
    }
}


static void fts_audio_activate_output_logical_device(fts_symbol_t name, fts_atom_t *data, void *user_data)
{
  int i;
  fts_audio_output_logical_device_t *ldev = (fts_audio_output_logical_device_t *) fts_get_ptr(data);

  if (ldev->active)
    {
      fts_vecx_fzero(ldev->buffer, ldev->vecsize * ldev->nchans);
      ldev->scheduled_object_count = 0;

      for (i = 0; i < ldev->nchans; i++)
	ldev->channel_set[i] = 0; 

      fts_sig_dev_activate(ldev->device);
    }
}

void
fts_audio_activate_devices(void)
{
  fts_hash_table_apply(&fts_audio_input_logical_device_table, fts_audio_activate_input_logical_device, 0);
  fts_hash_table_apply(&fts_audio_output_logical_device_table, fts_audio_activate_output_logical_device, 0);
}

/* call this after stopping  dsp */

static void fts_audio_deactivate_input_logical_device(fts_symbol_t name, fts_atom_t *data, void *user_data)
{
  fts_audio_input_logical_device_t *ldev = (fts_audio_input_logical_device_t *) fts_get_ptr(data);

  if (ldev->active)
    fts_sig_dev_deactivate(ldev->device);
}


static void fts_audio_deactivate_output_logical_device(fts_symbol_t name, fts_atom_t *data, void *user_data)
{
  int i;
  fts_audio_output_logical_device_t *ldev = (fts_audio_output_logical_device_t *) fts_get_ptr(data);

  if (ldev->active)
    {
      fts_vecx_fzero(ldev->buffer, ldev->vecsize * ldev->nchans);
      fts_sig_dev_deactivate(ldev->device);
    }
}


void fts_audio_deactivate_devices(void)
{
  fts_hash_table_apply(&fts_audio_input_logical_device_table, fts_audio_deactivate_input_logical_device, 0);
  fts_hash_table_apply(&fts_audio_output_logical_device_table, fts_audio_deactivate_output_logical_device, 0);
}


/******************************************************************************/
/*                                                                            */
/*             Compilation support (zeroing unused devices)                   */
/*                                                                            */
/******************************************************************************/



/* Generate ftl code to put zeros in all the outputs */

static void fts_audio_add_out_zero_fun_uncond(fts_symbol_t name, fts_atom_t *data, void *user_data)
{
  fts_audio_output_logical_device_t *ldev = (fts_audio_output_logical_device_t *) fts_get_ptr(data);

  if (ldev->active)
    {
      int i;
      fts_atom_t *argv;
      
      argv = (fts_atom_t *) fts_malloc(sizeof(fts_atom_t) * (ldev->nchans + 3));

      fts_set_ptr (argv + 0, &(ldev->device));
      fts_set_long(argv + 1, ldev->nchans);
      fts_set_long(argv + 2, ldev->vecsize);

      for (i = 0; i < ldev->nchans; i++)
	fts_set_symbol(argv + 3 + i, fts_s_sig_zero);

      dsp_add_funcall(fts_audio_get_output_ftl_function(ldev), 3 + ldev->nchans, argv);	  

      fts_free(argv);
    }
}


static void fts_audio_add_in_read_fun_uncond(fts_symbol_t name, fts_atom_t *data, void *user_data)
{
  fts_audio_input_logical_device_t *ldev = (fts_audio_input_logical_device_t *) fts_get_ptr(data);

  if (ldev->active)
    {
      int i;
      fts_atom_t *argv;
      
      argv = (fts_atom_t *) fts_malloc(sizeof(fts_atom_t) * (ldev->nchans + 3));

      fts_set_ptr (argv + 0, &(ldev->device));
      fts_set_long(argv + 1, ldev->nchans);
      fts_set_long(argv + 2, ldev->vecsize);

      for (i = 0; i < ldev->nchans; i++)
	fts_set_ptr(argv + 3 + i, fts_audio_get_input_buffer(ldev, i));

      dsp_add_funcall(fts_audio_get_input_ftl_function(ldev), 3 + ldev->nchans, argv);	  

      fts_free(argv);
    }
}


void fts_audio_add_all_zero_fun(void)
{
  fts_hash_table_apply(&fts_audio_input_logical_device_table, fts_audio_add_in_read_fun_uncond, 0);
  fts_hash_table_apply(&fts_audio_output_logical_device_table, fts_audio_add_out_zero_fun_uncond, 0);
}


static void fts_audio_add_zero_fun_unused(fts_symbol_t name, fts_atom_t *data, void *user_data)
{
  fts_audio_output_logical_device_t *ldev = (fts_audio_output_logical_device_t *) fts_get_ptr(data);

  if (ldev->active && (ldev->object_count == 0))
    {
      int i;
      fts_atom_t *argv;
      
      argv = (fts_atom_t *) fts_malloc(sizeof(fts_atom_t) * (ldev->nchans + 3));

      fts_set_ptr (argv + 0, &(ldev->device));
      fts_set_long(argv + 1, ldev->nchans);
      fts_set_long(argv + 2, ldev->vecsize);

      for (i = 0; i < ldev->nchans; i++)
	fts_set_symbol(argv + 3 + i, fts_s_sig_zero);

      dsp_add_funcall(fts_audio_get_output_ftl_function(ldev), 3 + ldev->nchans, argv);	  

      fts_free(argv);
    }
}


static void fts_audio_add_in_read_fun_unused(fts_symbol_t name, fts_atom_t *data, void *user_data)
{
  fts_audio_input_logical_device_t *ldev = (fts_audio_input_logical_device_t *) fts_get_ptr(data);

  if (ldev->active && (ldev->object_count == 0))
    {
      int i;
      fts_atom_t *argv;
      
      argv = (fts_atom_t *) fts_malloc(sizeof(fts_atom_t) * (ldev->nchans + 3));

      fts_set_ptr (argv + 0, &(ldev->device));
      fts_set_long(argv + 1, ldev->nchans);
      fts_set_long(argv + 2, ldev->vecsize);

      for (i = 0; i < ldev->nchans; i++)
	fts_set_ptr(argv + 3 + i, fts_audio_get_input_buffer(ldev, i));

      dsp_add_funcall(fts_audio_get_input_ftl_function(ldev), 3 + ldev->nchans, argv);	  

      fts_free(argv);
    }
}


void fts_audio_add_unused_zero_fun(void)
{
  fts_hash_table_apply(&fts_audio_input_logical_device_table, fts_audio_add_in_read_fun_unused, 0);
  fts_hash_table_apply(&fts_audio_output_logical_device_table, fts_audio_add_zero_fun_unused, 0);
}
