/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#ifndef _UNIXDEV_H_
#define _UNIXDEV_H_

#include "sys.h"
#include "lang.h"
#include "runtime/devices/devices.h"

extern void unixdev_init(void);

/* File descriptor based device */
typedef struct fd_dev_data fd_dev_data_t;

#define DEFAULT_GET_BUFSIZE 2048
#define DEFAULT_PUT_BUFSIZE 2048

extern fd_dev_data_t *fd_data_new( int get_bufsize, int put_bufsize);
extern void fd_data_delete( fd_dev_data_t *d);

extern void fd_data_set_input_fd( fd_dev_data_t *d, int fd);
extern int fd_data_get_input_fd( fd_dev_data_t *d);

extern void fd_data_set_output_fd( fd_dev_data_t *d, int fd);
extern int fd_data_get_output_fd( fd_dev_data_t *d);

/* Buffered non-blocking select based char read */
extern fts_status_t fd_dev_get( fts_dev_t *dev, unsigned char *cp);
/* Output buffering */
extern fts_status_t fd_dev_put( fts_dev_t *dev, unsigned char c);
extern fts_status_t fd_dev_flush( fts_dev_t *dev);

#endif
