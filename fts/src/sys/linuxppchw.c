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
   Platform initialization for the LinuxPPC platform.
*/

#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <sched.h>
#include <signal.h>

#include "sys.h"


/*****************************************************************************/
/*                                                                           */
/*               Hardware configuration functions                            */
/*                                                                           */
/*****************************************************************************/

static fts_welcome_t linuxppc_welcome = {"Linux-PPC platform\n"};
static int running_real_time = 1;
struct timespec pause_time = { 0, 10000};

void fts_platform_init(void)
{
  struct sched_param sp;

#ifdef DEBUG
  fts_add_welcome(&linuxppc_welcome);
#endif

  sp.sched_priority = 80;

  if (running_real_time)
    {
      if ( sched_setscheduler( 0, SCHED_FIFO, &sp) < 0)
	{
	  /* Should we post a message ? */
	  running_real_time = 0;
	}
    }

  /* Get rid of root privilege if we have them */
  if (setreuid(geteuid(), getuid()) == -1)
    {
      /* Should we post a message ? */
    }
}

void fts_set_no_real_time()
{
  running_real_time = 0;
}

void fts_pause(void)
{
  if ( running_real_time)
    nanosleep( &pause_time, 0);
}

int fts_memory_is_locked()
{
  return 0;
}

void fts_unlock_memory()
{
}


/* --------------------------------------------------------------------------- */
/*                                                                             */
/* Floating-point exceptions handling                                          */
/*                                                                             */
/* --------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------- */
/*                                                                             */
/* FTS specific handling                                                       */
/*                                                                             */
/* --------------------------------------------------------------------------- */

static fts_fpe_handler fpe_handler = 0;

static void fpe_signal_handler( int sig)
{
/*    if (fpe_handler) */
/*      { */
/*        unsigned int s; */

/*        _FPU_GET_SW( s); */
/*        _FPU_CLR_SW; */

/*        if (s & _FPU_STATUS_IE) */
/*  	(* fpe_handler)( FTS_INVALID_FPE); */
/*        else if (s & _FPU_STATUS_ZE) */
/*  	(* fpe_handler)( FTS_DIVIDE0_FPE); */
/*        else if (s & _FPU_STATUS_OE) */
/*  	(* fpe_handler)( FTS_OVERFLOW_FPE); */
/*        else if (s & _FPU_STATUS_UE) */
/*  	(* fpe_handler)( FTS_UNDERFLOW_FPE); */
/*        else if (s & _FPU_STATUS_PE) */
/*  	(* fpe_handler)( FTS_INEXACT_FPE); */
/*        else */
/*  	(* fpe_handler)(0); */
/*      } */
}

/*
   Function: fts_set_fpe_handler
   Description:
     sets the fpe handler.
     store the argument in the static variable fpe_handler
     enables fpe traps
   Arguments:
     fh: a pointer to the fpe handler function. This function will be
     called by the trap handler
   Returns:
*/
void fts_set_fpe_handler( fts_fpe_handler fh)
{
/*    fpe_handler = fh; */

/*    signal( SIGFPE, fpe_signal_handler); */

/*    enable_fpu_trap(); */
}

/*
   Function: fts_reset_fpe_handler
   Description:
     resets the fpe handler.
     sets the static variable fpe_handler to 0
     disables fpe traps
   Arguments:
     none
   Returns:
*/
void fts_reset_fpe_handler(void)
{
/*    disable_fpu_trap(); */

/*    signal( SIGFPE, SIG_IGN); */

/*    fpe_handler = 0; */
}

/*
   Function: fts_check_fpe
   Description:
     returns a flag describing which floating-point exceptions
     occured since the last call 
   Arguments: none
   Returns: one of (0, FTS_INVALID_FPE, FTS_DIVIDE0_FPE, FTS_OVERFLOW_FPE, 
   FTS_INEXACT_FPE, FTS_UNDERFLOW_FPE)
*/

unsigned int fts_check_fpe(void)
{
  return 0;
}
