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

#include <string.h>
#include "fts.h"

#define MIN_FLOAT -68719476736.

#define STRING_SIZE 256

typedef struct {
  fts_object_t o;
  fts_atom_t a;
  char string[STRING_SIZE];
  fts_alarm_t alarm;
  double period;
  int gate;
  int pending;
  int dsp;
  float absmax; /* maximum of absolute value */
  float last; /* last sent maximum */
} display_t;

static fts_symbol_t sym_display = 0;

static void
append_blank_and_atom(char *str, const fts_atom_t *a)
{
  int n = strlen(str);
  char *s = str + n;

  if(fts_is_int(a))
    snprintf(s, STRING_SIZE - n, " %d", fts_get_int(a));
  else if(fts_is_float(a))
    snprintf(s, STRING_SIZE - n, " %g", fts_get_float(a));
  else if(fts_is_symbol(a))
    snprintf(s, STRING_SIZE - n, " %s", fts_symbol_name(fts_get_symbol(a)));
  else
    snprintf(s, STRING_SIZE - n, " %s", "?");
}

static void
append_atom(char *str, const fts_atom_t *a)
{
  int n = strlen(str);
  char *s = str + n;

  if(fts_is_int(a))
    snprintf(s, STRING_SIZE - n, "%d", fts_get_int(a));
  else if(fts_is_float(a))
    snprintf(s, STRING_SIZE - n, "%g", fts_get_float(a));
  else if(fts_is_symbol(a))
    snprintf(s, STRING_SIZE - n, "%s", fts_symbol_name(fts_get_symbol(a)));
  else
    snprintf(s, STRING_SIZE - n, "%s", "?");
}

/************************************************************
 *
 *  send to client with time gate
 *
 */
static void
display_deliver(display_t *this)
{
  if(this->gate)
    {
      this->pending = 0;
      this->gate = 0;

      fts_client_send_message((fts_object_t *)this, fts_s_set, 1, &this->a);

      fts_alarm_set_delay(&this->alarm, this->period);
      fts_alarm_arm(&this->alarm);
    }
  else
    this->pending = 1;
}

static void
display_alarm(fts_alarm_t *alarm, void *o)
{
  display_t * this = (display_t *)o;

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
	      sprintf(this->string, "~ %g", this->absmax);
	      this->last = this->absmax;
	      
	      fts_client_send_message((fts_object_t *)this, fts_s_set, 1, &this->a);
	    }

	  this->absmax = MIN_FLOAT;
	  
	  fts_alarm_set_delay(&this->alarm, this->period);
	  fts_alarm_arm(&this->alarm);
	}
    }
  else if(this->pending)
    {
      this->gate = 0;
      this->pending = 0;

      fts_client_send_message(o, fts_s_set, 1, &this->a);

      fts_alarm_set_delay(&this->alarm, this->period);
      fts_alarm_arm(&this->alarm);
    }
  else
    this->gate = 1;
}

/************************************************************
 *
 *  dsp
 *
 */
static void
display_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  display_t *this = (display_t *)o;
  fts_dsp_descr_t* dsp = (fts_dsp_descr_t *)fts_get_ptr(at);

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
      
      fts_alarm_set_delay(&this->alarm, this->period);
      fts_alarm_arm(&this->alarm);

      fts_set_ptr(a + 0, this);
      fts_set_symbol(a + 1, fts_dsp_get_input_name(dsp, 0));
      fts_set_int(a + 2, fts_dsp_get_input_size(dsp, 0));
      
      dsp_add_funcall(sym_display, 3, a);
    }
}

static void
display_ftl(fts_word_t *argv)
{
  display_t *this = (display_t *) fts_word_get_ptr(argv + 0);
  float *in = (float *) fts_word_get_ptr(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  int i;

  for(i=0; i<n_tick; i++)
    {
      float absval = fabs(in[i]);

      if(absval > this->absmax)
	this->absmax = absval;
    }
}

/************************************************************
 *
 *  input methods
 *
 */
static void
display_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  display_t * this = (display_t *)o;

  sprintf(this->string, "%d", fts_get_int(at));

  display_deliver(this);
}

static void
display_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  display_t * this = (display_t *)o;

  sprintf(this->string, "%g", fts_get_float(at));

  display_deliver(this);
}

static void
display_symbol(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  display_t * this = (display_t *)o;

  sprintf(this->string, "%s", fts_symbol_name(fts_get_symbol(at)));

  display_deliver(this);
}

static void 
display_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  display_t * this = (display_t *)o;
  int i;

  this->string[0] = '\0';
  append_atom(this->string, at);

  for(i=1; i<ac; i++)
    append_blank_and_atom(this->string, at + i);

  display_deliver(this);
}

static void 
display_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  display_t * this = (display_t *)o;
  int i;

  sprintf(this->string, "<%s>", fts_symbol_name(s));

  for(i=0; i<ac; i++)
    append_blank_and_atom(this->string, at + i);

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

  dsp_list_insert(o);

  fts_set_string(&this->a, this->string);
  fts_alarm_init(&this->alarm, 0, display_alarm, (void *)this);

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

  dsp_list_remove(o);
}

static fts_status_t 
display_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(display_t), 1, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, display_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, display_delete);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, display_put);
  
  fts_method_define_varargs(cl, 0, fts_s_int, display_int);
  fts_method_define_varargs(cl, 0, fts_s_float, display_float);
  fts_method_define_varargs(cl, 0, fts_s_symbol, display_symbol);
  fts_method_define_varargs(cl, 0, fts_s_list, display_list);
  fts_method_define_varargs(cl, 0, fts_s_anything, display_anything);

  dsp_sig_inlet(cl, 0);

  return fts_Success;
}

void 
display_config(void)
{
  sym_display = fts_new_symbol("display");
  dsp_declare_function(sym_display, display_ftl);

  fts_class_install(sym_display, display_instantiate);

}
