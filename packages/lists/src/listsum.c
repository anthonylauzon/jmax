#include "fts.h"

#define MAX_length 128


typedef struct 
{
  fts_object_t ob;	 
} listsum_t;

static void
listsum_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int i;
  long i_sum = 0;
  float f_sum = 0.0f;

  for(i=0; i<ac; i++)
    {
      const fts_atom_t *atom = at + i;

      if(fts_is_a(atom, fts_s_int))
	i_sum += fts_get_int(atom);
      else if(fts_is_a(atom, fts_s_float))
	f_sum += fts_get_float(atom);
    }

  if(f_sum == 0.0f)
    fts_outlet_int(o, 0, i_sum);
  else
    fts_outlet_float(o, 0, (float)i_sum + f_sum);
}

static fts_status_t
listsum_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  /* initialize the class */

  fts_class_init(cl, sizeof(listsum_t), 1, 1, 0); 

  /* list args */

  fts_method_define_varargs(cl, 0, fts_s_list, listsum_list);

  return fts_Success;
}

void
listsum_config(void)
{
  fts_metaclass_create(fts_new_symbol("listsum"), listsum_instantiate, fts_always_equiv);
}
