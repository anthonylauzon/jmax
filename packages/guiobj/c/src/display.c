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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include <math.h>
#include <string.h>
#include <fts/fts.h>

#define MIN_FLOAT -68719476736.

#define STRING_SIZE 256

typedef struct 
{
  fts_dsp_object_t o;
  fts_memorystream_t *stream;
  double period;
  int gate;
  int pending;
  int dsp;
  float absmax; /* maximum of absolute value */
  float last; /* last sent maximum */
} display_t;

static fts_symbol_t sym_display = 0;

static void 
display_called(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);

/************************************************************
 *
 *  send to client with time gate
 *
 */
static void
display_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  display_t * this = (display_t *)o;
  fts_patcher_t *patcher = fts_object_get_patcher(o);

  if(patcher && fts_patcher_is_open(patcher))
    {
      if(this->dsp)
	{
	  if(this->absmax == MIN_FLOAT)
	    {
	      this->pending = 0;
	      this->gate = 1;
	      this->dsp = 0;
	    }
	  else
	    {
	      if(this->absmax != this->last)
		{
		  fts_atom_t a;

		  fts_memorystream_reset(this->stream);
  
		  fts_spost((fts_bytestream_t *)this->stream, "~ ");
		  fts_spost_float((fts_bytestream_t *)this->stream, this->absmax);
		  fts_bytestream_output_char((fts_bytestream_t *)this->stream, '\0');
		  
		  fts_set_string(&a, fts_memorystream_get_bytes(this->stream));
		  fts_client_send_message((fts_object_t *)this, fts_s_set, 1, &a);

		  this->last = this->absmax;
		}
	      
	      this->absmax = MIN_FLOAT;
	      
	      fts_timebase_add_call(fts_get_timebase(), o, display_called, 0, this->period);
	    }
	}
      else if(this->pending)
	{
	  fts_atom_t a;

	  this->gate = 0;
	  this->pending = 0;
	  
	  fts_bytestream_output_char((fts_bytestream_t *)this->stream, '\0');
	  fts_set_string(&a, fts_memorystream_get_bytes(this->stream));
	  fts_client_send_message(o, fts_s_set, 1, &a);
	  
	  fts_timebase_add_call(fts_get_timebase(), o, display_called, 0, this->period);
	}
      else
	this->gate = 1;
    }
  else
    { 
      this->gate = 1;
      this->pending = 0;
    }
}

static void
display_deliver(display_t *this)
{
  this->pending = 1;

  if(this->gate)
    fts_update_request( (fts_object_t *)this);
}

static void 
display_called(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if(fts_object_get_patcher(o) != NULL)  
    fts_update_request(o);
}

static void 
display_update_real_time(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  display_send( o, 0, 0, 0, 0);
}

static void
display_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  display_t *this = (display_t *)o;
  fts_dsp_descr_t* dsp = (fts_dsp_descr_t *)fts_get_pointer(at);

  if(fts_dsp_is_sig_inlet((fts_object_t *)this, 0) && !fts_dsp_is_input_null(dsp, 0))
    {
      fts_atom_t a[3];
      
      /* close gate */
      this->gate = 0;
      this->pending = 0;

      /* enable and init dsp */
      this->dsp = 1;
      this->absmax = MIN_FLOAT;
      this->last = MIN_FLOAT;

      fts_timebase_remove_object(fts_get_timebase(), o);
      fts_timebase_add_call(fts_get_timebase(), o, display_called, 0, this->period);

      fts_set_pointer(a + 0, this);
      fts_set_symbol(a + 1, fts_dsp_get_input_name(dsp, 0));
      fts_set_int(a + 2, fts_dsp_get_input_size(dsp, 0));
      
      fts_dsp_add_function(sym_display, 3, a);
    }
}

static void
display_ftl(fts_word_t *argv)
{
  display_t *this = (display_t *) fts_word_get_pointer(argv + 0);
  float *in = (float *) fts_word_get_pointer(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  int i;

  for(i=0; i<n_tick; i++)
    {
      float absval = fabs((double)in[i]);

      if(absval > this->absmax)
	this->absmax = absval;
    }
}

static void 
display_message(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  display_t * this = (display_t *)o;

  fts_memorystream_reset(this->stream);  

  fts_spost_symbol((fts_bytestream_t *)this->stream, s);
  fts_spost((fts_bytestream_t *)this->stream, " ");
  fts_spost_atoms((fts_bytestream_t *)this->stream, ac, at);

  display_deliver(this);
}

static void 
display_varargs(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  display_t * this = (display_t *)o;

  fts_memorystream_reset(this->stream);  

  if(ac == 1 && !fts_is_symbol(at))
    fts_spost_atoms((fts_bytestream_t *)this->stream, 1, at);	
  else
    {
      fts_spost((fts_bytestream_t *)this->stream, "(");
      fts_spost_atoms((fts_bytestream_t *)this->stream, ac, at);
      fts_spost((fts_bytestream_t *)this->stream, ")");
    }    

  display_deliver(this);
}

/************************************************************
 *
 *  class
 *
 */
static void
display_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  display_t * this = (display_t *)o;

  fts_dsp_object_init((fts_dsp_object_t *)o);

  this->stream = (fts_memorystream_t *)fts_object_create(fts_memorystream_type, NULL, 0, 0);

  this->period = 50.0;
  this->gate = 1;
  this->pending = 0;
  this->dsp = 0;
  this->absmax = MIN_FLOAT;
  this->last = MIN_FLOAT;
}

static void
display_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  display_t * this = (display_t *)o;

  fts_dsp_object_delete((fts_dsp_object_t *)o);
  
  fts_object_destroy((fts_object_t *)this->stream);
}

static void 
display_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(display_t), display_init, display_delete);

  fts_class_message_varargs(cl, fts_s_put, display_put);
  fts_class_message_varargs(cl, fts_s_update_real_time, display_update_real_time); 

  fts_dsp_declare_inlet(cl, 0);
  fts_class_inlet_varargs(cl, 0, display_varargs);
  fts_class_set_default_handler(cl, display_message);
}

void 
display_config(void)
{
  sym_display = fts_new_symbol("display");
  fts_dsp_declare_function(sym_display, display_ftl);

  fts_class_install(sym_display, display_instantiate);
}
