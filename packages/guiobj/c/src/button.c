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
#include <ftsprivate/patcher.h>

#define DEFAULT_FLASH 125.0f

fts_symbol_t sym_setColor         = 0;
fts_symbol_t sym_setFlash = 0;

typedef struct 
{
  fts_object_t o;
  int value;
  float flash;
  int color;
} button_t;

/************************************************
 *
 *    object
 *
 */

static void
button_send_properties(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  button_t *this = (button_t *)o;
  fts_atom_t a[1];

  fts_object_get_prop(o, fts_s_color, a);
  if( !fts_get_int( a))
    fts_set_int( a, this->color);
  else
    this->color = fts_get_int( a);
  
  fts_client_send_message(o, sym_setColor, 1, a);

  fts_object_get_prop(o, fts_s_flash, a);  
  if( !fts_get_int( a))
    fts_set_int( a, this->flash);
  else
    this->flash = fts_get_int( a);
  fts_client_send_message(o, sym_setFlash, 1, a);  
}

static void
button_send_ui_properties(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_ui_property_changed(o, fts_s_value);
}
 
/************************************************
 *
 *  user methods
 *
 */
 
static void 
button_off(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  button_t *this = (button_t *)o;

  this->value = 0;
  fts_object_ui_property_changed( (fts_object_t *)this, fts_s_value);
}

static void
button_on(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  button_t *this = (button_t *)o;
  fts_patcher_t *patcher = fts_object_get_patcher(o);

  if(patcher && fts_patcher_is_open(patcher))
    {
      if(this->value > 0)
	fts_timebase_remove_object(fts_get_timebase(), o);

      /* button on */
      this->value = this->color;
      fts_object_ui_property_changed(o, fts_s_value);

      /* schedule button off */
      fts_timebase_add_call(fts_get_timebase(), o, button_off, 0, this->flash);
    }

  fts_outlet_bang(o, 0);
}

/************************************************
 *
 *  deamons 
 *
 */
 
/* daemon to get the "value" property */
static void
button_get_value(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  button_t *this = (button_t *)obj;

  fts_set_int(value, this->value);
}


static void
button_put_value(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  button_t *this = (button_t *)o;

  button_on(o, 0, 0, 0, 0);
}

static void
button_set_color(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ((button_t *)o)->color = fts_get_int( at);
  fts_object_put_prop( o, s, at);
  
  fts_patcher_set_dirty((fts_patcher_t *)o->patcher, 1);
}

static void
button_set_flash(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ((button_t *)o)->flash = fts_get_int( at);
  fts_object_put_prop( o, s, at);

  fts_patcher_set_dirty((fts_patcher_t *)o->patcher, 1);
}

static void 
button_save_dotpat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  FILE *file;
  int x, y, w;
  fts_atom_t a;

  file = (FILE *)fts_get_pointer( at);

  fts_object_get_prop( o, fts_s_x, &a);
  x = fts_get_int( &a);
  fts_object_get_prop( o, fts_s_y, &a);
  y = fts_get_int( &a);
  fts_object_get_prop( o, fts_s_width, &a);
  w = fts_get_int( &a);

  fprintf( file, "#P button %d %d %d;\n", x, y, w);
}


/************************************************
 *
 *    class
 *
 */
 
static void
button_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  button_t *this = (button_t *)o;

  this->value = 0;
  this->flash = DEFAULT_FLASH;
  this->color = 1;
}

static fts_status_t
button_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(button_t), 1, 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, button_init);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_send_properties, button_send_properties); 
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_send_ui_properties, button_send_ui_properties); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_color, button_set_color); 
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_flash, button_set_flash); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_save_dotpat, button_save_dotpat); 
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_bang, button_on);

  fts_method_define_varargs(cl, 0, fts_s_anything, button_on);

  /* value daemons */
  fts_class_add_daemon(cl, obj_property_get, fts_s_value, button_get_value);
  fts_class_add_daemon(cl, obj_property_put, fts_s_value, button_put_value);

  fts_outlet_type_define_varargs(cl, 0, fts_s_bang);

  return fts_Success;
}


void
button_config(void)
{
  fts_class_install(fts_new_symbol("button"), button_instantiate);
  sym_setColor = fts_new_symbol("setColor");
  sym_setFlash = fts_new_symbol("setFlash");
}


