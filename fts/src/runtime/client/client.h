#ifndef _CLIENT_H_
#define _CLIENT_H_
/*
 *                      Copyright (c) 1993 by IRCAM
 *                          All rights reserved.
 *
 *  For any information regarding this and other IRCAM software, please
 *  send email to:
 *                              manager@ircam.fr
 *
 */


/* The client module */

extern fts_module_t fts_client_module;

/* parameters that might fix the size of some structure */

#define MSIZE 4096		/* maximum size of a host->CP message */
#define MARG 256		/* maximum number of args in a host->CP message */


/* Receive Subsystem installation */

extern fts_dev_t *client_dev;

/* EOF control */

extern void set_restart_on_eof(int v);

#endif
