/*
  Expr doctor.

  Fix expr boxes by avoiding new expression evaluation in their
  arguments; later may do a little bit more ...
 */

#include "fts.h"

static fts_object_t *expr_doctor(fts_patcher_t *patcher, int ac, const fts_atom_t *at)
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


void expr_doctor_init()
{
  fts_register_object_doctor(fts_new_symbol("expr"), expr_doctor);
}
    
