#include "sys.h"
#include "lang/mess.h"
#include "lang/utils.h"

static fts_heap_t *object_set_cell_heap = 0;
static fts_heap_t *object_set_heap = 0;
static fts_heap_t *object_set_iterator_heap = 0;


static fts_object_set_cell_t *
fts_object_set_cell_new(fts_object_t *obj, fts_object_set_cell_t *next)
{
  fts_object_set_cell_t *cell;

  if (! object_set_cell_heap)
    object_set_cell_heap = fts_heap_new(sizeof(fts_object_set_cell_t), 64);

  cell = (fts_object_set_cell_t *) fts_heap_alloc(object_set_cell_heap);

  if (cell)
    {
      cell->object = obj;
      cell->next   = next;
    }

  return cell;
}


static void
fts_object_set_cell_free( fts_object_set_cell_t *cell)
{
  fts_heap_free((char *) cell, object_set_cell_heap);
}


void 
fts_object_set_init( fts_object_set_t *set)
{
  set->head = 0;
}


fts_object_set_t *
fts_object_set_new( void)
{
  fts_object_set_t *set;

  if (! object_set_heap)
    object_set_heap = fts_heap_new(sizeof( fts_object_set_t), 16);

  set = (fts_object_set_t *) fts_heap_alloc(object_set_heap);

  if (set)
    fts_object_set_init( set);

  return set;
}


void 
fts_object_set_destroy( fts_object_set_t *set)
{
  fts_object_set_cell_t *cell, *next;

  for( cell = set->head; cell; cell = next)
    {
      next = cell->next;
      fts_object_set_cell_free( cell);
    }

  set->head = 0;
}

void
fts_object_set_free(fts_object_set_t *set)
{
  fts_object_set_destroy( set);
  fts_heap_free((char *)set, object_set_heap);
}


/* set manipulation */

int fts_object_set_have_member(fts_object_set_t *set, fts_object_t *object)
{
  fts_object_set_cell_t *p;

  for (p = set->head; p; p = p->next)
    if (p->object == object)
      return 1;
  
  return 0;
}

void fts_object_set_add( fts_object_set_t *set, fts_object_t *object)
{
  if (fts_object_set_have_member(set, object))
    return;
  else
    set->head = fts_object_set_cell_new(object, set->head);
}


void fts_object_set_remove(fts_object_set_t *set, fts_object_t *object)
{
  fts_object_set_cell_t **p;	/* indirect precusor */

  for (p = &(set->head); *p; p = &((*p)->next))
    if ((* p)->object == object)
      {
	fts_object_set_cell_t *cell;

	cell = (*p);
	(*p) = cell->next;
	fts_object_set_cell_free(cell);

	return;
      }
}


void fts_object_set_send_message(fts_object_set_t *set, int winlet, fts_symbol_t sel,
				 int ac, const fts_atom_t *av)
{
  fts_object_set_cell_t *p;

  for (p = set->head; p; p = p->next)
    fts_send_message(p->object, winlet, sel, ac, av);
}

/* Iterators */

void 
fts_object_set_iterator_init( fts_object_set_iterator_t *iter, const fts_object_set_t *set)
{
  iter->cell = set->head;
}


fts_object_set_iterator_t *
fts_object_set_iterator_new(const fts_object_set_t *set)
{
  fts_object_set_iterator_t *iter;

  if (! object_set_iterator_heap)
    object_set_iterator_heap = fts_heap_new(sizeof( fts_object_set_iterator_t), 16);

  iter = (fts_object_set_iterator_t *) fts_heap_alloc(object_set_iterator_heap);

  if (iter)
    fts_object_set_iterator_init( iter, set);

  return iter;
}


void
fts_object_set_iterator_free(fts_object_set_iterator_t *iter)
{
  fts_heap_free((char *) iter, object_set_iterator_heap);
}


void 
fts_object_set_iterator_next(fts_object_set_iterator_t *iter)
{
  if ( fts_object_set_iterator_end( iter) )
    return;

  iter->cell = iter->cell->next;
}


int 
fts_object_set_iterator_end(const fts_object_set_iterator_t *iter)
{
  return iter->cell == 0;
}


fts_object_t *
fts_object_set_iterator_current(const fts_object_set_iterator_t *iter)
{
  return iter->cell->object;
}

