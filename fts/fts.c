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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fts/fts.h>
#include <ftsprivate/platform.h>
#include <ftsprivate/package.h>
#include <fts/project.h>


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

void fts_load_project( void)
{
  fts_symbol_t project_symbol;
  fts_symbol_t project_file;
  fts_package_t* project = NULL;

  project_symbol = fts_new_symbol( "project");

  
  /* check if the user specified a project file on the command line  */
  project_file = fts_cmd_args_get( project_symbol);

  /* check if the user has a project file in the home directory  */
  if (project_file == NULL) {
    project_file = fts_get_user_config();
  }

  /* check if there's a system wide project */
  if (project_file == NULL) {
    project_file = fts_get_system_config();
  }

  /* create an empty project */
  if (project_file == NULL) {
    fts_log("[boot]: Starting fts with an empty project. This is probably not what you want. Make sure you have a valid project file.\n");
    project = fts_package_new(project_symbol);
    fts_project_set(project);
  } else {
    project = fts_project_open(project_file);
  }
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

#define _K_DECNCALL(F) { extern void F(void); F(); }

static void fts_kernel_init( void)
{
  /*
 *** Attention !!! The order is important, at least for the first lines ***
 */
  _K_DECNCALL( fts_kernel_hashtable_init);
  _K_DECNCALL( fts_kernel_symbol_init);
  _K_DECNCALL( fts_kernel_class_init);
  _K_DECNCALL( fts_kernel_atom_init);
  _K_DECNCALL( fts_kernel_post_init);
  _K_DECNCALL( fts_kernel_list_init);
  _K_DECNCALL( fts_kernel_property_init);
  _K_DECNCALL( fts_kernel_package_init);
  _K_DECNCALL( fts_kernel_tuple_init);
  _K_DECNCALL( fts_kernel_connection_init);
  _K_DECNCALL( fts_kernel_variable_init);
  _K_DECNCALL( fts_kernel_patcher_init);
  _K_DECNCALL( fts_kernel_parser_init);
  _K_DECNCALL( fts_kernel_expression_init); /* Must be after parser_init */
  _K_DECNCALL( fts_kernel_ftl_init);
  _K_DECNCALL( fts_kernel_param_init);
  _K_DECNCALL( fts_kernel_preset_init);
  _K_DECNCALL( fts_kernel_dsp_graph_init);
  _K_DECNCALL( fts_kernel_dsp_init);
  _K_DECNCALL( fts_kernel_abstraction_init);
  _K_DECNCALL( fts_kernel_template_init);
  _K_DECNCALL( fts_kernel_autosave_init);
  _K_DECNCALL( fts_kernel_patfile_init);
  _K_DECNCALL( fts_kernel_time_init);
  _K_DECNCALL( fts_kernel_sched_init);
  _K_DECNCALL( fts_kernel_audiofile_init);
}

/*
 * Installation of kernel packages 
 */
static void fts_kernel_classes_config( void)
{
  _K_DECNCALL( fts_patcher_config);
  _K_DECNCALL( fts_error_object_config);
  _K_DECNCALL( fts_message_config);
  _K_DECNCALL( fts_audio_config);
  _K_DECNCALL( fts_audioconfig_config);  
  _K_DECNCALL( fts_bytestream_config);
  _K_DECNCALL( fts_client_config);
  _K_DECNCALL( fts_update_config);
  _K_DECNCALL( fts_clipboard_config);
  _K_DECNCALL( fts_label_config);
  _K_DECNCALL( fts_midi_config);
  _K_DECNCALL( fts_config_config);
  _K_DECNCALL( fts_objectset_config);
  _K_DECNCALL( fts_selection_config);
  _K_DECNCALL( fts_saver_config);
  _K_DECNCALL( fts_thread_manager_config);
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

  fts_log("[fts]: Configure DSP timebase\n");

  fts_dsp_timebase_configure();

  fts_log("[fts]: Initializing kernel classes\n");

  /* Installation of kernel classes */
  fts_kernel_classes_config();

  fts_log("[fts]: Loading project\n");

  /* Load the initial project */
  fts_load_project();
}

/***********************************************************************
 *
 * Global shutdown
 *
 */

void fts_kernel_patcher_shutdown(void);

void fts_shutdown( void)
{
  fts_log("[fts]: Deleting root patcher\n");
  fts_kernel_patcher_shutdown();
}

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
