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
 * This file contains hardware dependent functions for FPU (Floating-Point Unit) 
 * settings.
 * 
 * FTS uses the FPU for:
 * - setting the "flush to zero on underflow" mode
 * - disabling all FPE (Floating-Point Exceptions) traps
 * - enable when needed the following FPE traps: divide by zero, overflow, invalid operand
 */

/*
 * This file contains templates for FPU functions
 */

#include "sys/hw.h"

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
  return 0;
}

