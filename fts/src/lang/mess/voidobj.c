/* 
   The voidobj; it have no methods, no inlets, not outlets;
   it is created by the patparser when it cannot find 
   an object, in order to mantain the consistency of the
   parsing (inlet and outlets are relative to the position).
*/

#include "fts.h"

typedef struct 
{
  fts_object_t ob;

} voidobj_t;


static fts_status_t
voidobj_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  /* initialize the class */

  fts_class_init(cl, sizeof(voidobj_t), 0, 0, 0); 

  return fts_Success;
}


void
fts_voidobj_config(void)
{
  fts_metaclass_create(fts_new_symbol("ERROR"), voidobj_instantiate, fts_always_equiv);
}

