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
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include "sys.h"
#include "lang/mess.h"
#include "lang/datalib.h"
#include "lang/veclib.h"
#include "runtime.h"	/* @@@@ what should we do ?? */

/*
 * An ftl_data_t object implementing a vector of integer.  Supported
 * by a corresponding Java class.  Used for the moment by the old
 * table object; for now, directly implement a number of operations on
 * integer vector used by the other objects.
 */


fts_symbol_t fts_s_integer_vector = 0;
fts_data_class_t *fts_integer_vector_data_class = 0;

/* remote call codes */

#define INTEGER_VECTOR_SET    1
#define INTEGER_VECTOR_UPDATE 2
#define INTEGER_VECTOR_NAME   3

/* local */

static void intvec_set_size(fts_integer_vector_t *vector, int size)
{
  if(size > vector->alloc)
    {
      if(vector->alloc)
	vector->values = (int *)fts_realloc((void *)vector->values, sizeof(int) * size);
      else
	vector->values = (int *)fts_malloc(sizeof(int) * size);

      vector->alloc = size;
    }

  vector->size = size;
}

/* new/delete */

fts_integer_vector_t *
fts_integer_vector_new(int size)
{
  fts_integer_vector_t *vector;
  int i;

  vector = (fts_integer_vector_t *)fts_malloc(sizeof(fts_integer_vector_t));

  if(size > 0)
    {
      vector->values = (int *) fts_malloc(size * sizeof(int));
      vector->size = size;
      fts_vec_izero(vector->values, vector->size);  
    }
  else
    {
      vector->values = 0;
      vector->size = 0;
    }

  vector->alloc = size;

  fts_data_init((fts_data_t *) vector, fts_integer_vector_data_class);

  return vector;
}

void 
fts_integer_vector_delete(fts_integer_vector_t *vector)
{
  fts_data_delete((fts_data_t *) vector);
  fts_free((void *)vector->values);
  fts_free((void *)vector);
}

int
fts_integer_vector_get_atoms(fts_integer_vector_t *vector, int ac, fts_atom_t *at)
{
  int i;
  int size = vector->size;

  if(ac > size)
    ac = size;

  for(i=0; i<ac; i++)
    fts_set_int(at + i, vector->values[i]);

  return size;
}

/* copy & zero */
void
fts_integer_vector_copy(fts_integer_vector_t *in, fts_integer_vector_t *out)
{
  intvec_set_size(out, in->size);
  fts_vec_icpy(in->values, out->values, in->size);
}

void
fts_integer_vector_zero(fts_integer_vector_t *vector)
{
  fts_vec_izero(vector->values, vector->size);  
}

/* set the size of the vector */
void
fts_integer_vector_set_size(fts_integer_vector_t *vector, int size)
{
  int old_size = vector->size;
  int tail = size - old_size;

  intvec_set_size(vector, size);

  if(tail > 0)
    fts_vec_izero(vector->values + old_size, tail);
}

void 
fts_integer_vector_set_from_ptr(fts_integer_vector_t *vector, int *ptr, int size)
{
  intvec_set_size(vector, size);
  fts_vec_icpy(ptr, vector->values, size);
}

void
fts_integer_vector_set_from_atom_list(fts_integer_vector_t *vector, int offset, int ac, const fts_atom_t *at)
{
  int size = fts_integer_vector_get_size(vector);
  int i;
  
  if(offset + ac > size)
    ac = size - offset;
  
  for(i=0; i<ac; i++)
    {
      if(fts_is_int(&at[i]))
	vector->values[i + offset] = fts_get_int(&at[i]);
      else if(fts_is_float(&at[i]))
	vector->values[i + offset] = (int)fts_get_float(&at[i]);
    }
}

/* sum, min, max */

int
fts_integer_vector_get_sum(fts_integer_vector_t *vector)
{
  int sum = 0;
  int i;

  for(i=0; i<vector->size; i++)
    sum += vector->values[i];

  return sum;
}

int
fts_integer_vector_get_sub_sum(fts_integer_vector_t *vector, int from, int to)
{
  int sum = 0;
  int i;
  
  if(from < 0)
    from = 0;

  if(to >= vector->size)
    to = vector->size - 1;

  for (i=from; i<=to; i++)
    sum +=vector->values[i];

  return sum;
}

int
fts_integer_vector_get_min_value(fts_integer_vector_t *vector)
{
  int min;
  int i;

  min = vector->values[0];

  for (i = 1; i < vector->size; i++)
    if (vector->values[i] < min)
      min = vector->values[i];

  return min;
}


int
fts_integer_vector_get_max_value(fts_integer_vector_t *vector)
{
  int max;
  int i;

  max = vector->values[0];

  for (i = 1; i < vector->size; i++)
    if (vector->values[i] > max)
      max = vector->values[i];

  return max;
}

/********************************************************************
 *
 *   FTS data functions
 *
 */

static void fts_integer_vector_export_fun(fts_data_t *data)
{
  fts_integer_vector_t *vector = (fts_integer_vector_t *)data;
  int i;

  if (fts_data_get_name(data))
    {
      fts_data_start_remote_call(data, INTEGER_VECTOR_NAME);
      fts_client_mess_add_symbol(fts_data_get_name(data));
      fts_data_end_remote_call();
    }

  fts_data_start_remote_call(data, INTEGER_VECTOR_SET);
  fts_client_mess_add_int(vector->size);

  for (i = 0; i < vector->size; i++)
    fts_client_mess_add_int(vector->values[i]);

  fts_data_end_remote_call();
}

static void 
fts_integer_vector_remote_set( fts_data_t *data, int ac, const fts_atom_t *at)
{
  fts_integer_vector_t *vector = (fts_integer_vector_t *)data;
  int offset;
  int nvalues;
  int i;

  /* arguments: offset, # of values, values */

  offset = fts_get_int(&at[0]);
  nvalues = fts_get_int(&at[1]);

  for (i=0; i<nvalues; i++)
    vector->values[i + offset] = fts_get_int(&at[i + 2]);
}


static void 
fts_integer_vector_remote_update( fts_data_t *data, int ac, const fts_atom_t *at)
{
  fts_integer_vector_t *vector = (fts_integer_vector_t *)data;
  int i;

  fts_data_start_remote_call(data, INTEGER_VECTOR_SET);
  fts_client_mess_add_int(vector->size);

  for (i=0; i<vector->size; i++)
    fts_client_mess_add_int(vector->values[i]);

  fts_data_end_remote_call();
}

fts_data_t *
fts_integer_vector_constructor(int ac, const fts_atom_t *at)
{
  fts_integer_vector_t *vec = 0;

  if(ac == 1 && fts_is_int(at))
    {
      vec = fts_integer_vector_new(fts_get_int(at));
    }
  else if(ac > 1)
    {
      vec = fts_integer_vector_new(ac);
      fts_integer_vector_set_from_atom_list(vec, 0, ac, at);
    }
  else
    vec = fts_integer_vector_new(0);
  
  return (fts_data_t *)vec;
}

void fts_integer_vector_config(void)
{
  fts_s_integer_vector = fts_new_symbol("integer_vector");
  fts_integer_vector_data_class = fts_data_class_new(fts_s_integer_vector);

  fts_data_class_define_export_function(fts_integer_vector_data_class, fts_integer_vector_export_fun);
  fts_data_class_define_function(fts_integer_vector_data_class, INTEGER_VECTOR_SET, fts_integer_vector_remote_set);
  fts_data_class_define_function(fts_integer_vector_data_class, INTEGER_VECTOR_UPDATE, fts_integer_vector_remote_update);
}

/********************************************************************
 *
 *  bmax format releated functions
 *
 */

/* vector is actually quite a temporary hack; there should be a real
   save standard technique for fts_data_t; it assume that is reloaded
   for a vector !!*/

void fts_integer_vector_save_bmax(fts_integer_vector_t *vector, fts_bmax_file_t *f)
{
  fts_atom_t av[256];
  int ac = 0;
  int i;
  int offset = 0;

  for  (i = 0; i < vector->size; i++)
    {
      fts_set_int(&av[ac], vector->values[i]);

      ac++;

      if (ac == 256)
	{
	  /* Code a push of all the values */
	  fts_bmax_code_push_atoms(f, ac, av);

	  /* Code a push of the offset */
	  fts_bmax_code_push_int(f, offset);

	  /* Code a "set" message for 256 values plus offset */
	  fts_bmax_code_obj_mess(f, fts_SystemInlet, fts_s_set, ac + 1);
	  offset = offset + ac;

	  /* Code a pop of all the values  */
	  fts_bmax_code_pop_args(f, ac);

	  ac = 0;
	}
    }

  if (ac != 0)
    {
      /* Code a push of all the values */
      fts_bmax_code_push_atoms(f, ac, av);

      /* Code a push of the offset */
      fts_bmax_code_push_int(f, offset);

      /* Code an "append" message for the values left */
      fts_bmax_code_obj_mess(f, fts_SystemInlet, fts_s_set, ac + 1);

      /* Code a pop of all the values  */
      fts_bmax_code_pop_args(f, ac);
    }
}

