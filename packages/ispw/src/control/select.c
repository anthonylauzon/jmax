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
  long *compare;
} select_multi_t;

typedef struct {
  fts_object_t o;
  long compare;
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

  this->compare = fts_get_long_arg(ac, at, 1, 0);
}

/* for multiple argument version */

static void
select_multi_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  select_multi_t *this = (select_multi_t *)o;
  int i;

  this->n_compares = ac - 1;
  this->compare = (long *)fts_malloc(this->n_compares * sizeof(long));
  for(i=0; i<this->n_compares; i++)
    this->compare[i] = fts_get_long_arg(ac, at, i+1, 0);    
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
  long number = (long)fts_get_int_arg(ac, at, 0, 0);

  if(number == this->compare)
    fts_outlet_send(o, 0, fts_s_bang, 0, 0);
  else
    fts_outlet_int(o, 1, number);
}

static void 
select_number_right(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  select_t *this = (select_t *)o;
  this->compare = (long)fts_get_int_arg(ac, at, 0, 0);
}

static void 
select_pair(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  select_t *this = (select_t *)o;
  long number, compare;

  number = (long)fts_get_int_arg(ac, at, 0, 0);
  compare = (long)fts_get_int_arg(ac, at, 1, 0);

  this->compare = compare;
  if(number == compare)
    fts_outlet_send(o, 0, fts_s_bang, 0, 0);
  else
    fts_outlet_int(o, 1, number);
}

/* for multiple argument version */

static void
select_multi_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  select_multi_t *this = (select_multi_t *)o;
  long number;
  int i;

  number = (long)fts_get_int_arg(ac, at, 0, 0);
  
  for(i=0; i<this->n_compares; i++)
    {
      if(number == this->compare[i]){
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
  fts_symbol_t a[2];
  int n_compares;
  int i;

  if(ac <= 2)
    {
      n_compares = 1;
      fts_class_init(cl, sizeof(select_t), 2, 2, 0);

      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, select_init);
      
      a[0] = fts_s_int;
      fts_method_define(cl, 0, fts_s_int, select_number_left, 1, a);
      a[0] = fts_s_float;
      fts_method_define(cl, 0, fts_s_float, select_number_left, 1, a);

      a[0] = fts_s_int;
      fts_method_define(cl, 1, fts_s_int, select_number_right, 1, a);
      a[0] = fts_s_float;
      fts_method_define(cl, 1, fts_s_float, select_number_right, 1, a);

      a[0] = fts_s_number;
      a[1] = fts_s_number;
      fts_method_define(cl, 0, fts_s_list, select_pair, 2, a);
    }
  else /* select multi */
    {
      n_compares = ac - 1;
      fts_class_init(cl, sizeof(select_multi_t), 1, n_compares + 1, 0);

      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, select_multi_init);
      fts_method_define(cl, fts_SystemInlet, fts_s_delete, select_multi_delete, 0, 0);
      
      a[0] = fts_s_int;
      fts_method_define(cl, 0, fts_s_int, select_multi_number, 1, a);
      a[0] = fts_s_float;
      fts_method_define(cl, 0, fts_s_float, select_multi_number, 1, a);      
    }

  /* type outlets */

  for (i=0; i<n_compares; i++)
    fts_outlet_type_define(cl, i, fts_s_bang, 0, 0);
  a[0] = fts_s_int;
  fts_outlet_type_define(cl, i, fts_s_int, 1, a);

  return fts_Success;
}

void
select_config(void)
{
  fts_metaclass_install(fts_new_symbol("select"), select_instantiate, fts_narg_equiv);
  fts_metaclass_alias(fts_new_symbol("sel"), fts_new_symbol("select"));
}