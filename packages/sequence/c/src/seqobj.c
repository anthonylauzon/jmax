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

#include "fts.h"

#include "sequence.h"
#include "track.h"

#include "eventtrk.h"
#include "noteevt.h"

#include "seqmidi.h"

static fts_symbol_t sym_openEditor = 0;
static fts_symbol_t sym_destroyEditor = 0;
static fts_symbol_t sym_addTracks = 0;

static fts_symbol_t sym_export_midi = 0;

#define SEQOBJ_ADD_BLOCK_SIZE 64

/******************************************************
 *
 *  object
 *
 */

void
seqobj_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;

  sequence_init(this);
}

void
seqobj_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  track_t *track = sequence_get_first_track(this);

  while(track)
    {
      sequence_remove_track(track);
      fts_object_delete((fts_object_t *)track);

      track = sequence_get_first_track(this);
    }    
  
  fts_client_send_message(o, sym_destroyEditor, 0, 0);
}

/******************************************************
 *
 *  system methods
 *
 */

/* add new track by client request */
void
seqobj_track_add_by_client_request(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  fts_symbol_t type = fts_get_symbol(at + 0);
  fts_symbol_t track_class;

  /*if(type == noteevt_symbol)*/
  track_class = eventtrk_symbol;
  /*else
    track_class = 0;*/

  if(track_class)
    {
      fts_object_t *track;
      fts_atom_t a[3];
      char str[] = "track9999";
      fts_symbol_t track_name;

      sprintf(str, "track%d", sequence_get_size(this));
      track_name = fts_new_symbol_copy(str);
  
      fts_set_symbol(a + 0, track_class);
      fts_set_symbol(a + 1, track_name);
      fts_set_symbol(a + 2, type);
      fts_object_new(0, 3, a, &track);  
      
      /* add it to the track */
      sequence_add_track(this, (track_t *)track);
      
      /* create track at client */
      fts_client_upload(track, track_symbol, 2, a + 1);
      
      /* add track to sequence at client */
      fts_set_object(a + 0, (fts_object_t *)track);	    
      fts_client_send_message(o, sym_addTracks, 1, a);
    }
}

/* remove track by client request */
void
seqobj_track_remove_by_client_request(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  track_t *track = (track_t *)fts_get_object(at + 0);

  /* remove track from sequence */
  sequence_remove_track(track);

  /* delete track object */
  fts_object_delete((fts_object_t *)track);
}

void
seqobj_update(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  track_t *track = sequence_get_first_track(this);
  fts_atom_t a[SEQOBJ_ADD_BLOCK_SIZE];
  int n = 0;

  while(track)
    {
      int exported = fts_object_has_id((fts_object_t *)track);

      /* upload track */
      fts_send_message((fts_object_t *)track, fts_SystemInlet, fts_s_upload, 0, 0);
	  
      if(!exported)
	{
	  fts_set_object(a + n, (fts_object_t *)track);
	  n++;

	  if(n == SEQOBJ_ADD_BLOCK_SIZE)
	    {
	      fts_client_send_message(o, sym_addTracks, n, a);
	      n = 0;
	    }
	}

      track = track_get_next(track);
    }

  if(n > 0)
    fts_client_send_message(o, sym_addTracks, n, a);
}

void
seqobj_open_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;

  sequence_set_editor_open(this);
  fts_client_send_message(o, sym_openEditor, 0, 0);
  seqobj_update(o, 0, 0, 0, 0);
}

void
seqobj_import(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  fts_symbol_t name = fts_get_symbol_arg(ac, at, 0, 0);

  if(name)
    sequence_read_midifile(this, name);

  if(sequence_editor_open(this))
    seqobj_update(o, 0, 0, 0, 0);
}

void
seqobj_export(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  fts_symbol_t track_name = fts_get_symbol_arg(ac, at, 0, 0);
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 1, 0);
  track_t *track = sequence_get_track_by_name(this, track_name);

  if(track)
    fts_send_message((fts_object_t *)track, fts_SystemInlet, sym_export_midi, 1, at + 1);  
}

void
seqobj_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  fts_symbol_t track_name = fts_get_symbol_arg(ac, at, 0, 0);
  int i = 0;

  post("sequence: %d track(s)\n", sequence_get_size(this));

  if(track_name)
    {
      track_t *track = sequence_get_track_by_name(this, track_name);

      fts_send_message((fts_object_t *)track, fts_SystemInlet, fts_s_print, 0, 0);
    }
  else
    {  
      track_t *track = sequence_get_first_track(this);

      while(track)
	{
	  fts_send_message((fts_object_t *)track, fts_SystemInlet, fts_s_print, 0, 0);
	  track = track_get_next(track);
	  i++;
	}
    }
}

/******************************************************
 *
 *  class
 *
 */

static fts_status_t
seqobj_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(ac == 1)
    {
      fts_class_init(cl, sizeof(sequence_t), 1, 0, 0); 

      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, seqobj_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, seqobj_delete);

      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("open_editor"), seqobj_open_editor);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("track_add"), seqobj_track_add_by_client_request);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("track_remove"), seqobj_track_remove_by_client_request);

      fts_method_define_varargs(cl, 0, fts_s_print, seqobj_print);
      fts_method_define_varargs(cl, 0, fts_new_symbol("import"), seqobj_import);
      fts_method_define_varargs(cl, 0, fts_new_symbol("export"), seqobj_export);
      
      return fts_Success;
    }
  else
    return &fts_CannotInstantiate;
}

void
seqobj_config(void)
{
  sym_openEditor = fts_new_symbol("openEditor");
  sym_destroyEditor = fts_new_symbol("destroyEditor");
  sym_addTracks = fts_new_symbol("addTracks");
  sym_export_midi = fts_new_symbol("export_midi");

  fts_class_install(fts_new_symbol("sequence"), seqobj_instantiate);
}
