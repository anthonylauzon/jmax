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
#include <windows.h>
#include <asiosys.h> /* must be included before asio.h */
#include <asio.h>
#include "jmax_asio_port.h"


/* driver error code */
#define DRVERR -5000
#define DRVERR_INVALID_PARAM DRVERR-1
#define DRVERR_DEVICE_ALREADY_OPEN DRVERR-2
#define DRVERR_DEVICE_NOT_FOUND	DRVERR-3

/* max length for a driver name */
#define MAX_DRIVER_NAME_LENGTH 128

/* max length for a path */
#define MAX_PATH_LEN 512

/* for registry */
#define ASIODRV_DESC "description"
#define INPROC_SERVER "InprocServer32"
#define ASIO_PATH "software\\asio"
#define COM_CLSID "clsid"

/* typedef struct _asio_driver_t asio_driver_t; */
// some external references
bool loadAsioDriver(char *name);


/*
 * Create the ASIO driver object (COM instance)
 */
static LONG asio_open_driver(asio_driver_t* asio_driver)
{
  LPVOID* asiodrv = (void**)&asio_driver->driver_interface;

  long ret;
  BOOL found = FALSE;

  if(!asio_driver->asiodrv)
  {
      ret = CoCreateInstance(asio_driver->clsid, 0, CLSCTX_INPROC_SERVER, asio_driver->clsid, asiodrv);
      if(S_OK == ret)
    	{
	      asio_driver->asiodrv = *asiodrv;
    	  ret = 0;
	    }
  }
  else
  {
    ret = DRVERR_DEVICE_ALREADY_OPEN;
  }

  return ret;
}



static LONG find_driver_path(char* clsid_str, char* dllpath, int dllpath_size)
{
  HKEY regKeyEnum;
  HKEY regKeySub;
  HKEY regKeyPath;
  char databuf[MAX_PATH_LEN];
  LONG err;
  LONG ret = -1;
  DWORD datatype;
  DWORD datasize;
  DWORD index;
  OFSTRUCT ofs;
  HFILE file;
  BOOL found = FALSE;
    
  CharLowerBuff(clsid_str, strlen(clsid_str));
  err = RegOpenKey(HKEY_CLASSES_ROOT, COM_CLSID, &regKeyEnum);
  if (ERROR_SUCCESS == err)
  {
      index = 0;
      while (err == ERROR_SUCCESS && !found)
	    {
    	  err = RegEnumKey(regKeyEnum, index++, (LPSTR)databuf, MAX_PATH_LEN);
	      if(ERROR_SUCCESS == err)
  	    {
	        CharLowerBuff(databuf, strlen(databuf));
	        if(!strcmp(databuf, clsid_str))
      		{
		        err = RegOpenKeyEx(regKeyEnum, (LPCSTR)databuf, 0, KEY_READ, &regKeySub);
		        if(ERROR_SUCCESS == err)
		        {
		          err = RegOpenKeyEx(regKeySub, (LPCTSTR)INPROC_SERVER, 0, KEY_READ, &regKeyPath);
		      
    		      datatype = REG_SZ;
		          datasize = (DWORD)dllpath_size;
		          ret = RegQueryValueEx(regKeyPath, 0, 0, &datatype, (LPBYTE)dllpath, &datasize);
		          if(ERROR_SUCCESS == err)
			        {
			          memset(&ofs, 0, sizeof(OFSTRUCT));
			          ofs.cBytes = sizeof(OFSTRUCT);
			          file = OpenFile(dllpath, &ofs, OF_EXIST);
			          if(file)
			          {
			            ret = 0;
		            }
			          RegCloseKey(regKeyPath);
			        }
		          RegCloseKey(regKeySub);
		        }
		        found = TRUE;
		      }
	      }
	    }
      RegCloseKey(regKeyEnum);
  }
  return ret;
}



/* This method creates an initialized asio_driver_t */
static asio_driver_t* asio_util_create_driver(HKEY regKey, char* regKeyName, int driverID)
{
  HKEY regSubKey;
  char databuf[MAX_DRIVER_NAME_LENGTH];
  char dllpath[MAX_PATH_LEN];
  WORD data[100];
  CLSID clsid;
  DWORD datatype;
  DWORD datasize;
  LONG err;
  LONG ret;
  asio_driver_t* driver = NULL;

  err = RegOpenKeyEx(regKey, (LPCSTR)regKeyName, 0, KEY_READ, &regSubKey);
  if (ERROR_SUCCESS == err)
  {
      datatype = REG_SZ;
      datasize = 256;
      err = RegQueryValueEx(regSubKey, COM_CLSID, 0, &datatype, (LPBYTE)databuf, &datasize);
      if (ERROR_SUCCESS == err)
	    {
	      ret = find_driver_path(databuf, dllpath, MAX_PATH_LEN);
	      if(0 == ret)
	      {
	        driver = (asio_driver_t*)fts_malloc(sizeof(asio_driver_t));
	        if (0 != driver)
		      {
		        memset(driver, 0, sizeof(asio_driver_t));
		        driver->ID = driverID;
		        driver->asiodrv = 0;
		        /* set dllpath */
		        strcpy(driver->dllpath, dllpath);
		        MultiByteToWideChar(CP_ACP, 0, (LPCSTR)databuf, -1, (LPWSTR)data, 100);
		        err = CLSIDFromString((LPOLESTR)data, (LPCLSID)&clsid);
		        if(S_OK == err)
		        {
		          memcpy(&driver->clsid, &clsid, sizeof(CLSID));
		        }
		        datatype = REG_SZ;
		        datasize = 256;
		        err = RegQueryValueEx(regSubKey, ASIODRV_DESC, 0, &datatype, (LPBYTE)databuf, &datasize);
		        if(ERROR_SUCCESS == err)
		        {
		      	  strcpy(driver->name, databuf);
		        }
		        else
		        {
		          strcpy(driver->name, regKeyName);
		        }
		    }
	    }
	  }
    RegCloseKey(regSubKey);
  }  

  return driver;
}

/*
 * scan registry for asio drivers, add each found and working driver to the
 * audiomanager port list, and return the number of drivers found.
 */
unsigned int asio_util_scan_drivers()
{
  /* local vars */
  HKEY regKeyEnum = 0;
  char regKeyName[MAX_DRIVER_NAME_LENGTH];
  LONG err;
  DWORD index = 0;
  unsigned int count = 0;
  asio_driver_t* driver = 0;
  fts_atom_t at;

  /* initialize the COM library */
  CoInitialize(0);

  /* open registry key for ASIO driver */
  err = RegOpenKey(HKEY_LOCAL_MACHINE, ASIO_PATH,&regKeyEnum);
  while(err == ERROR_SUCCESS)
  {
    /*loop on ASIO drivers */
    err = RegEnumKey(regKeyEnum, index++, (LPSTR)regKeyName, MAX_DRIVER_NAME_LENGTH);
    if(err == ERROR_SUCCESS)
    {	
      /* create an initialized asio_driver_t, but don't instantiate the COM object */  
	    driver = asio_util_create_driver(regKeyEnum, regKeyName, index);

      /* if there was a problem, we skip this driver */
	    if(NULL == driver)
        continue; /*return -1;*/
 
      /* we instantiate the corresponding COM object */
	    if(asio_open_driver(driver)==0)
        if(NULL != driver)         
        {
          /* if we succeeded, we create an asio_audioport_t object, bound to the driver */
          /* and if the object is correct, we add it into the audiomanager list.        */
          asio_audioport_t* port;
	        fts_set_pointer(&at, driver);
	        port = (asio_audioport_t*)fts_object_create(asio_audioport_type, 1, &at);
	        if (NULL != port)
  		    {
	  	      fts_object_refer((fts_object_t*)port);
		        fts_audiomanager_put_port(fts_new_symbol(port->driver->name), (fts_audioport_t*)port);
		        fts_log("[asio] put port : %s\n", port->driver->name);
		        fts_post("[asio] found port : %s\n", port->driver->name);
            count++;
  		    }
          else
          {
            /* TODO : free resources, raise error, and continue with next driver */
          }
        }  
	  }
  }

  /* close the registry key */
  if(regKeyEnum)
  {
    RegCloseKey(regKeyEnum);
  }
  
  fts_post("[asio] found %d port(s)\n", count);
  return count;
}

extern "C" 
{
	void asio_util_scan_drivers_c()
	{
		asio_util_scan_drivers();
	}
}

/** EMACS **
 * Local variables:
 * mode: c++
 * c-basic-offset:2
 * End:
 */

