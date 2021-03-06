/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

/* Voice allocator; from the original Miller Puckette code */


#include <fts/fts.h>

/* <a> totally rewritten to add voice-stealing */

typedef struct voice
{
  int state;
  int pitch;
} voice_t;

typedef struct poly
{
  fts_object_t ob;
  int in1;
  int nvoice;
  int count;
  struct voice *v;
  int strat;
} poly_t;


/* pitch or other unique index */

static void
poly_number(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  poly_t *x = (poly_t *)o;
  int n = (int) fts_get_int_arg(ac, at, 0, 0);
  int i;
  voice_t *v;

  if (x->in1)	/* if velo nonzero */
    {
      if (x->strat)
	{
	  voice_t *von = 0;
	  voice_t *voff = 0;
	  int ion = 0;
	  int ioff = 0;

	  for (i = 0, v = x->v; i < x->nvoice; i++, v++)
	    {
	      if (v->state & 1)
		{
		  if ((! von) || (v->state < von->state))
		    {
		      von = v;
		      ion = i;
		    }
		}
	      else
		{
		  if ((! voff) || (v->state < voff->state))
		    {
		      voff = v;
		      ioff = i;
		    }
		}
	    }
	  
	  if (! voff)	/* if all notes are on steal one */
	    {
	      int pwas = von->pitch;

	      von->state = ((x->count++) * 2) + 1;
	      von->pitch = n;
	      ioff       = ion;

	      fts_outlet_int(o, 2, 0L);
	      fts_outlet_int(o, 1, pwas);
	      fts_outlet_int(o, 0, ion+1);
	    }
	  else
	    {
	      voff->state = ((x->count++) * 2) + 1;
	      voff->pitch = n;
	    }

	  fts_outlet_int(o, 2, x->in1);
	  fts_outlet_int(o, 1, n);
	  fts_outlet_int(o, 0, ioff+1);
	}
      else	/* strategy zero: no voice stealing */
	{
	  for (i = 0, v = x->v; i < x->nvoice; i++, v++)
	    {
	      if (! (v->state & 1))
		{
		  v->state = 1;
		  v->pitch = n;
		  
		  fts_outlet_int(o, 2, x->in1);
		  fts_outlet_int(o, 1, n);
		  fts_outlet_int(o, 0, i+1);

		  return;
		}
	    }
	}
    }
  else	/* note off */
    {
      for (i = 0, v = x->v; i < x->nvoice; i++, v++)
	{
	  if ((v->pitch == n) && (v->state & 1))
	    {
	      v->state = (x->count++)<<1;

	      fts_outlet_int(o, 2, 0L);
	      fts_outlet_int(o, 1, n);
	      fts_outlet_int(o, 0, i+1);

	      return;
	    }
	}
    }
}

static void
poly_number_1(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  poly_t *x = (poly_t *)o;
  int n = (int) fts_get_int_arg(ac, at, 0, 0);

  x->in1 = ((n > 0) ? n : 0);
}


static void
poly_list(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  if ((ac > 1) && (fts_is_int(at+1) || fts_is_float(at+1)))
    poly_number_1(o, s, 1, at + 1, fts_nix);

  if ((ac > 0) && (fts_is_int(at) || fts_is_float(at)))
    poly_number(o, s, 1, at, fts_nix);
}

static void
poly_stop(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  poly_t *x = (poly_t *)o;
  int i;
  voice_t *v;

  for (i = 0, v = x->v; i < x->nvoice; i++, v++)
    if (v->state & 1)
      {
	fts_outlet_int(o, 2, 0L);
	fts_outlet_int(o, 1, v->pitch);
	fts_outlet_int(o, 0, i+1);
	v->state = (x->count++) * 2;
      }
}

static void
poly_clear(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  poly_t *x = (poly_t *)o;
  int i;
  voice_t *v;

  for (v = x->v, i = x->nvoice; i--; v++)
    {
      v->pitch = 0;
      v->state = 0;
    }
}

static void
poly_set(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  poly_t *x = (poly_t *)o;
  int pit = fts_get_int_arg(ac, at, 0, 0);
  int vel = fts_get_int_arg(ac, at, 1, 0);
  int which = fts_get_int_arg(ac, at, 2, 0);
  voice_t *v;

  v = x->v + (which - 1);

  if ((which > x->nvoice) || (which < 1))
    {
      fts_post("poly: 'set' voice %ld is out of range\n", which);
      return;
    }

  v->pitch = pit;

  if (vel)
    v->state = ((x->count++) * 2) + 1;
  else
    v->state = (x->count++) * 2;
}

static void
poly_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  poly_t *x  = (poly_t *)o;
  int n = fts_get_int_arg(ac, at, 0, 0);
  int strat = fts_get_int_arg(ac, at, 1, 0);
  int i;
  voice_t *v;

  if (n < 1)
    x->nvoice = 16;
  else
    x->nvoice = n;

  x->v = (voice_t *) fts_malloc(x->nvoice * sizeof(voice_t));

  for (v = x->v, i=n; i--; v++)
    v->pitch = v->state = 0;

  x->in1 = 0;
  x->strat = strat;
}


static void
poly_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  poly_t *x = (poly_t *)o;

  fts_free((void *)x->v);
}

static void
poly_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(poly_t), poly_init, poly_delete);

  fts_class_message_varargs(cl, fts_s_stop, poly_stop);
  fts_class_message_varargs(cl, fts_s_clear, poly_clear);

  fts_class_message_varargs(cl, fts_s_set, poly_set);
  fts_class_inlet_varargs(cl, 0, poly_list);
  fts_class_inlet_int(cl, 0, poly_number);
  fts_class_inlet_float(cl, 0, poly_number);

  fts_class_inlet_int(cl, 1, poly_number_1);
  fts_class_inlet_float(cl, 1, poly_number_1);

  fts_class_outlet_int(cl, 0);
  fts_class_outlet_int(cl, 1);
  fts_class_outlet_int(cl, 2);    
}

void
poly_config(void)
{
  fts_class_t *cl = fts_class_install(fts_new_symbol("poly"), poly_instantiate);
  fts_class_alias(cl, fts_new_symbol("loco"));
}

