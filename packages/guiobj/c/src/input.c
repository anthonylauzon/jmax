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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*
 * This file's authors:
 *  François Déchelle (dechelle@ircam.fr)
 */

#include <fts/fts.h>

typedef struct {
  fts_object_t head;
  fts_audioport_t *port;
} input_t;

static void 
input_dsp_active(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_update_request(o);
}

static void 
input_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  input_t *this = (input_t *)o;
  int outlets, i;

  if (ac == 0)
    outlets = 2;
  else
    outlets = ac;

  this->port = 0;
#warning (OLD API) input_init use fts_audioport_get_default (OLD API)
/*   this->port = fts_audioport_get_default(o); */
  if ( !this->port)
    {
      fts_object_set_error( o, "default audio port is not defined");
      return;    
    }

  fts_object_set_outlets_number( o, outlets);

  if ( ac != 0)
    {
#warning (OLD API) input_init use fts_audioport_add_input_object (OLD API)
/*       for ( i = 0; i < outlets; i++) */
/* 	fts_audioport_add_input_object( this->port, fts_get_int(at + i) - 1, (fts_object_t *)this); */
    }
  else
    {
/*       for ( i = 0; i < 2; i++) */
/* 	fts_audioport_add_input_object( this->port, i, (fts_object_t *)this); */
    }
  fts_dsp_active_add_listener(o, input_dsp_active);
}

static void 
input_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  input_t *this = (input_t *)o;

  if(this->port)
    {
      int i;
#warning (OLD API) input_delete use fts_audioport_remove_input_object (OLD API)
/*       for ( i = 0; i < fts_object_get_outlets_number( o); i++) */
/* 	fts_audioport_remove_input_object( this->port, i, (fts_object_t *)this); */
    }
  fts_dsp_active_remove_listener(o);
}

static void 
input_start(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_activate();
}

static void
input_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_desactivate();
}

static void 
input_toggle(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if(fts_dsp_is_active())
    fts_dsp_desactivate();
  else
    fts_dsp_activate();

  fts_update_request(o);
}

static void 
input_update_real_time(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int active = fts_dsp_is_active();
  fts_atom_t a;

  fts_set_int( &a, active);
  fts_client_send_message_real_time(o, fts_s_value, 1, &a);
}

static void 
input_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(input_t), input_init, input_delete);

  fts_class_message_varargs(cl, fts_s_update_real_time, input_update_real_time);

  fts_class_message_varargs(cl, fts_s_start, input_start);
  fts_class_message_varargs(cl, fts_s_stop, input_stop);

  fts_class_message_varargs(cl, fts_new_symbol("click"), input_toggle);

  fts_dsp_declare_outlet( cl, 0);
}

void input_config( void)
{
  fts_class_install( fts_new_symbol( "input~"), input_instantiate);
}
