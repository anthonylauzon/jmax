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

/*
 * This file contains Mac OS X platform dependent functions:
 *  - dynamic loader
 *  - FPU settings
 *  - real-time: scheduling mode and priority, memory locking
 * 
 */

#include <fts/fts.h>
#include <ftsconfig.h>

#include "unixfunc.h"

/**********************************/
/* SOCKET STUFF from <sys/socket> */
/**********************************/
int socket(int __domain, int __type, int __protocol)
{
  return -1;
}

int accept(int __fd, const struct sockaddr *addr, int *addr_len)
{
  return -1;
}

int bind(int __fd, const struct sockaddr *addr, int addr_len)
{
  return -1;
}

int listen(int __fd, int __n)
{
  return -1;
}

uint32_t htonl(uint32_t __hostlong)
{
  return -1;
}

uint16_t htons(uint16_t __hostshort)
{
  return -1;
}

ssize_t sendto(int __fd, const void *buf, size_t __n, int __flags, const struct sockaddr *addr, int addr_len)
{
  return -1;
}

/**********************************/
/* FILE         from <stdlib.h>   */
/**********************************/
char *realpath(const char *path, char *resolved_path)
{
  return (char *)path;
}

/***************************************/
/* FILE descriptors  from <unistd.h>   */
/***************************************/

int dup (int __fd)
{
  return -1;
}
int dup2 (int __fd, int __fd2)
{
  return -1;
}

/***************************************/
/* from socketstream.c                 */
/***************************************/
void fts_kernel_socketstream_init(void)
{
}

/***************************************/
/* from sched.c                        */
/***************************************/
void fts_sched_run(void){}
void fts_sched_halt(void){}
void fts_sleep(void){}
int fts_sched_remove( fts_object_t *obj)
{
	return -1;
}
int fts_sched_add( fts_object_t *obj, int flags, ...)
{
	return 0;
}
void fts_kernel_sched_init(void){}

/***************************************/
/* from clientmanager.c                */
/***************************************/
void fts_client_manager_pipe_start(void){}
void fts_client_manager_tcp_start(void){}
void fts_kernel_client_manager_init(void){}

/***************************************/
/* from file.c                         */
/***************************************/
typedef char *(*absolute_path_fun_t)(const char* parent, const char* file, char* buf, int len);
typedef char *(*file_find_fun_t)(const char *filename, char *buf, int len);

static absolute_path_fun_t absolute_path_fun = NULL;
static file_find_fun_t find_file_fun = NULL;

char fts_path_separator = ';';
char fts_file_separator = ':';

void 
fts_set_absolute_path_fun(void *fun)
{
  absolute_path_fun = (absolute_path_fun_t)fun;
}

void 
fts_set_find_file_fun(void *fun)
{
  find_file_fun = (file_find_fun_t)fun;
}

int
fts_file_exists( const char *filename)
{
  return 0;
}

int
fts_is_file(const char *name)
{
  return 0;
}

int 
fts_is_directory(const char *name)
{
  return 0;
}

char *
fts_dirname( char *name)
{
  char *end;

  if ((end = strrchr( name, fts_file_separator)) != NULL)
    *end = '\0';

  return name;
}

char *
fts_make_absolute_path(const char* parent, const char* file, char* buf, int len)
{
  if(absolute_path_fun != NULL)
    return absolute_path_fun(parent, file, buf, len);

  return NULL;
}

int
fts_file_find_in_path(const char* root, fts_list_t* paths, const char* filename, char* buf, int len)
{
  return 0;
}

int
fts_file_find_in_env( const char *root, fts_symbol_t env, const char *filename, char *buf, int len)
{
  return 0;
}

char *
fts_file_find(const char *filename, char *buf, int len)
{
  if(find_file_fun != NULL)
    return find_file_fun(filename, buf, len);
    
	return NULL;
}
