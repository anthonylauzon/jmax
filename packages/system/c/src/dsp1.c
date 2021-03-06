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


typedef struct 
{
  fts_object_t o;
} dsp_t;


static void 
dsp_active(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  int active = fts_get_int(at);

  fts_outlet_int(o, 0, active);
}

static void
dsp_start(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  if(fts_dsp_is_active())
    fts_dsp_desactivate();

  fts_dsp_activate();
}

static void
dsp_stop(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_dsp_desactivate();
}

static void
dsp_on_off(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  int active = fts_get_number_int(at);

  if(active)
    fts_dsp_activate();
  else
    fts_dsp_desactivate();
}

static void
dsp_print(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  if (!fts_dsp_is_active())
  {
    fts_object_error(o, "Cannot print dsp chains if dsp is inactive");
    fts_post("[dsp] Cannot print dsp chains if dsp is inactive\n");
  }
  else
  {
    dsp_chain_post();
  }
}

static void
dsp_save(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
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
dsp_print_signals(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
    dsp_chain_post_signals();
}


static void
dsp_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_dsp_active_add_listener(o, dsp_active);
}

static void
dsp_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_dsp_active_remove_listener(o);
}


static void
dsp_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(dsp_t), dsp_init, dsp_delete);

  fts_class_message_varargs(cl, fts_s_start, dsp_start);
  fts_class_message_varargs(cl, fts_s_stop, dsp_stop);

  fts_class_inlet_int(cl, 0, dsp_on_off);
  fts_class_inlet_float(cl, 0, dsp_on_off);
  
  fts_class_inlet_bang(cl, 0, dsp_print);
  fts_class_message_varargs(cl, fts_s_print, dsp_print);

  fts_class_message_varargs(cl, fts_new_symbol("save"), dsp_save);
  fts_class_message_varargs(cl, fts_new_symbol("print-signals"), dsp_print_signals);

  fts_class_outlet_int(cl, 0);
}

void
dsp_config(void)
{
  fts_class_t *cl = fts_class_install(fts_new_symbol("dsp"), dsp_instantiate);
  fts_class_alias(cl, fts_new_symbol("dsp~"));
}

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
