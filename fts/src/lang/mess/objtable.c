/* New version of the objtable.

   This version imply that the object IDs are assigned
   consecutively, starting from 1; any other client behaviour
   will eat enourmous amount of memory, and possibly will
   just kill the machine.

   Only one client is supported.

   The data structure is direct access table, adaptively resized 
   when needed.

   The 0 id map by definition to the NULL pointer.
   */

#include "sys.h"
#include "lang/mess.h"

#define TABLE_INIT_SIZE (16*1024)
#define TABLE_GROW  2

static int table_size = 0;
static fts_object_t **object_table = 0;

void
fts_object_table_put(int id, fts_object_t *obj)
{
  if (id <= 0)
    return;

  if (table_size == 0)
    {
      int i;

      object_table = fts_malloc(TABLE_INIT_SIZE * sizeof(fts_object_t *));
      table_size = TABLE_INIT_SIZE;

      for (i = 0; i < table_size; i ++)
	object_table[i] = 0;
    }
  else if (id >= table_size)
    {
      int i;
      int new_size;
      
      new_size = table_size;

      /* grow exponentially the table to avoid thousands of realloc;
	 this scheme produce ten realloc with 4 Mega objects :->
       */

      while (new_size < id)
	new_size *= TABLE_GROW;

      object_table = fts_realloc(object_table, new_size * sizeof(fts_object_t *));

      for (i = table_size; i < new_size; i ++)
	object_table[i] = 0;

      table_size = new_size;
    }

  object_table[id] = obj;
}


void
fts_object_table_remove(int id)
{
  if (id < table_size)
    object_table[id] = 0;
}


fts_object_t *
fts_object_table_get(int id)
{
  if (id == 0)
    return 0;
  else if (id < table_size)
    return object_table[id];
  else
    return 0;
}

/* for  restart */

void
fts_object_table_delete_all(void)
{
  int i;
  
  /* first slot of the table unused */

  for (i = 1; i < table_size; i++)
    if (object_table[i])
      fts_object_delete(object_table[i]);
}
