/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#ifndef _FILES_H_
#define _FILES_H_

extern void fts_file_get_write_path(const char *path, char *full_path);
extern int fts_file_get_read_path(const char *name, char *pathname);

extern int fts_file_open(const char *name, const char *mode);
extern int fts_file_close(int fd);

#endif
