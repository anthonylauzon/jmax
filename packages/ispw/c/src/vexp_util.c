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


#include <stdio.h>		/* for sprintf */
#include <fts/fts.h>
#include <fts/packages/data/data.h>
#include "vexp.h"
#include "vexp_util.h"
#include "naming.h"

#define	MAXBUF	256

/*
   * max_ex_tab -- evaluate this table access
   *		 eptr is the name of the table and arg is the index we
   *		 have to put the result in optr
   *		 return 1 on error and 0 otherwise
   *
   * Arguments:
   *  the expr object
   *  table 
   *  the argument 
   *  the result pointer 
   */
int
max_ex_tab(struct expr *exp, fts_symbol_t s, struct ex_ex *arg, struct ex_ex *optr)
{
  fts_object_t *obj = ispw_get_object_by_name(s);
  ivec_t *tw = 0;
  long i;
  float rest;
  
  if(obj && (fts_object_get_class_name(obj) == ivec_symbol))
    tw = (ivec_t *)obj;

  if (! tw)
    {
      optr->ex_type = ET_INT;
      optr->ex_int = 0;
      fts_object_error((fts_object_t *) exp, "expr: %s: can't find table\n", s);
      return 1;
    }

  optr->ex_type = ET_INT;

  if (! ivec_get_size(tw)) {
    optr->ex_int = 0;
    return (0);
  } 

  switch (arg->ex_type) {
  case ET_INT:

    optr->ex_int =  ivec_get_element(tw, arg->ex_int % ivec_get_size(tw)); 
    break;
  case ET_FLT:
    /* CHANGE are we zero based or one based? */

    i = (int)arg->ex_flt % ivec_get_size(tw); 

    optr->ex_int =  ivec_get_element(tw,i);
    if (i  == ivec_get_size(tw) - 1)
      break;

    /*
     * if the argument is float and it is not at the end of
     * the table do a interpolation with the fraction 
     */
    rest = arg->ex_flt - (int)arg->ex_flt;
    optr->ex_int+=(int) ((float)(ivec_get_element(tw,i)- ivec_get_element(tw,i+1))*rest);
    break;

  default:	/* do something with strings */
    fts_object_error((fts_object_t *) exp, "expr: bad argument for table '%s'\n", s);
    return (1);
  }
  return (0);
}

ivec_t *
ex_get_ivec_by_name(fts_symbol_t name)
{  
  fts_object_t *obj = ispw_get_object_by_name(name);

  if(obj && (fts_object_get_class_name(obj) == ivec_symbol))
    return (ivec_t *)obj;
  else
    return 0;
}

int ex_getsym(char *p, fts_symbol_t *s)
{
  *s = fts_new_symbol(p);
  return(*s ? 0 : 1);
}

const char *
ex_symname(fts_symbol_t s)
{
  return (s);
}


/*
 * argstostr -- transform the argc,argv argument pair of max that is
 *		passed to the creation function of an object to a string
 * Args:
 *  number of argument 
 *  argument array 
 *  the string buffer
 * 
 * Don't know who wrote this sub system, but the existence of such
 * a function is totally demential. MDC
 */


void
argstostr(int argc, const fts_atom_t *argv, char *buf, int size)
{
  char *p = buf;
  const char *s;
  int i;
  
  if (size <= 0)
    {
      post("argstostr: bad size\n");
      return;
    }
  
  *p = 0;
  i = 0;

  while (argc--)
    {
      if (fts_is_int(argv))
	{
	  sprintf(p, "%d ", fts_get_int(argv));
	  while (*p)
	    {
	      if (++i >= size)
		{
		  *p = 0;
		  post("argstostr: args truncated\n");
		  return;
		}
	      p++;
	    }
	}
      else if (fts_is_float(argv))
	{
	  sprintf(p, "%f ", fts_get_float(argv));
	  while (*p)
	    {
	      if (++i >= size)
		{
		  *p = 0;
		  post("argstostr: args truncated\n");
		  return;
		}
	      p++;
	    }
	}
      else if (fts_is_symbol(argv))
	{
	  char c;
	  int quoted = 0;

	  s = fts_get_symbol(argv);

	  while ((c = *s++))
	    {
	      if (quoted)
		{
		  quoted = 0;
		  *p++ = c;
		}
	      else
		{
		  if (++i >= size - 1)
		    {
		      *p = 0;
		      post("argstostr: args truncated\n");
		      return;
		    }

		  if (c == '\\')
		    {
		      quoted = 1;
		    }
		  else
		    *p++ = c;
		}
	    }

	  *p = 0;
	}
      else if (fts_is_void(argv))
	{
	}
      else
	post("argstostr: wrong type %ld\n", fts_get_class_name(argv));

      argv++;
    }
}
