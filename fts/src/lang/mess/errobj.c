/* 
   The error_obj; it have no methods, no inlets, not outlets;
   it is created by the patparser when it cannot find 
   an object, in order to mantain the consistency of the
   parsing (inlet and outlets are relative to the position).
*/

#include "fts.h"

/*
 * The error object; actually, error object are patchers, marked as errors,
 * because they can change dinamically their number of inlets and outs
 *
 */


fts_object_t *fts_error_object_new(fts_patcher_t *parent, int id, int ac, const fts_atom_t *at)
{
  fts_object_t *obj;
  int i;
  fts_atom_t a;
  fts_atom_t description[4];

  fts_set_symbol(&description[0], fts_s_patcher);
  fts_set_symbol(&description[1], fts_new_symbol("error"));
  fts_set_int(&description[2], 0);
  fts_set_int(&description[3], 0);

  obj = fts_object_new(parent, id, 4, description);

  /* Change the description in the object */

  fts_object_set_description(obj, ac, at);

  /* flag the patcher as error */

  fts_patcher_set_error((fts_patcher_t *)obj);


  /* Return the object  */

  return obj;
}


void fts_error_object_fit_inlet(fts_object_t *obj, int ninlet)
{
  if (fts_object_get_inlets_number(obj) <= ninlet)
    fts_patcher_redefine((fts_patcher_t *) obj,
			 fts_s_error,
			 ninlet + 1,
			 fts_object_get_outlets_number(obj));
}


void fts_error_object_fit_outlet(fts_object_t *obj, int noutlet)
{
  if (fts_object_get_outlets_number(obj) <= noutlet)
    fts_patcher_redefine((fts_patcher_t *) obj,
			 fts_s_error,
			 fts_object_get_inlets_number(obj),
			 noutlet + 1);
}





