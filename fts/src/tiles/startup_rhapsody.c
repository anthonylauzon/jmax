/* 
   This file contain the initial configuration of  the FTS modules and  device assignement,
   and the main; FTS don't support "foregneir" main anymore.

   Should be modularized by layer (there should be a sys_init, runtime_init 
   and lang_init).

*/

/* 
   NOTE for Rhapsody:
   This file is derived from startup.c, and includes
   static installation of fts1.5 modules. Static because
   Rhapsody does not support dynamic linking !
*/


#include <string.h>
#include <stdio.h>

#include "sys.h"
#include "lang.h"
#include "runtime.h"

#include "tiles/ucs.h"
#include "tiles/messtile.h"
#include "tiles/dsptile.h"


static void fts_kernel_config(void);

static void fts_assign_boot_devices();



/* fts_welcome_t: hw or module dependent welcome should be moved in the proper
   module init function */

static fts_welcome_t version_welcome = {FTS_RELEASE_STRING};
static fts_welcome_t author_welcome = {"by Francois Dechelle, Maurizio De Cecco and Norbert Schnell\n"};
static fts_welcome_t author_email_welcome = {"  dechelle@ircam.fr   dececco@ircam.fr  schnell@ircam.fr\n"};
static fts_welcome_t miller_welcome = {"A new implementation of Miller Puckette's FTS\n"};

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
	fprintf(stderr, "Usage: fts [-help] [-s] <client dev description>\n");
      else if (! strcmp(*argv, "-s"))
	set_restart_on_eof(1);

      /* others ???  */

      argc--;
      argv++;
    }

  /* Install the basic welcome messages */

  fts_add_welcome(&version_welcome);
  fts_add_welcome(&author_welcome);
  fts_add_welcome(&author_email_welcome);
  fts_add_welcome(&miller_welcome);

#ifdef DEBUG
  fts_add_welcome(&debug_welcome);
#endif

  /* sys level initialization */

  fts_sys_init();

  /* kernel modules configure */

  fts_kernel_config();

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

extern fts_module_t additive_module;
extern fts_module_t control_module;
extern fts_module_t data_module;
extern fts_module_t events_module;
extern fts_module_t filters_module;
extern fts_module_t io_module;
extern fts_module_t lists_module;
extern fts_module_t math_module;
extern fts_module_t midi_module;
extern fts_module_t sampling_module;
extern fts_module_t scorefollow_module;
extern fts_module_t sigbasic_module;
extern fts_module_t sigmath_module;
extern fts_module_t system_module;
extern fts_module_t ui_module;

static void
fts_install_1_5_classes( void)
{
  fts_install_module( &additive_module);
  fts_install_module( &control_module);
  fts_install_module( &data_module);
  fts_install_module( &events_module);
  fts_install_module( &filters_module);
  fts_install_module( &io_module);
  fts_install_module( &lists_module);
  fts_install_module( &math_module);
  fts_install_module( &midi_module);
  fts_install_module( &sampling_module);
  fts_install_module( &scorefollow_module);
  fts_install_module( &sigbasic_module);
  fts_install_module( &sigmath_module);
  fts_install_module( &system_module);
  fts_install_module( &ui_module);
}



static void
fts_kernel_config(void)
{
  /* LANG modules */

  fts_install_module(&fts_mess_module);   /* Install the MAX interpreter module */

  fts_install_module(&fts_utils_module);      /* Install the FTS utils */

  fts_install_module(&fts_veclib_module);  /* Install the FTS vector function*/

  fts_install_module(&fts_dsp_module);       /* Install the DSP engine */

  /* RUNTIME modules */

  fts_install_module(&fts_sched_module);     /* the fts scheduler module */
  fts_install_module(&fts_dev_module);      /* the device support  system */

  fts_install_module(&fts_midi_module);     /* the fts MIDI system */
  fts_install_module(&fts_time_module);    /* the time handling module */
  fts_install_module(&fts_audio_module);    /* the fts AUDIO I/O  system */
  fts_install_module(&fts_client_module);   /* the fts <--> client communication system */

  /* TILES modules */

  fts_install_module(&fts_ucs_module);      /* the fts universal configuration  system */
  fts_install_module(&fts_dsptile_module);      /* the fts universal configuration  system */
  fts_install_module(&fts_messtile_module);      /* the fts universal configuration  system */

  /* FTS1.5 classes modules */
  fts_install_1_5_classes();
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






