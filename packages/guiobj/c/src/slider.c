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


#include <fts/fts.h>

fts_symbol_t sym_setValue = 0;
fts_symbol_t sym_setOrientation = 0;
fts_symbol_t sym_setMaxValue = 0;
fts_symbol_t sym_setMinValue = 0;

typedef struct 
{
  fts_object_t o;
  int n;
  int min; /* display minimum value */
  int max; /* display maximum value */
  int orientation; /* orientation */
} slider_t;

static void
slider_update_gui(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  slider_t *this = (slider_t *)o;
  fts_atom_t a;

  fts_set_int(&a, this->orientation);
  fts_client_send_message(o, sym_setOrientation, 1, &a);

  fts_set_int(&a, this->min);
  fts_client_send_message(o, sym_setMinValue, 1, &a);

  fts_set_int(&a, this->max);
  fts_client_send_message(o, sym_setMaxValue, 1, &a);
}

static void
slider_update_real_time(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  slider_t *this = (slider_t *)o;
  fts_atom_t a;

  fts_set_int(&a, this->n);
  fts_client_send_message_real_time(o, fts_s_value, 1, &a);  
}

static void 
slider_set_value(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  slider_t *this = (slider_t *)o;
  int n;

  n = fts_get_int_arg( ac, at, 0, 0);

  if ( this->n != n)
    {
      this->n = n;
      fts_update_request( (fts_object_t *)this);
    }

  fts_outlet_int( o, 0, n);
}

static void
slider_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  slider_t *this = (slider_t *)o;

  fts_outlet_int(o, 0, this->n);
}


static void
slider_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  slider_t *this = (slider_t *)o;
  int n = fts_get_int(at);

  if (this->n != n)
    {
      fts_update_request(o);
      this->n = n;
    }

  fts_outlet_int(o, 0, n);
}


static void
slider_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  slider_t *this = (slider_t *)o;
  int n = (int)fts_get_float(at);

  if (this->n != n)
    {
      fts_update_request(o);
      this->n = n;
    }

  fts_outlet_int(o, 0, n);
}

/* in case of list, only the first value is significative */
static void
slider_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if (fts_is_int(at))
    slider_int(o, winlet, fts_s_int, 1, at);
  else if (fts_is_float(at))
    slider_float(o, winlet, fts_s_float, 1, at);
}

static void
slider_set_min(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  slider_t *this = (slider_t *)o;
  
  this->min = fts_get_number_int(at);
}

static void
slider_set_max(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  slider_t *this = (slider_t *)o;

  this->max = fts_get_number_int(at);
}

static void
slider_set_orientation(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  slider_t *this = (slider_t *)o;

  this->orientation = fts_get_number_int(at);
}

static void
slider_set_range(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  slider_t *this = (slider_t *)o;

  if(ac > 1 && fts_is_number(at))
    {
      this->min = fts_get_number_int(at);
      ac--;
      at++;
    }
  else
    this->min = 0;
  
  if(ac > 0 && fts_is_number(at))
    this->max = fts_get_number_int(at);
}

static void
slider_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  slider_t *this = (slider_t *)o;
  int n = fts_get_int_arg(ac, at, 0, 0);

  if (this->n != n)
    {
      this->n = n;
      fts_update_request(o);
    }
}


static void
slider_get_value(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  slider_t *this = (slider_t *)o;

  fts_set_int(value, this->n);
}

static void
slider_put_value(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  slider_t *this = (slider_t *)o;

  this->n = fts_get_int(value);

  fts_outlet_int(o, 0, this->n);
  fts_update_request(o);
}

static void
slider_put_min(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  slider_set_min(o, 0, 0, 1, value);
}

static void
slider_put_max(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  slider_set_max(o, 0, 0, 1, value);
}

static void
slider_put_orientation(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  slider_set_orientation(o, 0, 0, 1, value);
}

static void 
slider_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  slider_t *this = (slider_t *)o;

  this->min = 0;
  this->max = 127;
  this->orientation = 0;
}

static void
slider_dump(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  slider_t * this = (slider_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  fts_atom_t a;

  fts_set_int(&a, this->min);
  fts_dumper_send(dumper, fts_s_min_value, 1, &a);

  fts_set_int(&a, this->max);
  fts_dumper_send(dumper, fts_s_max_value, 1, &a);

  if(this->orientation != 0)
    {
      fts_set_int(&a, this->orientation);
      fts_dumper_send(dumper, fts_s_orientation, 1, &a);
    }
}

static void 
slider_save_dotpat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  slider_t *this = (slider_t *)o;
  FILE *file = (FILE *)fts_get_pointer( at);
  fts_atom_t a;
  int x, y, w;

  fts_object_get_prop( o, fts_s_x, &a);
  x = fts_get_int( &a);

  fts_object_get_prop( o, fts_s_y, &a);
  y = fts_get_int( &a);

  fts_object_get_prop( o, fts_s_width, &a);
  w = fts_get_int( &a);

  fprintf( file, "#P slider %d %d %d %d;\n", x, y, w, this->max - this->min + 1);
}


static void
slider_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(slider_t), slider_init, NULL); 

  fts_class_message_varargs(cl, fts_s_dump, slider_dump); 

  fts_class_message_varargs(cl, fts_s_update_gui, slider_update_gui); 
  fts_class_message_varargs(cl, fts_s_update_real_time, slider_update_real_time); 
  fts_class_message_varargs(cl, fts_s_save_dotpat, slider_save_dotpat); 

  fts_class_message_varargs(cl, fts_s_value, slider_set_value); 
  fts_class_message_varargs(cl, fts_s_min_value, slider_set_min); 
  fts_class_message_varargs(cl, fts_s_max_value, slider_set_max); 
  fts_class_message_varargs(cl, fts_s_orientation, slider_set_orientation); 

  fts_class_message_varargs(cl, fts_s_bang, slider_bang);
  fts_class_message_varargs(cl, fts_s_set, slider_set);
  fts_class_message_varargs(cl, fts_s_range, slider_set_range);

  fts_class_inlet_int(cl, 0, slider_int);
  fts_class_inlet_float(cl, 0, slider_float);
  fts_class_inlet_varargs(cl, 0, slider_list);

  /* property daemons for compatibilty with older bmax files */
  fts_class_add_daemon(cl, obj_property_put, fts_s_min_value, slider_put_min);
  fts_class_add_daemon(cl, obj_property_put, fts_s_max_value, slider_put_max);
  fts_class_add_daemon(cl, obj_property_put, fts_s_orientation, slider_put_orientation);

  fts_class_outlet_number(cl, 0);
}

void
slider_config(void)
{
  sym_setOrientation = fts_new_symbol("setOrientation");
  sym_setMinValue = fts_new_symbol("setMinValue");
  sym_setMaxValue = fts_new_symbol("setMaxValue");

  fts_class_install(fts_new_symbol("slider"),slider_instantiate);
}
