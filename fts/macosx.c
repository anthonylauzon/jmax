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

/*
 * This file contains Mac OS X platform dependent functions:
 *  - dynamic loader
 *  - FPU settings
 *  - real-time: scheduling mode and priority, memory locking
 * 
 */

#include "ftsconfig.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mach-o/dyld.h> 

#include <fts/fts.h>
#include <ftsprivate/fpe.h>
#include <ftsprivate/platform.h>


/***********************************************************************
 *
 * Root directory
 *
 */

fts_symbol_t fts_get_default_root_directory( void)
{
  return fts_new_symbol( DEFAULT_ROOT);
}

fts_symbol_t
fts_get_user_config( void)
{
  char* home;
  char path[MAXPATHLEN];

  home = getenv("HOME");
  fts_make_absolute_path(home, ".jmaxcf", path, MAXPATHLEN);
  if (fts_file_exists(path) && fts_is_file(path)) {
    return fts_new_symbol_copy(path);
  }

  return NULL;
}

fts_symbol_t 
fts_get_system_config( void)
{
  char path[MAXPATHLEN];

  fts_make_absolute_path(DEFAULT_ROOT, "config.jmax", path, MAXPATHLEN);
  if (fts_file_exists(path) && fts_is_file(path)) {
    return fts_new_symbol_copy(path);
  }

  return NULL;  
}

/* *************************************************************************** */
/*                                                                             */
/* Dynamic loader                                                              */
/*                                                                             */
/* *************************************************************************** */

/* The MacOS-X implementation uses undocumented NEXTSTEP rld()...

/* 
#define odbc_private_extern __private_extern__ 
*/ 
#define odbc_private_extern 

static char *stored_error_message;

odbc_private_extern void undefined_symbol_handler( const char *symbolName) 
{
  fprintf( stderr, "undefined symbol: %s\n", symbolName);

/*    if( stored_error_message) */
/*      sprintf( stored_error_message, "undefined symbol: %s", symbolName);  */
} 

odbc_private_extern NSModule multiple_symbol_handler( NSSymbol s, NSModule old, NSModule new) 
{ 
    /* 
     * Since we can't unload symbols, we're going to run into this 
     * every time we reload a module. Workaround here is to just 
     * rebind to the new symbol, and forget about the old one. 
     * This is crummy, because it's basically a memory leak. 
     * (See Radar 2262020 against dyld). 
     */ 

  if( stored_error_message)
    sprintf( stored_error_message, "multiply defined symbol: %s (old %s new %s)", NSNameOfSymbol(s), NSNameOfModule(old), NSNameOfModule(new)); 

  return new; 
} 

odbc_private_extern void linkEdit_symbol_handler (NSLinkEditErrors c, int errorNumber, const char *fileName, const char *errorString) 
{ 
  if( stored_error_message)
    sprintf( stored_error_message, "errors during link edit for file %s : %s", fileName, errorString); 
} 

fts_status_t fts_load_library( const char *filename, const char *symbol)
{
  static char error_description[1024];
  static fts_status_description_t load_library_error = { error_description};
  static int dl_init = 0;
  static char *full_sym_name;
  static int full_sym_name_length;
  NSSymbol s; 
  NSObjectFileImage image; 
  void *ret;

  if ( !dl_init)
    {
      NSLinkEditErrorHandlers handlers; 

      handlers.undefined = undefined_symbol_handler; 
      handlers.multiple = multiple_symbol_handler; 
      handlers.linkEdit = linkEdit_symbol_handler; 

      NSInstallLinkEditErrorHandlers( &handlers);

      dl_init = 1;
    }

  stored_error_message = error_description;

  if ( NSCreateObjectFileImageFromFile( filename, &image) != NSObjectFileImageSuccess )
    return &load_library_error;

  if ( !NSLinkModule( image, filename, NSLINKMODULE_OPTION_BINDNOW))
    return &load_library_error;

  if ( !full_sym_name || strlen( symbol) + 2 >= full_sym_name_length)
    {
      full_sym_name_length = strlen( symbol) + 2;
      full_sym_name = (char *)malloc( full_sym_name_length);
    }

  strcpy( full_sym_name, "_");
  strcat( full_sym_name, symbol);

  s = NSLookupSymbolInModule( (NSModule)handle, full_sym_name); 

  fun = (void (*)(void))NSAddressOfSymbol( s);

  stored_error_message = 0;

  if ( !fun)
    {
      return &load_library_error;
    }

  (*fun)();

  return fts_Success;
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

void fts_platform_init( int argc, char **argv)
{
}
