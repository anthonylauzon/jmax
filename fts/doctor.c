/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * See file LICENSE for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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

#include <fts/fts.h>

typedef struct fts_object_doctor
{
  fts_object_t *(* fun)(fts_patcher_t *patcher, int ac, const fts_atom_t *at);
} fts_object_doctor_t;

static fts_hashtable_t fts_doctor_table;

void fts_register_object_doctor(fts_symbol_t class_name,
				fts_object_t *(* fun)(fts_patcher_t *patcher, int ac, const fts_atom_t *at))
{
  fts_atom_t data, k;
  fts_object_doctor_t *d;

  fts_set_symbol( &k, class_name);
  if (fts_hashtable_get(&fts_doctor_table, &k, &data))
    {
      fts_hashtable_remove(&fts_doctor_table, &k);
      fts_free(fts_get_ptr(&data));
    }

  d = (fts_object_doctor_t *) fts_malloc(sizeof(fts_object_doctor_t));
  d->fun = fun;

  fts_set_ptr(&data, d);
  fts_hashtable_put(&fts_doctor_table, &k, &data);

}

int fts_object_doctor_exists(fts_symbol_t class_name)
{
  fts_atom_t data, k;

  fts_set_symbol( &k, class_name);
  return fts_hashtable_get(&fts_doctor_table, &k, &data);
}

fts_object_t *fts_call_object_doctor(fts_patcher_t *patcher, int ac, const fts_atom_t *at)
{
  fts_atom_t data, k;

  fts_set_symbol( &k, fts_get_symbol(at));
  if (fts_hashtable_get(&fts_doctor_table, &k, &data))
    {
      fts_object_doctor_t *d = (fts_object_doctor_t *) fts_get_ptr(&data);

      return (* d->fun)(patcher, ac, at);
    }
  else
    return 0;
}

/***********************************************************************
 *
 * Initialization
 *
 */

void fts_kernel_doctor_init()
{
  fts_hashtable_init( &fts_doctor_table, 0, FTS_HASHTABLE_MEDIUM);
}
