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


/*
 * vexp_func.c -- this file include all the functions for vexp.
 *		  the first two arguments to the function are the number
 *		  of argument and an array of arguments (argc, argv)
 *		  the last argument is a pointer to a struct ex_ex for
 *		  the result.  Up do this point, the content of the
 *		  struct ex_ex that these functions receive are either
 *		  ET_INT (long), ET_FLT (float), or ET_SYM (char **, it is
 *		  char ** and not char * since NewHandle of Mac returns
 *		  a char ** for relocatability.)  The common practice in
 *		  these functions is that they figure out the type of their
 *		  result according to the type of the arguments. In general
 *		  the ET_SYM is used an ET_INT when we expect a value.
 *		  It is the users responsibility not to pass strings to the
 *		  function.
 */

#include <stdlib.h>

#define __STRICT_BSD__
#include <math.h>
#undef __STRICT_BSD__

#include "fts.h"
#include "table.h"
#include "vexp.h"
#include "vexp_util.h"

/* forward declarations */

static void ex_min(long int argc, struct ex_ex *argv, struct ex_ex *optr);
static void ex_max(long int argc, struct ex_ex *argv, struct ex_ex *optr);
static void ex_toint(long int argc, struct ex_ex *argv, struct ex_ex *optr);
static void ex_tofloat(long int argc, struct ex_ex *argv, struct ex_ex *optr);
static void ex_pow(long int argc, struct ex_ex *argv, struct ex_ex *optr);
static void ex_exp(long int argc, struct ex_ex *argv, struct ex_ex *optr);
static void ex_log(long int argc, struct ex_ex *argv, struct ex_ex *optr);
static void ex_ln(long int argc, struct ex_ex *argv, struct ex_ex *optr);
static void ex_sin(long int argc, struct ex_ex *argv, struct ex_ex *optr);
static void ex_cos(long int argc, struct ex_ex *argv, struct ex_ex *optr);
static void ex_asin(long int argc, struct ex_ex *argv, struct ex_ex *optr);
static void ex_acos(long int argc, struct ex_ex *argv, struct ex_ex *optr);
static void ex_tan(long int argc, struct ex_ex *argv, struct ex_ex *optr);
static void ex_atan(long int argc, struct ex_ex *argv, struct ex_ex *optr);
static void ex_sinh(long int argc, struct ex_ex *argv, struct ex_ex *optr);
static void ex_cosh(long int argc, struct ex_ex *argv, struct ex_ex *optr);
static void ex_asinh(long argc, struct ex_ex *argv, struct ex_ex *optr);
static void ex_acosh(long argc, struct ex_ex *argv, struct ex_ex *optr);
static void ex_tanh(long int argc, struct ex_ex *argv, struct ex_ex *optr);
static void ex_atanh(long argc, struct ex_ex *argv, struct ex_ex *optr);
static void ex_atan2(long int argc, struct ex_ex *argv, struct ex_ex *optr);
static void ex_size(long int argc, struct ex_ex *argv, struct ex_ex *optr);
static void ex_sum(long int argc, struct ex_ex *argv, struct ex_ex *optr);
static void ex_Sum(long int argc, struct ex_ex *argv, struct ex_ex *optr);
static void ex_avg(long int argc, struct ex_ex *argv, struct ex_ex *optr);
static void ex_Avg(long int argc, struct ex_ex *argv, struct ex_ex *optr);
static void ex_sqrt(long int argc, struct ex_ex *argv, struct ex_ex *optr);
static void ex_fact(long int argc, struct ex_ex *argv, struct ex_ex *optr);
static void ex_store(long int argc, struct ex_ex *argv, struct ex_ex *optr);
static void ex_random(long int argc, struct ex_ex *argv, struct ex_ex *optr);

t_ex_func ex_funcs[] = {
  {"min",	ex_min,		2},	/* min */
  {"max",	ex_max,		2},	/* max */
  {"int",	ex_toint,	1},	/* convert to integer */
  {"float",	ex_tofloat,	1},	/* convert to float */
  {"pow",	ex_pow,		2},	/* power */
  {"sqrt",	ex_sqrt,	1},	/* sqrt */
  {"exp",	ex_exp,		1},	/* power of e to x */
  {"log10",	ex_log,		1},	/* log */
  {"ln",	ex_ln,		1},	/* nat. log */
  {"log",	ex_ln,		1},	/* nat. log */
  {"sin",	ex_sin,		1},	/* sinus */
  {"cos",	ex_cos,		1},	/* cosin */
  {"tan",	ex_tan,		1},	/* tan */
  {"asin",	ex_asin,	1},	/* asin */
  {"acos",	ex_acos,	1},	/* acos */
  {"atan",	ex_atan,	1},	/* atan */
  {"atan2",	ex_atan2,	2},	/* atan2 */
  {"sinh",	ex_sinh,	1},	/* hyperbolic sinus */
  {"cosh",	ex_cosh,	1},	/* hyperbolic cosin */
  {"tanh",	ex_tanh,	1},	/* hyperbolic tan */
  {"fact",	ex_fact,	1},	/* factorial */
  {"random",	ex_random,	2},	/* random number */
  {"asinh",	ex_asinh,	1},	/* hyperbolic asin */
  {"acosh",	ex_acosh,	1},	/* hyperbolic acos */
  {"atanh",	ex_atanh,	1},	/* hyperbolic atan */
  {"size",	ex_size,	1},	/* size of table */
  {"sum",	ex_sum,		1},	/* sum of contect of a table */
  {"Sum",	ex_Sum,		3},	/* sum with boundries */
  {"avg",	ex_avg,		1},	/* avarage of contect of table*/
  {"Avg",	ex_Avg,		3},	/* avarage with boundries */
  {"store",	ex_store,	3},	/* store in the table */
  {0,		0,		0}
};

/*
 * ex_min -- the type of the result is the type of the smaller value
 */

static void
ex_min(long int argc, struct ex_ex *argv, struct ex_ex *optr)
{
  struct ex_ex *left, *right;
  
  left = argv++;
  right = argv;
  if (left->ex_type == ET_FLT) {
    if (right->ex_type == ET_FLT) {
      if (left->ex_flt < right->ex_flt)
	*optr = *left;
      else
	*optr = *right;
    } else {
      if (left->ex_flt < (float)right->ex_int)
	*optr = *left;
      else
	*optr = *right;
    }
  } else {
    if (right->ex_type == ET_FLT) {
      if ((float)left->ex_int < right->ex_flt)
	*optr = *left;
      else
	*optr = *right;
    } else {
      if (left->ex_int < right->ex_int)
	*optr = *left;
      else
	*optr = *right;
    }
  }
}

/*
 *ex_max -- the type of the result is the type of the larger value
 */

static void
ex_max(long int argc, struct ex_ex *argv, struct ex_ex *optr)
{
  struct ex_ex *left, *right;
  
  left = argv++;
  right = argv;
  if (left->ex_type == ET_FLT) {
    if (right->ex_type == ET_FLT) {
      if (left->ex_flt > right->ex_flt)
	*optr = *left;
      else
	*optr = *right;
    } else {
      if (left->ex_flt > (float)right->ex_int)
	*optr = *left;
      else
	*optr = *right;
    }
  } else {
    if (right->ex_type == ET_FLT) {
      if ((float)left->ex_int > right->ex_flt)
	*optr = *left;
      else
	*optr = *right;
    } else {
      if (left->ex_int > right->ex_int)
	*optr = *left;
      else
	*optr = *right;
    }
  }
}

/*
 * ex_toint -- convert to integer
 */

static void
ex_toint(long int argc, struct ex_ex *argv, struct ex_ex *optr)
{
  if (argv->ex_type == ET_INT)
    *optr = *argv;
  else if (argv->ex_type == ET_FLT) {
    optr->ex_type = ET_INT;
    optr->ex_int = (int)argv->ex_flt;
  } else {
    optr->ex_type = ET_INT;
    optr->ex_int = (int)argv->ex_ptr;
  }
}

/*
 * ex_tofloat -- convert to float
 */

static void
ex_tofloat(long int argc, struct ex_ex *argv, struct ex_ex *optr)
{
  if (argv->ex_type == ET_FLT)
    *optr = *argv;
  else {
    optr->ex_type = ET_FLT;
    optr->ex_flt = (float)argv->ex_int;
  }
}


/*
 * ex_pow -- the power of
 */

static void
ex_pow(long int argc, struct ex_ex *argv, struct ex_ex *optr)
{
  struct ex_ex *left, *right;
  
  left = argv++;
  right = argv;
  optr->ex_type = ET_FLT;
  if (left->ex_type == ET_FLT) {
    if (right->ex_type == ET_FLT) {
      optr->ex_flt = (float)
	pow((double)left->ex_flt,(double)right->ex_flt);
    } else {
      optr->ex_flt = (float)
	pow((double)left->ex_flt,(double)right->ex_int);
    }
  } else {
    if (right->ex_type == ET_FLT) {
      optr->ex_flt = (float)
	pow((double)left->ex_int,(double)right->ex_flt);
    } else {
      optr->ex_type = ET_INT;
      optr->ex_int = (int) pow((double)left->ex_int,(double)right->ex_int);
    }
  }
}

/*
 * ex_sqrt -- square root
 */

static void
ex_sqrt(long int argc, struct ex_ex *argv, struct ex_ex *optr)
{
  optr->ex_type = ET_FLT;
  if (argv->ex_type == ET_FLT)
    optr->ex_flt = (float)sqrt((double)argv->ex_flt);
  else
    optr->ex_flt = (float)sqrt((double)argv->ex_int);
}

/*
 * ex_exp -- e to the power of
 */

static void
ex_exp(long int argc, struct ex_ex *argv, struct ex_ex *optr)
{
  optr->ex_type = ET_FLT;
  if (argv->ex_type == ET_FLT)
    optr->ex_flt = (float)exp((double)argv->ex_flt);
  else
    optr->ex_flt = (float)exp((double)argv->ex_int);
}

/*
 * ex_log -- 10 based logarithm
 */

static void
ex_log(long int argc, struct ex_ex *argv, struct ex_ex *optr)
{
  optr->ex_type = ET_FLT;
  if (argv->ex_type == ET_FLT)
    optr->ex_flt = (float)log10((double)argv->ex_flt);
  else
    optr->ex_flt = (float)log10((double)argv->ex_int);
}

/*
 * ex_ln -- natural log
 */

static void
ex_ln(long int argc, struct ex_ex *argv, struct ex_ex *optr)
{
  optr->ex_type = ET_FLT;
  if (argv->ex_type == ET_FLT)
    optr->ex_flt = (float)log((double)argv->ex_flt);
  else
    optr->ex_flt = (float)log((double)argv->ex_int);
}


static void
ex_sin(long int argc, struct ex_ex *argv, struct ex_ex *optr)
{
  optr->ex_type = ET_FLT;
  if (argv->ex_type == ET_FLT)
    optr->ex_flt = (float)sin((double)argv->ex_flt);
  else
    optr->ex_flt =  (float)sin((double)argv->ex_int);
}


static void
ex_cos(long int argc, struct ex_ex *argv, struct ex_ex *optr)
{
  optr->ex_type = ET_FLT;
  if (argv->ex_type == ET_FLT)
    optr->ex_flt = (float)cos((double)argv->ex_flt);
  else
    optr->ex_flt = (float)cos((double)argv->ex_int);
}


static void
ex_tan(long int argc, struct ex_ex *argv, struct ex_ex *optr)
{
  optr->ex_type = ET_FLT;
  if (argv->ex_type == ET_FLT)
    optr->ex_flt = (float)tan((double)argv->ex_flt);
  else
    optr->ex_flt = (float)tan((double)argv->ex_int);
}

static void
ex_asin(long int argc, struct ex_ex *argv, struct ex_ex *optr)
{
  optr->ex_type = ET_FLT;
  if (argv->ex_type == ET_FLT)
    optr->ex_flt = (float)asin((double)argv->ex_flt);
  else
    optr->ex_flt = (float)asin((double)argv->ex_int);
}

static void
ex_acos(long int argc, struct ex_ex *argv, struct ex_ex *optr)
{
  optr->ex_type = ET_FLT;
  if (argv->ex_type == ET_FLT)
    optr->ex_flt = (float)acos((double)argv->ex_flt);
  else
    optr->ex_flt = (float)acos((double)argv->ex_int);
}


static void
ex_atan(long int argc, struct ex_ex *argv, struct ex_ex *optr)
{
  optr->ex_type = ET_FLT;
  if (argv->ex_type == ET_FLT)
    optr->ex_flt = (float)atan((double)argv->ex_flt);
  else
    optr->ex_flt = (float)atan((double)argv->ex_int);
}

/*
 *ex_atan2 --
 */

static void
ex_atan2(long int argc, struct ex_ex *argv, struct ex_ex *optr)
{
  struct ex_ex *left, *right;
  
  left = argv++;
  right = argv;
  optr->ex_type = ET_FLT;
  if (left->ex_type == ET_FLT) {
    if (right->ex_type == ET_FLT) {
      optr->ex_flt = (float)atan2((double)left->ex_flt,
				  (double)right->ex_flt);
    } else {
      optr->ex_flt = (float)atan2((double)left->ex_flt,
				  (double)right->ex_int);
    }
  } else {
    if (right->ex_type == ET_FLT) {
      optr->ex_flt = (float)atan2((double)left->ex_int,
				  (double)right->ex_flt);
    } else {
      optr->ex_flt = (float)atan2((double)left->ex_int,
				  (double)right->ex_int);
    }
  }
}


static void
ex_sinh(long int argc, struct ex_ex *argv, struct ex_ex *optr)
{
  optr->ex_type = ET_FLT;
  if (argv->ex_type == ET_FLT)
    optr->ex_flt = (float)sinh((double)argv->ex_flt);
  else
    optr->ex_flt = (float)sinh((double)argv->ex_int);
}

static void
ex_cosh(long int argc, struct ex_ex *argv, struct ex_ex *optr)
{
  optr->ex_type = ET_FLT;
  if (argv->ex_type == ET_FLT)
    optr->ex_flt = (float)cosh((double)argv->ex_flt);
  else
    optr->ex_flt = (float)cosh((double)argv->ex_int);
}


static void
ex_tanh(long int argc, struct ex_ex *argv, struct ex_ex *optr)
{
  optr->ex_type = ET_FLT;
  if (argv->ex_type == ET_FLT)
    optr->ex_flt = (float)tanh((double)argv->ex_flt);
  else
    optr->ex_flt = (float)tanh((double)argv->ex_int);
}


static void
ex_asinh(long argc, struct ex_ex *argv, struct ex_ex *optr)
{
  optr->ex_type = ET_FLT;
  if (argv->ex_type == ET_FLT)
    optr->ex_flt = (float)asinh((double)argv->ex_flt);
  else
    optr->ex_flt = (float)asinh((double)argv->ex_int);
}

static void
ex_acosh(long argc, struct ex_ex *argv, struct ex_ex *optr)
{
  optr->ex_type = ET_FLT;
  if (argv->ex_type == ET_FLT)
    optr->ex_flt = (float)acosh((double)argv->ex_flt);
  else
    optr->ex_flt = (float)acosh((double)argv->ex_int);
}

static void
ex_atanh(long argc, struct ex_ex *argv, struct ex_ex *optr)
{
  optr->ex_type = ET_FLT;
  if (argv->ex_type == ET_FLT)
    optr->ex_flt = (float)atanh((double)argv->ex_flt);
  else
    optr->ex_flt = (float)atanh((double)argv->ex_int);
}


static void
ex_fact(long int argc, struct ex_ex *argv, struct ex_ex *optr)
{
  long i;
  
  if (argv->ex_type == ET_FLT)
    i = (long)argv->ex_flt;
  else
    i = argv->ex_int;

  optr->ex_type = ET_INT;

  if (i) 
    optr->ex_int = 1;
  else
    {
      optr->ex_int = 0;
      return;
    }

  do
    {
      optr->ex_int *= i;
    } while (--i);
}


/*
 * ex_random -- return a random number
 */

static void
ex_random(long int argc, struct ex_ex *argv, struct ex_ex *optr)
{
  int i1, i2;

  if (argv->ex_type == ET_FLT)
    i1 = (int)argv->ex_flt;
  else
    i1 = argv->ex_int;

  if (argv[1].ex_type == ET_FLT)
    i2 = (int)argv[1].ex_flt;
  else
    i2 = argv[1].ex_int;

  optr->ex_type = ET_INT;
  optr->ex_int = i1 + (((i2 - i1) * (rand() & 0x7fffL)) >> 15);
}


/* Previously in vexp_util.c (why ???? !!! ) */

/*
 * ex_size -- find the size of a table
 */

static void
ex_size(long int argc, struct ex_ex *argv, struct ex_ex *optr)
{
  fts_symbol_t s;
  int_vector_t *tw = 0;
  
  if (argv->ex_type != ET_SYM)
    {
      post("expr: size: need a table name\n");
      optr->ex_type = ET_INT;
      optr->ex_int = 0;
    }
  
  s = (fts_symbol_t ) argv->ex_ptr;

  tw = table_int_vector_get_by_name(s);

  if (tw)
    {
      optr->ex_type = ET_INT;
      optr->ex_int = int_vector_get_size(tw);
    }
  else
    {
      optr->ex_type = ET_INT;
      optr->ex_int = 0;
      post("expr: size: no such table %s\n", fts_symbol_name(s));
    }
}

/* utility function */

/*
 * ex_sum -- calculate the sum of all elements of a table
 */

static void
ex_sum(long int argc, struct ex_ex *argv, struct ex_ex *optr)
{
  fts_symbol_t s;
  int_vector_t *tw = 0;
  
  if (argv->ex_type != ET_SYM)
    {
      post("expr: sum: need a table name\n");
      optr->ex_type = ET_INT;
      optr->ex_int = 0;
    }
  s = (fts_symbol_t ) argv->ex_ptr;

  tw = table_int_vector_get_by_name(s);

  if (tw)
    {
      optr->ex_type = ET_INT;
      optr->ex_int = int_vector_get_sum(tw);
    }
  else
    {
      optr->ex_type = ET_INT;
      optr->ex_int = 0;
      post("expr: sum: no such table %s\n", fts_symbol_name(s));
    }
}


/*
 * ex_Sum -- calculate the sum of table with the given boundries
 */

static void
ex_Sum(long int argc, struct ex_ex *argv, struct ex_ex *optr)
{
  fts_symbol_t s;
  int_vector_t *tw = 0;
  
  if (argv->ex_type != ET_SYM)
    {
      post("expr: Sum: need a table name\n");
      optr->ex_type = ET_INT;
      optr->ex_int = 0;
    }
  
  s = (fts_symbol_t ) (argv++)->ex_ptr;

  tw = table_int_vector_get_by_name(s);

  if (! tw)
    {
      optr->ex_type = ET_INT;
      optr->ex_int = 0;
      post("expr: Sum: no such table %s\n", fts_symbol_name(s));
      return;
    }


  if (argv->ex_type != ET_INT || argv[1].ex_type != ET_INT)
    {
      post("expr: Sum: boundries have to be fix values\n");
      optr->ex_type = ET_INT;
      optr->ex_int = 0;
      return;
    }
  
  optr->ex_type = ET_INT;
  optr->ex_int = int_vector_get_sub_sum(tw, argv->ex_int, argv[1].ex_int);
}

/*
 * ex_avg -- calculate the avarage of a table
 */

static void
ex_avg(long int argc, struct ex_ex *argv, struct ex_ex *optr)
{
  fts_symbol_t s;
  int_vector_t *tw = 0;
  
  if (argv->ex_type != ET_SYM)
    {
      post("expr: avg: need a table name\n");
      optr->ex_type = ET_INT;
      optr->ex_int = 0;
    }

  s = (fts_symbol_t ) argv->ex_ptr;

  tw = table_int_vector_get_by_name(s);

  if (tw)
    {
      optr->ex_type = ET_INT;

      if (! int_vector_get_size(tw))
	optr->ex_int = 0;
      else
	optr->ex_int = int_vector_get_sum(tw) / int_vector_get_size(tw);
    }
  else
    {
      optr->ex_type = ET_INT;
      optr->ex_int = 0;
      post("expr: avg: no such table %s\n", fts_symbol_name(s));
    }
}


/*
 * ex_Avg -- calculate the avarage of table with the given boundries
 */

static void
ex_Avg(long int argc, struct ex_ex *argv, struct ex_ex *optr)
{
  fts_symbol_t s;
  int_vector_t *tw = 0;
  
  if (argv->ex_type != ET_SYM)
    {
      post("expr: Avg: need a table name\n");
      optr->ex_type = ET_INT;
      optr->ex_int = 0;
    }

  s = (fts_symbol_t ) (argv++)->ex_ptr;

  tw = table_int_vector_get_by_name(s);

  if (! tw)
    {
      optr->ex_type = ET_INT;
      optr->ex_int = 0;
      post("expr: Avg: no such table %s\n", fts_symbol_name(s));
      return;
    }

  if (argv->ex_type != ET_INT || argv[1].ex_type != ET_INT)
    {
      post("expr: Avg: boundries have to be fix values\n");
      optr->ex_type = ET_INT;
      optr->ex_int = 0;
      return;
    }

  optr->ex_type = ET_INT;

  if (argv[1].ex_int - argv->ex_int <= 0)
    optr->ex_int = 0;
  else
    optr->ex_int = (int_vector_get_sub_sum(tw, argv->ex_int, argv[1].ex_int) /
		    (argv[1].ex_int - argv->ex_int));
}

/*
 * ex_store -- store a value in a table
 *			   if the index is greater the size of the table, we will make a modulo the
 *			   size of the table
 */

static void
ex_store(long int argc, struct ex_ex *argv, struct ex_ex *optr)
{
  fts_symbol_t s;
  int_vector_t *tw = 0;
  
  if (argv->ex_type != ET_SYM)
    {
      post("expr: store: need a table name\n");
    }

  s = (fts_symbol_t ) (argv++)->ex_ptr;

  tw = table_int_vector_get_by_name(s);

  if (! tw)
    {
      optr->ex_type = ET_INT;
      optr->ex_int = 0;
      post("expr: store: no such table %s\n", fts_symbol_name(s));
      return;
    }

  if (argv->ex_type != ET_INT || argv[1].ex_type != ET_INT)
    {
      post("expr: store: arguments have to be integer\n");
      optr->ex_type = ET_INT;
      optr->ex_int = 0;
    }

  int_vector_set_element(tw, argv->ex_int < 0 ? 0 : argv->ex_int % int_vector_get_size(tw), argv[1].ex_int);
  *optr = argv[1]; 
}

