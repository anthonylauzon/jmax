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
/*
   Support for FTS configuration.

   The Fts Kernel is actually a library; the main program should build
   up a data structure declaring which modules are installed on top
   of the Fts Kernel, and then run the fts_main loop; this will automatically
   initialize the modules.

   A module is declared by a data structure having a number of fields:

   1- An init function, to be called before the compuation start.
   2- A set debug function, called with the required debugging options
      sent by the client.
   3- A poll function, that should be called at every tick, together
      with ordering information about the order to follow in the polling.
      *NOT YET IMPLEMENTED*

   The init function should install all the client and ipc protocols
   the module require, and install MIDI and ADC/DAC devices as required
   (devices not yet implemented).

   Up to MAX_FTS_SUBSYSTEMS can be specified.

   The kernel itself may include a number of module, that are
   automatically installed at init time; they are installed
   in kernel.c functions.

   A system cannot be installed or deinstalled at run time.

   Init and set debug functions are called in the order the system
   are installed, while the poll function is called in an order
   computed based on the ordering info, see below.
*/

#ifndef _MODULES_H_
#define _MODULES_H_

typedef struct _fts_module
{
  const char *name;		/* for documentation purpose */
  const char *doc;		/* for documentation purpose */

  void (* init_f)(void);	/* the init function */
  void (* restart_f)(void);	/* the restart function */
  void (* shutdown_f)(void);	/* the shutdown function */

  struct _fts_module *next;	/* pointer for list handling */
} fts_module_t;



extern void fts_install_module(fts_module_t *s);
extern void fts_modules_init(void);
extern void fts_modules_restart(void);
extern void fts_modules_shutdown(void);

extern fts_status_t  fts_module_load(const char *name, const char *filename);

#endif




