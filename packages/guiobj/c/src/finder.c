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

fts_symbol_t finder_symbol = 0;

typedef struct _fts_finder_t 
{
  fts_object_t ob;
} fts_finder_t;


static void fts_finder_find( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_finder_t *this = (fts_finder_t *)o;
  fts_objectset_t *set = (fts_objectset_t *)fts_get_object(at);
  fts_object_t *scope = fts_get_object(at+1);
  fts_atom_t a[256];
  int i;

  fts_objectset_clear(set);

  fts_set_object(&a[0], (fts_object_t *) set);

  for (i = 2; (i < ac) && (i < 256); i++)
    a[i-1] = at[i];

  fts_send_message(scope, fts_s_find, ac-1, a);
}

static void fts_finder_find_friends( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_finder_t *this = (fts_finder_t *)o;
  fts_objectset_t *set = (fts_objectset_t *)fts_get_object(at);
  fts_object_t *target = fts_get_object(at+1);

  fts_atom_t a[2];

  fts_objectset_clear(set);

  fts_set_object(&a[0], (fts_object_t *) set);
  
  if(fts_class_get_method( fts_object_get_class(target), fts_s_find_friends))
    fts_send_message(target, fts_s_find_friends, 1, a);
  else
    {
	fts_set_symbol(&a[1], fts_object_get_class_name(target));
	fts_send_message((fts_object_t *)fts_get_root_patcher(), fts_s_find, 2, a);
    }
}

static void
finder_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_finder_t), 0, 0); 

  fts_class_method_varargs(cl, fts_new_symbol("finder_find"), fts_finder_find);
  fts_class_method_varargs(cl, fts_new_symbol("finder_find_friends"), fts_finder_find_friends);
}

/***********************************************************************
 *
 * Initialization
 *
 */

void finder_config( void)
{
  finder_symbol = fts_new_symbol("__finder");

  fts_class_install(finder_symbol, finder_instantiate);
}




