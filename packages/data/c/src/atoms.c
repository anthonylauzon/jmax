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
#include "fts.h"

/****************************************************************
 *
 *  data -> atoms
 *
 */

/* get data size in # of atoms */
int 
data_get_size(fts_data_t *data)
{
  if(data->class == fts_integer_vector_data_class)
    return ((fts_integer_vector_t *)data)->size;
  else if(data->class == fts_integer_vector_data_class)
    return ((fts_float_vector_t *)data)->size;
  else if(data->class == fts_atom_array_data_class)
    return ((fts_atom_array_t *)data)->size;

  return 0;
}

/* get data as array of atoms (takes pointer to pre-allocated (!) array) and
   return original size (might be bigger or smaller than pre-allocated array with ac) */
int
data_get_atoms(fts_data_t *data, int ac, fts_atom_t *at)
{
  if(data->class == fts_integer_vector_data_class)
    return fts_integer_vector_get_atoms((fts_integer_vector_t *)data, ac, at);
  else if(data->class == fts_float_vector_data_class)
    return fts_float_vector_get_atoms((fts_float_vector_t *)data, ac, at);
  else if(data->class == fts_atom_array_data_class)
    return fts_atom_array_get_atoms((fts_atom_array_t *)data, ac, at);

  return 0;
}
