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

#include "fts.h"
#include "bpf.h"

fts_symbol_t bpf_symbol = 0;
fts_type_t bpf_type = 0;
fts_class_t *bpf_class = 0;

static fts_symbol_t sym_openEditor = 0;
static fts_symbol_t sym_set = 0;
static fts_symbol_t sym_append = 0;
static fts_symbol_t sym_addPoint = 0;
static fts_symbol_t sym_removePoints = 0;
static fts_symbol_t sym_setPoint = 0;

/************************************************************
 *
 *  bpf
 *
 */

#define BPF_BLOCK_SIZE 16

#define bpf_set_editor_open(b) ((b)->opened = 1)
#define bpf_set_editor_close(b) ((b)->opened = 0)
#define bpf_editor_is_open(b) ((b)->opened)

void
bpf_set_size(bpf_t *bpf, int size)
{
  int alloc = bpf->alloc;
  int i;

  if(size > alloc)
    {
      while(alloc < size)
	alloc += BPF_BLOCK_SIZE;

      if(bpf->alloc)
	fts_block_free((void *)bpf->points, sizeof(bp_t) * alloc);
      
      bpf->points = (bp_t *)fts_block_alloc(sizeof(bp_t) * alloc);

      bpf->alloc = alloc;
    }

  bpf->size = size;
}

static void
bpf_append(bpf_t *bpf, double time, double value)
{
  int size = bpf->size;
  
  bpf_set_size(bpf, size + 1);
  bpf_set_point(bpf, size, time, value);  
}

static void
bpf_insert_by_index(bpf_t *bpf, int index, double time, double value)
{
  int size = bpf->size;
  int i;

  if(index >= size)
    bpf_append(bpf, time, value);
  else
    {
      size++;
      
      bpf_set_size(bpf, size);
      
      /* kick points to the back */
      for(i=size-2; i>=index; i--)
	bpf->points[i + 1] = bpf->points[i];
      
      bpf_set_point(bpf, index, time, value);
    }
}

static void
bpf_insert_by_time(bpf_t *bpf, double time, double value)
{
  int size = bpf->size;
  int i;

  if(time > bpf->points[size - 1].time)
    bpf_append(bpf, time, value);    
  else
    {
      for(i=0; i<size; i++)
	if(bpf->points[i].time > time)
	  break;
      
      bpf_insert_by_index(bpf, i, time, value);
    }
}

static void
bpf_remove(bpf_t *bpf, int index)
{
  int size = bpf->size;
  int i;
  
  size--;
  
  if(index >= size)
    bpf_set_size(bpf, size);
  else
    {
      for(i=index; i<=size; i++)
	bpf->points[i + 1] = bpf->points[i];
      
      bpf_set_size(bpf, size);
    }
}

#define BPF_CLIENT_BLOCK_SIZE 64

static void
bpf_set_client(bpf_t *bpf)
{
  fts_atom_t a[2 * BPF_CLIENT_BLOCK_SIZE];
  int n = bpf_get_size(bpf);
  int append = 0;
  int i;

  while(n > 0)
    {
      int send = (n > BPF_CLIENT_BLOCK_SIZE)? BPF_CLIENT_BLOCK_SIZE: n;

      for(i=0; i<n; i++)
	{
	  fts_set_float(a + 2 * i, bpf_get_time(bpf, i));
	  fts_set_float(a + 2 * i + 1, bpf_get_value(bpf, i));
	}

      if(!append)
	{
	  fts_client_send_message((fts_object_t *)bpf, sym_set, 2 * send, a);
	  append = 1;
	}
      else
	fts_client_send_message((fts_object_t *)bpf, sym_append, 2 * send, a);
	
      n -= send;
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
  bpf_t *this = (bpf_t *)o;
  fts_atom_t a[1];

  bpf_atom_set(a, this);
  fts_outlet_send(o, 0, bpf_symbol, 1, a);
}

static void
bpf_set_absolute(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bpf_t *this = (bpf_t *)o;
  int odd = ac & 1;
  double time, value;
  int i;
  
  bpf_set_size(this, (ac + odd) / 2);

  if(odd)
    {
      double value;
      
      if(fts_is_number(at))
	value = fts_get_number_float(at);
      else
	value = 0.0;
      
      bpf_set_point(this, 0, 0.0, value);
    }
  
  for(i=odd; i<ac; i+=2)
    {
      double running_time = 0.0;
      double time;
      double value;
      
      if(fts_is_number(at + i))
	{
	  time = fts_get_number_float(at + i);

	  if(time < running_time)
	    time = running_time;
	}
      else
	time = running_time;
      
      if(fts_is_number(at + i + 1))
	value = fts_get_number_float(at + i + 1);
      else
	value = 0.0;
      
      bpf_set_point(this, (i + odd) / 2, time, value);

      running_time = time;
    }

  if(bpf_editor_is_open(this))
    bpf_set_client(this);
}

static void
bpf_set_relative(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bpf_t *this = (bpf_t *)o;
  int odd = ac & 1;
  double value;
  int i;
  
  bpf_set_size(this, (ac + odd) / 2);

  if(odd)
    {
      double value;
      
      if(fts_is_number(at))
	value = fts_get_number_float(at);
      else
	value = 0.0;
      
      bpf_set_point(this, 0, 0.0, value);
    }
  
  for(i=odd; i<ac; i+=2)
    {
      double running_time = 0.0;
      double value;
      
      if(fts_is_number(at + i))
	{
	  double time = fts_get_number_float(at + i);

	  if(time > 0.0)
	    running_time += time;
	}

      if(fts_is_number(at + i + 1))
	value = fts_get_number_float(at + i + 1);
      else
	value = 0.0;

      bpf_set_point(this, (i + odd) / 2, running_time, value);
    }

  if(bpf_editor_is_open(this))
    bpf_set_client(this);
}

static void
bpf_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bpf_t *this = (bpf_t *)o;
  int size = bpf_get_size(this);
  int i;

  post("{");

  if(size > 8)
    {
      int size8 = (size / 8) * 8;
      int i, j;

      for(i=0; i<size8; i+=8)
	{
	  /* print one line of 8 with indent */
	  post("\n  ");
	  for(j=0; j<8; j++)
	    post("(%f %f) ", bpf_get_time(this, i + j), bpf_get_value(this, i + j));
	}
	  
      /* print last line with indent */
      if(i < size)
	{
	  post("\n  ");
	  for(; i<size; i++)
	    post("(%f %f) ", bpf_get_time(this, i), bpf_get_value(this, i));
	}

      post("\n}");
    }
  else if(size)
    {
      for(i=0; i<size-1; i++)
	post("(%f %f) ", bpf_get_time(this, i), bpf_get_value(this, i));

      post("(%f %f)}", bpf_get_time(this, size - 1), bpf_get_value(this, size - 1));
    }
  else
    post("}");
}

/************************************************************
 *
 *  client methods
 *
 */

void
bpf_add_point_by_client_request(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bpf_t *this = (bpf_t *)o;
  int index = fts_get_int(at);
  double time = fts_get_float(at + 1);
  double value = fts_get_float(at + 2);

  bpf_insert_by_index(this, index, time, value);

  fts_client_send_message(o, sym_addPoint, ac, at);
}

void
bpf_remove_points_by_client_request(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bpf_t *this = (bpf_t *)o;
  int i;

  /* indices coming in decreasing order */
  for(i=0; i<ac; i++)
    {
      int index = fts_get_int(at + i);

      bpf_remove(this, index);
    }

  /*  remove event objects from client */
  fts_client_send_message(o, sym_removePoints, ac, at);
}

void
bpf_set_point_by_client_request(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bpf_t *this = (bpf_t *)o;
  int index = fts_get_int(at);
  double time = fts_get_float(at + 1);
  double value = fts_get_float(at + 2);

  bpf_set_point(this, index, time, value);

  fts_client_send_message(o, sym_setPoint, ac, at);
}

static void
bpf_open_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bpf_t *this = (bpf_t *)o;

  bpf_set_editor_open(this);
  fts_client_send_message(o, sym_openEditor, 0, 0);

  bpf_set_client(this);
}

static void
bpf_close_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
bpf_get_state(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  bpf_t *this = (bpf_t *)obj;

  bpf_atom_set(value, this);
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

  this->opened = 0;
  this->points = 0;
  this->alloc = 0;
  this->size = 0;

  bpf_set_relative(o, 0, 0, ac - 1, at + 1);
}

static void
bpf_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  bpf_t *this = (bpf_t *)o;

  bpf_clear(this);
}

static int
bpf_check(int ac, const fts_atom_t *at)
{
  int i;

  for(i=1; i<ac; i++)
    {
      if(!fts_is_number(at + i))
	return 0;
    }
  
  return 1;
}

static fts_status_t
bpf_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int i;

  if(bpf_check(ac - 1, at + 1))
    {
      fts_class_init(cl, sizeof(bpf_t), 1, 1, 0); 
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, bpf_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, bpf_delete);

      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_print, bpf_print);
      
      /* graphical editor */
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("open_editor"), bpf_open_editor);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("close_editor"), bpf_close_editor);
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("add_point"), bpf_add_point_by_client_request);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("remove_points"), bpf_remove_points_by_client_request);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("set_point"), bpf_set_point_by_client_request);

      
      fts_method_define_varargs(cl, 0, fts_s_bang, bpf_output);

      fts_method_define_varargs(cl, 0, fts_s_set, bpf_set_absolute);
      fts_method_define_varargs(cl, 0, fts_new_symbol("absolute"), bpf_set_absolute);
      fts_method_define_varargs(cl, 0, fts_new_symbol("relative"), bpf_set_relative);
      
      return fts_Success;
    }
  else
    return &fts_CannotInstantiate;
}

static int
bpf_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  return bpf_check(ac1 - 1, at1 + 1);
}

void
bpf_config(void)
{
  bpf_symbol = fts_new_symbol("bpf");
  bpf_type = bpf_symbol;

  sym_openEditor = fts_new_symbol("openEditor");
  sym_set = fts_new_symbol("set");
  sym_append = fts_new_symbol("append");
  sym_addPoint = fts_new_symbol("addPoint");
  sym_removePoints = fts_new_symbol("removePoints");
  sym_setPoint = fts_new_symbol("setPoint");

  fts_metaclass_install(bpf_symbol, bpf_instantiate, bpf_equiv);
  bpf_class = fts_class_get_by_name(bpf_symbol);

  fts_atom_type_register(bpf_symbol, bpf_class);
}














