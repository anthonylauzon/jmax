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

#include <math.h>
#include <string.h>
#include <fts/fts.h>
#include <fts/packages/utils/utils.h>
#include <fts/packages/data/data.h>

typedef struct 
{
  fts_object_t o;
  float min;
  float max;
  int size;
  int range;
  int n;
  fts_atom_t a[1024];
  double period;
  int gate;
  int pending;
  int scroll;
} vecdisplay_t;

static fts_symbol_t sym_vecdisplay = 0;
static fts_symbol_t sym_display = 0;
static fts_symbol_t sym_scroll = 0;
static fts_symbol_t sym_bounds = 0;

static void 
vecdisplay_called(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);

/************************************************************
 *
 *  send to client with time gate
 *
 */
static void
vecdisplay_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vecdisplay_t * this = (vecdisplay_t *)o;
  fts_patcher_t *patcher = fts_object_get_patcher(o);

  if(this->pending)
    {
      /* there is something to deliver */
      this->gate = 0; /* close gate for period */
      this->pending = 0; /* will be delivered */
	  
      /* send when patcher is open */
      if(patcher && fts_patcher_is_open(patcher))
	{
	  if(this->scroll)
	    fts_client_send_message(o, sym_scroll, this->n, this->a);
	  else
	    fts_client_send_message(o, sym_display, this->n, this->a);
	  
	  this->n = 0;
	  
	  fts_timebase_add_call(fts_get_timebase(), o, vecdisplay_called, 0, this->period);
	}
    }
  else
    this->gate = 1; /* open gate */
}

static void
vecdisplay_deliver(vecdisplay_t *this)
{
  this->pending = 1; /* there is something to deliver */
  
  /* if gate is open send right away */
  if(this->gate)
    fts_update_request( (fts_object_t *)this);
}

static void 
vecdisplay_called(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_update_request(o);
}

/************************************************************
 *
 *  input methods
 *
 */
static void
vecdisplay_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vecdisplay_t * this = (vecdisplay_t *)o;

  if(this->size)
    {
      float value = fts_get_number_float(at);
      float min = this->min;
      float max = this->max;
      float value_range = max - min;
      int display;

      if(value < min)
	value = min;
      else if (value > max)
	value = max;

      if(!this->scroll)
	{
	  this->n = 0;
	  this->scroll = 1;
	}

      if(this->n < this->size)
	{
	  display = (int)((this->range - 1) * (value - min) / value_range + 0.5);
	  
	  fts_set_int(this->a + this->n, display);
	  
	  this->n++;

	  vecdisplay_deliver( this);
	}
    }
}

static void 
vecdisplay_varargs(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vecdisplay_t * this = (vecdisplay_t *)o;
  int size = this->size;
  int n = ac;

  if(n > size)
    n = size;
  
  if(n)
    {
      float min = this->min;
      float max = this->max;
      float value_range = max - min;
      int i;

      if(!this->scroll)
	{
	  this->n = 0;
	  this->scroll = 1;
	}

      for(i=0; i<n; i++)
	{
	  if(fts_is_number(at + i))
	    {
	      float value = fts_get_number_float(at + i);
	      int display;
	      
	      if(value < min)
		value = min;
	      else if (value > max)
		value = max;
	      
	      display = (int)((float)(this->range - 1) * (value - min) / value_range + 0.5);

	      fts_set_int(this->a + i, display);
	    }
	  else
	    fts_set_int(this->a + i, 0);
	}

      this->n = n;
      
      vecdisplay_deliver( this);
    }
}

static void 
vecdisplay_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vecdisplay_t * this = (vecdisplay_t *)o;
  int size = this->size;
  ivec_t * vec = (ivec_t *)fts_get_object(at);
  int *ptr = ivec_get_ptr(vec);
  int n = ivec_get_size(vec);
  
  /* display vector */
  this->scroll = 0;
  
  if(n > size)
    n = size;
  
  if(n)
    {
      float min = this->min;
      float max = this->max;
      float value_range = max - min;
      int i;

      for(i=0; i<n; i++)
	{
	  float value = (float)ptr[i];
	  int display;
	  
	  if(value < min)
	    value = min;
	  else if (value > max)
	    value = max;
	  
	  display = (int)((float)(this->range - 1) * (value - min) / value_range + 0.5);
	  
	  fts_set_int(this->a + i, display);
	}

      this->n = n;
      
      vecdisplay_deliver( this);
    }
}

static void 
vecdisplay_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vecdisplay_t *this = (vecdisplay_t *)o;
  int size = this->size;
  fvec_t *vec = (fvec_t *)fts_get_object(at);
  float *ptr = fvec_get_ptr(vec);
  int n = fvec_get_size(vec);

  /* display vector */
  this->scroll = 0;
  
  if(n > size)
    n = size;
  
  if(n)
    {
      float min = this->min;
      float max = this->max;
      float value_range = max - min;
      int i;

      for(i=0; i<n; i++)
	{
	  float value = ptr[i];
	  int display;
	  
	  if(value < min)
	    value = min;
	  else if (value > max)
	    value = max;
	  
	  display = (int)((float)(this->range - 1) * (value - min) / value_range + 0.5);
	  
	  fts_set_int(this->a + i, display);
	}

      this->n = n;
      
      vecdisplay_deliver( this);
    }
}

static void 
vecdisplay_cvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vecdisplay_t * this = (vecdisplay_t *)o;
  int size = this->size;
  cvec_t *vec = (cvec_t *)fts_get_object(at);
  complex *ptr = cvec_get_ptr(vec);
  int n = cvec_get_size(vec);

  /* display vector */
  this->scroll = 0;
  
  if(n > size)
    n = size;
  
  if(n)
    {
      float min = this->min;
      float max = this->max;
      float value_range = max - min;
      int i;

      for(i=0; i<n; i++)
	{
	  float value = ptr[i].re;
	  int display;
	  
	  if(value < min)
	    value = min;
	  else if (value > max)
	    value = max;
	  
	  display = (int)((float)(this->range - 1) * (value - min) / value_range + 0.5);
	  
	  fts_set_int(this->a + i, display);
	}

      this->n = n;
      
      vecdisplay_deliver( this);
    }
}

static void 
vecdisplay_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vecdisplay_t * this = (vecdisplay_t *)o;

  this->scroll = 0;
  this->n = 0;
  
  vecdisplay_deliver( this);
}

static void
vecdisplay_set_size_by_client(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vecdisplay_t * this = (vecdisplay_t *)o;
  
  this->size = fts_get_int(at);
}

static void
vecdisplay_set_range_by_client(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vecdisplay_t * this = (vecdisplay_t *)o;
  
  this->range = fts_get_int(at);
}

static void
vecdisplay_set_bounds(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vecdisplay_t * this = (vecdisplay_t *)o;
  
  if(ac)
    {
      switch(ac)
	{
	case 2:
	  if(fts_is_number(at + 1))
	    this->max = fts_get_number_float(at + 1);
	case 1:
	  if(fts_is_number(at))
	    this->min = fts_get_number_float(at);
	  
	default:
	  break;
	}

      if(fts_object_has_id(o))
	{
	  fts_atom_t a[2];
	  
	  fts_set_float(a + 0, this->min);
	  fts_set_float(a + 1, this->max);
	  fts_client_send_message(o, sym_bounds, 2, a);

	  fts_patcher_set_dirty(fts_object_get_patcher(o), 1);
	}
    }
}

static void
vecdisplay_update_gui(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vecdisplay_t *this = (vecdisplay_t *)o;
  fts_atom_t a[2];

  fts_set_float(a + 0, this->min);
  fts_set_float(a + 1, this->max);
  fts_client_send_message(o, sym_bounds, 2, a);
}

static void 
vecdisplay_update_real_time(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vecdisplay_send( o, 0, 0, 0, 0);
}

static void 
vecdisplay_dump(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vecdisplay_t *this = (vecdisplay_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  fts_atom_t a[2];

  fts_set_float(a + 0, this->min);
  fts_set_float(a + 1, this->max);
  fts_dumper_send(dumper, sym_bounds, 2, a);
}

/************************************************************
 *
 *  class
 *
 */
static void
vecdisplay_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vecdisplay_t * this = (vecdisplay_t *)o;

  /* silent agreement with client */
  this->min = 0.0;
  this->max = 127.0;
  this->size = 128;
  this->range = 128;

  this->n = 0;
  this->period = 100.0;
  this->gate = 1;
  this->pending = 0;

  this->scroll = 0;
}

static void 
vecdisplay_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(vecdisplay_t), vecdisplay_init, 0);

  fts_class_message_varargs(cl, fts_s_dump, vecdisplay_dump);

  fts_class_message_varargs(cl, fts_s_update_gui, vecdisplay_update_gui); 
  fts_class_message_varargs(cl, fts_s_update_real_time, vecdisplay_update_real_time); 

  fts_class_message_varargs(cl, fts_s_size, vecdisplay_set_size_by_client);
  fts_class_message_varargs(cl, fts_new_symbol("range"), vecdisplay_set_range_by_client);

  fts_class_message_varargs(cl, sym_bounds, vecdisplay_set_bounds);
  fts_class_message_varargs(cl, fts_s_clear, vecdisplay_clear);
  
  fts_class_inlet_number(cl, 0, vecdisplay_number);
  fts_class_inlet_varargs(cl, 0, vecdisplay_varargs);
  fts_class_inlet(cl, 0, ivec_type, vecdisplay_ivec);
  fts_class_inlet(cl, 0, fvec_type, vecdisplay_fvec);
  fts_class_inlet(cl, 0, cvec_type, vecdisplay_cvec);
}

void 
vecdisplay_config(void)
{
  sym_vecdisplay = fts_new_symbol("vecdisplay");
  sym_display = fts_new_symbol("display");
  sym_scroll = fts_new_symbol("scroll");
  sym_bounds = fts_new_symbol("setBounds");

  fts_class_install(sym_vecdisplay, vecdisplay_instantiate);
}
