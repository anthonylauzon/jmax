/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
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
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#include <fts/fts.h>
#include <ftsprivate/client.h>

static fts_symbol_t s_update_group_begin;
static fts_symbol_t s_update_group_end;
static fts_heap_t *update_heap;

#define DEFAULT_PERIOD 20
#define DEFAULT_MAX_UPDATES 40

typedef struct update_entry {
  fts_object_t *obj;
  struct update_entry *next;
} update_entry_t;

typedef struct _update_group_t {
  fts_object_t head;
  int period;
  int max_updates;
  update_entry_t *update_fifo;
  int object_count;
} update_group_t;

static update_group_t **update_group_table = 0;
static int update_group_table_length = 0;

/***********************************************************************
 *
 * update_group table handling
 *
 */

static void update_group_table_add( update_group_t *update_group, int id)
{
  if (id >= update_group_table_length)
    {
      while (id >= update_group_table_length)
	update_group_table_length *= 2;

      update_group_table = (update_group_t **)fts_realloc( update_group_table, update_group_table_length * sizeof( update_group_t *));
    }

  update_group_table[ id ] = update_group;
}


/***********************************************************************
 *
 * update_group class
 *
 */

static void update_group_timebase( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  update_group_t *this = (update_group_t *)o;

  if ( this->update_fifo)
    {
      int count;

      fts_client_send_message( (fts_object_t *)this, s_update_group_begin, 0, 0);

      for( count = 0; this->update_fifo && count < this->max_updates; count++)
	{
	  update_entry_t *p = this->update_fifo;

	  this->update_fifo = p->next;
	  fts_send_message(p->obj, fts_system_inlet, fts_s_update_real_time, 0, 0);
	  fts_object_release( p->obj);
	  fts_heap_free( p, update_heap);
	}

      fts_client_send_message( (fts_object_t *)this, s_update_group_end, 0, 0);
    }

  fts_timebase_add_call( fts_get_timebase(), (fts_object_t *)this, update_group_timebase, 0, this->period);
}

static void update_group_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  update_group_t *this = (update_group_t *)o;

  this->period = fts_get_int_arg( ac, at, 0, DEFAULT_PERIOD);
  this->max_updates = fts_get_int_arg( ac, at, 1, DEFAULT_MAX_UPDATES);
  this->update_fifo = 0;
  this->object_count = 0;
}

static void update_group_start( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  update_group_table_add( (update_group_t *)o, fts_get_client_id( o));
}

static fts_status_t update_group_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init( cl, sizeof( update_group_t), 1, 0, 0);

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, update_group_init);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_start, update_group_start);

  return fts_ok;
}


/***********************************************************************
 *
 * FTS objects update API implementation (essentialy compatibility w.r.t. properties)
 *
 */

static update_group_t *object_get_update_group( fts_object_t *obj)
{
  int id, index;

  id = fts_object_get_id( obj);

  if (id <= FTS_NO_ID)
    return NULL;

  index = OBJECT_ID_CLIENT( id );

  if (index < 0 || index >= update_group_table_length)
    return NULL;

  return update_group_table[ index];
}

static void update_group_add_object( update_group_t *this, fts_object_t *obj)
{
  update_entry_t **pp;

  /* check if the object is already in the update fifo */
  for ( pp = &this->update_fifo; *pp; pp = &(*pp)->next)
    if ((*pp)->obj == obj)
      return;

  *pp = (update_entry_t *)fts_heap_alloc( update_heap);

  (*pp)->obj = obj;
  (*pp)->next = 0;

  fts_object_refer(obj);

  if (this->object_count == 0)
    fts_timebase_add_call( fts_get_timebase(), (fts_object_t *)this, update_group_timebase, 0, this->period);

  this->object_count++;
}

static void update_group_remove_object( update_group_t *this, fts_object_t *obj)
{
  update_entry_t **pp;

  for ( pp = &this->update_fifo; *pp; pp = &(*pp)->next)
    {
      if ( (*pp)->obj == obj)
	{
	  update_entry_t *to_remove = *pp;

	  *pp = (*pp)->next;

	  fts_object_release( to_remove->obj);

	  fts_heap_free( to_remove, update_heap);

	  this->object_count--;

	  if (this->object_count == 0)
	    fts_timebase_remove_object( fts_get_timebase(), (fts_object_t *) this);

	  break;
	}
    }
}

void fts_update_request( fts_object_t *obj)
{
  update_group_t *update_group;
  fts_patcher_t *patcher;

  update_group = object_get_update_group( obj);

  if ( !update_group)
    return;

  patcher = fts_object_get_patcher( obj);

  if ( !patcher || !fts_patcher_is_open(patcher))
    return;

  update_group_add_object( update_group, obj);
}

void fts_update_reset( fts_object_t *obj)
{
  update_group_t *update_group;

  update_group = object_get_update_group( obj);

  if ( !update_group)
    return;

  update_group_remove_object( update_group, obj);
}



/***********************************************************************
 *
 * Initialization
 *
 */

void fts_update_config( void)
{
  s_update_group_begin = fts_new_symbol( "update_group_begin");
  s_update_group_end = fts_new_symbol( "update_group_end");

  update_heap = fts_heap_new( sizeof( update_entry_t));

  update_group_table_length = 256;
  update_group_table = (update_group_t **)fts_zalloc( update_group_table_length * sizeof( update_group_t *));

  fts_class_install( fts_new_symbol( "update_group"), update_group_instantiate);
}
