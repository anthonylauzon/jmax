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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell, Miller Puckette, Zack Settel.
 *
 */
#include <math.h>
#include <fts/fts.h>
#include "Rsend.h"

typedef struct 
{
  fts_object_t _ob;
  int   sxindex;
  char  receiveBuff[SYSEXMAX+1];
  int   atomcount;
} Rreceive_t;


/* ******************** */

#define IGNORE -1

static void
Rreceive_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  Rreceive_t *this = (Rreceive_t *)o;

  this->sxindex = IGNORE;
}


static void
Rreceive_set_atom(fts_object_t *o, int winlet, fts_symbol_t sym, int argc, const fts_atom_t *at)
{
  Rreceive_t *this = (Rreceive_t *)o;
  long n = fts_get_int(at);
  fts_atom_t av[MAXLEN];
  int i,ac = 0;
  long l;
  fts_label_t *label;
  fts_symbol_t sel;

  if (n == 0xF0)
    {
      this->sxindex = 0;
      this->atomcount = 0;
      return;
    }
  else if (this->sxindex == 0 && n != 0x7F)	
    {		
      this->sxindex = IGNORE;
      return;
    }
  else if (this->sxindex == IGNORE) 
    return;
  else if (n != 0xF7)		/* get the stuff */
    {
      if (this->sxindex == SYSEXMAX)
	{
	  post("Rreceive: SYSEX message too long, aborting\n");
	  this->sxindex = IGNORE;
	  return;
	}
      this->receiveBuff[this->sxindex] = (char) n;
    }
  else		/* data ready */
    {
      this->receiveBuff[this->sxindex] = '\0'; /* write an end of string at end */

      for (i=0; i < this->sxindex; i++)
	{
	  if (this->receiveBuff[i] == '\0')
	    {
	      long l;
	      float f;

	      if (ac == MAXLEN)
		{
		  post("Rreceive: too many atoms in message, aborting\n");
		  return;
		}

	      switch (this->receiveBuff[i+1])
		{
		case RSEND_LONG:
		  sscanf(this->receiveBuff+i+2,"%ld",&l);
		  fts_set_int(&av[ac], l);
		  break;
		case RSEND_FLOAT:
		  sscanf(this->receiveBuff+i+2,"%f",&f);
		  fts_set_float(&av[ac], f);
		  break;
		case RSEND_SYM:
		  fts_set_symbol(&av[ac], fts_new_symbol_copy(this->receiveBuff+i+2)); 
		  break;
		default:
		  post("Rreceive: unrecognized atom type in message, aborting\n");
		  return;
		}

	      ac++;
	    }
	}

      sel = fts_new_symbol_copy(this->receiveBuff + 1);
      label = fts_label_get(fts_object_get_patcher(o), sel);
      
      if(fts_label_is_connected(label))
	{
	  if (fts_is_int(av))
	    {
	      if (ac >1)
		fts_label_send(label, fts_s_list, ac, av);
	      else
		fts_label_send(label, fts_s_int, ac, av);
	    }
	  else if (fts_is_float(av))
	    {
	      if (ac >1)
		fts_label_send(label, fts_s_list, ac, av);
	      else
		fts_label_send(label, fts_s_float, ac, av);
	    }
	  else if (fts_is_symbol(av))
	    fts_label_send(label, fts_get_symbol(av), ac - 1, av + 1);
	}
      else
	fts_outlet_send(o, 0, sel, ac, av);

      this->sxindex = IGNORE;

      return;
    }

  this->sxindex++;
}


static void
Rreceive_list(fts_object_t *o, int winlet, fts_symbol_t sym, int ac, const fts_atom_t *at)
{
  Rreceive_t *this = (Rreceive_t *)o;
  fts_atom_t a[1];
  int i;

  fts_set_int(a, 0xF0);
  Rreceive_set_atom(o, 0, 0, 1, a);

  for(i=0; i<ac; i++)
    Rreceive_set_atom(o, 0, 0, 1, at + i);

  fts_set_int(a, 0xF7);
  Rreceive_set_atom(o, 0, 0, 1, a);
}

/* No init , no delete */

/* One inlet, one outlet  */

static fts_status_t
Rreceive_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[1];

  fts_class_init(cl, sizeof(Rreceive_t), 1, 1, 0);

  fts_method_define(cl, 0, fts_s_bang, Rreceive_bang, 0, 0);

  fts_method_define_varargs(cl, 0, fts_s_list, Rreceive_list);

  return fts_Success;
}


void
Rreceive_config(void)
{
  fts_class_install(fts_new_symbol("Rreceive"), Rreceive_instantiate);
}







