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

#ifndef _FTS_MODULES_H_
#define _FTS_MODULES_H_

#include <fts/sys/status.h>

typedef struct _fts_module
{
  const char *name;		/* for documentation purpose */
  const char *doc;		/* for documentation purpose */

  void (* init_f)(void);	/* the init function */
  void (* shutdown_f)(void);	/* the shutdown function */

  struct _fts_module *next;	/* pointer for list handling */
} fts_module_t;

FTS_API void fts_install_module(fts_module_t *s);
FTS_API void fts_modules_init(void);
FTS_API void fts_modules_shutdown(void);

FTS_API fts_status_t fts_module_load(const char *name, const char *filename);

#endif
