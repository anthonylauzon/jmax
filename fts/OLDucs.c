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
 */

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
#if HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#include <fts/fts.h>
#include <fts/private/OLDclient.h>
#include <fts/private/patparser.h>
#include <fts/private/platform.h>

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
  post_atoms(argc, argv);
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
extern void trace_mess(const char *msg, int ac, const fts_atom_t *av);

static void
fts_ucs_client_dispatch(int argc, const fts_atom_t *argv)
{
  if (argc > 1)
    {
      /*
	ignore the first argument, the old cpno,
	for temporary compatibility with Max 
      */

      fts_status_t ret;

      trace_mess("ucs message", argc, argv);

      ret = fts_ucs_execute_command(argc, argv);

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
  post_atoms(argc, argv);
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

/*
 * SYSTEM COMMANDS 
 *
 */

static fts_status_t
fts_ucs_lib_set_path(int argc, const fts_atom_t *argv)
{
  if (fts_is_symbol(argv))
    fts_set_search_path(fts_get_symbol(argv));

  return fts_Success;
}

static fts_status_t
fts_ucs_lib_set_project_dir(int argc, const fts_atom_t *argv)
{
  if ((argc >= 1) && fts_is_symbol(argv))
    fts_set_project_dir(fts_get_symbol(argv));

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

#define MODULE_INIT_FUNCTION_SUFFIX "_config"

static fts_status_t fts_ucs_load_module(int argc, const fts_atom_t *argv)
{
  if ((argc == 2) && fts_is_symbol(&argv[0]) && fts_is_symbol(&argv[1]))
    {
      fts_status_t ret;
      const char *module_name = fts_symbol_name(fts_get_symbol(argv + 0));
      const char *file = fts_symbol_name(fts_get_symbol(argv + 1));
      char *name = (char *)fts_malloc( strlen( module_name) + sizeof( MODULE_INIT_FUNCTION_SUFFIX) + 1);

      strcpy( name, module_name);
      strcat( name, MODULE_INIT_FUNCTION_SUFFIX);

      ret = fts_load_library( file, module_name);

      if (ret != fts_Success)
	post("Error loading module %s: %s (%s)\n", module_name, ret->description, file);

      fts_free( name);
    }

  return fts_Success;
}

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


extern void fts_set_mess_trace(int b);

static fts_status_t
fts_ucs_set_mess_trace(int argc, const fts_atom_t *argv)
{
  if ((argc == 1)  && fts_is_int(&argv[0]))
    {
      fts_set_mess_trace(fts_get_int(&argv[0]));
    }

  return fts_Success;
}

/* ucs function to set the default audio and midi ports */

static fts_status_t fts_ucs_newobj(int argc, const fts_atom_t *argv)
{
  fts_object_t *obj;

  obj = fts_eval_object_description( fts_get_root_patcher(), argc, argv);

  return fts_Success;
}

/*  static fts_status_t fts_ucs_default_audio(int argc, const fts_atom_t *argv) */
/*  { */
/*    if ((argc >= 1)  && (fts_is_symbol(&argv[0]))) */
/*      { */
/*        post( "Setting default audio port to \"%s\"", fts_symbol_name( fts_get_symbol( argv))); */
/*        if (argc >= 2) */
/*  	{ */
/*  	  post( "arguments: "); */
/*  	  post_atoms( argc-1, argv+1); */
/*  	  post( ")"); */
/*  	} */
/*        post( "\n"); */

/*        fts_audioport_set_default( argc, argv); */
/*      } */

/*    return fts_Success; */
/*  } */

static fts_status_t fts_ucs_default_midi(int argc, const fts_atom_t *argv)
{
  if ((argc >= 1)  && (fts_is_symbol(&argv[0])))
    { 
      post( "Setting default MIDI port to \"%s\"", fts_symbol_name( fts_get_symbol( argv)));
      if (argc >= 2)
	{
	  post( "arguments: ");
	  post_atoms( argc-1, argv+1);
	  post( ")");
	}
      post( "\n");

      fts_midiport_set_default( argc, argv);
    }

  return fts_Success;
}


/*
 * Generic Set Parameters command
 * (Should contribute to make the others obsoletes
 */

static fts_status_t
fts_ucs_set_param(int argc, const fts_atom_t *argv)
{
  if ((argc == 2)  && fts_is_symbol(&argv[0]))
    fts_param_set(fts_get_symbol(&argv[0]), &argv[1]);

  return fts_Success;
}

/*
 * Command to set the index/font size translation table for .pat files 
 */


static fts_status_t fts_ucs_set_fonts(int argc, const fts_atom_t *argv)
{
  fts_patparse_set_font_size_table(argc, argv);

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

  /* System commands */

  fts_ucs_define_command(fts_new_symbol("set"), fts_new_symbol("defaultpath"), fts_ucs_lib_set_path,
			 "set defaultpath <path>", "system command");

  fts_ucs_define_command(fts_new_symbol("set"), fts_new_symbol("projectdir"), fts_ucs_lib_set_project_dir,
			 "set projectdir <path>", "system command");

  fts_ucs_define_command(fts_new_symbol("set"), fts_new_symbol("updates_per_ticks"),
			 fts_ucs_set_updates_per_ticks,
			 "set update_per_ticks <int>",
			 "Set how many value updates are sent by FTS in a single scheduling tick");

  fts_ucs_define_command(fts_new_symbol("set"), fts_new_symbol("update_period"),
			 fts_ucs_set_updates_period,
			 "set update_period <int>",
			 "Set every how much FTS perform an  update");

  fts_ucs_define_command(fts_new_symbol("load"), fts_new_symbol("module"), fts_ucs_load_module,
			 "load module <name> <filename>",
			 "dynamically load a module");

  /* Message system function  */

  fts_ucs_define_command(fts_new_symbol("set"), fts_new_symbol("msgmode"),  fts_ucs_mess_set_msgmode,
			 "set msgmode [check | fast]",
			 "Set the FTS message mode\ncheck mode give you more tests\nduring object developement");

  fts_ucs_define_command(fts_new_symbol("mess"), fts_new_symbol("trace"),  fts_ucs_set_mess_trace,
			 "mess trace [1 | 0]",
			 "Activate the trace of messages received by FTS");

  /* Audio and MIDI related commands  */

  fts_ucs_define_command( fts_new_symbol("newobj"), 0, fts_ucs_newobj,
			 "newobj [<args>]*",
			 "creates an object");


/*    fts_ucs_define_command(fts_new_symbol("default"), fts_new_symbol("audio"), fts_ucs_default_audio, */
/*  			 "default audio <name> [<args>]*", */
/*  			 "defines the default audio port"); */

  fts_ucs_define_command(fts_new_symbol("default"), fts_new_symbol("midi"), fts_ucs_default_midi,
			 "default midi <name> [<args>]*",
			 "defines the default midi port");

  /* Parameters */

  fts_ucs_define_command(fts_new_symbol("set"), fts_new_symbol("param"), fts_ucs_set_param,
			 "set param <name> <value>",
			 "set an FTS parameter");

  /* Fonts */

  fts_ucs_define_command(fts_new_symbol("set"), fts_new_symbol("fonts"), fts_ucs_set_fonts,
			 "set fonts <int> <int> <int> <int> <int> <int> <int> <int>",
			 "set an FTS parameter");
}


/***********************************************************************
 *
 * UCS kernel initialization
 *
 */

static void fts_kernel_ucs_init(void)
{
  fts_client_install(UCS_CODE, fts_ucs_client_dispatch);
  fts_ucs_install_commands();
}
