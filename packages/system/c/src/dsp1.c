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


typedef struct 
{
  fts_object_t o;
} dsp_t;


static void 
dsp_active(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int active = fts_get_int(at);

  fts_outlet_int(o, 0, active);
}

static void
dsp_start(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if(fts_dsp_get_active() != 0)
    fts_dsp_desactivate();

  fts_dsp_activate();
}

static void
dsp_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_desactivate();
}

static void
dsp_on_off(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int active = fts_get_number_int(at);

  if(active)
    fts_dsp_activate();
  else
    fts_dsp_desactivate();
}

static void
dsp_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dsp_chain_post();
}

static void
dsp_save(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  FILE *f;
  
  if (fts_is_symbol(at))
    {
      const char *filename;

      filename = fts_get_symbol(at);

      f = fopen(filename, "wb");

      if (f)
	dsp_chain_fprint(f);

      fclose(f);
    }
}

static void
dsp_print_signals(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dsp_chain_post_signals();
}


static void
dsp_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_active_add_listener(o, dsp_active);
}

static void
dsp_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_active_remove_listener(o);
}


static fts_status_t
dsp_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(dsp_t), 1, 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, dsp_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, dsp_delete);

  fts_method_define_varargs(cl, 0, fts_s_start, dsp_start);
  fts_method_define_varargs(cl, 0, fts_s_stop, dsp_stop);

  fts_method_define_varargs(cl, 0, fts_s_int, dsp_on_off);
  fts_method_define_varargs(cl, 0, fts_s_float, dsp_on_off);
  
  fts_method_define_varargs(cl, 0, fts_s_bang, dsp_print);
  fts_method_define_varargs(cl, 0, fts_s_print, dsp_print);

  fts_method_define_varargs(cl, 0, fts_new_symbol("save"), dsp_save);

  fts_method_define_varargs(cl, 0, fts_new_symbol("print-signals"), dsp_print_signals);
  
  return fts_Success;
}

void
dsp_config(void)
{
  fts_class_install(fts_new_symbol("dsp"),dsp_instantiate);
  fts_alias_install(fts_new_symbol("dsp~"), fts_new_symbol("dsp"));
}

