#ifndef _OBJECTSET_H_
#define _OBJECTSET_H_


typedef struct fts_object_set_cell
{
  fts_object_t *object;
  struct fts_object_set_cell *next;
} fts_object_set_cell_t;

typedef struct fts_object_set
{
  fts_object_set_cell_t *head;
}  fts_object_set_t;

extern void fts_object_set_init(fts_object_set_t *set);
extern fts_object_set_t *fts_object_set_new(void);

extern void fts_object_set_destroy(fts_object_set_t *set);
extern void fts_object_set_free(fts_object_set_t *set);

extern void fts_object_set_add(fts_object_set_t *set, fts_object_t *object);
extern void fts_object_set_remove(fts_object_set_t *set, fts_object_t *object);

extern int fts_object_set_have_member(fts_object_set_t *set, fts_object_t *object);

extern void fts_object_set_send_message(fts_object_set_t *set, int winlet, fts_symbol_t sel,
					int ac, const fts_atom_t *av);

/* Iterators */


typedef struct fts_object_set_iterator
{
  fts_object_set_cell_t *cell;
} fts_object_set_iterator_t;

extern void fts_object_set_iterator_init( fts_object_set_iterator_t *iter, const fts_object_set_t *set);
extern fts_object_set_iterator_t *fts_object_set_iterator_new( const fts_object_set_t *set);
extern void fts_object_set_iterator_free(fts_object_set_iterator_t *iter);

extern void fts_object_set_iterator_next( fts_object_set_iterator_t *iter);
extern int fts_object_set_iterator_end( const fts_object_set_iterator_t *iter);
extern fts_object_t *fts_object_set_iterator_current( const fts_object_set_iterator_t *iter);


#endif
