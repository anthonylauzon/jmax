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
#include <assert.h>
#include "sys.h"
#include "lang/mess.h"
#include "lang/utils.h"
#include "lang/datalib.h"
#include "lang/mess/messP.h"

/* (fd) I know, this is not correct. But it is needed ... */
#include "runtime.h"

/* (nos):
   Added a pseudo generic "new" function for fts_data.
   In order not to mess up too much I just did it as this hack:
     if(class_name == ...) call the class' constructor 
   (Should there be a difference between "remote" constructors and others??)

   See down!
 */

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

/**************************************************************************
 *
 *   FTS data "meta"
 *
 */

/* Meta data remote call keys */
#define REMOTE_NEW     1
#define REMOTE_DELETE  2
#define REMOTE_RELEASE 3

static fts_data_t *meta_data = 0;
static fts_hash_table_t fts_data_class_table;

static void fts_data_remote_new(fts_data_t *data, int ac, const fts_atom_t *at);
static void fts_data_remote_delete(fts_data_t *data, int ac, const fts_atom_t *at);

static void 
meta_data_init(void)
{
  meta_data = (fts_data_t *) fts_malloc( sizeof( fts_data_t));
  assert(meta_data != 0);

  meta_data->class = fts_data_class_new(fts_new_symbol("_meta_data"));
  meta_data->id = 1;

  fts_data_id_put( 1, meta_data);
  fts_data_class_define_function(meta_data->class, REMOTE_NEW, fts_data_remote_new);
  fts_data_class_define_function(meta_data->class, REMOTE_DELETE, fts_data_remote_delete);
}

/* Initialize the fts data module */
void fts_data_module_init()
{
  fts_hash_table_init(&fts_data_class_table);
  meta_data_init();
}

/**************************************************************************
 *
 *   FTS data classes
 *
 */

struct fts_data_class {
  fts_symbol_t data_class_name;
  fts_data_export_fun_t export_fun;
  fts_data_remote_constructor_t remote_constructor;
  fts_data_remote_destructor_t remote_destructor;
  fts_data_fun_t functions_table[FUNCTION_TABLE_SIZE];
};

fts_data_class_t *
fts_data_class_new( fts_symbol_t data_class_name)
{
  fts_atom_t atom;
  fts_data_class_t *class;
  int i;

  if (fts_hash_table_lookup(&fts_data_class_table, data_class_name, &atom))
    return (fts_data_class_t *) fts_get_ptr(&atom);
  else
    {
      class = (fts_data_class_t *)fts_malloc( sizeof( fts_data_class_t));
      assert( class != 0);

      class->data_class_name = data_class_name;

      class->export_fun = 0;
      class->remote_constructor = 0;
      class->remote_destructor = 0;

      for (i = 0; i < FUNCTION_TABLE_SIZE; i++)
	class->functions_table[i] = 0;

      fts_set_ptr(&atom, class);
      fts_hash_table_insert(&fts_data_class_table, data_class_name, &atom);

      return class;
    }
}

void fts_data_class_define_export_function( fts_data_class_t *class, fts_data_export_fun_t export_fun)
{
  class->export_fun = export_fun;
}

void fts_data_class_define_remote_constructor( fts_data_class_t *class, fts_data_remote_constructor_t constructor)
{
  class->remote_constructor = constructor;
}

void fts_data_class_define_remote_destructor( fts_data_class_t *class, fts_data_remote_destructor_t destructor)
{
  class->remote_destructor = destructor;
}

void fts_data_class_define_function( fts_data_class_t *class, int key, fts_data_fun_t function)
{
  assert( key < FUNCTION_TABLE_SIZE);

  class->functions_table[key] = function;
}

fts_symbol_t 
fts_data_get_class_name(fts_data_t *data)
{
  return data->class->data_class_name;
}

int
fts_data_is(fts_data_t *data, fts_symbol_t class_name)
{
  return (data->class->data_class_name == class_name);
}

int 
fts_data_is_class_name(fts_symbol_t class_name)
{
  fts_atom_t atom;
  return (fts_hash_table_lookup(&fts_data_class_table, class_name, &atom));
}

/**************************************************************************
 *
 *   FTS data data
 *
 */

#define NO_ID -1

static int new_id(void)
{
  /* First allocated ID is two */

  static int count = 0;

  count = count + 2;
  return count;
}

/* Local creation and deletion: when a fts data is created locally
   In FTS, it is created and destroyed by C functions, and not 
   by pubblic constructors/destructors; when is created and destroyed
   remotely, the constructors/destructors are used; this is tricky,
   the problem being the low level nature and use of fts data.
 */

/* (nos:) pseudo generic "new" hack, implemented for: */

extern fts_data_t *fts_integer_vector_constructor(int ac, const fts_atom_t *at);
extern fts_data_t *fts_float_vector_constructor(int ac, const fts_atom_t *at);
extern fts_data_t *fts_atom_array_constructor(int ac, const fts_atom_t *at);

fts_data_t *
fts_data_new(fts_symbol_t class_name, int ac, const fts_atom_t *at)
{
  fts_atom_t atom;
  fts_data_t *data = 0;

  if(class_name == fts_s_integer_vector)
    data = fts_integer_vector_constructor(ac, at);
  else if(class_name == fts_s_float_vector)
    data = fts_float_vector_constructor(ac, at);
  else if(class_name == fts_s_atom_array)
    data = fts_atom_array_constructor(ac, at);

  return data;
}

fts_data_t *
fts_data_new_const(fts_symbol_t class_name, int ac, const fts_atom_t *at)
{
  fts_data_t *data = fts_data_new(class_name, ac, at);

  /* overwrite constant flag set in fts_data_init */
  data->cnst = 1;
  
  return data;
}

/* add reference */
void 
fts_data_refer(fts_data_t *data)
{
  data->refcnt++;
}

/* release reference and (if) deconstruct data */
int 
fts_data_release(fts_data_t *data)
{
  if(!--data->refcnt)
    {
      if(data->class == fts_integer_vector_data_class)
	fts_integer_vector_delete((fts_integer_vector_t *)data);
      else if(data->class == fts_float_vector_data_class)
	fts_float_vector_delete((fts_float_vector_t *)data);
      else if(data->class == fts_atom_array_data_class)
	fts_atom_array_delete((fts_atom_array_t *)data);

      return 0;
    }

  return data->refcnt;
}

/* get data size in # of atoms */
int 
fts_data_get_size(fts_data_t *data)
{
  if(data->class == fts_integer_vector_data_class)
    return ((fts_integer_vector_t *)data)->size;
  else if(data->class == fts_integer_vector_data_class)
    return ((fts_float_vector_t *)data)->size;
  else if(data->class == fts_atom_array_data_class)
    return ((fts_atom_array_t *)data)->size;

  return 0;
}

/* get data as array of atoms (takes pointer to pre-allocated (!) array) and
   return original size (might be bigger or smaller than pre-allocated array with ac) */
int
fts_data_get_atoms(fts_data_t *data, int ac, fts_atom_t *at)
{
  if(data->class == fts_integer_vector_data_class)
    return fts_integer_vector_get_atoms((fts_integer_vector_t *)data, ac, at);
  else if(data->class == fts_float_vector_data_class)
    return fts_float_vector_get_atoms((fts_float_vector_t *)data, ac, at);
  else if(data->class == fts_atom_array_data_class)
    return fts_atom_array_get_atoms((fts_atom_array_t *)data, ac, at);

  return 0;
}

void
fts_method_define_data(fts_class_t *class, int winlet, fts_method_t fun)
{
  fts_method_define_optargs(class, winlet, fts_s_integer_vector, fun, 1, &fts_s_data, 1);
  fts_method_define_optargs(class, winlet, fts_s_float_vector, fun, 1, &fts_s_data, 1);
  fts_method_define_optargs(class, winlet, fts_s_atom_array, fun, 1, &fts_s_data, 1);
}

void
fts_outlet_data(fts_object_t *o, int woutlet, fts_data_t *data)
{
  fts_connection_t *conn;
  fts_atom_t atom;

  fts_set_data(&atom, data);

  conn = o->out_conn[woutlet];

  while(conn)
    {
      fts_send_message(conn->dst, conn->winlet, fts_data_get_class_name(data), 1, &atom); 

      conn = conn->next_same_src;
    }
}

/*******************************************************************
 *
 *  (nos): "old" stuff
 *
 */ 

void fts_data_init(fts_data_t *data, fts_data_class_t *class)
{
  data->name = 0;
  data->class = class;
  data->id = NO_ID;
  data->refcnt = 0; /* no reference (yet) */
  data->cnst = 0; /* not constant */
}

void fts_data_delete(fts_data_t *data)
{
  if (data->id != NO_ID)
    {
      fts_atom_t a;

      fts_data_id_remove(data->id, data);
      fts_set_data(&a, data);
      fts_data_remote_call( meta_data, REMOTE_RELEASE, 1, &a);
    }
}

/* Remote version, as remote function of the meta data object;
   the remote constructor should return a properly initialized
   fts_data_t (i.e. must call fts_data_init).
 */
static void fts_data_remote_new(fts_data_t *data, int ac, const fts_atom_t *at)
{
  fts_atom_t atom;
  fts_data_class_t *class;
  fts_data_t *new;
  int id;
  fts_symbol_t data_class_name;

  if (ac < 2)
    return;			/* error */

  data_class_name = fts_get_symbol(&at[1]);
  id = fts_get_int(&at[0]);

  if (fts_hash_table_lookup(&fts_data_class_table, data_class_name, &atom))
    class = (fts_data_class_t *) fts_get_ptr(&atom);
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

static void fts_data_remote_delete(fts_data_t *data, int ac, const fts_atom_t *at)
{
  if (data->id != NO_ID)
    {
      if (data->class->remote_destructor)
	(* data->class->remote_destructor)(data);
      else
	fts_data_id_remove(data->id, data);
    }
}

/* Exporting */
void fts_data_export( fts_data_t *data)
{
  if ( data->id == NO_ID)
    {
      fts_atom_t a[2];

      data->id = new_id();

      fts_data_id_put( data->id, data);

      fts_set_int( &(a[0]), data->id);
      fts_set_symbol( &(a[1]), data->class->data_class_name);
      fts_data_remote_call( meta_data, REMOTE_NEW, 2, a);

      if (data->class->export_fun)
	(* data->class->export_fun)(data);
    }
}

int fts_data_is_exported( fts_data_t *data)
{
  return data->id != NO_ID;
}

int fts_data_get_id( fts_data_t *data)
{
  return data->id;
}

void fts_data_call( fts_data_t *data, int key, int ac, const fts_atom_t *at)
{
  fts_data_fun_t function;

  function = data->class->functions_table[key];

  if (! function)
    {
      fprintf( stderr, "%d: no function for this key, data", key);
      fprintf_data( stderr, data);
      fprintf( stderr, "\n");
      return;
    }

  (*function)( data, ac, at);
}


void fts_data_remote_call( fts_data_t *data, int key, int ac, const fts_atom_t *at)
{
  if (! fts_data_is_exported(data))
    fts_data_export(data);

  fts_client_mess_start_msg( REMOTE_CALL_CODE);
  fts_client_mess_add_data(data);
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

void fts_data_start_remote_call( fts_data_t *data, int key)
{
  if (! fts_data_is_exported(data))
    fts_data_export(data);

  fts_client_mess_start_msg( REMOTE_CALL_CODE);
  fts_client_mess_add_data(data);
  fts_client_mess_add_int(key);
}

void fts_data_end_remote_call()
{
  fts_client_mess_send_msg();
}

/* Debug and printout functions */
void fprintf_data(FILE *f, fts_data_t *data)
{
  if (! data)
    {
      fprintf(f, "<NULL DATA>");
    }
  else if (data->class)
    {
      fprintf(f, "<:%s #%lx(%d):>", fts_symbol_name(data->class->data_class_name),
	      (unsigned long) data, (unsigned int) data->id);
    }
  else
    fprintf(f, "<: Unconsistent data #%lx :>", (unsigned long) data);
  
}
