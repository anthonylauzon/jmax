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
#include <lmerr.h>
#include <direct.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "ftsconfig-win32.h"
#include <common/config.h>

// Default (hard-coded) priorities for FTS process
#define FTS_DEFAULT_WIN32_PRIORITY_CLASS HIGH_PRIORITY_CLASS
#define FTS_DEFAULT_WIN32_THREAD_PRIORITY THREAD_PRIORITY_LOWEST


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
    int i, j;
    
    /* retrieve the current exe's file path */
    if ( ! GetModuleFileName(NULL, root, _MAX_PATH)) {
        return NULL;
    }

    fts_log("[win32]: Current exe is '%s'\n", root);

    /* The jmax root is typically located in the parent directory,
    ** as the fts executable is typically in <ftsroot>/bin.
    */
    i = strlen(root);
    j = 2;
    while (--i >= 0) {
        if (root[i] == '\\') {
            root[i] = 0;
            j--;
            if(j<=0)
                break;
        }
    }
    
    fts_log("[win32]: Using default fts root (relative to fts's executable): '%s'\n", root);
    
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
/* Log last win32 error                                                        */
/*                                                                             */
/* *************************************************************************** */



// [RS] The following function is copy-paste from:
// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/netmgmt/netmgmt/looking_up_text_for_error_code_numbers.asp
void fts_log_last_win32_error()
{
    DWORD dwLastError;
    HMODULE hModule = NULL; // default to system source
    LPSTR MessageBuffer;
    DWORD dwBufferLength;
    DWORD dwFormatFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_IGNORE_INSERTS |
        FORMAT_MESSAGE_FROM_SYSTEM ;

    dwLastError = GetLastError();


    //
    // If dwLastError is in the network range, 
    //  load the message source.
    //

    if(dwLastError >= NERR_BASE && dwLastError <= MAX_NERR) {
        hModule = LoadLibraryEx(
            TEXT("netmsg.dll"),
            NULL,
            LOAD_LIBRARY_AS_DATAFILE
            );

        if(hModule != NULL)
            dwFormatFlags |= FORMAT_MESSAGE_FROM_HMODULE;
    }

    //
    // Call FormatMessage() to allow for message 
    //  text to be acquired from the system 
    //  or from the supplied module handle.
    //

    if(dwBufferLength = FormatMessageA(
        dwFormatFlags,
        hModule, // module to get message from (NULL == system)
        dwLastError,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // default language
        (LPSTR) &MessageBuffer,
        0,
        NULL
        ))
    {
        DWORD dwBytesWritten;

        //
        // Output message string on FTS log
        //
        fts_log("[win32] Last win32 error: %s\n", (char *)MessageBuffer);

        //
        // Free the buffer allocated by the system.
        //
        LocalFree(MessageBuffer);
    }

    //
    // If we loaded a message source, unload it.
    //
    if(hModule != NULL)
        FreeLibrary(hModule);
}



/* *************************************************************************** */
/*                                                                             */
/* Win32 process & thread priorities functions                                 */
/*                                                                             */
/* *************************************************************************** */



static struct priorityClassesStruct {

    DWORD code;
    const char *id;
    const char *desc;

} priorityClasses[] =
{ 
    { IDLE_PRIORITY_CLASS, "IDLE_PRIORITY_CLASS", "Idle"},

// We don't allow priorities supported by win9x only.
//#if WINVER < 0x400 
//    { BELOW_NORMAL_PRIORITY_CLASS, "BELOW_NORMAL_PRIORITY_CLASS", "Below normal"},
//#endif 

    { NORMAL_PRIORITY_CLASS, "NORMAL_PRIORITY_CLASS", "Normal"},

// We don't allow priorities supported by win9x only.
//#if WINVER < 0x400 
//    { ABOVE_NORMAL_PRIORITY_CLASS, "ABOVE_NORMAL_PRIORITY_CLASS", "Above normal"},
//#endif

    { HIGH_PRIORITY_CLASS, "HIGH_PRIORITY_CLASS", "High"},
    { REALTIME_PRIORITY_CLASS, "REALTIME_PRIORITY_CLASS", "Realtime"}
};

#define NUM_PRIORITY_CLASSES (sizeof(priorityClasses)/sizeof(struct priorityClassesStruct))



static struct threadPrioritiesStruct {

    int code;
    const char *id;
    const char *desc;

} threadPriorities[] = 
{
    { THREAD_PRIORITY_IDLE, "THREAD_PRIORITY_IDLE", "Idle"},
    { THREAD_PRIORITY_LOWEST, "THREAD_PRIORITY_LOWEST", "Lowest"},
    { THREAD_PRIORITY_BELOW_NORMAL, "THREAD_PRIORITY_BELOW_NORMAL", "Below normal"},
    { THREAD_PRIORITY_NORMAL, "THREAD_PRIORITY_NORMAL", "Normal"},
    { THREAD_PRIORITY_ABOVE_NORMAL, "THREAD_PRIORITY_ABOVE_NORMAL", "Above normal"},
    { THREAD_PRIORITY_HIGHEST, "THREAD_PRIORITY_HIGHEST", "Highest"},
    { THREAD_PRIORITY_TIME_CRITICAL, "THREAD_PRIORITY_TIME_CRITICAL", "Critical"}
};

#define NUM_THREAD_PRIORITIES (sizeof(threadPriorities)/sizeof(struct threadPrioritiesStruct))



int fts_get_win32_priority_class_desc(DWORD code, const char **ppDesc, const char **ppID)
{
    int i;

    for (i=0; i<NUM_PRIORITY_CLASSES; i++) {

        if( priorityClasses[i].code == code ) {

            if( ppID != NULL )
                *ppID = priorityClasses[i].id;

            if( ppDesc != NULL )
                *ppDesc = priorityClasses[i].desc;

            return 0;
        }
    }

    if( ppID != NULL )
        *ppID = "<unknown priority>";

    if( ppDesc != NULL )
        *ppDesc = "<unknown priority>";

    return -1;
}


DWORD fts_get_win32_priority_class_by_desc(const char *desc, DWORD *pCode)
{
    int i;

    for (i=0; i<NUM_PRIORITY_CLASSES; i++) {

        if( !_stricmp( priorityClasses[i].desc, desc )) {

            if (pCode != NULL)
                *pCode = priorityClasses[i].code;

            return 0;
        }
    }
    return -1;
}


DWORD fts_get_win32_priority_class_by_id(const char *id, int *pCode)
{
    int i;

    for (i=0; i<NUM_PRIORITY_CLASSES; i++) {

        if( !_stricmp( priorityClasses[i].id, id )) {

            if (pCode != NULL)
                *pCode = priorityClasses[i].code;

            return 0;
        }
    }
    return -1;
}


int fts_get_win32_thread_priority_desc(int code, const char **ppDesc, const char **ppID)
{
    int i;

    for (i=0; i<NUM_THREAD_PRIORITIES; i++) {

        if( threadPriorities[i].code == code ) {

            if( ppID != NULL )
                *ppID = threadPriorities[i].id;

            if( ppDesc != NULL )
                *ppDesc = threadPriorities[i].desc;

            return 0;
        }
    }

    if( ppID != NULL )
        *ppID = "<unknown priority>";

    if( ppDesc != NULL )
        *ppDesc = "<unknown priority>";

    return -1;
}


int fts_get_win32_thread_priority_by_desc(const char *desc, DWORD *pCode)
{
    int i;

    for (i=0; i<NUM_THREAD_PRIORITIES; i++) {

        if( !_stricmp( threadPriorities[i].desc, desc )) {

            if (pCode != NULL)
                *pCode = threadPriorities[i].code;

            return 0;
        }
    }
    return -1;
}


int fts_get_win32_thread_priority_by_id(const char *id, int *pCode)
{
    int i;

    for (i=0; i<NUM_THREAD_PRIORITIES; i++) {

        if( !_stricmp( threadPriorities[i].id, id )) {

            if (pCode != NULL)
                *pCode = threadPriorities[i].code;

            return 0;
        }
    }
    return -1;
}


int fts_set_win32_thread_priority(int threadPriority)
{
    const char *pszDesc = NULL;
    const char *pszID = NULL;
    
    if( ! fts_get_win32_thread_priority_desc(threadPriority, &pszDesc, &pszID) ) {

        fts_log("[win32] Attempting to change thread priority to '%s' (%s)\n", pszDesc, pszID);

        if(!SetThreadPriority(GetCurrentThread(), threadPriority)) {
            fts_log("[win32] Error: Failed to set current thread's priority.\n");
            fts_log_last_win32_error();
            return -1;
        }

        fts_log("[win32] Thread priority changed.\n");

        return 0;

    } else {

        fts_log("[win32] Error: Tried to change to unknown thread priority %d\n", threadPriority);
        return -1;

    }
}



int fts_get_win32_thread_priority() 
{
    int code;
    code = GetThreadPriority(GetCurrentThread());
    if(code == THREAD_PRIORITY_ERROR_RETURN) {
        fts_log("[win32] Error: Failed to get current thread's priority.\n");
        fts_log_last_win32_error();
        return THREAD_PRIORITY_ERROR_RETURN;
    }
    else 
        return code;
}


int fts_set_win32_priority_class(DWORD priorityClass)
{
    const char *pszDesc = NULL;
    const char *pszID = NULL;
    
    if( ! fts_get_win32_priority_class_desc(priorityClass, &pszDesc, &pszID) ) {

        fts_log("[win32] Attempting to change process priority class to '%s' (%s)\n", pszDesc, pszID);

        if(!SetPriorityClass(GetCurrentProcess(), priorityClass)) {
            fts_log("[win32] Error: Failed to set current process's priority class.\n");
            fts_log_last_win32_error();
            return -1;
        }

        fts_log("[win32] Process priority class changed.\n");

        return 0;

    } else {

        fts_log("[win32] Error: Tried to change to unknown priority class %d\n", priorityClass);
        return -1;

    }
}


DWORD fts_get_win32_priority_class() 
{
    int code;
    code = GetPriorityClass(GetCurrentProcess());
    if(!code) {
        fts_log("[win32] Error: Failed to get current process's priority class.\n");
        fts_log_last_win32_error();
        return THREAD_PRIORITY_ERROR_RETURN;
    }
    else 
        return code;
}


static void fts_init_win32_thread_priority(void)
{
    char szPriorityClass[128];
    char szThreadPriority[128];
    int i;
    int bThreadPriorityChanged = 0, bPriorityClassChanged = 0;
        
    if(fts_config_get_string("priorityClass", szPriorityClass, 127)>=0) {

        DWORD PriorityClass;

        fts_log("[win32] Using priority class specified in registry:\n");

        if( !fts_get_win32_priority_class_by_desc(szPriorityClass, &PriorityClass) ) {

            bPriorityClassChanged = ! fts_set_win32_priority_class(PriorityClass);

        } else {

            fts_log("[win32] Unknown priority class '%s'\n", szPriorityClass);
            fts_log("[win32] Known values are:\n");

            for(i=0; i<NUM_PRIORITY_CLASSES; i++) {
                fts_log("[win32] %s\n", priorityClasses[i].desc);
            }
        }

    } else {
        fts_log("[win32] Using default hard-coded priority class:\n");
        bPriorityClassChanged = ! fts_set_win32_priority_class(FTS_DEFAULT_WIN32_PRIORITY_CLASS);
    }
    
    if( ! bPriorityClassChanged ) {
        DWORD priorityClass;
        const char *pID = NULL;

        fts_log("[win32] Default process priority class was NOT modified.\n");

        priorityClass = fts_get_win32_priority_class();
        fts_get_win32_priority_class_desc(priorityClass, NULL, &pID);

        fts_log("[win32] Process priority class is %s (%d).\n", pID, priorityClass); 
    }

    if(fts_config_get_string("threadPriority", szThreadPriority, 127)>=0) {

        DWORD threadPriority;

        fts_log("[win32] Using thread priority specified in registry:\n");

        if( !fts_get_win32_thread_priority_by_desc(szThreadPriority, &threadPriority) ) {

            bThreadPriorityChanged = ! fts_set_win32_thread_priority(threadPriority);

        } else {

            fts_log("[win32] Unknown thread priority '%s'\n", szThreadPriority);
            fts_log("[win32] Known values are:\n");
            for(i=0; i<NUM_THREAD_PRIORITIES; i++) {
                fts_log("[win32] %s\n", threadPriorities[i].desc);
            }
        }
    } else {
        fts_log("[win32] Using default hard-coded thread priority:\n");
        bThreadPriorityChanged = ! fts_set_win32_thread_priority(FTS_DEFAULT_WIN32_THREAD_PRIORITY);
    }
    
    if( ! bThreadPriorityChanged ) {
        int threadPriority;
        const char *pID = NULL;

        fts_log("[win32] Default thread priority was NOT modified.\n");

        threadPriority = fts_get_win32_thread_priority();
        fts_get_win32_thread_priority_desc(threadPriority, NULL, &pID);

        fts_log("[win32] Thread priority is %s (%d).\n", pID, threadPriority); 
    }
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

    /* print out the current version in the log file */
    /* [RS] ...no more, as everything is in config files now */
    /*
    if ((RegOpenKeyEx(HKEY_LOCAL_MACHINE, JMAX_KEY, 0, KEY_READ, &key) == 0) &&
        (fts_get_registry_string(key, "FtsVersion", version, 256) == 0)) {
        fts_log("[win32]: FtsVersion %s\n", version);
    } else {
        post("Error opening registry key '%s'\n", JMAX_KEY);
        fts_log("[win32]: Error opening registry key '%s'\n", JMAX_KEY);
    }
    */

    /* set thread priority */
    fts_init_win32_thread_priority();
}

