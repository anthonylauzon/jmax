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

#ifndef _FTS_PLATFORM_H_
#define _FTS_PLATFORM_H_

#ifdef WIN32

// The following is to have DWORD defined only.
#include <wtypes.h>

/** Registry access functions. FTS uses the registry to set and get
    parameters until the work on the FTS configuration is
    completed. */
FTS_API int fts_get_regvalue_string(char* name, char* buf, int len);
FTS_API int fts_set_regvalue_string(char* name, const char* value);
FTS_API int fts_get_regvalue_int(char* name, int* value);
FTS_API int fts_set_regvalue_int(char* name, int value);

/**
 *  Get strings describing a Win32 process priority class.
 *  @param code The win32 process priority class code (for example: PRIORITY_CLASS_NORMAL).
 *  @param ppDesc A pointer to a pointer to a constant string. If not NULL, the pointed pointer will be modified
 *         to point to a description string. For example: "Normal" for NORMAL_PRIORITY_CLASS.
 *  @param ppID A pointer to a pointer to a constant string. If not NULL, the pointed pointer will be modified
 *         to point to an ID string. For example: "NORMAL_PRIORITY_CLASS".
 *  @return >=0 if ok, <0 if failure.
 */
FTS_API int fts_get_win32_priority_class_desc(DWORD code, const char **ppDesc,  const char **ppID);

/**
 *  Get a win32 priority class code by its description. 
 *  @param desc The description. For example "Normal".
 *  @param pCode Pointer to a variable that will receive the code.
 *  @return >=0 if ok, <0 if failure.
 */
FTS_API DWORD fts_get_win32_priority_class_by_desc(const char *desc, DWORD *pCode);

/**
 *  Get a win32 priority class code by its ID.
 *  @param desc The ID. For example "NORMAL_PRIORITY_CLASS".
 *  @param pCode Pointer to a variable that will receive the code.
 *  @return >=0 if ok, <0 if failure. 
 */
FTS_API DWORD fts_get_win32_priority_class_by_id(const char *id, int *pCode);

/**
 *  Get strings describing a Win32 thread priority.
 *  @param code The win32 thread priority class code (for example: THREAD_PRIORITY_NORMAL).
 *  @param ppDesc A pointer to a pointer to a constant string. If not NULL, the pointed pointer will be modified
 *         to point to a description string. For example: "Normal" for THREAD_PRIORITY_NORMAL.
 *  @param ppID A pointer to a pointer to a constant string. If not NULL, the pointed pointer will be modified
 *         to point to an ID string. For example: "THREAD_PRIORITY_NORMAL".
 *  @return >=0 if ok, <0 if failure.
 */
FTS_API int fts_get_win32_thread_priority_desc(int code, const char **ppDesc,  const char **ppID);

/**
 *  Get a win32 thread priority code by its description. 
 *  @param desc The description. For example "Normal" or "normal" (comparison is case insensitive)
 *  @param pCode Pointer to a variable that will receive the code.
 *  @return >=0 if ok, <0 if failure.
 */
FTS_API int fts_get_win32_thread_priority_by_desc(const char *desc, DWORD *pCode);

/**
 *  Get a win32 thread priority code by its ID. 
 *  @param desc The description. For example "THREAD_PRIORITY_NORMAL" (comparison is case insensitive)
 *  @param pCode Pointer to a variable that will receive the code.
 *  @return >=0 if ok, <0 if failure.
 */
FTS_API int fts_get_win32_thread_priority_by_id(const char *id, int *pCode);

/**
 *  Set the current (active) win32 thread's priority. Log the operation with fts_log.
 *  @return >=0 if ok, <0 if failure.
 */
FTS_API int fts_set_win32_thread_priority(int threadPriority);

/**
 *  Get the current (active) win32 thread's priority.
 */
FTS_API int fts_get_win32_thread_priority();

/**
 *  Set the current (active) win32 process's priority class. Log the operation with fts_log.
 *  @return >=0 if ok, <0 if failure. 
 */
FTS_API int fts_set_win32_priority_class(DWORD priorityClass);

/**
 *  Get the current (active) win32 process's priority class.
 */
FTS_API DWORD fts_get_win32_priority_class();

#endif

#endif
