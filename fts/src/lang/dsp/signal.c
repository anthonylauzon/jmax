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

#include <fts/sys.h>
#include <fts/lang/mess.h>
#include <fts/lang/ftl.h>
#include <fts/lang/dsp.h>
/* (fd) For post */
#include <fts/runtime/files.h>

static int Sig_count = 0;

typedef struct _SignalCell 
{
  dsp_signal *s;
  struct _SignalCell *next;
} SignalCell, *SignalList;

static fts_heap_t *signal_cell_heap;

static SignalList freeList = 0, inUseList = 0;

static fts_symbol_t 
Sig_genName( int id)
{
  char tmp[12];

  sprintf( tmp, "_sig_%d", id);
  return fts_new_symbol_copy( tmp);
}

dsp_signal *
Sig_new( int vectorSize, float sampleRate)
{
  SignalList *previous;
  SignalList current, tmp;
  dsp_signal *s;

  previous = &freeList;
  for ( current = freeList; current; current = current->next )
    {
      s = current->s; 
      if ( s->length == vectorSize)
	{
	  *previous = current->next;
	  fts_heap_free((char *)current, signal_cell_heap);
	  break;
	}
      else
	previous = &(current->next);
    }

  if ( !current)
    {
      s = (dsp_signal *)fts_zalloc( sizeof(dsp_signal));
      s->id = Sig_count;
      Sig_count++;
      s->name = Sig_genName( s->id );
      dsp_add_signal(s->name, vectorSize);
    }

  s->refcnt = 0;
  s->length = vectorSize;
  s->srate = sampleRate;

  tmp = (SignalList) fts_heap_zalloc(signal_cell_heap);
  tmp->s = s;
  tmp->next = inUseList;
  inUseList = tmp;
  return s;
}

/* sig_free take away the sig from the inUseList,
   and put it back in the freelist.
 */

void
Sig_free( dsp_signal *s)
{
  /* First, delete the signal from the inUseList */

  SignalList *previous;
  SignalList current;
  SignalList tmp;

  previous = &inUseList;
  for (current = inUseList; current; current = current->next )
    {
      if (s == current->s)
	{
	  *previous = current->next;
	  fts_heap_free((char *) current, signal_cell_heap);

	  break;
	}
      else
	previous = &(current->next);
    }

  /* Then add it to the free list */

  tmp = (SignalList) fts_heap_zalloc(signal_cell_heap);
  tmp->s = s;
  tmp->next = freeList;
  freeList = tmp;
}

void
Sig_unreference(dsp_signal *s)
{
  s->refcnt--;

  if ( !s->refcnt)
    Sig_free( s);
}

void
Sig_reference(dsp_signal *s)
{
  s->refcnt++;
}

dsp_signal *
Sig_getById( int id)
{
  dsp_signal *s;
  SignalList current;

  for( current = inUseList; current; current = current->next)
    {
      s = current->s;
      if ( s->id == id)
	return s;
    }
  return 0;
}

void
Sig_print( dsp_signal *s)
{
  post( "dsp_signal *%p{ id=%d name=\"%s\" refCount=%p vs=%d}\n", s, s->id, fts_symbol_name(s->name), s->refcnt, s->length);
}

static void
SignalList_free(SignalList *list)
{
  SignalList current, next;

  for( current = *list; current; current = next)
    {
      fts_free( current->s );
      next = current->next;
      fts_heap_free((char *) current, signal_cell_heap);
    }
  *list = 0;
}
 
void
Sig_setup( int vectorSize)
{
  SignalList_free(&freeList);
  SignalList_free(&inUseList);
  Sig_count = 0;
}

int
Sig_getCount( void )
{
  return Sig_count;
}

int
Sig_check( void )
{
  return 1;
}

void Sig_init(void)
{
  signal_cell_heap = fts_heap_new(sizeof(SignalCell));
}
