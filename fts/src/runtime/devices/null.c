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
/* This file include three null devices, one for chars and one
   for DSP; the null device dac for DSP implement a dummy sincronization
   by using nanosleep, and the null adc device always return zeros.
*/

/* Include files */


#include "sys.h"
#include "lang.h"
#include "runtime/files.h"
#include "runtime/devices/devices.h"
#include <sys/time.h>



/* forward declarations */

static void signull_dac_init(void);
static void null_dev_init(void);


void null_init(void)
{
  signull_dac_init();
  null_dev_init();
}

/******************************************************************************/
/*                                                                            */
/*                              CHAR Device                                   */
/*                                                                            */
/******************************************************************************/

static fts_status_t null_dev_open(fts_dev_t *dev, int nargs, const fts_atom_t *args);
static fts_status_t null_dev_close(fts_dev_t *dev);

static fts_status_t null_dev_get(fts_dev_t *dev, unsigned char *cp);
static fts_status_t null_dev_put(fts_dev_t *dev, unsigned char c);
static fts_status_t null_dev_seek(fts_dev_t *dev, long offset, int whence); /* NOP */

static void null_dev_init(void)
{
  fts_dev_class_t *null_dev_class;

  null_dev_class = fts_dev_class_new(fts_char_dev, fts_new_symbol("null"));

  /* adding device functions: the device support only basic 
   character i/o; no callback functions, no sync functions */

  fts_dev_class_set_open_fun(null_dev_class, null_dev_open);
  fts_dev_class_set_close_fun(null_dev_class, null_dev_close);
  fts_dev_class_char_set_get_fun(null_dev_class, null_dev_get);
  fts_dev_class_char_set_put_fun(null_dev_class, null_dev_put);
  fts_dev_class_char_set_seek_fun(null_dev_class, null_dev_seek);
}


static fts_status_t null_dev_open(fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  return fts_Success;
}


static fts_status_t
null_dev_close(fts_dev_t *dev)
{
  return fts_Success;
}

static fts_status_t
null_dev_get(fts_dev_t *dev, unsigned char *cp)
{
  return &fts_data_not_ready;
}

static fts_status_t
null_dev_put(fts_dev_t *dev, unsigned char c)
{
  return fts_Success;
}

static fts_status_t
null_dev_seek(fts_dev_t *dev, long offset, int whence)
{
  return fts_Success;
}


/******************************************************************************/
/*                                                                            */
/*                              DAC Devices                                   */
/*                                                                            */
/******************************************************************************/


/* Forward declarations of DAC dev and dev class static functions */


static fts_status_t signull_dac_open(fts_dev_t *dev, int nargs, const fts_atom_t *args);
static fts_status_t signull_dac_close(fts_dev_t *dev);

static void         signull_dac_put(fts_word_t *argv);
static fts_status_t signull_dac_activate(fts_dev_t *dev);
static fts_status_t signull_dac_deactivate(fts_dev_t *dev);
static int          signull_dac_get_nchans(fts_dev_t *dev);


static struct 
{
  fts_timer_t timer;

  /* Configuration */

  int nchans;			/* number of channels */

} signull_dac_data;



/* Init functions */

static void signull_dac_init(void)
{
  fts_status_t ret;
  fts_dev_class_t *signull_dac_class;

  /* Profiling DAC class  */

  signull_dac_class = fts_dev_class_new(fts_sig_dev, fts_new_symbol("null~"));

  /* device functions */

  fts_dev_class_set_open_fun(signull_dac_class, signull_dac_open);
  fts_dev_class_set_close_fun(signull_dac_class, signull_dac_close);
  fts_dev_class_sig_set_put_fun(signull_dac_class, signull_dac_put);

  fts_dev_class_sig_set_activate_fun(signull_dac_class, signull_dac_activate);
  fts_dev_class_sig_set_deactivate_fun(signull_dac_class, signull_dac_deactivate);
  fts_dev_class_sig_set_get_nchans_fun(signull_dac_class, signull_dac_get_nchans);
}


static fts_status_t
signull_dac_open(fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  fts_atom_t local_args[10];
  int local_nargs;			/* X/Motif style arg counter */

  /* initialize the global structure */

  /* Complete the initialization of the structure */

  fts_timer_init(&signull_dac_data.timer, 0);
  fts_timer_start(&signull_dac_data.timer);

  /* Parameter parsing  */
  
  signull_dac_data.nchans = fts_get_int_by_name(nargs, args, fts_new_symbol("channels"), 2);

  return fts_Success;
}

static fts_status_t
signull_dac_close(fts_dev_t *dev)
{
  return fts_Success;
}

static void
signull_dac_put(fts_word_t *argv)
{
  if (fts_timer_elapsed_time(&signull_dac_data.timer) >= (double) 50.0)
    {
      struct timespec pause_time;

      pause_time.tv_sec = 0;
      pause_time.tv_nsec = 50000000L;

      nanosleep( &pause_time, 0);
      fts_timer_zero(&signull_dac_data.timer);
    }
}



static fts_status_t 
signull_dac_activate(fts_dev_t *dev)
{
  return fts_Success;
}


static fts_status_t 
signull_dac_deactivate(fts_dev_t *dev)
{
  return fts_Success;
}

static int
signull_dac_get_nchans(fts_dev_t *dev)
{
  return signull_dac_data.nchans;
}





