#ifndef _OBJECTSET_H_
#define _OBJECTSET_H_

struct fts_objectset_cell;
typedef struct fts_objectset_cell fts_objectset_cell_t;

struct fts_objectset;
typedef struct fts_objectset fts_objectset_t;

extern fts_objectset_t *fts_objectset_new(void);
extern void fts_objectset_delete(fts_objectset_t *set);

extern void fts_objectset_add(fts_objectset_t *set, fts_object_t *object);
extern void fts_objectset_remove(fts_objectset_t *set, fts_object_t *object);

extern int fts_objectset_have_member(fts_objectset_t *set, fts_object_t *object);

extern void fts_objectset_send_message(fts_objectset_t *set, int winlet, fts_symbol_t sel,
					int ac, const fts_atom_t *av);

/* Iterators */

struct fts_objectset_iterator;
typedef struct fts_objectset_iterator fts_objectset_iterator_t;

extern void fts_objectset_iterator_init( fts_objectset_iterator_t *iter, const fts_objectset_t *set);
extern fts_objectset_iterator_t *fts_objectset_iterator_new( const fts_objectset_t *set);
extern void fts_objectset_iterator_free(fts_objectset_iterator_t *iter);

extern void fts_objectset_iterator_next( fts_objectset_iterator_t *iter);
extern int fts_objectset_iterator_end( const fts_objectset_iterator_t *iter);
extern fts_object_t *fts_objectset_iterator_current( const fts_objectset_iterator_t *iter);


#endif
