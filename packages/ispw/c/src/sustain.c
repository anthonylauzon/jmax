/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

/* The sustain object */


#include <fts/fts.h>

struct slink
{
  long pitch;
  struct slink *next;
};


typedef struct sustain
{
  fts_object_t ob;
  long vel;
  long sust;
  struct slink *sustained_notes;
} sustain_t;


static void
sustain_number(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  sustain_t *this = (sustain_t *)o;
  long n = fts_get_int_arg(ac, at, 0, 0);

  if (this->vel || !this->sust)
    {
      fts_outlet_int(o, 1, this->vel);
      fts_outlet_int(o, 0, n);
    }
  else
    {
      struct slink *p;

      p = (struct slink *) fts_malloc(sizeof(struct slink));
      p->pitch = n;
      p->next  = this->sustained_notes;
      this->sustained_notes = p;
    }
}

static void
sustain_number_1(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  sustain_t *this = (sustain_t *)o;

  this->vel = fts_get_int_arg(ac, at, 0, 0);
}

static void
sustain_number_2(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  sustain_t *this = (sustain_t *)o;

  this->sust = fts_get_int_arg(ac, at, 0, 0);

  if ((this->sust == 0) && this->sustained_notes)
    {
      struct slink *s, *s2;

      for (s = this->sustained_notes; s; s = s2)
	{
	  s2 = s->next;
	  fts_outlet_int(o, 1, 0L);
	  fts_outlet_int(o, 0, s->pitch);
	  fts_free(s);
	}

      this->sustained_notes = 0;
    }
}

static void
sustain_list(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  sustain_t *this = (sustain_t *)o;

  if ((ac >= 3) && fts_is_number(&at[2]))
    sustain_number_2(o, s, 1, at+2, fts_nix);

  if ((ac >= 2) && fts_is_number(&at[1]))
    this->vel = (long) fts_get_int_arg(ac, at, 1, 0);

  if ((ac >= 1) && fts_is_number(&at[0]))
    sustain_number(o, s, 1, at, fts_nix);
}


/* The clear method is also installed for the $delete message */

static void
sustain_clear(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  sustain_t *this = (sustain_t *)o;
  struct slink *s1, *s2;

  for (s1 = this->sustained_notes; s1; s1 = s2)
    {
      s2 = s1->next;
      fts_free(s1);
    }

  this->sustained_notes = 0;
}


static void
sustain_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  sustain_t *this = (sustain_t *)o;

  this->sustained_notes = 0;
  this->vel = fts_get_int_arg(ac, at, 0, 0);
  this->sust = fts_get_int_arg(ac, at, 1, 0);
}


static void
sustain_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(sustain_t), sustain_init, sustain_clear);

  fts_class_message_varargs(cl, fts_s_clear, sustain_clear);

  fts_class_inlet_number(cl, 0, sustain_number);
  fts_class_inlet_varargs(cl, 0, sustain_list);
  fts_class_inlet_number(cl, 1, sustain_number_1);
  fts_class_inlet_number(cl, 2, sustain_number_2);

  fts_class_outlet_int(cl, 0);
  fts_class_outlet_int(cl, 1);
}


void
sustain_config(void)
{
  fts_class_install(fts_new_symbol("sustain"),sustain_instantiate);
}
