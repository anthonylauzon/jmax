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


/* class prepend */

#include <fts/fts.h>
#include <math.h>
#include <string.h>

enum {
  ilow = 0,
  ihi,
  olow,
  ohi,
  logincr,
  nvalues
};

typedef struct prepend_t {
  fts_object_t _o;
  fts_symbol_t presym;
  int ac;
  fts_atom_t *at;
} prepend_t;


#define max_prepend 32

static void
prepend_set_realize(fts_object_t *o, int ac, const fts_atom_t *at)
{
  prepend_t *x = (prepend_t *)o;

  if (x->at)
    fts_free(x->at);

  if (ac >= max_prepend)
    ac = max_prepend;

  x->presym = 0;

  if (ac && fts_is_symbol(at))
    {
      x->presym = fts_get_symbol(at);
      at++;
      ac--;
    }

  if (ac)
    {
      x->ac = ac;
      x->at = (fts_atom_t *)fts_malloc(sizeof(fts_atom_t)*ac);
      memcpy(x->at, at, sizeof(fts_atom_t)*ac);
    }
  else
    {
      x->ac = 0;
      x->at = 0;
    }
}

static void
prepend_init(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  prepend_set_realize(o, ac, at);
}

static void
prepend_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  prepend_t *x = (prepend_t *)o;

  if (x->at)
    fts_free(x->at);
}

static void
prepend_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  prepend_set_realize(o, ac, at);
}

static void
prepend_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  prepend_t *x = (prepend_t *)o;
  fts_atom_t sat[max_prepend];
  int n;

  if (ac + x->ac >= max_prepend)
    {
      post("prepend: input list too long (total %d max)\n", max_prepend);
      ac = max_prepend - x->ac;
    }

  n = ac + x->ac;
  memcpy(&sat[0],     x->at, x->ac * sizeof(fts_atom_t));
  memcpy(&sat[x->ac], at,    ac    * sizeof(fts_atom_t));

  if (x->presym)
    fts_outlet_send(o, 0, x->presym, n, sat);
  else
    fts_outlet_atoms(o, 0, n, sat);
}

/* As list, but prepend the selector; shadock shadock, but for compatibility 
   with Max 0.26
*/

static void
prepend_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  prepend_t *x = (prepend_t *)o;
  fts_atom_t sat[max_prepend];
  int n;

  if (ac + x->ac + 1 >= max_prepend)
    {
      post("prepend: input list too long (total %d max)\n", max_prepend);
      ac = max_prepend - x->ac - 1;
    }

  n = ac + x->ac + 1;
  memcpy(&sat[0],     x->at, x->ac * sizeof(fts_atom_t));
  fts_set_symbol(&sat[x->ac], s);
  memcpy(&sat[x->ac + 1], at,    ac    * sizeof(fts_atom_t));

  if (x->presym)
    fts_outlet_send(o, 0, x->presym, n, sat);
  else
    fts_outlet_atoms(o, 0, n, sat);
}

static fts_status_t
prepend_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(prepend_t), 1, 1, 0);

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, prepend_init);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_delete, prepend_delete);

  fts_method_define_varargs(cl, 0, fts_s_set, prepend_set);

  fts_method_define_varargs(cl, 0, fts_s_int, prepend_list);
  fts_method_define_varargs(cl, 0, fts_s_float, prepend_list);
  fts_method_define_varargs(cl, 0, fts_s_symbol, prepend_list);
  fts_method_define_varargs(cl, 0, fts_s_list, prepend_list);
  fts_method_define_varargs(cl, 0, fts_s_anything, prepend_anything);

  return fts_ok;
}

void
prepend_config(void)
{
  fts_class_install(fts_new_symbol("prepend"),prepend_instantiate);
}
