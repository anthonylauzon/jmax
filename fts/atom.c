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


/*
  Generic, may be debug, function to deal with atoms
 */

#include <string.h>

#include <fts/fts.h>
#include <ftsprivate/connection.h>
/*#include <ftsprivate/OLDftsdata.h>*/

static fts_hashtable_t fts_atom_type_table;

/* Currently, there is no way to extend this function when adding  new atom types */
void fprintf_atoms(FILE *f, int ac, const fts_atom_t *at)
{
  int i;

  for (i = 0; i < ac; i++)
    {
      char *ps;

      if (i == (ac-1))
	ps = "";
      else
	ps = " ";

      if (fts_is_symbol(&at[i]))
	fprintf(f,"%s%s", fts_symbol_name(fts_get_symbol(&at[i])), ps);
      else if (fts_is_int(&at[i]))
	fprintf(f,"%d%s", fts_get_int(&at[i]), ps);
      else if (fts_is_float(&at[i]))
	fprintf(f,"%f%s", fts_get_float(&at[i]), ps);
      else if (fts_is_ptr(&at[i]) )
	fprintf(f,"(ptr)%lx%s", (unsigned long) fts_get_ptr( &at[i]), ps);
      else if (fts_is_object(&at[i]))
	{
	  fts_object_t *obj;

	  obj = fts_get_object(&at[i]);

	  fprintf_object(f, obj);
	  fprintf(f,"%s", ps);
	}
      else if (fts_is_connection(&at[i]))
	{
	  fts_connection_t *c;

	  c = fts_get_connection(&at[i]);

	  fprintf_connection(f, c);
	  fprintf(f,"%s", ps);
	}
      /*else if (fts_is_data(&at[i]))
	{
	fts_data_t *obj;
	
	obj = fts_get_data(&at[i]);
	
	fprintf_data(f, obj);
	fprintf(f,"%s", ps);
	}
	else*/ if (fts_is_void(&at[i]))
	fprintf(f,"<void>%s", ps);
      else if (fts_is_error(&at[i]))
	fprintf(f,"<error>%s", ps);
      else if (fts_get_type(&at[i]))
	fprintf(f,"<%s>%lx%s", fts_symbol_name(fts_get_type(&at[i])), 
		(unsigned long) fts_get_ptr( &at[i]), ps);
      else
	fprintf(f,"<NULL TYPE>%lx%s", 
		(unsigned long) fts_get_ptr( &at[i]), ps);
    }
}

/* Currently, there is no way to extend this function when adding
   new atom types */

void sprintf_atoms(char *s, int ac, const fts_atom_t *at)
{
  int i;

  *s = '\0';

  for (i = 0; i < ac; i++)
    {
      char *ps;

      if (i == (ac-1))
	ps = "";
      else
	ps = " ";

      if (fts_is_symbol(&at[i]))
	sprintf(s + strlen(s),"%s%s", fts_symbol_name(fts_get_symbol(&at[i])), ps);
      else if (fts_is_int(&at[i]))
	sprintf(s + strlen(s),"%d%s", fts_get_int(&at[i]), ps);
      else if (fts_is_float(&at[i]))
	sprintf(s + strlen(s),"%f%s", fts_get_float(&at[i]), ps);
      else if (fts_is_ptr(&at[i]) )
	sprintf(s + strlen(s),"(ptr)%lx%s", (unsigned long) fts_get_ptr( &at[i]), ps);
      else if (fts_is_void(&at[i]))
	sprintf(s + strlen(s),"<void>%s", ps);
      else if (fts_is_error(&at[i]))
	sprintf(s + strlen(s),"<error>%s", ps);
      else if (fts_get_type(&at[i]))
	sprintf(s + strlen(s),"<%s>%lx%s", fts_symbol_name(fts_get_type(&at[i])), 
		(unsigned long) fts_get_ptr( &at[i]), ps);
      else
	sprintf(s + strlen(s),"<NULL TYPE>%lx%s", 
		(unsigned long) fts_get_ptr( &at[i]), ps);
    }
}

/* Currently, there is no way to extend this function when adding
   new atom types */

int fts_atom_are_equals(const fts_atom_t *a1, const fts_atom_t *a2)
{
  if (fts_same_types(a1, a2))
    {
      if (fts_is_void(a1))
	return fts_is_void(a2);
      if (fts_is_error(a1))
	return fts_is_error(a2);
      else if (fts_is_symbol(a1))
	return fts_get_symbol(a1) == fts_get_symbol(a2);
      else if (fts_is_string(a1))
	return ! strcmp(fts_get_string(a1), fts_get_string(a2));
      else if (fts_is_ptr(a1))
	return fts_get_ptr(a1) == fts_get_ptr(a2);
      else if (fts_is_int(a1))
	return fts_get_int(a1) == fts_get_int(a2);
      else if (fts_is_float(a1))
	return fts_get_float(a1) == fts_get_float(a2);
      else if (fts_is_object(a1))
	return fts_get_object(a1) == fts_get_object(a2);
      else if (fts_is_connection(a1))
	return fts_get_connection(a1) == fts_get_connection(a2);
      /*else if (fts_is_data(a1))
	return fts_get_data(a1) == fts_get_data(a2);*/
      else
	return 0;
    }
  else
    return 0;
}

int fts_atom_is_null(const fts_atom_t *a)
{
  if (fts_is_void(a))
    return 1;
  else if (fts_is_symbol(a))
    return fts_get_symbol(a) == 0;
  else if (fts_is_string(a))
    return fts_get_string(a) == 0;
  else if (fts_is_ptr(a))
    return fts_get_ptr(a) == 0;
  else if (fts_is_int(a))
    return fts_get_int(a) == 0;
  else if (fts_is_float(a))
    return fts_get_float(a) == 0.0;
  else if (fts_is_object(a))
    return fts_get_object(a) == 0;
  else if (fts_is_connection(a))
    return fts_get_connection(a) == 0;
  else
    return 0;
}

/* this function is declared by each  */
static int
fts_atom_type_function(int ac, const fts_atom_t *at, fts_atom_t *result)
{
  fts_symbol_t name = fts_get_symbol(at);
  fts_class_t *cl = 0;

  if(fts_atom_type_lookup(name, &cl))
    {
      if(cl)
	{
	  fts_object_t *obj = fts_object_create(cl, ac - 1, at + 1);
	  
	  if(obj)
	    {
	      fts_set_object_with_type(result, obj, name);
	      return FTS_EXPRESSION_OK;
	    }
	  else
	    return FTS_EXPRESSION_SYNTAX_ERROR;
	}
      else
	{
	  if(ac == 2 && name == fts_get_selector(at + 1))
	    {
	      *result = at[1];
	      return FTS_EXPRESSION_OK;
	    }
	  else
	    return FTS_EXPRESSION_SYNTAX_ERROR;
	}
    }

  return FTS_EXPRESSION_UNDEFINED_FUNCTION;
}

void
fts_atom_type_register(fts_symbol_t name, fts_class_t *cl)
{
  fts_atom_t k, v;

  fts_set_symbol( &k, name);
  fts_set_ptr(&v, cl);
  fts_hashtable_put(&fts_atom_type_table, &k, &v);

  fts_expression_declare_fun(name, fts_atom_type_function);
}

int
fts_atom_type_lookup(fts_symbol_t name, fts_class_t **cl)
{
  fts_atom_t a, k;

  fts_set_symbol( &k, name);
  if(fts_hashtable_get(&fts_atom_type_table, &k, &a))
    {
      *cl = (fts_class_t *)fts_get_ptr(&a);
      
      return 1;
    }

  return 0;
}

/***********************************************************************
 *
 * Initialization
 *
 */

void fts_kernel_atom_init(void)
{
  fts_hashtable_init( &fts_atom_type_table, 0, FTS_HASHTABLE_MEDIUM);
}
