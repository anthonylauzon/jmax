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



/*
 * Extended with the keyword "t", that means thru; a t outlet
 * always reproduce exactly what sent in the input, including
 * messages different from the "canonic" types.
 * The forced conversion behaviour is not natural, but is kept
 * for backward compatibility.
 * Now there are no limit in the number of arguments.
 *
 */

#include <fts/fts.h>

typedef enum
{
  trigger_outlet_long,
  trigger_outlet_float,
  trigger_outlet_symbol,
  trigger_outlet_list,
  trigger_outlet_bang,
  trigger_outlet_thru
} trigger_outlet_t;

typedef struct 
{
  fts_object_t ob;
  int noutlets;
  trigger_outlet_t *trigger_outlet_table;
} trigger_t;


static void
trigger_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  trigger_t *x = (trigger_t *)o;
  int outlet = x->noutlets;

  while (outlet--)
    {
      trigger_outlet_t tot = x->trigger_outlet_table[outlet];
      
      if (tot == trigger_outlet_long)
	fts_outlet_send(o, outlet, s, ac, at);
      else if (tot == trigger_outlet_bang)
	fts_outlet_bang(o, outlet);
      else if (tot == trigger_outlet_float)
	fts_outlet_float(o, outlet, (float)fts_get_long(at));
      else if (tot == trigger_outlet_symbol)
	fts_outlet_symbol(o, outlet, fts_new_symbol(""));	/* ???? */
      else if (tot == trigger_outlet_list)
	fts_outlet_send(o, outlet, fts_s_list, ac, at);
      else if (tot == trigger_outlet_thru)
	fts_outlet_send(o, outlet, s, ac, at);
    }
}


/* Trigger bang is installed also for anything */

static void
trigger_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  trigger_t *x = (trigger_t *)o;
  int outlet = x->noutlets;

  while (outlet--)
    {
      trigger_outlet_t tot = x->trigger_outlet_table[outlet];

      if (tot == trigger_outlet_bang)
	fts_outlet_bang(o, outlet);
      else if (tot == trigger_outlet_long)
	fts_outlet_int(o, outlet, 0L);
      else if (tot == trigger_outlet_float)
	fts_outlet_float(o, outlet, 0.0f);
      else if (tot == trigger_outlet_symbol)
	fts_outlet_symbol(o, outlet, fts_new_symbol(""));	/* ???? */
      else if (tot == trigger_outlet_list)
	{
	  fts_atom_t at;
	  fts_set_long(&at, 0);
	  fts_outlet_list(o, outlet, 1, &at);
	}
      else if (tot == trigger_outlet_thru)
	fts_outlet_send(o, outlet, s, ac, at);
    }
}

static void
trigger_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  trigger_t *x = (trigger_t *)o;
  int outlet = x->noutlets;

  while (outlet--)
    {
      trigger_outlet_t tot = x->trigger_outlet_table[outlet];

      if (tot == trigger_outlet_float)
	fts_outlet_send(o, outlet, s, ac, at);
      else if (tot == trigger_outlet_long)
	fts_outlet_int(o, outlet, (long)fts_get_float(at));
      else if (tot == trigger_outlet_symbol)
	fts_outlet_symbol(o, outlet, fts_new_symbol(""));	/* ??? */
      else if (tot == trigger_outlet_list)
	fts_outlet_send(o, outlet, fts_s_list, ac, at);
      else if (tot == trigger_outlet_bang)
	fts_outlet_bang(o, outlet);
      else if (tot == trigger_outlet_thru)
	fts_outlet_send(o, outlet, s, ac, at);
    }
}

static void
trigger_symbol(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  trigger_t *x = (trigger_t *)o;
  int outlet = x->noutlets;

  while (outlet--)
    {
      trigger_outlet_t tot = x->trigger_outlet_table[outlet];

      if (tot == trigger_outlet_long)
	fts_outlet_int(o, outlet, 0L);
      else if (tot == trigger_outlet_float)
	fts_outlet_float(o, outlet, 0.0f);
      else if (tot == trigger_outlet_symbol)
	fts_outlet_send(o, outlet, s, ac, at);
      else if (tot == trigger_outlet_list)
	fts_outlet_send(o, outlet, fts_s_list, ac, at);
      else if (tot == trigger_outlet_bang)
	fts_outlet_bang(o, outlet);
      else if (tot == trigger_outlet_thru)
	fts_outlet_send(o, outlet, s, ac, at);
    }
}
  
static void
trigger_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if (!ac)
    return;
  else
    {
      trigger_t *x = (trigger_t *)o;
      int outlet = x->noutlets;

      while (outlet--)
	{
	  switch (x->trigger_outlet_table[outlet])
	    {
	    case trigger_outlet_long:
	      {
		if (fts_is_long(at))
		  fts_outlet_send(o, outlet, fts_s_int, 1, at);
		else if (fts_is_float(at))
		  fts_outlet_int(o, outlet, (long) fts_get_float(at));
		else
		  fts_outlet_int(o, outlet, 0L);
	      }
	      break;

	    case trigger_outlet_float:
	      {
		if (fts_is_long(at))
		  fts_outlet_float(o, outlet, (float) fts_get_long(at));
		else if (fts_is_float(at))
		  fts_outlet_send(o, outlet, fts_s_float, 1, at);
		else
		  fts_outlet_float(o, outlet, 0.0f);
	      }
	      break;

	    case trigger_outlet_symbol:
	      if (fts_is_symbol(at))
		fts_outlet_send(o, outlet, fts_s_symbol, 1, at);
	      else
		fts_outlet_symbol(o, outlet, fts_new_symbol(""));	/* ??? */
	      break;

	    case trigger_outlet_list:
	      fts_outlet_send(o, outlet, s, ac, at);
	      break;

	    case trigger_outlet_bang:
	      fts_outlet_bang(o, outlet);
	      break;

	    case trigger_outlet_thru:
	      fts_outlet_send(o, outlet, s, ac, at);
	      break;
	    }
	}
    }
}

/* Special method that is the only installed for trigger with only "b" arguments */

static void
trigger_all_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  trigger_t *x = (trigger_t *)o;
  int outlet = x->noutlets;

  while (outlet--)
    fts_outlet_send(o, outlet, fts_s_bang, 0, 0);
}


/* Special method that is the only installed for trigger with only "t" arguments */

static void
trigger_all_thru(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  trigger_t *x = (trigger_t *)o;
  int outlet = x->noutlets;

  while (outlet--)
    fts_outlet_send(o, outlet, s, ac, at);
}


/* The init method get the user data from the class and put
   a pointer to it in the object itself */

static void
trigger_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  trigger_t *x = (trigger_t *)o;

  x->trigger_outlet_table = (trigger_outlet_t *)fts_object_get_user_data(o);
  x->noutlets = ac - 1;		/* don't count the class name in the arguments !!! */
}

/* no delete method */

static fts_status_t
trigger_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a;
  int n;
  trigger_outlet_t *trigger_outlet_table;
  int noutlets;
  int all_bang, all_thru;

  ac--;at++;			/* throw away the class name argument */

  /* Create the trigger_outlet_table structure, to be put in the user data of the class 
     Looks for each argument; arguments can be:

     ARGUMENT   TYPE OF OUTLET
     'i'        int outlet
     'f'        float outlet
     'b'        bang outlet
     's'        symbol outlet
     'l'        list outlet
     't'        thru outlet
     int        int outlet
     float      float outlet
     other-symbol bang.
     Actually, in this table the symbol 'i' stay for any symbol
     starting with 'i'; this is awfull, but kept for backward 
     compatibility.

     Actually, the 't' introduce a rare incompatibility; in the
     old version, a trigger with a t outlet would creare a bang
     outlet, beacuse no error control was done.

     If there are no arguments, the trigger default to two long
     outlets (backward compatibility, it would make more sense having two 
     thru as default).
     */

  if (ac > 0)
    noutlets = ac;
  else
    noutlets = 2;

  trigger_outlet_table = fts_malloc(noutlets * sizeof(trigger_outlet_t));

  all_bang = 1;
  all_thru  = 1;

  if (ac > 0)
    for (n = 0; n < ac; at++, n++)
      {
	if (fts_is_float(at))
	  {
	    trigger_outlet_table[n] = trigger_outlet_float;
	    all_bang = 0;
	    all_thru = 0;
	  }
	else if (fts_is_long(at))
	  {
	    trigger_outlet_table[n] = trigger_outlet_long;
	    all_bang = 0;
	    all_thru = 0;
	  }
	else if (fts_is_symbol(at))
	  {
	    const char *s = fts_symbol_name(fts_get_symbol(at));

	    switch(s[0])
	      {
	      case 'i':
		trigger_outlet_table[n] = trigger_outlet_long;
		all_bang = 0;
		all_thru = 0;
		break;

	      case 'f':
		trigger_outlet_table[n] = trigger_outlet_float;
		all_bang = 0;
		all_thru = 0;
		break;

	      case 's':
		trigger_outlet_table[n] = trigger_outlet_symbol;
		all_bang = 0;
		all_thru = 0;
		break;

	      case 'b':
		trigger_outlet_table[n] = trigger_outlet_bang;
		all_thru = 0;
		break;

	      case 'l':
		trigger_outlet_table[n] = trigger_outlet_list;
		all_bang = 0;
		all_thru = 0;
		break;

	      case 't':
		trigger_outlet_table[n] = trigger_outlet_thru;
		all_bang = 0;
		break;

	      default:
		trigger_outlet_table[n] = trigger_outlet_bang;
		all_thru = 0;
		break;
	      }
	  }
      }
  else
    {
      trigger_outlet_table[0] = trigger_outlet_long;
      trigger_outlet_table[1] = trigger_outlet_long;
      all_bang = 0;
      all_thru = 0;
    }

  /* initialize the class: create as many outlets that there are arguments */

  fts_class_init(cl, sizeof(trigger_t), 1, noutlets, (void *)trigger_outlet_table);

  /* define message template entries */

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, trigger_init);

  if (all_bang)
    {
      fts_method_define_varargs(cl, 0, fts_s_anything, trigger_all_bang);
    }
  else if (all_thru)
    {
      fts_method_define_varargs(cl, 0, fts_s_anything, trigger_all_thru);
    }
  else
    {
      a = fts_s_int;
      fts_method_define(cl, 0, fts_s_int, trigger_int, 1, &a);

      a = fts_s_float;
      fts_method_define(cl, 0, fts_s_float, trigger_float, 1, &a);

      a = fts_s_symbol;
      fts_method_define(cl, 0, fts_s_symbol, trigger_symbol, 1, &a);

      fts_method_define(cl, 0, fts_s_bang, trigger_bang, 0, 0);

      fts_method_define_varargs(cl, 0, fts_s_list, trigger_list);
      fts_method_define_varargs(cl, 0, fts_s_anything, trigger_bang);
    }

  /* Type the outlet using the trigger_outlet_table */

  for (n = 0; n < noutlets;  n++)
    switch (trigger_outlet_table[n])
      {
      case trigger_outlet_long:
	{
	  fts_symbol_t a;
	    
	  a = fts_s_int;
	  fts_outlet_type_define(cl,  n,  fts_s_int, 1, &a);
	}
	break;
      case trigger_outlet_float:
	{
	  fts_symbol_t a;
	    
	  a = fts_s_float;
	  fts_outlet_type_define(cl,  n,  fts_s_float, 1, &a);
	}
	break;
      case trigger_outlet_symbol:
	{
	  fts_symbol_t a;
	  
	  a = fts_s_symbol;
	  fts_outlet_type_define(cl,  n,  fts_s_symbol, 1, &a);
	}
	break;
      case trigger_outlet_list:
	fts_outlet_type_define_varargs(cl,  n,  fts_s_list);
	break;

      case trigger_outlet_bang:
	fts_outlet_type_define(cl,  n,  fts_s_bang, 0, 0);
	break;

      case trigger_outlet_thru:
	/* no type in this case */
	break;
      }

    return fts_Success;
}


static int
trigger_atoms_conforms(const fts_atom_t *a1, const fts_atom_t *a2)
{
  if (fts_is_float(a1))
    return (fts_is_float(a2) || (fts_is_symbol(a2) && (fts_symbol_name(fts_get_symbol(a2))[0] == 'f')));
  else if (fts_is_long(a1))
    return (fts_is_long(a2) || (fts_is_symbol(a2) && (fts_symbol_name(fts_get_symbol(a2))[0] == 'i')));
  else if (fts_is_symbol(a1))
    {
      if  (fts_is_symbol(a2))
	return ((fts_symbol_name(fts_get_symbol(a1)))[0] == (fts_symbol_name(fts_get_symbol(a2)))[0]);
      else if (fts_is_float(a2))
	return ((fts_symbol_name(fts_get_symbol(a1)))[0] == 'f');
      else if (fts_is_long(a2))
	return ((fts_symbol_name(fts_get_symbol(a1)))[0] == 'i');
    }

  return 1;		/* should not be reached */
}


static int
trigger_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  /* skip the two class names */

  ac0--; ac1--;
  at0++; at1++;

  /* do the test */

  if (ac0 == 0)
    {
      if (ac1 == 0)
	return 1;
      else if (ac1 == 2)
	{
	  /* look if at1 is two longs */

	  fts_atom_t a;

	  fts_set_long(&a, 0);

	  return (trigger_atoms_conforms(&a, at1) &&
		  trigger_atoms_conforms(&a, at1 + 1));
	}
      else
	return 0;
    }
  else
    {
      if (ac1 == 0)
	{
	  if (ac0 == 2)
	    {
	      /* look if at0 is two longs */

	      fts_atom_t a;

	      fts_set_long(&a, 0);

	      return (trigger_atoms_conforms(&a, at0) &&
		      trigger_atoms_conforms(&a, at0 + 1));
	    }
	  else
	    return 0;
	}
      else if (ac1 == ac0)
	{
	  /* do the complete test */

	  int i;

	  for (i = 0; i < ac0; i++)
	    if (! trigger_atoms_conforms(at0 + i, at1 + i))
	      return 0;

	  return 1;
	}
      else
	return 0;
    }
}


void
trigger_config(void)
{
  fts_metaclass_install(fts_new_symbol("trigger"),trigger_instantiate, trigger_equiv);
  fts_metaclass_alias(fts_new_symbol("t"), fts_new_symbol("trigger"));
}

