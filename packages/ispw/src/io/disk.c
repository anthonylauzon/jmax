/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
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

/* The DSP function name */

static fts_symbol_t readsf_function = 0;

/* utility function to find the real path */


static char buf[1024];

static const char *get_readsf_path(fts_symbol_t filename)
{
  fts_file_get_read_path(fts_symbol_name(filename), buf);
  
  return buf;
}

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
 * open filename
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
  fts_symbol_t filename;
  fts_dev_t *device;
  enum {idle, playing, pause} status;
  int nchans;
} readsf_t;

/* Service functions */

static void readsf_file_open(readsf_t *this)
{
  if (this->status == idle)
    {
      fts_status_t ret;
      fts_atom_t a[3];

      /* Build the open arg list: current version, use the default device fifosize and fileblock size */

      fts_set_symbol(&a[0], fts_new_symbol( get_readsf_path( this->filename)));
      fts_set_symbol(&a[1], fts_new_symbol("channels"));
      fts_set_int(&a[2],    this->nchans);

      /* Open the device; note that the device is opened not active !! */

      ret = fts_dev_open(&(this->device), fts_new_symbol("readsf"), 3, a);

      if (ret != fts_Success)
	post("readsf~: cannot open file '%s' for reading\n", fts_symbol_name(this->filename));
      else
	this->status = pause;
    }
}


static void
readsf_file_close(readsf_t *this)
{
  if (this->status != idle)
    {
      fts_dev_close(this->device);
      this->device = 0;
      this->status = idle;
    }
}


static void
readsf_file_start(readsf_t *this)
{
  if (this->status == pause)
    {
      fts_sig_dev_activate(this->device);
      this->status = playing;
    }
}


static void
readsf_file_pause(readsf_t *this)
{
  if (this->status == playing)
    {
      fts_sig_dev_deactivate(this->device);
      this->status = pause;
    }
}

/* Methods */

static void readsf_start(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t *this = (readsf_t *)o;

  readsf_file_start(this);
}


static void
readsf_pause(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t *this = (readsf_t *)o;

  readsf_file_pause(this);
}


static void
readsf_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t *this = (readsf_t *)o;

  readsf_file_close(this);
}


static void
readsf_open(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  readsf_t *this = (readsf_t *)o;
  fts_symbol_t filename = fts_get_symbol_arg(ac, at, 0, 0);

  if (this->status != idle)
    readsf_file_close(this);

  if (filename)
    {
      this->filename = filename;
      readsf_file_open(this);
    }
}

static void
readsf_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  readsf_t *this = (readsf_t *)o;
  int cmd = fts_get_int_arg(ac, at, 0, 0);

  if (cmd == 0)
    readsf_stop(o, winlet, s, ac, at);
  else if (cmd == 1)
    readsf_start(o, winlet, s, ac, at);
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

  argv = (fts_atom_t *) fts_malloc(sizeof(fts_atom_t) * (3 + this->nchans));

  fts_set_ptr(argv + 0, &(this->device));
  fts_set_long(argv + 1, this->nchans);
  fts_set_long(argv + 2, fts_dsp_get_output_size(dsp, 0));

  for (i = 0; i < this->nchans; i++)
    fts_set_symbol(argv + 3 + i, fts_dsp_get_output_name(dsp, i));

  dsp_add_funcall(fts_dev_class_get_sig_get_fun_name(fts_dev_class_get_by_name(fts_new_symbol("readsf"))),
		  3 + this->nchans,
		  argv);	  

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

  a[0] = fts_s_symbol;
  fts_method_define(cl, 0, fts_new_symbol("start"), readsf_start, 1, a);
  fts_method_define(cl, 0, fts_new_symbol("stop"),  readsf_stop, 1, a);
  fts_method_define(cl, 0, fts_new_symbol("pause"), readsf_pause, 1, a);

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

void disk_config(void)
{
  fts_metaclass_create(fts_new_symbol("readsf~"), readsf_instantiate, fts_first_arg_equiv);
}








