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
#include <fts/fts.h>
#include <windows.h>
#include <direct.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "ftsconfig-win32.h"

HINSTANCE fts_hinstance = NULL;

void fts_set_hinstance(HINSTANCE hinstance)
{
  if (fts_hinstance == NULL) {
    fts_hinstance = hinstance;
  }	
}

HINSTANCE fts_get_hinstance()
{
  return fts_hinstance;
}

BOOL WINAPI DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
  fts_set_hinstance((HINSTANCE) hModule);
  return TRUE;
}


/* *************************************************************************** */
/*                                                                             */
/* Dynamic loader                                                              */
/*                                                                             */
/* *************************************************************************** */

fts_status_t fts_load_library( const char *filename, const char *symbol)
{
  static char error_description[1024];
  static fts_status_description_t load_library_error = { error_description};
  HINSTANCE handle;
  void (*fun)(void);

  handle = LoadLibrary(filename);

  if (!handle)
    {
      LPVOID msg;
      FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		    NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &msg, 0, NULL);
      strcpy( error_description, msg);
      LocalFree(msg);
      return &load_library_error;
    }

  fun = (void (*)(void))GetProcAddress(handle, symbol);

  if ( fun == NULL)
    {
      LPVOID msg;
      FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		    NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &msg, 0, NULL);
      strcpy( error_description, msg);
      LocalFree(msg);
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

/*
  
      FIXME: Not implemented yet. check out _clear87, _control87, _status87.

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
  return NULL;
}


/* *************************************************************************** */
/*                                                                             */
/* System time                                                                   */
/*                                                                             */
/* *************************************************************************** */

double fts_systime()
{
  return (double) GetTickCount();
}

/* *************************************************************************** */
/*                                                                             */
/* Platform specific initialization                                            */
/*                                                                             */
/* *************************************************************************** */

void fts_platform_init( int argc, char **argv)
{
  WORD wVersionRequested;
  WSADATA wsaData;
  int result;
  SOCKET sock;

  wVersionRequested = MAKEWORD(2, 2);
  
  result = WSAStartup( wVersionRequested, &wsaData );
  if (result != 0) {
    MessageBox(NULL, "Couldn't initialize the TCP/IP layer", 
	       "FTS Initialization", MB_OK | MB_ICONSTOP | MB_APPLMODAL); 
    return /* FIXME */;
  }
  
  if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
    WSACleanup();
    MessageBox(NULL, "The version of the TCP/IP layer installed on your machine is invalid", 
	       "FTS Initialization", MB_OK | MB_ICONSTOP | MB_APPLMODAL); 
    return /* FIXME */;
  }
  
  if ((sock = socket(AF_INET, SOCK_STREAM, 0) ) == -1)	{
    MessageBox(NULL, "Coulnd't create a socket. " 
	       "Make sure your machine is configured with TCP/IP and uses a recent TCP/IP library.", 
	       "FTS Initialization", MB_OK | MB_ICONSTOP | MB_APPLMODAL); 
    return;
  }
  closesocket(sock);

  /* boost the priority of the fts thread */
/*    SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS); */
  SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
}


/* *************************************************************************** */
/*                                                                             */
/* Root                                                                        */
/*                                                                             */
/* *************************************************************************** */

#define JMAX_KEY        "Software\\Ircam\\jMax" 

static int
fts_get_string_from_registry(HKEY key, const char *name, char *buf, int bufsize)
{
  DWORD type, size;

  if (RegQueryValueEx(key, name, 0, &type, 0, &size) == 0
      && type == REG_SZ
      && (size < (unsigned int)bufsize)) {
    if (RegQueryValueEx(key, name, 0, 0, buf, &size) == 0) {
      return 1;
    }
  }
  return 0;
}

static int 
fts_get_root_from_registry(char *buf, int bufsize)
{
  HKEY key;
  HKEY version_key;
  char version[256];

  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, JMAX_KEY, 0, KEY_READ, &key) != 0) {
    post("Error opening registry key '%s'\n", JMAX_KEY);
    return 0;
  }

  if (!fts_get_string_from_registry(key, "FtsVersion", version, 256)) {
    post("Failed to read the value of registry key: '%s\\FtsVersion'\n", JMAX_KEY);
    RegCloseKey(key);
    return 0;
  }

  if (RegOpenKeyEx(key, version, 0, KEY_READ, &version_key) != 0) {
    post("Error opening registry key '%s\\%s'\n", JMAX_KEY, version);
    return 0;
  }

  if (!fts_get_string_from_registry(version_key, "ftsRoot", buf, bufsize)) {
    post("Failed to read the value of registry key: '%s\\%s\\ftsRoot'\n", JMAX_KEY, version);
    RegCloseKey(key);
    RegCloseKey(version_key);
    return 0;
  }

  RegCloseKey(key);
  RegCloseKey(version_key);

  return 1;
}

fts_symbol_t 
fts_get_default_root_directory( void)
{
  char root[_MAX_PATH];
  int i;

  /* first check the registry */
  if (!fts_get_root_from_registry(root, _MAX_PATH)) {

    /* otherwise, calculate the root from the current directory */
    if (GetCurrentDirectory(_MAX_PATH, root) == 0) {
      return NULL;
    }
    /* move one directory up */
    i = strlen(root);
    while (--i >= 0) {
      if (root[i] == '\\') {
	root[i] = 0;
	break;
      }
    }
  }

  fts_log("[win32]: Using '%s' as document root\n", root);

  return fts_new_symbol_copy( root);
}

fts_symbol_t 
fts_get_user_config( void)
{
  char cwd[_MAX_PATH];
  char path[_MAX_PATH];

  /* check for a config file in the current directory */
  if (GetCurrentDirectory(_MAX_PATH, cwd) == 0) {
    return NULL;
  }

  fts_make_absolute_path(cwd, "config.jmax", path, _MAX_PATH);
  if (fts_file_exists(path) && fts_is_file(path)) {
    return fts_new_symbol_copy(path);
  }

  return NULL;
}

fts_symbol_t 
fts_get_system_config( void)
{
  fts_symbol_t root;
  char win[_MAX_PATH];
  char path[_MAX_PATH];

  /* check the config file in the root directory */
  root = fts_get_default_root_directory();

  fts_make_absolute_path(root, "config.jprj", path, _MAX_PATH);
  if (fts_file_exists(path) && fts_is_file(path)) {
    return fts_new_symbol_copy(path);
  }

  /* check the config file in the windows directory */
  if (GetWindowsDirectory(win, _MAX_PATH) == 0) {
    return NULL;
  }

  fts_make_absolute_path(win, "config.jprj", path, _MAX_PATH);
  if (fts_file_exists(path) && fts_is_file(path)) {
    return fts_new_symbol_copy(path);
  }

  return NULL;
}

