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


/* modified by DDZ to format polyphonic aftertouch a0 messages */
/* ported by mdc to the new FTS */

#include "fts.h"


typedef struct midiformat
{
  fts_object_t obj;

  long poly;
  long noteon;	
  long ctl;
  long prog;	
  long touch;	
  long bend;	
  long channel;
} midiformat_t;


/* note on/off  :  midiformat_note, method on inlet 0, message list */


static void
midiformat_note(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiformat_t *this = (midiformat_t *)o;
  long val;

  if (ac < 2)
    return;

  fts_outlet_int((fts_object_t *)this, 0, this->noteon);

  if (fts_is_long(at))
    val = fts_get_long(at);
  else if (fts_is_float(at))
    val = (long)fts_get_float(at);
  else
    val = 0;

  fts_outlet_int((fts_object_t *)this, 0, val);

  at++;

  if (fts_is_long(at))
    val = fts_get_long(at);
  else if (fts_is_float(at))
    val = (long)fts_get_float(at);
  else
    val = 0;

  fts_outlet_int((fts_object_t *)this, 0, val);
}

/* poly touch: method midiformat_poly_touch, inlet 1, list input */

static void
midiformat_poly_touch(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiformat_t *this = (midiformat_t *)o;
  long val;

  if (ac < 2)
    return;

  fts_outlet_int((fts_object_t *)this, 0, this->poly);

  if (fts_is_long(at))
    val = fts_get_long(at);
  else if (fts_is_float(at))
    val = (long)fts_get_float(at);
  else
    val = 0;

  fts_outlet_int((fts_object_t *)this, 0, val);

  at++;

  if (fts_is_long(at))
    val = fts_get_long(at);
  else if (fts_is_float(at))
    val = (long)fts_get_float(at);
  else
    val = 0;

  fts_outlet_int((fts_object_t *)this, 0, val);
}

/* ctl change: midiformat_ctlchange, method for inlet 2, message list */

static void
midiformat_ctl_change(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiformat_t *this = (midiformat_t *)o;
  long val;

  if (ac < 2)
    return;

  fts_outlet_int((fts_object_t *)this, 0, this->ctl);

  if (fts_is_long(at))
    val = fts_get_long(at);
  else if (fts_is_float(at))
    val = (long)fts_get_float(at);
  else
    val = 0;

  fts_outlet_int((fts_object_t *)this, 0, val);

  at++;

  if (fts_is_long(at))
    val = fts_get_long(at);
  else if (fts_is_float(at))
    val = (long)fts_get_float(at);
  else
    val = 0;

  fts_outlet_int((fts_object_t *)this, 0, val);
}


/* prog change: method midiformat_progchange, inlet 3, long/float message */

static void
midiformat_prog_change(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiformat_t *this = (midiformat_t *)o;

  fts_outlet_int((fts_object_t *)this, 0, this->prog);
  fts_outlet_int((fts_object_t *)this, 0, fts_get_int_arg(ac, at, 0, 0));
}


/* after touch: method midiformat_after_touch, inlet 4, long message */

static void
midiformat_after_touch(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiformat_t *this = (midiformat_t *)o;

  fts_outlet_int((fts_object_t *)this, 0, this->touch);
  fts_outlet_int((fts_object_t *)this, 0, fts_get_int_arg(ac, at, 0, 0));
}

/* bend:  method midiformat_bend, inlet 5, long message */

static void
midiformat_bend(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiformat_t *this = (midiformat_t *)o;
  long int v1 = fts_get_int_arg(ac, at, 0, 0);

  fts_outlet_int((fts_object_t *)this, 0, this->bend);
  fts_outlet_int((fts_object_t *)this, 0, (v1 > 65) ? (long)(2 * (v1 - 64)) : 0L);
  fts_outlet_int((fts_object_t *)this, 0, v1);
}

/* set channel: method midiformat_set_channel, inlet 6, long message */

static void
midiformat_set_channel(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiformat_t *this = (midiformat_t *)o;
  long int v = fts_get_int_arg(ac, at, 0, 0);

  v = ((v-1) & 0xFL);
  this->noteon = (this->noteon & 0xF0L) + v;
  this->ctl    = (this->ctl    & 0xF0L) + v;
  this->prog   = (this->prog   & 0xF0L) + v;
  this->touch  = (this->touch  & 0xF0L) + v;
  this->bend   = (this->bend   & 0xF0L) + v;
  this->poly   = (this->poly   & 0xF0L) + v;
}

/* Init methods: accept an optional long giving the channel */

static void
midiformat_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiformat_t *this = (midiformat_t *)o;
  long int channel = fts_get_int_arg(ac, at, 1, 0);
  
  channel = (channel == 0) ? 1 : channel;

  channel = ((channel - 1) & 0xFL);

  this->noteon = 0x90L + channel;
  this->ctl    = 0xB0L + channel;	
  this->prog   = 0xC0L + channel;	
  this->touch  = 0xD0L + channel;	
  this->bend   = 0xE0L + channel;
  this->poly   = 0xA0L + channel;
}



static fts_status_t
midiformat_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  /* initialize the class */

  fts_class_init(cl, sizeof(midiformat_t), 7, 1, 0); 

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_int;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, midiformat_init, 2, a, 1);

  /* Midiformat args */

  fts_method_define_varargs(cl, 0, fts_s_list, midiformat_note);

  fts_method_define_varargs(cl, 1, fts_s_list,  midiformat_poly_touch);

  fts_method_define_varargs(cl, 2, fts_s_list,  midiformat_ctl_change);

  a[0] = fts_s_int;
  fts_method_define(cl, 3, fts_s_int,   midiformat_prog_change, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 3, fts_s_float, midiformat_prog_change, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 4, fts_s_int,   midiformat_after_touch, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 4, fts_s_float, midiformat_after_touch, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 5, fts_s_int,   midiformat_bend, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 5, fts_s_float, midiformat_bend, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 6, fts_s_int,   midiformat_set_channel, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 6, fts_s_float, midiformat_set_channel, 1, a);

  /* Type the outlet */

  a[0] = fts_s_int;
  fts_outlet_type_define(cl, 0,	fts_s_int, 1, a);

  return fts_Success;
}

void
midiformat_config(void)
{
  fts_class_install(fts_new_symbol("midiformat"),midiformat_instantiate);
}




