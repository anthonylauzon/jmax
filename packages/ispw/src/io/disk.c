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
 * New device based dtd; currently, only readsf~ implemented.
 * This file is platform independent; actual platform dependencies
 * are in the DtD devices.
 *
 */

#include "fts.h"

#include <string.h>

/*
 * readsf~
 *
 * Syntax:
 *
 *        readsf~ [(int) <nchans>] 
 *
 * The number of channels fix the number of outlets; note the effect of
 * a mismatch of nchans with the actual number of channels in the
 * file depend on the platform; on the SGI, the file channels will
 * be automatically mixed down to the object channels.
 * 
 * Accept the following messages, like in the ISPW:
 *
 * 
 * open file_name
 * <int> 1 -> start play, or resume playing after a pause
 * <int> 0 -> stop play, and close the file.
 *
 * And a few new messages:
 *
 * <symbol> start -> start play, or resume playing after a pause
 * <symbol> pause ->  suspend playing
 * <symbol> resume -> stop play, and close the file.
 *
 * About future expansion, the asynchnonius architecture prevent
 * the implementation of a reasonable "jump" (or "seek") command,
 * that do direct real time acces to a file location.
 * We can envisage a "reopen" command, that reopen a closed file 
 * (or close a currently running file) at a given position; but the command
 * will imply a delay, possibly in the order of a few tens of milliseconds.
 * The only way to do "playlists" is to create a playlist object, and a play
 * list device that actually execute the playlist, doing all the necessary
 * preloading, so that no real-time switching is needed.
 *
 * On the implementational level, this object use directly fts devices,
 * do not go thru the logical device structure; in order to do this, the 
 * fts_dev device API has been extended to support all the access/manipulation
 * operation needed (in fact, this is the first component to use directly devices).
 * Also, the readsf put method put directly a call device function and do not
 * define its own function.
 *
 * Also note that this object is not really related to disks; it will
 * work with any signal device, including network communication; it
 * may be quite usefull, the day that we decide to start to actually
 * exploit the potentiality of the architecture we have built, instead
 * of continuing to add only experimental features.
 */


typedef struct
{
  fts_object_t _o;
  fts_symbol_t file_name;
  fts_dev_t *device;
  enum {idle, playing, pause} status;
  int nchans;
} readsf_t;

/* Service functions */
static void readsf_file_open(readsf_t *this)
{
  fts_atom_t a[3];
  char file_path[1024];

  if(fts_file_get_read_path(fts_symbol_name(this->file_name), file_path))
    {  
      /* Build the open arg list: current version, use the default device fifosize and fileblock size */
      fts_set_symbol(&a[0], fts_new_symbol_copy(file_path));
      fts_set_symbol(&a[1], fts_new_symbol("channels"));
      fts_set_int(&a[2], this->nchans);
      
      /* Open the device; note that the device is opened not active !! */
      fts_dev_open(&(this->device), fts_new_symbol("readsf"), 3, a);
      
      this->status = pause;
    }
  else
    post("readsf~: cannot open file '%s' for reading\n", fts_symbol_name(this->file_name));
}


static void
readsf_file_close(readsf_t *this)
{
  fts_dev_close(this->device);
  this->device = 0;
  this->status = idle;
}


static void
readsf_file_start(readsf_t *this)
{
  fts_sig_dev_activate(this->device);
  this->status = playing;
}


static void
readsf_file_pause(readsf_t *this)
{
  fts_sig_dev_deactivate(this->device);
  this->status = pause;
}

/* Methods */

static void 
readsf_start(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t *this = (readsf_t *)o;

  if (this->status == pause)
    readsf_file_start(this);
}

static void
readsf_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t *this = (readsf_t *)o;

  if (this->status == playing)
    readsf_file_pause(this);
  else if (this->status == pause)
    readsf_file_start(this);
}

static void
readsf_pause(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t *this = (readsf_t *)o;

  if (this->status == playing)
    readsf_file_pause(this);
}


static void
readsf_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t *this = (readsf_t *)o;

  if (this->status != idle)
    readsf_file_close(this);
}


static void
readsf_open(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  readsf_t *this = (readsf_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);

  if (this->status != idle)
    readsf_file_close(this);

  if (file_name)
    {
      this->file_name = file_name;
      readsf_file_open(this);
    }
}

static void
readsf_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t *this = (readsf_t *)o;
  int n = fts_get_int_arg(ac, at, 0, 0);

  if (n == 0 && this->status == playing)
    readsf_file_pause(this);
  else if (n == 1 && this->status == pause)
    readsf_file_start(this);
}


static void
readsf_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t *this  = (readsf_t *)o;

  switch(this->status)
    {
    case idle:
      post("readsf~: idle\n");
      break;
    case playing:
      post("readsf~: opened, playing\n");
      break;
    case pause:
      post("readsf~: opened, paused\n");
      break;
    }
}


/* The put method put the following arguments:
   <*dev> <nchans> <nvec> <v1> ... <vn>
 */

static void
readsf_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t *this = (readsf_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);
  fts_atom_t *argv;
  int i;
  fts_dev_class_t *dev_class;

  dev_class = fts_dev_class_get_by_name(fts_new_symbol("readsf"));
  if (!dev_class)
    {
      post( "reasf~: error: cannot find Direct-To-Disk device\n");
      return;
    }

  argv = (fts_atom_t *) fts_malloc(sizeof(fts_atom_t) * (3 + this->nchans));

  fts_set_ptr(argv + 0, &(this->device));
  fts_set_long(argv + 1, this->nchans);
  fts_set_long(argv + 2, fts_dsp_get_output_size(dsp, 0));

  for (i = 0; i < this->nchans; i++)
    fts_set_symbol(argv + 3 + i, fts_dsp_get_output_name(dsp, i));

  dsp_add_funcall(fts_dev_class_get_sig_get_fun_name( dev_class), 3 + this->nchans, argv);

  fts_free(argv);
}


static void
readsf_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t *this = (readsf_t *)o;
  int nchans = fts_get_int_arg(ac, at, 1, 1); /* ignore other args */

  if (nchans < 1)
    this->nchans = 1;
  else
    this->nchans = nchans;

  this->device = 0;
  this->status = idle;

  dsp_list_insert(o); /* just put object in list */
}


static void
readsf_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t *this = (readsf_t *)o;

  if (this->status != idle)
    readsf_file_close(this);

  dsp_list_remove(o);
}


static fts_status_t
readsf_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int nchans, i;
  fts_symbol_t a[4];

  nchans = fts_get_long_arg(ac, at, 1, 1);

  if (nchans < 1)
    nchans = 1;

  fts_class_init(cl, sizeof(readsf_t), 1, nchans, 0);

  a[0] = fts_s_symbol;
  a[1] = fts_s_int;
  a[2] = fts_s_int;
  a[3] = fts_s_int;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, readsf_init, 4, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, readsf_delete, 0, 0);

  a[0] = fts_s_ptr;
  fts_method_define(cl, fts_SystemInlet, fts_s_put, readsf_put, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, readsf_number, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, readsf_number, 1, a);

  fts_method_define_varargs(cl, 0, fts_s_bang, readsf_bang);
  fts_method_define_varargs(cl, 0, fts_new_symbol("start"), readsf_start);
  fts_method_define_varargs(cl, 0, fts_new_symbol("stop"), readsf_stop);
  fts_method_define_varargs(cl, 0, fts_new_symbol("pause"), readsf_pause);

  a[0] = fts_s_symbol;
  a[1] = fts_s_int;
  a[2] = fts_s_symbol;
  fts_method_define_optargs(cl, 0, fts_new_symbol("open"),  readsf_open, 3, a, 0);

  fts_method_define(cl, 0, fts_new_symbol("print"), readsf_print, 0, 0);

  dsp_sig_inlet(cl, 0);	/* only for order forcing (usefull ?) */

  for (i = 0; i < nchans; i++)
    dsp_sig_outlet(cl, i);

  return fts_Success;
}


/*
 * writesf~
 *
 * Syntax:
 *
 *        writesf~ [(int) <nchans>] [(symbol) <format>]
 *
 * The number of channels fix the number of outlets; note the effect of
 * a mismatch of nchans with the actual number of channels in the
 * file depend on the platform; on the SGI, the file channels will
 * be automatically mixed down to the object channels.
 *
 * Note that the file will be written at the current sampling rate;
 * i.e. if running at 44100, a writesf~ after a down will write 
 * a file with a sampling rate of 22050.
 * 
 * Accept the following messages, like in the ISPW:
 *
 * 
 * open file_name
 * <int> 1 -> start writing, or resume writing after a pause
 * <int> 0 -> stop writing, and close the file.
 *
 * And a few new messages:
 *
 * <symbol> start -> start writing, or resume writing after a pause
 * <symbol> pause ->  suspend writing
 * <symbol> resume -> stop writing, and close the file.
 *
 * About future expansion, the asynchnonius architecture prevent
 * the implementation of a reasonable "jump" (or "seek") command,
 * that do direct real time acces to a file location.
 * We can envisage a "reopen" command, that reopen a closed file 
 * (or close a currently running file) at a given position; but the command
 * will imply a delay, possibly in the order of a few tens of milliseconds.
 * The only way to do "playlists" is to create a playlist object, and a play
 * list device that actually execute the playlist, doing all the necessary
 * preloading, so that no real-time switching is needed.
 *
 * On the implementational level, this object use directly fts devices,
 * do not go thru the logical device structure; in order to do this, the 
 * fts_dev device API has been extended to support all the access/manipulation
 * operation needed (in fact, this is the first component to use directly devices).
 * Also, the writesf put method put directly a call device function and do not
 * define its own function.
 *
 * Also note that this object is not really related to disks; it will
 * work with any signal device, including network communication; it
 * may be quite usefull, the day that we decide to start to actually
 * exploit the potentiality of the architecture we have built, instead
 * of continuing to add only experimental features.
 */

typedef struct
{
  fts_object_t _o;
  fts_symbol_t file_name;
  fts_symbol_t format;
  float sampling_rate;
  int nchans;
  
  fts_dev_t *device;

  enum
  {
    writesf_idle,
    writesf_pending_pausing,
    writesf_pausing,
    writesf_pending_playing,
    writesf_playing
  } status;

} writesf_t;

/* Service functions */

static fts_status_t 
writesf_file_open(writesf_t *this)
{
  char file_path[1024];
  fts_status_t ret;
  fts_atom_t a[7];

  /* Build the open arg list: current version, use the default device fifosize and fileblock size */
  fts_file_get_write_path(fts_symbol_name(this->file_name), file_path);
  
  /* Build the open arg list: current version, use the default device fifosize and fileblock size */
  fts_set_symbol(&a[0], fts_new_symbol_copy(file_path));
  fts_set_symbol(&a[1], fts_new_symbol("channels"));
  fts_set_int(&a[2], this->nchans); 
  fts_set_symbol(&a[3], fts_s_sampling_rate);
  fts_set_float(&a[4], this->sampling_rate);
  
  if (this->format != fts_s_void)
    {
      fts_set_symbol(&a[5], fts_new_symbol("format"));
      fts_set_symbol(&a[6], this->format);
      
      /* Open the device; note that the device is opened not active !! */      
      return fts_dev_open(&(this->device), fts_new_symbol("writesf"), 7, a);
    }
  else
    /* Open the device; note that the device is opened not active !! */      
    return fts_dev_open(&(this->device), fts_new_symbol("writesf"), 5, a);

  /* post("writesf~: cannot open file '%s' for writing\n", fts_symbol_name(this->file_name)); */
}


static void
writesf_file_close(writesf_t *this)
{
  fts_dev_close(this->device);
  this->device = 0;
  this->status = writesf_idle;
}



/* Methods */

static void 
writesf_start(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  writesf_t *this = (writesf_t *)o;

  switch (this->status)
    {
    case writesf_idle:
      /* Ignore if idle */
      break;
    case writesf_playing:
      /* Ignore if playing */
      break;
    case writesf_pausing:
      /* Start the recording if paused */
      fts_sig_dev_activate(this->device);
      this->status = writesf_playing;
      break;
    case writesf_pending_playing:
      /* Ignore if pending playing */
      break;
    case writesf_pending_pausing:
      /* Set to pending playing if pending pause */
      this->status = writesf_pending_playing;
      break;
    }
}


static void
writesf_pause(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  writesf_t *this = (writesf_t *)o;

  switch (this->status)
    {
    case writesf_idle:
      /* Ignore if idle */
      break;
    case writesf_playing:
      /* Pause if playing */
      fts_sig_dev_deactivate(this->device);
      this->status = writesf_pausing;
      break;
    case writesf_pausing:
      /* Ignore if already pause */
      break;
    case writesf_pending_playing:
      /* Set to pending pause if pending playing */
      this->status = writesf_pending_pausing;
      break;
    case writesf_pending_pausing:
      /* Ignore if pending pause  */
      break;
    }
}


static void
writesf_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  writesf_t *this = (writesf_t *)o;

  switch (this->status)
    {
    case writesf_idle:
      /* Ignore if idle */
      break;
    case writesf_playing:
      /* Close file if playing */
      this->status = writesf_idle;
      writesf_file_close(this);
      break;
    case writesf_pausing:
      /* Close file if pause */
      this->status = writesf_idle;
      writesf_file_close(this);
      break;
    case writesf_pending_playing:
      /* Reset status if pending playing */
      this->status = writesf_idle;
      break;
    case writesf_pending_pausing:
      /* Reset status if pending pause */
      this->status = writesf_idle;
      break;
    }
}


static void
writesf_open(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  writesf_t *this = (writesf_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);
  fts_symbol_t format = fts_get_symbol_arg(ac, at, 1, fts_s_void);

  /* Clean up if needed */
  writesf_stop(o, 0, 0, 0, 0);

  /* Store the state */
  if (file_name)
    {
      this->file_name = file_name;
      this->format   = format;
    }

  /* Open or set the state */
  if (dsp_is_running())
    {
      fts_status_t ret;

      ret = writesf_file_open(this);

      if (ret != fts_Success)
	post("writesf~: cannot open file '%s' for reading\n", fts_symbol_name(this->file_name));
      else
	this->status = writesf_pausing;
    }
  else
    this->status = writesf_pending_pausing;
}


static void
writesf_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  writesf_t *this = (writesf_t *)o;
  int n = fts_get_int_arg(ac, at, 0, 0);

  if (n == 0 && this->status >= writesf_playing)
    writesf_stop(o, winlet, s, ac, at);
  else if (n == 1  && this->status >= writesf_pausing)
    writesf_start(o, winlet, s, ac, at);
}

static void
writesf_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  writesf_t *this = (writesf_t *)o;

  if (this->status >= writesf_playing)
    writesf_pause(o, winlet, s, ac, at);
  else if (this->status >= writesf_pausing)
    writesf_start(o, winlet, s, ac, at);
}

static void
writesf_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  writesf_t *this  = (writesf_t *)o;

  switch (this->status)
    {
    case writesf_idle:
      post("writesf~: idle\n");
      break;
    case writesf_playing:
      post("writesf~: opened, writing\n");
      break;
    case writesf_pausing:
      post("writesf~: opened, paused\n");
      break;
    case writesf_pending_playing:
      post("writesf~: opened, ready to write, waiting for DSP on\n");
      break;
    case writesf_pending_pausing:
      post("writesf~: opened, paused, waiting for DSP on.\n");
      break;
    }
}


/* The put method put the following arguments:
   <*dev> <nchans> <nvec> <v1> ... <vn>
 */

static void
writesf_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  writesf_t *this = (writesf_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);
  fts_atom_t *argv;
  int i;

  /* Set the local sampling rate */

  this->sampling_rate = fts_dsp_get_input_srate(dsp, 0);

  /* Open the file if needed, and change the state accordingly;
   * If the file is already open, we close it
   */

  switch (this->status)
    {
    case writesf_idle:
      /* Ignore if idle */
      break;
    case writesf_playing:
      /* Close file and reopen it if playing, active */
      writesf_file_close(this);
      writesf_file_open(this);
      fts_sig_dev_activate(this->device);
      break;
    case writesf_pausing:
      /* Close file and reopen it if paused, inactive*/
      writesf_file_close(this);
      writesf_file_open(this);
      break;
    case writesf_pending_playing:
      /* open the file if pending playing, active */
      writesf_file_open(this);
      fts_sig_dev_activate(this->device);
      this->status = writesf_playing;
      break;
    case writesf_pending_pausing:
      /* open the file if pending pause, inactive */
      writesf_file_open(this);
      this->status = writesf_pausing;
      break;
    }

  /* Generate the dsp code  */

  argv = (fts_atom_t *) fts_malloc(sizeof(fts_atom_t) * (3 + this->nchans));

  fts_set_ptr(argv + 0, &(this->device));
  fts_set_long(argv + 1, this->nchans);
  fts_set_long(argv + 2, fts_dsp_get_input_size(dsp, 0));

  for (i = 0; i < this->nchans; i++)
    fts_set_symbol(argv + 3 + i, fts_dsp_get_input_name(dsp, i));

  dsp_add_funcall(fts_dev_class_get_sig_put_fun_name(fts_dev_class_get_by_name(fts_new_symbol("writesf"))),
		  3 + this->nchans,
		  argv);	  

  fts_free(argv);
}


static void
writesf_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  writesf_t *this = (writesf_t *)o;
  int nchans = fts_get_int_arg(ac, at, 1, 1); /* ignore other args */

  if (nchans < 1)
    this->nchans = 1;
  else
    this->nchans = nchans;

  this->device = 0;
  this->status = writesf_idle;
  this->sampling_rate = 0.0f;

  dsp_list_insert(o); /* just put object in list */
}


static void
writesf_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  writesf_t *this = (writesf_t *)o;

  if (this->status != writesf_idle)
    writesf_file_close(this);

  dsp_list_remove(o);
}


static fts_status_t
writesf_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int nchans, i;
  fts_symbol_t a[4];

  nchans = fts_get_long_arg(ac, at, 1, 1);

  if (nchans < 1)
    nchans = 1;

  fts_class_init(cl, sizeof(writesf_t), nchans, 0, 0);

  a[0] = fts_s_symbol;
  a[1] = fts_s_int;
  a[2] = fts_s_int;
  a[3] = fts_s_int;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, writesf_init, 4, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, writesf_delete, 0, 0);

  a[0] = fts_s_ptr;
  fts_method_define(cl, fts_SystemInlet, fts_s_put, writesf_put, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, writesf_number, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, writesf_number, 1, a);

  fts_method_define_varargs(cl, 0, fts_s_bang, writesf_bang);
  fts_method_define_varargs(cl, 0, fts_new_symbol("start"), writesf_start);
  fts_method_define_varargs(cl, 0, fts_new_symbol("stop"),  writesf_stop);
  fts_method_define_varargs(cl, 0, fts_new_symbol("pause"), writesf_pause);

  a[0] = fts_s_symbol;
  a[1] = fts_s_int;
  a[2] = fts_s_symbol;
  fts_method_define_optargs(cl, 0, fts_new_symbol("open"),  writesf_open, 3, a, 0);

  fts_method_define(cl, 0, fts_new_symbol("print"), writesf_print, 0, 0);

  for (i = 0; i < nchans; i++)
    dsp_sig_inlet(cl, i);

  return fts_Success;
}


void disk_config(void)
{
  fts_metaclass_install(fts_new_symbol("readsf~"), readsf_instantiate, fts_first_arg_equiv);
  fts_metaclass_install(fts_new_symbol("writesf~"), writesf_instantiate, fts_first_arg_equiv);
}
