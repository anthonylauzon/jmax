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
      track_get_next(track) = sequence->tracks;      
      sequence->tracks = track;
      sequence->size = 1;
    }
  else
    {
      /* insert after track here */
      track_get_next(track) = here->next;
      here->next = track;
      sequence->size++;
    }

  fts_object_refer((fts_object_t *)track);

  track_set_sequence(track, sequence);
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
      sequence->tracks = track_get_next(track);
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
 *  system methods
 *
 */

static void
sequence_add_track_at_client(sequence_t *this, track_t *track)
{
  if(!fts_object_has_id((fts_object_t *)track))
    {
      fts_symbol_t track_type = track_get_type(track);
      fts_atom_t a;
      
      /* create track at client */
      fts_set_symbol(&a, track_type);
      fts_client_upload((fts_object_t *)track, seqsym_track, 1, &a);

      /* add track to sequence at client */
      fts_set_object(&a, (fts_object_t *)track);
      fts_client_send_message((fts_object_t *)this, seqsym_addTracks, 1, &a);
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

/* move track by client request */
static void
sequence_upload(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  track_t *track = sequence_get_first_track(this);

  while(track)
    {
      /* add track at client and upload events */
      sequence_add_track_at_client(this, track);
      fts_send_message((fts_object_t *)track, fts_SystemInlet, fts_s_upload, 0, 0);

      /* next track */
      track = track_get_next(track);
    }
}

static void
sequence_open_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;

  sequence_set_editor_open(this);
  fts_client_send_message(o, fts_s_openEditor, 0, 0);
  sequence_upload(o, 0, 0, 0, 0);
}

static void
sequence_destroy_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;

  sequence_set_editor_close(this);
}

static void
sequence_close_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;

  if(sequence_editor_is_open(this))
    {
      sequence_set_editor_close(this);
      fts_client_send_message((fts_object_t *)this, fts_s_closeEditor, 0, 0);
    }
}

static void
sequence_import_midifile(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;

  if(ac == 1 && fts_is_symbol(at))
    {
      fts_symbol_t name = fts_get_symbol(at);
      fts_midifile_t *file = fts_midifile_open_read(name);
      
      if(file)
	{
	  int size = sequence_import_from_midifile(this, file);
	  char *error = fts_midifile_get_error(file);

	  if(error)
	    fts_object_signal_runtime_error(o, "import: read error in \"%s\" (%s)\n", fts_symbol_name(name), error);
	  else if(size <= 0)
	    fts_object_signal_runtime_error(o, "import: couldn't get any data from \"%s\"\n", fts_midifile_get_name(file));
	  
	  fts_midifile_close(file);
	  
	  if(sequence_editor_is_open(this))
	    sequence_upload(o, 0, 0, 0, 0);
	  
	  sequence_array_update(this);
	}
      else
	fts_object_signal_runtime_error(o, "import: cannot open \"%s\"\n", fts_symbol_name(name));
    }
}
      
static void 
sequence_import_midifile_dialog(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  fts_symbol_t default_name;
  char str[512];
  fts_atom_t a[4];
  
  snprintf(str, 1024, " ");
  default_name = fts_new_symbol_copy(str);
  
  fts_set_symbol(a, seqsym_import_midifile);
  fts_set_symbol(a + 1, fts_new_symbol("Import standard MIDI file"));
  fts_set_symbol(a + 2, fts_project_get_dir());
  fts_set_symbol(a + 3, default_name);
  fts_client_send_message(o, seqsym_openFileDialog, 4, a);
}

static void
sequence_import(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;

  if(ac == 0)
    sequence_import_midifile_dialog(o, 0, 0, 0, 0);
  else if(ac == 1 && fts_is_symbol(at))
    sequence_import_midifile(o, 0, 0, 1, at);
  else if(ac > 0 && fts_is_number(at))
    {
      int index = fts_get_number_int(at);
      track_t *track = sequence_get_track_by_index(this, index);

      if(track)
	{
	  if(ac > 1)
	    fts_send_message((fts_object_t *)track, fts_SystemInlet, fts_s_import, ac - 1, at + 1);
	  else
	    /* for now this is not allowed */
	    fts_object_signal_runtime_error(o, "import: file name required");
	}
      else	
	fts_object_signal_runtime_error(o, "import: no track #%d", index);
    }
  else
    fts_object_signal_runtime_error(o, "import: wrong arguments");  
}

static void
sequence_export_track_by_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;

  if(ac > 0 && fts_is_number(at))
    {
      int index = fts_get_number_int(at);
      track_t *track = sequence_get_track_by_index(this, index);
      
      if(track)
	{
	  if(ac > 1)
	    fts_send_message((fts_object_t *)track, fts_SystemInlet, fts_s_export, ac - 1, at + 1);
	  else
	    /* for now this is not allowed */
	    fts_object_signal_runtime_error(o, "export: file name required");
	}
      else	
	fts_object_signal_runtime_error(o, "export: no track #%d", index);
    }
  else
    fts_object_signal_runtime_error(o, "export: track number required");  
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
	  
	  if(sequence_editor_is_open(this))
	    fts_client_send_message(o, seqsym_removeTracks, 1, at);

	  track = sequence_get_first_track(this);
	}    

      sequence_array_update(this);
    }
  else if(ac && fts_is_number(at))
    {
      int index = fts_get_number_int(at);
      track_t *track = sequence_get_track_by_index(this, index);
      
      if(track)
	fts_send_message((fts_object_t *)track, fts_SystemInlet, fts_s_clear, 0, 0);
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

      if(size == 0)
	post("(empty sequence)\n");
      else
	{
	  post("(%d track%s) {\n", size, (size > 1)? "s": "");

	  for(i=0; i<size; i++)
	    {
	      fts_symbol_t track_name = track_get_name(track);
	      fts_symbol_t track_type = track_get_type(track);
	      int track_size = track_get_size(track);
	      const char *name_str = track_name? fts_symbol_name(track_name): "untitled";

	      post("  track %d: \"%s\" %d %s event%s\n", i, name_str, track_size, 
		   fts_symbol_name(track_type), (track_size == 1)? "": "s");

	      track = track_get_next(track);
  	    }

	  post("}\n");      
  	}
    }
  else if(ac && fts_is_number(at))
    {
      int index = fts_get_number_int(at);
      track_t *track = sequence_get_track_by_index(this, index);

      if(track)
	fts_send_message((fts_object_t *)track, fts_SystemInlet, fts_s_print, 0, 0);
      else
	fts_object_signal_runtime_error(o, "print: no track %d", index);
    }
}

static void
sequence_set_keep(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  sequence_t *this = (sequence_t *)obj;

  if(this->keep != fts_s_args && fts_is_symbol(value))
    this->keep = fts_get_symbol(value);
}

static void
sequence_return_keep(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  sequence_t *this = (sequence_t *)obj;

  fts_set_symbol(value, this->keep);
}

static void
sequence_get_state(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  sequence_t *this = (sequence_t *)o;

  fts_set_array(value, &this->array);
}

/******************************************************
 *
 *  add, remove, move, dump
 *
 */

/* add new track by client request */
static void
sequence_add_track_and_update(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  track_t *track = (track_t *)fts_object_create(track_class, 1, at);

  /* add it to the sequence */
  sequence_add_track(this, track);

  /* set name */
  if(ac > 1 && fts_is_symbol(at + 1))
    track_set_name(track, fts_get_symbol(at + 1));

  this->last_track = track;

  if(sequence_editor_is_open(this))
    sequence_add_track_at_client(this, track);

  sequence_array_append(this, track);
}

/* remove track by client request */
static void
sequence_remove_track_and_update(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  track_t *track = (track_t *)fts_get_object(at);

  sequence_remove_track(this, track);
  
  if(fts_object_has_id((fts_object_t *)track))
    fts_client_send_message(o, seqsym_removeTracks, 1, at);

  sequence_array_update(this);
}

static void
sequence_move_track_and_update(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
sequence_add_event_to_last_track(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;

  track_add_event_from_array((fts_object_t *)this->last_track, 0, 0, ac, at);
}  

static void
sequence_dump(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  track_t *track = sequence_get_first_track(this);

  while(track)
    {
      fts_symbol_t name = track_get_name(track);
      fts_message_t *mess = fts_dumper_message_new(dumper, seqsym_add_track);

      fts_message_append_symbol(mess, track_get_type(track));

      if(name)
	fts_message_append_symbol(mess, name);

      fts_dumper_message_send(dumper, mess);

      /* write track events */
      track_dump((fts_object_t *)track, 0, 0, 1, at);
      track = track_get_next(track);
    }
}

/******************************************************
 *
 *  class
 *
 */

static void
sequence_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;

  ac--;
  at++;

  this->tracks = 0;
  this->size = 0;
  this->open = 0;  
  this->keep = fts_s_no;

  fts_array_init(&this->array, 0, 0);

  if(ac == 1 && fts_is_int(at))
    {
      /* create void tracks */
      int size = fts_get_int(at);
      fts_atom_t a;
      int i;
      
      if(size < 0)
	size = 0;
      
      /* create void tracks */
      fts_set_symbol(&a, fts_s_void);
      
      for(i=0; i<size; i++)
	{
	  track_t *track = (track_t *)fts_object_create(track_class, 1, &a);
	  
	  /* add it to the sequence */
	  sequence_add_track(this, track);
	  sequence_array_append(this, track);
	}
      
      this->keep = fts_s_args;
    }
  else if(ac > 0)
    {
      int i;
      
      /* create typed tracks */
      for(i=0; i<ac; i++)
	{
	  if(fts_is_symbol(at + i))
	    {
	      fts_symbol_t type = fts_get_symbol(at + i);
	      track_t *track = (track_t *)fts_object_create(track_class, 1, at + i);
	      
	      if(track)
		{
		  /* add it to the sequence */
		  sequence_add_track(this, track);
		  sequence_array_append(this, track);
		}
	      else
		fts_object_set_error(o, "Cannot create track of type %s", fts_symbol_name(type));
	    }
	  else
	    fts_object_set_error(o, "Wrong arguments");
	}
      
      this->keep = fts_s_args;
    }
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
  
  fts_client_send_message(o, fts_s_destroyEditor, 0, 0);
}

static fts_status_t
sequence_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(sequence_t), 1, 0, 0); 
  
  fts_class_add_daemon(cl, obj_property_put, fts_s_keep, sequence_set_keep);
  fts_class_add_daemon(cl, obj_property_get, fts_s_keep, sequence_return_keep);
  fts_class_add_daemon(cl, obj_property_get, fts_s_state, sequence_get_state);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, sequence_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, sequence_delete);
  
  fts_method_define_varargs(cl, fts_SystemInlet, seqsym_add_track, sequence_add_track_and_update);
  fts_method_define_varargs(cl, fts_SystemInlet, seqsym_remove_track, sequence_remove_track_and_update);
  fts_method_define_varargs(cl, fts_SystemInlet, seqsym_move_track, sequence_move_track_and_update);
  fts_method_define_varargs(cl, fts_SystemInlet, seqsym_add_event, sequence_add_event_to_last_track);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_dump, sequence_dump);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_print, sequence_print);
  
  /* graphical editor */
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("getName"), sequence_send_name_to_client);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_openEditor, sequence_open_editor);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_destroyEditor, sequence_destroy_editor);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_closeEditor, sequence_close_editor); 
  
  fts_method_define_varargs(cl, 0, fts_s_clear, sequence_clear);
  fts_method_define_varargs(cl, 0, fts_s_print, sequence_print);
  
  /* MIDI files */
  fts_method_define_varargs(cl, fts_SystemInlet, seqsym_import_midifile_dialog, sequence_import_midifile_dialog);
  fts_method_define_varargs(cl, fts_SystemInlet, seqsym_import_midifile, sequence_import_midifile);
  fts_method_define_varargs(cl, 0, fts_s_import, sequence_import);
  fts_method_define_varargs(cl, 0, fts_s_export, sequence_export_track_by_index);
  
  return fts_Success;
}

void
sequence_class_config(void)
{
  fts_class_install(seqsym_sequence, sequence_instantiate);
}
