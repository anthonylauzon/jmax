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
#include <fts/runtime.h> /* (fd) For post */

/* ********************************************************************** */
/* Signal list handling                                                   */
/* ********************************************************************** */

typedef struct _fts_dsp_signal_list_t 
{
  fts_dsp_signal_t *signal;
  struct _fts_dsp_signal_list_t *next;
} fts_dsp_signal_list_t;

static void fts_dsp_signal_list_insert( fts_dsp_signal_list_t **list, fts_dsp_signal_t *sig)
{
  fts_dsp_signal_list_t *tmp;

  tmp = (fts_dsp_signal_list_t *)fts_malloc( sizeof( fts_dsp_signal_list_t) );
  tmp->signal = sig;
  tmp->next = *list;
  *list = tmp;
}

static void fts_dsp_signal_list_delete( fts_dsp_signal_list_t *list)
{
  fts_dsp_signal_list_t *current, *next;

  for ( current = list; current; current = next )
    {
      next = current->next;
      fts_free( current);
    }
}

/* ********************************************************************** */
/* Signals functions                                                      */
/* ********************************************************************** */

static int signal_count = 0;
static fts_dsp_signal_list_t *signal_list = 0;

#define SIGNAL_PENDING -1

fts_dsp_signal_t *fts_dsp_signal_new( int vector_size, float sample_rate)
{
  fts_dsp_signal_list_t *current;
  fts_dsp_signal_t *sig;
  char buffer[16];

  for ( current = signal_list; current; current = current->next )
    {
      if ( current->signal->refcnt == 0 && current->signal->length == vector_size)
	{
	  current->signal->refcnt = SIGNAL_PENDING;
	  return current->signal;
	}
    }

  sig = (fts_dsp_signal_t *)fts_zalloc( sizeof(fts_dsp_signal_t));

  sprintf( buffer, "_sig_%d", signal_count++);
  sig->name = fts_new_symbol_copy( buffer);
  sig->refcnt = SIGNAL_PENDING;
  sig->length = vector_size;
  sig->srate = sample_rate;

  dsp_add_signal( sig->name, vector_size);

  fts_dsp_signal_list_insert( &signal_list, sig);

  return sig;
}

void fts_dsp_signal_free( fts_dsp_signal_t *sig)
{
  sig->refcnt = 0;
}

int fts_dsp_signal_is_pending( fts_dsp_signal_t *sig)
{
  return sig->refcnt == SIGNAL_PENDING;
}

void fts_dsp_signal_unreference( fts_dsp_signal_t *sig)
{
  if (sig->refcnt != SIGNAL_PENDING)
    sig->refcnt--;
}

void fts_dsp_signal_reference(fts_dsp_signal_t *sig)
{
  if (sig->refcnt == SIGNAL_PENDING)
    sig->refcnt = 1;
  else
    sig->refcnt++;
}

void fts_dsp_signal_print( fts_dsp_signal_t *s)
{
  post( "dsp_signal *%p{ name=\"%s\" refcnt=%p vs=%d}\n", s, fts_symbol_name(s->name), s->refcnt, s->length);
}

void fts_dsp_signal_init( void)
{
  fts_dsp_signal_list_t *current;

  /* free all the signal currently in signal list */
  for ( current = signal_list; current; current = current->next )
    {
      fts_free( current->signal);
    }

  fts_dsp_signal_list_delete( signal_list);

  signal_count = 0;
  signal_list = 0;
}



