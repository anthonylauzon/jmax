/*
   Universal Configuration System.

   The FTS Universal Configuration System (fts-ucs) provide
   a set of mechanisms and conventions to configure FTS modules
   and applications on any platform.

   The goal of fts-ucs are:

	1-  hardware and architecture indipendent.
	2- client independent (i.e. client are not required to have
	   complex tools or knowledge about the various configuration
	   options hard coded in the source).
	3- Human readable configuration language: the configuration information
	   must be stored in a human readable format.
	4- Extendible configuration language: the modules should be able to
	   install extension to the configuration language


   A ucs command is a list of atoms; the first atom must be a symbol,
   and is called the command opcode; the second atom may be sometime being
   used as a sub-opcode, and in that case should also be an atom;
   the other can be also floats or int.

   No symbol should contain a space or control char.

   Each command can be represented also as a string, with the textual representation
   of atoms separated by any number of blanks or '\t'.

   A ucs command can be sent coded according to the FTS client protocol,
   as a message to the UCS subsystem; in that case a processor identification
   value (a long) is added at the beginning of the message.

   The client library provide an utility function to translate a string
   represented UCS command to a FTS message.

   A command can be executed by directly calling the proper
   fts-ucs function, or by sending it as a message.

   The semantic of the command itself is module dependent;
   SOme command are implemented in the fts kernel modules.

   Note that a command cannot exists in version with a sub-opcode
   and without.

*/

#include <string.h>

#include <sys/time.h>

#include "sys.h"
#include "lang.h"
#include "runtime.h"
#include "tiles/ucs.h"
#include "tiles/messtile.h"


/******************************************************************************/
/*                                                                            */
/*                ERROR STRUCTURE DECLARATIONS                                */
/*                                                                            */
/******************************************************************************/

fts_status_description_t bad_command =
{
  "malformed configuration command"
};

fts_status_description_t unknown_command =
{
  "unknown configuration command"
};



/******************************************************************************/
/*                                                                            */
/*             FS UCS module definition                                       */
/*                                                                            */
/******************************************************************************/


/* 
   The init function install the client subprotocol 
   handler for the configuration.
*/

static void fts_ucs_client_dispatch(int argc, const fts_atom_t *argv);

static void fts_ucs_install_commands(void );

static void fts_ucs_init(void);

fts_module_t fts_ucs_module = {"UCS", "FTS Universal Configuration System", fts_ucs_init, 0};

static void
fts_ucs_init(void)
{
  fts_client_mess_install(UCS_CODE, fts_ucs_client_dispatch);

  fts_ucs_install_commands();
}

/******************************************************************************/
/*                                                                            */
/*                     UCS functions                                          */
/*                                                                            */
/******************************************************************************/

/* Command table, and Function to define new commands */

struct fts_ucs_cmd_entry
{
  fts_symbol_t opcode;
  fts_symbol_t sub_opcode;

  fts_status_t (* fun)(int argc, const fts_atom_t *argv);

  const char   *usage_string;
  const char   *help_string;

  struct fts_ucs_cmd_entry *next;
};

static struct fts_ucs_cmd_entry *cmd_list = 0;


/* Add a command to the command parser; if the sub_opcode argument is
   null, the command have no sub-opcode 
*/

static void
fts_ucs_define_command(fts_symbol_t opcode, fts_symbol_t sub_opcode, fts_status_t (* fun)(int argc, const fts_atom_t *argv), const char *usage_string, const char *help_string)
{
  struct fts_ucs_cmd_entry *entry;

  entry = (struct  fts_ucs_cmd_entry *) fts_malloc(sizeof(struct fts_ucs_cmd_entry));
  
  entry->opcode = opcode;
  entry->sub_opcode = sub_opcode;
  entry->fun = fun;
  entry->usage_string = usage_string;
  entry->help_string = help_string;

  entry->next = cmd_list;
  cmd_list = entry;
}


/* Execute a configuration command: the subcode, if any, is given as first argument  */

fts_status_t
fts_ucs_execute_command(int argc, const fts_atom_t *argv)
{
  struct fts_ucs_cmd_entry *p;

  p = cmd_list;
      
  while(p)
    {
      if (fts_is_symbol(&argv[0]) && (p->opcode == fts_get_symbol(&argv[0])))
	{
	  if (p->sub_opcode)
	    {
	      if (fts_is_symbol(&argv[1]) && (p->sub_opcode == fts_get_symbol(&argv[1])))
		return (* p->fun)(argc - 2, argv + 2);
	    }
	  else
	    return (* p->fun)(argc - 1, argv + 1);
	}
      
      p = p->next;
    }

  post("Unknown UCS Command: ");
  postatoms(argc, argv);
  post("\n");

  return fts_Success;		/* shadock */
  /* return &unknown_command; */ /* error: unknown command */
}


/* Special version when you have the opcode in a separate symbol */

fts_status_t
fts_ucs_execute_command_opcode(fts_symbol_t opcode, int argc, const fts_atom_t *argv)
{
  struct fts_ucs_cmd_entry *p;

  p = cmd_list;
      
  while(p)
    {
      if (opcode == p->opcode)
	{
	  if (p->sub_opcode)
	    {
	      if (fts_is_symbol(&argv[0]) && (p->sub_opcode == fts_get_symbol(&argv[0])))
		return (* p->fun)(argc - 1, argv + 1);
	    }
	  else
	    return (* p->fun)(argc, argv);
	}
      
      p = p->next;
    }

  return &unknown_command; /* error: unknown command */
}

/* the fts_ucs client message dispatcher */

static void
fts_ucs_client_dispatch(int argc, const fts_atom_t *argv)
{
  if (argc > 1)
    {
      /* ignore the first argument, the old cpno,
       for temporary compatibility with Max 
       */

      fts_status_t ret;

      ret = fts_ucs_execute_command(argc, argv );

      if (ret != fts_Success)
	post("Error: %s\n", ret->description);
    }
  else
    post("Error: %s\n", bad_command.description);		/* Should become raise error !!! */
}



/******************************************************************************/
/*                                                                            */
/*                COMMAND Definitions                                         */
/*                                                                            */
/******************************************************************************/

/* 
 * Help, print and welcome: generic help/support/printout
 *
 */


static fts_status_t
fts_ucs_lib_print(int argc, const fts_atom_t *argv)
{
  postatoms(argc, argv);
  post("\n");

  return fts_Success;
}


static fts_status_t
fts_ucs_lib_help(int argc, const fts_atom_t *argv)
{
  struct fts_ucs_cmd_entry *p;

  p = cmd_list;

  post("\n");
  post("UCS Commands:\n");
  post("\n");

  while(p)
    {
      if (p->usage_string)
	{
	  post("%s\n", p->usage_string);

	  if (p->help_string)
	    post(p->help_string);

	  post("\n");
	}
      
      p = p->next;
    }

  return fts_Success;
}


static void
ucs_post(const char *str)
{
  post(str);
}

static fts_status_t
fts_ucs_lib_show_welcome(int argc, const fts_atom_t *argv)
{
  fts_welcome_apply(ucs_post);

  return fts_Success;
}

/*
 * SYSTEM COMMANDS 
 *
 */

static fts_status_t
fts_ucs_lib_set_path(int argc, const fts_atom_t *argv)
{
  if (fts_is_symbol(argv))
    fts_set_default_search_path(fts_get_symbol(argv));

  return fts_Success;
}

static fts_status_t
fts_ucs_set_updates_per_ticks(int argc, const fts_atom_t *argv)
{
  if ((argc == 1) && fts_is_int(&argv[0]))
    {
      fts_updates_set_updates_per_ticks(fts_get_int(&argv[0]));
    }

  return fts_Success;
}

static fts_status_t
fts_ucs_set_updates_period(int argc, const fts_atom_t *argv)
{
  if ((argc == 1) && fts_is_int(&argv[0]))
    {
      fts_updates_set_update_period(fts_get_int(&argv[0]));
    }

  return fts_Success;
}


static fts_status_t
fts_ucs_load_module(int argc, const fts_atom_t *argv)
{
  if ((argc == 2) && fts_is_symbol(&argv[0]) && fts_is_symbol(&argv[1]))
    {
      fts_status_t ret;

      ret = fts_module_load(fts_symbol_name(fts_get_symbol(&argv[0])),
			    fts_symbol_name(fts_get_symbol(&argv[1])));

      if (ret != fts_Success)
	post("Error: %s\n", ret->description);
    }

  return fts_Success;
}





static fts_status_t
fts_ucs_restart_on_eof(int argc, const fts_atom_t *argv)
{
  if ((argc == 1) && fts_is_symbol(&argv[0]))
    {
      fts_symbol_t v = fts_get_symbol(&argv[0]);

      if (v == fts_new_symbol("on"))
	set_restart_on_eof(1);
      else if (v == fts_new_symbol("off"))
	set_restart_on_eof(0);
    }

  return fts_Success;
}


static fts_status_t
fts_ucs_restart(int argc, const fts_atom_t *argv)
{
  fts_restart();

  return fts_Success;
}

/*
 * Scheduler debug commands
 */

#ifdef DEBUG

static fts_status_t
fts_ucs_sched_describe_order(int argc, const fts_atom_t *argv)
{
  fts_sched_describe_order();

  return fts_Success;
}

static fts_status_t
fts_ucs_sched_describe_declarations(int argc, const fts_atom_t *argv)
{
  fts_sched_describe_declarations();

  return fts_Success;
}

#endif

/*
 * Message system commands 
 *
 */

static fts_status_t
fts_ucs_mess_set_msgmode(int argc, const fts_atom_t *argv)
{
  if ((argc == 1) && fts_is_symbol(&argv[0]))
    {
      fts_symbol_t mode = fts_get_symbol(&argv[0]);

      if (mode == fts_new_symbol("check"))
	fts_mess_set_run_time_check(1);
      else if (mode == fts_new_symbol("fast"))
	fts_mess_set_run_time_check(0);
    }

  return fts_Success;
}


/*
 * DEVICE related commands 
 *
 */

static fts_status_t
fts_ucs_dev_close_device(int argc, const fts_atom_t *argv)
{
  fts_symbol_t logical_dev_name;

  const fts_atom_t *ld_argv;
  int ld_argc;

  if ((argc >= 1) && fts_is_symbol(&argv[0]))
    {
      logical_dev_name = fts_get_symbol(&argv[0]);
      ld_argv = argv + 1;
      ld_argc = argc - 1;
    }
  else
    return &bad_command; /* bad unset device command */


  return fts_close_logical_device(logical_dev_name, ld_argc, ld_argv);
}


static fts_status_t
fts_ucs_dev_open_device(int argc, const fts_atom_t *argv)
{
  if ((argc >= 3) && fts_is_symbol(&argv[0]))
    {
      fts_status_t ret;
      fts_symbol_t logical_dev_name = 0;
      fts_symbol_t class_name = 0;

      const fts_atom_t *ld_argv;
      int ld_argc;

      const fts_atom_t *pd_argv;
      int pd_argc;

      fts_dev_t *dev;

      /* First, parse the command, and found logical and physical device name  and args  */

      logical_dev_name = fts_get_symbol(&argv[0]);
      ld_argv = argv + 1;
      ld_argc = 0;

      for (pd_argv = ld_argv ; ld_argc < argc - 1; ld_argc++, pd_argv++)
	if (fts_is_symbol(pd_argv) && (fts_get_symbol(pd_argv) == fts_new_symbol("as")))
	  {
	    break;
	  }

      if (ld_argc == argc - 1)
        return &bad_command; /* bad set device command */

      pd_argv++;
      class_name = fts_get_symbol(pd_argv);
      pd_argv++;

      pd_argc = argv + argc - pd_argv;

      return fts_open_logical_device(logical_dev_name, ld_argc, ld_argv, class_name, pd_argc, pd_argv);
    }
  else
    return &bad_command; /* bad set device command */
}

/*
 * Audio related commands
 *
 */

static fts_status_t
fts_ucs_audio_set_sampling_rate(int argc, const fts_atom_t *argv)
{
  if ((argc == 1)  && (fts_is_long(&argv[0]) || fts_is_float(&argv[0])))
    {
      float f;

      f = (float) fts_get_number(&argv[0]);

      fts_dsp_set_sampling_rate(f); /* set the dsp compiler sampling rate */
      fts_sched_set_tick_length((MAXVS * 1000) / f); /* and set the scheduler tick */
    }

  return fts_Success;
}


/* ucs function to set the default in and out */

static fts_status_t
fts_ucs_audio_set_default_in(int argc, const fts_atom_t *argv)
{
  if ((argc == 1)  && (fts_is_symbol(&argv[0])))
    { 
      fts_audio_set_default_in(fts_get_symbol(&argv[0]));
    }

  return fts_Success;
}


static fts_status_t
fts_ucs_audio_set_default_out(int argc, const fts_atom_t *argv)
{
  if ((argc == 1)  && (fts_is_symbol(&argv[0])))
    {
      fts_audio_set_default_out(fts_get_symbol(&argv[0]));
    }

  return fts_Success;
}


static fts_status_t
fts_ucs_set_mess_trace(int argc, const fts_atom_t *argv)
{
  if ((argc == 1)  && fts_is_int(&argv[0]))
    {

      fts_set_mess_trace(fts_get_int(&argv[0]));
    }

  return fts_Success;
}

/* Install all of them */

static void
fts_ucs_install_commands()
{
  /* Help and printout */

  fts_ucs_define_command(fts_new_symbol("print"), 0,  fts_ucs_lib_print,
			 "print [<args>]*", "print its arguments");

  fts_ucs_define_command(fts_new_symbol("help"), 0,  fts_ucs_lib_help,
			 "help", "print a description of the available UCS commands");

  fts_ucs_define_command(fts_new_symbol("show"), fts_new_symbol("welcome"), fts_ucs_lib_show_welcome,
			 "show welcome", "print the welcome message");


  /* System commands */

  fts_ucs_define_command(fts_new_symbol("set"), fts_new_symbol("defaultpath"),  fts_ucs_lib_set_path,
			 "set defaultpath path", "system command");

  fts_ucs_define_command(fts_new_symbol("set"), fts_new_symbol("updates_per_ticks"),
			 fts_ucs_set_updates_per_ticks,
			 "set update_per_ticks <int>",
			 "Set how many value updates are sent by FTS in a sigle scheduling tick");

  fts_ucs_define_command(fts_new_symbol("set"), fts_new_symbol("update_period"),
			 fts_ucs_set_updates_period,
			 "set update_period <int>",
			 "Set every how much FTS perform an  update");

  fts_ucs_define_command(fts_new_symbol("load"), fts_new_symbol("module"), fts_ucs_load_module,
			 "load module <name> <filename>",
			 "dynamically load a module");

  fts_ucs_define_command(fts_new_symbol("eof"), fts_new_symbol("restart"), fts_ucs_restart_on_eof,
			 "eof restart [ on | off]",
			 "ask for a restart on eof condition on the client connection");

  fts_ucs_define_command(fts_new_symbol("restart"), fts_new_symbol("now"), fts_ucs_restart,
			 "restart now",
			 "ask for a restart now");

  /* scheduler debug functions  */

#ifdef DEBUG
  fts_ucs_define_command(fts_new_symbol("show"), fts_new_symbol("scheduling"), fts_ucs_sched_describe_order,
			 "show scheduling",
			 "show the computed scheduling loop");

  fts_ucs_define_command(fts_new_symbol("show"), fts_new_symbol("schedinfo"), fts_ucs_sched_describe_declarations,
			 "show schedinfo",
			 "show the scheduler declarations");
#endif

  /* Message system function  */

  fts_ucs_define_command(fts_new_symbol("set"), fts_new_symbol("msgmode"),  fts_ucs_mess_set_msgmode,
			 "set msgmode [check | fast]",
			 "Set the FTS message mode\ncheck mode give you more tests\nduring object developement");

  fts_ucs_define_command(fts_new_symbol("mess"), fts_new_symbol("trace"),  fts_ucs_set_mess_trace,
			 "mess trace [1 | 0]",
			 "Activate the trace of messages received by FTS");

  /* DEVICE related commands  */

  fts_ucs_define_command(fts_new_symbol("open"), fts_new_symbol("device"),  fts_ucs_dev_open_device,
			 "open device <logdev> as <physdev>",
			 "open the <logdev> logical device\nby assigning to it the <physdev> device");

  fts_ucs_define_command(fts_new_symbol("close"), fts_new_symbol("device"), fts_ucs_dev_close_device,
			 "close <logdev>",
			 "close the <logdev> logical device");


  /* Audio related commands */

  fts_ucs_define_command(fts_new_symbol("set"), fts_new_symbol("sample_rate"), fts_ucs_audio_set_sampling_rate,
			 "set sample_rate <sr>",
			 "set sampling rate in a processor");

  fts_ucs_define_command(fts_new_symbol("default"), fts_new_symbol("in~"), fts_ucs_audio_set_default_in,
			 "default in~ <name>",
			 "set the name of the default audio input device");

  fts_ucs_define_command(fts_new_symbol("default"), fts_new_symbol("out~"), fts_ucs_audio_set_default_out,
			 "default out~ <name>",
			 "set the name of the default audio output device");
}





