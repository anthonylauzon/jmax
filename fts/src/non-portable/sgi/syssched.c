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
 * Author: Francois Dechelle.
 *
 */

/*
 * This file contains SGI implementation of hardware dependent functions for system scheduler access.
 * 
 */

#include <sched.h>
/* For sysmp(2) function */
#include <sys/types.h>
#include <sys/sysmp.h>
#include <sys/sysinfo.h>
#include <sys/pda.h>
#include <errno.h>

/* (fd) HACK !!! */
extern void post( const char *format, ...);

static int running_real_time = 0;

/*
   Function: fts_real_time_on
   Description:
    sets the execution mode to 'real-time' by calling the platform
    specific functions, for instance to change the scheduling family
    and the execution priority of the process.
   Arguments: none
   Returns: none
*/

static void run_real_time_scheduling( void)
{
  struct sched_param param;
  
  /* raise priority to a high value */
  param.sched_priority  = 199;

  if (sched_setscheduler( 0, SCHED_FIFO, &param) < 0)
    {
      post( "Warning: cannot switch to real-time scheduling mode. \n");
      post( "         Real-time performance will be degraded resulting in audio clicks.\n");
      post( "         This warning result from installation problems (jMax server not owned by root).\n");
      return;
    }
  running_real_time = 1;

  /* Iff we are running real time, lock to physical memory, if we can */
  if ( !fts_lock_memory() )
    {
      post("Warning: cannot lock memory (error \"%s\")\n", strerror (errno));
    }
}

/*
 * Returns: -1 if an error occured, 
 *           0 if no isolated processor was found,
 *           1 if isolated processor was found and process succeeded in requesting to run on it
 */
static int run_on_isolated_cpu( void)
{
  struct pda_stat *p;
  int n_processors, n;

  n_processors = sysmp( MP_NPROCS);
  if ( n_processors < 0)
    return -1;

  p = (struct pda_stat *)malloc( n_processors * sizeof( struct pda_stat));
  if ( sysmp( MP_STAT, p) < 0)
    return -1;

  for ( n = 0; n < n_processors; n++)
    {
      if (p[n].p_flags & PDAF_ISOLATED)
	{
	  if (sysmp( MP_MUSTRUN, n) < 0)
	    return -1;

	  post( "Running on isolated processor %d\n", n);

	  return 1;
	}
    }

  return 0;
}

void fts_real_time_on()
{
  run_real_time_scheduling();
  run_on_isolated_cpu();
}

/*
   Function: fts_real_time_off
   Description:
    sets the execution mode to 'normal' by calling the platform
    specific functions, for instance to change the execution
    priority of the process.
   Arguments: none
   Returns: none
*/
void fts_real_time_off()
{
}

/*
  Function: fts_pause
  Description:
   pause a little during real-time execution, in order not to freeze the machine.
   (fd) This mechanism doesn't work (see NOTE below).
  Arguments: none
  Returns: none
*/
void fts_pause( void)
{
  if ( running_real_time)
    {
      struct timespec pause_time;

      pause_time.tv_sec = 0;
      pause_time.tv_nsec = 100000;

      nanosleep( &pause_time, 0);
    }
}


