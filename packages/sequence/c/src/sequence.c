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

#define SEQUENCE_ADD_BLOCK_SIZE 64

fts_class_t *multitrack_class = 0;

/*static void sequence_init_editor(sequence_t *self);
static void sequence_upload_editor(sequence_t *self);*/

/*********************************************************
*
*  get, add, remove and move tracks
*
*/

track_t *
sequence_get_track_by_name(sequence_t *sequence, fts_symbol_t name)
{
  track_t *track = sequence->tracks;

  while(track != NULL)
  {
    if(sequence_track_get_name(track) == name)
      return track;
    
    track = sequence_track_get_next(track);
  }
  
  return NULL;
}

track_t *
sequence_get_track_by_index(sequence_t *sequence, int index)
{
  track_t *track = NULL;

  if(index >= 0)
  {
    track = sequence->tracks;

    while(track != NULL && index > 0)
    {
      track = sequence_track_get_next(track);
      index--;
    }
  }

  return track;
}

void
sequence_track_set_name(track_t *track, fts_symbol_t name)
{
  sequence_context_t *context = (sequence_context_t *)fts_object_get_context((fts_object_t *)track);
  sequence_t *sequence = context->container;
  
  if(sequence_get_track_by_name(sequence, name) == NULL)
    context->name = name;
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
  sequence_context_t *context = (sequence_context_t *)fts_object_get_context(obj);

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
  sequence_context_t *context = (sequence_context_t *)fts_object_get_context(obj);
  
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
  
  if(fts_object_has_client((fts_object_t *)track) == 0)
  {
		fts_atom_t a[2];
    
    fts_client_register_object((fts_object_t *)track, fts_object_get_client_id((fts_object_t *)this));
    
		fts_set_int(a, fts_object_get_id((fts_object_t *)track));
    fts_set_symbol(a + 1, fts_class_get_name(track_type));
    fts_client_send_message( (fts_object_t *)this, seqsym_addTracks, 2, a);		
  }
}

void
sequence_upload(sequence_t *self)
{
  track_t *track = sequence_get_first_track(self);
  fts_atom_t a;
  
  fts_client_send_message((fts_object_t *)self, fts_s_start_upload, 0, 0);
  
  /* upload editor stuff */
  /*if(self->save_editor != 0)
  {
    fts_set_int(&a, self->save_editor); 
    fts_client_send_message(o, seqsym_save_editor, 1, &a);
    sequence_upload_editor(self);
  }*/  
  
  while(track)
  {
    /* add track at client and upload events */
    sequence_add_track_at_client(self, track);
    fts_send_message((fts_object_t *)track, fts_s_upload, 0, 0, fts_nix);
		fts_send_message((fts_object_t *)track, seqsym_set_editor, 0, 0, fts_nix); /* to create trackeditor */
      
    /* next track */
    track = sequence_track_get_next(track);
  }
  
  fts_client_send_message((fts_object_t *)self, fts_s_end_upload, 0, 0);
}

static fts_method_status_t
sequence_member_upload(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  sequence_add_track_at_client((sequence_t *)o, (track_t *)fts_get_object(&at[0]));
  
  return fts_ok;
}

static fts_method_status_t
sequence_member_dirty(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_object_set_state_dirty(o);

  return fts_ok;
}

/* move track by client request */
static fts_method_status_t
_sequence_upload(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  sequence_t *self = (sequence_t *)o;
  
  sequence_upload(self);
    
  return fts_ok;
}

static fts_method_status_t
sequence_open_editor(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  sequence_t *this = (sequence_t *)o;
	  
  sequence_set_editor_open(this);
  fts_client_send_message(o, fts_s_openEditor, 0, 0);
  sequence_upload(this);
  
  return fts_ok;
}

static fts_method_status_t
sequence_destroy_editor(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  sequence_t *this = (sequence_t *)o;

  sequence_set_editor_close(this);
  
  return fts_ok;
}

static fts_method_status_t
sequence_close_editor(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  sequence_t *this = (sequence_t *)o;

  if(sequence_editor_is_open(this))
  {
    sequence_set_editor_close(this);
    fts_client_send_message((fts_object_t *)this, fts_s_closeEditor, 0, 0);
  }
  
  return fts_ok;
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

static fts_method_status_t
sequence_clear(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  sequence_t *this = (sequence_t *)o;
  int i = 0;
  
  if(ac == 0 && sequence_get_size(this) > 0)
  {
    track_t *track = sequence_get_first_track(this);

    while(track)
    {
      sequence_remove_track(this, track);
      track = sequence_get_first_track(this);
      i++;
    }
    
    //if(sequence_editor_is_open(this))    
    fts_client_send_message(o, fts_s_clear, 0, 0);
    /* fts_name_update(o); */
  }
  
  return fts_ok;
}

static fts_method_status_t
sequence_get_element(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
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
      *ret = a;
    }
  }
  
  return fts_ok;
}
  
static fts_method_status_t
sequence_print(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  sequence_t *this = (sequence_t *)o;
  track_t *track = sequence_get_first_track(this);
  int size = sequence_get_size(this);
  fts_bytestream_t* stream = fts_get_default_console_stream();
  int i;
  
  if(ac > 0 && fts_is_object(at))
    stream = (fts_bytestream_t *)fts_get_object(at);
  
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
  
  return fts_ok;
}




/******************************************************
 *
 *  add, remove, move, dump
 *
 */

/* add new track by client request */
static fts_method_status_t
sequence_add_track_and_update(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  sequence_t *this = (sequence_t *)o;
  
  if(ac > 0)
  {
    track_t *track = NULL;
    
    if(fts_is_a(at, track_class))
      track = (track_t *)fts_get_object(at);
    else if(fts_is_symbol(at))
      track = (track_t *)fts_object_create(track_class, 1, at);
    
    if(track != NULL)
    {
      sequence_add_track(this, track);
      
      /* set name */
      if(ac > 1 && fts_is_symbol(at + 1))
        sequence_track_set_name(track, fts_get_symbol(at + 1));
      
      if(sequence_editor_is_open(this))
      {
        track_set_editor_open(track);
        sequence_add_track_at_client(this, track);
        fts_send_message((fts_object_t *)track, fts_s_upload, 0, 0, fts_nix);
        fts_send_message((fts_object_t *)track, seqsym_set_editor, 0, 0, fts_nix);
        fts_object_set_state_dirty(o);
      }
      
      this->last_loaded_track = track; /* hack to fix loading */
    }
  }
  
  /* return self */
  fts_set_object(ret, o);
  return fts_ok;
}


/* remove track by client request */
static fts_method_status_t
sequence_remove_track_and_update(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  sequence_t *this = (sequence_t *)o;
  track_t *track = (track_t *)fts_get_object(at);

  fts_client_send_message(o, seqsym_removeTracks, 1, at);
  sequence_remove_track(this, track);

  /* fts_name_update(o); */
  fts_object_set_state_dirty(o);
  
  /* return self */
  fts_set_object(ret, o);
  return fts_ok;
}


static fts_method_status_t
sequence_move_track_and_update(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  sequence_t *this = (sequence_t *)o;
  track_t *track = (track_t *)fts_get_object(at + 0);
  int index = fts_get_int(at + 1);

  sequence_move_track(this, track, index);
  fts_client_send_message(o, seqsym_moveTrack, 2, at);

  /* fts_name_update(o); */
  fts_object_set_state_dirty(o);
  
  /* return self */
  fts_set_object(ret, o);
  return fts_ok;
}


static fts_method_status_t
sequence_dump_state(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  sequence_t *this = (sequence_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  track_t *track = sequence_get_first_track(this);
  fts_message_t *mess = (fts_message_t *)fts_object_create(fts_message_class, 0, 0);
  
  fts_object_refer((fts_object_t *)mess);  
  
  while(track != NULL)
  {
    fts_symbol_t name = track_get_name(track);
    
    fts_message_set(mess, seqsym_add_track, 0, 0);
    fts_message_append_object(mess, (fts_object_t *)track);
    
    if(name != NULL)
      fts_message_append_symbol(mess, name);
    
    fts_dumper_message_send(dumper, mess);
    
    track = sequence_track_get_next(track);
  }
 
  /*if(this->save_editor == 1)
  {
    fts_atom_t a[4];
    fts_set_int(a, this->save_editor);
    fts_dumper_send(dumper, seqsym_save_editor, 1, a);

    fts_set_int(a, this->win_x);
    fts_set_int(a + 1, this->win_y);
    fts_set_int(a + 2, this->win_w);
    fts_set_int(a + 3, this->win_h);
    fts_dumper_send(dumper, seqsym_window, 4, a);
      
    fts_set_float(a, this->zoom);
    fts_dumper_send(dumper, seqsym_zoom, 1, a);
    
    fts_set_int(a, this->transp);
    fts_dumper_send(dumper, seqsym_transp, 1, a);
  }*/
  
  fts_object_release((fts_object_t *)mess);  
  
  return fts_ok;
}

/* hack to fix loading */
static fts_method_status_t
sequence_append_event_at_last_loaded_track(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  sequence_t *this = (sequence_t *)o;

  if(this->last_loaded_track!=NULL)
    fts_send_message((fts_object_t *)this->last_loaded_track, fts_s_append, ac, at, fts_nix); 
  
  /* return self */
  fts_set_object(ret, o);
  return fts_ok;
}


/*static fts_method_status_t
sequence_set_save_editor(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  sequence_t *this = (sequence_t *)o;
  if(ac == 1) 
  {
    this->save_editor = fts_get_int(at);
    
    if(sequence_editor_is_open(this))
      fts_client_send_message(o, seqsym_save_editor, 1, at);
    
    fts_object_set_dirty(o);
  }
  
  return fts_ok;
}*/

/***
 * sequence editor
 ***/

/*static void 
sequence_upload_editor(sequence_t *self)
{
  fts_atom_t a[6];
  fts_set_int(a, self->win_x);
  fts_set_int(a+1, self->win_y);
  fts_set_int(a+2, self->win_w);
  fts_set_int(a+3, self->win_h);
  fts_set_int(a+4, self->zoom);
  fts_set_int(a+5, self->transp);
  
  fts_client_send_message((fts_object_t *)self, seqsym_editor, 6, a);
}

static fts_method_status_t
sequence_editor_window(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  sequence_t *this = (sequence_t *)o;
  if(ac == 4)
  {
    int x = fts_get_int(at);
    int y = fts_get_int(at+1);
    int w = fts_get_int(at+2);
    int h = fts_get_int(at+3);
    if(this->win_x!=x || this->win_y != y || this->win_w!=w || this->win_h != h)
    {
      this->win_x = x;
      this->win_y = y;
      this->win_w = w;
      this->win_h = h;
      
      if(track_do_save_editor(this->track))
        fts_object_set_dirty((fts_object_t *)this->track);
    }
  }
  
  return fts_ok;
}	

static fts_method_status_t
sequence_editor_zoom(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  sequence_t *this = (sequence_t *)o;
  if(ac == 1 && fts_is_float(at))
  {
    float zoom = fts_get_float(at);
    if(this->zoom != zoom)
    {
      this->zoom = zoom;
      if(track_do_save_editor(this->track))
        fts_object_set_dirty((fts_object_t *)this->track);
    }
  }	
  
  return fts_ok;
}
static fts_method_status_t
sequence_editor_transp(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  sequence_t *this = (sequence_t *)o;
  if(ac == 1 && fts_is_int(at))
  {
    int transp = fts_get_int(at);
    if(this->transp != transp)
    {
      this->transp = transp;
      if(track_do_save_editor(this->track))
        fts_object_set_dirty((fts_object_t *)this->track);
    }
  }	
  
  return fts_ok;
}	*/



/******************************************************
*
*  class
*
*/

static fts_method_status_t
sequence_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  sequence_t *this = (sequence_t *)o;
  
  this->tracks = 0;
  this->size = 0;
  this->open = 0;
  
  this->last_loaded_track = NULL;/* hack to fix loading */
  
  /* editor*/ 
  this->save_editor = 0;
  this->win_x = -1; 
  this->win_y = -1;
  this->win_w = -1;
  this->win_h = -1;
  this->zoom = 0.2; 
  this->transp = 0;  
  
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
          return fts_ok;
      }
      else
      {
        fts_object_error(o, "bad arguments");
        return fts_ok;
      }
    }
  }
  
  return fts_ok;
}

static fts_method_status_t
sequence_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  sequence_t *this = (sequence_t *)o;
  track_t *track = sequence_get_first_track(this);

  while(track)
  {
    sequence_remove_track(this, track);
    track = sequence_get_first_track(this);
  }

  fts_client_send_message(o, fts_s_destroyEditor, 0, 0);
  
  return fts_ok;
}

static void
sequence_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(sequence_t), sequence_init, sequence_delete);

  fts_class_message_varargs(cl, fts_s_name, fts_object_name);
  fts_class_message_varargs(cl, fts_s_persistence, fts_object_persistence);
  /*fts_class_message_varargs(cl, seqsym_save_editor, sequence_set_save_editor);*/
  fts_class_message_varargs(cl, fts_s_dump_state, sequence_dump_state);

  fts_class_message_varargs(cl, fts_s_member_upload, sequence_member_upload);
  fts_class_message_varargs(cl, fts_s_member_dirty, sequence_member_dirty);
  fts_class_message_varargs(cl, fts_s_upload, _sequence_upload);

  fts_class_message_varargs(cl, seqsym_add_track, sequence_add_track_and_update);
  fts_class_message_varargs(cl, seqsym_remove_track, sequence_remove_track_and_update);
  fts_class_message_varargs(cl, seqsym_move_track, sequence_move_track_and_update);
  
  fts_class_message_varargs(cl, fts_s_append, sequence_append_event_at_last_loaded_track);

  fts_class_message_varargs(cl, fts_s_print, sequence_print);

  fts_class_message_varargs(cl, fts_s_get_element, sequence_get_element);
  /* sequence editor */
  /*fts_class_message_varargs(cl, seqsym_window, sequence_editor_window);
  fts_class_message_varargs(cl, seqsym_zoom, sequence_editor_zoom);
  fts_class_message_varargs(cl, seqsym_transp, sequence_editor_transp);*/

  /* graphical editor */
  fts_class_message_varargs(cl, fts_s_openEditor, sequence_open_editor);
  fts_class_message_varargs(cl, fts_s_destroyEditor, sequence_destroy_editor);
  fts_class_message_varargs(cl, fts_s_closeEditor, sequence_close_editor);

  fts_class_message_varargs(cl, fts_s_clear, sequence_clear);

  fts_class_inlet_thru(cl, 0);
}

FTS_MODULE_INIT(sequence_class)
{
  multitrack_class = fts_class_install(seqsym_multitrack, sequence_instantiate);
  fts_class_alias(multitrack_class, seqsym_sequence);
}
