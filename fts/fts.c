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

#include <fts/fts.h>


typedef struct _v_t {
  fts_object_t head;
  fts_atom_t value;
} v_t;

static fts_symbol_t s___v;
static fts_patcher_t *env_patcher;

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

static void test_define( fts_symbol_t s, int i)
{
  fts_atom_t a[4];
  fts_object_t *newobj;

  fts_set_symbol( a+0, s);
  fts_set_symbol( a+1, fts_s_colon);
  fts_set_symbol( a+2, s___v);
  fts_set_int( a+3, i);

  newobj = fts_eval_object_description( env_patcher, 4, a);

  if (!newobj)
    {
      fprintf( stderr, "Error instantiating v object\n");
    }
}

static void test_get( fts_symbol_t s)
{
  fts_atom_t *p;

  p = fts_variable_get_value( env_patcher, s);

  if (!fts_is_void( p))
    fprintf( stderr, "%s -> %d\n", fts_symbol_name( s), fts_get_int( p));
  else
    fprintf( stderr, "%s undef\n", fts_symbol_name( s));
}

static void test_variables( void)
{
  fts_atom_t a[2];

  s___v = fts_new_symbol( "__v");
  fts_class_install( s___v, v_instantiate);

  fts_set_symbol( a+0, fts_s_patcher);
  fts_set_symbol( a+1, fts_new_symbol("environnment"));
  fts_object_new_to_patcher( fts_get_root_patcher(), 2, a, (fts_object_t **)&env_patcher);
  if ( !env_patcher)
    {
      fprintf( stderr, "cannot create environnment patcher\n");
      return;
    }

  test_define( fts_new_symbol( "foo"), 1);
  test_define( fts_new_symbol( "boo"), 2);
  test_get( fts_new_symbol( "boo"));
  test_get( fts_new_symbol( "foo"));
  test_get( fts_new_symbol( "ttt"));
}





extern void fts_kernel_abstraction_init( void);
extern void fts_kernel_atom_init( void);
extern void fts_kernel_audio_init( void);
extern void fts_kernel_autosave_init( void);
extern void fts_kernel_bytestream_init( void);
extern void fts_kernel_class_init( void);
extern void fts_kernel_clipboard_init( void);
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

void fts_init( void)
{
  /* *** Attention !!! The order is important *** */
  fts_kernel_hashtable_init();
  fts_kernel_symbol_init();
  fts_kernel_atom_init();
  fts_kernel_objtable_init();
  fts_kernel_class_init();
  fts_kernel_doctor_init();
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
  fts_kernel_objtable_init();
  fts_kernel_patparser_init();
  fts_kernel_sched_init();
  fts_kernel_selection_init();
  fts_kernel_soundfile_init();

  fts_kernel_oldclient_init();

#if 1
  test_variables();
#endif
}
