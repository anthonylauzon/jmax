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

#ifndef _FTS_HW_H_
#define _FTS_HW_H_

/* generic include with the prototypes of HW dependent functions. */


/*
  Function: fts_dl_open
  Description:
   loads a dynamic library
  Arguments: 
   filename: the name of the library. This file will be searched using
   platform specific search algorithm.
   error: a string in which a description of the error will be stored
   in case of error.
  Returns: a "handle" to be used for further invocation of fts_dl_lookup
  0 if an error occured.
*/
extern void *fts_dl_open( const char *filename, char *error);

/*
  Function: fts_dl_lookup
  Description:
   lookup the specified symbol in symbol table, returning the address
   of this symbol
  Arguments: 
   handle: a handle returned by fts_dl_open
   symbol: the symbol name
   address: a pointer to a pointer: the address of the symbol
   will be stored here upon return, if the function does not
   return an error.
   error: a string in which a description of the error will be stored
   in case of error.
  Returns: 1 if symbol is found, 0 if not
*/
extern int fts_dl_lookup( void *handle, const char *symbol, void **address, char *error);

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
extern void fts_enable_fpe_traps( void);

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
extern void fts_disable_fpe_traps( void);

/* 
 * Bit masks returned with fts_check_fpe;
 */
#define FTS_INVALID_FPE  0x01
#define FTS_DIVIDE0_FPE  0x02
#define FTS_OVERFLOW_FPE 0x04
#define FTS_INEXACT_FPE  0x08
#define FTS_UNDERFLOW_FPE 0x10
#define FTS_DENORMALIZED_FPE 0x20

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
extern unsigned int fts_check_fpe( void);


/*
   Function: fts_unlock_memory
   Description:
    unlocks memory (enable swapping of FTS process)
   Arguments: none
   Returns: 1 if memory has been unlocked, 0 if not
*/
extern int fts_unlock_memory( void);

/*
  Function: fts_platform_init
  Description:
   performs platform specific initializations
  Arguments: none
  Returns: none
*/
extern void fts_platform_init( int argc, char **argv);

#endif
