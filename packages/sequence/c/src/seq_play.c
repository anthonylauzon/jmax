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


/* play from a settable instance of sequence, set by name.  Messages:
   
   int: nonzero for start, zero for stop.
   bang: output next event.
   set <symbol>: sets the name of the sequence sequenced.
   int in right inlet: sets to "sequence%d"
   "startat", "nth": same as for sequence.

   */

#include "fts.h"
#include "sequence.h"

typedef struct
{
  fts_object_t obj;

  evt_t    *current;
  long      serial;
  fts_symbol_t sequence_name;

} seq_play_t;


static void
gen_sequence_name(char *buf, const char *base, long int n)
{
  char b2[30];
  char *s = b2+29;
  *s = 0;

  if (n < 0)
    n = 0;

  while ((*buf = *base++))
    buf++;

  if (!n)
    *(--s) = '0';
  else while (n)
    {
      *(--s) = '0' + (n%10);
      n /= 10;
    }
  while ((*buf++ = *s++))
    ;
}

static void
seq_play_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seq_play_t *this = (seq_play_t *)o;
  sequence_t *sequence;

  sequence = sequence_get_by_name(this->sequence_name);

  if (! sequence)
    {
      post("seq_play: %s: can not find sequence\n", fts_symbol_name(this->sequence_name));
      this->current = 0;
    }
  else if (! this->current)
    post("seq_play: not playing\n");
  else if (this->serial != sequence->serial)
    {
      post("seq_play: %s: has been modified while playing\n", fts_symbol_name(this->sequence_name));
      this->current = 0;
    }
  else
    {
      evt_t *e = this->current;

      fts_outlet_int(o, 4, e->chan);
      fts_outlet_int(o, 3, e->dur);
      fts_outlet_int(o, 2, e->vel);
      fts_outlet_int(o, 1, e->pit);

      this->current = e->next;

      if (this->current)
	fts_outlet_int(o, 0, this->current->time - e->time);
    }
}


static void
seq_play_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seq_play_t *this = (seq_play_t *)o;
  long n = (long) fts_get_number_int(at);

  if (n)
    {
      sequence_t *sequence;

      sequence = sequence_get_by_name(this->sequence_name);

      if (! sequence)
	{
	  post("seq_play: %s: can not find sequence\n", fts_symbol_name(this->sequence_name));
	  this->current = 0;
	}
      else
	{
	  this->serial = sequence->serial;

	  if (! this->serial)
	    {
	      post("seq_play: %s: recording\n", fts_symbol_name(this->sequence_name));
	      this->current = 0;
	    }
	  else
	    {
	      this->current = sequence->data.evt;

	      if (this->current)
		fts_outlet_int(o, 0, this->current->time);
	      else
		post("seq_play: %s: empty\n", fts_symbol_name(this->sequence_name));
	    }
	}
    }
  else
    this->current = 0;
}

static void
seq_play_startat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seq_play_t *this = (seq_play_t *)o;
  long n1 = fts_get_long(at + 0);
  long n2 = fts_get_long(at + 1);
  long n3 = fts_get_long(at + 2);
  sequence_t *sequence;

  sequence = sequence_get_by_name(this->sequence_name); 

  if (! sequence)
    {
      post("seq_play: %s: can not find sequence\n", fts_symbol_name(this->sequence_name));
      this->current = 0;
    }
  else
    {
      this->serial = sequence->serial;

      if (! this->serial)
	{
	  post("seq_play: %s: recording\n", fts_symbol_name(this->sequence_name));
	  this->current = 0;
	}
      else
	{
	  evt_t *e;

	  for (e = sequence->data.evt; e; e = e->next)
	    if (e->pit == n1 && e->vel == n2 && e->chan == n3)
	      break;
	  
	  if (! e)
	    {
	      post("seq_play: startat: note not found\n");
	      this->current = 0;
	    }
	  else if (e->next)
	    {
	      fts_outlet_int(o, 0, e->next->time - e->time);
	      this->current = e->next;
	    }
	}
    }
}



static void
seq_play_nth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seq_play_t *this = (seq_play_t *)o;
  long n = fts_get_long_arg(ac, at, 0, 0);
  sequence_t *sequence;

  sequence = sequence_get_by_name(this->sequence_name); 

  if (! sequence)
    {
      post("seq_play: %s: no such sequence\n", fts_symbol_name(this->sequence_name));
      this->current = 0;
    }
  else
    {
      evt_t *e;

      for (e = sequence->data.evt; e && n--; e = e->next)
	;

      if (e)
	{
	  fts_outlet_int(o, 4, e->chan);
	  fts_outlet_int(o, 3, e->dur);
	  fts_outlet_int(o, 2, e->vel);
	  fts_outlet_int(o, 1, e->pit);
	  fts_outlet_int(o, 0, e->time);
	}
    }
}


static void
seq_play_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seq_play_t *this = (seq_play_t *)o;
  
  this->sequence_name = fts_get_symbol(at);
  this->current = 0;
}


static void
seq_play_number_1(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seq_play_t *this = (seq_play_t *)o;
  char buf[30];

  gen_sequence_name(buf, "sequence", fts_get_number_int(at));
  this->sequence_name = fts_new_symbol_copy(buf);
  this->current = 0;
}


static void
seq_play_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seq_play_t *this = (seq_play_t *)o;
  fts_symbol_t sym = fts_get_symbol_arg(ac, at, 0, 0);

  post("seq_play: %s: %s\n", (sym ? fts_symbol_name(s) : "seq_play"), fts_symbol_name(this->sequence_name));
}



static void
seq_play_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seq_play_t *this = (seq_play_t *)o;

  this->sequence_name = fts_get_symbol_arg(ac, at, 1, 0);
  this->current = 0;
}


static fts_status_t
seq_play_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[5];

  /* initialize the class */

  fts_class_init(cl, sizeof(seq_play_t), 2, 5, 0); 

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, seq_play_init, 2, a, 1);

  /* Seq_Play number methods */

  fts_method_define(cl, 0, fts_s_bang, seq_play_bang, 0, 0);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, seq_play_number, 1, a);
  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, seq_play_number, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 1, fts_s_int, seq_play_number_1, 1, a);
  a[0] = fts_s_float;
  fts_method_define(cl, 1, fts_s_float, seq_play_number_1, 1, a);

  /* Other methods */

  a[0] = fts_s_int;
  fts_method_define_optargs(cl, 0, fts_new_symbol("nth"), seq_play_nth, 1, a, 0);

  a[0] = fts_s_symbol;
  fts_method_define(cl, 0, fts_new_symbol("set"), seq_play_set, 1, a);

  a[0] = fts_s_symbol;
  fts_method_define_optargs(cl, 0, fts_new_symbol("print"), seq_play_print, 1, a, 0);

  a[0] = fts_s_int;
  a[1] = fts_s_int;
  a[2] = fts_s_int;
  fts_method_define(cl, 0, fts_new_symbol("startat"), seq_play_startat, 3, a);

  /* Type the outlet */

  a[0] = fts_s_int;
  fts_outlet_type_define(cl, 0,	fts_s_int, 1, a);
  fts_outlet_type_define(cl, 1,	fts_s_int, 1, a);
  fts_outlet_type_define(cl, 2,	fts_s_int, 1, a);
  fts_outlet_type_define(cl, 3,	fts_s_int, 1, a);
  fts_outlet_type_define(cl, 4,	fts_s_int, 1, a);

  return fts_Success;
}

void
seq_play_config(void)
{
  fts_class_install(fts_new_symbol("seq_play"),seq_play_instantiate);
}





