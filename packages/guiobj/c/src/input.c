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
 */

/*
 * This file's authors:
 *  François Déchelle (dechelle@ircam.fr)
 */

#include <fts/fts.h>
#include <ftsconfig.h>

typedef struct {
  fts_dsp_object_t head;
  fts_audiolabel_t* left_label;
  fts_audiolabel_t* right_label;
} input_t;


static fts_symbol_t input_symbol = 0;

static void
input_ftl(fts_word_t* argv)
{
  input_t* self = (input_t*)fts_word_get_pointer(argv+0);
  float* restrict left_out = (float*)fts_word_get_pointer(argv+1);
  float* restrict right_out = (float*)fts_word_get_pointer(argv+2);
  int n_tick = fts_word_get_int(argv+3);

  if (self->left_label)
    fts_audiolabel_input(self->left_label, left_out, n_tick);

  if (self->right_label)
    fts_audiolabel_input(self->right_label, right_out, n_tick);
}

static void
input_put(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  input_t* self = (input_t*)o;
  fts_dsp_descr_t* dsp = (fts_dsp_descr_t*)fts_get_pointer(at);
  fts_atom_t argv[4];

  fts_set_object(argv+0, self);
  fts_set_symbol(argv+1, fts_dsp_get_output_name(dsp, 0));
  fts_set_symbol(argv+2, fts_dsp_get_output_name(dsp, 1));
  fts_set_int(argv+3, fts_dsp_get_output_size(dsp, 0));

  fts_dsp_add_function(input_symbol, 4, argv);
}

static void 
input_dsp_active(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_update_request(o);
}

static void 
input_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  input_t *self = (input_t *)o;
  int outlets, i;

  fts_dsp_object_init((fts_dsp_object_t*)self);

  self->left_label = fts_audiolabel_get(fts_new_symbol("default L"));
  self->right_label = fts_audiolabel_get(fts_new_symbol("default R"));

  if (!self->left_label || !self->right_label)
  {
    self->left_label = fts_audiolabel_get(fts_new_symbol("default"));
    self->right_label = NULL;
  }

  fts_dsp_active_add_listener(o, input_dsp_active);
}

static void 
input_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_active_remove_listener(o);
}

static void 
input_start(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_activate();
}

static void
input_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_desactivate();
}

static void 
input_toggle(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if(fts_dsp_is_active())
    fts_dsp_desactivate();
  else
    fts_dsp_activate();

  fts_update_request(o);
}

static void 
input_update_real_time(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int active = fts_dsp_is_active();
  fts_atom_t a;

  fts_set_int( &a, active);
  fts_client_send_message_real_time(o, fts_s_value, 1, &a);
}

static void 
input_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(input_t), input_init, input_delete);

  fts_class_message_varargs(cl, fts_s_update_real_time, input_update_real_time);

  fts_class_message_varargs(cl, fts_s_put, input_put);

  fts_class_message_varargs(cl, fts_s_start, input_start);
  fts_class_message_varargs(cl, fts_s_stop, input_stop);

  fts_class_message_varargs(cl, fts_new_symbol("click"), input_toggle);

  fts_dsp_declare_outlet( cl, 0);
  fts_dsp_declare_outlet(cl, 1);
}

void input_config( void)
{
  input_symbol = fts_new_symbol("input~");
  fts_class_install( input_symbol, input_instantiate);

  fts_dsp_declare_function(input_symbol, input_ftl);
}

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
