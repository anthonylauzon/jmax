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
 */

#include <fts/fts.h>

static fts_symbol_t runtime_errors_symbol = 0;

typedef struct _fts_runtime_errors_t 
{
  fts_object_t ob;
} fts_runtime_errors_t;

static void fts_runtime_errors_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_runtime_errors_t *this = (fts_runtime_errors_t *)o;

  fts_runtime_error_handler_set(o);
}

static void fts_runtime_errors_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_runtime_errors_t *this = (fts_runtime_errors_t *)o;

  fts_runtime_error_handler_remove(o);
}

static fts_status_t
runtime_errors_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(fts_runtime_errors_t), 0, 0, 0); 

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, fts_runtime_errors_init);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_delete, fts_runtime_errors_delete);

  return fts_ok;
}

/***********************************************************************
 *
 * Initialization
 *
 */

void runtime_errors_config( void)
{
  runtime_errors_symbol = fts_new_symbol("__runtimeerrors");

  fts_class_install(runtime_errors_symbol, runtime_errors_instantiate);
}
