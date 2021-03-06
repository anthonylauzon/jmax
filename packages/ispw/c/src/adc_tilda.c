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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */

#include <fts/fts.h>

typedef struct {
  fts_object_t head;
  fts_audioport_t *port;
} adc_tilda_t;

static void adc_tilda_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  adc_tilda_t *this = (adc_tilda_t *)o;
  int outlets;

  if (ac == 0)
    outlets = 2;
  else
    outlets = ac;
/* #warning (OLD API) fts_audioport_get_default (OLD API) */
/*   this->port = fts_audioport_get_default(o); */
  if ( !this->port)
    {
      fts_object_error( o, "default audio port is not defined");
      return;    
    }

  fts_object_set_outlets_number( o, outlets);

  if ( ac != 0)
    {
/* #warning (OLD API) fts_audioport_add_input_object (OLD API) */
/*       for (int i = 0; i < outlets; i++) */
/* 	fts_audioport_add_input_object( this->port, fts_get_int(at + i) - 1, (fts_object_t *)this); */
    }
  else
    {
/* #warning (OLD API) fts_audioport_add_input_object (OLD API) */
/*       for (int i = 0; i < 2; i++) */
/* 	fts_audioport_add_input_object( this->port, i, (fts_object_t *)this); */
    }
}

static void adc_tilda_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  adc_tilda_t *this = (adc_tilda_t *)o;

  if(this->port)
    {
/* #warning (OLD API) fts_audioport_remove_input_object (OLD API) */
/*       for (int i = 0; i < fts_object_get_outlets_number( o); i++) */
/* 	fts_audioport_remove_input_object( this->port, i, (fts_object_t *)this); */
    }
}

static void adc_tilda_start(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_dsp_activate();
}

static void adc_tilda_stop(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_dsp_desactivate();
}

static void adc_tilda_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof( adc_tilda_t), adc_tilda_init, adc_tilda_delete);

  fts_class_message_varargs(cl, fts_s_start, adc_tilda_start);
  fts_class_message_varargs(cl, fts_s_stop, adc_tilda_stop);

  fts_dsp_declare_outlet( cl, 0);
}

void adc_tilda_config( void)
{
/*   fts_class_install( fts_new_symbol( "adc~"), adc_tilda_instantiate); */
}
