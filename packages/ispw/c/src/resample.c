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

#include <fts/fts.h>
#include <ftsconfig.h>

#include <string.h>

static fts_symbol_t sigup_function = 0;

typedef struct
{
  float last[3];
} up_state_t;

typedef struct up
{
  fts_dsp_object_t _o;
  ftl_data_t up_ftl_data;
} sigup_t;


static void
ftl_up(fts_word_t *argv)
{
  float * restrict in  = (float *)fts_word_get_pointer(argv);
  float * restrict out = (float *)fts_word_get_pointer(argv+1);
  up_state_t *x = (up_state_t *)fts_word_get_pointer(argv+2);
  int n = fts_word_get_int(argv+3);
  int i, j;
  float x0;
  float x1 = x->last[0];
  float x2 = x->last[1];
  float x3 = x->last[2];

  for (i = 0, j = 0; i < n; i++, j += 2)
    {
      x0 = x1;
      x1 = x2;
      x2 = x3;
      x3 = in[i];
      out[j]  = .5625f * (x1 + x2) - .0625f * (x0 + x3);
      out[j+1] = x2;
    }

  x->last[0] = x1;
  x->last[1] = x2;
  x->last[2] = x3;
}


static void
sigup_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigup_t *this = (sigup_t *)o;
  fts_atom_t argv[4];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  const float zero = 0.0f;

  ftl_data_set(up_state_t, this->up_ftl_data, last[0], &zero);
  ftl_data_set(up_state_t, this->up_ftl_data, last[1], &zero);
  ftl_data_set(up_state_t, this->up_ftl_data, last[2], &zero);

  fts_set_symbol(argv,   fts_dsp_get_input_name(dsp, 0));
  fts_set_symbol(argv+1, fts_dsp_get_output_name(dsp, 0));

  fts_set_ftl_data(argv+2, this->up_ftl_data);
  fts_set_int(argv+3, fts_dsp_get_input_size(dsp, 0));

  fts_dsp_add_function(sigup_function, 4, argv);
}

static void
sigup_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigup_t *this = (sigup_t *)o;

  fts_dsp_object_init((fts_dsp_object_t *)o);
  fts_dsp_object_set_resampling((fts_dsp_object_t *)o, 1);

  this->up_ftl_data = ftl_data_new(up_state_t);
}

static void
sigup_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigup_t *this = (sigup_t *)o;

  ftl_data_free(this->up_ftl_data);

  fts_dsp_object_delete((fts_dsp_object_t *)o);
}

static void
sigup_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(sigup_t), sigup_init, sigup_delete);

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0);
  fts_class_message_varargs(cl, fts_s_put, sigup_put);
  
  sigup_function = fts_new_symbol("up");
  fts_dsp_declare_function(sigup_function, ftl_up);
}

void
sigup_config(void)
{
  fts_class_install(fts_new_symbol("up~"),sigup_instantiate);
}

static fts_symbol_t sigdown_function = 0;

typedef struct down
{
  fts_dsp_object_t _o;
} sigdown_t;


static void
ftl_down(fts_word_t *argv)
{
  float * restrict in = (float *)fts_word_get_pointer(argv);
  float * restrict out = (float *)fts_word_get_pointer(argv+1);
  long int n = fts_word_get_int(argv+2);
  int i, j;

  for (i = 0, j = 0; i < n; i++, j += 2)
    out[i] = in[j];
}


static void
sigdown_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_t argv[3];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);

  fts_set_symbol(argv,   fts_dsp_get_input_name(dsp, 0));
  fts_set_symbol(argv+1, fts_dsp_get_output_name(dsp, 0));
  fts_set_int  (argv+2, fts_dsp_get_input_size(dsp, 0) >> 1);

  fts_dsp_add_function(sigdown_function, 3, argv);
}

static void
sigdown_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_object_init((fts_dsp_object_t *)o);
  fts_dsp_object_set_resampling((fts_dsp_object_t *)o, -1);
}

static void
sigdown_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_object_delete((fts_dsp_object_t *)o);
}

static void
sigdown_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(sigdown_t), sigdown_init, sigdown_delete);

  fts_class_message_varargs(cl, fts_s_put, sigdown_put);

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0);
  
  sigdown_function = fts_new_symbol("down");
  fts_dsp_declare_function(sigdown_function, ftl_down);
}

void
sigdown_config(void)
{
  fts_class_install(fts_new_symbol("down~"),sigdown_instantiate);
}
