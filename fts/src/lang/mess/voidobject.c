/* 
   The void object; it have no methods, no inlets, not outlets;
   do nothing at all; it used by the editor if it need to build
   a temporary placeholder object.
*/

#include "sys.h"
#include "lang/mess.h"

/********************* void_object ************************/

typedef struct 
{
  fts_object_t o;
} void_object_t;


static fts_status_t
void_object_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(void_object_t), 0, 0, 0);

  return fts_Success;
}


void
fts_void_object_config(void)
{
  fts_metaclass_create(fts_new_symbol("__void"),void_object_instantiate, fts_always_equiv);
}
