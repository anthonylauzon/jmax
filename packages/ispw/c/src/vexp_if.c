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

/*------------------------- expr class -------------------------------------*/

extern int ex_new(struct expr *expr, char *exp_string);

/*#define EXPR_DEBUG*/

static void expr_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);

/* static utils functions: could be macros */
static void
expr_int_realize(struct expr *x, long int n, int in)
{
  if (in > MAX_VARS) {
    post("expr: int: inlet out of range %d\n", in + 1);
    return;
  }
  if (x->exp_var[in].ex_type == ET_II)
    x->exp_var[in].ex_int = n;
  else if (x->exp_var[in].ex_type == ET_FI)
    x->exp_var[in].ex_flt = n;
  else if (in) 
    post("expr: int: bad type for inlet %d\n", in + 1);
}

static void
expr_float_realize(struct expr *x, double n, int in)
{
  if (in > MAX_VARS) {
    post("expr: float: inlet out of range %d\n", in + 1);
    return;
  }

  if (x->exp_var[in].ex_type == ET_FI)
    x->exp_var[in].ex_flt = n;
  else if (x->exp_var[in].ex_type == ET_II)
    x->exp_var[in].ex_int = n;
  else if (in) 
    post("expr: float: bad type for inlet %d\n", in + 1);
}

static void
expr_sym_realize(struct expr *x, fts_symbol_t s, int in)
{
  if (in > MAX_VARS) {
    post("expr: symbol: inlet out of range %d\n", in + 1);
    return;
  }
  if (x->exp_var[in].ex_type == ET_SI)
    x->exp_var[in].ex_ptr = (char *)s;
  else if (in) 
    post("expr: symbol: bad type for inlet %d\n", in + 1);
}


/* Called only for list on inlet 0 */

static void
expr_list_realize(struct expr *x, int argc, const fts_atom_t *argv)
{
  int i;

  if (argc > MAX_VARS)
    {
      post("expr: list: too many values\n");
      return;
    }

  for (i = argc - 1; i >= 0; i--)
    {
      if (fts_is_int(&argv[i]))
	expr_int_realize(x, fts_get_int(&argv[i]), i);
      else if (fts_is_float(&argv[i]))
	expr_float_realize(x, fts_get_float(&argv[i]), i);
      else if (fts_is_symbol(&argv[i]))
	expr_sym_realize(x, fts_get_symbol(&argv[i]), i);
    }
}


/* method definitions */
static void
expr_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
}

static void
expr_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  expr_int_realize((t_expr *)o, fts_get_int(at), 0);
  expr_bang(o, winlet, s, ac, at);
}

static void
expr_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  expr_float_realize((t_expr *)o, fts_get_float(at), 0);
  expr_bang(o, winlet, s, ac, at);
}

static void
expr_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  expr_list_realize((t_expr *)o, ac, at);
  expr_bang(o, winlet, s, ac, at);
}

static void
expr_symbol(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  expr_sym_realize((t_expr *)o, fts_get_symbol_arg(ac, at, 0, 0), 0);
  expr_bang(o, winlet, s, ac, at);
}

static void
expr_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  t_expr *x = (t_expr *)o;

#ifdef EXPR_DEBUG
  {
    int i;
    struct ex_ex *eptr;

    for (i = 0, eptr = x->exp_var;  ; eptr++, i++)
      {
	if (!eptr->ex_type)
	  break;
	switch (eptr->ex_type)
	  {
	  case ET_II:
	    fts_log("ET_II: %d \n", eptr->ex_int);
	    break;

	  case ET_FI:
	    fts_log("ET_FT: %f \n", eptr->ex_flt);
	    break;

	  default:
	    fts_log("oups\n");
	  }
      }
  }
#endif

  if (!ex_eval(x, x->exp_stack, &x->exp_res))
    {
      /*       fts_log("expr_bang(error evaluation)\n"); */
      return;
    }
  

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

static void
expr_setint(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  expr_int_realize((t_expr *)o, fts_get_int_arg(ac, at, 0, 0), winlet);
}

static void
expr_setfloat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  expr_float_realize((t_expr *)o, (double) fts_get_float_arg(ac, at, 0, 0) , winlet);
}

static void
expr_setsymb(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  expr_sym_realize((t_expr *)o, fts_get_symbol_arg(ac, at, 0, 0), winlet);
}


static void
expr_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  t_expr *udata = (t_expr *)fts_object_get_user_data(o);
  t_expr *e = (t_expr *)o;

  e->exp_stack = udata->exp_stack;
  e->exp_res   = udata->exp_res;
  memcpy(e->exp_var, udata->exp_var, sizeof(struct ex_ex)*MAX_VARS);
}


static fts_status_t
expr_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  const char *buf;
  struct expr *x;
  int i, mark;
  int varnum;
  struct ex_ex *eptr;

  if (!ac)
    return &fts_ArgumentMissing;

  x = (t_expr *)fts_zalloc(sizeof(t_expr)); /* never freed ? */
  x->exp_stack = (struct ex_ex *)0;

  buf = fts_get_symbol(at);

  if (ex_new(x, buf))
    {
      post("expr: error in `%s'\n", buf);
      return &fts_ArgumentMissing;
    }

  /* computing the var number, before initing the class */
  mark = 0;
  varnum = 0;
  for (i = MAX_VARS-1, eptr = &x->exp_var[MAX_VARS-1]; i >= 0 ; i--, eptr--)
    if (eptr->ex_type || mark)
      {
	mark = 1;
	varnum++;
      }

  /* declaring the class */
  fts_class_init(cl, sizeof(t_expr), (varnum ? varnum : 1), 1, (void *)x);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, expr_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, expr_delete);

  fts_method_define_varargs(cl, 0, fts_s_int, expr_int);
  fts_method_define_varargs(cl, 0, fts_s_float, expr_float);
  fts_method_define_varargs(cl, 0, fts_s_bang, expr_bang);

  fts_method_define_varargs(cl, 0, fts_s_symbol, expr_symbol);
  fts_method_define_varargs(cl, 0, fts_s_list, expr_list);

  mark = 0;
  for (i = MAX_VARS-1, eptr = &x->exp_var[MAX_VARS-1]; i > 0 ; i--, eptr--)
    {
      switch (eptr->ex_type)
	{
	case 0:
	  if (mark)
	    {
	      fts_method_define_varargs(cl, i, fts_s_int, expr_setint);
	      fts_method_define_varargs(cl, i, fts_s_float, expr_setint);
	    }
	  break;

	case ET_II:
	  mark = 1;
	  fts_method_define_varargs(cl, i, fts_s_int, expr_setint);
	  fts_method_define_varargs(cl, i, fts_s_float, expr_setint);
	  break;

	case ET_FI:
	  mark = 1;
	  fts_method_define_varargs(cl, i, fts_s_float, expr_setfloat);
	  fts_method_define_varargs(cl, i, fts_s_int, expr_setfloat);
	  break;

	case ET_SI:
	  mark = 1;
	  fts_method_define_varargs(cl, i, fts_s_symbol, expr_setsymb);
	  break;

	default:
	  post("expr: bad type (%lx) inlet = %d\n", eptr->ex_type, i);
	  return &fts_ArgumentTypeMismatch;
	}
    }

  return fts_Success;
}

void
expr_config(void)
{
  fts_metaclass_install(fts_new_symbol("expr"),expr_instantiate, fts_never_equiv);
}
