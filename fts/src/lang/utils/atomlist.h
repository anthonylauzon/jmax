#ifndef _ATOMLIST_H_
#define _ATOMLIST_H_

#define FTS_ATOM_LIST_CELL_MAX_ATOMS 5 /* was 16 */

typedef struct fts_atom_list_cell {
  int n;
  fts_atom_t atoms[FTS_ATOM_LIST_CELL_MAX_ATOMS];
  struct fts_atom_list_cell *next;
} fts_atom_list_cell;

extern void fts_atom_list_cell_init( fts_atom_list_cell *cell);
extern fts_atom_list_cell *fts_atom_list_cell_new( void );

extern void fts_atom_list_cell_destroy( fts_atom_list_cell *cell);
extern void fts_atom_list_cell_free( fts_atom_list_cell *cell);

typedef struct fts_atom_list
{
  fts_atom_list_cell *head, *tail;
  int changed;			/* changed flag for update, is the only update related field needed here */
} fts_atom_list_t;

extern void fts_atom_list_init( fts_atom_list_t *list);
extern fts_atom_list_t *fts_atom_list_new( void);

extern void fts_atom_list_destroy( fts_atom_list_t *list);
extern void fts_atom_list_free( fts_atom_list_t *list);

extern void fts_atom_list_append( fts_atom_list_t *list, int ac, const fts_atom_t *atom);
extern void fts_atom_list_set( fts_atom_list_t *list, int ac, const fts_atom_t *atom);

/* Iterators */


typedef struct fts_atom_list_iterator
{
  fts_atom_list_cell *cell;
  fts_atom_t *atom, *last;
} fts_atom_list_iterator_t;

extern void fts_atom_list_iterator_init( fts_atom_list_iterator_t *iter, const fts_atom_list_t *list);
extern fts_atom_list_iterator_t *fts_atom_list_iterator_new( const fts_atom_list_t *list);
extern void fts_atom_list_iterator_free(fts_atom_list_iterator_t *iter);

extern void fts_atom_list_iterator_next( fts_atom_list_iterator_t *iter);
extern int fts_atom_list_iterator_end( const fts_atom_list_iterator_t *iter);
extern fts_atom_t *fts_atom_list_iterator_current( const fts_atom_list_iterator_t *iter);

#endif
