/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
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
