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



/* --------------------- metaclass integer: integer.c ---------------------- */

/* -------------------------- include files --------------------------------

   "fts.h"      defines all basic types and function prototypes

                 AND defines utility macros such as fts_get_int_arg()
   	         and useful symbol constants such as fts_s_int, fts_s_float ...

                 AND defines the macro versions of the most used message passing
		 functions: fts_outlet_send(), fts_outlet_int() and so on...
		 the macros will be defined if and only if the flag
		 OPTIMIZE is set at compilation time
*/

#include <fts/fts.h>

/* ---------------------- the integer object type ---------------------------

   a fts type object must be a structure which includes a 'fts_object_t' field
   at its first position; this is done to simulate the inheritance
*/

typedef struct {
  fts_object_t o; /* fts_object_t inheritance */
  long n;       /* the accumulator of a integer object */
} integer_t;

/* ---------------------- forward declarations ---------------------------- */

static void integer_bang(fts_object_t *, int, fts_symbol_t , int, const fts_atom_t *);
static void integer_float(fts_object_t *, int, fts_symbol_t , int, const fts_atom_t *);
static void integer_int(fts_object_t *, int, fts_symbol_t , int, const fts_atom_t *);
static void integer_in1(fts_object_t *, int, fts_symbol_t , int, const fts_atom_t *);
static void integer_float_1(fts_object_t *, int, fts_symbol_t , int, const fts_atom_t *);
static void integer_list(fts_object_t *, int, fts_symbol_t , int, const fts_atom_t *);

static void
integer_init(fts_object_t *, int, fts_symbol_t , int, const fts_atom_t *);

static fts_status_t
integer_instantiate(fts_class_t *, int, const fts_atom_t *);

/* ---------------- the metaclass configuration function ------------------ */

/* takes no arguments, returns nothing */

void
integer_config(void)
{
  /* create the class 'integer' */

  fts_class_install(fts_new_symbol("int"),integer_instantiate);

  /* ... and register 2 aliases for the "int" name: "i" and "integer" */

  fts_class_alias(fts_new_symbol("integer"), fts_new_symbol("int"));
  fts_class_alias(fts_new_symbol("i"), fts_new_symbol("int"));
}

/* ---------------- the metaclass instantiation function ------------------ */

/* fts_class_t *cl: the pointer to the class being instantiated
   int ac, const fts_atom_t *at: the instantiation arguments
   they are dummy as this function do not take them into account
*/

static fts_status_t
integer_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  /* initialize the class */
  fts_class_init(
		 cl, 		     /* the instantiated class pointer */
		 sizeof(integer_t),  /* the size of objects of this class */
		 2,		     /* the number of inlets */
		 1,		     /* the number of outlets */
		 0		     /* user_data member */
		 );

  a[0] = fts_s_symbol;
  a[1] = fts_s_int;

  /* define message template entries */
  fts_method_define_optargs(cl,
			    fts_SystemInlet, /* the system inlet */
			    fts_s_init,      /* the symbol "$init" */
			    integer_init,    /* a method defined in this module */
			    2, a,	     /* the argument types for this entry;
						in this case: one symbol and one optional argument of
						type long */
			    1	             /* one mandatory argument */
		    );
  a[0] = fts_s_int;

  fts_method_define(cl,
		    0,		 /* the inlet #0 */
		    fts_s_int,   /* the symbol "int" (from fts.h) */
		    integer_int, /* a method defined in this module */
		    1, a	 /* the argument types for this entry;
				    in this case: one argument of type long */
		    );

  a[0] = fts_s_float;
  fts_method_define(cl,
		    0,		 /* the inlet #0 */
		    fts_s_float,   /* the symbol "int" (from fts.h) */
		    integer_float, /* a method defined in this module */
		    1, a	 /* the argument types for this entry;
				    in this case: one argument of type long */
		    );

  fts_method_define_varargs(cl,
		    0,		 /* the inlet #0 */
		    fts_s_list,   /* the symbol "list" (from fts.h) */
		    integer_list /* a method defined in this module */
		    );

  fts_method_define(cl,
		    0,		  /* the inlet #0 */
		    fts_s_bang,   /* the symbol "bang" (from fts.h) */
		    integer_bang, /* a method defined in this module */
		    0, 0	  /* no arguments */
		    );

  a[0] = fts_s_int;
  fts_method_define(cl,
		    1,		  /* the inlet #1 */
		    fts_s_int,    /* the symbol "int" (from fts.h) */
		    integer_in1,  /* a method defined in this module */
		    1, a	  /* the argument types for this entry;
				     in this case: one argument of type long */
		    );

  a[0] = fts_s_float;
  fts_method_define(cl,
		    1,		 /* the inlet #0 */
		    fts_s_float,   /* the symbol "int" (from fts.h) */
		    integer_float_1, /* a method defined in this module */
		    1, a	 /* the argument types for this entry;
				    in this case: one argument of type long */
		    );

  /* defined outlet selector */

  a[0] = fts_s_int;
  fts_outlet_type_define(cl,
			 0,	    /* the outlet #0 */
			 fts_s_int, /* the symbol "int" (from fts.h) */
			 1, a	    /* the argument types for this outlet */
			 );

  return fts_Success;
}

/* ------------------------- the $init method --------------------------- */

/*
 fts_object_t *o:		the object the message is sent to
 int ac, const fts_atom_t *at:	the initialization arguments
*/

static void
integer_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  integer_t *this = (integer_t *) o;

  /* put the given value in the accumulator
     as this method takes a default long argument, if no argument is given
     one stores a 0 in the accumulator */
     
  /* one can cast the received object in a integer_t because the current
     method has been set for this type of object (in integer_instantiate) */
  this->n = fts_get_int_arg(ac, /* the atom array */
				      at, /* the number of atoms */
				      1,  /* the index in the array */
				      0   /* the default value in case of
					     the index of the array is greater
					     than the number of atoms */
				      );
}

/* ------------------------- the bang method ----------------------------- */

/*
 fts_object_t *o:		the object the message is sent to
 int winlet:      		the #inlet on which the message is sent to
 fts_symbol_t s:		the selector of the message
 int ac, const fts_atom_t *at:	the arguments of the message
*/

/* the bang method simply send the accumulator value to the outlet of
   the object */

static void
integer_bang(fts_object_t *o, int winlet, fts_symbol_t s,
	     int ac, const fts_atom_t *at)
{
  integer_t *this = (integer_t *) o;

  /* the easy and efficient way to do that, but no type checking is performed
     at runtime ! */

  fts_outlet_int(o, 0, this->n);
}

/* ------------------------- the int method ----------------------------- */

/* same arguments as integer_bang() */

/* the int method put the received value into the object accumulator and
   sent what it receives to its outlet */

static void
integer_int(fts_object_t *o, int winlet, fts_symbol_t s,
	    int ac, const fts_atom_t *at)
{
  integer_t *this = (integer_t *) o;

  this->n = fts_get_int(at);
  fts_outlet_send(o, 0, fts_s_int, ac, at);
}

/* ------------------------- the float method ----------------------------- */

/* same arguments as integer_bang() */

static void
integer_float(fts_object_t *o, int winlet, fts_symbol_t s,
	      int ac, const fts_atom_t *at)
{
  integer_t *this = (integer_t *) o;

  this->n = (long)fts_get_float_arg(ac, at, 0, 0);
  fts_outlet_int(o, 0, this->n);
}

/* ------------------------- the in1 method ----------------------------- */

/* same arguments as integer_bang() */

/* the in1 method simply set its accumulator */

static void
integer_in1(fts_object_t *o, int winlet, fts_symbol_t s, int ac,
	    const fts_atom_t *at)
{
  integer_t *this = (integer_t *) o;

  this->n = fts_get_int_arg(ac, at, 0, 0);
}

/* the float 1 method simply set its accumulator */

static void
integer_float_1(fts_object_t *o, int winlet, fts_symbol_t s, int ac,
	    const fts_atom_t *at)
{
  integer_t *this = (integer_t *) o;

  this->n = fts_get_float_arg(ac, at, 0, 0);
}


/* in case of list, only the first value is significative */

static void
integer_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if (fts_is_int(at))
    integer_int(o, winlet, fts_s_int, 1, at);
  else if (fts_is_float(at))
    integer_float(o, winlet, fts_s_float, 1, at);
}


