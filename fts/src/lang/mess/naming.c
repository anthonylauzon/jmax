#include "sys.h"
#include "lang/mess.h"
#include "lang/utils.h"

/*
 * Basic naming services; for now, just provide global names,
 * later will provide a local naming service, too.
 * Now, it handle only named fts_object_t .
 */

static fts_hash_table_t global_name_table; /* the name binding table */

static char string_buffer[1024]; /* string buffer */

static int
expand_patcher(fts_patcher_t *p, char *s)
{
  if(p)
    {
      if(expand_patcher(p->o.patcher, s))
	strcat(s, ".");

      if(p->name == fts_new_symbol("unnamed"))
	strcat(s, "[patcher]");
      else
	strcat(s, fts_symbol_name(p->name));

      return 1;
    }
  else
    return 0;
}

char *
fts_patcher_get_full_name_string(fts_patcher_t *p)
{
  string_buffer[0] = '\0';
  expand_patcher(p, string_buffer);

  return string_buffer;
}

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


