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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 */

#include <fts/fts.h>
#include <ftsconfig.h>

typedef struct {
  fts_dsp_object_t head;
  fts_audiolabel_t *left_label;
  fts_audiolabel_t *right_label;
} monitor_t;

static fts_symbol_t monitor_symbol = 0;

static void
monitor_ftl( fts_word_t *argv)
{
  monitor_t *self = (monitor_t *)fts_word_get_pointer( argv+0);
  float * restrict left_in = (float *) fts_word_get_pointer(argv + 1);
  float * restrict right_in = (float *) fts_word_get_pointer(argv + 2);
  int n_tick = fts_word_get_int(argv + 3);

  if (self->left_label)
    fts_audiolabel_output( self->left_label, left_in, n_tick);

  if (self->right_label)
    fts_audiolabel_output( self->right_label, right_in, n_tick);
}

static void 
monitor_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  monitor_t *self = (monitor_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  fts_atom_t argv[4];

  fts_set_object( argv+0, self);
  fts_set_symbol( argv+1, fts_dsp_get_input_name( dsp, 0));
  fts_set_symbol( argv+2, fts_dsp_get_input_name( dsp, 1));
  fts_set_int( argv+3, fts_dsp_get_input_size(dsp, 0));

  fts_dsp_add_function( monitor_symbol, 4, argv);
}

static void 
monitor_start(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_activate();
}

static void 
monitor_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_desactivate();
}

static void 
monitor_update_real_time(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int active = fts_dsp_is_active();
  fts_atom_t a;

  fts_set_int( &a, active);
  fts_client_send_message_real_time(o, fts_s_value, 1, &a);
}

static void 
monitor_dsp_active(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_update_request(o);
}

static void 
monitor_toggle(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if(fts_dsp_is_active())
    fts_dsp_desactivate();
  else
    fts_dsp_activate();

  fts_update_request(o);
}

static void 
monitor_get_value(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  int active = fts_dsp_is_active();

  fts_set_int(value, active);
}

static void 
monitor_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  monitor_t *self = (monitor_t *)o;

  fts_dsp_object_init((fts_dsp_object_t *)self);

  self->left_label = fts_audiolabel_get( fts_new_symbol( "default L"));
  self->right_label = fts_audiolabel_get( fts_new_symbol( "default R"));

  if ( !self->left_label || !self->right_label)
  {
    self->left_label = fts_audiolabel_get( fts_new_symbol( "default"));
    self->right_label = NULL;
  }

  fts_dsp_active_add_listener(o, monitor_dsp_active);
}

static void 
monitor_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_active_remove_listener(o);
  fts_dsp_object_delete((fts_dsp_object_t*)o);
}

static void 
monitor_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(monitor_t), monitor_init, monitor_delete);

  fts_class_message_varargs(cl, fts_s_update_real_time, monitor_update_real_time); 

  fts_class_add_daemon(cl, obj_property_get, fts_s_value, monitor_get_value);

  fts_class_message_varargs(cl, fts_s_put, monitor_put);

  fts_class_message_varargs(cl, fts_s_start, monitor_start);
  fts_class_message_varargs(cl, fts_s_stop, monitor_stop);

  fts_class_message_varargs(cl, fts_new_symbol("click"), monitor_toggle);

  fts_class_inlet_bang(cl, 0, monitor_toggle);

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_inlet(cl, 1);
}

void 
monitor_config( void)
{
  fts_class_install(fts_new_symbol( "monitor~"), monitor_instantiate);

  fts_dsp_declare_function( monitor_symbol, monitor_ftl);
}

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
