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
#include "send.h"

/*
 * Basic naming services; for now, just provide global names,
 * Only for compatibility with the ISPW Object set; don't use for new objects.
 * It handle only named fts_object_t; used by the message box to send messages
 * to tables and table~s
 */

static fts_hash_table_t ispw_name_table; /* the name binding table */

fts_object_t *
ispw_get_object_by_name(fts_symbol_t name)
{
  fts_atom_t d;

  if (fts_hash_table_lookup(&ispw_name_table, name, &d))
    return  (fts_object_t *) fts_get_object(&d);
  else
    return 0;
}

void 
ispw_register_named_object(fts_object_t *obj, fts_symbol_t name)
{
  fts_atom_t a;

  fts_set_object(&a, obj);
  fts_hash_table_insert(&ispw_name_table, name, &a);
}

void 
ispw_unregister_named_object(fts_object_t *obj, fts_symbol_t name)
{
  fts_hash_table_remove(&ispw_name_table, name);
}

fts_object_t *
ispw_get_target(fts_patcher_t *scope, fts_symbol_t name)
{
  fts_label_t *label = fts_label_get(scope, name);
  
  if(fts_label_is_connected(label))
    return (fts_object_t *)label;
  else
    return ispw_get_object_by_name(name);
}

void 
ispw_naming_init(void)
{
  fts_hash_table_init(&ispw_name_table);
}
