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

/** @file config.c
 *  FTS low-level config system implementation.
 *  This file is common to FTS and FTSclient.
 *  @todo Implement all this file on other platforms (if you need it...)
 *  @todo Fix obvious (and known) memory leaks
 *  @ingroup FtsLowLevelConfigSystem
 */

#ifndef WIN32
#error "Implemented only for win32, yet to be implemented on other platforms."
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <windows.h>
#include <shlobj.h>

#include <common/config.h>
#include <common/configfile.h>


/****************************************************
 *
 * Local defines
 *
 ****************************************************/

#define MAX_LENGTH 1024
#define CONFIG_FILE_NAME "fts.cfg"

/** To avoid platform SDK update,
 *  I define some M$ constants by hand (yes, it's awful) (but THEY're awful).
 *  http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/shell/reference/enums/csidl.asp
 * @TODO well, let's talk about this one around a table ?
 */

#ifndef CSIDL_LOCAL_APPDATA
#define CSIDL_LOCAL_APPDATA 0x001c
#endif

#ifndef CSIDL_COMMON_APPDATA
#define CSIDL_COMMON_APPDATA 0x0023
#endif


/************************************************************************/
/*                                                                      */
/*                     Static variables                                 */
/*                                                                      */
/************************************************************************/

/** These are static globals as their values are computed
 *  from the current's process executable file location only.
 */

/** Directory where the user should be pointed to save his files */
static char *l_pzUserFilesDir = NULL;

/** Directory where the config files specific to the user should be put */
static char *l_pzUserConfigDir = NULL;

/** Directory for config files specific to the local machine */
static char *l_pzLocalConfigDir = NULL;

/** Directory for config files specific to the user AND to the local machine */
static char *l_pzUserLocalConfigDir = NULL;

/** Directory for log files */
static char *l_pzLogDir = NULL;

/** Path to user's config file */
static char *l_pzUserConfigFilePath = NULL;

/** Path to local machine's config file */
static char *l_pzLocalConfigFilePath = NULL;

/** Path to user on local machine's config file */
static char *l_pzUserLocalConfigFilePath = NULL;

/** App name (specified by user code) */
static char *l_pzAppName = NULL;

/** App version string (specified by user code) */
static char *l_pzAppVersion = NULL;

/** Vendor name (specified by user code) */
static char *l_pzVendorName = NULL;

/** UserLocalConfigFile */
static config_file_t *l_pUserLocalConfigFile = NULL;

/** LocalConfigFile */
static config_file_t *l_pLocalConfigFile = NULL;

/** UserConfigFile */
static config_file_t *l_pUserConfigFile = NULL;

/** Log function */
void (*l_pLogFunc)(char *msg, ...) = NULL;

/** 1 if config.c has been initialized */
int l_bInitialized = 0;


/************************************************************************/
/*                                                                      */
/*                      Static functions                                */
/*                                                                      */
/************************************************************************/


static int isCurrentProcessExecutedFromNetShare() 
{
    char exePath[MAX_LENGTH];
    
    /* Get the current executable file path. */
    GetModuleFileName(NULL, exePath, sizeof(exePath)-1);

    return fts_is_network_path(exePath);
}

static void stripFileNameFromPath(char *path, const char separator)
{
    int i;

    i = strlen(path)-1;

    while (i>0) {
        if (path[i] == separator)
            path[i]=0;
    }
}


static void createVersionFileIfNotExist(const char *verFileName)
{
    FILE *versionFile = NULL;

    if(fopen(verFileName, "rb") == NULL) {

        versionFile = fopen(verFileName, "wb");
        if(versionFile) {
            fprintf(versionFile, "version = %s\n", l_pzAppVersion);
            fclose(versionFile);
        }
    }
}

static void initConfigDirectories()
{
    char verFileName[MAX_LENGTH];

    /* Retrieve the various "standard" directories. */
    
    /* [RS] Note: I didn't use the more recent SHGetFolderPath function 
     * as it requires to install an update package (redistributable .exe).
     */
    
    /* UserFilesDir */
    if( ! SHGetSpecialFolderPath(NULL, l_pzUserFilesDir, CSIDL_PERSONAL, TRUE)) {
        strcpy(l_pzUserFilesDir, "");
    }

    /* UserLocalConfigDir : <VENDORNAME>\<LOCAL_APPDATA>\<APPNAME> 
     * (or <VENDORNAME>\<APP_DATA>\<APPNAME> if not available 
     * (or C:\<VENDORNAME>\<APPNAME>)) 
     */
    if( ! SHGetSpecialFolderPath(NULL, l_pzUserLocalConfigDir, CSIDL_LOCAL_APPDATA, TRUE)) {
        if( ! SHGetSpecialFolderPath(NULL, l_pzUserLocalConfigDir, CSIDL_APPDATA, TRUE)) {
            strcpy(l_pzUserLocalConfigDir, "c:");
        }
    }
    strcat(l_pzUserLocalConfigDir, "\\");
    strcat(l_pzUserLocalConfigDir, l_pzVendorName);
    CreateDirectory(l_pzUserLocalConfigDir, NULL);
    strcat(l_pzUserLocalConfigDir, "\\");
    strcat(l_pzUserLocalConfigDir, l_pzAppName);
    CreateDirectory(l_pzUserLocalConfigDir, NULL);
    
    /* LocalConfigDir : <VENDORNAME>\<COMMON_APPDATA>\<APPNAME>
     * (or <VENDORNAME>\<APP_DATA>\<APPNAME> if not available 
     *  (or C:\<VENDORNAME>\<APPNAME>)
     * )
     */
    if( ! SHGetSpecialFolderPath(NULL, l_pzLocalConfigDir, CSIDL_COMMON_APPDATA, TRUE)) {
        if( ! SHGetSpecialFolderPath(NULL, l_pzLocalConfigDir, CSIDL_APPDATA, TRUE)) {
            strcpy(l_pzLocalConfigDir, "c:");
        }
    }
    strcat(l_pzLocalConfigDir, "\\");
    strcat(l_pzLocalConfigDir, l_pzVendorName);
    CreateDirectory(l_pzLocalConfigDir, NULL);
    strcat(l_pzLocalConfigDir, "\\");
    strcat(l_pzLocalConfigDir, l_pzAppName);
    CreateDirectory(l_pzLocalConfigDir, NULL);
    
    /* UserConfigDir : <VENDORNAME>\<APPDATA>\<APPNAME>
     * (or C:\<VENDORNAME>\<APPNAME>)
     */
    if(!SHGetSpecialFolderPath(NULL, l_pzUserConfigDir, CSIDL_APPDATA, TRUE)) {
        strcpy(l_pzUserConfigDir, "c:");
    }
    strcat(l_pzUserConfigDir, "\\");
    strcat(l_pzUserConfigDir, l_pzVendorName);
    CreateDirectory(l_pzUserConfigDir, NULL);
    strcat(l_pzUserConfigDir, "\\");
    strcat(l_pzUserConfigDir, l_pzAppName);
    CreateDirectory(l_pzUserConfigDir, NULL);
    
    /* LogDir = UserLocalConfigDir */
    strcpy(l_pzLogDir, l_pzUserLocalConfigDir);

    /* Create a profile version file in each config directory */
    strcpy(verFileName, l_pzUserLocalConfigDir);
    strcat(verFileName, "\\version.cfg");
    createVersionFileIfNotExist(verFileName);

    strcpy(verFileName, l_pzLocalConfigDir);
    strcat(verFileName, "\\version.cfg");
    createVersionFileIfNotExist(verFileName);

    strcpy(verFileName, l_pzUserConfigDir);
    strcat(verFileName, "\\version.cfg");
    createVersionFileIfNotExist(verFileName);
}


/************************************************************************/
/*                                                                      */
/*                      Public functions                                */
/*                                                                      */
/************************************************************************/


enum fts_net_path_type_enum
fts_detect_network_path(const char *path, char *pHostName, 
                        char *pShareName, char *pRoot, 
                        int maxLen
                       )
{
    if(strlen(path)>=2)
    {
        /* If the path starts with \\, it must be a network share
         */
        if (path[0]=='\\' && path[1]=='\\') 
        {
            if((pHostName != NULL) || (pShareName != NULL)) {
                int i, j;
                i = 0;
                j = 2; 
                while( (path[j] != '\\') && (path[j] != 0) ) {
                    if(pHostName != NULL)
                        pHostName[i] = path[j];
                    if(i<maxLen-1)
                        i++;
                    j++;                    
                }
                if(pHostName != NULL)
                    pHostName[i] = 0;
                
                if ((pShareName != NULL) && (path[j] == '\\')) {
                    i = 0;
                    j++;
                    while( (path[j] != '\\') && (path[j] != 0) ) {
                        pShareName[i] = path[j];
                        if(i<maxLen-1)
                            i++;
                        j++;
                    }
                    pShareName[i] = 0;
                }
            }

            if((pRoot!=NULL) && (pShareName != NULL) && (pHostName != NULL)) 
                _snprintf(pRoot, maxLen, "\\\\%s\\%s", pHostName, pShareName);

            return fts_net_path_type_unc;
        }

        /** If the path starts with a letter followed by a ':', and
         *  the letter is a network share mounted as a drive...
         */
        else if(isalpha(path[0]) && path[1]==':')
        {
            char rootPath[4];
			UINT type;
            rootPath[0] = path[0];
            rootPath[1] = path[1];
            rootPath[2] = '\\';
            rootPath[3] = 0;

            type = GetDriveType(rootPath);
            if(type == DRIVE_REMOTE) {
                if ((pRoot != NULL) && maxLen>4) {
                    strcpy(pRoot, rootPath);
                }
                return fts_net_path_type_mounted_drive;
            }
        }
    }

    return fts_net_path_type_local;
}


int fts_is_network_path(const char *path)
{
    enum fts_net_path_type_enum e;
    e = fts_detect_network_path(path, NULL, NULL, NULL, 0);
    return    (e == fts_net_path_type_mounted_drive)
           || (e == fts_net_path_type_unc);
}


/* #define local to the following function */
#define INIT_STRING(s,l) if(s) free(s); s=(char*)malloc(l); strcpy(s, "");
void fts_config_init(const char *vendorName, const char *appName, 
                     const char *appVersion,
                     void (*pLogFunc)(char *msg, ...)
                    )
{
  if(l_bInitialized ) {
      l_pLogFunc("[ftsconfig] fts_config_init(): Already initialized!\n");
  }
  else {
    
      FILE *f = NULL;
    
      INIT_STRING(l_pzUserFilesDir, MAX_LENGTH);
      INIT_STRING(l_pzUserConfigDir, MAX_LENGTH);
      INIT_STRING(l_pzLocalConfigDir, MAX_LENGTH);
      INIT_STRING(l_pzUserLocalConfigDir, MAX_LENGTH);
      INIT_STRING(l_pzLogDir, MAX_LENGTH);  
      INIT_STRING(l_pzAppName, MAX_LENGTH);
      INIT_STRING(l_pzAppVersion, MAX_LENGTH);
      INIT_STRING(l_pzVendorName, MAX_LENGTH);
      INIT_STRING(l_pzUserConfigFilePath, MAX_LENGTH);
      INIT_STRING(l_pzUserLocalConfigFilePath, MAX_LENGTH);
      INIT_STRING(l_pzLocalConfigFilePath, MAX_LENGTH);
    
      l_pLogFunc = pLogFunc;
    
      strcpy(l_pzLogDir, "C:");
      strcpy(l_pzAppName, appName);
      strcpy(l_pzVendorName, vendorName);

      if(appVersion != NULL)
          strcpy(l_pzAppVersion, appVersion);
      else
          strcpy(l_pzAppVersion, "<unspecified>");
      
      /* platform specific initialization */
      initConfigDirectories();
        
      strcpy(l_pzUserLocalConfigFilePath, l_pzUserLocalConfigDir);
      strcat(l_pzUserLocalConfigFilePath, "\\" CONFIG_FILE_NAME);
      if((f=fopen(l_pzUserLocalConfigFilePath, "r")) != NULL) {
          fclose(f);
          l_pUserLocalConfigFile = config_file_open(l_pzUserLocalConfigFilePath);
      }
    
      strcpy(l_pzLocalConfigFilePath, l_pzLocalConfigDir);
      strcat(l_pzLocalConfigFilePath, "\\" CONFIG_FILE_NAME);
      if((f=fopen(l_pzLocalConfigFilePath, "r")) != NULL) {
          fclose(f);
          l_pLocalConfigFile = config_file_open(l_pzLocalConfigFilePath);
      }
    
      strcpy(l_pzUserConfigFilePath, l_pzUserConfigDir);
      strcat(l_pzUserConfigFilePath, "\\" CONFIG_FILE_NAME);
      if((f=fopen(l_pzUserConfigFilePath, "r")) != NULL) {
          fclose(f);
          l_pUserConfigFile = config_file_open(l_pzUserConfigFilePath);
      }
      
    l_bInitialized = 1;  
  }
}
#undef INIT_STRING


void fts_config_log_info()
{
      if(l_pLogFunc) {

          l_pLogFunc("[fts_config] vendor name is '%s'\n", l_pzVendorName);
          l_pLogFunc("[fts_config] app name is '%s'\n", l_pzAppName);

          l_pLogFunc("[fts_config] user local config file %s (%s)\n",
              (l_pUserLocalConfigFile != NULL) ? "found" : "NOT found",
              l_pzUserLocalConfigFilePath
              );
          l_pLogFunc("[fts_config] local config file %s (%s)\n",
              (l_pLocalConfigFile != NULL) ? "found" : "NOT found",
              l_pzLocalConfigFilePath
              );
          l_pLogFunc("[fts_config] user config file %s (%s)\n",
              (l_pUserConfigFile != NULL) ? "found" : "NOT found",
              l_pzUserConfigFilePath
              );
          l_pLogFunc("[fts_config] log dir: '%s'\n",
              l_pzLogDir
              );
          l_pLogFunc("[fts_config] user files dir: '%s'\n",
              l_pzUserFilesDir
              );
      }
}


int fts_config_get_int(const char *name, int *pValue) {

    if(l_pUserLocalConfigFile) {
        if(config_file_get_int(l_pUserLocalConfigFile, name, pValue)>=0)
            return 0;
    }
    else if(l_pLocalConfigFile) {
        if(config_file_get_int(l_pLocalConfigFile, name, pValue)>=0)
            return 0;
    }
    else if(l_pUserConfigFile) {
        if(config_file_get_int(l_pUserConfigFile, name, pValue)>=0)
            return 0;
    }

    return -1;
}


int fts_config_get_float(const char *name, float *pValue) {

    if(l_pUserLocalConfigFile) {
        if(config_file_get_float(l_pUserLocalConfigFile, name, pValue)>=0)
            return 0;
    }
    else if(l_pLocalConfigFile) {
        if(config_file_get_float(l_pLocalConfigFile, name, pValue)>=0)
            return 0;
    }
    else if(l_pUserConfigFile) {
        if(config_file_get_float(l_pUserConfigFile, name, pValue)>=0)
            return 0;
    }

    return -1;
}


int fts_config_get_string(const char *name, char *pValue, int maxLen) {

    if(l_pUserLocalConfigFile) {
        if(config_file_get_string(l_pUserLocalConfigFile, name, pValue, maxLen)>=0)
            return 0;
    }
    else if(l_pLocalConfigFile) {
        if(config_file_get_string(l_pLocalConfigFile, name, pValue, maxLen)>=0)
            return 0;
    }
    else if(l_pUserConfigFile) {
        if(config_file_get_string(l_pUserConfigFile, name, pValue, maxLen)>=0)
            return 0;
    }

    return -1;
}


char *fts_config_get_s(const char *name, const char *pDefault) {
    char buffer[1024];

    if(fts_config_get_string(name, buffer, sizeof(buffer)-1) >= 0)
        return strdup(buffer);
    else {
        if(l_pLogFunc)
            l_pLogFunc("[fts_config] string '%s' not found, using default value: %s\n", name, pDefault);
        return strdup(pDefault);
    }
}

int fts_config_get_i(const char *name, int dflt) {
    int i;

    if(fts_config_get_int(name, &i) >= 0)
        return i;
    else {
        if(l_pLogFunc)
            l_pLogFunc("[fts_config] int '%s' not found, using default value: %i\n", name, dflt);
        return dflt;
    }
}

float fts_config_get_f(const char *name, float dflt) {
    float f;

    if(fts_config_get_float(name, &f) >= 0) {
        return f;
    }
    else {
        if(l_pLogFunc)
            l_pLogFunc("[fts_config] float '%s' not found, using default value: %g\n", name, dflt);
        return dflt;
    }
}

const char *fts_config_get_log_dir() {
    if(l_pzLogDir)
        return l_pzLogDir;
    else
        return "C:";
}

const char *fts_config_get_user_local_config_dir() {
    return l_pzUserLocalConfigDir;    
}

const char *fts_config_get_local_config_dir() {
    return l_pzLocalConfigDir;    
}

const char *fts_config_get_user_config_dir() {
    return l_pzUserConfigDir;    
}

const char *fts_config_get_user_files_dir() {
    return l_pzUserFilesDir;
}

const char *fts_config_get_app_name() {
    return l_pzAppName;
}

const char *fts_config_get_vendor_name() {
    return l_pzVendorName;
}

const char *fts_config_get_app_version() {
    return l_pzAppVersion;
}
