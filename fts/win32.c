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

#define JMAX_KEY        "Software\\Ircam\\jMax" 

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

BOOL WINAPI DllMain(HANDLE hModule, DWORD reason, LPVOID lpReserved)
{
  fts_set_hinstance((HINSTANCE) hModule);

  switch (reason) {
  case DLL_PROCESS_ATTACH:
    break;

  case DLL_PROCESS_DETACH:
    break;
  }
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
/* Root                                                                        */
/*                                                                             */
/* *************************************************************************** */

static int
fts_get_registry_string(HKEY key, const char *name, char *buf, int bufsize)
{
  DWORD type, size;

  if (RegQueryValueEx(key, name, 0, &type, 0, &size) == 0
      && type == REG_SZ
      && (size < (unsigned int)bufsize)) {
    if (RegQueryValueEx(key, name, 0, 0, buf, &size) == 0) {
      return 0;
    }
  }
  return -1;
}

int
fts_get_regvalue_string(char* name, char* buf, int len)
{
  HKEY key;
  HKEY version_key;
  char version[256];

  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, JMAX_KEY, 0, KEY_READ, &key) != 0) {
    return -1;
  }
  if (fts_get_registry_string(key, "FtsVersion", version, 256) != 0) {
    RegCloseKey(key);
    return -1;
  }
  if (RegOpenKeyEx(key, version, 0, KEY_READ, &version_key) != 0) {
    return -1;
  }
  if (fts_get_registry_string(version_key, name, buf, len) != 0) {
    RegCloseKey(key);
    RegCloseKey(version_key);
    return -1;
  }

  RegCloseKey(key);
  RegCloseKey(version_key);

  return 0;  
}

int
fts_set_regvalue_string(char* name, const char* value)
{
  HKEY key;
  HKEY version_key;
  char version[256];

  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, JMAX_KEY, 0, KEY_READ, &key) != 0) {
    return -1;
  }
  if (fts_get_registry_string(key, "FtsVersion", version, 256) != 0) {
    RegCloseKey(key);
    return -1;
  }
  if (RegOpenKeyEx(key, version, 0, KEY_WRITE, &version_key) != 0) {
    return -1;
  }
  if (RegSetValueEx(version_key, name, 0, REG_SZ, value, strlen(value) + 1) != ERROR_SUCCESS) {
    RegCloseKey(key);
    RegCloseKey(version_key);
    return -1;
  }

  RegCloseKey(key);
  RegCloseKey(version_key);

  return 0;  
}

int
fts_get_regvalue_int(char* name, int* value)
{
  char buf[256];

  if (fts_get_regvalue_string(name, buf, 256) != 0) {
    return -1;
  }
  *value = atoi(buf);
  return 0;
}

int
fts_set_regvalue_int(char* name, int value)
{
  char buf[256];

  _snprintf(buf, 256, "%i", value);
  return fts_set_regvalue_string(name, buf);
}

fts_symbol_t 
fts_get_default_root_directory( void)
{
  char root[_MAX_PATH];
  int i;

  /* first check the registry */
  if (fts_get_regvalue_string("ftsRoot", root, _MAX_PATH) != 0) {

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

    fts_log("[win32]: Using a fts root based on current directory: '%s'\n", root);

  } else {
    fts_log("[win32]: Using the fts root from the registry: '%s'\n", root);
  }

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

  fts_make_absolute_path(fts_symbol_name(root), "config.jmax", path, _MAX_PATH);
  if (fts_file_exists(path) && fts_is_file(path)) {
    return fts_new_symbol_copy(path);
  }

  /* check the config file in the windows directory */
  if (GetWindowsDirectory(win, _MAX_PATH) == 0) {
    return NULL;
  }

  fts_make_absolute_path(win, "config.jmax", path, _MAX_PATH);
  if (fts_file_exists(path) && fts_is_file(path)) {
    return fts_new_symbol_copy(path);
  }

  return NULL;
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

  HKEY key;
  char version[256];

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
  SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

  /* print out the current version in the log file */
  if ((RegOpenKeyEx(HKEY_LOCAL_MACHINE, JMAX_KEY, 0, KEY_READ, &key) == 0) &&
      (fts_get_registry_string(key, "FtsVersion", version, 256) == 0)) {
    fts_log("[win32]: FtsVersion %s\n", version);
  } else {
    post("Error opening registry key '%s'\n", JMAX_KEY);
    fts_log("[win32]: Error opening registry key '%s'\n", JMAX_KEY);
  }
}

