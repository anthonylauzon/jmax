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
/* Rsend - remote send/receive ---   zack settel 11/93  */
/* note: this is an experimental object and may change  */

#include <math.h>
#include <stdio.h>		/* for sprintf */
#include "fts.h"
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
  char outstr[SYSEXMAX+1];
  char data[STRLENMAX];
  int bytecount = 0;
	
  if (argc > MAXLEN)
    {
      post("Rsend: list too long\n");
      return;
    }
	
  strcpy(outstr, fts_symbol_name(s));   /* get name */
  bytecount += strlen(fts_symbol_name(s))+1; /* increment past inserted '\0' */

  for (i = 0; i < argc; i++)
    {
      if (fts_is_long(&av[i]))
	{
	  data[0] = (char) RSEND_LONG;
	  sprintf(data+1,"%d", fts_get_int(&av[i]));
	}
      else if (fts_is_float(&av[i]))
	{
	  data[0] = (char) RSEND_FLOAT;
	  sprintf(data+1,"%#f", fts_get_float(&av[i]));
	}
      else if (fts_is_symbol(&av[i]))
	{
	  data[0] = (char) RSEND_SYM;
	  sprintf(data+1,"%s", fts_symbol_name(fts_get_symbol(&av[i])));
	}
      else
	{
	  post("Rsend: unrecognized token\n",STRLENMAX);
	  return;
	}

      len = strlen(data)+1;
      if (bytecount+len >= SYSEXMAX)
	{
	  post("Rsend: message longer than %d charactors\n",SYSEXMAX);
	  return;
	}
      strcpy(outstr+bytecount, data);
      bytecount += len;
    }

  fts_outlet_int(o, 0, 0xF0L);		/* header */
  fts_outlet_int(o, 0, 0x7FL);

  for (i = 0; i < (bytecount - 1); i++)
    fts_outlet_int(o, 0, outstr[i]);
  
  fts_outlet_int(o, 0, 0xF7L);
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
