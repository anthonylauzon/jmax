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

fts_symbol_t error_finder_symbol = 0;
static fts_objectset_t *error_set = 0;

typedef struct _fts_error_finder_t 
{
  fts_object_t ob;
} fts_error_finder_t;

static void fts_error_finder_find( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_error_finder_t *this = (fts_error_finder_t *)o;
  fts_objectset_t *set = (fts_objectset_t *)fts_get_object(at);
  fts_object_t *scope = fts_get_object(at+1);
  fts_atom_t a;

  fts_objectset_clear(set);

  fts_set_object(&a, (fts_object_t *) set);
  fts_send_message(scope, fts_s_find_errors, 1, &a);
}

static void
error_finder_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_error_finder_t), NULL, NULL); 

  fts_class_method_varargs(cl, fts_new_symbol("error_finder_find"), fts_error_finder_find);
}

/***********************************************************************
 *
 * Initialization
 *
 */

void error_finder_config( void)
{
  error_finder_symbol = fts_new_symbol("__errorfinder");

  fts_class_install(error_finder_symbol, error_finder_instantiate);
}




