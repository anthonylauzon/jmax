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
   Platform initialization the GNU platform.
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

static fts_welcome_t linuxpc_welcome = {"Linux-PC platform\n"};
static int running_real_time = 1;
struct timespec pause_time = { 0, 10000};

static void init_fpu( void);
static void enable_fpu_trap( void);
static void disable_fpu_trap( void);

void fts_platform_init(void)
{
  struct sched_param sp;

#ifdef DEBUG
  fts_add_welcome(&linuxpc_welcome);
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

  init_fpu();

  signal( SIGFPE, SIG_IGN);
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
/* Floating-point exceptions traping                                           */
/*                                                                             */
/* --------------------------------------------------------------------------- */

static fts_fpe_handler fpe_handler = 0;

/*
   Function: fts_set_fpe_handler
   Description:
     sets the fpe handler.
     store the argument in the static variable fpe_handler
     enables fpe trapping
   Arguments:
     fh: a pointer to the fpe handler function. This function will be
     called by the trap handler
   Returns:
*/
void fts_set_fpe_handler( fts_fpe_handler fh)
{
  fpe_handler = fh;
}

void fts_reset_fpe_handler(void)
{
}

/* --------------------------------------------------------------------------- */
/*                                                                             */
/* (Francois, dechelle@ircam.fr                                                */
/* As Linux does not provide (yet?) a way to access the status of the          */
/* floating-point unit, it is done by hand using __asm__. Great ;-(            */
/*                                                                             */
/* --------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------- */
/*                                                                             */
/* Pentium FPU Control Word                                                    */
/*                                                                             */
/* --------------------------------------------------------------------------- */
/* Exception masks */
#define _FPU_CONTROL_IM   0x001  /* Invalid Operation */
#define _FPU_CONTROL_DM   0x002  /* Denormalized Operand */
#define _FPU_CONTROL_ZM   0x004  /* Zero Divide */
#define _FPU_CONTROL_OM   0x008  /* Overflow */
#define _FPU_CONTROL_UM   0x010  /* Underflow */
#define _FPU_CONTROL_PM   0x020  /* Precision */

/* Precision control */
#define _FPU_CONTROL_SP   0x000  /* Single Precision */
#define _FPU_CONTROL_DP   0x200  /* Double Precision */
#define _FPU_CONTROL_EP   0x300  /* Extended Precision */

/* Rounding control */
#define _FPU_CONTROL_NR   0x000  /* Nearest */
#define _FPU_CONTROL_DW   0x400  /* Down */
#define _FPU_CONTROL_UP   0x800  /* Up */
#define _FPU_CONTROL_ZR   0x800  /* Zero */

/* Macros for accessing the FPU control word.  */
/* get the FPU control word */
#define _FPU_GET_CW(st) __asm__ ("fnstcw %0" : "=m" (*&cw))
#define _FPU_SET_CW(st) __asm__ ("fldcw %0" : : "m" (*&cw))

/* --------------------------------------------------------------------------- */
/*                                                                             */
/* Pentium FPU Status Word                                                     */
/*                                                                             */
/* --------------------------------------------------------------------------- */

/* Exception flags */
#define _FPU_STATUS_IE    0x001  /* Invalid Operation */
#define _FPU_STATUS_DE    0x002  /* Denormalized Operand */
#define _FPU_STATUS_ZE    0x004  /* Zero Divide */
#define _FPU_STATUS_OE    0x008  /* Overflow */
#define _FPU_STATUS_UE    0x010  /* Underflow */
#define _FPU_STATUS_PE    0x020  /* Precision */
/* Stack Fault */
#define _FPU_STATUS_SF    0x040  /* Precision */
/* Exception summary */
#define _FPU_STATUS_ES    0x080  /* Error Summary Status */

/* Macros for accessing the FPU status word.  */
/* get the FPU status */
#define _FPU_GET_SW(sw) __asm__ ("fnstsw %0" : "=m" (*&sw))
/* clear the FPU status */
#define _FPU_CLR_SW __asm__ ("fnclex" : : )


static void init_fpu( void)
{
  unsigned  int cw;

  cw = _FPU_CONTROL_IM /* masked */
    + _FPU_CONTROL_DM /* masked */
    + _FPU_CONTROL_ZM /* masked */
    + _FPU_CONTROL_OM /* masked */
    + _FPU_CONTROL_UM /* masked */
    + _FPU_CONTROL_PM /* masked */
    + _FPU_CONTROL_EP
    + _FPU_CONTROL_NR
    + 0x1000;

  _FPU_SET_CW( cw);
}

static void enable_fpu_trap( void)
{
}

static void disable_fpu_trap( void)
{
}


/*
   Function: fts_set_fpe_handler
   Description:
     returns a flag describing which floating-point exceptions
     occured since the last call 
   Arguments: none
   Returns: one of (0, FTS_INVALID_FPE, FTS_DIVIDE0_FPE, FTS_OVERFLOW_FPE, 
   FTS_INEXACT_FPE, FTS_UNDERFLOW_FPE)
*/

unsigned int fts_check_fpe(void)
{
  unsigned int s, ret;

  _FPU_GET_SW( s);
  _FPU_CLR_SW;

  ret = 0;

  if (s & _FPU_STATUS_IE)
    ret |= FTS_INVALID_FPE;

  if (s & _FPU_STATUS_ZE)
    ret |= FTS_DIVIDE0_FPE;

  if (s & _FPU_STATUS_OE)
    ret |= FTS_OVERFLOW_FPE;

  return ret;
}
