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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mach-o/dyld.h> 

#include <fts/sys.h>


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

void *fts_dl_open( const char *filename, char *error)
{
  NSObjectFileImage image; 
  void *ret;
  static int dl_init = 0;

  if ( !dl_init)
    {
      NSLinkEditErrorHandlers handlers; 

      handlers.undefined = undefined_symbol_handler; 
      handlers.multiple = multiple_symbol_handler; 
      handlers.linkEdit = linkEdit_symbol_handler; 

      NSInstallLinkEditErrorHandlers( &handlers);

      dl_init = 1;
    }

  stored_error_message = error;

  if ( NSCreateObjectFileImageFromFile( filename, &image) != NSObjectFileImageSuccess )
    return 0;

  ret = NSLinkModule( image, filename, NSLINKMODULE_OPTION_BINDNOW); 

  stored_error_message = 0;

  return ret;
}

int fts_dl_lookup( void *handle, const char *symbol, void **address, char *error)
{
  static char *full_sym_name;
  static int full_sym_name_length;
  NSSymbol s; 

  if ( !full_sym_name || strlen( symbol) + 2 >= full_sym_name_length)
    {
      full_sym_name_length = strlen( symbol) + 2;
      full_sym_name = (char *)malloc( full_sym_name_length);
    }

  strcpy( full_sym_name, "_");
  strcat( full_sym_name, symbol);

  stored_error_message = error;

  s = NSLookupSymbolInModule( (NSModule)handle, full_sym_name); 

  *address = NSAddressOfSymbol( s);

  stored_error_message = 0;

  return 1;
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

