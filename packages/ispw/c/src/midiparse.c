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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell, Miller Puckette.
 *
 */



/* modified by DDZ to parse/format polyphonic aftertouch a0 messages */
/* Ported to the new fts by mdc */

#include <fts/fts.h>

#define NOTEOFF 0x80
#define NOTEON 0x90
#define POLY 0xA0  
#define CTL 0xB0
#define PGM 0xC0
#define TOUCH 0xD0
#define BEND 0xE0


typedef struct 
{
  fts_object_t obj;	

  long status;			
  long byte1;
} midiparse_t;


/*  A Bang  reset the  parser; installed also as init.   */

static void
midiparse_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiparse_t *this = (midiparse_t *)o;

  this->status = 0;
  this->byte1 = 0;
}

/* installed for floats and ints */

static void
midiparse_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiparse_t *this = (midiparse_t *)o;
  long int byte = fts_get_int_arg(ac, at, 0, 0);
  fts_atom_t av[2];

  if (byte > 127)
    {
      /*   status byte here   ?   */
      this->status = byte & 0xFFL;
      return;
    }
  
  switch (this->status & 0xF0L)
    {
    case CTL:
      if (! this->byte1)
	this->byte1 = byte & 0x7F;
      else
	{
	  fts_outlet_int((fts_object_t *)this, 6, 1 + (this->status & 0xF));
	  fts_set_long(av + 0, this->byte1);
	  fts_set_long(av + 1, byte & 0x7F);
	  fts_outlet_send((fts_object_t *)this, 2, fts_s_list, 2, av);
	  this->byte1 = 0;
	}
      return;

    case POLY:
      if (! this->byte1)
	this->byte1 = byte & 0x7F;
      else
	{
	  fts_outlet_int((fts_object_t *)this, 6, 1 + (this->status & 0xF));
	  fts_set_long(av + 0, this->byte1);
	  fts_set_long(av + 1, byte & 0x7F);
	  fts_outlet_send((fts_object_t *)this, 1, fts_s_list, 2, av);
	  this->byte1 = 0;
	}
      return;

    case BEND:
      if (! this->byte1)
	this->byte1 = 1;
      else
	{
	  fts_outlet_int((fts_object_t *)this, 6, 1 + (this->status & 0xF));
	  fts_outlet_int((fts_object_t *)this, 5, byte & 0x7F);
	  this->byte1 = 0;
	}
      return;

    case TOUCH:
      fts_outlet_int((fts_object_t *)this, 6, 1 + (this->status & 0xF));
      fts_outlet_int((fts_object_t *)this, 4, byte & 0x7F);
      return;

    case NOTEON:
      if (! this->byte1)
	this->byte1 = byte & 0x7F;
      else
	{
	  fts_outlet_int((fts_object_t *)this, 6, 1 + (this->status & 0xF));
	  fts_set_long(av + 0, this->byte1);
	  fts_set_long(av + 1, byte & 0x7F);
	  fts_outlet_send((fts_object_t *)this, 0, fts_s_list, 2, av);
	  this->byte1 = 0;
	}
      return;

    case PGM:
      fts_outlet_int((fts_object_t *)this, 6, 1 + (this->status & 0xF));
      fts_outlet_int((fts_object_t *)this, 3, byte & 0x7F);
      return;

    case NOTEOFF:
      if (! this->byte1)
	this->byte1 = byte & 0x7F;
      else
	{
	  fts_outlet_int((fts_object_t *)this, 6, 1 + (this->status & 0xF));
	  fts_set_long(av + 0, this->byte1);
	  fts_set_long(av + 1, 0L);
	  fts_outlet_send((fts_object_t *)this, 0, fts_s_list, 2, av);
	  this->byte1 = 0;
	}
      return;
    }
}

static fts_status_t
midiparse_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  /* initialize the class */

  fts_class_init(cl, sizeof(midiparse_t), 1, 7, 0); 

  /* define the system methods */

  a[0] = fts_s_symbol;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, midiparse_bang, 1, a);

  /* Midiparse args */

  fts_method_define(cl, 0, fts_s_bang, midiparse_bang, 0, 0);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, midiparse_number, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, midiparse_number, 1, a);

  /* Type the outlet */

  fts_outlet_type_define_varargs(cl, 0,	fts_s_list);
  fts_outlet_type_define_varargs(cl, 1,	fts_s_list);
  fts_outlet_type_define_varargs(cl, 2,	fts_s_list);

  a[0] = fts_s_int;
  fts_outlet_type_define(cl, 3,	fts_s_int, 1, a);
  fts_outlet_type_define(cl, 4,	fts_s_int, 1, a);
  fts_outlet_type_define(cl, 5,	fts_s_int, 1, a);
  fts_outlet_type_define(cl, 6,	fts_s_int, 1, a);

  return fts_Success;
}

void
midiparse_config(void)
{
  fts_class_install(fts_new_symbol("midiparse"),midiparse_instantiate);
}




