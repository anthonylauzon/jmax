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

#include <alloca.h>
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
		fts_atom_t *atoms = fts_array_get_atoms(&this->columns);
		int size = fts_array_get_size(&this->columns);

		if(this->win_x!=-1 && this->win_y!=-1 && this->win_w!=-1 && this->win_h!=-1)
		{
			fts_atom_t a[9];
			fts_set_int(a, this->win_x);
			fts_set_int(a+1, this->win_y);
			fts_set_int(a+2, this->win_w);
			fts_set_int(a+3, this->win_h);
			fts_set_symbol(a+4, this->label);
			fts_set_float(a+5, this->zoom);
			fts_set_int(a+6, this->transp);
			fts_set_int(a+7, this->view);
			fts_set_int(a+8, this->range_mode);

			fts_client_send_message((fts_object_t *)this, seqsym_editor, 9, a);
		}
		
		if(this->tab_w!=-1 && this->tab_h!=-1)
		{
			fts_atom_t b[2];
			fts_set_int(b, this->tab_w);
			fts_set_int(b+1, this->tab_h);
			fts_client_send_message((fts_object_t *)this, seqsym_table_size, 2, b);
		}
		
		if(size > 0)
			fts_client_send_message((fts_object_t *)this, seqsym_columns, size, atoms);
	}
}

void 
track_editor_dump_gui(track_editor_t *this, fts_dumper_t *dumper)
{
  fts_atom_t a[5];

  /* ones for all */
  fts_set_symbol(a, seqsym_window);
  fts_set_int(a + 1, this->win_x);
  fts_set_int(a + 2, this->win_y);
  fts_set_int(a + 3, this->win_w);
  fts_set_int(a + 4, this->win_h);
  fts_dumper_send(dumper, seqsym_editor, 5, a);

  fts_set_symbol(a, seqsym_label);
  fts_set_symbol(a + 1, this->label);
  fts_dumper_send(dumper, seqsym_editor, 2, a);

  fts_set_symbol(a, seqsym_zoom);
  fts_set_float(a + 1, this->zoom);
  fts_dumper_send(dumper, seqsym_editor, 2, a);

  fts_set_symbol(a, seqsym_transp);
  fts_set_int(a + 1, this->transp);
  fts_dumper_send(dumper, seqsym_editor, 2, a);
	
  fts_set_symbol(a, seqsym_view);
	fts_set_int(a + 1, this->view);
  fts_dumper_send(dumper, seqsym_editor, 2, a);
	
  fts_set_symbol(a, seqsym_range_mode);
	fts_set_int(a + 1, this->range_mode);
  fts_dumper_send(dumper, seqsym_editor, 2, a);
		
	if(this->tab_w != -1 && this->tab_h != -1)
	{
    fts_set_symbol(a, seqsym_table_size);
		fts_set_int(a + 1, this->tab_w);
		fts_set_int(a + 2, this->tab_h);
		fts_dumper_send(dumper, seqsym_editor, 3, a);
	}
  
	if(fts_array_get_size(&this->columns) > 0)
  {
    fts_atom_t *cols = fts_array_get_atoms(&this->columns);
    int n_cols = fts_array_get_size(&this->columns);
    fts_atom_t *b = alloca(sizeof(fts_atom_t) * (n_cols + 1));
    int i;
    
    fts_set_symbol(b, seqsym_columns);

    for(i=0; i<n_cols; i++)
      b[i + 1] = cols[i];
      
		fts_dumper_send(dumper, seqsym_editor, n_cols + 1, b);
  }
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
track_editor_view(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_editor_t *this = (track_editor_t *)o;
  if(ac == 1 && fts_is_int(at))
  {
    int view = fts_get_int(at);
    if(this->view != view)
    {
      this->view = view;
      if(track_do_save_editor(this->track))
        fts_object_set_dirty((fts_object_t *)this->track);
    }
  }	
}	
static void
track_editor_range_mode(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_editor_t *this = (track_editor_t *)o;
  if(ac == 1 && fts_is_int(at))
  {
    int range_mode = fts_get_int(at);
    if(this->range_mode != range_mode)
    {
      this->range_mode = range_mode;
      if(track_do_save_editor(this->track))
        fts_object_set_dirty((fts_object_t *)this->track);
    }
  }	
}	
static void
track_editor_set_table_columns(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_editor_t *this = (track_editor_t *)o;
	if(ac > 1 && fts_is_symbol(at))
	{
		fts_array_set(&this->columns, ac, at);

		if(track_do_save_editor(this->track))
			fts_object_set_dirty((fts_object_t *)this->track);
  }	
}	

static void
track_editor_set_table_size(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_editor_t *this = (track_editor_t *)o;
	if(ac == 2 && fts_is_int(at))
	{
		int tab_w = fts_get_int(at);
		int tab_h =  fts_get_int(at+1);
    if(this->tab_w != tab_w || this->tab_h != tab_h)
    {
      this->tab_w = tab_w;
			this->tab_h = tab_h;
      if(track_do_save_editor(this->track))
        fts_object_set_dirty((fts_object_t *)this->track);
    }
  }	
}	

static void
track_editor_set_state(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_editor_t *this = (track_editor_t *)o;
  if(ac >= 9)
  {
		int x = fts_get_int(at);
    int y = fts_get_int(at+1);
    int w = fts_get_int(at+2);
    int h = fts_get_int(at+3);
		fts_symbol_t label = fts_get_symbol(at+4);
		float zoom = fts_get_float(at+5);
    int transp = fts_get_int(at+6);
		int view = fts_get_int(at+7);
		int range_mode = fts_get_int(at+8);
		if(this->win_x!=x || this->win_y != y || 
			 this->win_w!=w || this->win_h != h || 
			 this->label != label || this->zoom != zoom || 
			 this->transp != transp || this->view != view || this->range_mode != range_mode)
    {
      this->win_x = x;
      this->win_y = y;
      this->win_w = w;
      this->win_h = h;
			this->label = label;
			this->zoom = zoom;
      this->transp = transp;
			this->view = view;
			this->range_mode = range_mode;
			if(track_do_save_editor(this->track))
				fts_object_set_dirty((fts_object_t *)this->track);
		}
  }	
}

/******************************************************
 *
 *  class
 *
 */
static void
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
	this->view = 0;
	this->range_mode = 0;
	
	this->tab_w = -1;
	this->tab_h = -1;
	
	fts_array_init(&this->columns, 0, NULL);
	
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
track_editor_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  track_editor_t *this = (track_editor_t *)o;
	
	fts_array_destroy(&this->columns);
}

static void
track_editor_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(track_editor_t), track_editor_init, track_editor_delete);

  fts_class_message_varargs(cl, seqsym_window, track_editor_window);
  fts_class_message_varargs(cl, seqsym_label, track_editor_label);
  fts_class_message_varargs(cl, seqsym_zoom, track_editor_zoom);
  fts_class_message_varargs(cl, seqsym_transp, track_editor_transp);
	fts_class_message_varargs(cl, seqsym_view, track_editor_view);
	fts_class_message_varargs(cl, seqsym_range_mode, track_editor_range_mode);
	fts_class_message_varargs(cl, seqsym_columns, track_editor_set_table_columns);
	fts_class_message_varargs(cl, seqsym_table_size, track_editor_set_table_size);
	fts_class_message_varargs(cl, seqsym_editor_state, track_editor_set_state);
}

void
track_editor_config(void)
{
  track_editor_class = fts_class_install(seqsym_track_editor, track_editor_instantiate);
}
