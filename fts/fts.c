/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <fts/fts.h>

#include <ftsprivate/platform.h>
#include <ftsprivate/package.h>
#include <ftsprivate/bmaxfile.h>
#include <ftsprivate/audioconfig.h> /* require bmaxfile.h */
#include <ftsprivate/midi.h> /* require bmaxfile.h */
#include <ftsprivate/config.h> /* require audioconfig.h and midi.h */
#include "ftsprivate/clientmanager.h"

/***********************************************************************
 * 
 * Command line arguments
 * 
 */

static fts_hashtable_t cmd_args;

static void fts_cmd_args_put( fts_symbol_t name, fts_symbol_t value)
{
  fts_atom_t k, v;

  fts_set_symbol( &k, name);
  fts_set_symbol( &v, value);
  fts_hashtable_put( &cmd_args, &k, &v);
}

fts_symbol_t fts_cmd_args_get( fts_symbol_t name)
{
  fts_atom_t k, v;

  fts_set_symbol( &k, name);
  if (fts_hashtable_get( &cmd_args, &k, &v))
    return fts_get_symbol( &v);
  
  return 0;
}

static void fts_cmd_args_parse( int argc, char **argv)
{
  int filecount = 1;
  char filevar[32];
  fts_symbol_t name, value, s_yes;

  {
    int i;

    fts_log("[fts]: Command line arguments:\n");
    for ( i = 0; i < argc; i++)
      fts_log( "[fts]: argument %d = \"%s\"\n", i, argv[i]);
  }

  fts_hashtable_init( &cmd_args, FTS_HASHTABLE_SMALL);

  s_yes = fts_new_symbol( "yes");

  argc--;
  argv++;
  while (argc)
    {
      if (!strncmp( *argv, "--", 2))
	{
	  char *p = strchr( *argv, '=');

	  if (p != NULL)
	    *p = '\0';

	  name = fts_new_symbol( *argv + 2);

	  if (p == NULL || p[1] == '\0')
	    value = s_yes;
	  else
	    {
	      p++;
	      value = fts_new_symbol( p);
	    }
	}
      else
	{
	  sprintf( filevar, "file%d", filecount++);
	  name = fts_new_symbol( filevar);
	  value = fts_new_symbol( *argv);
	}

      fts_cmd_args_put( name, value);

      argc--;
      argv++;
    }
}

fts_symbol_t fts_get_root_directory( void)
{
  fts_symbol_t r;

  /* get it from command line */
  r = fts_cmd_args_get( fts_new_symbol( "root"));
  
  if ( r)
    return r;

  /* if not passed on command line, use the platform specific default value */
  return fts_get_default_root_directory();
}

fts_symbol_t get_user_directory(void);
fts_symbol_t fts_get_user_directory(void)
{
  return get_user_directory();
}


/***********************************************************************
 *
 * Project and configuration loading
 *
 */

static void 
fts_load_project( void)
{
  fts_symbol_t project_file;
  fts_package_t *project = NULL;
  int project_found = 0;

  /* check if the user specified a project file on the command line  */
  project_file = fts_cmd_args_get( fts_s_project);
  if (fts_s_none != project_file)
    {
      if (project_file != NULL)
	{
	  project_found = fts_file_exists( project_file) && fts_is_file( project_file);
	}
    
      /* check if the user has a project file in the home directory  */
      if (!project_found)
	{
	  project_file = fts_get_user_project();
	  project_found = fts_file_exists( project_file) && fts_is_file( project_file);
	}
    
      /* check if there's a system wide project */
      if (!project_found)
	{
	  project_file = fts_get_system_project();
	  project_found = fts_file_exists( project_file) && fts_is_file( project_file);
	}

      if (project_found)
	project = fts_project_open(project_file);
    }

  /* create an empty project */
  if (project == NULL)
    {
      /* create an empty project */
      project = fts_package_new( fts_s_project);

      fts_log("[boot]: Starting fts with an empty project. This is probably not what you want. Make sure you have a valid project file.\n");
      fts_post( "Warning: no project found\n");
    }
  else
    {
      fts_log( "[boot] opened project: %s\n", project_file);
    }

  fts_project_set( project);
}


static void 
fts_load_config( void)
{
  fts_symbol_t config_file;
  fts_config_t *config = NULL;
  int config_found = 0;

  /* check if the user specified a config file on the command line  */
  config_file = fts_cmd_args_get( fts_s_config);
  if (fts_s_none != config_file)
    {
      if (config_file != NULL)
	{
	  config_found = fts_file_exists( config_file) && fts_is_file( config_file);
	}
    
      /* check if the user has a config file in the home directory  */
      if (!config_found)
	{
	  config_file = fts_get_user_configuration();
	  config_found = fts_file_exists( config_file) && fts_is_file( config_file);
	}

      /* check if there's a system wide config */
      if (!config_found)
	{
	  config_file = fts_get_system_configuration();
	  config_found = fts_file_exists( config_file) && fts_is_file( config_file);
	}
      if (config_found)
	config = fts_config_open( config_file);
    }    
  
  if (config == NULL)
    {
      /* create an empty config */
      config = (fts_config_t*)fts_object_create(fts_config_class, 0, 0);
      fts_midiconfig_set_defaults(config->midi_config);
      fts_audioconfig_set_defaults(config->audio_config);

      fts_log("[boot]: Starting fts with an empty AUDIO/MIDI configuration. This is probably not what you want. Make sure you have a valid AUDIO/MIDI configuration file.\n");
      fts_post( "Warning: no configuration found\n");
    }
  else
    {
      fts_log( "[boot] opened config : %s\n", config_file);
    }

  fts_config_set(config);
}

/***********************************************************************
 *
 * Global initialization
 *
 */

/*
 * Kernel initialisation:
 * these functions initializes the kernel structures
 */

void
fts_kernel_init(void)
{
  /*
   * Attention !!! The order is important (at least for the first lines)
   */
  FTS_MODULE_INIT_CALL( mem);
  FTS_MODULE_INIT_CALL( hashtable);
  FTS_MODULE_INIT_CALL( symbol);
  FTS_MODULE_INIT_CALL( class);
  FTS_MODULE_INIT_CALL( atom);
  FTS_MODULE_INIT_CALL( post);
  FTS_MODULE_INIT_CALL( list);
  FTS_MODULE_INIT_CALL( package);
  FTS_MODULE_INIT_CALL( tuple);
  FTS_MODULE_INIT_CALL( connection);
  FTS_MODULE_INIT_CALL( variable);
  FTS_MODULE_INIT_CALL( function);
  FTS_MODULE_INIT_CALL( define);
  FTS_MODULE_INIT_CALL( bytestream); /* Must be before patcher_init */
  FTS_MODULE_INIT_CALL( pipestream); /* Must be before patcher_init */
  FTS_MODULE_INIT_CALL( socketstream);
  FTS_MODULE_INIT_CALL( patcher);
  FTS_MODULE_INIT_CALL( parser);
  FTS_MODULE_INIT_CALL( expression); /* Must be after parser_init */
  FTS_MODULE_INIT_CALL( ftl);
  FTS_MODULE_INIT_CALL( param);
  FTS_MODULE_INIT_CALL( preset);
  FTS_MODULE_INIT_CALL( dsp_graph);
  FTS_MODULE_INIT_CALL( dsp);
  FTS_MODULE_INIT_CALL( abstraction);
  FTS_MODULE_INIT_CALL( template);
  FTS_MODULE_INIT_CALL( autosave);
  FTS_MODULE_INIT_CALL( patfile);
  FTS_MODULE_INIT_CALL( time);
  FTS_MODULE_INIT_CALL( sched);
  FTS_MODULE_INIT_CALL( audiofile);
  FTS_MODULE_INIT_CALL( error_object);
  FTS_MODULE_INIT_CALL( message);
  FTS_MODULE_INIT_CALL( audio);
  FTS_MODULE_INIT_CALL( audioconfig);  
  FTS_MODULE_INIT_CALL( client);
  FTS_MODULE_INIT_CALL( client_manager);
  FTS_MODULE_INIT_CALL( update);
  FTS_MODULE_INIT_CALL( clipboard);
  FTS_MODULE_INIT_CALL( label);
  FTS_MODULE_INIT_CALL( midievent);
  FTS_MODULE_INIT_CALL( midi);
  FTS_MODULE_INIT_CALL( config);
  FTS_MODULE_INIT_CALL( objectset);
  FTS_MODULE_INIT_CALL( selection);
  FTS_MODULE_INIT_CALL( saver);
  FTS_MODULE_INIT_CALL( thread_manager);
  FTS_MODULE_INIT_CALL( binary_protocol);
}

void fts_init( int argc, char **argv)
{
  fts_log("[fts]: Kernel initialization\n");

  /* Initialization */
  fts_kernel_init();

  fts_log("[fts]: Parsing command line arguments\n");

  /* Must be here, since it can be used by further modules */
  fts_cmd_args_parse( argc, argv);

  fts_log("[fts]: Platform initialization\n");

  /* Platform dependant initialization */
  fts_platform_init();

  fts_log("[fts]: Initializing kernel classes\n");

  /* if we have to start fts with a client */
  if (fts_cmd_args_get(fts_new_symbol("no-client")) == NULL)
  {
    /* check whether we should use a piped connection thru the stdio file handles */
    if ( fts_cmd_args_get( fts_new_symbol( "stdio")) != NULL )
      fts_client_manager_pipe_start();
    else
      fts_client_manager_tcp_start();
  }

  fts_log("[fts]: Loading project\n");

  /* Load the initial project */
  fts_load_project();

  /* Load the initial configuration */
  fts_load_config();
}

/***********************************************************************
 *
 * Global shutdown
 *
 */

void 
fts_shutdown( void)
{
}

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
