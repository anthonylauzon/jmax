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
#include <fts/fts.h>
#include <ftsconfig.h>
#include <fts/packages/data/data.h>
#include <fts/packages/sequence/sequence.h>
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
    track = sequence_track_get_next(track);

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
      track = sequence_track_get_next(track);
  }

  return track;
}

/*********************************************************
 *
 *  sequence context
 *
 */
static fts_heap_t *sequence_context_heap = NULL;

static sequence_context_t *
sequence_context_get(fts_object_t *obj)
{
  sequence_context_t *context = (sequence_context_t *)obj->context;

  if(context == NULL)
  {
    if(sequence_context_heap == NULL)
      sequence_context_heap = fts_heap_new(sizeof(sequence_context_t));
    
    context = fts_heap_alloc(sequence_context_heap);
    context->container = NULL;
    context->next = NULL;
    
    obj->context = (fts_context_t *)context;
  }
  
  return context;
}

static void
sequence_context_remove(fts_object_t *obj)
{
  sequence_context_t *context = (sequence_context_t *)obj->context;
  
  if(context != NULL)
  {
    fts_heap_free((void *)context, sequence_context_heap);
    obj->context = NULL;
  }
}

static void
sequence_insert_track(sequence_t *sequence, track_t *here, track_t *track)
{
	sequence_context_t *context = sequence_context_get((fts_object_t *)track);
		
  if(here == NULL)
  {
    /* first track */
		context->next = sequence->tracks;
    sequence->tracks = track;
    sequence->size++;
  }
  else
  {
    /* insert after track here */
		context->next = sequence_track_get_next(here);
    sequence_track_set_next(here, track);
    sequence->size++;
  }

  context->container = sequence;
  fts_object_refer((fts_object_t *)track);
}

void
sequence_add_track(sequence_t *sequence, track_t *track)
{
  track_t *last = sequence->tracks;

  while(last && sequence_track_get_next(last))
    last = sequence_track_get_next(last);

  sequence_insert_track(sequence, last, track);
}

void
sequence_remove_track(sequence_t *sequence, track_t *track)
{
  if(track == sequence->tracks)
  {
    /* first track */
    sequence->tracks = sequence_track_get_next(track);
    sequence->size--;

    sequence_context_remove((fts_object_t *)track);
    fts_object_release((fts_object_t *)track);
  }
  else
  {
    track_t *prev = sequence->tracks;
    track_t *this = sequence_track_get_next(prev);

    while(this && this != track)
    {
      prev = this;
      this = sequence_track_get_next(this);
    }

    if(this)
    {
      sequence_track_set_next(prev, sequence_track_get_next(this));
      sequence->size--;

      sequence_context_remove((fts_object_t *)track);
      fts_object_release((fts_object_t *)track);
    }
  }
}

static void
sequence_move_track(sequence_t *sequence, track_t *track, int index)
{
  track_t *here = sequence_get_track_by_index(sequence, index - 1);

  if(track != here && (index == 0 || here != NULL))
  {
    fts_object_refer((fts_object_t *)track);

    sequence_remove_track(sequence, track);
    sequence_insert_track(sequence, here, track);

    fts_object_release((fts_object_t *)track);
  }
}

/******************************************************
 *
 *  system methods
 *
 */
static void
sequence_add_track_at_client(sequence_t *this, track_t *track)
{
  fts_class_t *track_type = track_get_type(track);

  if( !fts_object_has_id((fts_object_t *)track))
  {
		fts_atom_t a[2];
    
    fts_client_register_object((fts_object_t *)track, fts_object_get_client_id((fts_object_t *)this));

		fts_set_int(a, fts_object_get_id((fts_object_t *)track));
    
    if(track_type != NULL)
      fts_set_symbol(a + 1, fts_class_get_name(track_type));
    else
      fts_set_symbol(a + 1, fts_s_void);     

    fts_client_send_message( (fts_object_t *)this, seqsym_addTracks, 2, a);		
  }
}

static void
sequence_member_upload( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_add_track_at_client((sequence_t *)o, (track_t *)fts_get_object(&at[0]));
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
    fts_send_message((fts_object_t *)track, fts_s_upload, 0, 0);
		fts_send_message((fts_object_t *)track, seqsym_set_editor, 0, 0);/* to create trackeditor */

    /* next track */
    track = sequence_track_get_next(track);
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

void 
sequence_set_editor_open(sequence_t *sequence)
{
  track_t *track = sequence_get_first_track(sequence);

  sequence->open = 1;
  
  while(track != NULL)
  {
    track_set_editor_open(track);
    track = sequence_track_get_next(track);
  }
}

void 
sequence_set_editor_close(sequence_t *sequence)
{
  track_t *track = sequence_get_first_track(sequence);
  
  sequence->open = 0;
  
  while(track != NULL)
  {
    track_set_editor_close(track);
    track = sequence_track_get_next(track);
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
        fts_object_error(o, "import: read error in \"%s\" (%s)", fts_symbol_name(name), error);
      else if(size <= 0)
        fts_object_error(o, "import: couldn't get any data from \"%s\"", fts_symbol_name(fts_midifile_get_name(file)));

      fts_midifile_close(file);

      if(sequence_editor_is_open(this))
        sequence_upload(o, 0, 0, 0, 0);

      /* fts_name_update(o); */
    }
    else
      fts_object_error(o, "import: cannot open \"%s\"", fts_symbol_name(name));
  }
}

static void
sequence_import_midifile_dialog(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
	fts_object_open_dialog(o, seqsym_import_midifile, fts_new_symbol("Import standard MIDI file"));
}

static void
sequence_import(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if(ac == 0)
    sequence_import_midifile_dialog(o, 0, 0, 0, 0);
  else if(ac == 1 && fts_is_symbol(at))
    sequence_import_midifile(o, 0, 0, 1, at);
  else
    fts_object_error(o, "import: wrong arguments");
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

    /* fts_name_update(o); */
  }
}

static void
sequence_get_element(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  track_t *track = NULL;
  fts_atom_t a;
  
  if(ac > 0)
  {
    if(fts_is_number(at))
    {
      int index = fts_get_number_int(at);

      if(index >= 0 && index < sequence_get_size(this))
        track = sequence_get_track_by_index(this, index);
    }
    else if(fts_is_symbol(at))
      track = sequence_get_track_by_name(this, fts_get_symbol(at));

    if(track != NULL)
    {
      fts_set_object( &a, track);
      fts_return( &a);
    }
  }
}
  
static void
sequence_post(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  fts_bytestream_t *stream = fts_post_get_stream(ac, at);
  int size = sequence_get_size(this);

  if(size == 0)
    fts_spost(stream, "<sequence>");
  else
  {
    track_t *track = sequence_get_first_track(this);

    fts_spost(stream, "<sequence");

    while(track != NULL)
    {
      fts_class_t *track_type = track_get_type(track);

      if(track_type != NULL)
      {
        fts_spost(stream, " ");
        fts_spost_symbol(stream, fts_class_get_name(track_type));
      }
      else
        fts_spost(stream, " -");

      track = sequence_track_get_next(track);
    }

    fts_spost(stream, ">");
  }
}

static void
sequence_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  fts_bytestream_t *stream = fts_post_get_stream(ac, at);
  track_t *track = sequence_get_first_track(this);
  int size = sequence_get_size(this);
  int i;

  if(size == 0)
    fts_spost(stream, "<empty sequence>\n");
  else
  {
    if(size == 1)
      fts_spost(stream, "<sequence of 1 track>\n");
    else
      fts_spost(stream, "<sequence of %d tracks>\n", size);

    fts_spost(stream, "{\n", size);

    for(i=0; i<size; i++)
    {
      fts_symbol_t track_name = track_get_name(track);
      fts_class_t *track_type = track_get_type(track);
      int track_size = track_get_size(track);
      const char *name_str = track_name? fts_symbol_name(track_name): "untitled";

      if(track_type != NULL)
      {
        fts_symbol_t type_name = fts_class_get_name(track_type);
        fts_spost(stream, "  track %d: \"%s\" %d %s event(s)\n", i, name_str, track_size, fts_symbol_name(type_name));
      }
      else
        fts_spost(stream, "  track %d: \"%s\" %d event(s)\n", i, name_str, track_size);

      track = sequence_track_get_next(track);
    }

    fts_spost(stream, "}\n");
  }
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
  {
    track_set_editor_open(track);
    sequence_add_track_at_client(this, track);
    fts_send_message((fts_object_t *)track, fts_s_upload, 0, 0);
		fts_send_message((fts_object_t *)track, seqsym_set_editor, 0, 0);
    fts_object_set_state_dirty(o);
  }
}

/* remove track by client request */
static void
sequence_remove_track_and_update(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  track_t *track = (track_t *)fts_get_object(at);

  if(fts_object_has_id((fts_object_t *)track))
    fts_client_send_message(o, seqsym_removeTracks, 1, at);

  sequence_remove_track(this, track);

  /* fts_name_update(o); */
  fts_object_set_state_dirty(o);
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

  /* fts_name_update(o); */
  fts_object_set_state_dirty(o);
}

static void
sequence_add_event_from_file(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;

  track_add_event_from_file((fts_object_t *)this->last_track, 0, 0, ac, at);
}

static void
sequence_event_dump_mess(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;

  track_event_dump_mess((fts_object_t *)this->last_track, 0, 0, ac, at);
}

static void
sequence_dump_state(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  track_t *track = sequence_get_first_track(this);
  
  while(track)
  {
    fts_symbol_t name = track_get_name(track);
    fts_class_t *type = track_get_type(track);
    fts_message_t *mess = fts_dumper_message_new(dumper, seqsym_add_track);
    
    if(type != NULL)
      fts_message_append_symbol(mess, fts_class_get_name(type));
    else
      fts_message_append_symbol(mess, fts_s_void);
    
    if(name)
      fts_message_append_symbol(mess, name);
    
    fts_dumper_message_send(dumper, mess);
    
    /* write track events */
    track_dump_state((fts_object_t *)track, 0, 0, 1, at);
    track = sequence_track_get_next(track);
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
  
  this->tracks = 0;
  this->size = 0;
  this->open = 0;
  
  if(ac == 1 && fts_is_number(at))
  {
    /* create void tracks */
    int size = fts_get_number_int(at);
    int i;
    
    if(size < 0)
      size = 0;
    
    for(i=0; i<size; i++)
    {
      track_t *track = (track_t *)fts_object_create(track_class, 0, NULL);
      
      /* add it to the sequence */
      sequence_add_track(this, track);
    }
  }
  else if(ac > 0)
  {
    int i;
    
    /* create typed tracks */
    for(i=0; i<ac; i++)
    {
      if(fts_is_symbol(at + i))
      {
        track_t *track = (track_t *)fts_object_create(track_class, 1, at + i);
        
        if(track != NULL)
          sequence_add_track(this, track);
        else
          return;
      }
      else
      {
        fts_object_error(o, "bad arguments");
        return;
      }
    }
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

static void
sequence_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(sequence_t), sequence_init, sequence_delete);

  fts_class_message_varargs(cl, fts_s_name, fts_object_name);
  fts_class_message_varargs(cl, fts_s_persistence, fts_object_persistence);
  fts_class_message_varargs(cl, fts_s_dump_state, sequence_dump_state);

  /* old sequence bmax files */
  fts_class_message_varargs(cl, fts_new_symbol("bmax_add_track"), sequence_add_track_and_update);
  fts_class_message_varargs(cl, fts_new_symbol("bmax_add_event"), sequence_add_event_from_file);

  fts_class_message_varargs(cl, fts_s_member_upload, sequence_member_upload);
  fts_class_message_varargs(cl, fts_s_upload, sequence_upload);

  fts_class_message_varargs(cl, seqsym_add_track, sequence_add_track_and_update);
  fts_class_message_varargs(cl, seqsym_remove_track, sequence_remove_track_and_update);
  fts_class_message_varargs(cl, seqsym_move_track, sequence_move_track_and_update);

  fts_class_message_varargs(cl, seqsym_add_event, sequence_add_event_from_file);
  fts_class_message_varargs(cl, seqsym_dump_mess, sequence_event_dump_mess);

  fts_class_message_varargs(cl, fts_s_post, sequence_post);
  fts_class_message_varargs(cl, fts_s_print, sequence_print);

  fts_class_message_varargs(cl, fts_s_get_element, sequence_get_element);

  /* MIDI files */
  fts_class_message_varargs(cl, seqsym_import_midifile_dialog, sequence_import_midifile_dialog);
  fts_class_message_varargs(cl, seqsym_import_midifile, sequence_import_midifile);

  /* graphical editor */
  fts_class_message_varargs(cl, fts_s_openEditor, sequence_open_editor);
  fts_class_message_varargs(cl, fts_s_destroyEditor, sequence_destroy_editor);
  fts_class_message_varargs(cl, fts_s_closeEditor, sequence_close_editor);

  fts_class_message_varargs(cl, fts_s_clear, sequence_clear);
  fts_class_message_varargs(cl, fts_s_import, sequence_import);

  fts_class_inlet_thru(cl, 0);
}

void
sequence_class_config(void)
{
  fts_class_install(seqsym_sequence, sequence_instantiate);
}
