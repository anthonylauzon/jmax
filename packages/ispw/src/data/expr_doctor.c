/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
/*
  Expr doctor.

  Fix expr boxes by avoiding new expression evaluation in their
  arguments; also, it translate the list of arguments in a single
  argument, a symbol including the expression; this was actually
  done anyway in the exp object; not the expr accept a single
  symbol argument
 */

#include "fts.h"

static fts_symbol_t get_expr_symbol(int argc, const fts_atom_t *argv);


static fts_object_t *expr_doctor(fts_patcher_t *patcher, int ac, const fts_atom_t *at)
{
  if (ac >= 1)
    {
      fts_atom_t a[2];
      fts_object_t *obj;

      fts_set_symbol(&a[0], fts_new_symbol("expr"));
      fts_set_symbol(&a[1], get_expr_symbol(ac - 1, at + 1));
      fts_make_object(patcher, 2, a, &obj);

      if (obj == 0)
	obj = fts_error_object_new(patcher, 2, a, "Error in expression");
      else
	fts_object_set_description(obj, 2, a);

      return obj;
    }
  else
    return 0;
}


void expr_doctor_init(void)
{
  fts_register_object_doctor(fts_new_symbol("expr"), expr_doctor);
}
    

/*
 * From the old vexp code ...
 *
 * argstostr -- transform the argc,argv argument pair of  that is
 *		passed to the creation function of an object to a symbol
 * Args:
 *  number of argument 
 *  argument array 
 * 
 */


static fts_symbol_t get_expr_symbol(int argc, const fts_atom_t *argv)
{
  char buf[1024];
  int size = 1024;
  char *p = buf;
  const char *s;
  int i;
  
  *p = 0;
  i = 0;

  while (argc--)
    {
      if (fts_is_long(argv))
	{
	  sprintf(p, "%d ", fts_get_int(argv));
	  while (*p)
	    {
	      if (++i >= size)
		{
		  *p = 0;
		  return fts_new_symbol_copy(buf);
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
		  return fts_new_symbol_copy(buf);
		}
	      p++;
	    }
	}
      else if (fts_is_symbol(argv))
	{
	  char c;
	  int quoted = 0;

	  s = (const char *)fts_symbol_name(fts_get_symbol(argv));

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
		      return fts_new_symbol_copy(buf);
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

      argv++;
    }

  return fts_new_symbol_copy(buf);
}








