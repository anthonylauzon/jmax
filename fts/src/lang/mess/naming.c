/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#include "sys.h"
#include "lang/mess.h"
#include "lang/utils.h"

#include "lang/mess/messP.h"

/*
 * Basic naming services; for now, just provide global names,
 * Only for compatibility with the ISPW Object set; don't use for new objects.
 * Now, it handle only named fts_object_t .
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


