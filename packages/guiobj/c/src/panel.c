/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#include "fts.h"

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

  this->tag = fts_get_long(at);
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


static void
panel_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
}


/* Register panel for savepanel and openpanel */


static fts_status_t
panel_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[1];

  fts_class_init(cl, sizeof(panel_t), 1, 1, 0);

  a[0] = fts_s_symbol;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, panel_init, 1, a);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, panel_delete, 0, 0);

  /* fts_method_define(cl, fts_SystemInlet, fts_s_send_event, panel_evsched, 0, 0); */

  a[0] = fts_s_int;
  fts_method_define(cl, fts_SystemInlet, fts_new_symbol("set_update_tag"), panel_connect, 1, a); 

  fts_method_define(cl, 0, fts_s_bang, panel_bang, 0, 0);

  a[0] = fts_s_symbol;
  fts_method_define(cl, 0, fts_s_symbol, panel_symbol, 1, a);

  a[0] = fts_s_int;
  fts_outlet_type_define(cl, 0, fts_s_bang, 1, a);

  return fts_Success;
}


void
panel_config(void)
{
  fts_class_install( fts_new_symbol("openpanel"), panel_instantiate);
  fts_class_alias( fts_new_symbol("savepanel"), fts_new_symbol("openpanel"));
}
