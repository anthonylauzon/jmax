/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */

/* 
   The error_obj; it have no methods, no inlets, not outlets;
   it is created by the patparser when it cannot find 
   an object, in order to mantain the consistency of the
   parsing (inlet and outlets are relative to the position).
*/

#include <stdarg.h>
#include "sys.h"
#include "lang/mess.h"
#include "lang/datalib.h"
#include "lang/mess/messP.h"

/*
 * The error object; actually, error object are patchers, marked as errors,
 * because they can change dinamically their number of inlets and outs
 *
 */


fts_object_t *fts_error_object_new(fts_patcher_t *parent, int ac, const fts_atom_t *at, const char *format, ...)
{
  va_list ap;
  char buf[1024];
  fts_atom_t a;
  fts_object_t *obj;

  fts_atom_t description[1];

  fts_set_symbol(&description[0], fts_s_patcher);

  fts_make_object(parent, 1, description, &obj);

  /* flag the patcher as error */

  fts_patcher_set_error((fts_patcher_t *)obj);

  /* Make up the errdesc property (eat symbols !!) */

  va_start(ap, format);
  vsprintf(buf, format, ap);
  va_end(ap);

  fts_set_symbol(&a, fts_new_symbol_copy(buf));
  fts_object_put_prop(obj, fts_s_error_description, &a);

  /* Return the object  */

  return obj;
}


void fts_error_object_fit_inlet(fts_object_t *obj, int ninlet)
{
  if (fts_object_get_inlets_number(obj) <= ninlet)
    fts_patcher_redefine_number_of_inlets((fts_patcher_t *) obj, ninlet + 1);
}


void fts_error_object_fit_outlet(fts_object_t *obj, int noutlet)
{
  if (fts_object_get_outlets_number(obj) <= noutlet)
    fts_patcher_redefine_number_of_outlets((fts_patcher_t *) obj, noutlet + 1);
}


/* Just locally mark that recomputing errors is needed */

static int need_recompute_errors = 0;

void fts_recompute_errors()
{
  need_recompute_errors = 1;
}

/*
  Try to recompute all the error objects; to be called after
  an "env" change.
   */

void fts_do_recompute_errors(void)
{
  fts_object_set_t *errors;
  fts_object_t *root;
  fts_atom_t a[1];
  fts_object_set_iterator_t *iterator;

  if (need_recompute_errors)
    {
      /* Find all the errors */

      errors = fts_object_set_new();
      root = (fts_object_t *) fts_get_root_patcher();

      fts_set_data(&a[0], (fts_data_t *) errors);
      fts_send_message(root, fts_SystemInlet, fts_s_find_errors, 1, a);
  
      /* Recompute them all */

      iterator = fts_object_set_iterator_new(errors);

      while (! fts_object_set_iterator_end(iterator))
	{
	  fts_object_t *object;

	  object = fts_object_set_iterator_current(iterator);

	  fts_object_recompute(object);

	  fts_object_set_iterator_next(iterator);
	}

      fts_object_set_iterator_free(iterator);
      fts_object_set_delete(errors);
    }
}
