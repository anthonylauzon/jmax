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

#include "sys.h"
#include "lang/mess.h"
#include "lang/utils.h"

#include "lang/mess/messP.h"

/*
 * Basic naming services; for now, just provide global names,
 * Only for compatibility with the ISPW Object set; don't use for new objects.
 * It handle only named fts_object_t; used by the message box to send messages
 * to tables and table~s
 */

static fts_hash_table_t global_name_table; /* the name binding table */

fts_object_t *fts_get_object_by_name(fts_symbol_t name)
{
  fts_atom_t d;

  if (fts_hash_table_lookup(&global_name_table, name, &d))
    return  (fts_object_t *) fts_get_object(&d);
  else
    return 0;
}


void fts_register_named_object(fts_object_t *obj, fts_symbol_t name)
{
  fts_atom_t a;

  fts_set_object(&a, obj);
  fts_hash_table_insert(&global_name_table, name, &a);
}

void fts_unregister_named_object(fts_object_t *obj, fts_symbol_t name)
{
  fts_hash_table_remove(&global_name_table, name);
}


/* Return true (!= 0) if the named argument correspond to a receive or
   to a named object */

int fts_named_object_exists(fts_symbol_t name)
{
  if (fts_receive_exists(name))
    return 1;
  else
    {
      fts_atom_t d;

      if (fts_hash_table_lookup(&global_name_table, name, &d))
	return  1;
      else
	return 0;
    }
}

/* Utility to send a message to a named object;
   first, check if there is a receive with the good name,
   and in that case send to it; otherwise, send to the named
   object; messages are always sent to inlet zero.
   */


void fts_named_object_send(fts_symbol_t name, fts_symbol_t s, int argc, const fts_atom_t *argv)
{
  if (fts_receive_exists(name))
    fts_send_message_to_receives(name, s, argc, argv);
  else
    {
      fts_atom_t d;

      if (fts_hash_table_lookup(&global_name_table, name, &d))
	fts_message_send((fts_object_t *) fts_get_object(&d), 0,  s, argc, argv);
    }
}

void fts_mess_naming_init(void)
{
  fts_hash_table_init(&global_name_table);
}


