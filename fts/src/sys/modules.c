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

/*
   Support for FTS modules.

   A module is declared by a data structure having a number of fields:

   1- An init function, to be called before the computation start.
   2- A shutdown function, to be called before shutting down fts.

   The init function have the responsability to install the scheduled
   function of the module, and possibly welcomes and others; however,
   these functions are not stored in the module data structure.

   Support dynamic loading of C modules.

   FTS modules are initted in the main functions startup.c;
   user, dynamically loaded modules are initted as loaded.

   Init functions are called in the order the modules
   are installed.

   The shutdown functions are called in the *Inverse* order, so that
   if there are installation/shutdown dependencies, they may be naturally
   respected.
*/

#include <fts/sys.h>

#ifdef LINUX
#include <unistd.h>
#endif

#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>

extern int fts_file_get_read_path(const char *path, char *full_path);

/******************************************************************************/
/*                                                                            */
/*        Configuration Support                                               */
/*                                                                            */
/******************************************************************************/

fts_module_t *fts_installed_modules = 0;
static fts_module_t *last_installed_module = 0;

/* Install a module at the end of the list of user modules */

void
fts_install_module(fts_module_t *s)
{
  if (last_installed_module)
    last_installed_module->next = s;
  else
    fts_installed_modules = s;

  last_installed_module = s;
  s->next = (fts_module_t *) 0;
}


void
fts_modules_init(void)
{
  fts_module_t *s;

  for (s = fts_installed_modules; s; s = s->next)
    {
      if (s->init_f)
	(* s->init_f)();
    }
}


/* use a recursive function to call the shutdown functions 
   in the inverse order of the  init functions 
*/

static void
fts_recursive_module_shutdown(fts_module_t *s)
{
  if (s->next)
    fts_recursive_module_shutdown(s->next);

  if (s->shutdown_f)
    (* s->shutdown_f)();
}

void
fts_modules_shutdown(void)
{
  fts_recursive_module_shutdown(fts_installed_modules);
}


static fts_status_description_t  module_loaded = { "module already loaded"};
static fts_status_description_t  library_not_found = { "library not found"};
static fts_status_description_t  error_loading = { "error loading module"};
static fts_status_description_t  module_not_found = { "module not found"};

/* for architecuture with dynamic loading, load the module;
   the module variable name *must* be called <name>_module;
   the filename default to lib<name>.so if not specified;
   The file is looked for using fts_file_get_read_path.

   Note that this function overwrite the module name
   to be sure that correspond to the name passed;
   i.e. force it to adhere to conventions.
 */


fts_status_t fts_module_load(const char *name, const char *filename)
{
  char namebuf[1024], full_path[1024], errorbuf[1024];
  const char *libname;
  char module_name[512];
  fts_module_t *module, *s;
  void *handle;

  /* test if the module has been already loaded  */

  for (s = fts_installed_modules; s; s = s->next)
    {
      if (s->name && (! strcmp(s->name, name)))
	{
	  fprintf(stderr, "Module %s already loaded\n", name);
	  return &module_loaded;
	}
    }

  if (! filename)
    {
      sprintf( namebuf, "lib%s.so", name);
      libname = namebuf;
    }
  else
    libname = filename;
      
  if ( !fts_file_get_read_path( libname, full_path))
    {
      return &library_not_found;
    }

  handle = fts_dl_open( full_path, errorbuf);

  if (! handle)
    {
      fprintf(stderr, "Error: %s\n", errorbuf);
      return &error_loading;
    }

  sprintf( module_name, "%s_module", name);

  if ( !fts_dl_lookup( handle, module_name, (void **)&module, errorbuf))
    {
      fprintf(stderr, "Looking for module: %s\n", errorbuf);
      return &module_not_found;
    }

  /* add */

  module->name = name;		/* overwrite user declared name */
  fts_install_module(module);

  (* module->init_f)();

  return fts_Success;
}




