/* config.h */
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

#ifndef _FTS_LOW_LEVEL_CONFIG_SYSTEM_HEADER_
#define _FTS_LOW_LEVEL_CONFIG_SYSTEM_HEADER_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup FtsLowLevelConfigSystem FTS low-level configuration system
 */

/** @file config.h
 *  FTS low-level config system (interface header).
 *  This module is common to FTS and FTSclient. 
 *  Its purpose is to allow to configure an application w.r.t.
 *  the current user, and the current machine. It retrieves
 *  OS's standard directories where it is suitable to put
 *  configuration files.
 *  
 *  There is some additional documentation about it in MusiqueLab's
 *  developement wiki's pages.  Right now, the wiki is accessible 
 *  from IRCAM only. For now, please ask Rodolphe Saugier if you need 
 *  to know more about it.
 *  @ingroup FtsLowLevelConfigSystem
 */

/** @name Initialization 
 */
/*@{*/

/**
 *  Initializes this config module. Should be explicitely called before calling
 *  any other function in this module.
 *  @param vendorName Application's vendor/developper name
 *  @param appName Application name
 *  @param appVersion Application's version, as a string
 *  @param pLogFunc If not NULL, specifies a log function to be used by 
 *                   this module.
 */
void fts_config_init(const char *vendorName, const char *appName,
                     const char *appVersion,
                     void (*pLogFunc)(char *msg, ...)
                    );

/**
 *  Log informations (directories...) retrieved by this module.
 *  Should be called after having fts_config_init() with a non-NULL
 *  pLogFunc parameter.
 */
void fts_config_log_info();

/*@}*/

 
/** @name Config parameters access functions
 */
/*@{*/

/**
 *  Get an integer parameter from the config file(s).
 *  @param name The parameter key.
 *  @param pValue A pointer to an int that will receive the parameter value.
 *  @return Returns value is
 *          - >= 0 if the parameter could be read.
 *          - <0 otherwise. 
 */ 
int fts_config_get_int(const char *name, int *pValue);

/**
 *  Get a float parameter from the config file(s).
 *  @param name The parameter key.
 *  @param pValue A pointer to a float that will receive the parameter value.
 *  @return Returns value is
 *          - >= 0 if the parameter could be read.
 *          - <0 otherwise. 
 */ 
int fts_config_get_float(const char *name, float *pValue);

/**
 *  Get a string parameter from the config file(s).
 *  @param name The parameter key.
 *  @param pValue A pointer to a string buffer that will receive the parameter value.
 *  @param maxLen The maximum length of the string in pValue.
 *  @return Returns value is
 *          - >= 0 if the parameter could be read.
 *          - <0 otherwise. 
 */ 
int fts_config_get_string(const char *name, char *pValue, int maxLen);

/**
 *  Get an integer parameter from the config file(s). If it doesnt exist,
 *   return a default value.
 *  @param name The parameter key.
 *  @param dflt The default value.
 */ 
int fts_config_get_i(const char *name, int dflt);

/**
 *  Get a float parameter from the config file(s). If it doesnt exist,
 *   return a default value.
 *  @param name The parameter key.
 *  @param dflt The default value.
 */ 
float fts_config_get_f(const char *name, float dflt);

/**
 *  Get a string parameter from the config file(s). If it doesnt exist, 
 *   return a default value.
 *  @param name The parameter key.
 *  @param dflt The default value.
 */ 
char *fts_config_get_s(const char *name, const char *dflt);

/*@}*/


/** @name Config directories functions
 */
/*@{*/

/** Get a directory suitable for writing log files.
 */
const char *fts_config_get_log_dir();
    
/** Get a directory suitable to place configuration files
 *  which depend both on the local machine AND the current user.
 */
const char *fts_config_get_user_local_config_dir();

/** Get a directory suitable to place configuration files
 *  which depend on the current user.
 */
const char *fts_config_get_user_config_dir();

/** Get a directory suitable to place configuration files
 *  which contents depend on the local machine.
 */
const char *fts_config_get_local_config_dir();

/** Get a directory suitable to save/load files modified
 *  or created by the user. This may fit as a good initial 
 *  directory for open/save dialog boxes.
 */
const char *fts_config_get_user_files_dir();

/*@}*/


/** @name Application information functions
 */
/*@{*/
     
/** Get the application vendor name.
 */
const char *fts_config_get_vendor_name();

/** Get the application name.
 */
const char *fts_config_get_app_name();

/** Get the application version.
 */
const char *fts_config_get_app_version();

/*@}*/


/** @name Utilitary functions
 */
/*@{*/

/**
 *  Network share type enumeration
 */
enum fts_net_path_type_enum {
    fts_net_path_type_local = 0,   /** Not a net share path */
    fts_net_path_type_unc = 1,           /** UNC net share path */
    fts_net_path_type_mounted_drive = 2  /** Mounted network drive */
};

/**
 *  Detect if a path is a network path
 *  @warning WIN32 only.
 *  @param path The path to test.
 *  @param pHostName If not NULL, it is filled with the host name
 *  @param pShareName If not NULL, it is filled with the share name
 *  @param pRoot If not NULL, it is filled with the root path to access the share 
 *               (for example "\\server\share\" or "J:\")
 *  @param maxLen The maximum length of pHostName, pShareName and pRoot 
 *                (we assume it's the same for the three to keep the interface simple).
 *  @return The return value is:
 *          - fts_net_path_type_unc if path is a UNC network path matching "\\<server>\<share>"...
 *          - fts_net_path_type_mounted_drive if path is a network path matching "<drive>:\..." with "<drive>" being a network drive
 *          - fts_net_path_type_local if path is NOT a network path
 */
enum fts_net_path_type_enum
fts_detect_network_path(const char *path,
                        char *pHostName, char *pShareName,
                        char *pRoot, int maxLen
                       );

/**
 *  Return 1 if path is a network path, 0 otherwise.
 *  @warning WIN32 only.
 *  @param The path to test.
 *  @see fts_detect_network_path
 */
int fts_is_network_path(const char *path);

/*@}*/

#ifdef __cplusplus
}
#endif

#endif /* _FTS_LOW_LEVEL_CONFIG_SYSTEM_HEADER_ */

