#include "sys.h"
#include "lang/mess.h"
#include "lang/mess/messP.h"


static fts_heap_t atom_array_heap;

fts_atom_array_t *fts_atom_array_new(int ac)
{
  fts_atom_array_t *v;

  v = (fts_atom_array_t *) fts_heap_alloc(&atom_array_heap);
  v->ac = ac;
  v->at = (fts_atom_t *)fts_block_zalloc(ac * sizeof(fts_atom_t));

  return v;
}

fts_atom_array_t *fts_atom_array_new_fill(int ac, const fts_atom_t *at)
{
  int i;
  fts_atom_array_t *v;

  v = (fts_atom_array_t *) fts_heap_alloc(&atom_array_heap);
  v->ac = ac;
  v->at = (fts_atom_t *)fts_block_zalloc(ac * sizeof(fts_atom_t));

  for (i = 0; i < ac; i++)
    v->at[i] = at[i];

  return v;
}


void fts_atom_array_free(fts_atom_array_t *v)
{
  fts_block_free((char *)v->at, v->ac * sizeof(fts_atom_t));
  fts_heap_free((char *) v, &atom_array_heap);
}

void fts_atom_array_init()
{
  fts_heap_init(&atom_array_heap, sizeof(fts_atom_array_t), 64);
}





