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

#include "naming.h"

/*
 * Basic naming services; for now, just provide global names,
 * Only for compatibility with the ISPW Object set; don't use for new objects.
 * It handle only named fts_object_t; used by the message box to send messages
 * to tables and table~s
 */

static fts_hashtable_t ispw_name_table; /* the name binding table */

fts_object_t *
ispw_get_object_by_name(fts_symbol_t name)
{
  fts_atom_t d, k;

  fts_set_symbol( &k, name);
  if (fts_hashtable_get(&ispw_name_table, &k, &d))
    return  (fts_object_t *) fts_get_object(&d);
  else
    return 0;
}

void 
ispw_register_named_object(fts_object_t *obj, fts_symbol_t name)
{
  fts_atom_t a, k;

  fts_set_symbol( &k, name);
  fts_set_object(&a, obj);
  fts_hashtable_put(&ispw_name_table, &k, &a);
}

void 
ispw_unregister_named_object(fts_object_t *obj, fts_symbol_t name)
{
  fts_atom_t k;

  fts_set_symbol( &k, name);
  fts_hashtable_remove( &ispw_name_table, &k);
}

fts_object_t *
ispw_get_target(fts_patcher_t *scope, fts_symbol_t name)
{
  fts_label_t *label = fts_label_get(scope, name);
  
  if(label)
    return (fts_object_t *)label;
  else
    return ispw_get_object_by_name(name);
}

void
ispw_target_send(fts_object_t *target, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_class_t *cl = fts_object_get_class(target);

  if(cl == fts_label_class)
    fts_label_send((fts_label_t *)target, s, ac, at);
  else if(s != NULL)
    {
      if(!fts_send_message(target, s, ac, at))
	fts_object_error(target, "don't understand message %s (from ispw message box)", s);
    }
}

void 
ispw_naming_init(void)
{
  fts_hashtable_init(&ispw_name_table, FTS_HASHTABLE_MEDIUM);
}
