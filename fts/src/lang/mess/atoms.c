/*
  Generic, may be debug, function to deal with atoms
 */

#include <stdio.h>

#include "sys.h"
#include "lang/mess.h"

void
fprintf_atoms(FILE *f, int ac, const fts_atom_t *at)
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
	fprintf(f,"(symbol)%s%s", fts_symbol_name(fts_get_symbol(&at[i])), ps);
      else if (fts_is_long(&at[i]))
	fprintf(f,"(int)%d%s", fts_get_long(&at[i]), ps);
      else if (fts_is_float(&at[i]))
	fprintf(f,"(float)%f%s", fts_get_float(&at[i]), ps);
      else if (fts_is_ptr(&at[i]) )
	fprintf(f,"(ptr)%lx%s", (unsigned long) fts_get_ptr( &at[i]), ps);
      else if (fts_is_object(&at[i]))
	{
	  fts_object_t *obj;

	  obj = fts_get_object(&at[i]);

	  if (obj)
	    fprintf(f,"(obj)%d%s", fts_object_get_id(obj), ps);
	  else
	    fprintf(f,"(obj)<null>");
	}
      else if (fts_is_void(&at[i]))
	fprintf(f,"(void)%s", ps);
      else
	fprintf(f,"(type %d)???%s", fts_get_type(&at[i]), ps);
    }
}

