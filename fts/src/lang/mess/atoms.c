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
  Generic, may be debug, function to deal with atoms
 */

#include "sys.h"
#include "lang/mess.h"

const fts_atom_t fts_null = FTS_NULL;

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
      else if (fts_is_long(&at[i]))
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
      else if (fts_is_data(&at[i]))
	{
	  fts_data_t *obj;

	  obj = fts_get_data(&at[i]);

	  fprintf_data(f, obj);
	  fprintf(f,"%s", ps);
	}
      else if (fts_is_void(&at[i]))
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
      else if (fts_is_long(&at[i]))
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
      else if (fts_is_long(a1))
	return fts_get_long(a1) == fts_get_long(a2);
      else if (fts_is_float(a1))
	return fts_get_float(a1) == fts_get_float(a2);
      else if (fts_is_object(a1))
	return fts_get_object(a1) == fts_get_object(a2);
      else if (fts_is_data(a1))
	return fts_get_data(a1) == fts_get_data(a2);
      else if (fts_is_true(a1))
	return fts_is_true(a2);
      else if (fts_is_false(a1))
	return fts_is_false(a2);
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
  else if (fts_is_long(a))
    return fts_get_long(a) == 0;
  else if (fts_is_float(a))
    return fts_get_float(a) == 0.0;
  else if (fts_is_object(a))
    return fts_get_object(a) == 0;
  else if (fts_is_true(a))
    return 0;
  else if (fts_is_false(a))
    return 1;
  else
    return 0;
}

/* Functions to compare list of atoms */
int fts_atom_is_subsequence(int sac, const fts_atom_t *sav, int ac, const fts_atom_t *av)
{
  int i,j;

  for (i = 0; i < (ac - sac + 1); i++)
    if (fts_atom_are_equals(&sav[0], &av[i]))
      {
	/* Found the beginning, test the rest */
	
	for (j = 1; j < sac; j++)
	  if (! fts_atom_are_equals(&sav[j], &av[j + i]))
	    return 0;

	return 1;
      }

  return 0;
}



