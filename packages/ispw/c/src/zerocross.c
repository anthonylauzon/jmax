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
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include <fts/fts.h>

typedef struct S_zerocross
{
  fts_object_t obj;
  int count;			/* zero crossing counter*/
  float lastsample;		/* last stored samples */
} zerocross_t;

static fts_symbol_t zerocross_function = 0;

static void
ftl_zerocross(fts_word_t *argv)
{
  float *in = (float *)fts_word_get_pointer(argv + 0);
  zerocross_t *this = (zerocross_t *)fts_word_get_pointer(argv + 1);
  long n = (long)fts_word_get_int(argv + 2);
  float lastsample, current = 0.0;
  unsigned int lastsamplesign, currentsign, count;
  int i;

  lastsample = this->lastsample;
  lastsamplesign = (lastsample <= 0.0f);
  count = this->count;

  for (i = 0; i < n; i++)
    {
      current = in[i];
      currentsign = (current <= 0.0f);
      count += lastsamplesign ^ currentsign;
      lastsamplesign = currentsign;
    }

  this->lastsample = current;
  this->count = count;
}

static void
zerocross_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  zerocross_t *this = (zerocross_t *)o;
  fts_atom_t argv[3];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer_arg(ac, at, 0, 0);

  fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
  fts_set_pointer   (argv + 1, this);
  fts_set_int  (argv + 2, fts_dsp_get_input_size(dsp, 0));

  fts_dsp_add_function(zerocross_function, 3, argv);
}

static void
zerocross_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  zerocross_t *this = (zerocross_t *)o;

  fts_outlet_int(o, 0, this->count);
  this->count = 0;
}

static void
zerocross_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  zerocross_t *this = (zerocross_t *)o;

  /* initialise control */
  this->count = 0;
  this->lastsample = 0.0f;

  /* just put object in the dsp list */
  fts_dsp_add_object(o); 
}


static void
zerocross_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_remove_object(o);
}


static fts_status_t
zerocross_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(zerocross_t), 1, 1, 0); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, zerocross_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, zerocross_delete);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, zerocross_put);

  fts_method_define_varargs(cl, 0, fts_s_bang, zerocross_bang);

  fts_dsp_declare_inlet(cl, 0);

  fts_outlet_type_define_varargs(cl, 0,	fts_s_int);

  zerocross_function = fts_new_symbol("zerocross");
  fts_dsp_declare_function(zerocross_function, ftl_zerocross);

  return fts_Success;
}

void
zerocross_config(void)
{
  fts_class_install(fts_new_symbol("zerocross~"),zerocross_instantiate);
  fts_alias_install(fts_new_symbol("zerocross"), fts_new_symbol("zerocross~"));
}
