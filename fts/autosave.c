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
 */


#include <signal.h>
#include <stdlib.h>

#include <fts/fts.h>
#include <ftsprivate/bmaxhdr.h>
#include <ftsprivate/saver.h>
#include <ftsprivate/patcher.h>

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


static void fts_autosave(const char *postfix)
{
  fts_patcher_t *root = fts_get_root_patcher();  
  fts_object_t *p;

  /* First, look if the objects in the patchers are to be found */
  for (p = root->objects; p ; p = p->next_in_patcher)
    {
      if (fts_object_is_patcher(p))
	{
	  fts_patcher_t *patcher = (fts_patcher_t *)p;
	  fts_symbol_t file_name = fts_patcher_get_file_name(patcher);
	  
	  if(file_name)
	    {
	      char buf[FILENAME_MAX];
	      
	      sprintf(buf, "%s%s", file_name, postfix);
	      
	      fts_save_simple_as_bmax(buf, p);
	    }
	}
    }
}


/* Signal handler */

static void autosave_signal_handler(int sig)
{
  fts_autosave("%%");
  exit(99);
}


/***********************************************************************
 *
 * Initialization
 *
 */

void fts_kernel_autosave_init( void)
{
/*#ifndef DEBUG */
  /* Standard quit/int signals */

/*#ifndef WIN32
  signal(SIGHUP,  autosave_signal_handler);
  #endif
  signal(SIGINT,  autosave_signal_handler);
  #ifndef WIN32
  signal(SIGQUIT, autosave_signal_handler);
  #endif
  signal(SIGABRT, autosave_signal_handler);*/

  /* Corruption signals */

/*#ifndef WIN32
    signal(SIGBUS, autosave_signal_handler);
    #endif
    signal(SIGSEGV, autosave_signal_handler);
    signal(SIGTERM, autosave_signal_handler);
    #endif*/
}
