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
 *   ARGUMENT   TYPE OF OUTPUT
 *   'i'        int outlet
 *   'f'        float outlet
 *   'b'        bang outlet
 *   's'        symbol outlet
 *   'l'        list outlet
 *   't'        thru outlet
 *   int        int outlet
 *   float      float outlet
 *   other-symbol bang.
 *
 *   Actually, in this table the symbol 'i' stay for any symbol
 *   starting with 'i'; this is awfull, but kept for backward 
 *   compatibility.
 *
 *   Actually, the 't' introduce a rare incompatibility; in the
 *   old version, a trigger with a t outlet would creare a bang
 *   outlet, beacuse no error control was done.
 *
 *   If there are no arguments, the trigger default to two int
 *   outlets (backward compatibility, it would make more sense 
 *   having two thru as default).
 *
 */

#include <fts/fts.h>

typedef enum
{
  trigger_outlet_int,
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
trigger_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  trigger_t *x = (trigger_t *)o;
  int outlet = x->noutlets;

  while (outlet--)
    {
      switch (x->trigger_outlet_table[outlet])
	{
	case trigger_outlet_int:
	  {
	    int x = 0;
	    
	    if (fts_is_number(at))
	      x = fts_get_number_int(at);
	    
	    fts_outlet_int(o, outlet, x);
	  }
	  break;
	  
	case trigger_outlet_float:
	  {
	    double x = 0.0;
	    
	    if (fts_is_number(at))
	      x = fts_get_number_float(at);
	    
	    fts_outlet_float(o, outlet, x);
	  }
	  break;
	  
	case trigger_outlet_symbol:
	  if (fts_is_symbol(at))
	    fts_outlet_symbol(o, outlet, fts_get_symbol(at));
	  else
	    fts_outlet_symbol(o, outlet, fts_new_symbol(""));
	  break;
	  
	case trigger_outlet_list:
	  fts_outlet_varargs(o, outlet, ac, at);
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

static void
trigger_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  trigger_t *x = (trigger_t *)o;
  trigger_outlet_t *trigger_outlet_table;
  int noutlets;
  int n;

  if (ac > 0)
    noutlets = ac;
  else
    noutlets = 2;

  trigger_outlet_table = fts_malloc(noutlets * sizeof(trigger_outlet_t));

  if (ac > 0)
    for (n = 0; n < ac; at++, n++)
      {
	if (fts_is_float(at))
	  trigger_outlet_table[n] = trigger_outlet_float;
	else if (fts_is_int(at))
	  trigger_outlet_table[n] = trigger_outlet_int;
	else if (fts_is_symbol(at))
	  {
	    const char *s = fts_get_symbol(at);

	    switch(s[0])
	      {
	      case 'i':
		trigger_outlet_table[n] = trigger_outlet_int;
		break;

	      case 'f':
		trigger_outlet_table[n] = trigger_outlet_float;
		break;

	      case 's':
		trigger_outlet_table[n] = trigger_outlet_symbol;
		break;

	      case 'b':
		trigger_outlet_table[n] = trigger_outlet_bang;
		break;

	      case 'l':
		trigger_outlet_table[n] = trigger_outlet_list;
		break;

	      case 't':
		trigger_outlet_table[n] = trigger_outlet_thru;
		break;

	      default:
		trigger_outlet_table[n] = trigger_outlet_bang;
		break;
	      }
	  }
      }
  else
    {
      trigger_outlet_table[0] = trigger_outlet_int;
      trigger_outlet_table[1] = trigger_outlet_int;
    }

  x->trigger_outlet_table = trigger_outlet_table;
  x->noutlets = ac;

  fts_object_set_outlets_number(o, noutlets);
}

static void
trigger_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  trigger_t *x = (trigger_t *)o;

  fts_free(x->trigger_outlet_table);
}

static void
trigger_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(trigger_t), trigger_init, trigger_delete);

  fts_class_inlet_varargs(cl, 0, trigger_anything);
  fts_class_set_default_handler(cl, trigger_anything);

  fts_class_outlet_anything(cl, 0);
}


void
trigger_config(void)
{
  fts_metaclass_t *mcl = fts_class_install(fts_new_symbol("trigger"), trigger_instantiate);
  fts_class_alias(mcl, fts_new_symbol("t"));
}

