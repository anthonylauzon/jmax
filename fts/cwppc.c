/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

/*
 * This file contains Mac OS X platform dependent functions:
 *  - dynamic loader
 *  - FPU settings
 *  - real-time: scheduling mode and priority, memory locking
 * 
 */

#include <fts/fts.h>
#include <ftsconfig.h>

#include <stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ftsprivate/fpe.h>
#include <ftsprivate/platform.h>
#include <ftsprivate/package.h>
#include <fts/thread.h>

/***********************************************************************
 *
 * Root directory
 *
 */

fts_symbol_t fts_get_default_root_directory( void)
{
  return fts_new_symbol( DEFAULT_ROOT);
}

/***********************************************************************
 *
 * Project and configuration files
 *
 */

#define PREF_DIR ".jmax"

static fts_symbol_t get_user_directory(void)
{
  char dir[MAXPATHLEN];

  strcpy( dir, getenv("HOME"));
  strcat( dir, "/");
  strcat( dir, PREF_DIR);
  
  /* create the directory if necessary */
  if (!fts_file_exists(dir) && mkdir( dir, 0755) < 0)
    fts_log( "[user] error: cannot create directory %s\n", dir);

  if (fts_is_file( dir))
    fts_log( "[user] error: %s is a file, but must be a directory\n", dir);

  return fts_new_symbol(dir);
}

fts_symbol_t
fts_get_user_project( void)
{
  char path[MAXPATHLEN];

  fts_make_absolute_path( get_user_directory(), fts_s_default_project, path, MAXPATHLEN);

  return fts_new_symbol( path);
}

fts_symbol_t 
fts_get_system_project( void)
{
  char path[MAXPATHLEN];

  fts_make_absolute_path( fts_get_root_directory(), fts_s_default_project, path, MAXPATHLEN);

  return fts_new_symbol(path);
}

fts_symbol_t 
fts_get_user_configuration( void)
{
  char path[MAXPATHLEN];

  fts_make_absolute_path( get_user_directory(), fts_s_default_config, path, MAXPATHLEN);

  return fts_new_symbol(path);
}

fts_symbol_t 
fts_get_system_configuration(void)
{
  char path[MAXPATHLEN];

  fts_make_absolute_path( fts_get_root_directory(), fts_s_default_config, path, MAXPATHLEN);

  return fts_new_symbol(path);
}

/* *************************************************************************** */
/*                                                                             */
/* Dynamic loader                                                              */
/*                                                                             */
/* *************************************************************************** */


fts_status_t fts_load_library( const char *filename, const char *symbol)
{
  return fts_ok;
}

/* *************************************************************************** */
/*                                                                             */
/* Floating-point unit                                                         */
/*                                                                             */
/* *************************************************************************** */

void fts_enable_fpe_traps( void)
{
}

void fts_disable_fpe_traps( void)
{
}

unsigned int fts_check_fpe( void)
{
  return 0;
}

/* *************************************************************************** */
/*                                                                             */
/* Memory locking                                                              */
/*                                                                             */
/* *************************************************************************** */

int fts_unlock_memory( void)
{
  return 0;
}


/* *************************************************************************** */
/*                                                                             */
/* Platform specific initialization                                            */
/*                                                                             */
/* *************************************************************************** */

void fts_platform_init( void)
{
}


/* *************************************************************************** */
/*                                                                             */
/* System time                                                                   */
/*                                                                             */
/* *************************************************************************** */

double fts_systime()
{
  return 0.0;
}

/* ************************************************** */
/*                                                    */
/* Thread manager platform dependent function         */
/*                                                    */
/* ************************************************** */
int thread_manager_start(thread_manager_t* self)
{
    return -1;
}

void* thread_manager_run_thread(void* arg)
{
    return 0;
}


/** 
 * Main function of the thread manager
 * 
 * @param arg 
 * 
 * @return 
 */
void* thread_manager_main(void* arg)
{
    return NULL;
}

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
