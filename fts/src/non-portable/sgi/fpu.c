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
 * This file contains SGI implementation of hardware dependent functions for FPU (Floating-Point Unit) 
 * settings.
 */
#include <signal.h>
#include <sys/fpu.h>
#include <sigfpe.h>

#include "sys/hw.h"
/* (fd) HACK !!! */
extern void fts_fpe_handler( int which);

/*
  To avoid loosing (a lot of) time with underflows:

  set the special "flush zero" but (FS, bit 24) in the
  Control Status Register of the FPU of R4k and beyond
  so that the result of any underflowing operation will
  be clamped to zero, and no exception of any kind will
  be generated on the CPU.  
  */


void sgi_init_fpu( void)
{
  union fpc_csr f;

  f.fc_word = get_fpc_csr();

  f.fc_struct.flush = 1;
  
  /* Disable all the exceptions by default */

  f.fc_struct.en_invalid = 0;
  f.fc_struct.en_divide0 = 0;
  f.fc_struct.en_overflow = 0;
  f.fc_struct.en_underflow = 0;
  f.fc_struct.en_inexact = 0;

  set_fpc_csr(f.fc_word);
}

/* HACK ! (fd)
 * Prototyped here for now...
 */
extern void fts_fpe_handler( int which);

/* Sgi man page tell to use this not ANSI prototype !! */
int sgi_fpe_signal_handler( sig, code, sc )
     int sig, code;
     struct sigcontext *sc;
{
  switch (__fpe_trap_type()) {
  case _OVERFL:
    fts_fpe_handler( FTS_OVERFLOW_FPE);
    break;
  case _UNDERFL:
    fts_fpe_handler( FTS_UNDERFLOW_FPE);
    break;
  case _DIVZERO:
    fts_fpe_handler( FTS_DIVIDE0_FPE);
    break;
  case _INVALID:
    fts_fpe_handler( FTS_INVALID_FPE);
    break;
  default:
    fts_fpe_handler( 0);
    break;
  }

  return 0;
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
  union fpc_csr f;

  f.fc_word = get_fpc_csr();

  f.fc_struct.flush = 1;
  
  /* Enable all the exceptions except inexact and underflow */

  f.fc_struct.en_invalid = 1;
  f.fc_struct.en_divide0 = 1;
  f.fc_struct.en_overflow = 1;
  f.fc_struct.en_underflow = 0;
  f.fc_struct.en_inexact = 0;

  set_fpc_csr(f.fc_word);

  handle_sigfpes (_ON, _EN_OVERFL | _EN_DIVZERO | _EN_INVALID, 0,
		  _USER_HANDLER,  sgi_fpe_signal_handler);
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
  union fpc_csr f;

  f.fc_word = get_fpc_csr();
  f.fc_struct.flush = 1;
  
  /* Disable all the exceptions */

  f.fc_struct.en_invalid = 0;
  f.fc_struct.en_divide0 = 0;
  f.fc_struct.en_overflow = 0;
  f.fc_struct.en_underflow = 0;
  f.fc_struct.en_inexact = 0;

  set_fpc_csr(f.fc_word);

  handle_sigfpes(_OFF, 0, 0, _USER_HANDLER, 0);
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
  union fpc_csr f;
  unsigned int ret = 0;

  f.fc_word = get_fpc_csr();

  if (f.fc_struct.se_invalid)
    ret |= FTS_INVALID_FPE;

  if (f.fc_struct.se_divide0)
    ret |= FTS_DIVIDE0_FPE;

  if (f.fc_struct.se_overflow)
    ret |= FTS_OVERFLOW_FPE;

  /* put the flags to zero anyway */

  f.fc_struct.se_invalid = 0;
  f.fc_struct.se_divide0 = 0;
  f.fc_struct.se_overflow = 0;

  set_fpc_csr(f.fc_word);

  return ret;
}

