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
#include <fts/fts.h>
#include <ftsprivate/patcher.h>
#include <ftsprivate/platform.h>


/***********************************************************************
 *
 * Command line arguments:
 *  - stored in variable in a subpatcher
 *  - can be retrieved from C code or in a patcher
 *
 */

typedef struct _v_t {
  fts_object_t head;
  fts_atom_t value;
} v_t;

static fts_symbol_t s___v;
static fts_symbol_t s_yes;
static fts_patcher_t *cmd_args_patcher;

static void v_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  v_t *this = (v_t *)o;

  this->value = at[1];
}

static void v_get_state( fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  v_t *this = (v_t *)o;

  *value = this->value;
}

static fts_status_t v_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init( cl, sizeof( v_t), 0, 0, 0);

  fts_method_define_varargs( cl, fts_SystemInlet, fts_s_init, v_init);
  fts_class_add_daemon( cl, obj_property_get, fts_s_state, v_get_state);

  return fts_Success;
}

static int fts_cmd_args_put( fts_symbol_t name, fts_symbol_t value)
{
  fts_atom_t a[4];
  fts_object_t *newobj;

  fts_set_symbol( a+0, name);
  fts_set_symbol( a+1, fts_s_colon);
  fts_set_symbol( a+2, s___v);
  fts_set_symbol( a+3, value);

  newobj = fts_eval_object_description( cmd_args_patcher, 4, a);

  return !fts_object_is_error( newobj);
}

fts_symbol_t fts_cmd_args_get( fts_symbol_t name)
{
  fts_atom_t *value;

  value = fts_variable_get_value( cmd_args_patcher, name);

  if (!fts_is_symbol( value))
    return 0;

  return fts_get_symbol( value);
}

static void fts_cmd_args_parse( int argc, char **argv)
{
  int filecount = 1, r;
  char filevar[32];
  fts_symbol_t name, value;

  argc--;
  argv++;
  while (argc)
    {
      if (!strncmp( *argv, "--", 2))
	{
	  char *p = strchr( *argv, '=');

	  if (p != NULL)
	    *p = '\0';

	  name = fts_new_symbol_copy( *argv + 2);

	  if (p == NULL || p[1] == '\0')
	    value = s_yes;
	  else
	    {
	      p++;
	      value = fts_new_symbol_copy( p);
	    }
	}
      else
	{
	  sprintf( filevar, "file%d", filecount++);
	  name = fts_new_symbol_copy( filevar);
	  value = fts_new_symbol_copy( *argv);
	}

      fts_cmd_args_put( name, value);

      argc--;
      argv++;
    }
}

static void fts_kernel_cmd_args_init( void)
{
  fts_atom_t a[2];

  s___v = fts_new_symbol( "__v");
  fts_class_install( s___v, v_instantiate);

  s_yes = fts_new_symbol( "yes");

  fts_set_symbol( a+0, fts_s_patcher);
  fts_set_symbol( a+1, fts_new_symbol("environnment"));
  fts_object_new_to_patcher( fts_get_root_patcher(), 2, a, (fts_object_t **)&cmd_args_patcher);
  if ( !cmd_args_patcher)
    {
      fprintf( stderr, "cannot create environnment patcher\n");
      return;
    }
}


/***********************************************************************
 *
 * Global initialization
 *
 */

extern void fts_kernel_abstraction_init( void);
extern void fts_kernel_atom_init( void);
extern void fts_kernel_audio_init( void);
extern void fts_kernel_autosave_init( void);
extern void fts_kernel_bytestream_init( void);
extern void fts_kernel_class_init( void);
extern void fts_kernel_clipboard_init( void);
extern void fts_kernel_connection_init( void);
extern void fts_kernel_doctor_init( void);
extern void fts_kernel_dsp_graph_init( void);
extern void fts_kernel_dsp_init( void);
extern void fts_kernel_expression_init( void);
extern void fts_kernel_ftl_init( void);
extern void fts_kernel_hashtable_init( void);
extern void fts_kernel_midi_init( void);
extern void fts_kernel_objtable_init( void);
extern void fts_kernel_objtable_init( void);
extern void fts_kernel_oldclient_init( void);
extern void fts_kernel_oldftsdata_init( void);
extern void fts_kernel_oldpatcherdata_init( void);
extern void fts_kernel_param_init( void);
extern void fts_kernel_patcher_init( void);
extern void fts_kernel_patparser_init( void);
extern void fts_kernel_property_init( void);
extern void fts_kernel_sched_init( void);
extern void fts_kernel_selection_init( void);
extern void fts_kernel_soundfile_init( void);
extern void fts_kernel_symbol_init( void);
extern void fts_kernel_template_init( void);
extern void fts_kernel_variable_init( void);
extern void fts_kernel_olducs_init( void);

extern void fts_oldclient_start( void);


void fts_init( int argc, char **argv)
{
  /* *** Attention !!! The order is important *** */
  fts_kernel_hashtable_init();
  fts_kernel_symbol_init();
  fts_kernel_atom_init();
  fts_kernel_objtable_init();
  fts_kernel_class_init();
  fts_kernel_doctor_init();
  fts_kernel_connection_init();
  fts_kernel_property_init();
  fts_kernel_oldftsdata_init();
  fts_kernel_oldpatcherdata_init();
  fts_kernel_variable_init();
  fts_kernel_patcher_init();
  fts_kernel_expression_init();
  fts_kernel_ftl_init();
  fts_kernel_param_init();
  fts_kernel_dsp_graph_init();
  fts_kernel_dsp_init();
  fts_kernel_abstraction_init();
  fts_kernel_template_init();

  /* For the rest, the order is no longer important */
  fts_kernel_audio_init();
  fts_kernel_bytestream_init();
  fts_kernel_autosave_init();
  fts_kernel_clipboard_init();
  fts_kernel_midi_init();
  fts_kernel_patparser_init();
  fts_kernel_sched_init();
  fts_kernel_selection_init();
  fts_kernel_soundfile_init();
  fts_kernel_oldclient_init();
  fts_kernel_olducs_init();

  fts_kernel_cmd_args_init();

  fts_cmd_args_parse( argc, argv);

  fts_platform_init();

  fts_oldclient_start();
}
