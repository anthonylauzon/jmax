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
} adc_tilda_t;

static void adc_tilda_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  adc_tilda_t *this = (adc_tilda_t *)o;
  int i, outlets;

  this->port = fts_audioport_get_default(o);
  if ( !this->port)
    {
      fts_object_set_error( o, "Default audio port is not defined");
      return;    
    }

  outlets = fts_object_get_outlets_number( o);

  if ( ac != 0)
    {
      for ( i = 0; i < outlets; i++)
	fts_audioport_add_input_object( this->port, fts_get_int(at + i) - 1, (fts_object_t *)this);
    }
  else
    {
      for ( i = 0; i < 2; i++)
	fts_audioport_add_input_object( this->port, i, (fts_object_t *)this);
    }
}

static void adc_tilda_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  adc_tilda_t *this = (adc_tilda_t *)o;
  int i;

  for ( i = 0; i < fts_object_get_outlets_number( o); i++)
    fts_audioport_remove_input_object( this->port, i, (fts_object_t *)this);
}

static void adc_tilda_start(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_activate();
}

static void adc_tilda_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_desactivate();
}

static fts_status_t adc_tilda_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int outlets, i;

  if (ac == 0)
    outlets = 2;
  else
    outlets = ac;

  fts_class_init( cl, sizeof( adc_tilda_t), 1, outlets, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, adc_tilda_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, adc_tilda_delete);

  fts_method_define_varargs( cl, 0, fts_s_start, adc_tilda_start);
  fts_method_define_varargs( cl, 0, fts_s_stop, adc_tilda_stop);

  for ( i = 0; i < outlets; i++)
    fts_dsp_declare_outlet( cl, i);

  return fts_Success;
}

void adc_tilda_config( void)
{
  fts_metaclass_install( fts_new_symbol( "adc~"), adc_tilda_instantiate, fts_narg_equiv);
}
