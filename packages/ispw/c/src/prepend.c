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
prepend_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  prepend_t *x = (prepend_t *)o;

  if(s == fts_s_set)
    prepend_set_realize(o, ac, at);
  else
    {
      fts_atom_t sat[max_prepend];
      int onset = 0;
      int n;

      if(s != NULL)
	{
	  fts_set_symbol(&sat[x->ac], s);	  
	  onset = 1;
	}

      if(ac + x->ac + onset >= max_prepend)
	ac = max_prepend - x->ac - onset;
      
      n = ac + x->ac + onset;
      memcpy(&sat[0], x->at, x->ac * sizeof(fts_atom_t));
      memcpy(&sat[x->ac + onset], at, ac * sizeof(fts_atom_t));

      fts_outlet_send(o, 0, x->presym, n, sat);
    }
}

static void
prepend_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(prepend_t), prepend_init, prepend_delete);

  fts_class_input_handler(cl, prepend_input);

  fts_class_outlet_thru(cl, 0);
  fts_class_outlet_varargs(cl, 0);
}

void
prepend_config(void)
{
  fts_class_install(fts_new_symbol("prepend"), prepend_instantiate);
}
