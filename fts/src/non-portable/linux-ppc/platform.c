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
 * This file contains Linux (Power-PC) hardware dependent initialization function.
 * 
 */

#include <unistd.h>
#include <sched.h>

#include "sys/hw.h"

extern void linux_ppc_init_fpu( void);

/*
  Function: fts_platform_init
  Description:
   performs platform specific initializations
  Arguments: none
  Returns: none
*/
void fts_platform_init( void)
{
  struct sched_param sp;

  fts_real_time_on();

  /* Get rid of root privilege if we have them */
  if (setreuid(geteuid(), getuid()) == -1)
    {
      /* Should we post a message ? */
    }

  linux_ppc_init_fpu();
}

