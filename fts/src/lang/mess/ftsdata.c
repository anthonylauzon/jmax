#include <assert.h>
#include "sys.h"
#include "lang/mess.h"
#include "lang/utils.h"
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

/* Meta data remote call keys */

#define REMOTE_NEW     1
#define REMOTE_DELETE  2
#define REMOTE_RELEASE 3

#define FUNCTION_TABLE_SIZE 32


struct fts_data_class {
  fts_symbol_t data_class_name;
  fts_data_export_fun_t export_fun;
  fts_data_remote_constructor_t remote_constructor;
  fts_data_remote_destructor_t remote_destructor;
  fts_data_fun_t  functions_table[FUNCTION_TABLE_SIZE];
};

static fts_data_t *meta_data = 0;

static fts_hash_table_t fts_data_class_table;

static void fts_data_remote_new(fts_data_t *d, int ac, const fts_atom_t *at);
static void fts_data_remote_delete(fts_data_t *d, int ac, const fts_atom_t *at);

static void meta_data_init()
{
  meta_data = (fts_data_t *) fts_malloc( sizeof( fts_data_t));
  assert( meta_data != 0);

  meta_data->class = fts_data_class_new(fts_new_symbol("_meta_data"));
  meta_data->id = 1;

  fts_data_id_put( 1, meta_data);
  fts_data_class_define_function(meta_data->class, REMOTE_NEW, fts_data_remote_new);
  fts_data_class_define_function(meta_data->class, REMOTE_DELETE, fts_data_remote_delete);
}

fts_data_class_t *fts_data_class_new( fts_symbol_t data_class_name)
{
  fts_atom_t data;
  fts_data_class_t *class;
  int i;

  if (fts_hash_table_lookup(&fts_data_class_table, data_class_name, &data))
    return (fts_data_class_t *) fts_get_ptr(&data);
  else
    {
      class = (fts_data_class_t *)fts_malloc( sizeof( fts_data_class_t));
      assert( class != 0);

      class->data_class_name = data_class_name;

      class->export_fun = 0;
      class->remote_constructor = 0;
      class->remote_destructor  = 0;

      for (i = 0; i < FUNCTION_TABLE_SIZE; i++)
	class->functions_table[i] = 0;

      fts_set_ptr(&data, class);
      fts_hash_table_insert(&fts_data_class_table, data_class_name, &data);

      return class;
    }
}

void fts_data_class_define_export_function( fts_data_class_t *class, fts_data_export_fun_t export_fun)
{
  class->export_fun = export_fun;
}


void fts_data_class_define_remote_constructor( fts_data_class_t *class,
					       fts_data_remote_constructor_t constructor)
{
  class->remote_constructor = constructor;
}


void fts_data_class_define_remote_destructor( fts_data_class_t *class,
					       fts_data_remote_destructor_t destructor)
{
  class->remote_destructor = destructor;
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

/* Local creation and deletion: when a fts data is created locally
   In FTS, it is created and destroyed by C functions, and not 
   by pubblic constructors/destructors; when is created and destroyed
   remotely, the constructors/destructors are used; this is tricky,
   the problem being the low level nature and use of fts data.
 */

void fts_data_init( fts_data_t *d, fts_data_class_t *class)
{
  d->class = class;
  d->id = NO_ID;
}

void fts_data_delete( fts_data_t *d)
{
  if (d->id != NO_ID)
    {
      fts_atom_t a;

      fts_data_id_remove(d->id, d);
      fts_set_data(&a, d);
      fts_data_remote_call( meta_data, REMOTE_RELEASE, 1, &a);
    }
}

/* Remote version, as remote function of the meta data object;
   the remote constructor should return a properly initialized
   fts_data_t (i.e. must call fts_data_init).
 */

static void fts_data_remote_new(fts_data_t *d, int ac, const fts_atom_t *at)
{
  fts_atom_t data;
  fts_data_class_t *class;
  fts_data_t *new;
  int id;
  fts_symbol_t data_class_name;

  if (ac < 2)
    return;			/* error */

  data_class_name = fts_get_symbol(&at[1]);
  id = fts_get_int(&at[0]);

  if (fts_hash_table_lookup(&fts_data_class_table, data_class_name, &data))
    class = (fts_data_class_t *) fts_get_ptr(&data);
  else
    return;			/* error */

  if (class->remote_constructor)
    {
      /* Automatically export the object back to the server */

      fts_atom_t a[2];

      new = class->remote_constructor(ac - 2, at + 2);
      new->id = id;

      fts_data_id_put(id, new);

      fts_set_int( &(a[0]), new->id);
      fts_set_symbol( &(a[1]), new->class->data_class_name);
      fts_data_remote_call( meta_data, REMOTE_NEW, 2, a);

      if (new->class->export_fun)
	(* new->class->export_fun)(new);
    }
  else
    return;			/* error */
}

/* Remote Delete is a remote function of the object itself;
   there is no remote release; the release is only meaningfull
   for the client. Note that the remote destructor, if existing,
   must call fts_data_delete.
 */

static void fts_data_remote_delete(fts_data_t *d, int ac, const fts_atom_t *at)
{
  if (d->id != NO_ID)
    {
      if (d->class->remote_destructor)
	(* d->class->remote_destructor)(d);
      else
	fts_data_id_remove(d->id, d);
    }
}

/* Exporting */

void fts_data_export( fts_data_t *d)
{
  if ( d->id == NO_ID)
    {
      fts_atom_t a[2];

      d->id = new_id();

      fts_data_id_put( d->id, d);

      fts_set_int( &(a[0]), d->id);
      fts_set_symbol( &(a[1]), d->class->data_class_name);
      fts_data_remote_call( meta_data, REMOTE_NEW, 2, a);

      if (d->class->export_fun)
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
  fts_client_mess_add_data(d);
  fts_client_mess_add_int(key);
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
  fts_client_mess_add_data(d);
  fts_client_mess_add_int(key);
}

void fts_data_end_remote_call()
{
  fts_client_mess_send_msg();
}


/* Initialize the fts data module */

void fts_data_module_init()
{
  fts_hash_table_init(&fts_data_class_table);
  meta_data_init();
}






