#include "sys.h"
#include "lang/mess.h"
#include "lang/mess/messP.h"


static fts_heap_t *atom_array_heap;

fts_atom_array_t *fts_atom_array_new(int ac)
{
  fts_atom_array_t *v;

  v = (fts_atom_array_t *) fts_heap_alloc(atom_array_heap);
  v->ac = ac;

  if (ac > 0)
    v->at = (fts_atom_t *)fts_block_zalloc(ac * sizeof(fts_atom_t));
  else
    v->at = 0;

  return v;
}

fts_atom_array_t *fts_atom_array_new_fill(int ac, const fts_atom_t *at)
{
  int i;
  fts_atom_array_t *v;

  v = (fts_atom_array_t *) fts_heap_alloc(atom_array_heap);

  v->ac = ac;

  if (ac > 0)
    v->at = (fts_atom_t *)fts_block_zalloc(ac * sizeof(fts_atom_t));
  else
    v->at = 0;

  for (i = 0; i < ac; i++)
    v->at[i] = at[i];

  return v;
}


void fts_atom_array_free(fts_atom_array_t *v)
{
  if (v->at)
    fts_block_free((char *)v->at, v->ac * sizeof(fts_atom_t));

  fts_heap_free((char *) v, atom_array_heap);
}

void fprintf_atom_array(FILE *file, fts_atom_array_t *v)
{
  int i;

  fprintf(file, "<{");
  for (i = 0; i < v->ac ; i++)
    {
      fprintf_atoms(file, 1, &(v->at[i]));
      fprintf(file, ",");
    }

  fprintf(file, "} %lx>",(unsigned long) v);
}

void fts_atom_array_init(void)
{
  atom_array_heap = fts_heap_new(sizeof(fts_atom_array_t));
}





