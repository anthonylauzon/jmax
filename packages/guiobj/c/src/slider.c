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

fts_symbol_t sym_setValue = 0;
fts_symbol_t sym_setOrientation = 0;
fts_symbol_t sym_setMaxValue = 0;
fts_symbol_t sym_setMinValue = 0;

typedef struct 
{
  fts_object_t o;
  int persistence;
  int n;
  int min; /* display minimum value */
  int max; /* display maximum value */
  int orientation; /* orientation */
} slider_t;

static void
slider_update_gui(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  slider_t *self = (slider_t *)o;  
  fts_atom_t a;
  
  fts_set_int(&a, self->orientation);
  fts_client_send_message(o, sym_setOrientation, 1, &a);
  
  fts_set_int(&a, self->min);
  fts_client_send_message(o, sym_setMinValue, 1, &a);
  
  fts_set_int(&a, self->max);
  fts_client_send_message(o, sym_setMaxValue, 1, &a);
}

static void
slider_update_real_time(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  slider_t *self = (slider_t *)o;
  fts_atom_t a;

  fts_set_int(&a, self->n);
  fts_client_send_message_real_time(o, fts_s_value, 1, &a);  
}

static void
slider_set(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  slider_t *self = (slider_t *)o;

  if(ac > 0 && fts_is_number(at))
  {
    int n = fts_get_number_int(at);

    if (self->n != n)
    {
      self->n = n;
      fts_update_request(o);
    }
  }
}

static void
slider_bang(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  slider_t *self = (slider_t *)o;

  fts_outlet_int(o, 0, self->n);
}

static void 
slider_number(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  slider_t *self = (slider_t *)o;
  int n = fts_get_number_int(at);
  
  if (self->n != n)
    {
      fts_update_request(o);
      self->n = n;
    }
  
  fts_outlet_int(o, 0, n);
}

static void
slider_incr(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  slider_t *self = (slider_t*)o;
  int n;

  if (ac > 0 && fts_is_number(at))
  {
    n = self->n + fts_get_number_int(at);
    self->n = n;
  }
  else
  {
    n = self->n + 1;
    self->n = n;
  }
  
  fts_update_request(o);
  fts_outlet_int(o, 0, n);
}

static void 
slider_set_and_output(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  if(ac > 0 && fts_is_number(at))
    slider_number(o, 0, 1, at, fts_nix);
}

static void
slider_varargs(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  slider_set_and_output(o, 0, 1, at, fts_nix);
}

static void
slider_set_min(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  slider_t *self = (slider_t *)o;
  fts_atom_t a;
  
  self->min = fts_get_number_int(at);

  fts_set_int(&a, self->min);
  fts_client_send_message(o, sym_setMinValue, 1, &a);  
}

static void
slider_set_max(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  slider_t *self = (slider_t *)o;
  fts_atom_t a;

  self->max = fts_get_number_int(at);

  fts_set_int(&a, self->max);
  fts_client_send_message(o, sym_setMaxValue, 1, &a);
}

static void
slider_set_range(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  slider_t *self = (slider_t *)o;
  fts_atom_t a;

  if(ac > 1 && fts_is_number(at))
    {
      self->min = fts_get_number_int(at);
      ac--;
      at++;
    }
  else
    self->min = 0;
  
  if(ac > 0 && fts_is_number(at))
    self->max = fts_get_number_int(at);

  fts_set_int(&a, self->min);
  fts_client_send_message(o, sym_setMinValue, 1, &a);

  fts_set_int(&a, self->max);
  fts_client_send_message(o, sym_setMaxValue, 1, &a);
}

static void
slider_set_orientation(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  slider_t *self = (slider_t *)o;

  self->orientation = fts_get_number_int(at);
}

static void 
slider_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  slider_t *self = (slider_t *)o;

  self->persistence = 0;
  self->min = 0;
  self->max = 127;
  self->orientation = 0;
}

static void
slider_set_from_instance(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  slider_t *self = (slider_t *)o;
  slider_t *in = (slider_t *)fts_get_object(at);

  self->n = in->n;
  self->min = in->min;
  self->max = in->max;

  if(fts_object_has_client(o))
  {
    fts_atom_t a;
    
    fts_set_int(&a, self->min);
    fts_client_send_message(o, sym_setMinValue, 1, &a);

    fts_set_int(&a, self->max);
    fts_client_send_message(o, sym_setMaxValue, 1, &a);

    fts_update_request(o);
  }
}

static void
slider_dump_state(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  slider_t * self = (slider_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  fts_atom_t a;

  fts_set_int(&a, self->n);
  fts_dumper_send(dumper, fts_s_set, 1, &a);
}

static void
slider_dump_gui(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  slider_t * self = (slider_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  fts_atom_t a;

  fts_set_int(&a, self->min);
  fts_dumper_send(dumper, fts_s_min_value, 1, &a);

  fts_set_int(&a, self->max);
  fts_dumper_send(dumper, fts_s_max_value, 1, &a);

  if(self->orientation != 0)
  {
    fts_set_int(&a, self->orientation);
    fts_dumper_send(dumper, fts_s_orientation, 1, &a);
  }
}

static void 
slider_save_dotpat(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  slider_t *self = (slider_t *)o;
  FILE *file = (FILE *)fts_get_pointer( at);
  fts_atom_t a;
  int x, y, w;

  fts_object_get_prop( o, fts_s_x, &a);
  x = fts_get_int( &a);

  fts_object_get_prop( o, fts_s_y, &a);
  y = fts_get_int( &a);

  fts_object_get_prop( o, fts_s_width, &a);
  w = fts_get_int( &a);

  fprintf( file, "#P slider %d %d %d %d;\n", x, y, w, self->max - self->min + 1);
}


static void
slider_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(slider_t), slider_init, NULL); 

  fts_class_message_varargs(cl, fts_s_name, fts_object_name);
  fts_class_message_varargs(cl, fts_s_persistence, fts_object_persistence); 
  
  fts_class_message_varargs(cl, fts_s_dump_state, slider_dump_state); 
  fts_class_message_varargs(cl, fts_s_dump_gui, slider_dump_gui); 
  
  fts_class_message_varargs(cl, fts_s_update_gui, slider_update_gui); 
  fts_class_message_varargs(cl, fts_s_update_real_time, slider_update_real_time);
  
  fts_class_message_varargs(cl, fts_s_set_from_instance, slider_set_from_instance);

  fts_class_message_varargs(cl, fts_s_save_dotpat, slider_save_dotpat); 

  fts_class_message_varargs(cl, fts_s_value, slider_set_and_output); 
  fts_class_message_varargs(cl, fts_s_min_value, slider_set_min); 
  fts_class_message_varargs(cl, fts_s_max_value, slider_set_max); 
  fts_class_message_varargs(cl, fts_s_orientation, slider_set_orientation); 

  fts_class_message_varargs(cl, fts_s_set, slider_set);
  fts_class_message_varargs(cl, fts_s_range, slider_set_range);

  fts_class_message_varargs(cl, fts_s_send, slider_set_and_output);

  fts_class_message_varargs(cl, fts_new_symbol("incr"), slider_incr);

  fts_class_inlet_void(cl, 0, slider_bang);
  fts_class_inlet_number(cl, 0, slider_number);
  fts_class_inlet_varargs(cl, 0, slider_varargs);

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

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
