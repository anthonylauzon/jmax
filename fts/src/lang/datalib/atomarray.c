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
 * An ftl_data_t object implementing a array of integer.  Supported
 * by a corresponding Java class.  Used for the moment by the old
 * table object; for now, directly implement a number of operations on
 * integer array used by the other objects.
 */


fts_data_class_t *fts_atom_array_data_class = 0;

/* remote call codes */

#define ATOM_ARRAY_SET    1
#define ATOM_ARRAY_UPDATE 2
#define ATOM_ARRAY_NAME   3

/* local */

static void atom_array_realloc(fts_atom_array_t *array, int size)
{
  int i;

  if(size > array->alloc)
    {
      if(array->alloc)
	array->atoms = (fts_atom_t *)fts_realloc((void *)array->atoms, sizeof(fts_atom_t) * size);
      else
	array->atoms = (fts_atom_t *)fts_malloc(sizeof(fts_atom_t) * size);

      array->alloc = size;
    }
  else
    {
      /* if shorter, void atoms at the end (release references) */
      for(i=size; i<array->size; i++)
	{
	  fts_atom_t *atom = array->atoms + i;
	  
	  if(fts_is_data(atom))
	    fts_data_derefer(fts_get_data(atom));
	  
	  fts_set_void(atom);
	}
    }
  
  array->size = size;
}

/* new/delete */

fts_atom_array_t *
fts_atom_array_new(int size)
{
  fts_atom_array_t *array;
  int i;

  array = (fts_atom_array_t *)fts_malloc(sizeof(fts_atom_array_t));

  if(size > 0)
    {
      int i;
      array->atoms = (fts_atom_t *) fts_malloc(size * sizeof(fts_atom_t));
      array->size = size;
      
      for(i=0; i<size; i++)
	fts_set_void(array->atoms + i);
    }
  else
    {
      array->atoms = 0;
      array->size = 0;
    }

  array->alloc = size;

  fts_data_init((fts_data_t *)array, fts_atom_array_data_class);

  return array;
}

fts_atom_array_t *
fts_atom_array_new_from_atom_list(int ac, const fts_atom_t *at)
{
  fts_atom_array_t *array;
  int i;

  /* create array */
  array = fts_atom_array_new(ac);

  /* fill it */
  for(i=0; i<array->size; i++)
    array->atoms[i] = at[i];

  return array;
}

void 
fts_atom_array_delete(fts_atom_array_t *array)
{
  fts_atom_array_void(array);

  if (array->atoms)
    fts_free((void *)array->atoms);

  fts_data_delete((fts_data_t *)array);
  fts_free((void *)array);
}

int
fts_atom_array_get_atoms(fts_atom_array_t *array, int ac, fts_atom_t *at)
{
  int i;
  int size = array->size;

  if(ac > size)
    ac = size;
  
  for(i=0; i<ac; i++)
    at[i] = array->atoms[i];

  return size;
}

/* copy & void */
void
fts_atom_array_copy(fts_atom_array_t *in, fts_atom_array_t *out)
{
  int i;
  atom_array_realloc(out, in->size);
  
  for(i=0; i<in->size; i++)
    out->atoms[i] = in->atoms[i];
}

void
fts_atom_array_void(fts_atom_array_t *array)
{
  int i;
  
  for(i=0; i<array->size; i++)
    {
      fts_atom_t *atom = array->atoms + i;
      
      if(fts_is_data(atom))
	fts_data_derefer(fts_get_data(atom));
      
      fts_set_void(atom);
    }
}

/* set the size of the array */
void
fts_atom_array_set_size(fts_atom_array_t *array, int size)
{
  int old_size = array->size;
  int i;

  atom_array_realloc(array, size);

  /* if longer, set new atoms at the end to void */
  for(i=old_size; i<size; i++)
    fts_set_void(array->atoms + i);
}

void
fts_atom_array_set_from_atom_list(fts_atom_array_t *array, int offset, int ac, const fts_atom_t *at)
{
  int size = fts_atom_array_get_size(array);
  int i;
  
  if(offset + ac > size)
    ac = size - offset;
  
  for(i=0; i<ac; i++)
    array->atoms[i + offset] = at[i];
}

/********************************************************************
 *
 *   FTS data functions
 *
 */

static void fts_atom_array_export_fun(fts_data_t *data)
{
  fts_atom_array_t *array = (fts_atom_array_t *)data;
  int i;

  fts_data_start_remote_call(data, ATOM_ARRAY_SET);
  fts_client_mess_add_int(array->size);
  fts_client_mess_add_atoms(array->size, array->atoms);

  fts_data_end_remote_call();
}

static void 
fts_atom_array_remote_set( fts_data_t *data, int ac, const fts_atom_t *at)
{
  fts_atom_array_t *array = (fts_atom_array_t *)data;
  int offset;
  int n_atoms;
  int i;

  /* arguments: offset, # of atoms, atoms */
  offset = fts_get_int(&at[0]);
  n_atoms = fts_get_int(&at[1]);

  for (i=0; i<n_atoms; i++)
    array->atoms[i + offset] = at[i + 2];
}


static void 
fts_atom_array_remote_update( fts_data_t *data, int ac, const fts_atom_t *at)
{
  fts_atom_array_t *array = (fts_atom_array_t *)data;
  int i;

  fts_data_start_remote_call(data, ATOM_ARRAY_SET);
  fts_client_mess_add_int(array->size);
  fts_client_mess_add_atoms(array->size, array->atoms);

  fts_data_end_remote_call();
}

fts_data_t *
fts_atom_array_constructor(int ac, const fts_atom_t *at)
{
  fts_atom_array_t *array = 0;

  if(ac == 1 && fts_is_int(at))
    {
      array = fts_atom_array_new(fts_get_int(at));
    }
  else if(ac > 1)
    {
      array = fts_atom_array_new_from_atom_list(ac, at);
    }
  else
    array = fts_atom_array_new(0);
    
  
  return (fts_data_t *)array;
}

void fts_atom_array_config(void)
{
  fts_atom_array_data_class = fts_data_class_new(fts_s_atom_array);

  fts_data_class_define_export_function(fts_atom_array_data_class, fts_atom_array_export_fun);
  fts_data_class_define_function(fts_atom_array_data_class, ATOM_ARRAY_SET, fts_atom_array_remote_set);
  fts_data_class_define_function(fts_atom_array_data_class, ATOM_ARRAY_UPDATE, fts_atom_array_remote_update);
}

/********************************************************************
 *
 *  bmax format releated functions
 *
 */

/* array is actually quite a temporary hack; there should be a real
   save standard technique for fts_data_t; it assume that is reloaded
   for a array !!*/

void fts_atom_array_save_bmax(fts_atom_array_t *array, fts_bmax_file_t *f)
{
  fts_atom_t av[256];
  int ac = 0;
  int i;
  int offset = 0;

  for(i=0; i<array->size; i++)
    {
      av[ac] = array->atoms[i];

      ac++;

      if (ac == 256)
	{
	  /* Code a push of all the atoms */
	  fts_bmax_code_push_atoms(f, ac, av);

	  /* Code a push of the offset */
	  fts_bmax_code_push_int(f, offset);

	  /* Code a "set" message for 256 atoms plus offset */
	  fts_bmax_code_obj_mess(f, fts_SystemInlet, fts_s_set, ac + 1);
	  offset = offset + ac;

	  /* Code a pop of all the atoms  */
	  fts_bmax_code_pop_args(f, ac);

	  ac = 0;
	}
    }

  if (ac != 0)
    {
      /* Code a push of all the atoms */
      fts_bmax_code_push_atoms(f, ac, av);

      /* Code a push of the offset */
      fts_bmax_code_push_int(f, offset);

      /* Code an "append" message for the atoms left */
      fts_bmax_code_obj_mess(f, fts_SystemInlet, fts_s_set, ac + 1);

      /* Code a pop of all the atoms  */
      fts_bmax_code_pop_args(f, ac);
    }
}

void fprintf_atom_array(FILE *file, fts_atom_array_t *array)
{
  int i;

  fprintf(file, "<{");
  for (i=0; i<array->size; i++)
    {
      fprintf_atoms(file, 1, &(array->atoms[i]));
      fprintf(file, ",");
    }

  fprintf(file, "} %lx>",(unsigned long) array);
}
