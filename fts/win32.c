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

#include <fts/fts.h>
#include <ftsconfig.h>
#include <direct.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <ftsprivate/platform.h>
#include <ftsprivate/package.h>
#include <fts/thread.h>

#include <shlobj.h> /* SHGetFolderPath */

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

  return fts_ok;
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

  /* load audio packages */
  fts_package_load(fts_new_symbol("dsdev"));
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
    fts_post("Error opening registry key '%s'\n", JMAX_KEY);
    return 0;
  }

  if (!fts_get_string_from_registry(key, "FtsVersion", version, 256)) {
    fts_post("Failed to read the value of registry key: '%s\\FtsVersion'\n", JMAX_KEY);
    RegCloseKey(key);
    return 0;
  }

  if (RegOpenKeyEx(key, version, 0, KEY_READ, &version_key) != 0) {
    fts_post("Error opening registry key '%s\\%s'\n", JMAX_KEY, version);
    return 0;
  }

  if (!fts_get_string_from_registry(version_key, "ftsRoot", buf, bufsize)) {
    fts_post("Failed to read the value of registry key: '%s\\%s\\ftsRoot'\n", JMAX_KEY, version);
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

  return fts_new_symbol( root);
}


fts_symbol_t get_user_directory(void)
{
  char dir[_MAX_PATH];
  /*  
  SHGetFolderLocation();
  SHGetSpecialFolderPath();
  */
  HRESULT err;
  err = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, dir);
  if (SUCCEEDED(err))
    {
      fts_log("[user] user directory: %s\n", dir);
    }
  else
  {
    if ((S_FALSE == err) ||(E_FAIL == err))
    {
      fts_post("[user] get_user_directory : The folder does not exist \n");
    }
    if (E_INVALIDARG == err)
    {
      fts_post("[user] get_user_directory: The CSIDL in nFolder is not valid \n");
    }
    fts_log("[user] cannot get user directory \n");
    return NULL;
  }

  return fts_new_symbol(dir);
}

/* *************************************************************************** 
 *                                                                             
 * Project and configuration files
 *                                                                             
 */

fts_symbol_t 
fts_get_user_project( void)
{  
  char *cwd = NULL;
  char path[_MAX_PATH];
  cwd = (char*)get_user_directory();
  /*  strcat(cwd,"\\config"); */
  
  fts_make_absolute_path(cwd, fts_s_default_project, path, _MAX_PATH);
  return fts_new_symbol(path);
  
}

fts_symbol_t 
fts_get_system_project( void)
{
  fts_symbol_t root;
  char win[_MAX_PATH];
  char path[_MAX_PATH];
  char* cwd = NULL;

  /* check the config file in the root directory */
  root = fts_get_default_root_directory();  
  strcat(root, "\\config");
  fts_make_absolute_path(root, fts_s_default_project, path, _MAX_PATH);
  if (fts_file_exists(path) && fts_is_file(path)) {
    return fts_new_symbol(path);
  }

  /* check the config file in the windows directory */
  if (GetWindowsDirectory(win, _MAX_PATH) == 0) {
    return NULL;
  }

  fts_make_absolute_path(win, fts_s_default_project, path, _MAX_PATH);
  if (fts_file_exists(path) && fts_is_file(path)) {
    return fts_new_symbol(path);
  }

  return NULL;
}


fts_symbol_t 
fts_get_user_configuration( void)
{
  char path[_MAX_PATH];


  fts_make_absolute_path(get_user_directory(), fts_s_default_config, path, _MAX_PATH);
  return fts_new_symbol(path);
}

fts_symbol_t 
fts_get_system_configuration( void)
{
  fts_symbol_t root;
  char win[_MAX_PATH];
  char path[_MAX_PATH];

  /* check the config file in the root directory */
  root = fts_get_default_root_directory();

  fts_make_absolute_path(root, fts_s_default_config, path, _MAX_PATH);
  if (fts_file_exists(path) && fts_is_file(path)) {
    return fts_new_symbol(path);
  }

  /* check the config file in the windows directory */
  if (GetWindowsDirectory(win, _MAX_PATH) == 0) {
    return NULL;
  }

  fts_make_absolute_path(win, fts_s_default_config, path, _MAX_PATH);
  if (fts_file_exists(path) && fts_is_file(path)) {
    return fts_new_symbol(path);
  }

  return NULL;
}

/* **********************************************************************
 *
 * Windows specific code
 *
 */
int
win_close(int socket)
{
  int r;
  char buf[1024];
  if (socket != INVALID_SOCKET) {
    shutdown(socket, 0x02);
    while (1) {
      r = recv(socket, buf, 1024, 0);
      if ((r == 0) || (r == SOCKET_ERROR)) {
	break;
      }
    }
    closesocket(socket);
  }
  return 0;
}


/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */

void* WINAPI 
thread_manager_run_thread(void* arg)
{
    fts_thread_function_t* thread_func = (fts_thread_function_t*)arg;
    fts_object_t* object = thread_func->object;
    fts_method_t method = thread_func->method;
    fts_symbol_t symbol = thread_func->symbol;
    int ac = thread_func->ac;
    fts_atom_t* at = thread_func->at;
    int delay_ms = thread_func->delay_ms;
   
	unsigned long timeToSleep = delay_ms * 1000 * 1000;

    while(1)
    {
	if (0 == thread_func->is_dead)
	{
	    method(object, fts_system_inlet, symbol, ac, at);
	}
	Sleep(timeToSleep);
    }

    return 0;
}

FTS_API int
thread_manager_start(thread_manager_t * self)
{

	 HANDLE hThread; 	
   
   hThread = CreateThread(
	   NULL,
	   0,
	   thread_manager_main,
	   &(void*)self,
	   0,
	   &self->thread_manager_ID);
	if(hThread != NULL)
		return 0;
	CloseHandle(hThread);
	return -1;
}

/** 
 * Main function of the thread manager
 * 
 * @param arg 
 * 
 * @return 
 */
void* WINAPI
thread_manager_main(void* arg)
{
    int work_done;
	DWORD threadCheck;
    HANDLE success;
    thread_manager_t* manager = (thread_manager_t*)arg;

    while(1)
    {
		work_done = 0;
		/* Is there some data in create FIFO */
		if (fts_fifo_read_level(&manager->create_fifo) >= sizeof(fts_atom_t))
		{
			fts_atom_t* atom = (fts_atom_t*)fts_fifo_read_pointer(&manager->create_fifo);	    
			fts_thread_worker_t* worker;

		    fts_fifo_incr_read(&manager->create_fifo, sizeof(fts_atom_t));

		    worker = (fts_thread_worker_t*)fts_get_pointer(atom);

			success = CreateThread(NULL,
									0,
									thread_manager_run_thread,
									(void*)worker->thread_function,
									0,
									&worker->id);
			worker->threadHandle = success;
			if (NULL == success)
			{
				fts_post("[thread_manager] cannot start a new thread \n");
			}
			else
			{
		
				work_done++;

			}

		}
		/* Is there some data in cancel FIFO */
		if (fts_fifo_read_level(&manager->cancel_fifo) >= sizeof(fts_atom_t))
		{
			fts_atom_t* atom = (fts_atom_t*)fts_fifo_read_pointer(&manager->cancel_fifo);	    
			fts_thread_worker_t* worker;

			fts_fifo_incr_read(&manager->cancel_fifo, sizeof(fts_atom_t));

			worker = (fts_thread_worker_t*)fts_get_pointer(atom);
			success = worker->threadHandle;
			threadCheck = TerminateThread(success, GetExitCodeThread(success, 0));
			
			if (0xFFFFFFFF == threadCheck)
			{
				if (NULL == success)
				{
					fts_post("[thread manager] no such thread \n");
				}
				else{ CloseHandle(success);}
				fts_post("[thread manager] error while cancelling thread \n");
			}
			work_done++;
		}
		if (0 == work_done)
		{	
			int delay_ms = manager->delay_ms;
			unsigned long timeToSleep = delay_ms * 1000 * 1000;
			Sleep(timeToSleep);
		}
    }
    
    return 0;
}
