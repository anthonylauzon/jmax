/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */

#include <signal.h>

#include "sys.h"
#include "lang/mess.h"

/* Implement autosave functions; autosave is implemented
   only for catastrophic situations, but can be easily implemented 
   in automatic style, of course; note that in the current implementation,
   it save only top level patchers that have the fts_s_filename property
   explicitly set; this property is set by the patcher loader, usually.

   fts_autosave save all the files adding "postfix" at the end;
   fts_autosave try to don't call malloc/free, including symbol
   allocations; it is important in order to try to work with a memory
   corruption situation; also, please do never use daemon for the fts_s_filename
   property, for the same reason.

   Note that the current implementation stop any core dump from being
   done; this is not very good in debugging situation; for this reason
   autosave is disabled when compiling in debug mode; a more 
   sophisticated signal handler should be written.
   */


void fts_autosave(const char *postfix)
{
  fts_patcher_t *root = fts_get_root_patcher();  
  fts_object_t *p;

  /* First, look if the objects in the patchers are to be found */

  for (p = root->objects; p ; p = p->next_in_patcher)
    {
      fts_atom_t a;

      fts_object_get_prop(p, fts_s_filename, &a);

      if (fts_is_symbol(&a))
	{
	  char buf[FILENAME_MAX];

	  sprintf(buf, "%s%s", fts_symbol_name(fts_get_symbol(&a)), postfix);

	  fts_save_simple_as_bmax(buf, p);
	}
    }
}


/* Signal handler */

static void autosave_signal_handler(int sig)
{
  fts_autosave("%%");
  exit(99);
}

void fts_autosave_init( void)
{
#ifndef DEBUG 
  /* Standard quit/int signals */

  signal(SIGHUP,  autosave_signal_handler);
  signal(SIGINT,  autosave_signal_handler);
  signal(SIGQUIT, autosave_signal_handler);
  signal(SIGABRT, autosave_signal_handler);

  /* Corruption signals */

  signal(SIGBUS, autosave_signal_handler);
  signal(SIGSEGV, autosave_signal_handler);
  signal(SIGTERM, autosave_signal_handler);
  signal(SIGPWR, autosave_signal_handler);
#endif
}





