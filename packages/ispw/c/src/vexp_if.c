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


#include <stdio.h>
#include <string.h>

#include <fts/fts.h>
#include "vexp.h"
#include "vexp_util.h"

extern struct ex_ex *ex_eval(struct expr *exp, struct ex_ex *eptr, struct ex_ex *optr);
extern int ex_new(struct expr *expr, char *exp_string);

static void
expr_atom_realize(struct expr *x, const fts_atom_t *at, int in)
{
  if (in > MAX_VARS) 
    {
      fts_object_signal_runtime_error((fts_object_t *)x, "inlet out of range %d\n", in + 1);
      return;
    }

  if(fts_is_number(at))
    {
      if (x->exp_var[in].ex_type == ET_II)
	x->exp_var[in].ex_int = fts_get_number_int(at);
      else if (x->exp_var[in].ex_type == ET_FI)
	x->exp_var[in].ex_flt = fts_get_number_float(at);
      else if(in)
	fts_object_signal_runtime_error((fts_object_t *)x, "bad type for inlet %d\n", in + 1);
    }
  else if(fts_is_symbol(at))
    {
      if (x->exp_var[in].ex_type == ET_SI)
	x->exp_var[in].ex_ptr = (char *)fts_get_symbol(at);
      else if(in)
	fts_object_signal_runtime_error((fts_object_t *)x, "bad type for inlet %d\n", in + 1);
    }
}

static void
expr_atoms_realize(struct expr *x, int argc, const fts_atom_t *argv)
{
}

static void
expr_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  t_expr *x = (t_expr *)o;

  if (winlet == 0 && ex_eval(x, x->exp_stack, &x->exp_res))
    {
      switch(x->exp_res.ex_type)
	{
	case ET_INT:
	  fts_outlet_int((fts_object_t *)x, 0, x->exp_res.ex_int);
	  break;
	  
	case ET_FLT:
	  fts_outlet_float((fts_object_t *)x, 0, x->exp_res.ex_flt);
	  break;
	  
	case ET_SYM:
	  /* CHANGE this will have to be taken care of */
	  
	default:
	  post("expr: bang: unrecognized result %ld\n", x->exp_res.ex_type);
	}
    }
}

static void
expr_atom(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  expr_atom_realize((t_expr *)o, at, winlet);

  if(winlet == 0)
    expr_bang(o, 0, s, ac, at);
}

static void
expr_atoms(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  t_expr *x = (t_expr *)o;

  if(winlet == 0)
    {
      int i;
      
      if (ac > MAX_VARS)
	{
	  post("expr: too many values\n");
	  return;
	}
      
      for (i=ac-1; i>=0; i--)
	{
	  if (fts_is_int(at + i))
	    expr_atom_realize(x, at + i, i);
	  else if (fts_is_float(at + i))
	    expr_atom_realize(x, at + i, i);
	  else if (fts_is_symbol(at + i))
	    expr_atom_realize(x, at + i, i);
	}
      
      expr_bang(o, 0, s, ac, at);
    }
}

static void
expr_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  t_expr *e = (t_expr *)o;

  if(ac > 0)
    {
      char *buf = (char *)fts_get_symbol(at);
      struct ex_ex *eptr;

      e->exp_stack = (struct ex_ex *)0;
      
      if(ex_new(e, buf) == 0)
	{
	  int mark = 0;
	  int varnum = 0;
	  int i;

	  /* computing the var number, before initing the class */
	  for (i = MAX_VARS-1, eptr = &e->exp_var[MAX_VARS-1]; i >= 0 ; i--, eptr--)
	    {
	      if (eptr->ex_type || mark)
		{
		  mark = 1;
		  varnum++;
		}
	    }
	  
	  if(varnum < 1)
	    varnum = 1;
	  
	  fts_object_set_inlets_number(o, varnum);

	  return;
	}
    }

  fts_object_set_error(o, "bad arguments");
}

static void
expr_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
}

static void
expr_instantiate(fts_class_t *cl)
{
  /* declaring the class */
  fts_class_init(cl, sizeof(t_expr), expr_init, expr_delete);

  fts_class_inlet_bang(cl, 0, expr_bang);

  fts_class_inlet_int(cl, 0, expr_atom);
  fts_class_inlet_float(cl, 0, expr_atom);
  fts_class_inlet_symbol(cl, 0, expr_atom);
  fts_class_inlet_varargs(cl, 0, expr_atoms);

  fts_class_outlet_number(cl, 0);
}

void
expr_config(void)
{
  fts_class_install(fts_new_symbol("expr"), expr_instantiate);
}
