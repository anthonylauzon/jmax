
#include <stdlib.h>
#include "fts.h"

static void 
classprint( fts_symbol_t s, void *c, void *u)
{
  const fts_metaclass_t *mcl = (const fts_metaclass_t *) c;
  int *pi = (int *)u;

  post( "%4d %s\n", *pi, fts_symbol_name(s));
  (*pi)++;
}


typedef fts_symbol_t entry_t;

static void 
classget( fts_symbol_t s, void *c, void *u)
{
  const fts_metaclass_t *mcl = (const fts_metaclass_t *) c;
  entry_t **pp, *p;

  pp = (entry_t **)u;
  p = *pp;
  *p = s;
  (*pp)++;
}

static int
scomp( const void *p1, const void *p2)
{
  const entry_t *e1 = (const entry_t *)p1, *e2 = (const entry_t *)p2;

  return strcmp( fts_symbol_name(*e1), fts_symbol_name(*e2));
}

static void
listclasses_sorted( int count)
{
  int i;
  entry_t *tab, *p;

  tab = fts_malloc( count*sizeof( entry_t));
  if ( !tab)
    {
      post( "listclasses : no more memory\n");
      return;
    }

  p = tab;

  /* fts_metaclasses_exec( classget, &p); */

  qsort( tab, count, sizeof( entry_t), scomp);

  for( i=0; i<count; i++)
    post( "%4d %s\n", i, fts_symbol_name(tab[i]));

  fts_free( tab);
}

static void 
classcount( fts_symbol_t s, void *c, void *u)
{
  const fts_metaclass_t *mcl = (const fts_metaclass_t *) c;
  int *p = (int *)u;

  (*p)++;
}

static void
listclasses_bang(  fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int count = 0;

  /* fts_metaclasses_exec( classcount, &count); */

  post( "List of %d registered FTS classes :\n", count);

  listclasses_sorted( count);
}

static fts_status_t
listclasses_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(fts_object_t), 1, 0, 0);

  fts_method_define(cl, 0, fts_s_bang, listclasses_bang, 0, 0);

  return fts_Success;
}

void
listclasses_config()
{
  fts_metaclass_create(fts_new_symbol( "listclasses"), listclasses_instantiate, fts_always_equiv);
}
