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
 */
#include <fts/fts.h>
#include <ftsconfig.h>
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

void
sequence_set_dirty(sequence_t *sequence)
{
  if(sequence->persistence == 1)
    fts_patcher_set_dirty( fts_object_get_patcher( (fts_object_t *)sequence), 1);
}

static void
sequence_set_persistence(sequence_t *this, int persistence)
{
  track_t *track = sequence_get_first_track(this);
  fts_atom_t a;

  fts_set_int(&a, persistence);
  
  this->persistence = persistence;
  fts_client_send_message((fts_object_t *)this, fts_s_persistence, 1, &a);

  /* set flag of all tracks */
  while(track != NULL)
  {
    track->persistence = persistence;
    track = track->next;
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
  fts_symbol_t track_name;

  if( !fts_object_has_id((fts_object_t *)track))
  {
    fts_client_register_object((fts_object_t *)track, fts_get_client_id((fts_object_t *)this));

    fts_client_start_message( (fts_object_t *)this, seqsym_addTracks);
    fts_client_add_int( (fts_object_t *)this, fts_get_object_id((fts_object_t *)track));
    fts_client_add_symbol( (fts_object_t *)this, fts_class_get_name(track_type));

    track_name = track_get_name(track);
    if( track_name)
      fts_client_add_symbol( (fts_object_t *)this, track_name);

    fts_client_done_message( (fts_object_t *)this);
  }
}

static void
sequence_upload_child( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_add_track_at_client((sequence_t *)o, (track_t *)fts_get_object(&at[0]));
}

static void
sequence_send_name_to_client(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_t a;

  fts_set_symbol(&a, seqsym_sequence);
  fts_client_send_message(o, seqsym_setName, 1, &a);
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
        fts_object_error(o, "import: read error in \"%s\" (%s)\n", name, error);
      else if(size <= 0)
        fts_object_error(o, "import: couldn't get any data from \"%s\"\n", fts_midifile_get_name(file));

      fts_midifile_close(file);

      if(sequence_editor_is_open(this))
        sequence_upload(o, 0, 0, 0, 0);

      fts_object_update_name(o);
    }
    else
      fts_object_error(o, "import: cannot open \"%s\"\n", name);
  }
}

static void
sequence_import_midifile_dialog(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_symbol_t default_name;
  char str[512];
  fts_atom_t a[4];

  snprintf(str, 1024, " ");
  default_name = fts_new_symbol(str);

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
        fts_send_message((fts_object_t *)track, fts_s_import, ac - 1, at + 1);
      else
        /* for now this is not allowed */
        fts_object_error(o, "import: file name required");
    }
    else
      fts_object_error(o, "import: no track #%d", index);
  }
  else
    fts_object_error(o, "import: wrong arguments");
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
        fts_send_message((fts_object_t *)track, fts_s_export, ac - 1, at + 1);
      else
        /* for now this is not allowed */
        fts_object_error(o, "export: file name required");
    }
    else
      fts_object_error(o, "export: no track #%d", index);
  }
  else
    fts_object_error(o, "export: track number required");
}

static void
sequence_insert_event(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;

  if(ac > 0 && fts_is_number(at))
  {
    int index = fts_get_number_int(at);
    track_t *track = sequence_get_track_by_index(this, index);

    if(track)
      fts_send_message((fts_object_t *)track, seqsym_insert, ac - 1, at + 1);
    else
      fts_object_error(o, "insert: no track #%d", index);
  }
  else
    fts_object_error(o, "insert: track number required");
}

static void
sequence_remove_event(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;

  if(ac > 0 && fts_is_number(at))
  {
    int index = fts_get_number_int(at);
    track_t *track = sequence_get_track_by_index(this, index);

    if(track)
      fts_send_message((fts_object_t *)track, seqsym_remove, ac - 1, at + 1);
    else
      fts_object_error(o, "remove: no track #%d", index);
  }
  else
    fts_object_error(o, "remove: track number required");
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

    fts_object_update_name(o);
  }
  else if(ac && fts_is_number(at))
  {
    int index = fts_get_number_int(at);
    track_t *track = sequence_get_track_by_index(this, index);

    if(track)
      fts_send_message((fts_object_t *)track, fts_s_clear, 0, 0);
  }
}

static void
sequence_get_element(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;

  if(ac > 0 && fts_is_number(at))
  {
    int index = fts_get_number_int(at);

    if(index >= 0 && index < sequence_get_size(this))
    {
      track_t *track = sequence_get_track_by_index(this, index);
      fts_atom_t a;

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
    fts_spost(stream, "(:sequence)");
  else
  {
    track_t *track = sequence_get_first_track(this);

    fts_spost(stream, "(:sequence");

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

      track = track_get_next(track);
    }

    fts_spost(stream, ")");
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
      const char *name_str = track_name? track_name: "untitled";

      if(track_type != NULL)
      {
        fts_symbol_t type_name = fts_class_get_name(track_type);
        fts_spost(stream, "  track %d: \"%s\" %d %s event(s)\n", i, name_str, track_size, type_name);
      }
      else
        fts_spost(stream, "  track %d: \"%s\" %d event(s)\n", i, name_str, track_size);

      track = track_get_next(track);
    }

    fts_spost(stream, "}\n");
  }
}



static void
sequence_persistence(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;

  if(ac > 0)
  {
    /* set persistence flag */
    if(fts_is_number(at) && this->persistence >= 0)
      sequence_set_persistence(this, (fts_get_number_int(at) != 0));
  }
  else
  {
    /* return persistence flag */
    fts_atom_t a;

    fts_set_int(&a, this->persistence);
    fts_return(&a);
  }
}

static void
sequence_update_gui(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;
  fts_atom_t a;

  fts_set_int(&a, (this->persistence > 0));
  fts_client_send_message(o, fts_s_persistence, 1, &a);

  fts_name_gui_method(o, 0, 0, 0, 0);
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
    sequence_add_track_at_client(this, track);
    sequence_set_dirty( this);
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

  fts_object_update_name(o);
  sequence_set_dirty( this);
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

  fts_object_update_name(o);
  sequence_set_dirty( this);
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

  if(this->persistence == 1)
  {
    fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
    track_t *track = sequence_get_first_track(this);

    while(track)
    {
      fts_symbol_t name = track_get_name(track);
      fts_class_t *type = track_get_type(track);
      fts_message_t *mess = fts_dumper_message_new(dumper, seqsym_add_track);

      fts_message_append_symbol(mess, fts_class_get_name(type));

      if(name)
        fts_message_append_symbol(mess, name);

      fts_dumper_message_send(dumper, mess);

      /* write track events */
      track_dump((fts_object_t *)track, 0, 0, 1, at);
      track = track_get_next(track);
    }
  }

  fts_name_dump_method(o, 0, 0, ac, at);
}

static void
sequence_dump(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sequence_t *this = (sequence_t *)o;

  if(this->persistence == 1)
  {
    fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
    fts_atom_t a;

    sequence_dump_state(o, 0, 0, ac, at);

    /* save persistence flag */
    fts_set_int(&a, 1);
    fts_dumper_send(dumper, fts_s_persistence, 1, &a);
  }

  fts_name_dump_method(o, 0, 0, ac, at);
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

  this->persistence = 0;
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

    this->persistence = -1;
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

    this->persistence = -1;
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

  fts_class_message_varargs(cl, fts_s_name, fts_name_set_method);
  fts_class_message_varargs(cl, fts_s_persistence, sequence_persistence);
  fts_class_message_varargs(cl, fts_s_update_gui, sequence_update_gui);
  fts_class_message_varargs(cl, fts_s_dump, sequence_dump);

  /* old sequence bmax files */
  fts_class_message_varargs(cl, fts_new_symbol("bmax_add_track"), sequence_add_track_and_update);
  fts_class_message_varargs(cl, fts_new_symbol("bmax_add_event"), sequence_add_event_from_file);

  fts_class_message_varargs(cl, fts_s_upload_child, sequence_upload_child);
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
  fts_class_message_varargs(cl, fts_new_symbol("getName"), sequence_send_name_to_client);
  fts_class_message_varargs(cl, fts_s_openEditor, sequence_open_editor);
  fts_class_message_varargs(cl, fts_s_destroyEditor, sequence_destroy_editor);
  fts_class_message_varargs(cl, fts_s_closeEditor, sequence_close_editor);

  fts_class_message_varargs(cl, fts_s_clear, sequence_clear);

  fts_class_message_varargs(cl, seqsym_insert, sequence_insert_event);
  fts_class_message_varargs(cl, seqsym_remove, sequence_remove_event);

  fts_class_message_varargs(cl, fts_s_import, sequence_import);
  fts_class_message_varargs(cl, fts_s_export, sequence_export_track_by_index);

  fts_class_inlet_thru(cl, 0);
}

void
sequence_class_config(void)
{
  fts_class_install(seqsym_sequence, sequence_instantiate);
}
