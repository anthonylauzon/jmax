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
 * Authors: Francois Dechelle, Norbert Schnell
 *
 */

#include <fts/fts.h>
#include <float.h>
#include "bpf.h"

fts_symbol_t bpf_symbol = 0;
fts_metaclass_t *bpf_type = 0;

static fts_symbol_t sym_addPoint = 0;
static fts_symbol_t sym_removePoints = 0;
static fts_symbol_t sym_setPoints = 0;

/************************************************************
 *
 *  bpf
 *
 */

#define BPF_BLOCK_SIZE 16

#define bpf_set_editor_open(b) ((b)->opened = 1)
#define bpf_set_editor_close(b) ((b)->opened = 0)
#define bpf_editor_is_open(b) ((b)->opened)

static void
set_size(bpf_t *bpf, int size)
{
  int alloc = bpf->alloc;

  if(size > alloc)
    {
      alloc += BPF_BLOCK_SIZE;

      while(alloc < size)
	alloc += BPF_BLOCK_SIZE;

      bpf->points = (bp_t *)fts_realloc(bpf->points, sizeof(bp_t) * alloc);
      bpf->alloc = alloc;
    }

  bpf->size = size;
}

void
bpf_clear(bpf_t *bpf)
{
  bpf->size = 0;
}

void
bpf_append_point(bpf_t *bpf, double time, double value)
{
  int size = bpf->size;

  set_size(bpf, size + 1);

  if(size > 0)
    {
      double duration = bpf_get_time(bpf, size - 1);
      
      if(time <= duration)
	{
	  time = duration;
	  
	  /* set jump for previous point */
	  bpf->points[size - 1].slope = DBL_MAX;
	}
      else
	{
	  /* set slope for previous point */
	  bpf->points[size - 1].slope = 
	    (value - bpf->points[size - 1].value) / 
	    (time - bpf->points[size - 1].time);
	}
    }
  
  /* set point */
  bpf->points[size].time = time;
  bpf->points[size].value = value;
  bpf->points[size].slope = 0.0;
}

void
bpf_set_point(bpf_t *bpf, int index, double time, double value)
{
  int size = bpf->size;

  if(index < size)
    {
      double dt;

      /* set slope of point before */
      if(index > 0)
	{
	  dt = time - bpf->points[index - 1].time;

	  if(dt == 0.0)
	    bpf->points[index - 1].slope = DBL_MAX;
	  else
	    bpf->points[index - 1].slope = (value - bpf->points[index - 1].value) / dt;
	}

      /* set point */
      bpf->points[index].time = time;
      bpf->points[index].value = value;
      
      if(index < size - 1)
	{
	  dt = bpf->points[index + 1].time - bpf->points[index].time;
	  
	  if(dt == 0.0)
	    bpf->points[index].slope = DBL_MAX;
	  else
	    bpf->points[index].slope = (bpf->points[index + 1].value - value) / dt;
	}
      else
	bpf->points[index].slope = 0.0;
    }
}

void
bpf_insert_point(bpf_t *bpf, double time, double value)
{
  int size = bpf->size;

  if(time > bpf->points[size - 1].time)
    bpf_append_point(bpf, time, value);
  else
    {
      int i, j;

      for(i=0; i<size; i++)
	{
	  if(bpf->points[i].time > time)
	    break;
	}
      
      set_size(bpf, size + 1);
      
      /* kick points to the back */
      for(j=size; j>i; j--)
	bpf->points[j] = bpf->points[j - 1];

      /* set point */
      bpf_set_point(bpf, i, time, value);
    }
}

void
bpf_remove_points(bpf_t *bpf, int index, int n)
{
  int size = bpf->size;

  if(index < size)
    {
      if(n > size - index)
	n = size - index;
      
      if(size <= n)
	set_size(bpf, 0);
      else
	{
	  int i;
	  
	  size -= n;
	  
	  if(index == size)
	    bpf->points[size - 1].slope = 0.0; /* erase last point */
	  else
	    {
	      double dt;

	      /* fill the hole */
	      for(i=index; i<size; i++)
		bpf->points[i] = bpf->points[i + n];

	      dt = bpf->points[index].time - bpf->points[index - 1].time;

	      /* set slope of previous point */
	      if(dt == 0.0)
		bpf->points[index - 1].slope = DBL_MAX;
	      else
		bpf->points[index - 1].slope = (bpf->points[index].value - bpf->points[index - 1].value) / dt;
	    }

	  bpf->size = size;
	}
    }
}

void
bpf_simplify(bpf_t *bpf, double time, double value)
{
  int size = bpf->size;
  int i = 0;

  while(i < size-1)
    {
      if((bpf->points[i].time == bpf->points[i + 1].time && bpf->points[i].value == bpf->points[i + 1].value) || 
	 (bpf->points[i].time == bpf->points[i + 2].time && bpf->points[i].value == bpf->points[i + 2].value) ||
	 (bpf->points[i].slope == bpf->points[i + 1].slope))
	bpf_remove_points(bpf, i + 1, 1);
      else
	i++;
    }
}

void
bpf_copy(bpf_t *org, bpf_t *copy)
{
  int size = bpf_get_size(org);
  int i;

  set_size(copy, size);

  for(i=0; i<size; i++)
    copy->points[i] = org->points[i];
}

#define BPF_CLIENT_BLOCK_SIZE 64

static void
bpf_set_client(bpf_t *bpf)
{
  fts_atom_t a[2 * BPF_CLIENT_BLOCK_SIZE];
  int size = bpf_get_size(bpf);
  int sent = 0;
  int i;

  while(size > 0)
    {
      int n = (size > BPF_CLIENT_BLOCK_SIZE)? BPF_CLIENT_BLOCK_SIZE: size;

      for(i=0; i<n; i++)
	{
	  fts_set_float(a + 2 * i, bpf_get_time(bpf, sent + i));
	  fts_set_float(a + 2 * i + 1, bpf_get_value(bpf, sent + i));
	}

      if(!sent)
	fts_client_send_message((fts_object_t *)bpf, fts_s_set, 2 * n, a);
      else
	fts_client_send_message((fts_object_t *)bpf, fts_s_append, 2 * n, a);

      sent += n;
      size -= n;
    }
}

/************************************************************
 *
 *  user methods
 *
 */

static void
bpf_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_object(o, 0, o);
}

static void
bpf_append(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bpf_t *this = (bpf_t *)o;

  if(ac)
    {
      double onset_time = bpf_get_duration(this);
      double time = 0.0;
      double value = 0.0;
      int i;
      
      if(ac & 1)
	{
	  if(fts_is_number(at))
	    value = fts_get_number_float(at);
	  
	  bpf_append_point(this, onset_time, value);
	  
	  /* skip first value */
	  ac--;
	  at++;
	}
      
      for(i=0; i<ac; i+=2)
	{
	  if(fts_is_number(at + i))
	    time = onset_time + fts_get_number_float(at + i);
	  else
	    time = onset_time;
	  
	  if(fts_is_number(at + i + 1))
	    value = fts_get_number_float(at + i + 1);
	  else
	    value = 0.0;
	  
	  bpf_append_point(this, time, value);
	}      

      this->editid++;
    }
}

static void
bpf_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bpf_t *this = (bpf_t *)o;

  bpf_clear(this);

  if(ac > 0)
    {
      double time = 0.0;
      double value = 0.0;
      int i;
      
      if(ac & 1)
	{
	  set_size(this, 1);

	  if(fts_is_number(at))
	    value = fts_get_number_float(at);
	  else
	    value = 0.0;
	  
	  this->points[0].time = 0.0;
	  this->points[0].value = value;
	  this->points[0].slope = 0.0;
	  
	  /* skip value */
	  ac--;
	  at++;
	}

      for(i=0; i<ac; i+=2)
	{
	  if(fts_is_number(at + i))
	    time = fts_get_number_float(at + i);
	  else
	    time = 0.0;
	  
	  if(fts_is_number(at + i + 1))
	    value = fts_get_number_float(at + i + 1);
	  else
	    value = 0.0;
	  
	  bpf_append_point(this, time, value);
	}
    }

  if(bpf_editor_is_open(this))
    bpf_set_client(this);

  this->editid++;
}

static void
bpf_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bpf_t *this = (bpf_t *)o;
  int i;

  post("{\n");

  for(i=0; i<this->size; i++)
    post("  (%f %f)\n", this->points[i].time, this->points[i].value);

  post("}\n");
}

static void
bpf_get_array(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bpf_t *this = (bpf_t *)o;
  int size = bpf_get_size(this);
  fts_array_t *array = (fts_array_t *)fts_get_pointer(at);
  fts_atom_t *atoms;
  int i;
  
  fts_array_set_size(array, size * 2);
  atoms = fts_array_get_atoms(array);

  for(i=0; i<size; i++)
    {
      fts_set_float(atoms + 2 * i, bpf_get_time(this, i));
      fts_set_float(atoms + 2 * i + 1, bpf_get_value(this, i));
    }
}

/************************************************************
 *
 *  client methods
 *
 */

static void
bpf_add_point_by_client_request(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bpf_t *this = (bpf_t *)o;
  int index = fts_get_int(at);
  double time = fts_get_float(at + 1);
  double value = fts_get_float(at + 2);

  bpf_insert_point(this, time, value);
  fts_client_send_message(o, sym_addPoint, ac, at);

  this->editid++;
}

static void
bpf_remove_points_by_client_request(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bpf_t *this = (bpf_t *)o;
  int index = fts_get_int(at + 0);
  int size = fts_get_int(at + 1);
  
  bpf_remove_points(this, index, size);
  fts_client_send_message(o, sym_removePoints, ac, at);

  this->editid++;
}

static void
bpf_set_points_by_client_request(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bpf_t *this = (bpf_t *)o;
  int index = fts_get_int(at);
  int n = (ac - 1) / 2;
  int i;

  for(i=0; i<n; i++)
    {
      double time = fts_get_float(at + 2 * i + 1);
      double value = fts_get_float(at + 2 * i + 2);

      bpf_set_point(this, index + i, time, value);
    }
  
  fts_client_send_message(o, sym_setPoints, ac, at);

  this->editid++;
}

static void
bpf_open_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bpf_t *this = (bpf_t *)o;

  bpf_set_editor_open(this);
  fts_client_send_message(o, fts_s_openEditor, 0, 0);

  bpf_set_client(this);
}

static void 
bpf_close_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bpf_t *this = (bpf_t *) o;

  if(bpf_editor_is_open(this))
    {
      bpf_set_editor_close(this);
      fts_client_send_message((fts_object_t *)this, fts_s_closeEditor, 0, 0);  
    }
}

static void
bpf_destroy_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bpf_t *this = (bpf_t *)o;

  bpf_set_editor_close(this);
}

/************************************************************
 *
 *  system methods
 *
 */

static void
bpf_set_from_instance(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bpf_t *this = (bpf_t *)o;
  bpf_t *in = bpf_atom_get(at);
  
  bpf_copy(in, this);

  if(bpf_editor_is_open(this))
    bpf_set_client(this);
}

static void
bpf_dump(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bpf_t *this = (bpf_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  int size = bpf_get_size(this);
  fts_message_t *mess;
  int i;
  
  /* first message will be send */
  mess = fts_dumper_message_new(dumper, fts_s_set);

  for(i=0; i<size; i++)
    {
      double time = this->points[i].time;
      double value = this->points[i].value;
      
      fts_message_append_float(mess, time);
      fts_message_append_float(mess, value);
    }
  
  fts_dumper_message_send(dumper, mess);
}

static void
bpf_set_keep(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  bpf_t *this = (bpf_t *)o;

  if(this->keep != fts_s_args && fts_is_symbol(value))
    this->keep = fts_get_symbol(value);
}

static void
bpf_get_keep(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  bpf_t *this = (bpf_t *)o;

  fts_set_symbol(value, this->keep);
}

static void
bpf_get_state(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  fts_set_object(value, o);
}

/************************************************************
 *
 *  class
 *
 */

static void
bpf_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  bpf_t *this = (bpf_t *)o;

  ac--;
  at++;

  this->points = 0;
  this->alloc = 0;
  this->size = 0;
  this->opened = 0;

  if(ac)
    {
      bpf_set(o, 0, 0, ac, at);
      this->keep = fts_s_args;
    }
  else
    {
      set_size(this, 1);
      
      this->points[0].time = 0.0;
      this->points[0].value = 0.0;

      this->keep = fts_s_no;
    }

  this->editid = 0;
}

static void
bpf_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  bpf_t *this = (bpf_t *)o;

  if(fts_object_has_id(o))
    fts_client_send_message(o, fts_s_destroyEditor, 0, 0);

  if(this->points)
    fts_free(this->points);
}

static fts_status_t
bpf_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(bpf_t), 1, 1, 0); 
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, bpf_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, bpf_delete);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set_from_instance, bpf_set_from_instance);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_dump, bpf_dump);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_get_array, bpf_get_array);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set_from_array, bpf_set);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_print, bpf_print);
  
  fts_class_add_daemon(cl, obj_property_put, fts_s_keep, bpf_set_keep);
  fts_class_add_daemon(cl, obj_property_get, fts_s_keep, bpf_get_keep);
  fts_class_add_daemon(cl, obj_property_get, fts_s_state, bpf_get_state);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set, bpf_set);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_append, bpf_append);
  
  /* graphical editor */
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_openEditor, bpf_open_editor);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_closeEditor, bpf_close_editor); 
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_destroyEditor, bpf_destroy_editor);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("add_point"), bpf_add_point_by_client_request);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("remove_points"), bpf_remove_points_by_client_request);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("set_points"), bpf_set_points_by_client_request);
  
  fts_method_define_varargs(cl, 0, fts_s_bang, bpf_output);
  
  fts_method_define_varargs(cl, 0, fts_s_set, bpf_set);
  
  return fts_Success;
}

void
bpf_config(void)
{
  bpf_symbol = fts_new_symbol("bpf");

  sym_addPoint = fts_new_symbol("addPoint");
  sym_removePoints = fts_new_symbol("removePoints");
  sym_setPoints = fts_new_symbol("setPoints");

  bpf_type = fts_class_install(bpf_symbol, bpf_instantiate);
}
