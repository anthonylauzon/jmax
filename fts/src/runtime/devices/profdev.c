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

/* This file include a simple performance measuring  device.

   The idea is very simple: if this device is installed
   as the audio stream (any number of channels), it send a time 
   marker event every  n samples, where n is set with two parameters
   (block_size, and profile_sample).

   The event contain the number of samples computed since the last
   event (0 if it is the first event), and if the architecture we are
   running in provide the gettimeoftheday functions, also the number
   of sample per second computed in the last profiling interval; 
   this value is always zero if there is no timeofthe day.

   This device should be installed only on CP 0, of course.
*/

/* Include files */

#include <sys/time.h>
#include <sys/resource.h>

#include <fts/sys.h>
#include <fts/lang.h>
#include <fts/runtime/files.h>
#include <fts/runtime/devices/devices.h>

/* forward declarations */

static void prof_dac_init(void);

void profdev_init(void)
{
  prof_dac_init();
}


/******************************************************************************/
/*                                                                            */
/*                              DAC Devices                                   */
/*                                                                            */
/******************************************************************************/


/* Forward declarations of DAC dev and dev class static functions */


static fts_status_t prof_dac_open(fts_dev_t *dev, int nargs, const fts_atom_t *args);
static fts_status_t prof_dac_close(fts_dev_t *dev);

static void         prof_dac_put(fts_word_t *argv);
static fts_status_t prof_dac_activate(fts_dev_t *dev);
static fts_status_t prof_dac_deactivate(fts_dev_t *dev);
static int          prof_dac_get_nchans(fts_dev_t *dev);


/*
   We don't lock channels, we just fake open them as needed.

   Time is kept Only for channel zero, that MUST be opened,
   and is kept in the prof_dac_data.
*/

static struct 
{
  /* Configuration */

  int profile_interval; 	/* the profiling interval,  in number of vectors */
  int nchans;			/* number of channels */

  /* status */

  int active;			/* if not zero, do profiling; actived by dsp_chain on */
  int one_done;			/* one interval done, accumulated time/samples valid */

  /* profiling accumulators */

  int total_samples;		/* accumulator for number of samples in the profing interval */
  int done_vectors;		/* number of vectors computer */

  struct timeval daytime;

} prof_dac_data;



/* Init functions */

static void prof_dac_init(void)
{
  fts_status_t ret;
  fts_dev_class_t *prof_dac_class;

  /* Profiling DAC class  */

  prof_dac_class = fts_dev_class_new(fts_sig_dev, fts_new_symbol("prof_dac"));

  /* device functions */

  fts_dev_class_set_open_fun(prof_dac_class, prof_dac_open);
  fts_dev_class_set_close_fun(prof_dac_class, prof_dac_close);
  fts_dev_class_sig_set_put_fun(prof_dac_class, prof_dac_put);

  fts_dev_class_sig_set_activate_fun(prof_dac_class, prof_dac_activate);
  fts_dev_class_sig_set_deactivate_fun(prof_dac_class, prof_dac_deactivate);
  fts_dev_class_sig_set_get_nchans_fun(prof_dac_class, prof_dac_get_nchans);
}

/* Prof DAC control/options functions */

/*
   The dev_ctrl operation for the prof_dac
   The arguments are parsed according to the generic parsing rules
   for FTS control device, look for comments in the file dev.c.

   The parameters can be passed only at open time, there is no io_ctrl
   operation for the sgi dac.


   PARAMATER    DESCRIPTION            TYPE     DEFAULT  VALUES

   profile_interval    the size of the profiling interval, in vector size units
   channels            the number of channels the device have
*/


/* Prof DAC dev class functions */

static fts_status_t
prof_dac_open(fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  fts_atom_t local_args[10];
  int local_nargs;			/* X/Motif style arg counter */

  /* initialize the global structure */

  /* Complete the initialization of the structure */

  prof_dac_data.active   = 0;
  prof_dac_data.one_done = 0;
  prof_dac_data.total_samples = 0;
  prof_dac_data.done_vectors  = 0;

  /* Parameter parsing  */
  
  prof_dac_data.profile_interval = fts_get_int_by_name(nargs, args, fts_new_symbol("profile_interval"), 10000);
  prof_dac_data.nchans = fts_get_int_by_name(nargs, args, fts_new_symbol("channels"), 2);

  return fts_Success;
}

static fts_status_t
prof_dac_close(fts_dev_t *dev)
{
  return fts_Success;
}



static void
prof_dac_put(fts_word_t *argv)
{
  long n = fts_word_get_long(argv + 2);

  if (prof_dac_data.active)
    {
      if (prof_dac_data.one_done)
	{
	  /* Set the current time of the day */

	  prof_dac_data.total_samples += n;
	  (prof_dac_data.done_vectors)++;

	  if (prof_dac_data.done_vectors == prof_dac_data.profile_interval)
	    {
	      struct timeval last_time;
	      int samples_per_seconds = 0;
	      long dsec, dusec;

	      prof_dac_data.done_vectors = 0;

	      /* Get the current time of the day */

	      gettimeofday(&last_time, 0); /* 0 is for compatibility with BSD */

	      /* Compute the difference */

	      dsec = last_time.tv_sec - prof_dac_data.daytime.tv_sec;

	      if (last_time.tv_usec > prof_dac_data.daytime.tv_usec)
		dusec = last_time.tv_usec - prof_dac_data.daytime.tv_usec;
	      else
		{
		  dsec--;
		  dusec = 1000000 + last_time.tv_usec - prof_dac_data.daytime.tv_usec;
		}

	      /* Store the current time */

	      prof_dac_data.daytime = last_time;

	      /* Compute the samples per seconds rate; use only hundres of second to make computation*/

	      samples_per_seconds = (int) ((long) (100L * (long) prof_dac_data.total_samples) /
					  (long) (100L * dsec + (dusec / 10000)));
	      /* Post the results event */

	      post("Computed %d samples at %d samples per seconds\n", 
		   prof_dac_data.total_samples,
		   samples_per_seconds);

	      /* reset the total sample counter */
	      prof_dac_data.total_samples = 0;
	    }
	}
      else
	{
	  prof_dac_data.one_done = 1;

	  /* Set the current time of the day */

	  gettimeofday(&prof_dac_data.daytime, 0); /* 0 is for compatibility with BSD */

	  /* reset the total sample counter */
	  prof_dac_data.total_samples = 0;
	}
    }
}



static fts_status_t 
prof_dac_activate(fts_dev_t *dev)
{
  post("prof_dac Activated\n");	/* @@@ */
  prof_dac_data.active = 1;
  
  return fts_Success;
}


static fts_status_t 
prof_dac_deactivate(fts_dev_t *dev)
{
  post("prof_dac Deactivated\n");	/* @@@ */

  prof_dac_data.active = 0;
  prof_dac_data.one_done = 0;

  return fts_Success;
}

static int
prof_dac_get_nchans(fts_dev_t *dev)
{
  return prof_dac_data.nchans;
}





