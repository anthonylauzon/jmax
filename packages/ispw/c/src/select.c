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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell, Miller Puckette.
 *
 */



#include <fts/fts.h>

typedef struct {
  fts_object_t o;
  int n_compares;
  int *compare;
} select_multi_t;

typedef struct {
  fts_object_t o;
  int compare;
} select_t;


/***************************************
 *
 *  object
 *
 */

static void
select_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  select_t *this = (select_t *)o;
  int i;

  this->compare = fts_get_int_arg(ac, at, 1, 0);
}

static void
select_multi_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  select_multi_t *this = (select_multi_t *)o;
  int i;

  this->n_compares = ac - 1;
  this->compare = (int *)fts_malloc(this->n_compares * sizeof(int));

  for(i=0; i<this->n_compares; i++)
    this->compare[i] = fts_get_int_arg(ac, at, i+1, 0);    
}

static void
select_multi_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  select_multi_t *this = (select_multi_t *)o;

  fts_free(this->compare);
}


/***************************************
 *
 *  user methods
 *
 */

static void 
select_number_left(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  select_t *this = (select_t *)o;
  int number = fts_get_int_arg(ac, at, 0, 0);

  if(number == this->compare)
    fts_outlet_send(o, 0, fts_s_bang, 0, 0);
  else
    fts_outlet_int(o, 1, number);
}

static void 
select_number_right(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  select_t *this = (select_t *)o;
  this->compare = fts_get_int_arg(ac, at, 0, 0);
}

static void 
select_pair(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  select_t *this = (select_t *)o;
  int number, compare;

  number = fts_get_int_arg(ac, at, 0, 0);
  compare = fts_get_int_arg(ac, at, 1, 0);

  this->compare = compare;
  if(number == compare)
    fts_outlet_send(o, 0, fts_s_bang, 0, 0);
  else
    fts_outlet_int(o, 1, number);
}

static void
select_multi_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  select_multi_t *this = (select_multi_t *)o;
  int number;
  int i;

  number = fts_get_int_arg(ac, at, 0, 0);
  
  for(i=0; i<this->n_compares; i++)
    {
      if(number == this->compare[i])
	{
	  fts_outlet_send(o, i, fts_s_bang, 0, 0);
	  return;
	}
    }
  
  fts_outlet_int(o, i, number);
}

/***************************************
 *
 *  class 
 *
 */

static fts_status_t 
select_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int i;

  if(ac <= 2)
    {
      fts_class_init(cl, sizeof(select_t), 2, 2, 0);

      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, select_init);
      
      fts_method_define_varargs(cl, 0, fts_s_int, select_number_left);
      fts_method_define_varargs(cl, 0, fts_s_float, select_number_left);

      fts_method_define_varargs(cl, 1, fts_s_int, select_number_right);
      fts_method_define_varargs(cl, 1, fts_s_float, select_number_right);

      fts_method_define_varargs(cl, 0, fts_s_list, select_pair);
    }
  else /* select multi */
    {
      fts_class_init(cl, sizeof(select_multi_t), 1, ac, 0);

      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, select_multi_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, select_multi_delete);
      
      fts_method_define_varargs(cl, 0, fts_s_int, select_multi_number);
      fts_method_define_varargs(cl, 0, fts_s_float, select_multi_number);      
    }

  return fts_Success;
}

void
select_config(void)
{
  fts_metaclass_install(fts_new_symbol("select"), select_instantiate, fts_narg_equiv);
  fts_alias_install(fts_new_symbol("sel"), fts_new_symbol("select"));
}
