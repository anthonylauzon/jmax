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

#define DEFAULT_FLASH 125.0

fts_symbol_t sym_setColor = 0;
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
button_update_gui(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  button_t *this = (button_t *)o;
  fts_atom_t a;

  fts_set_int( &a, this->color);  
  fts_client_send_message(o, sym_setColor, 1, &a);

  fts_set_int( &a, this->flash);
  fts_client_send_message(o, sym_setFlash, 1, &a);  
}

static void
button_update_real_time(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  button_t *this = (button_t *)o;
  fts_atom_t a;

  fts_set_int( &a, this->value);  
  fts_client_send_message_real_time(o, fts_s_value, 1, &a);
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
  fts_update_request( (fts_object_t *)this);
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
      fts_update_request(o);

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
 
static void
button_set_color(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  button_t *this = (button_t *)o;

  if( this->color != fts_get_int( at))
    {
      this->color = fts_get_int( at);
  
      fts_patcher_set_dirty(fts_object_get_patcher(o), 1);
    }
}

static void
button_set_flash(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  button_t *this = (button_t *)o;

  if( this->flash != fts_get_int( at))
    {
      this->flash = fts_get_int( at);
      
      fts_patcher_set_dirty(fts_object_get_patcher(o), 1);
    }
}

static void
button_put_color(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  button_set_color(o, 0, 0, 1, value);
}

static void
button_put_flash(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  button_set_flash(o, 0, 0, 1, value);
}

static void
button_dump(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  button_t * this = (button_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  fts_atom_t a;

  fts_set_int(&a, this->color);
  fts_dumper_send(dumper, fts_s_color, 1, &a);

  fts_set_int(&a, this->flash);
  fts_dumper_send(dumper, fts_s_flash, 1, &a);
}

static void 
button_save_dotpat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  FILE *file = (FILE *)fts_get_pointer( at);
  fts_atom_t a;
  int x, y, w;

  fts_object_get_prop( o, fts_s_x, &a);
  x = fts_get_int( &a);

  fts_object_get_prop( o, fts_s_y, &a);
  y = fts_get_int( &a);

  fts_object_get_prop( o, fts_s_width, &a);
  w = fts_get_int( &a);

  fprintf( file, "#P button %d %d %d;\n", x, y, w);
}


static void
button_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  button_t *this = (button_t *)o;

  this->value = 0;
  this->flash = DEFAULT_FLASH;
  this->color = 1;
}

static void
button_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(button_t), button_init, NULL);

  fts_class_message_varargs(cl, fts_s_dump, button_dump);

  fts_class_message_varargs(cl, fts_s_update_gui, button_update_gui); 
  fts_class_message_varargs(cl, fts_s_update_real_time, button_update_real_time); 

  fts_class_message_varargs(cl, fts_s_save_dotpat, button_save_dotpat); 

  fts_class_message_varargs(cl, fts_s_color, button_set_color); 
  fts_class_message_varargs(cl, fts_s_flash, button_set_flash); 

  /* property daemons for compatibilty with older bmax files */
  fts_class_add_daemon(cl, obj_property_put, fts_s_color, button_put_color);
  fts_class_add_daemon(cl, obj_property_put, fts_s_flash, button_put_flash);

  fts_class_message_varargs(cl, fts_new_symbol("click"), button_on);

  fts_class_input_handler(cl, button_on);
 
  fts_class_outlet_bang(cl, 0);
}


void
button_config(void)
{
  sym_setColor = fts_new_symbol("setColor");
  sym_setFlash = fts_new_symbol("setFlash");

  fts_class_install(fts_new_symbol("button"), button_instantiate);
}
