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
  Object doctor.

  When an object instantiation in fts_eval_object_description fail,
  we try a doctor for it; a doctor is a function registered on the
  class name, and called with the same arguments as fts_eval_object_description.
  
  It should return an FTS object, made in any possible way; it can
  call recursively fts_eval_object_description.

  Doctors are used to fix small changes in the object arguments
  without adding compatibility code to the object themselves,
  but instead specifing a compatibilty fixer clearly isolated
  from the original code.
  
  They can produce objects with the old description or with 
  the new description (i.e. the fix made by the doctor can
  be persistent or just loading time).

  doctors can be used to implemented C based "macros", or to 
  implement big compatibility packages and the like.

  One one doctor for a name is registered.

  Doctors are not registered in the meta class structure, because the
  meta class itself may just non exists anymore.

  */

#include "sys.h"
#include "lang/mess.h"
#include "lang/utils.h"
#include "lang/mess/messP.h"


typedef struct fts_object_doctor
{
  fts_object_t *(* fun)(fts_patcher_t *patcher, int ac, const fts_atom_t *at);
} fts_object_doctor_t;

static fts_hash_table_t fts_doctor_table;


void fts_doctor_init()
{
  fts_hash_table_init(&fts_doctor_table);
}


void fts_register_object_doctor(fts_symbol_t class_name,
				fts_object_t *(* fun)(fts_patcher_t *patcher, int ac, const fts_atom_t *at))
{
  fts_atom_t data;
  fts_object_doctor_t *d;

  if (fts_hash_table_lookup(&fts_doctor_table, class_name, &data))
    {
      fts_hash_table_remove(&fts_doctor_table, class_name);
      fts_free(fts_get_ptr(&data));
    }

  d = (fts_object_doctor_t *) fts_malloc(sizeof(fts_object_doctor_t));
  d->fun = fun;

  fts_set_ptr(&data, d);
  fts_hash_table_insert(&fts_doctor_table, class_name, &data);

}

int fts_object_doctor_exists(fts_symbol_t class_name)
{
  fts_atom_t data;

  return fts_hash_table_lookup(&fts_doctor_table, class_name, &data);
}

fts_object_t *fts_call_object_doctor(fts_patcher_t *patcher, int ac, const fts_atom_t *at)
{
  fts_atom_t data;
  fts_symbol_t class_name;

  class_name = fts_get_symbol(at);

  if (fts_hash_table_lookup(&fts_doctor_table, class_name, &data))
    {
      fts_object_doctor_t *d = (fts_object_doctor_t *) fts_get_ptr(&data);

      return (* d->fun)(patcher, ac, at);
    }
  else
    return 0;
}

