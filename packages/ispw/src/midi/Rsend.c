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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell, Zack Settel.
 *
 */

#include <string.h>
#include <math.h>
#include <stdio.h>
#include <fts/fts.h>
#include "Rsend.h"



typedef struct 
{
  fts_object_t _ob;

} Rsend_t;

/* the format look like not correct; , ReceiveName collide with the type of non commercial
   sysex */

/* output a non-commercial SYSEX message using the format: 
0xF0 0x7F ReceiveName<ascii> NUL TYPE<A_LONG etc> ATOM<ascii> NUL TYPE ATOM TYPE ATOM.... 0xF7 */


static void
Rsend_anything(fts_object_t *o, int winlet, fts_symbol_t s, int argc, const fts_atom_t *av)
{
  Rsend_t *this = (Rsend_t *)o;
  int i;
  int len;
  fts_atom_t a[SYSEXMAX + 1];
  char outstr[SYSEXMAX+1];
  char data[STRLENMAX];
  int bytecount = 0;
	
  if (argc > MAXLEN)
    {
      post("Rsend: list too long\n");
      return;
    }
	
  strcpy(outstr, fts_symbol_name(s)); /* get name */
  bytecount += strlen(fts_symbol_name(s)) + 1; /* increment past inserted '\0' */

  for(i=0; i<argc; i++)
    {
      if (fts_is_long(&av[i]))
	{
	  data[0] = (char) RSEND_LONG;
	  sprintf(data + 1,"%d", fts_get_int(&av[i]));
	}
      else if (fts_is_float(&av[i]))
	{
	  data[0] = (char) RSEND_FLOAT;
	  sprintf(data + 1,"%#f", fts_get_float(&av[i]));
	}
      else if (fts_is_symbol(&av[i]))
	{
	  data[0] = (char) RSEND_SYM;
	  sprintf(data + 1,"%s", fts_symbol_name(fts_get_symbol(&av[i])));
	}
      else
	{
	  post("Rsend: unrecognized token\n");
	  return;
	}

      len = strlen(data) + 1;
      if (bytecount + len >= SYSEXMAX)
	{
	  post("Rsend: message longer than %d charactors\n", SYSEXMAX);
	  return;
	}

      strcpy(outstr + bytecount, data);
      bytecount += len;
    }

  for(i=0; i<bytecount-2; i++)
    fts_set_int(a + i, (int)outstr[i]);
  
  fts_outlet_send(o, 0, fts_s_list, bytecount - 1, a);
}



/* No init , no delete */

/* One inlet, one outlet  */

static fts_status_t
Rsend_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[1];

  fts_class_init(cl, sizeof(Rsend_t), 1, 1, 0);

  fts_method_define_varargs(cl, 0, fts_s_anything, Rsend_anything);

  return fts_Success;
}


void
Rsend_config(void)
{
  fts_class_install(fts_new_symbol("Rsend"), Rsend_instantiate);
}