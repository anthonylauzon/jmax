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



typedef struct sigthrough
{
  fts_dsp_object_t o;
} sigthru_t;

static void
sigthru_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac,  const fts_atom_t *av)
{
  fts_outlet_send(o, 1, s, ac, av); 
}

static void
sigthru_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac,  const fts_atom_t *av)
{
  fts_dsp_object_init((fts_dsp_object_t *)o);
}

static void
sigthru_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac,  const fts_atom_t *av)
{
  fts_dsp_object_delete((fts_dsp_object_t *)o);
}

static void
sigthru_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);

  if (fts_dsp_get_input_name(dsp, 0) != fts_dsp_get_output_name(dsp, 0))
    fts_dsp_add_function_copy(fts_dsp_get_input_name(dsp, 0), fts_dsp_get_output_name(dsp, 0), fts_dsp_get_input_size(dsp, 0));
}

static void
sigthru_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(sigthru_t), sigthru_init, sigthru_delete);

  fts_class_message_varargs(cl, fts_s_put, sigthru_put);

  fts_class_set_default_handler(cl, sigthru_input);
  fts_class_inlet_anything(cl, 0);
  fts_class_outlet_anything(cl, 1);

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0);
}

void
sigthru_config(void)
{
  fts_class_install(fts_new_symbol("thru~"), sigthru_instantiate);
}
