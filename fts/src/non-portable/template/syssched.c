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
 * This file contains hardware dependent functions for system scheduler access.
 * 
 * FTS uses these functions to:
 * - set real-time mode
 * - pause a little during real-time execution (not to freeze the machine)
 */


/*
 * NOTE (fd):
 * the fts_pause mechanism is broken: on some platforms, it is impossible
 * to guaranty not to freeze the machine by pausing sometime. Example:
 * on Linux, nanosleep() is implemented using active wait for sleeping
 * time < 100 milliseconds. This means that a real-time process sync'ed
 * to audio cannot use nanosleep() to give some time back to the system
 * without causing big clicks...
 * The only solution is to implement a watchdog and to throw away
 * the fts_pause() mechanism.
 */

/*
 * This file contains templates for system scheduler management functions
 */

#include "sys/hw.h"

/*
   Function: fts_real_time_on
   Description:
    sets the execution mode to 'real-time' by calling the platform
    specific functions, for instance to change the scheduling family
    and the execution priority of the process.
   Arguments: none
   Returns: none
*/
void fts_real_time_on( void)
{
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
void fts_real_time_off( void)
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
}


