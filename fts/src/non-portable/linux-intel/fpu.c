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
 * This file contains Linux (Intel and compatible processors) implementation of hardware 
 * dependent functions for FPU (Floating-Point Unit) settings.
 * 
 */

/*
 * #define this if you want denormalized f.p. traps to be reported 
 */
#undef ENABLE_DENORMALIZED_TRAPS

#include <signal.h>

#include "sys/hw.h"

/* (fd) HACK !!! */
extern void fts_fpe_handler( int which);

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


static void linux_fpe_signal_handler( int sig)
{
  unsigned int s;

  _FPU_GET_SW( s);
  _FPU_CLR_SW;

  if (s & _FPU_STATUS_ZE)
    fts_fpe_handler( FTS_DIVIDE0_FPE);
  else if (s & _FPU_STATUS_IE)
    fts_fpe_handler( FTS_INVALID_FPE);
  else if (s & _FPU_STATUS_OE)
    fts_fpe_handler( FTS_OVERFLOW_FPE);
  else if (s & _FPU_STATUS_UE)
    fts_fpe_handler( FTS_UNDERFLOW_FPE);
  else if (s & _FPU_STATUS_PE)
    fts_fpe_handler( FTS_INEXACT_FPE);
  else if (s & _FPU_STATUS_DE)
    fts_fpe_handler( FTS_DENORMALIZED_FPE);
  else
    fts_fpe_handler(0);
}

/*
   Function: fts_enable_fpe_traps
   Description:
     enables fpe traps.
     After calling this function, division by zero, overflow and invalid operand
     should cause a trap and a call to the FTS standard trap handler (fts_fpe_handler)
     that will signal a runtime error. fts_fpe_handler is defined in mess/fpe.c

     This function usually set various bits in the floating-point unit control
     register.
   Arguments: none
   Returns: none
*/
void fts_enable_fpe_traps( void)
{
  unsigned  int cw;

  signal( SIGFPE, linux_fpe_signal_handler);

#ifdef ENABLE_DENORMALIZED_TRAPS
  cw = 0x1000 + _FPU_CONTROL_EP + _FPU_CONTROL_NR /* extended precision + round to nearest */
    + _FPU_CONTROL_UM /* disable underflow */
    + _FPU_CONTROL_PM; /* disable precision */
#else
  cw = 0x1000 + _FPU_CONTROL_EP + _FPU_CONTROL_NR /* extended precision + round to nearest */
    + _FPU_CONTROL_DM /* disable denormalized */
    + _FPU_CONTROL_UM /* disable underflow */
    + _FPU_CONTROL_PM; /* disable precision */
#endif

  _FPU_SET_CW( cw);
}


/*
   Function: fts_disable_fpe_traps
   Description:
     disables fpe traps.
     After calling this function, no more traps will be triggered by floating-point
     exceptions.
     This function usually set various bits in the floating-point unit control
     register.
   Arguments: none
   Returns: none
*/
void fts_disable_fpe_traps( void)
{
  unsigned  int cw;

  signal( SIGFPE, SIG_IGN);

  cw = 0x1000 + _FPU_CONTROL_EP + _FPU_CONTROL_NR
    + _FPU_CONTROL_IM /* disable invalid operand */
    + _FPU_CONTROL_DM /* disable denormalized */
    + _FPU_CONTROL_ZM /* disable divide by zero */
    + _FPU_CONTROL_OM /* disable overflow */
    + _FPU_CONTROL_UM /* disable underflow */
    + _FPU_CONTROL_PM; /* disable precision */

  _FPU_SET_CW( cw);
}

/*
   Function: fts_check_fpe
   Description:
     returns a flag describing which floating-point exceptions
     occured since the last call.
     This function usually polls the status bits of the floating-point
     unit status register.
   Arguments: none
   Returns: one of (0, FTS_INVALID_FPE, FTS_DIVIDE0_FPE, FTS_OVERFLOW_FPE, 
   FTS_INEXACT_FPE, FTS_UNDERFLOW_FPE)
*/
unsigned int fts_check_fpe( void)
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

void linux_intel_init_fpu( void)
{
  fts_disable_fpe_traps();
}


