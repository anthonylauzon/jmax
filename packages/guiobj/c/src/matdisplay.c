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
 * Authors: Norbert Schnell and Remy Muller
 */

#include <math.h>
#include <string.h>
#include <fts/fts.h>
#include <data/c/include/ivec.h>
#include <data/c/include/fvec.h>
#include <data/c/include/cvec.h>
#include <data/c/include/fmat.h>

typedef struct 
{
  fts_object_t o;
  float min;
  float max;
  int m_size;
  int n_size; 
  int m_wind;
  int n_wind;
  int min_color;
  int max_color;
  int m_zoom;
  int n_zoom;
  fts_array_t a;
  double period;
  int gate;
  int pending;
  int scroll;
} matdisplay_t;

static fts_symbol_t sym_matdisplay = 0;
static fts_symbol_t sym_display = 0;
static fts_symbol_t sym_scroll = 0;
static fts_symbol_t sym_range = 0;
static fts_symbol_t sym_zoom = 0;
static fts_symbol_t sym_window = 0;
static fts_symbol_t sym_color = 0;
static fts_symbol_t sym_size = 0;

static void 
matdisplay_called(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);

/************************************************************
 *
 *  send to client with time gate
 *
 */
static void
matdisplay_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  matdisplay_t * this = (matdisplay_t *)o;
  fts_patcher_t *patcher = fts_object_get_patcher(o);

  if(this->pending)
    {
      /* there is something to deliver */
      this->gate = 0; /* close gate for period */
      this->pending = 0; /* will be delivered */
	  
      /* send when patcher is open */
      if(patcher && fts_patcher_is_open(patcher))
	{ 
	  fts_atom_t a[2];
	  
	  /* send matrix dimensions */
	  fts_set_int(a + 0, this->m_size);
	  fts_set_int(a + 1, this->n_size);
	  fts_client_send_message(o, sym_size, 2, a);
	  
	  if(this->scroll)
	    fts_client_send_message(o, sym_scroll, fts_array_get_size(&this->a), fts_array_get_atoms(&this->a));
	  else
	    fts_client_send_message(o, sym_display, fts_array_get_size(&this->a), fts_array_get_atoms(&this->a));
	 
	  fts_array_set_size(&this->a, 0);
	  fts_timebase_add_call(fts_get_timebase(), o, /*matdisplay_send*/matdisplay_called, 0, this->period);
	}
    }
  else
    this->gate = 1; /* open gate */
}

static void
matdisplay_deliver(matdisplay_t *this)
{
  this->pending = 1; /* there is something to deliver */
  
  /* if gate is open send right away */
  if(this->gate)
    /*matdisplay_send((fts_object_t *)this, 0, 0, 0, 0);*/
    fts_update_request( (fts_object_t *)this);
}

static void 
matdisplay_called(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_update_request(o);
}

/************************************************************
 *
 *  input methods
 *
 */
static void
matdisplay_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  /* todo:  a column per number, intensity is function of (number - min)/range */
}

static void 
matdisplay_varargs(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  /* todo: same as matdisplay_fvec */
}

static void 
matdisplay_cvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  matdisplay_t * this = (matdisplay_t *)o;
  cvec_t * vec = (cvec_t *)fts_get_object(at);
  float min = this->min;
  float max = this->max;
  float range = max - min; 
  int m = cvec_get_size(vec);
  int n = this->n_wind / this->n_zoom + 1;

  /* to send only the useful data */
  
  if(m * this->m_zoom > this->m_wind)
    m = this->m_wind / this->m_zoom;

  if(m > 0 && range != 0.0)
    {
      complex *c = cvec_get_ptr(vec);
      fts_atom_t *atoms;
      int i;
      
      this->m_size = m;
      this->n_size = n;           

      fts_array_set_size(&this->a, m);
      atoms = fts_array_get_atoms(&this->a);

      for(i=0; i<m;i++)
	{
	  float value = c[i].re;
	  int val;
	  
	  if(value < min)
	    value = min;	      
	  else if(value > max)
	    value = max;
	  
	  val = (int)((255) * ((value - min) / range));
	  
	  fts_set_int(atoms + i, val); 
	}
      this->scroll = 1; /* mode scroll = true */
      matdisplay_deliver(this);
    }
}

static void 
matdisplay_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  matdisplay_t * this = (matdisplay_t *)o;
  ivec_t * vec = (ivec_t *)fts_get_object(at);
  int min = (int)this->min;
  int max = (int)this->max;
  int range = max - min; 
  int m = ivec_get_size(vec);
  int n = this->n_wind / this->n_zoom + 1;

  /* to send only the useful data */
  
  if(m * this->m_zoom > this->m_wind)
    m = this->m_wind / this->m_zoom;

  if(m > 0 && range != 0)
    {
      int *p = ivec_get_ptr(vec);
      fts_atom_t *atoms;
      int i;
      
      this->m_size = m;
      this->n_size = n;           

      fts_array_set_size(&this->a, m);
      atoms = fts_array_get_atoms(&this->a);

      for(i=0; i<m;i++)
	{
	  int value = p[i];
	  int val;
	  
	  if(value < min)
	    value = min;	      
	  else if(value > max)
	    value = max;
	  
	  val = (int)((255) * ((value - min) / range));
	  
	  fts_set_int(atoms + i, val); 
	}
      this->scroll = 1; /* mode scroll = true */
      matdisplay_deliver(this);
    }
}

static void 
matdisplay_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  matdisplay_t * this = (matdisplay_t *)o;
  fvec_t *vec = (fmat_t *)fts_get_object(at);
  float min = this->min;
  float max = this->max;
  float range = max - min; 
  int m = fvec_get_size(vec);
  int n = this->n_wind / this->n_zoom + 1;

  /* to send only the useful data */
  
  if(m * this->m_zoom > this->m_wind)
    m = this->m_wind / this->m_zoom;

  if(m > 0 && range != 0.0)
    {
      float *f = fvec_get_ptr(vec);
      fts_atom_t *atoms;
      int i;
      
      this->m_size = m;
      this->n_size = n;           

      fts_array_set_size(&this->a, m);
      atoms = fts_array_get_atoms(&this->a);

      for(i=0; i<m;i++)
	{
	  float value = f[i];
	  int val;
	  
	  if(value < min)
	    value = min;	      
	  else if(value > max)
	    value = max;
	  
	  val = (int)((255) * ((value - min) / range));
	  
	  fts_set_int(atoms + i, val); 
	}
      this->scroll = 1; /* mode scroll = true */
      matdisplay_deliver(this);
    }
}

static void 
matdisplay_fmat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  matdisplay_t *this = (matdisplay_t *)o;
  fmat_t *mat = (fmat_t *)fts_get_object(at);
  float min = this->min;
  float max = this->max;
  float range = max - min; 
  int m = fmat_get_m(mat);
  int n = fmat_get_n(mat);
  int width = n;
  int size;

  /* to send only the useful data */
  if(n * this->n_zoom > this->n_wind)
    n = this->n_wind / this->n_zoom;

  if(m * this->m_zoom > this->m_wind)
    m = this->m_wind / this->m_zoom;

  size = m * n;
  
  if(size > 0 && range != 0.0)
    {
      float *f = fmat_get_ptr(mat);
      fts_atom_t *atoms;
      int i, j;
      
      this->m_size = m;
      this->n_size = n;
      
      fts_array_set_size(&this->a, size);
      atoms = fts_array_get_atoms(&this->a);

      for(i=0; i<m;i++)
	{
	  for(j=0; j<n; j++)
	    {
	      float value = f[i * width + j];
	      int val;
	      
	      if(value < min)
		value = min;	      
	      else if(value > max)
		value = max;
	      
	      val = (int)((255) * ((value - min) / range));
	      
	      fts_set_int(atoms + i * n + j, val); 
	    }
	}
      this->scroll = 0; /* mode scroll = false */
      matdisplay_deliver(this);
    }
}

static void 
matdisplay_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  matdisplay_t * this = (matdisplay_t *)o;

  this->scroll = 0;
  matdisplay_deliver(this);  
}

static void
matdisplay_set_zoom_by_client(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  matdisplay_t * this = (matdisplay_t *)o;
  
  if(ac)
    {
      switch(ac)
	{
	case 2:
	  if(fts_is_number(at + 1))
	    this->n_zoom = fts_get_number_int(at + 1);
	case 1:
	  if(fts_is_number(at))
	    this->m_zoom = fts_get_number_int(at);
	  
	default:
	  break;
	}

      if(fts_object_has_id(o))
	{
	  fts_atom_t a[2];
	  
	  fts_set_int(a + 0, this->m_zoom);
	  fts_set_int(a + 1, this->n_zoom);
	  fts_client_send_message(o, sym_zoom, 2, a);
	}
    }
}

static void
matdisplay_set_window_size_by_client(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  matdisplay_t * this = (matdisplay_t *)o;

  this->m_wind = fts_get_int(at);
  this->n_wind = fts_get_int(at + 1);

  if(fts_object_has_id(o))
    {
      fts_atom_t a[2];
      
      fts_set_int(a + 0, this->m_wind);
      fts_set_int(a + 1, this->n_wind);
      fts_client_send_message(o, sym_window, 2, a);
    }
}

static void
matdisplay_set_range(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  matdisplay_t * this = (matdisplay_t *)o;
  
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
	  fts_client_send_message(o, sym_range, 2, a);
	}
    }
}

static void
matdisplay_set_color(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  matdisplay_t * this = (matdisplay_t *)o;
  
  if(ac)
    {
      switch(ac)
	{
	case 2:
	  if(fts_is_number(at + 1))
	    this->max_color = fts_get_number_int(at + 1);
	case 1:
	  if(fts_is_number(at))
	    this->min_color = fts_get_number_int(at);
	  
	default:
	  break;
	}
      
      if(fts_object_has_id(o))
	{
	  fts_atom_t a[2];
	  
	  fts_set_int(a + 0, this->min_color);
	  fts_set_int(a + 1, this->max_color);
	  fts_client_send_message(o, sym_color, 2, a);
	}
    }
}

static void 
matdisplay_update_gui(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  matdisplay_t *this = (matdisplay_t *)o;
  fts_atom_t a[2];  

  fts_set_float(a + 0, this->min);
  fts_set_float(a + 1, this->max);
  fts_client_send_message(o, sym_range, 2, a);

  fts_set_int(a + 0, this->min_color);
  fts_set_int(a + 1, this->max_color);
  fts_client_send_message(o, sym_color, 2, a);  

  fts_set_int(a + 0, this->m_zoom);  
  fts_set_int(a + 1, this->n_zoom);
  fts_client_send_message(o, sym_zoom, 2, a);
}

static void 
matdisplay_dump(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  matdisplay_t *this = (matdisplay_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  fts_atom_t a[2];  

  fts_set_float(a + 0, this->min);
  fts_set_float(a + 1, this->max);
  fts_dumper_send(dumper, sym_range, 2, a);

  fts_set_int(a + 0, this->min_color);
  fts_set_int(a + 1, this->max_color);
  fts_dumper_send(dumper, sym_color, 2, a);

  fts_set_int(a + 0, this->m_zoom);
  fts_set_int(a + 1, this->n_zoom);
  fts_dumper_send(dumper, sym_zoom, 2, a);
}

static void 
matdisplay_update_real_time(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  matdisplay_send( o, 0, 0, 0, 0);
}

/************************************************************
 *
 *  class
 *
 */
static void
matdisplay_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  matdisplay_t * this = (matdisplay_t *)o;
  
  this->min = 0.0;
  this->max = 1.0;
  this->m_size = 1;
  this->n_size = 1; 
  this->m_wind = 1;
  this->n_wind = 1;
  this->min_color = 0x00e3ffe3;
  this->max_color = 0;
  this->m_zoom = 1;
  this->n_zoom = 1;

  this->period = 100.0; 
  this->gate = 1;
  this->pending = 0;

  this->scroll = 0;

  fts_array_init(&this->a, 0, 0);
}

static void
matdisplay_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  matdisplay_t * this = (matdisplay_t *)o;

  fts_array_destroy(&this->a);
}

static void 
matdisplay_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(matdisplay_t), matdisplay_init, matdisplay_delete);

  fts_class_message_varargs(cl, fts_s_update_gui, matdisplay_update_gui); 
  fts_class_message_varargs(cl, fts_s_update_real_time, matdisplay_update_real_time); 
  fts_class_message_varargs(cl, fts_s_dump, matdisplay_dump);

  fts_class_message_varargs(cl, sym_window, matdisplay_set_window_size_by_client);
  fts_class_message_varargs(cl, sym_zoom, matdisplay_set_zoom_by_client);

  fts_class_message_varargs(cl, sym_range, matdisplay_set_range);
  fts_class_message_varargs(cl, sym_color, matdisplay_set_color);
  fts_class_message_varargs(cl, fts_s_clear, matdisplay_clear);
 
  fts_class_inlet_number(cl, 0, matdisplay_number);
  fts_class_inlet_varargs(cl, 0, matdisplay_varargs);
  fts_class_inlet(cl, 0, fvec_type, matdisplay_fvec);
  fts_class_inlet(cl, 0, ivec_type, matdisplay_ivec);
  fts_class_inlet(cl, 0, cvec_type, matdisplay_cvec);
  fts_class_inlet(cl, 0, fmat_type, matdisplay_fmat);
}

void 
matdisplay_config(void)
{
  sym_matdisplay = fts_new_symbol("matdisplay");
  sym_display = fts_new_symbol("display");
  sym_scroll = fts_new_symbol("scroll");
  sym_range = fts_new_symbol("range");  
  sym_zoom = fts_new_symbol("zoom");
  sym_window = fts_new_symbol("window");
  sym_color = fts_new_symbol("color");
  sym_size = fts_new_symbol("size");
 
  fts_class_install(sym_matdisplay, matdisplay_instantiate);
}
