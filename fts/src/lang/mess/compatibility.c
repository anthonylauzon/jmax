#include "sys.h"
#include "lang/mess.h"
#include "lang/utils.h"

/* Compatibility support for Max 0.26 objects in dynamic shared library.
   For now, it implement access to receive object by name, and store
   the receive table; probabily go away with new naming services.
   */

static fts_hash_table_t through_table; /* the name binding table */

fts_object_t *
fts_get_receive_by_name(fts_symbol_t name)
{
  void *d;

  if (fts_hash_table_lookup(&through_table, name, &d))
    return  (fts_object_t *) d;
  else
    return 0;
}


void
fts_set_receive_by_name(fts_symbol_t name, fts_object_t *obj)
{
  fts_hash_table_insert(&through_table, name, (void *)obj);
}

void
fts_remove_receive_by_name(fts_symbol_t name)
{
  fts_hash_table_remove(&through_table, name);
}


void
fts_mess_compatibility_init(void)
{
  fts_hash_table_init(&through_table);
}
