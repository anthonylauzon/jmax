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
#include "seqsym.h"
#include "sequence.h"
#include "track.h"
#include "seqmidi.h"

#define SEQUENCE_ADD_BLOCK_SIZE 64

/*********************************************************
 *
 *  add/remove tracks
 *
 */

void
sequence_add_track(sequence_t *sequence, track_t *track)
{
  if(!sequence->tracks)
    {
      /* first track */
      sequence->tracks = track;
      sequence->size = 1;
    }
  else
    {
      /* append at end */
      track_t *last = sequence->tracks;

      while(last->next)
	last = last->next;

      last->next = track;
      sequence->size++;
    }
  
  track->sequence = sequence;
  track->next = 0;
}

void
sequence_remove_track(track_t *track)
{
  sequence_t *sequence = track_get_sequence(track);

  if(track == sequence->tracks)
    {
      /* first track */
      sequence->tracks = track->next;
      sequence->size--;
    }
  else
    {
      track_t *prev = sequence->tracks;
      track_t *this = track_get_next(prev);

      while(this && this != track)
	{
	  prev = this;
	  this = this->next;
	}

      if(this)
	{
	  prev->next = this->next;
	  sequence->size--;	  
	}
    }
}

track_t *
sequence_get_track_by_name(sequence_t *sequence, fts_symbol_t name)
{
  track_t *track = sequence->tracks;

  while(track && track_get_name(track) != name)
    track = track_get_next(track);

  return track;
}

/******************************************************
 *
 *  object
 *
 */

void
sequence_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;

  this->tracks = 0;
  this->size = 0;
  this->open = 0;  
}

void
sequence_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  track_t *track = sequence_get_first_track(this);

  while(track)
    {
      sequence_remove_track(track);
      fts_object_delete((fts_object_t *)track);

      track = sequence_get_first_track(this);
    }    
  
  fts_client_send_message(o, seqsym_destroyEditor, 0, 0);
}

/******************************************************
 *
 *  system methods
 *
 */

/* add new track by client request */
void
sequence_add_track_by_client_request(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  fts_symbol_t type = fts_get_symbol(at + 0);

  fts_object_t *track;
  fts_atom_t a[3];
  char str[] = "track9999";
  fts_symbol_t track_name;

  sprintf(str, "track%d", sequence_get_size(this));
  track_name = fts_new_symbol_copy(str);
  
  fts_set_symbol(a + 0, seqsym_eventtrk);
  fts_set_symbol(a + 1, track_name);
  fts_set_symbol(a + 2, type);
  fts_object_new(0, 3, a, &track);  
      
  /* add it to the track */
  sequence_add_track(this, (track_t *)track);
      
  /* create track at client */
  fts_client_upload(track, seqsym_track, 2, a + 1);
      
  /* add track to sequence at client */
  fts_set_object(a + 0, (fts_object_t *)track);	    
  fts_client_send_message(o, seqsym_addTracks, 1, a);
}

/* remove track by client request */
void
sequence_remove_track_by_client_request(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  track_t *track = (track_t *)fts_get_object(at + 0);
  fts_atom_t a[1];

  /* remove track from sequence */
  sequence_remove_track(track);
  
  /* add track to sequence at client */
  fts_set_object(a + 0, (fts_object_t *)track);	    
  fts_client_send_message(o, seqsym_deleteTracks, 1, a);
  
  /* delete track object */
  fts_object_delete((fts_object_t *)track);
}

void
sequence_upload(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  track_t *track = sequence_get_first_track(this);
  fts_atom_t a[SEQUENCE_ADD_BLOCK_SIZE];
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

	  if(n == SEQUENCE_ADD_BLOCK_SIZE)
	    {
	      fts_client_send_message(o, seqsym_addTracks, n, a);
	      n = 0;
	    }
	}

      track = track_get_next(track);
    }

  if(n > 0)
    fts_client_send_message(o, seqsym_addTracks, n, a);
}

void
sequence_open_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;

  sequence_set_editor_open(this);
  fts_client_send_message(o, seqsym_createEditor, 0, 0);
  sequence_upload(o, 0, 0, 0, 0);
}

void
sequence_close_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;

  sequence_set_editor_close(this);
  /* here we could aswell un-upload the objects (and the client would have to destroy the proxies) */
}

void
sequence_import_midifile(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  fts_symbol_t name = fts_get_symbol_arg(ac, at, 0, 0);

  if(name)
    sequence_read_midifile(this, name);

  if(sequence_editor_is_open(this))
    sequence_upload(o, 0, 0, 0, 0);
}

void 
sequence_import_midifile_with_dialog(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  fts_symbol_t default_name;
  char str[1024];
  fts_atom_t a[4];

  snprintf(str, 1024, " ");
  default_name = fts_new_symbol_copy(str);
      
  fts_set_symbol(a, seqsym_import_midi);
  fts_set_symbol(a + 1, fts_new_symbol("Import standard MIDI file"));
  fts_set_symbol(a + 2, fts_get_project_dir());
  fts_set_symbol(a + 3, default_name);
  fts_client_send_message((fts_object_t *)this, seqsym_openFileDialog, 4, a);
}

void
sequence_import(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  if(ac > 0 && fts_is_symbol(at))
    sequence_import_midifile(o, winlet, s, ac, at);
  else
    sequence_import_midifile_with_dialog(o, winlet, s, ac, at);
}

void
sequence_export_track_by_name(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  fts_symbol_t track_name = fts_get_symbol_arg(ac, at, 0, 0);
  track_t *track = sequence_get_track_by_name(this, track_name);

  if(track)
    {
      if(ac > 1 && fts_is_symbol(at + 1))
	fts_send_message((fts_object_t *)track, fts_SystemInlet, seqsym_export_midi, 1, at + 1);
      else
	fts_send_message((fts_object_t *)track, fts_SystemInlet, seqsym_export_midi_dialog, 0, 0);
    }
}

void
sequence_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  fts_symbol_t track_name = fts_get_symbol_arg(ac, at, 0, 0);
  int i = 0;

  post("sequence: %d track(s)\n", sequence_get_size(this));

  if(track_name)
    {
      track_t *track = sequence_get_track_by_name(this, track_name);

      if(track)
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

static void
sequence_get_state(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  fts_set_object(value, o);
}

/******************************************************
 *
 *  class
 *
 */

static fts_status_t
sequence_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(ac == 1)
    {
      fts_class_init(cl, sizeof(sequence_t), 1, 0, 0); 

      /* define variable */
      fts_class_add_daemon(cl, obj_property_get, fts_s_state, sequence_get_state);

      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, sequence_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, sequence_delete);

      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("open_editor"), sequence_open_editor);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("close_editor"), sequence_close_editor);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("add_track"), sequence_add_track_by_client_request);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("remove_track"), sequence_remove_track_by_client_request);

      fts_method_define_varargs(cl, 0, fts_s_print, sequence_print);
      fts_method_define_varargs(cl, 0, fts_new_symbol("import"), sequence_import);
      fts_method_define_varargs(cl, 0, fts_new_symbol("export"), sequence_export_track_by_name);

      fts_method_define_varargs(cl, fts_SystemInlet, seqsym_import_midi_dialog, sequence_import_midifile_with_dialog);
      fts_method_define_varargs(cl, fts_SystemInlet, seqsym_import_midi, sequence_import_midifile);
      
      return fts_Success;
    }
  else
    return &fts_CannotInstantiate;
}

void
sequence_config(void)
{
  fts_class_install(seqsym_sequence, sequence_instantiate);
}
