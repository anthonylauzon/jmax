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
#include "bpf.h"

fts_symbol_t bpf_symbol = 0;
fts_type_t bpf_type = 0;
fts_class_t *bpf_class = 0;

static fts_symbol_t sym_openEditor = 0;
static fts_symbol_t sym_destroyEditor = 0;
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

static void
set_size(bpf_t *bpf, int size)
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
bpf_remove_point(bpf_t *bpf, int index)
{
  int size = bpf->size;
  int i;
  
  size--;
  
  for(i=index; i<size; i++)
    bpf->points[i] = bpf->points[i + 1];


  bpf->size = size;
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

int
bpf_get_index(bpf_t *bpf, double time)
{
  bp_t *points = bpf->points;
  int size = bpf_get_size(bpf);
  int index = size / 2;

  if(time >= points[index].time)
    {
      if(time >= points[size - 2].time)
	return size - 2;
      else
	{
	  while(time >= points[index + 1].time)
	    index++;
	}
    }
  else
    {
      if(time <= points[0].time)
	return 0;
      else
	{
	  while(time < points[index].time)
	    index--;
	}
    }
 
  return index;
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
bpf_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bpf_t *this = (bpf_t *)o;

  set_size(this, 1);
  
  this->points[0].time = 0.0;
  this->points[0].value = 0.0;
}

static void
bpf_append(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bpf_t *this = (bpf_t *)o;
  int size = this->size;
  double last_time = this->points[size - 1].time;
  double last_value = this->points[size - 1].value;
  double onset_time = last_time;
  int index = size;
  int jump = 0;
  int i;

  set_size(this, size + ac / 2);
  
  /* detect jump value */
  if(size > 1 && this->points[size - 2].time == last_time)
    jump = 1;

  for(i=0; i<ac; i+=2)
    {
      double time;
      double value;
      
      if(fts_is_number(at + i))
	time = onset_time + fts_get_number_float(at + i);
      else
	time = last_time;
      
      if(fts_is_number(at + i + 1))
	value = fts_get_number_float(at + i + 1);
      else
	value = 0.0;
      
      if(time <= last_time)
	{
	  if(jump)
	    this->points[index - 1].value = value; /* replace jump value */
	  else
	    {
	      /* append new (jump) value */
	      this->points[index].time = last_time;
	      this->points[index].value = value;
	      index++;

	      /* set jump */
	      jump = 1;
	    }
	}	    
      else
	{
	  /* append new value */
	  this->points[index].time = time;
	  this->points[index].value = value;
	  index++;
  
	  /* no jump */
	  jump = 0;

	  last_time = time;
	}      
    }
  
  set_size(this, size + index);  
}

static void
bpf_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bpf_t *this = (bpf_t *)o;
  int odd = ac & 1;
  double time, value;
  int i;
  
  /* clear */
  set_size(this, 1);
  
  if(odd || (ac && fts_is_number(at) && fts_get_number_float(at) == 0.0))
    {
      double value;
      
      if(!odd)
	{
	  /* skip time 0.0 */
	  ac--;
	  at++;
	}

      if(fts_is_number(at))
	value = fts_get_number_float(at);
      else
	value = 0.0;
      
      this->points[0].time = 0.0;
      this->points[0].value = value;

      /* skip first value */
      ac--;
      at++;
    }
  else
    {
      this->points[0].time = 0.0;
      this->points[0].value = 0.0;
    }

  if(ac)
    bpf_append(o, 0, 0, ac, at);

  if(bpf_editor_is_open(this))
    bpf_set_client(this);
}

static void
bpf_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bpf_t *this = (bpf_t *)o;
  int size = bpf_get_size(this);
  int i;

  post("{\n");

  for(i=0; i<size; i++)
    post("  (%f %f)\n", this->points[i].time, this->points[i].value);

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
  int size = this->size;
  int i;

  if(index == 0)
    index = 1;

  /* check for double value and jump */
  if(time <= this->points[index - 1].time || (index < size - 1 && time >= this->points[index + 1].time))
    {
      time = this->points[index - 1].time;
      
      if(value == this->points[index - 1].value || (index < size - 1 && value == this->points[index + 1].value))
	{
	  /* don't add double point */
	  return;
	}
      else if(index > 1 && time == this->points[index - 2].time)
	{
	  fts_atom_t a[3];

	  /* set previous jump point */
	  this->points[index - 1].value = value;
	  
	  fts_set_int(a, index - 1);
	  fts_set_float(a + 1, time);
	  fts_set_float(a + 2, value);	      
	  fts_client_send_message(o, sym_setPoint, 3, a);

	  return;
	}
    }

  /* insert point */
  set_size(this, size + 1);
  
  /* kick points to the back */
  for(i=size-1; i>index; i--)
    this->points[i] = this->points[i - 1];

  /* set new point */
  this->points[index].time = time;
  this->points[index].value = value;  
  
  fts_client_send_message(o, sym_addPoint, ac, at);
}

void
bpf_remove_points_by_client_request(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bpf_t *this = (bpf_t *)o;
  int size = this->size;
  int i;

  /* indices coming in decreasing order !! */
  for(i=0; i<ac; i++)
    {
      int index = fts_get_int(at + i);

      if(index != 0)
	{
	  int j;
	  
	  size--;
	  
	  for(j=index; j<size; j++)
	    this->points[j] = this->points[j + 1];
	}
    }

  this->size = size;

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
  fts_atom_t a[3];

  if(index == 0)
    {
      /* set value of point 0.0 */
      this->points[0].value = value;

      fts_set_int(a, 0);
      fts_set_float(a + 1, 0.0);
      fts_set_float(a + 2, value);

      fts_client_send_message(o, sym_setPoint, 3, a);
    }
  else
    {
      int size = this->size;

      if(time <= this->points[index - 1].time)
	{
	  time = this->points[index - 1].time;

	  if(value == this->points[index - 1].value)
	    {
	      /* delete double point */
	      bpf_remove_point(this, index);

	      fts_set_int(a, index);
	      fts_client_send_message(o, sym_removePoints, 1, a);
	    }
	  else if(index > 1 && time == this->points[index - 2].time)
	    {
	      /*  remove double jump point */
	      bpf_remove_point(this, index);

	      fts_set_int(a, index);
	      fts_client_send_message(o, sym_removePoints, 1, a);

	      /* set previous jump point */
	      this->points[index - 1].value = value;

	      fts_set_int(a, index - 1);
	      fts_set_float(a + 1, time);
	      fts_set_float(a + 2, value);	      
	      fts_client_send_message(o, sym_setPoint, 3, a);
	    }
	}
      else if (index < size - 1 && time >= this->points[index + 1].time)
	{
	  time = this->points[index + 1].time;

	  if(value == this->points[index + 1].value)
	    {
	      /* delete double point */
	      bpf_remove_point(this, index);
	      
	      fts_set_int(a, index);
	      fts_client_send_message(o, sym_removePoints, 1, a);
	    }
	  else if(index < size - 2 && time == this->points[index + 2].time)
	    {
	      /*  remove double jump point */
	      bpf_remove_point(this, index);

	      fts_set_int(a, index);
	      fts_client_send_message(o, sym_removePoints, 1, a);

	      /* set next jump point */
	      this->points[index + 1].value = value;

	      fts_set_int(a, index + 1);
	      fts_set_float(a + 1, time);
	      fts_set_float(a + 2, value);	      
	      fts_client_send_message(o, sym_setPoint, 3, a);	      
	    }
	}
      else
	{
	  this->points[index].time = time;
	  this->points[index].value = value;

	  fts_client_send_message(o, sym_setPoint, 3, at);
	}	
    }
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
bpf_save_bmax(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bpf_t *this = (bpf_t *)o;
  double append_time = 0.0;
  int append = 0;

  if(this->persistent)
    {
      fts_bmax_file_t *f = (fts_bmax_file_t *)fts_get_ptr(at);      
      int size = bpf_get_size(this);
      fts_atom_t av[256];
      int ac = 0;
      int i;
      
      for(i=0; i<size; i++)
	{
	  double time = this->points[i].time;

	  fts_set_float(av + ac, time - append_time);
	  fts_set_float(av + ac + 1, this->points[i].value);

	  ac += 2;
	  
	  if(ac == 256)
	    {
	      if(!append)
		{
		  fts_bmax_save_message(f, fts_s_set, ac, av);
		  append = 1;
		}
	      else
		fts_bmax_save_message(f, fts_s_append, ac, av);

	      append_time = time;
	      ac = 0;
	    }
	}
      
      if(ac > 1) 
	{
	  if(!append)
	    fts_bmax_save_message(f, fts_s_set, ac, av);
	  else
	    fts_bmax_save_message(f, fts_s_append, ac, av);
	}
    }
}

static void
bpf_assist(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_symbol_t cmd = fts_get_symbol_arg(ac, at, 0, 0);

  if (cmd == fts_s_object)
    fts_object_blip(o, "break point function");
  else if (cmd == fts_s_inlet)
    {
      int n = fts_get_int_arg(ac, at, 1, 0);

      switch(n)
	{
	case 0:
	  /* fts_object_blip(o, "no comment"); */
	  break;
	}
    }
}

static void
bpf_set_persistent(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  bpf_t *this = (bpf_t *)obj;

  if(fts_is_symbol(value))
    {
      fts_symbol_t s = fts_get_symbol(value);

      if(s == fts_s_yes)
	this->persistent = 1;
      else
	this->persistent = 0;	
    }
}

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

  ac--;
  at++;

  this->opened = 0;
  this->points = 0;
  this->alloc = 0;
  this->size = 0;
  this->persistent = 0;

  bpf_set(o, 0, 0, ac, at);
}

static void
bpf_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  bpf_t *this = (bpf_t *)o;

  if(fts_object_has_id(o))
    fts_client_send_message(o, sym_destroyEditor, 0, 0);

  if(this->points)
    fts_free(this->points);
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

      fts_class_add_daemon(cl, obj_property_put, fts_new_symbol("keep"), bpf_set_persistent);
      fts_class_add_daemon(cl, obj_property_get, fts_s_state, bpf_get_state);

      /* save and restore bmax file */
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_save_bmax, bpf_save_bmax); 
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set, bpf_set);

      /* graphical editor */
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("open_editor"), bpf_open_editor);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("close_editor"), bpf_close_editor);
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("add_point"), bpf_add_point_by_client_request);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("remove_points"), bpf_remove_points_by_client_request);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("set_point"), bpf_set_point_by_client_request);

      fts_method_define_varargs(cl, 0, fts_s_bang, bpf_output);

      fts_method_define_varargs(cl, 0, fts_s_set, bpf_set);
      fts_method_define_varargs(cl, 0, fts_new_symbol("absolute"), bpf_set);
      
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
  sym_destroyEditor = fts_new_symbol("destroyEditor");
  sym_set = fts_new_symbol("set");
  sym_append = fts_new_symbol("append");
  sym_addPoint = fts_new_symbol("addPoint");
  sym_removePoints = fts_new_symbol("removePoints");
  sym_setPoint = fts_new_symbol("setPoint");

  fts_metaclass_install(bpf_symbol, bpf_instantiate, bpf_equiv);
  bpf_class = fts_class_get_by_name(bpf_symbol);

  fts_atom_type_register(bpf_symbol, bpf_class);
}
