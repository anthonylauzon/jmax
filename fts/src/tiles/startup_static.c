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
#include "sys.h"
#include "lang.h"
#include "runtime.h"

#include "tiles/ucs.h"
#include "tiles/messtile.h"
#include "tiles/dsptile.h"

static void fts_kernel_config(void);

static void fts_assign_boot_devices();

extern fts_module_t additive_module;
extern fts_module_t control_module;
extern fts_module_t data_module;
extern fts_module_t eric_module;
extern fts_module_t explode_module;
extern fts_module_t filters_module;
extern fts_module_t guiobj_module;
extern fts_module_t io_module;
extern fts_module_t lists_module;
extern fts_module_t manoury_module;
extern fts_module_t midi_module;
extern fts_module_t pitch_module;
extern fts_module_t qlist_module;
extern fts_module_t sampling_module;
extern fts_module_t signal_module;
extern fts_module_t system_module;

static void fts_static_install_modules( void)
{
  fts_install_module( &additive_module);
  fts_install_module( &control_module);
  fts_install_module( &data_module);
  fts_install_module( &eric_module);
  fts_install_module( &explode_module);
  fts_install_module( &filters_module);
  fts_install_module( &guiobj_module);
  fts_install_module( &io_module);
  fts_install_module( &lists_module);
  fts_install_module( &manoury_module);
  fts_install_module( &midi_module);
  fts_install_module( &pitch_module);
  fts_install_module( &qlist_module);
  fts_install_module( &sampling_module);
  fts_install_module( &signal_module);
  fts_install_module( &system_module);
}

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

  /* statically install the modules */
  fts_static_install_modules();

  /* Initialize all the fts and application modules */

  fts_modules_init();

  /* Load the dso boot modules  */

  fts_load_boot_modules();

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

  /* TILES modules */

  fts_install_module( &fts_ucs_module);      /* the fts universal configuration  system */
  fts_install_module( &fts_dsptile_module);      /* the fts universal configuration  system */
  fts_install_module( &fts_messtile_module);      /* the fts universal configuration  system */
}


static void 
fts_assign_boot_devices(int argc, char **argv)
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
}
