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

void fts_pause( void)
{
}

unsigned int fts_check_fpe(void)
{
  return 0;
}
