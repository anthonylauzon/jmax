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

#include <fts/fts.h>
#include <ftsconfig.h>


/* ********************************************************************** */
/* in~ object                                                             */
/* ********************************************************************** */

typedef struct {
  fts_dsp_object_t head;
  fts_audiolabel_t* label;
  fts_symbol_t label_name;
} in_tilda_t;

static fts_symbol_t in_tilda_symbol = 0;

static void
in_tilda_ftl(fts_word_t* argv)
{
  in_tilda_t* self = (in_tilda_t*)fts_word_get_pointer(argv+0);
  float* restrict out = (float*)fts_word_get_pointer(argv+1);
  int n_tick = fts_word_get_int(argv+2);

  if (self->label)
    fts_audiolabel_input(self->label, out, n_tick);
}

static void
in_tilda_put(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  in_tilda_t* self = (in_tilda_t*)o;
  fts_dsp_descr_t* dsp = (fts_dsp_descr_t*)fts_get_pointer(at);
  fts_atom_t argv[3];

  self->label = fts_audiolabel_get(self->label_name);
  if (NULL == self->label)
  {
    fts_object_error(o, "no audiolabel named: %s", self->label_name);
    return;
  }
    
  fts_set_object(argv+0, self);
  fts_set_symbol(argv+1, fts_dsp_get_output_name(dsp, 0));
  fts_set_int(argv+2, fts_dsp_get_output_size(dsp, 0));

  fts_dsp_add_function(in_tilda_symbol, 3, argv);
}

static void in_tilda_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  in_tilda_t *self = (in_tilda_t *)o;
  fts_symbol_t label_name;

  fts_dsp_object_init((fts_dsp_object_t*)self);

  if (ac > 0)
  {
    if (!fts_is_symbol(at))
    {
      fts_object_error(o, "in~ need an audiolabel as first argument");
      return;
    }
    label_name = fts_get_symbol(at);

    self->label = fts_audiolabel_get(label_name);
    if (NULL == self->label)
    {
      fts_object_error(o, "no audiolabel named: %s", label_name);
      return;
    }
    self->label_name = label_name;
  }
  else
  {
    self->label = fts_audiolabel_get(fts_s_default);
    self->label_name = fts_s_default;
  }

}

static void in_tilda_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_object_delete((fts_dsp_object_t*)o);
}

static void in_tilda_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof( fts_object_t), in_tilda_init, in_tilda_delete);
  
  fts_class_message_varargs(cl, fts_s_put, in_tilda_put);
  fts_dsp_declare_outlet( cl, 0);
}


/* ********************************************************************** */
/* out~ object                                                            */
/* ********************************************************************** */

typedef struct {
  fts_dsp_object_t head;
  fts_audiolabel_t *label;
  fts_symbol_t label_name;
} out_tilda_t;

static fts_symbol_t out_tilda_symbol = 0;

static void
out_tilda_ftl(fts_word_t* argv)
{
  out_tilda_t* self = (out_tilda_t*)fts_word_get_pointer(argv+0);
  float* restrict in = (float*)fts_word_get_pointer(argv+1);
  int n_tick = fts_word_get_int(argv+2);
  
  if (self->label)
    fts_audiolabel_output(self->label, in, n_tick);
}

static void 
out_tilda_put(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  out_tilda_t* self = (out_tilda_t*)o;
  fts_dsp_descr_t* dsp = (fts_dsp_descr_t*)fts_get_pointer(at);
  fts_atom_t argv[3];

  self->label = fts_audiolabel_get(self->label_name);
  if (NULL == self->label)
  {
    fts_object_error(o, "no audiolabel named: %s\n", self->label_name);
    return;
  }

  fts_set_object(argv+0, self);
  fts_set_symbol(argv+1, fts_dsp_get_input_name(dsp, 0));
  fts_set_int(argv+2, fts_dsp_get_input_size(dsp, 0));

  fts_dsp_add_function(out_tilda_symbol, 3, argv);
}

static void out_tilda_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  out_tilda_t *self = (out_tilda_t *)o;
  fts_symbol_t label_name;

  fts_dsp_object_init((fts_dsp_object_t*)self);

  if (ac > 0)
  {
    if (!fts_is_symbol(at))
    {
      fts_object_error(o, "out~ need an audiolabel as first argument");
      return;
    }
    label_name = fts_get_symbol(at);
    self->label = fts_audiolabel_get(label_name);
    if (NULL == self->label)
    {
      fts_object_error(o, "no audiolabel named: %s\n", label_name);
      return;
    }
    self->label_name = label_name;
  }
  else
  {
    self->label = fts_audiolabel_get(fts_s_default);
    self->label_name = fts_s_default;
  }

}

static void out_tilda_delete(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  fts_dsp_object_delete((fts_dsp_object_t*)o);
}

static void out_tilda_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof( out_tilda_t), out_tilda_init, out_tilda_delete);

  fts_class_message_varargs(cl, fts_s_put, out_tilda_put);

  fts_dsp_declare_inlet( cl, 0);
}

void in_out_tilda_config( void)
{
  in_tilda_symbol = fts_new_symbol( "in~");
  out_tilda_symbol = fts_new_symbol( "out~");
  fts_class_install(in_tilda_symbol, in_tilda_instantiate);
  fts_class_install(out_tilda_symbol, out_tilda_instantiate);

  fts_dsp_declare_function(in_tilda_symbol, in_tilda_ftl);
  fts_dsp_declare_function(out_tilda_symbol, out_tilda_ftl);
}


/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
