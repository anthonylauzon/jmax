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
 * Author: Remy Muller
 *
 */


#include <fts/fts.h>

fts_symbol_t sym_setXMaxValue = 0;
fts_symbol_t sym_setXMinValue = 0;
fts_symbol_t sym_setYMaxValue = 0;
fts_symbol_t sym_setYMinValue = 0;
fts_symbol_t sym_position = 0;


typedef struct {
  fts_object_t o;
  int x;
  int y;
  int xMinValue;
  int yMinValue;
  int xMaxValue;
  int yMaxValue;
} xypad_t;


static void
xypad_send_properties(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  xypad_t * this = (xypad_t *)o;
  fts_atom_t a[1];
 
  fts_set_int(a + 0, this->xMinValue);
  fts_client_send_message(o, sym_setXMinValue, 1, a);
  
  fts_set_int(a + 0, this->yMinValue);
  fts_client_send_message(o, sym_setYMinValue, 1, a);
  
  fts_set_int(a + 0, this->xMaxValue);
  fts_client_send_message(o, sym_setXMaxValue, 1, a);

  fts_set_int(a + 0, this->yMaxValue);
  fts_client_send_message(o, sym_setYMaxValue, 1, a);
}

static void xypad_set_couple(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  xypad_t *this = (xypad_t *)o;
  int x = 0;
  int y = 0;

  switch(ac)
    {
    default:
    case 2:
      if(fts_is_number(at + 1))
	y = fts_get_number_int(at + 1);
    case 1:
      if(fts_is_number(at))
	x = fts_get_number_int(at);
    case 0:
      break;
    }

  if((this->x != x || this->y != y) && fts_object_has_id(o))
    {
      fts_atom_t a[2];

      this->x = x;
      this->y = y;
      fts_set_int(a + 0, this->x);
      fts_set_int(a + 1, this->y);
      fts_client_send_message(o, sym_position, 2, a);
    }

  fts_outlet_atoms(o, 0, ac, at); 
}

static void
xypad_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  xypad_t *this = (xypad_t *)o;
  fts_atom_t a[2];
      
  fts_set_int(a + 0, this->x);
  fts_set_int(a + 1, this->y);
  fts_outlet_atoms(o, 0, 2, a); 
}

static void
xypad_set_xminvalue(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  xypad_t *this = (xypad_t *)o;
  int v = fts_get_int(at);
  fts_atom_t a[1];  

  this->xMinValue = v;
 
  if(fts_object_has_id(o))
    {
      fts_set_int(a + 0, this->xMinValue);
      fts_client_send_message(o, sym_setXMinValue, 1, a);
    }
}

static void
xypad_set_yminvalue(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  xypad_t *this = (xypad_t *)o;
  int v = fts_get_int(at);
  fts_atom_t a[1];  
  
  this->yMinValue = v;
  
  if(fts_object_has_id(o))
    {
      fts_set_int(a + 0, this->yMinValue);
      fts_client_send_message(o, sym_setYMinValue, 1, a);
    }
}

static void
xypad_set_xmaxvalue(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  xypad_t *this = (xypad_t *)o;
  int v = fts_get_int(at);
  fts_atom_t a[1];  
  
  this->xMaxValue = v;
  
  if(fts_object_has_id(o))
    {
      fts_set_int(a + 0, this->xMaxValue);
      fts_client_send_message(o, sym_setXMaxValue, 1, a);
    }
}

static void
xypad_set_ymaxvalue(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  xypad_t *this = (xypad_t *)o;
  int v = fts_get_int(at);
  fts_atom_t a[1];  
  
  this->yMaxValue = v;
 
  if(fts_object_has_id(o))
    {
      fts_set_int(a + 0, this->yMaxValue);
      fts_client_send_message(o, sym_setYMaxValue, 1, a);
    }
}

static void
xypad_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  xypad_t *this = (xypad_t *)o;
  int x = 0;
  int y = 0;
  
  switch(ac)
    {
    default:
    case 2:
      if(fts_is_number(at + 1))
	y = fts_get_number_int(at + 1);
    case 1:
      if(fts_is_number(at))
	x = fts_get_number_int(at);
    case 0:
      break;
    }
  
  if((this->x != x || this->y != y) && fts_object_has_id(o))
    {
      fts_atom_t a[2];
        
      this->x = x;
      this->y = y;
      fts_set_int(a + 0, this->x);
      fts_set_int(a + 1, this->y);
      fts_client_send_message(o, sym_position, 2, a);
    }
}  

static void
xypad_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  xypad_t * this = (xypad_t *)o;

  this->xMinValue = 0;
  this->yMinValue = 0;
  this->xMaxValue = 127;
  this->yMaxValue = 127;

  /* silent agreement with client */
  this->x = 64;
  this->y = 64;
}

static void
xypad_dump(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  xypad_t * this = (xypad_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  fts_atom_t a[1];  

  fts_set_int(a + 0, this->xMinValue);
  fts_dumper_send(dumper, sym_setXMinValue, 1, a);

  fts_set_int(a + 0, this->xMaxValue);
  fts_dumper_send(dumper, sym_setXMaxValue, 1, a);

  fts_set_int(a + 0, this->yMinValue);
  fts_dumper_send(dumper, sym_setYMinValue, 1, a);
  
  fts_set_int(a + 0, this->yMaxValue);
  fts_dumper_send(dumper, sym_setYMaxValue, 1, a);
}

static fts_status_t
xypad_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(xypad_t), 1, 1, 0);

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, xypad_init);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_dump, xypad_dump);  

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_send_properties, xypad_send_properties); 
  fts_method_define_varargs(cl, fts_system_inlet, sym_setXMinValue, xypad_set_xminvalue); 
  fts_method_define_varargs(cl, fts_system_inlet, sym_setXMaxValue, xypad_set_xmaxvalue); 
  fts_method_define_varargs(cl, fts_system_inlet, sym_setYMinValue, xypad_set_yminvalue); 
  fts_method_define_varargs(cl, fts_system_inlet, sym_setYMaxValue, xypad_set_ymaxvalue); 
  fts_method_define_varargs(cl, fts_system_inlet, sym_position, xypad_set_couple); 

  fts_method_define_varargs(cl, 0, fts_s_set, xypad_set);
  fts_method_define_varargs(cl, 0, fts_s_bang, xypad_bang);
  fts_method_define_varargs(cl, 0, fts_s_list, xypad_set_couple);

  fts_outlet_type_define_varargs(cl, 0, fts_s_list);

  return fts_ok;
}

void
xypad_config(void)
{
  sym_setXMaxValue = fts_new_symbol("setXMaxValue");
  sym_setXMinValue = fts_new_symbol("setXMinValue");
  sym_setYMaxValue = fts_new_symbol("setYMaxValue");
  sym_setYMinValue = fts_new_symbol("setYMinValue");
  sym_position = fts_new_symbol("position");

  fts_class_install(fts_new_symbol("xypad"), xypad_instantiate);
}



