#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "sys.h"
#include "lang/mess.h"
#include "lang/mess/messP.h"
/* (fd) I know, this is not correct. But it is needed ... */
#include "runtime.h"


/*
 * (MDC): done some change: the function table is now directly
 * accessed; this means that the key must be <= FUNCTION_TABLE_SIZE
 * (assert changed); the function table is an array part of the
 * structure, then.
 *
 * A mechanism to handle data object content export here has been added; each
 * data_class have added field that is the class export function; this
 * function is called in the export, and should actually copy the
 * content of the data object to the client; it is called at most once for
 * every data object; this mechanism is needed
 * to separate the users of the fts_data from the implementation of
 * the fts_data; in some case, it is the kernel itself that decide to
 * export an fts_data.
 * A good question is why a separate function and not a function in the
 * function table: mostly because is not called remotely but locally;
 * it can be changed, possibly.
 *
 * Now fts_data_remote_call automatically export and upload the data if needed.
 * Also, the property update system automatically export a data that is the 
 * value of a property sent to the client.
 *
 *
 * An init function is explicitly called by the mess.c global init function.
 *
 * fts_malloc are used instead malloc.
 */


#define FUNCTION_TABLE_SIZE 32


struct fts_data_class {
  fts_symbol_t data_class_name;
  fts_data_export_fun_t export_fun;
  fts_data_fun_t  functions_table[FUNCTION_TABLE_SIZE];
};

static fts_data_t *meta_data = 0;

static void meta_data_init()
{
  meta_data = (fts_data_t *) fts_malloc( sizeof( fts_data_t));
  assert( meta_data != 0);

  meta_data->class = 0;
  meta_data->id = 1;

  fts_data_id_put( 1, meta_data);
}

fts_data_class_t *fts_data_class_new( fts_symbol_t data_class_name)
{
  int i;
  fts_data_class_t *class;

  class = (fts_data_class_t *)fts_malloc( sizeof( fts_data_class_t));
  assert( class != 0);

  class->data_class_name = data_class_name;

  for (i = 0; i < FUNCTION_TABLE_SIZE; i++)
    class->functions_table[i] = 0;

  return class;
}

void fts_data_class_define_export_function( fts_data_class_t *class, fts_data_export_fun_t export_fun)
{
  class->export_fun = export_fun;
}

void fts_data_class_define_function( fts_data_class_t *class, int key, fts_data_fun_t function)
{
  assert( key < FUNCTION_TABLE_SIZE);

  class->functions_table[key] = function;
}

static int new_id()
{
  static int count = 2;

  return count++;
}

#define NO_ID -1

void fts_data_init( fts_data_t *d, fts_data_class_t *class)
{
  d->class = class;
  d->id = NO_ID;
}

void fts_data_export( fts_data_t *d)
{
  fts_atom_t a[2];

  if ( d->id == NO_ID)
    {
      d->id = new_id();

      fts_data_id_put( d->id, d);

      fts_set_int( &(a[0]), d->id);
      fts_set_symbol( &(a[1]), d->class->data_class_name);
      fts_data_remote_call( meta_data, 1, 2, a);

      (* d->class->export_fun)(d);
    }
}


int fts_data_is_exported( fts_data_t *d)
{
  return d->id != NO_ID;
}

int fts_data_get_id( fts_data_t *d)
{
  return d->id;
}

void fts_data_call( fts_data_t *d, int key, int ac, const fts_atom_t *at)
{
  fts_data_fun_t function;

  function = d->class->functions_table[key];

  if (! function)
    {
      fprintf( stderr, "%d: no function for this key\n", key);
      return;
    }

  (*function)( d, ac, at);
}

void fts_data_remote_call( fts_data_t *d, int key, int ac, const fts_atom_t *at)
{
  if (! fts_data_is_exported(d))
    fts_data_export(d);

  fts_client_mess_start_msg( REMOTE_CALL_CODE);
  fts_client_mess_add_int( d->id);
  fts_client_mess_add_int( key);
  fts_client_mess_add_atoms( ac, at);
  fts_client_mess_send_msg();
}


/* A version where the arguments are sent directly by the caller;
   usefull to avoid allocation of big atom arrays.
   Of course, if somebody call fts_data_start_remote_call
   without calling fts_data_end_remote_call, the message
   and the next one are lost, but these functions are absolutely
   needed (see intvec.c for example).
   */

void fts_data_start_remote_call( fts_data_t *d, int key, int ac, fts_atom_t *at)
{
  if (! fts_data_is_exported(d))
    fts_data_export(d);

  fts_client_mess_start_msg( REMOTE_CALL_CODE);
  fts_client_mess_add_int( d->id);
  fts_client_mess_add_int( key);
}

void fts_data_end_remote_call()
{
  fts_client_mess_send_msg();
}


/* Initialize the fts data module */

void fts_data_module_init()
{
  meta_data_init();
}
