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

#include "fts.h"

typedef struct
{
  fts_atom_t *at;
  int ac;
  int alloc;
} list_t;
  
typedef struct 
{
  fts_object_t o;
  list_t list;
  list_t right_list;
  fts_atom_t right_atom;
  fts_symbol_t right_type;
} listarith_t;

#define LIST_ALLOC_BLOCK 32

/*********************************************
 *
 *  atom arithmetics
 *
 */

static void
atom_add(const fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  if(fts_is_a(left, fts_s_int))
    {
      long l = fts_get_int(left);
      
      if(fts_is_a(right, fts_s_int))
	fts_set_int(result, l + fts_get_int(right));
      else if(fts_is_a(right, fts_s_float))
	fts_set_float(result, l + fts_get_float(right));
      else
	*result = *left;
    }
  else if(fts_is_a(left, fts_s_float))
    {
      float l = fts_get_float(left);
      
      if(fts_is_a(right, fts_s_int))
	fts_set_float(result, l + fts_get_int(right));
      else if(fts_is_a(right, fts_s_float))
	fts_set_float(result, l + fts_get_float(right));
      else
	*result = *left;
    }
  else
    {
      if(fts_is_a(right, fts_s_int) || fts_is_a(right, fts_s_float))
	*result = *right;
      else
	*result = *left;
    }
}

static void
atom_sub(const fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  if(fts_is_a(left, fts_s_int))
    {
      long l = fts_get_int(left);
      
      if(fts_is_a(right, fts_s_int))
	fts_set_int(result, l - fts_get_int(right));
      else if(fts_is_a(right, fts_s_float))
	fts_set_float(result, l - fts_get_float(right));
      else
	*result = *left;
    }
  else if(fts_is_a(left, fts_s_float))
    {
      float l = fts_get_float(left);
      
      if(fts_is_a(right, fts_s_int))
	fts_set_float(result, l - fts_get_int(right));
      else if(fts_is_a(right, fts_s_float))
	fts_set_float(result, l - fts_get_float(right));
      else
	*result = *left;
    }
  else
    {
      if(fts_is_a(right, fts_s_int))
	fts_set_int(result, -fts_get_int(right));
      else if(fts_is_a(right, fts_s_float))
	fts_set_float(result, fts_get_float(right));
      else
	fts_set_int(result, 0);
    }
}

static void
atom_mul(const fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  if(fts_is_a(left, fts_s_int))
    {
      long l = fts_get_int(left);
      
      if(fts_is_a(right, fts_s_int))
	fts_set_int(result, l * fts_get_int(right));
      else if(fts_is_a(right, fts_s_float))
	fts_set_float(result, l * fts_get_float(right));
      else
	*result = *left;
    }
  else if(fts_is_a(left, fts_s_float))
    {
      float l = fts_get_float(left);
      
      if(fts_is_a(right, fts_s_int))
	fts_set_float(result, l * fts_get_int(right));
      else if(fts_is_a(right, fts_s_float))
	fts_set_float(result, l * fts_get_float(right));
      else
	*result = *left;
    }
  else
    {
      if(fts_is_a(right, fts_s_int) || fts_is_a(right, fts_s_float))
	*result = *right;
      else
	fts_set_int(result, 1);
    }
}

static void
atom_div(const fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  if(fts_is_a(left, fts_s_int))
    {
      long l = fts_get_int(left);
      
      if(fts_is_a(right, fts_s_int))
	fts_set_int(result, l / fts_get_int(right));
      else if(fts_is_a(right, fts_s_float))
	fts_set_float(result, l / fts_get_float(right));
      else
	*result = *left;
    }
  else if(fts_is_a(left, fts_s_float))
    {
      float l = fts_get_float(left);
      
      if(fts_is_a(right, fts_s_int))
	fts_set_float(result, l / fts_get_int(right));
      else if(fts_is_a(right, fts_s_float))
	fts_set_float(result, l / fts_get_float(right));
      else
	*result = *left;
    }
  else
    {
      if(fts_is_a(right, fts_s_int))
	fts_set_float(result, 1.0f / (float)fts_get_int(right));
      else if(fts_is_a(right, fts_s_float))
	fts_set_float(result, 1.0f / fts_get_float(right));
      else
	fts_set_int(result, 1);
    }
}

void
atom_gt(const fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  if(fts_is_a(left, fts_s_int))
    {
      long l = fts_get_int(left);
      
      if(fts_is_a(right, fts_s_int))
	fts_set_int(result, l > fts_get_int(right));
      else if(fts_is_a(right, fts_s_float))
	fts_set_int(result, l > fts_get_float(right));
      else
	fts_set_int(result, l > 0);
    }
  else if(fts_is_a(left, fts_s_float))
    {
      float l = fts_get_float(left);
      
      if(fts_is_a(right, fts_s_int))
	fts_set_int(result, l > fts_get_int(right));
      else if(fts_is_a(right, fts_s_float))
	fts_set_int(result, l > fts_get_float(right));
      else
	fts_set_int(result, l > 0.0f);
    }
  else
    {
      if(fts_is_a(right, fts_s_int))
	fts_set_int(result, 0.0f > fts_get_int(right));
      else if(fts_is_a(right, fts_s_float))
	fts_set_int(result, 0.0f > fts_get_float(right));
      else
	fts_set_int(result, 0);
    }
}

void
atom_ge(const fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  if(fts_is_a(left, fts_s_int))
    {
      long l = fts_get_int(left);
      
      if(fts_is_a(right, fts_s_int))
	fts_set_int(result, l >= fts_get_int(right));
      else if(fts_is_a(right, fts_s_float))
	fts_set_int(result, l >= fts_get_float(right));
      else
	fts_set_int(result, l >= 0);
    }
  else if(fts_is_a(left, fts_s_float))
    {
      float l = fts_get_float(left);
      
      if(fts_is_a(right, fts_s_int))
	fts_set_int(result, l >= fts_get_int(right));
      else if(fts_is_a(right, fts_s_float))
	fts_set_int(result, l >= fts_get_float(right));
      else
	fts_set_int(result, l >= 0.0f);
    }
  else
    {
      if(fts_is_a(right, fts_s_int))
	fts_set_int(result, 0.0f >= fts_get_int(right));
      else if(fts_is_a(right, fts_s_float))
	fts_set_int(result, 0.0f >= fts_get_float(right));
      else
	fts_set_int(result, 0);
    }
}

void
atom_lt(const fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  if(fts_is_a(left, fts_s_int))
    {
      long l = fts_get_int(left);
      
      if(fts_is_a(right, fts_s_int))
	fts_set_int(result, l < fts_get_int(right));
      else if(fts_is_a(right, fts_s_float))
	fts_set_int(result, l < fts_get_float(right));
      else
	fts_set_int(result, l < 0);
    }
  else if(fts_is_a(left, fts_s_float))
    {
      float l = fts_get_float(left);
      
      if(fts_is_a(right, fts_s_int))
	fts_set_int(result, l < fts_get_int(right));
      else if(fts_is_a(right, fts_s_float))
	fts_set_int(result, l < fts_get_float(right));
      else
	fts_set_int(result, l < 0.0f);
    }
  else
    {
      if(fts_is_a(right, fts_s_int))
	fts_set_int(result, 0.0f < fts_get_int(right));
      else if(fts_is_a(right, fts_s_float))
	fts_set_int(result, 0.0f < fts_get_float(right));
      else
	fts_set_int(result, 0);
    }
}

void
atom_le(const fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  if(fts_is_a(left, fts_s_int))
    {
      long l = fts_get_int(left);
      
      if(fts_is_a(right, fts_s_int))
	fts_set_int(result, l <= fts_get_int(right));
      else if(fts_is_a(right, fts_s_float))
	fts_set_int(result, l <= fts_get_float(right));
      else
	fts_set_int(result, l <= 0);
    }
  else if(fts_is_a(left, fts_s_float))
    {
      float l = fts_get_float(left);
      
      if(fts_is_a(right, fts_s_int))
	fts_set_int(result, l <= fts_get_int(right));
      else if(fts_is_a(right, fts_s_float))
	fts_set_int(result, l <= fts_get_float(right));
      else
	fts_set_int(result, l <= 0.0f);
    }
  else
    {
      if(fts_is_a(right, fts_s_int))
	fts_set_int(result, 0.0f <= fts_get_int(right));
      else if(fts_is_a(right, fts_s_float))
	fts_set_int(result, 0.0f <= fts_get_float(right));
      else
	fts_set_int(result, 0);
    }
}

void
atom_ne(const fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  if(fts_is_a(left, fts_s_int))
    {
      long l = fts_get_int(left);
      
      if(fts_is_a(right, fts_s_int))
	fts_set_int(result, l != fts_get_int(right));
      else if(fts_is_a(right, fts_s_float))
	fts_set_int(result, l != fts_get_float(right));
      else
	fts_set_int(result, 1);
    }
  else if(fts_is_a(left, fts_s_float))
    {
      float l = fts_get_float(left);
      
      if(fts_is_a(right, fts_s_int))
	fts_set_int(result, l != fts_get_int(right));
      else if(fts_is_a(right, fts_s_float))
	fts_set_int(result, l != fts_get_float(right));
      else
	fts_set_int(result, 1);
    }
  else
    {
      if(fts_is_a(right, fts_s_int))
	fts_set_int(result, 1);
      else if(fts_is_a(right, fts_s_float))
	fts_set_int(result, 1);
      else
	fts_set_int(result, fts_get_symbol(left) != fts_get_symbol(right));
    }
}

void
atom_ee(const fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  if(fts_is_a(left, fts_s_int))
    {
      long l = fts_get_int(left);
      
      if(fts_is_a(right, fts_s_int))
	fts_set_int(result, l == fts_get_int(right));
      else if(fts_is_a(right, fts_s_float))
	fts_set_int(result, l == fts_get_float(right));
      else
	fts_set_int(result, 0);
    }
  else if(fts_is_a(left, fts_s_float))
    {
      float l = fts_get_float(left);
      
      if(fts_is_a(right, fts_s_int))
	fts_set_int(result, l == fts_get_int(right));
      else if(fts_is_a(right, fts_s_float))
	fts_set_int(result, l == fts_get_float(right));
      else
	fts_set_int(result, 0);
    }
  else
    {
      if(fts_is_a(right, fts_s_int))
	fts_set_int(result, 0);
      else if(fts_is_a(right, fts_s_float))
	fts_set_int(result, 0);
      else
	fts_set_int(result, fts_get_symbol(left) == fts_get_symbol(right));
    }
}

/*********************************************
 *
 *  mem utils
 *
 */

static void
list_set_size(list_t *list, int ac)
{
  int alloc = list->alloc;

  if(ac > alloc)
    {
      if(list->alloc) 
	fts_block_free(list->at, list->alloc);
      
      while(alloc < ac)
	alloc += LIST_ALLOC_BLOCK;
      
      list->at = (fts_atom_t *) fts_block_alloc(alloc * sizeof(fts_atom_t));
      list->alloc = alloc;
    }
  else
    list->ac = ac;
}

static void
list_init(list_t *list)
{
  list->at = 0;
  list->ac = 0;
  list->alloc = 0;
}

/*********************************************
 *
 *  methods
 *
 */

static void
listarith_set_right_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listarith_t *this = (listarith_t *)o;
  int i;

  list_set_size(&(this->right_list), ac);
  memcpy((char *)this->right_list.at, at, ac * sizeof(fts_atom_t)); /* copy incomming list to right */

  this->right_type = fts_s_list;
}

static void
listarith_set_right_atom(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listarith_t *this = (listarith_t *)o;

  this->right_atom = at[0];
  this->right_type = fts_get_type(at);
} 


static void
listarith_add(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listarith_t *this = (listarith_t *)o;
  int i;

  if(this->right_type == fts_s_list)
    {
      if(this->right_list.ac < ac)
	ac = this->right_list.ac;
      
      list_set_size(&(this->list), ac);
      
      for(i=0; i<ac; i++)
	atom_add(at + i, &(this->right_list.at[i]), &(this->list.at[i]));
    }
  else
    {
      list_set_size(&(this->list), ac);
      
      for(i=0; i<ac; i++)
	atom_add(at + i, &(this->right_atom), &(this->list.at[i]));
    }

  fts_outlet_send(o, 0, fts_s_list, ac, this->list.at);
}

static void
listarith_sub(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listarith_t *this = (listarith_t *)o;
  int i;

  if(this->right_type == fts_s_list)
    {
      if(this->right_list.ac < ac)
	ac = this->right_list.ac;
      
      list_set_size(&(this->list), ac);
      
      for(i=0; i<ac; i++)
	atom_sub(at + i, &(this->right_list.at[i]), &(this->list.at[i]));
    }
  else
    {
      list_set_size(&(this->list), ac);
      
      for(i=0; i<ac; i++)
	atom_sub(at + i, &(this->right_atom), &(this->list.at[i]));
    }
  
  fts_outlet_send(o, 0, fts_s_list, ac, this->list.at);
}

static void
listarith_mul(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listarith_t *this = (listarith_t *)o;
  int i;

  if(this->right_type == fts_s_list)
    {
      if(this->right_list.ac < ac)
	ac = this->right_list.ac;
      
      list_set_size(&(this->list), ac);
      
      for(i=0; i<ac; i++)
	atom_mul(at + i, &(this->right_list.at[i]), &(this->list.at[i]));
    }
  else
    {
      list_set_size(&(this->list), ac);
      
      for(i=0; i<ac; i++)
	atom_mul(at + i, &(this->right_atom), &(this->list.at[i]));
    }

  fts_outlet_send(o, 0, fts_s_list, ac, this->list.at);
}

static void
listarith_div(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listarith_t *this = (listarith_t *)o;
  int i;

  if(this->right_type == fts_s_list)
    {
      if(this->right_list.ac < ac)
	ac = this->right_list.ac;
      
      list_set_size(&(this->list), ac);
      
      for(i=0; i<ac; i++)
	atom_div(at + i, &(this->right_list.at[i]), &(this->list.at[i]));
    }
  else
    {
      list_set_size(&(this->list), ac);
      
      for(i=0; i<ac; i++)
	atom_div(at + i, &(this->right_atom), &(this->list.at[i]));
    }
  
  fts_outlet_send(o, 0, fts_s_list, ac, this->list.at);
}

static void
listarith_gt(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listarith_t *this = (listarith_t *)o;
  int i;

  if(this->right_type == fts_s_list)
    {
      if(this->right_list.ac < ac)
	ac = this->right_list.ac;
      
      list_set_size(&(this->list), ac);
      
      for(i=0; i<ac; i++)
	atom_gt(at + i, &(this->right_list.at[i]), &(this->list.at[i]));
    }
  else
    {
      list_set_size(&(this->list), ac);
      
      for(i=0; i<ac; i++)
	atom_gt(at + i, &(this->right_atom), &(this->list.at[i]));
    }
  
  fts_outlet_send(o, 0, fts_s_list, ac, this->list.at);
}

static void
listarith_ge(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listarith_t *this = (listarith_t *)o;
  int i;

  if(this->right_type == fts_s_list)
    {
      if(this->right_list.ac < ac)
	ac = this->right_list.ac;
      
      list_set_size(&(this->list), ac);
      
      for(i=0; i<ac; i++)
	atom_ge(at + i, &(this->right_list.at[i]), &(this->list.at[i]));
    }
  else
    {
      list_set_size(&(this->list), ac);
      
      for(i=0; i<ac; i++)
	atom_ge(at + i, &(this->right_atom), &(this->list.at[i]));
    }
  
  fts_outlet_send(o, 0, fts_s_list, ac, this->list.at);
}

static void
listarith_lt(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listarith_t *this = (listarith_t *)o;
  int i;

  if(this->right_type == fts_s_list)
    {
      if(this->right_list.ac < ac)
	ac = this->right_list.ac;
      
      list_set_size(&(this->list), ac);
      
      for(i=0; i<ac; i++)
	atom_lt(at + i, &(this->right_list.at[i]), &(this->list.at[i]));
    }
  else
    {
      list_set_size(&(this->list), ac);
      
      for(i=0; i<ac; i++)
	atom_lt(at + i, &(this->right_atom), &(this->list.at[i]));
    }
  
  fts_outlet_send(o, 0, fts_s_list, ac, this->list.at);
}

static void
listarith_le(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listarith_t *this = (listarith_t *)o;
  int i;

  if(this->right_type == fts_s_list)
    {
      if(this->right_list.ac < ac)
	ac = this->right_list.ac;
      
      list_set_size(&(this->list), ac);
      
      for(i=0; i<ac; i++)
	atom_le(at + i, &(this->right_list.at[i]), &(this->list.at[i]));
    }
  else
    {
      list_set_size(&(this->list), ac);
      
      for(i=0; i<ac; i++)
	atom_le(at + i, &(this->right_atom), &(this->list.at[i]));
    }
  
  fts_outlet_send(o, 0, fts_s_list, ac, this->list.at);
}

static void
listarith_ne(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listarith_t *this = (listarith_t *)o;
  int i;

  if(this->right_type == fts_s_list)
    {
      if(this->right_list.ac < ac)
	ac = this->right_list.ac;
      
      list_set_size(&(this->list), ac);
      
      for(i=0; i<ac; i++)
	atom_ne(at + i, &(this->right_list.at[i]), &(this->list.at[i]));
    }
  else
    {
      list_set_size(&(this->list), ac);
      
      for(i=0; i<ac; i++)
	atom_ne(at + i, &(this->right_atom), &(this->list.at[i]));
    }
  
  fts_outlet_send(o, 0, fts_s_list, ac, this->list.at);
}

static void
listarith_ee(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listarith_t *this = (listarith_t *)o;
  int i;

  if(this->right_type == fts_s_list)
    {
      if(this->right_list.ac < ac)
	ac = this->right_list.ac;
      
      list_set_size(&(this->list), ac);
      
      for(i=0; i<ac; i++)
	atom_ee(at + i, &(this->right_list.at[i]), &(this->list.at[i]));
    }
  else
    {
      list_set_size(&(this->list), ac);
      
      for(i=0; i<ac; i++)
	atom_ee(at + i, &(this->right_atom), &(this->list.at[i]));
    }
  
  fts_outlet_send(o, 0, fts_s_list, ac, this->list.at);
}

/*********************************************
 *
 *  object
 *
 */

static void
listarith_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listarith_t *this = (listarith_t *)o;

  list_init(&this->list);
  list_init(&this->right_list);

  list_set_size(&this->list, LIST_ALLOC_BLOCK);
  list_set_size(&this->right_list, LIST_ALLOC_BLOCK);

  if(ac == 2)
    listarith_set_right_atom(o, 0, 0, 1, at+1);
  else
    listarith_set_right_list(o, 0, 0, ac-1, at+1);

}

static void
listarith_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listarith_t *this = (listarith_t *)o;

  if(this->list.at)
    fts_block_free(this->list.at, this->list.alloc);
  if(this->right_list.at)
    fts_block_free(this->right_list.at, this->right_list.alloc);
}

/*********************************************
 *
 *  class
 *
 */

static fts_status_t
listarith_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];
  fts_symbol_t class_name = fts_get_symbol(at);

  /* initialization */
  fts_class_init(cl, sizeof(listarith_t), 2, 1, 0); 

  /* system methods */
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, listarith_init);
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, listarith_delete, 0, 0);

  /* user methods */
  fts_method_define_varargs(cl, 1, fts_s_list, listarith_set_right_list);

  fts_method_define_float(cl, 1, listarith_set_right_atom);
  fts_method_define_int(cl, 1, listarith_set_right_atom);
  fts_method_define_symbol(cl, 1, listarith_set_right_atom);

  if(class_name == fts_new_symbol("list+"))
    fts_method_define_varargs(cl, 0, fts_s_list, listarith_add);
  else if(class_name == fts_new_symbol("list-"))
    fts_method_define_varargs(cl, 0, fts_s_list, listarith_sub);
  else if(class_name == fts_new_symbol("list*"))
    fts_method_define_varargs(cl, 0, fts_s_list, listarith_mul);
  else if(class_name == fts_new_symbol("list/"))
    fts_method_define_varargs(cl, 0, fts_s_list, listarith_div);
  else if(class_name == fts_new_symbol("list>"))
    fts_method_define_varargs(cl, 0, fts_s_list, listarith_gt);
  else if(class_name == fts_new_symbol("list>="))
    fts_method_define_varargs(cl, 0, fts_s_list, listarith_ge);
  else if(class_name == fts_new_symbol("list<"))
    fts_method_define_varargs(cl, 0, fts_s_list, listarith_lt);
  else if(class_name == fts_new_symbol("list<="))
    fts_method_define_varargs(cl, 0, fts_s_list, listarith_le);
  else if(class_name == fts_new_symbol("list!="))
    fts_method_define_varargs(cl, 0, fts_s_list, listarith_ne);
  else if(class_name == fts_new_symbol("list=="))
    fts_method_define_varargs(cl, 0, fts_s_list, listarith_ee);
  else 
    return &fts_CannotInstantiate;

  /* outlet */
  fts_outlet_type_define_varargs(cl, 0,	fts_s_list);

  return fts_Success;
}

void
listarith_config(void)
{
  fts_class_install(fts_new_symbol("list+"), listarith_instantiate);
  fts_class_install(fts_new_symbol("list-"), listarith_instantiate);
  fts_class_install(fts_new_symbol("list*"), listarith_instantiate);
  fts_class_install(fts_new_symbol("list/"), listarith_instantiate);
  fts_class_install(fts_new_symbol("list<"), listarith_instantiate);
  fts_class_install(fts_new_symbol("list<="), listarith_instantiate);
  fts_class_install(fts_new_symbol("list>"), listarith_instantiate);
  fts_class_install(fts_new_symbol("list>="), listarith_instantiate);
  fts_class_install(fts_new_symbol("list!="), listarith_instantiate);
  fts_class_install(fts_new_symbol("list=="), listarith_instantiate);
}
