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
#include <ftsconfig.h>
#include "seqsym.h"
#include "sequence.h"
#include "track.h"
#include "seqmidi.h"

#define SEQUENCE_ADD_BLOCK_SIZE 64

/*********************************************************
 *
 *  get, add, remove and move tracks
 *
 */

track_t *
sequence_get_track_by_name(sequence_t *sequence, fts_symbol_t name)
{
  track_t *track = sequence->tracks;

  while(track && track_get_name(track) != name)
    track = track_get_next(track);

  return track;
}

track_t *
sequence_get_track_by_index(sequence_t *sequence, int index)
{
  track_t *track = 0;

  if(index >= 0)
    {
      track = sequence->tracks;
      
      while(track && index--)
	track = track_get_next(track);
    }

  return track;
}

static void
sequence_insert_track(sequence_t *sequence, track_t *here, track_t *track)
{
  if(!here)
    {
      /* first track */
      track->next = sequence->tracks;      
      sequence->tracks = track;
      sequence->size = 1;
    }
  else
    {
      /* insert after track here */
      track->next = here->next;
      here->next = track;
      sequence->size++;
    }

  fts_object_refer((fts_object_t *)track);

  track->sequence = sequence;
}

void
sequence_add_track(sequence_t *sequence, track_t *track)
{
  track_t *last = sequence->tracks;

  while(last && last->next)
    last = last->next;
  
  sequence_insert_track(sequence, last, track);
}

void
sequence_remove_track(sequence_t *sequence, track_t *track)
{
  if(track == sequence->tracks)
    {
      /* first track */
      sequence->tracks = track->next;
      sequence->size--;

      fts_object_release((fts_object_t *)track);
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

	  fts_object_release((fts_object_t *)track);
	}
    }
}

static void
sequence_move_track(sequence_t *sequence, track_t *track, int index)
{
  track_t *here = sequence_get_track_by_index(sequence, index - 1);
    
  if(track != here && (index == 0 || here))
    {
      fts_object_refer((fts_object_t *)track);
      
      sequence_remove_track(sequence, track);
      sequence_insert_track(sequence, 0, track);

      fts_object_release((fts_object_t *)track);
    }
}

static void
sequence_array_update(sequence_t *this)
{
  track_t *track = sequence_get_first_track(this);
  
  fts_array_set_size(&this->array, 0);

  while(track)
    {
      fts_atom_t a;

      fts_set_object_with_type(&a, (fts_object_t *)track, seqsym_track);
      fts_array_append(&this->array, 1, &a);

      track = track_get_next(track);
    }

  fts_object_redefine_variable((fts_object_t *)this);
}

static void
sequence_array_append(sequence_t *this, track_t *track)
{
  fts_atom_t a;

  fts_set_object_with_type(&a, (fts_object_t *)track, seqsym_track);
  fts_array_append(&this->array, 1, &a);

  fts_object_redefine_variable((fts_object_t *)this);
}

/******************************************************
 *
 *  object
 *
 */

static void
sequence_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;

  this->tracks = 0;
  this->size = 0;
  this->open = 0;  

  fts_array_init(&this->array, 0, 0);
}

static void
sequence_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  track_t *track = sequence_get_first_track(this);

  while(track)
    {
      sequence_remove_track(this, track);

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
static void
sequence_add_track_by_client_request(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  track_t *track;
  fts_atom_t a[1];

  /* create new track (for now anyway just event tracks!!! - later to be changed) */
  fts_set_symbol(a, seqsym_note);  
  track = (track_t *)fts_object_create(track_class, 1, at);

  /* add it to the sequence */
  sequence_add_track(this, track);

  if(sequence_editor_is_open(this))
    {
      /* create track at client */
      fts_client_upload((fts_object_t *)track, seqsym_track, 1, at);
      
      /* add track to sequence at client */
      fts_set_object(a + 0, (fts_object_t *)track);	    
      fts_client_send_message(o, seqsym_addTracks, 1, a);
    }

  sequence_array_append(this, track);
}

/* remove track by client request */
static void
sequence_remove_track_by_client_request(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  track_t *track = (track_t *)fts_get_object(at);

  sequence_remove_track(this, track);
  
  if(fts_object_has_id((fts_object_t *)track))
    fts_client_send_message(o, seqsym_deleteTracks, 1, at);

  sequence_array_update(this);
}

/* move track by client request */
static void
sequence_move_track_by_client_request(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  track_t *track = (track_t *)fts_get_object(at + 0);
  int index = fts_get_int(at + 1);

  sequence_move_track(this, track, index);
  
  if(fts_object_has_id((fts_object_t *)track))
    fts_client_send_message(o, seqsym_moveTrack, 2, at);

  sequence_array_update(this);
}

static void
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

static void
sequence_open_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;

  sequence_set_editor_open(this);
  fts_client_send_message(o, seqsym_createEditor, 0, 0);
  sequence_upload(o, 0, 0, 0, 0);
}

static void
sequence_close_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;

  sequence_set_editor_close(this);
}

static void 
sequence_hide_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  
  if(sequence_editor_is_open(this))
    {
      sequence_set_editor_close(this);
      fts_client_send_message((fts_object_t *)this, seqsym_closeEditor, 0, 0);  
    }
}

static void
sequence_import_from_midifile(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  fts_symbol_t name = fts_get_symbol(at);
  fts_midifile_t *file = fts_midifile_open_read(name);

  if(file)
    {
      int size = sequence_import_midifile(this, file);

      if(fts_midifile_get_error(file))
	post("sequence: error reading MIDI file %s (%s)\n", fts_symbol_name(name), fts_midifile_get_error(file));
      else if(size <= 0)
	post("sequence: couldn't import from MIDI file %s\n", fts_midifile_get_name(file));
      
      fts_midifile_close(file);
      
      if(sequence_editor_is_open(this))
	sequence_upload(o, 0, 0, 0, 0);
      
      sequence_array_update(this);
    }
  else
    post("sequence: cannot open file %s\n", fts_symbol_name(name));
}
      
static void 
sequence_import_from_midifile_dialog(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  fts_symbol_t default_name;
  char str[1024];
  fts_atom_t a[4];

  snprintf(str, 1024, " ");
  default_name = fts_new_symbol_copy(str);
      
  fts_set_symbol(a, seqsym_import_midifile);
  fts_set_symbol(a + 1, fts_new_symbol("Import standard MIDI file"));
  fts_set_symbol(a + 2, fts_project_get_dir());
  fts_set_symbol(a + 3, default_name);
  fts_client_send_message((fts_object_t *)this, seqsym_openFileDialog, 4, a);
}

static void
sequence_import(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;

  if(ac > 0 && fts_is_symbol(at))
    sequence_import_from_midifile(o, 0, 0, 1, at);
  else
    sequence_import_from_midifile_dialog(o, 0, 0, 1, at);
}

static void
sequence_export_track_by_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;

  if(ac && fts_is_number(at))
    {
      int index = fts_get_number_int(at) - 1;
      track_t *track = sequence_get_track_by_index(this, index);
      
      if(track)
	{
	  if(ac > 1 && fts_is_symbol(at + 1))
	    fts_send_message((fts_object_t *)track, fts_SystemInlet, seqsym_export_midifile, 1, at + 1);
	  else
	    fts_send_message((fts_object_t *)track, fts_SystemInlet, seqsym_export_midifile_dialog, 0, 0);
	}
    }
}

static void
sequence_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;

  if(ac == 0)
    {  
      track_t *track = sequence_get_first_track(this);
      
      while(track)
	{      
	  sequence_remove_track(this, track);
	  fts_client_send_message(o, seqsym_deleteTracks, 1, at);

	  track = sequence_get_first_track(this);
	}    

      sequence_array_update(this);
    }
  else if(ac && fts_is_number(at))
    {
      int index = fts_get_number_int(at) - 1;
      track_t *track = sequence_get_track_by_index(this, index);
      
      if(track)
	fts_send_message((fts_object_t *)track, fts_SystemInlet, seqsym_clear, 0, 0);
    }
}

static void
sequence_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;

  if(ac == 0)
    {
      track_t *track = sequence_get_first_track(this);
      int size = sequence_get_size(this);
      int i;

      post("sequence: %d track(s)\n", size);

      for(i=1; i<=size; i++)
	{
	  post("%d: ", i);
	  
	  fts_send_message((fts_object_t *)track, fts_SystemInlet, seqsym_print, 0, 0);
	  track = track_get_next(track);
	}
    }
  else if(ac && fts_is_number(at))
    {
      int index = fts_get_number_int(at) - 1; 
      track_t *track = sequence_get_track_by_index(this, index);
      
      if(track)
	fts_send_message((fts_object_t *)track, fts_SystemInlet, seqsym_print, 0, 0);
    }
}

static void
sequence_send_name_to_client(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;

  if(fts_object_get_variable(o))
    {
      fts_atom_t a[1];
      
      fts_set_symbol(a, fts_object_get_variable(o));
      fts_client_send_message(o, seqsym_setName, 1, a);
    }
}

static void
sequence_get_state(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  sequence_t *this = (sequence_t *)o;

  fts_set_array(value, &this->array);
}

/******************************************************
 *
 *  load/save bmax
 *
 */

static void
sequence_add_track_from_bmax(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  fts_atom_t a[1];
  track_t *track;
  
  /* create new track (for now anyway just event tracks!!! - later to be changed) */
  fts_set_symbol(a, seqsym_note);  
  track = (track_t *)fts_object_create(track_class, 1, at);

  /* add it to the sequence */
  sequence_add_track(this, track);

  if(ac > 1)
    {
      fts_symbol_t name = fts_get_symbol(at + 1);
      track_set_name(track, name);
    }

  this->currently_loaded_event_track = track;

  sequence_array_update(this);
}

static void
sequence_add_event_from_bmax(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;

  fts_send_message((fts_object_t *)this->currently_loaded_event_track, fts_SystemInlet, seqsym_bmax_add_event, ac, at);
}  

static void
sequence_save_bmax(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  fts_bmax_file_t *f = (fts_bmax_file_t *) fts_get_ptr(at);  
  track_t *track = sequence_get_first_track(this);

  while(track)
    {
      fts_send_message((fts_object_t *)track, fts_SystemInlet, fts_s_save_bmax, ac, at);
      track = track_get_next(track);
    }
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
      
      /* bmax files */
      fts_method_define_varargs(cl, fts_SystemInlet, seqsym_bmax_add_track, sequence_add_track_from_bmax);
      fts_method_define_varargs(cl, fts_SystemInlet, seqsym_bmax_add_event, sequence_add_event_from_bmax);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_save_bmax, sequence_save_bmax);

      /* graphical editor */
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("open_editor"), sequence_open_editor);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("close_editor"), sequence_close_editor);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("hide"), sequence_hide_editor);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("add_track"), sequence_add_track_by_client_request);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("remove_track"), sequence_remove_track_by_client_request);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("move_track"), sequence_move_track_by_client_request);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("get_name"), sequence_send_name_to_client);

      fts_method_define_varargs(cl, 0, seqsym_clear, sequence_clear);
      fts_method_define_varargs(cl, 0, seqsym_print, sequence_print);
      fts_method_define_varargs(cl, 0, fts_new_symbol("add_track"), sequence_add_track_by_client_request);

      fts_method_define_varargs(cl, 0, fts_new_symbol("open"), sequence_open_editor);

      /* MIDI files */
      fts_method_define_varargs(cl, fts_SystemInlet, seqsym_import_midifile_dialog, sequence_import_from_midifile_dialog);
      fts_method_define_varargs(cl, fts_SystemInlet, seqsym_import_midifile, sequence_import_from_midifile);
      fts_method_define_varargs(cl, 0, fts_new_symbol("import"), sequence_import);
      fts_method_define_varargs(cl, 0, fts_new_symbol("export"), sequence_export_track_by_index);

      return fts_Success;
    }
  else
    return &fts_CannotInstantiate;
}

void
_sequence_config(void)
{
  fts_class_install(seqsym_sequence, sequence_instantiate);
}
