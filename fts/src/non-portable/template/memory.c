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
 * This file contains hardware dependent functions for memory management.
 * 
 * FTS uses these functions to:
 * - check if memory is locked (when allocating memory)
 * - lock or unlock the memory 
 */

/*
 * NOTE (fd):
 * May be the memory management should be more platform specific: i.e.
 * part of sys/mem.c should go here ?
 */

/*
 * This file contains templates for memory management functions
 */

#include "sys/hw.h"


/*
  Function: fts_memory_is_locked
  Description:
   return 1 if memory is currently locked, 0 if not
  Arguments: none
  Returns: 1 or 0
*/
int fts_memory_is_locked( void)
{
  return 0;
}


/*
   Function: fts_lock_memory
   Description:
    locks FTS process in physical memory (disable swapping of FTS process)
   Arguments: none
   Returns: 1 if lock succeeded, 0 if failed
*/
int fts_lock_memory( void)
{
  return 0;
}

/*
   Function: fts_unlock_memory
   Description:
    unlocks memory (enable swapping of FTS process)
   Arguments: none
   Returns: none
*/
void fts_unlock_memory( void)
{
}

