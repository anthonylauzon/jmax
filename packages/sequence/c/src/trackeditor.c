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

fts_class_t *track_editor_class = 0;

void
track_editor_upload(track_editor_t *this)
{
  if(this)
  {
		if(this->win_x!=-1 && this->win_y!=-1 && this->win_w!=-1 && this->win_h!=-1)
		{
			fts_atom_t a[7];
			fts_set_int(a, this->win_x);
			fts_set_int(a+1, this->win_y);
			fts_set_int(a+2, this->win_w);
			fts_set_int(a+3, this->win_h);
			fts_set_symbol(a+4, this->label);
			fts_set_float(a+5, this->zoom);
			fts_set_int(a+6, this->transp);

			fts_client_send_message((fts_object_t *)this, seqsym_editor, 7, a);
		}
	}
}

void 
track_editor_dump_gui(track_editor_t *this, fts_dumper_t *dumper)
{
  fts_atom_t a[4];

  fts_set_int(a, this->win_x);
  fts_set_int(a + 1, this->win_y);
  fts_set_int(a + 2, this->win_w);
  fts_set_int(a + 3, this->win_h);
  fts_dumper_send(dumper, seqsym_window, 4, a);

  fts_set_symbol(a, this->label);
  fts_dumper_send(dumper, seqsym_label, 1, a);

  fts_set_float(a, this->zoom);
  fts_dumper_send(dumper, seqsym_zoom, 1, a);

  fts_set_int(a, this->transp);
  fts_dumper_send(dumper, seqsym_transp, 1, a);
}


static void
track_editor_window(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_editor_t *this = (track_editor_t *)o;
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
}	

static void
track_editor_label(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_editor_t *this = (track_editor_t *)o;
  if(ac == 1 && fts_is_symbol(at))
  {
    fts_symbol_t label = fts_get_symbol(at);
    if(this->label != label)
    {
      this->label = label;
      if(track_do_save_editor(this->track))
        fts_object_set_dirty((fts_object_t *)this->track);
    }
  }
}	

static void
track_editor_zoom(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_editor_t *this = (track_editor_t *)o;
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
}
static void
track_editor_transp(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_editor_t *this = (track_editor_t *)o;
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
}	

static void
track_editor_set_state(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_editor_t *this = (track_editor_t *)o;
  if(ac >= 7)
  {
		int x = fts_get_int(at);
    int y = fts_get_int(at+1);
    int w = fts_get_int(at+2);
    int h = fts_get_int(at+3);
		fts_symbol_t label = fts_get_symbol(at+4);
		float zoom = fts_get_float(at+5);
    int transp = fts_get_int(at+6);
		if(this->win_x!=x || this->win_y != y || this->win_w!=w || this->win_h != h || this->label != label || this->zoom != zoom || this->transp != transp)
    {
      this->win_x = x;
      this->win_y = y;
      this->win_w = w;
      this->win_h = h;
			this->label = label;
			this->zoom = zoom;
      this->transp = transp;
			if(track_do_save_editor(this->track))
				fts_object_set_dirty((fts_object_t *)this->track);
		}
  }	
}	

/******************************************************
*
*  client calls
*
*/
static event_t *
track_editor_create_event(int ac, const fts_atom_t *at)
{
  event_t *event = NULL;
  fts_symbol_t class_name;

  if(fts_is_symbol(at) && fts_get_symbol(at) == fts_s_colon)
  {
    ac--;
    at++;
  }

  class_name = fts_get_symbol(at);
  ac--;
  at++;

  if(class_name == fts_s_int || class_name == fts_s_float || class_name == fts_s_symbol)
    event = (event_t *)fts_object_create(event_class, 1, at + 1);
  else
  {
    fts_class_t *type = fts_get_class_by_name(class_name);

    if(type)
    {
      fts_object_t *obj = fts_object_create(type, 0, 0);

      if(obj)
      {
        fts_atom_t a;
        int i;

        for(i=0; i<ac-1; i+=2)
        {
          if(fts_is_symbol(at + i))
          {
            fts_symbol_t prop = fts_get_symbol(at + i);

            fts_send_message(obj, prop, 1, at + i + 1);
          }
        }
        fts_set_object(&a, obj);
        event = (event_t *)fts_object_create(event_class, 1, &a);
      }
    }
  }
  return event;
}

/* create new event and upload by client request */
static void
track_editor_add_event(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_editor_t *this = (track_editor_t *)o;
  double time = fts_get_float(at + 0);
  event_t *event = track_editor_create_event(ac - 1, at + 1);
	
  if(event)
    track_add_event_and_upload( this->track, time, event);
}

/* create new event by client request without uploading */
static void
track_editor_make_event(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_editor_t *this = (track_editor_t *)o;
  double time = fts_get_float(at + 0);
  event_t *event;
	
  /* make new event object */
  event = track_editor_create_event(ac - 1, at + 1);
	
  /* add event to track */
  if(event)
    track_add_event(this->track, time, event);
	
  fts_object_set_state_dirty((fts_object_t *)this->track);
}

/* delete event by client request */
static void
track_editor_remove_events(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_editor_t *this = (track_editor_t *)o;
  int i;
	
  /*  remove event objects from client */
  fts_client_send_message((fts_object_t *)this->track, seqsym_removeEvents, ac, at);
	
  for(i=0; i<ac; i++)
  {
    fts_object_t *event = fts_get_object(at + i);
    track_remove_event(this->track, (event_t *)event);
  }
	
  fts_object_set_state_dirty((fts_object_t *)this->track);
}


/* move event by client request */
static void
track_editor_move_events(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_editor_t *this = (track_editor_t *)o;
  int i;
	
  for(i=0; i<ac; i+=2)
  {
    event_t *event = (event_t *)fts_get_object(at + i);
    float time = fts_get_float(at + i + 1);
		
    track_move_event(this->track, (event_t *)event, time);
  }
	
  fts_client_send_message((fts_object_t *)this->track, seqsym_moveEvents, ac, at);
	
  fts_object_set_state_dirty((fts_object_t *)this->track);
}

/******************************************************
 *
 *  class
 *
 */
void
track_editor_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_editor_t *this = (track_editor_t *)o;
	
	this->win_x = -1;
  this->win_y = -1;
  this->win_w = -1;
  this->win_h = -1;
  this->label = fts_s_empty_string;
  this->zoom = 0.2;
	this->transp = 0;
	
  this->track = 0;
	
  if(ac > 0)
  {
    if(fts_is_object(at))
      this->track = (track_t *)fts_get_object(at);
    else
      fts_object_error(o, "bad argument");
  }
}

static void
track_editor_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(track_editor_t), track_editor_init, 0);

  fts_class_message_varargs(cl, seqsym_window, track_editor_window);
  fts_class_message_varargs(cl, seqsym_label, track_editor_label);
  fts_class_message_varargs(cl, seqsym_zoom, track_editor_zoom);
  fts_class_message_varargs(cl, seqsym_transp, track_editor_transp);
	fts_class_message_varargs(cl, seqsym_editor_state, track_editor_set_state);

  fts_class_message_varargs(cl, seqsym_addEvent, track_editor_add_event);
  fts_class_message_varargs(cl, seqsym_makeEvent, track_editor_make_event);
  fts_class_message_varargs(cl, seqsym_removeEvents, track_editor_remove_events);
  fts_class_message_varargs(cl, seqsym_moveEvents, track_editor_move_events);
}

void
track_editor_config(void)
{
  track_editor_class = fts_class_install(seqsym_track_editor, track_editor_instantiate);
}