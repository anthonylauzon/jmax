/*
 * jMax
 * Copyright (C) 1999 by IRCAM
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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

/* 
   This file contain the initial configuration of  the FTS modules and  device assignement,
   and the main; FTS don't support "foregneir" main anymore.

   Should be modularized by layer (there should be a sys_init, runtime_init 
   and lang_init).

*/

#include <string.h>

#include "sys.h"
#include "lang.h"
#include "runtime.h"

#include "tiles/ucs.h"
#include "tiles/messtile.h"
#include "tiles/dsptile.h"


static void fts_kernel_config(void);
static void fts_assign_boot_devices(int argc, char **argv);


/* fts_welcome_t: hw or module dependent welcome should be moved in the proper
   module init function */

static fts_welcome_t version_welcome = {FTS_VERSION_STRING};
static fts_welcome_t compilation_info_welcome = {COMPILATION_INFO_STRING};

#ifdef DEBUG
static fts_welcome_t debug_welcome = {"Compiled for DEBUG\n"};
#endif



int
main(int argc, char **argv)
{
  /* Argument parsing
     To be done
     */

  argv++;			/* skip the command name */
  argc--;

  while (argc && **argv == '-')
    {
      if (! strcmp(*argv, "-help"))
	fprintf(stderr, "Usage: fts [-help] [-s] -norealtime <client dev description>\n");
      else if (! strcmp(*argv, "-s"))
	set_restart_on_eof(1);
      else if (! strcmp( *argv, "-norealtime"))
	fts_set_no_real_time();

      /* others ???  */

      argc--;
      argv++;
    }

  /* Install the basic welcome messages */

  fts_add_welcome(&version_welcome);
  fts_add_welcome(&compilation_info_welcome);

#ifdef DEBUG
  fts_add_welcome(&debug_welcome);
#endif

  /* sys level initialization */

  fts_sys_init();

  /* kernel modules configure */

  fts_kernel_config();

  /* Initialize all the fts and application modules */

  fts_modules_init();

  /* Install the boot device, deciding on the base of the command
     line arguments.
   */

  fts_assign_boot_devices(argc, argv);

  /* After module initialization, compile the scheduler list */

  fts_sched_compile(); /* compile the scheduler function list  */

  /* Run the scheduler */

  fts_sched_run();

  /* When and if the scheduler exit, run the shutdown functions and return */

  fts_modules_shutdown();

  return 0;
}


static void
fts_kernel_config(void)
{
  /* LANG modules */

  fts_install_module( &fts_mess_module);      /* Install the MAX interpreter module */
  fts_install_module( &fts_utils_module);     /* Install the FTS utils */
  fts_install_module( &fts_datalib_module);   /* Install the FTS data lib */
  fts_install_module( &fts_veclib_module);    /* Install the FTS vector function*/
  fts_install_module( &fts_ftl_module);       /* Install the FTL engine ***MUST BE BEFORE DSP*** */
  fts_install_module( &fts_dsp_module);       /* Install the DSP engine */

  /* RUNTIME modules */

  fts_install_module( &fts_sched_module);    /* the fts scheduler module */
  fts_install_module( &fts_dev_module);      /* the device support  system */
  fts_install_module( &fts_midi_module);     /* the fts MIDI system */
  fts_install_module( &fts_time_module);     /* the time handling module */
  fts_install_module( &fts_audio_module);    /* the fts AUDIO I/O  system */
  fts_install_module( &fts_client_module);   /* the fts <--> client communication system */
  fts_install_module( &fts_files_module);   /* the fts file handling module */

  /* Thread module
     This is temporary: the thread module declares a scheduled function
     that polls the file descriptors set of the thread.
     Evolution: this function will become the only function called by the thread
     and the current scheduler will disappear
  */
  fts_install_module( &fts_thread_module);   /* the "thread" module */

  /* TILES modules */

  fts_install_module( &fts_ucs_module);      /* the fts universal configuration  system */
  fts_install_module( &fts_dsptile_module);      /* the fts universal configuration  system */
  fts_install_module( &fts_messtile_module);      /* the fts universal configuration  system */
}


static void  fts_assign_boot_devices(int argc, char **argv)
{
  fts_atom_t a;
  fts_symbol_t class_name;
  int pd_argc;
  fts_atom_t pd_argv[32];	/* max 32 args */

  if (argc > 32)
    argc = 32;

  if (argc >= 2)
    {
      fts_atom_parse(argv[0], &a);
      
      if (! fts_is_symbol(&a))
	{
	  class_name = fts_new_symbol("stdio");
	  pd_argc = 0;
	}
      else
	{
	  int i;

	  class_name = fts_get_symbol(&a);

	  for (i = 1; i < argc; i++)
	    fts_atom_parse(argv[i], &pd_argv[i - 1]);

	  pd_argc = argc - 1;
	}
    }
  else
    {
      class_name = fts_new_symbol("stdio");
      pd_argc = 0;
    }

  /* Set the client dev from the command line arguments.
     */

  fts_open_logical_device(fts_new_symbol("client"), 0, 0,
			  class_name, pd_argc, pd_argv);

  /* Open the null~ device as audio out; this
     will prevent fts taking 100% CPU time by running
     unsyncronized */

  {
    fts_atom_t ld_argv[1];

    fts_set_symbol(&ld_argv[0], fts_new_symbol("foo"));

    fts_open_logical_device(fts_new_symbol("out~"), 1, ld_argv,
			    fts_new_symbol("null~"), 0, 0);

    /* Declare this audio device as pending close, so that it will be automagically
       closed when other audio device are opened.
     */

    fts_audio_set_pending_close();
  }
}






