#include "sys.h"
#include "lang/mess.h"
#include "lang/utils.h"

/*
 * Basic naming services; for now, just provide global names,
 * later will provide a local naming service, too.
 * Now, it handle only named fts_object_t .
 */

static fts_hash_table_t global_name_table; /* the name binding table */

fts_object_t *
fts_get_object_by_name(fts_symbol_t name)
{
  void *d;

  if (fts_hash_table_lookup(&global_name_table, name, &d))
    return  (fts_object_t *) d;
  else
    return 0;
}


void
fts_register_named_object(fts_object_t *obj, fts_symbol_t name)
{
  fts_hash_table_insert(&global_name_table, name, (void *)obj);
}

void
fts_unregister_named_object(fts_object_t *obj, fts_symbol_t name)
{
  fts_hash_table_remove(&global_name_table, name);
}


void
fts_mess_naming_init(void)
{
  fts_hash_table_init(&global_name_table);
}


