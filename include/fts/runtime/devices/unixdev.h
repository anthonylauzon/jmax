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
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#ifndef _FTS_UNIXDEV_H_
#define _FTS_UNIXDEV_H_

/*#include "sys.h"*/
/*#include "lang.h"*/
/*#include "runtime/devices/devices.h"*/

FTS_API void unixdev_init(void);

/* File descriptor based device */
typedef struct fd_dev_data fd_dev_data_t;

#define DEFAULT_GET_BUFSIZE 2048
#define DEFAULT_PUT_BUFSIZE 2048

FTS_API fd_dev_data_t *fd_data_new( int get_bufsize, int put_bufsize);
FTS_API void fd_data_delete( fd_dev_data_t *d);

FTS_API void fd_data_set_input_fd( fd_dev_data_t *d, int fd);
FTS_API int fd_data_get_input_fd( fd_dev_data_t *d);

FTS_API void fd_data_set_output_fd( fd_dev_data_t *d, int fd);
FTS_API int fd_data_get_output_fd( fd_dev_data_t *d);

/* Buffered non-blocking select based char read */
FTS_API fts_status_t fd_dev_get( fts_dev_t *dev, unsigned char *cp);
/* Output buffering */
FTS_API fts_status_t fd_dev_put( fts_dev_t *dev, unsigned char c);
FTS_API fts_status_t fd_dev_flush( fts_dev_t *dev);

#endif
