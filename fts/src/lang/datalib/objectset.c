#include <assert.h>
#include "sys.h"
#include "lang/mess.h"
#include "lang/datalib.h"

/* Implementation of an object set as FTS Data; note that for the moment
   the implementation is very limited, and include a *readonly* java part.
   */

extern void fts_variable_find_users(fts_patcher_t *scope, fts_symbol_t name, fts_objectset_t *set);

/* Structure definitions */

static fts_data_class_t *fts_objectset_data_class = 0;

struct fts_objectset_cell
{
  fts_object_t *object;
  struct fts_objectset_cell *next;
};

struct fts_objectset
{
  fts_data_t dataobj;
  fts_objectset_cell_t *head;
};

struct fts_objectset_iterator
{
  fts_objectset_cell_t *cell;
};

static fts_heap_t *objectset_cell_heap;
static fts_heap_t *objectset_heap;
static fts_heap_t *objectset_iterator_heap;

/* Remote call codes */

#define REMOTE_CLEAN        1
#define REMOTE_APPEND       2
#define REMOTE_REMOVE_ONE   3
#define REMOTE_FIND         4
#define REMOTE_FIND_ERRORS  5
#define REMOTE_FIND_FRIENDS 6


/********************************************************************/
/*                                                                  */
/*            OBJECT SET                                            */
/*                                                                  */
/********************************************************************/

static fts_objectset_cell_t *
fts_objectset_cell_new(fts_object_t *obj, fts_objectset_cell_t *next)
{
  fts_objectset_cell_t *cell;

  cell = (fts_objectset_cell_t *) fts_heap_alloc(objectset_cell_heap);

  if (cell)
    {
      cell->object = obj;
      cell->next   = next;
    }

  return cell;
}


static void
fts_objectset_cell_free( fts_objectset_cell_t *cell)
{
  fts_heap_free((char *) cell, objectset_cell_heap);
}



fts_objectset_t *
fts_objectset_new( void)
{
  fts_objectset_t *set;

  set = (fts_objectset_t *) fts_heap_alloc(objectset_heap);
  assert( set != 0);
  set->head = 0;
  fts_data_init((fts_data_t *)set, fts_objectset_data_class);

  return set;
}


void
fts_objectset_delete(fts_objectset_t *set)
{
  fts_objectset_cell_t *cell, *next;

  fts_data_delete((fts_data_t *) set);

  for( cell = set->head; cell; cell = next)
    {
      next = cell->next;
      fts_objectset_cell_free( cell);
    }

  fts_heap_free((char *)set, objectset_heap);
}


/* set manipulation */

int fts_objectset_have_member(fts_objectset_t *set, fts_object_t *object)
{
  fts_objectset_cell_t *p;

  for (p = set->head; p; p = p->next)
    if (p->object == object)
      return 1;
  
  return 0;
}

void fts_objectset_add( fts_objectset_t *set, fts_object_t *object)
{
  if (fts_objectset_have_member(set, object))
    return;
  else
    {
      if (fts_data_is_exported((fts_data_t *) set))
	{
	  fts_atom_t a;

	  if (object->id == FTS_NO_ID)
	    fts_client_upload_object(object);

	  fts_set_object(&a, object);
	  fts_data_remote_call((fts_data_t *) set, REMOTE_APPEND, 1, &a);
	}
	    
      set->head = fts_objectset_cell_new(object, set->head);
    }
}


void fts_objectset_remove(fts_objectset_t *set, fts_object_t *object)
{
  fts_objectset_cell_t **p;	/* indirect precusor */

  for (p = &(set->head); *p; p = &((*p)->next))
    if ((* p)->object == object)
      {
	fts_objectset_cell_t *cell;

	cell = (*p);
	(*p) = cell->next;
	fts_objectset_cell_free(cell);

	/* remote remove */

	if (fts_data_is_exported((fts_data_t *) set))
	  {
	    fts_atom_t a;

	    fts_set_object(&a, object);
	    fts_data_remote_call((fts_data_t *) set, REMOTE_REMOVE_ONE, 1, &a);
	  }
	    
	return;
      }
}

void fts_objectset_remove_all(fts_objectset_t *set)
{
  fts_objectset_cell_t *p;

  if (fts_data_is_exported((fts_data_t *) set))
    fts_data_remote_call((fts_data_t *) set, REMOTE_CLEAN, 0, 0);

  p = set->head;
  while (p)
    {
      fts_objectset_cell_t *cell;

      cell = p;
      p = p->next;
      fts_objectset_cell_free(cell);
    }
  set->head = 0;
}

void fts_objectset_send_message(fts_objectset_t *set, int winlet, fts_symbol_t sel,
				 int ac, const fts_atom_t *av)
{
  fts_objectset_cell_t *p;

  for (p = set->head; p; p = p->next)
    fts_send_message(p->object, winlet, sel, ac, av);
}


/********************************************************************/
/*                                                                  */
/*            OBJECT SET ITERATOR                                   */
/*                                                                  */
/********************************************************************/

/* Iterators */

void 
fts_objectset_iterator_init( fts_objectset_iterator_t *iter, const fts_objectset_t *set)
{
  iter->cell = set->head;
}


fts_objectset_iterator_t *
fts_objectset_iterator_new(const fts_objectset_t *set)
{
  fts_objectset_iterator_t *iter;

  iter = (fts_objectset_iterator_t *) fts_heap_alloc(objectset_iterator_heap);

  if (iter)
    fts_objectset_iterator_init( iter, set);

  return iter;
}


void
fts_objectset_iterator_free(fts_objectset_iterator_t *iter)
{
  fts_heap_free((char *) iter, objectset_iterator_heap);
}


void 
fts_objectset_iterator_next(fts_objectset_iterator_t *iter)
{
  if ( fts_objectset_iterator_end( iter) )
    return;

  iter->cell = iter->cell->next;
}


int 
fts_objectset_iterator_end(const fts_objectset_iterator_t *iter)
{
  return iter->cell == 0;
}


fts_object_t *
fts_objectset_iterator_current(const fts_objectset_iterator_t *iter)
{
  return iter->cell->object;
}

/********************************************************************/
/*                                                                  */
/*            FTS_DATA functions on Object sets                     */
/*                                                                  */
/********************************************************************/

/* utilities */

static void fts_objectset_upload_objects(fts_objectset_t *set)
{
  fts_objectset_cell_t *p;

  for (p = set->head; p; p = p->next)
    if (p->object->id == FTS_NO_ID)
      fts_client_upload_object(p->object);
}


/* Just a very limited version for now */


static fts_data_t *fts_objectset_remote_constructor(int ac, const fts_atom_t *at)
{
  return (fts_data_t *) fts_objectset_new();
}


static void fts_objectset_remote_destructor(fts_data_t *d)
{
  fts_objectset_t *this = (fts_objectset_t *)d;

  fts_objectset_delete(this);
}


/*
 * The export function
 */


static void fts_objectset_export_fun(fts_data_t *d)
{
  fts_objectset_t *this = (fts_objectset_t *)d;
  fts_objectset_cell_t *p;

  fts_objectset_upload_objects(this);

  fts_data_export(d);

  fts_data_remote_call(d, REMOTE_CLEAN, 0, 0);

  fts_data_start_remote_call(d, REMOTE_APPEND, 0, 0);

  for (p = this->head; p; p = p->next)
    fts_client_mess_add_object(p->object);

  fts_data_end_remote_call();
}

static void fts_objectset_find( fts_data_t *d, int ac, const fts_atom_t *at)
{
  fts_objectset_t *this = (fts_objectset_t *)d;
  fts_object_t *scope = fts_get_object(at);
  fts_atom_t a[256];
  int i;
  
  fts_objectset_remove_all(this);

  fts_set_data(&a[0], (fts_data_t *) this);

  for (i = 1; (i < ac) && (i < 256); i++)
    a[i] = at[i];

  fts_send_message(scope, fts_SystemInlet, fts_s_find, ac, a);
}

static void fts_objectset_find_errors( fts_data_t *d, int ac, const fts_atom_t *at)
{
  fts_objectset_t *this = (fts_objectset_t *)d;
  fts_object_t *scope = fts_get_object(at);
  fts_atom_t a[1];

  fts_objectset_remove_all(this);

  fts_set_data(&a[0], (fts_data_t *) this);
  fts_send_message(scope, fts_SystemInlet, fts_s_find_errors, 1, a);
}

static void fts_objectset_find_friends( fts_data_t *d, int ac, const fts_atom_t *at)
{
  fts_objectset_t *this = (fts_objectset_t *)d;
  fts_object_t *target = fts_get_object(at);

  fts_objectset_remove_all(this);

  if (fts_object_get_variable(target))
    {
      fprintf(stderr, "Looking for friends of variable %s\n", fts_symbol_name(fts_object_get_variable(target)));
      fts_variable_find_users(target->patcher, fts_object_get_variable(target), this);
    }
  else
    {
      fts_atom_t a[1];

      fprintf(stderr, "Looking for standard friends\n");

      fts_set_data(&a[0], (fts_data_t *) this);
      fts_send_message(target, fts_SystemInlet, fts_s_find_friends, 1, a);
    }
}


/********************************************************************/
/*                                                                  */
/*            INIT_DATA functions on integer vectors                */
/*                                                                  */
/********************************************************************/

void fts_objectset_config(void)
{
  objectset_cell_heap = fts_heap_new(sizeof(fts_objectset_cell_t));
  objectset_heap = fts_heap_new(sizeof( fts_objectset_t));
  objectset_iterator_heap = fts_heap_new(sizeof( fts_objectset_iterator_t));

  fts_objectset_data_class = fts_data_class_new( fts_new_symbol( "objectset_data"));
  fts_data_class_define_export_function(fts_objectset_data_class, fts_objectset_export_fun);

  fts_data_class_define_remote_constructor(fts_objectset_data_class, fts_objectset_remote_constructor);
  fts_data_class_define_remote_destructor(fts_objectset_data_class, fts_objectset_remote_destructor);

  fts_data_class_define_function(fts_objectset_data_class, REMOTE_FIND, fts_objectset_find);
  fts_data_class_define_function(fts_objectset_data_class, REMOTE_FIND_ERRORS, fts_objectset_find_errors);
  fts_data_class_define_function(fts_objectset_data_class, REMOTE_FIND_FRIENDS, fts_objectset_find_friends);
}
