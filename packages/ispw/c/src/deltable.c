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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include <fts/fts.h>
#include "delbuf.h"
#include "deltable.h"


/***************************************************************************
 *
 *   Housekeeping for the signal delay classes:
 *
 *   The following hash table keep track of the delay line names.
 *
 *   Entries in the table are structure that are used also to track
 *   the order in the dsp execution (based on the temporary assumption
 *   that the _put order call is the same as the execution order !!!!),
 *   in order to compensate for the infamous 1 tick delays in out of
 *   order execution.
 */

/***************************************************************************
 *
 *   the delay buffer hashtable
 *
 */
 
static fts_hashtable_t delay_table;

typedef struct{
  del_buf_t *delbuf; /* the delayline buffer */
  fts_object_t *delwrite; /* the single dewrite~ object */
  int delwrite_put; /* flag  to say if delwrite put has been executed*/
  fts_object_t *first_delreader; /* list of delreaders */
  int n_delreader; /* total # of delreaders */
  int n_delreader_put; /* # of delreaders already put */
} delay_entry_t;

static delay_entry_t *
delay_table_get_entry(fts_symbol_t delay_name)
{
  fts_atom_t data, k;
  
  fts_set_symbol( &k, delay_name);

  if (fts_hashtable_get(&delay_table, &k, &data))
    return (delay_entry_t *) fts_get_pointer(&data);
  else
    {
      delay_entry_t *p;

      /* entry not found, create and initialize one; delay entry are never freed */

      p = (delay_entry_t *) fts_malloc(sizeof(delay_entry_t));
      
      p->delbuf = 0;                /* put to zero to check delwrite existence !!! */
      p->delwrite_put = 0;
      p->n_delreader = 0;
      p->n_delreader_put = 0;
      p->delwrite = 0;
      p->first_delreader = 0;

      fts_set_pointer(&data, p);
      fts_hashtable_put(&delay_table, &k, &data);
      
      return p;
    }
}

void 
delay_table_init(void)
{
  fts_hashtable_init(&delay_table, FTS_HASHTABLE_MEDIUM);
}

del_buf_t *
delay_table_get_delbuf(fts_symbol_t delay_name)
{
  delay_entry_t *p;

  p = delay_table_get_entry(delay_name);

  return p->delbuf;
}

/***************************************************************************
 *
 *    properties
 *
 */


static fts_atom_t *
delay_table_delreader_get_or_prop(fts_object_t *o, fts_symbol_t property)
{
  delay_table_delreader_t *this = (delay_table_delreader_t *)o;
  static fts_atom_t a;

  fts_object_get_prop(o, property, &a);

  if (! fts_is_void(&a))
    return &a;
  else if (this->next)
    return delay_table_delreader_get_or_prop(this->next, property);
  else
    return 0;
}

fts_atom_t *
delay_table_get_or_delreader_prop(fts_symbol_t delay_name, fts_symbol_t property)
{
  delay_entry_t *p;
  fts_atom_t    *a;

  p = delay_table_get_entry(delay_name);

  if (p->first_delreader)
    {
      a = delay_table_delreader_get_or_prop(p->first_delreader, property);

      if (a)
	return a;
    }

  return 0;
}

/***************************************************************************
 *
 *    scheduling ("_put") delay objects 
 *
 */

/* clear delay object put entries if everything scheduled */
static void 
delay_table_everything_scheduled(delay_entry_t *p)
{
  delbuf_clear_is_init_flag(p->delbuf);
  p->n_delreader_put = 0;
  p->delwrite_put = 0;
}

/* delwrite~ */

void
delay_table_delwrite_scheduled(fts_symbol_t delay_name)
{
  delay_entry_t *p;

  p = delay_table_get_entry(delay_name);

  if (p->n_delreader == p->n_delreader_put)
    delay_table_everything_scheduled(p);
  else
    p->delwrite_put = 1;
}

int
delay_table_is_delwrite_scheduled(fts_symbol_t delay_name)
{
  delay_entry_t *p;

  p = delay_table_get_entry(delay_name);

  return p->delwrite_put;
}

/* delreader */

void
delay_table_delreader_scheduled(fts_symbol_t delay_name)
{
  delay_entry_t *p;

  p = delay_table_get_entry(delay_name);

  (p->n_delreader_put)++;

  if ((p->n_delreader == p->n_delreader_put) && p->delwrite_put)
    delay_table_everything_scheduled(p);
}

int
delay_table_is_delreader_scheduled(fts_symbol_t delay_name)
{
  delay_entry_t *p;

  p = delay_table_get_entry(delay_name);

  return (p->n_delreader_put != 0);
}

/***************************************************************************
 *
 *    hashtable for delay objects
 *
 */

/* delwrite~ */

void
delay_table_add_delwrite(fts_object_t *o, fts_symbol_t delay_name, del_buf_t *buf)
{
  delay_entry_t *p;

  p = delay_table_get_entry(delay_name);

  p->delbuf = buf;
  p->delwrite = o; /* ignored in this implementation */
}

void
delay_table_remove_delwrite(fts_object_t *o, fts_symbol_t delay_name)
{
  delay_entry_t *p;

  p = delay_table_get_entry(delay_name);

  p->delbuf = 0;
  p->delwrite = 0;
}

/* delreader */

static void
delay_table_delreader_add_next(fts_object_t *o, fts_object_t *next)
{
  delay_table_delreader_t *obj = (delay_table_delreader_t *)o;

  while (obj->next)
    obj = (delay_table_delreader_t *) obj->next;

  obj->next = next;
}

static fts_object_t *
delay_table_delreader_remove_next(fts_object_t *o, fts_object_t *next)
{
  delay_table_delreader_t *this = (delay_table_delreader_t *)o;

  if (o == next)
    return this->next;
  else
    {
      this->next = delay_table_delreader_remove_next(this->next, next);
      return o;
    }
}

void
delay_table_add_delreader(fts_object_t *o, fts_symbol_t delay_name)
{
  delay_entry_t *p;

  p = delay_table_get_entry(delay_name);

  (p->n_delreader)++;
  
  if (p->first_delreader)
    delay_table_delreader_add_next(p->first_delreader, o);
  else
    p->first_delreader =  o;
}

void
delay_table_remove_delreader(fts_object_t *o, fts_symbol_t delay_name)
{
  delay_entry_t *p;

  p = delay_table_get_entry(delay_name);

  (p->n_delreader)--;

  if (p->first_delreader)
    p->first_delreader = delay_table_delreader_remove_next(p->first_delreader, o);
}

