#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "sys.h"
#include "lang/mess.h"
#include "lang/mess/messP.h"
#include "lang/mess/ftsdata.h"
#include "lang/mess/ftsdataid.h"
/* (fd) I know, this is not correct. But it is needed ... */
#include "runtime.h"

#define FUNCTION_TABLE_SIZE 32

typedef struct _fts_data_fun_entry_t fts_data_fun_entry_t;

struct fts_data_fun_entry {
  int key;
  fts_data_fun_t fun;
};

struct fts_data_class {
  fts_symbol_t java_class_name;
  int n_functions;
  struct fts_data_fun_entry *functions_table;
};

static fts_data_t *meta_data = 0;

static void meta_data_init()
{
  meta_data = (fts_data_t *) malloc( sizeof( fts_data_t));
  assert( meta_data != 0);

  meta_data->class = 0;
  meta_data->id = 1;

  fts_data_id_put( 1, meta_data);
}

fts_data_class_t *fts_data_class_new( fts_symbol_t java_class_name)
{
  fts_data_class_t *class;

  class = (fts_data_class_t *)malloc( sizeof( fts_data_class_t));
  assert( class != 0);

  class->java_class_name = java_class_name;

  class->n_functions = 0;
  class->functions_table = (struct fts_data_fun_entry *)malloc( sizeof( struct fts_data_fun_entry) * FUNCTION_TABLE_SIZE);
  assert( class->functions_table != 0);

  return class;
}

void fts_data_class_define_function( fts_data_class_t *class, int key, fts_data_fun_t function)
{
  assert( class->n_functions < FUNCTION_TABLE_SIZE);

  class->functions_table[class->n_functions].key = key;
  class->functions_table[class->n_functions].fun = function;

  class->n_functions++;
}

static fts_data_fun_t fts_data_class_function_lookup( fts_data_class_t *class, int key)
{
  int i;

  for ( i = 0; i < class->n_functions; i++)
    if ( class->functions_table[i].key == key)
      return class->functions_table[i].fun;

  return 0;
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

  if ( meta_data == 0)
    meta_data_init();

  if ( d->id == NO_ID)
    {
      d->id = new_id();

      fts_data_id_put( d->id, d);

      fts_set_int( &(a[0]), d->id);
      fts_set_symbol( &(a[1]), d->class->java_class_name);
      fts_data_remote_call( meta_data, 1, 2, a);
    }
}

int fts_data_get_id( fts_data_t *d)
{
  return d->id;
}

void fts_data_call( fts_data_t *d, int key, int ac, fts_atom_t *at)
{
  fts_data_fun_t function;

  function = fts_data_class_function_lookup( d->class, key);

  if (!function)
    {
      fprintf( stderr, "%d: no function for this key\n", key);
      return;
    }

  (*function)( d, ac, at);
}

void fts_data_remote_call( fts_data_t *d, int key, int ac, fts_atom_t *at)
{
  fts_client_mess_start_msg( REMOTE_CALL_CODE);
  fts_client_mess_add_long( d->id);
  fts_client_mess_add_long( key);
  fts_client_mess_add_atoms( ac, at);
  fts_client_mess_send_msg();
}

