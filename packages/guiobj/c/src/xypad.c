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
xypad_update_gui(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
      /*fts_atom_t a[2];*/

      this->x = x;
      this->y = y;
      /*fts_set_int(a + 0, this->x);
	fts_set_int(a + 1, this->y);
	fts_client_send_message(o, sym_position, 2, a);*/
      fts_update_request(o);
    }

  fts_outlet_varargs(o, 0, ac, at); 
}

static void
xypad_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  xypad_t *this = (xypad_t *)o;
  fts_atom_t a[2];
      
  fts_set_int(a + 0, this->x);
  fts_set_int(a + 1, this->y);
  fts_outlet_varargs(o, 0, 2, a); 
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
      /*fts_atom_t a[2];*/
      
      this->x = x;
      this->y = y;
      /*fts_set_int(a + 0, this->x);
	fts_set_int(a + 1, this->y);
	fts_client_send_message(o, sym_position, 2, a);*/
      fts_update_request(o);
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
xypad_update_real_time(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  xypad_t *this = (xypad_t *) o;  
  fts_atom_t a[2];

  fts_set_int( a, this->x);
  fts_set_int( a + 1, this->y);
  fts_client_send_message(o, sym_position, 2, a);
}

static void
xypad_dump_gui(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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

static void
xypad_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(xypad_t), xypad_init, 0);

  fts_class_message_varargs(cl, fts_s_dump_gui, xypad_dump_gui);  
  fts_class_message_varargs(cl, fts_s_update_gui, xypad_update_gui); 
  fts_class_message_varargs(cl, fts_s_update_real_time, xypad_update_real_time); 

  fts_class_message_varargs(cl, sym_setXMinValue, xypad_set_xminvalue); 
  fts_class_message_varargs(cl, sym_setXMaxValue, xypad_set_xmaxvalue); 
  fts_class_message_varargs(cl, sym_setYMinValue, xypad_set_yminvalue); 
  fts_class_message_varargs(cl, sym_setYMaxValue, xypad_set_ymaxvalue); 
  fts_class_message_varargs(cl, sym_position, xypad_set_couple); 

  fts_class_message_varargs(cl, fts_s_set, xypad_set);
  fts_class_inlet_bang(cl, 0, xypad_bang);

  fts_class_inlet_varargs(cl, 0, xypad_set_couple);

  fts_class_outlet_varargs(cl, 0);
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



