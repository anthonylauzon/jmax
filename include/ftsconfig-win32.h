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
 */

#ifndef _CONFIG_WIN32_H
#define _CONFIG_WIN32_H

#ifndef M_PI
#define M_PI  3.1415926535897932384626433832795
#endif

/* defines for included headers */
#undef HAVE_UNISTD_H 
#undef HAVE_ALLOCA_H 
#undef HAVE_SYS_TIME_H
#undef HAVE_SYS_PARAM_H 
#define HAVE_ALLOCA 1
#define HAVE_DIRECT_H 1
#define HAVE_PROCESS_H 1
#define HAVE_IO_H 1
#define HAVE_WINDOWS_H 1
#define HAVE_STRING_H 1

/* defines for language features */
#ifndef restrict
#define restrict
#endif
 
#undef WORDS_BIGENDIAN 

/* mapping posix names to win32 names */
#define getcwd _getcwd
#define isatty _isatty
#define alloca _alloca
#define getpid _getpid
#define snprintf _snprintf
#define alloca _alloca
#define bcopy(src,dst,n)  memcpy(dst,src,n)

/* file handling macros */

#define MAXPATHLEN _MAX_PATH
#define realpath  win32_realpath

/* Valid absolute paths are:  /c/jmax, c:/jmax, c:\jmax */
#define fts_path_is_absolute(_p) \
 ((_p[0] == '/') || \
  ((_p[1] == ':') && (_p[2] == '/')) || \
  ((_p[1] == ':') && (_p[2] == '\\')))

char* win32_realpath(const char* path, char* resolved_path);

#endif /* _CONFIG_WIN32_H */
