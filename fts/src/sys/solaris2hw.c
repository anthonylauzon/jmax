/*
 * jMax
 * Copyright (C) 1999 by IRCAM
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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

/*
   Platform dependent functions for the SOLARIS platoform.

   This file include all the platform dependent functions that are
   *not* related to devices; porting FTS imply writing the good
   devices for the platform and writing one of this file.

   Functions that must be defined here are defined in the file
   hwdefs.h.
*/

#include "sys.h"

static fts_welcome_t  solaris_welcome = {"SOLARIS version"};


void fts_platform_init(void)
{
  fts_add_welcome(&solaris_welcome);
}

void fts_set_no_real_time()
{
}

void fts_pause( void)
{
}

/* API to catch the exceptions */


void fts_set_fpe_handler(fts_fpe_handler fh)
{
}


void fts_reset_fpe_handler(void)
{
}

int fts_memory_is_locked()
{
  return 0;
}

void fts_unlock_memory()
{
}

unsigned int fts_check_fpe(void)
{
  return 0;
}
