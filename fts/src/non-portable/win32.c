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
 * Win32 specific functions
 *
 * Author Peter Hanappe
 */
#include <windows.h>

HINSTANCE fts_hinstance = NULL;

void fts_set_hinstance(HINSTANCE hinstance)
{
  if (fts_hinstance == NULL) {
    fts_hinstance = hinstance;
#if DEBUG
    printf("fts_set_hinstance: instance=%d\n", (int) fts_hinstance);
#endif
  }	
}

HINSTANCE fts_get_hinstance()
{
  return fts_hinstance;
}

BOOL WINAPI DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
#if DEBUG
  printf("DllMain\n");
#endif
  fts_set_hinstance((HINSTANCE) hModule);
  return TRUE;
}


/* *************************************************************************** */
/*                                                                             */
/* Dynamic loader                                                              */
/*                                                                             */
/* *************************************************************************** */

/* The Win32 implementation uses dlopen() and dlsym(). */

/*
    The dl code was adapted from TiMidity++

    TiMidity++ -- MIDI to WAVE converter and player
    Copyright (C) 1999-2001 Masanao Izumo <mo@goice.co.jp>
    Copyright (C) 1995 Tuukka Toivonen <tt@cgs.fi>
*/
void *fts_dl_open( const char *filename, char *error)
{
  void *handle;
  
  handle = (void*) LoadLibrary(filename);

  if (!handle) {
    LPVOID msg;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		  NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &msg, 0, NULL);
    strcpy( error, msg);
    LocalFree(msg);
  }

  return handle;
}


int fts_dl_lookup( void *handle, const char *symbol, void **address, char *error)
{
  *address  = (void*) GetProcAddress((HINSTANCE) handle, symbol);

  if (*address == NULL) {
    LPVOID msg;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		  NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &msg, 0, NULL);
    strcpy( error, msg);
    LocalFree(msg);
    return 0;
  }

  return 1;
}


/* *************************************************************************** */
/*                                                                             */
/* Floating-point unit                                                         */
/*                                                                             */
/* *************************************************************************** */

/*
  
      FIXME: Not implemented yet   

 */

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
/* Real path                                                              */
/*                                                                             */
/* *************************************************************************** */

char* win32_realpath(const char* path, char* resolved_path)
{
  return strcpy(resolved_path, path);
}
