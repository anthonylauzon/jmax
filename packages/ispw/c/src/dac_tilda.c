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
 * 
 */

#include <fts/fts.h>

typedef struct {
  fts_object_t head;
  fts_audioport_t *port;
  int *indexes;
} dac_tilda_t;

static void dac_tilda_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dac_tilda_t *this = (dac_tilda_t *)o;
  int i, inlets;

  ac--;
  at++;

  this->port = fts_audioport_get_default_out();
  if ( !this->port)
    {
      fts_object_set_error( o, "Default audio port is not defined");
      return;    
    }

  inlets = fts_object_get_inlets_number( o);

  this->indexes = (int *)fts_malloc( inlets * sizeof( int));

  if ( ac != 0)
    {
      for ( i = 0; i < inlets; i++)
	this->indexes[i] = fts_get_int(at + i) - 1;
    }
  else
    {
      this->indexes[0] = 0;
      this->indexes[1] = 1;
    }
}

static void dac_tilda_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dac_tilda_t *this = (dac_tilda_t *)o;

  fts_free( this->indexes);
}

static void dac_tilda_start(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_param_set_int(fts_s_dsp_on, 1);
}

static void dac_tilda_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_param_set_int(fts_s_dsp_on, 0);
}

static void dac_tilda_propagate_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dac_tilda_t *this  = (dac_tilda_t *)o;
  fts_propagate_fun_t propagate_fun = (fts_propagate_fun_t)fts_get_fun(at + 0);
  void *propagate_context = fts_get_ptr(at + 1);
  int inlet = fts_get_int(at + 2);
  int i;
  fts_object_t *outdispatcher;

  outdispatcher = fts_audioport_get_output_dispatcher( this->port);
  i = this->indexes[ inlet];

  if ( outdispatcher && i >= 0 && i < fts_audioport_get_output_channels( this->port))
    (*propagate_fun)( propagate_context, outdispatcher, i);
}

static fts_status_t dac_tilda_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int inlets, i;
  fts_audioport_t *port;

  ac--;
  at++;

  if (ac == 0)
    inlets = 2;
  else
    inlets = ac;

  fts_class_init( cl, sizeof( dac_tilda_t), inlets, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, dac_tilda_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, dac_tilda_delete);

  fts_method_define_varargs( cl, 0, fts_s_start, dac_tilda_start);
  fts_method_define_varargs( cl, 0, fts_s_stop, dac_tilda_stop);

  for ( i = 0; i < inlets; i++)
    fts_dsp_declare_inlet( cl, i);

  fts_class_define_thru( cl, dac_tilda_propagate_input);

  return fts_Success;
}

void dac_tilda_config( void)
{
  fts_metaclass_install( fts_new_symbol( "dac~"), dac_tilda_instantiate, fts_narg_equiv);
}
