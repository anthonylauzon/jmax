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


/* play from a settable instance of explode, set by name.  Messages:
   
   int: nonzero for start, zero for stop.
   bang: output next event.
   set <symbol>: sets the name of the explode sequenced.
   int in right inlet: sets to "explode%d"
   "startat", "nth": same as for explode.

   */

#include <fts/fts.h>
#include "explode.h"

typedef struct
{
  fts_object_t obj;

  evt_t    *current;
  long      serial;
  fts_symbol_t explode_name;

} explay_t;


static void
gen_explode_name(char *buf, const char *base, long int n)
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
explay_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explay_t *this = (explay_t *)o;
  explode_t *explode;

  explode = explode_get_by_name(this->explode_name);

  if (! explode)
    {
      post("explay: %s: can't find explode\n", this->explode_name);
      this->current = 0;
    }
  else if (! this->current)
    post("explay: not playing\n");
  else if (this->serial != explode->serial)
    {
      post("explay: %s: has been modified while playing\n", this->explode_name);
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
explay_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explay_t *this = (explay_t *)o;

  if(fts_is_number(at))
    {
      long n = (long) fts_get_number_int(at);
      
      if (n)
	{
	  explode_t *explode;
	  
	  explode = explode_get_by_name(this->explode_name);
	  
	  if (! explode)
	    {
	      post("explay: %s: can't find explode\n", this->explode_name);
	      this->current = 0;
	    }
	  else
	    {
	      this->serial = explode->serial;
	      
	      if (! this->serial)
		{
		  post("explay: %s: recording\n", this->explode_name);
		  this->current = 0;
		}
	      else
		{
		  this->current = explode->data.evt;
		  
		  if (this->current)
		    fts_outlet_int(o, 0, this->current->time);
		  else
		    post("explay: %s: empty\n", this->explode_name);
		}
	    }
	}
      else
	this->current = 0;
    }
}

static void
explay_startat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explay_t *this = (explay_t *)o;

  if(ac > 2 && fts_is_int(at + 0) && fts_is_int(at + 1) && fts_is_int(at + 2))
    {
      long n1 = fts_get_int(at + 0);
      long n2 = fts_get_int(at + 1);
      long n3 = fts_get_int(at + 2);
      explode_t *explode;
      
      explode = explode_get_by_name(this->explode_name); 
      
      if (! explode)
	{
	  post("explay: %s: can't find explode\n", this->explode_name);
	  this->current = 0;
	}
      else
	{
	  this->serial = explode->serial;
	  
	  if (! this->serial)
	    {
	      post("explay: %s: recording\n", this->explode_name);
	      this->current = 0;
	    }
	  else
	    {
	      evt_t *e;
	      
	      for (e = explode->data.evt; e; e = e->next)
		if (e->pit == n1 && e->vel == n2 && e->chan == n3)
		  break;
	      
	      if (! e)
		{
		  post("explay: startat: note not found\n");
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
}



static void
explay_nth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explay_t *this = (explay_t *)o;
  long n = fts_get_int_arg(ac, at, 0, 0);
  explode_t *explode;

  explode = explode_get_by_name(this->explode_name); 

  if (! explode)
    {
      post("explay: %s: no such explode\n", this->explode_name);
      this->current = 0;
    }
  else
    {
      evt_t *e;

      for (e = explode->data.evt; e && n--; e = e->next)
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
explay_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explay_t *this = (explay_t *)o;
  
  if(fts_is_symbol(at))
    {
      this->explode_name = fts_get_symbol(at);
      this->current = 0;
    }
}


static void
explay_number_1(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explay_t *this = (explay_t *)o;
  char buf[30];

  gen_explode_name(buf, "explode", fts_get_number_int(at));
  this->explode_name = fts_new_symbol(buf);
  this->current = 0;
}


static void
explay_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explay_t *this = (explay_t *)o;

  if(ac > 0 && fts_is_symbol(at))
    {
      this->explode_name = fts_get_symbol(at);
      this->current = 0;
    }
  else
    fts_object_error(o, "argument of explode name required");
}


static void
explay_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(explay_t), explay_init, NULL);

  fts_class_message_varargs(cl, fts_s_set, explay_set);
  fts_class_message_varargs(cl, fts_new_symbol("nth"), explay_nth);
  fts_class_message_varargs(cl, fts_new_symbol("startat"), explay_startat);

  fts_class_inlet_bang(cl, 0, explay_bang);
  fts_class_inlet_number(cl, 0, explay_number);
  fts_class_inlet_number(cl, 1, explay_number_1);

  fts_class_outlet_int(cl, 0);
  fts_class_outlet_int(cl, 1);
  fts_class_outlet_int(cl, 2);
  fts_class_outlet_int(cl, 3);
  fts_class_outlet_int(cl, 4);
}

void
explay_config(void)
{
  fts_class_install(fts_new_symbol("explay"),explay_instantiate);
}
