/*
  Comment doctor.

  Fix comment boxes by avoiding expression evaluation in their
  arguments; later comments should work like message boxes.
 */

#include "fts.h"

static fts_object_t *comment_doctor(fts_patcher_t *patcher, int ac, const fts_atom_t *at)
{
  if (ac >= 1)
    {
      fts_object_t *obj;

      fts_make_object(patcher, ac, at, &obj);

      return obj;
    }
  else
    return 0;
}


void comment_doctor_init()
{
  fts_register_object_doctor(fts_new_symbol("comment"), comment_doctor);
}
    
