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
  fts_object_t obj;

  long tag;
  long clientid;

} panel_t;

/* code to flush a panel up from CP to host */

static void
panel_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  panel_t *this = (panel_t *)o;

  /* fts_obj_have_events((fts_object_t *)this); */
}


static void
panel_evsched(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
				/* @@@ */
}


static void
panel_connect(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  panel_t *this = (panel_t *)o;

  this->tag = fts_get_int(at);
  /* this->clientid = fts_get_clientid(); */
  /* @@@@ */
}


static void
panel_symbol(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_symbol(o, 0, fts_get_symbol(at));
}


static void
panel_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  panel_t *this = (panel_t *)o;

  this->tag = 0;
}

static fts_status_t
panel_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(panel_t), 1, 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, panel_init);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("set_update_tag"), panel_connect); 

  fts_method_define_varargs(cl, 0, fts_s_bang, panel_bang);
  fts_method_define_varargs(cl, 0, fts_s_symbol, panel_symbol);

  fts_outlet_type_define_varargs(cl, 0, fts_s_bang);

  return fts_Success;
}


void
panel_config(void)
{
  fts_class_install( fts_new_symbol("openpanel"), panel_instantiate);
  fts_alias_install( fts_new_symbol("savepanel"), fts_new_symbol("openpanel"));
}
