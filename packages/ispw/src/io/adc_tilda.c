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
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Francois Dechelle, Norbert Schnell, Riccardo Borghesi.
 *
 */

/*
 * This file's authors:
 *  François Déchelle (dechelle@ircam.fr)
 */

#include <fts/fts.h>

typedef struct {
  fts_object_t head;
  fts_object_t **targets;
} adc_tilda_t;

static void adc_tilda_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  adc_tilda_t *this = (adc_tilda_t *)o;
  fts_audioport_t *port;
  int i, inlets;

  ac--;
  at++;

  inlets = fts_object_get_inlets_number( o);

  this->targets = (fts_object_t **)fts_malloc( inlets * sizeof( fts_object_t *));

  port = fts_audioport_get_default();

  if ( ac != 0)
    {
      for ( i = 0; i < inlets; i++)
	this->targets[i] = fts_audioport_get_out_object( port, fts_get_long(at + i) - 1);
    }
  else
    {
      this->targets[0] = fts_audioport_get_out_object( port, 0);
      this->targets[1] = fts_audioport_get_out_object( port, 1);
    }
}

static void adc_tilda_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  adc_tilda_t *this = (adc_tilda_t *)o;
  int i;

  /* For now, we leave the objects that were created
     Otherwise, it crashes when you quit because it tries to delete
     objects that were already deleted
     Anyway, if you delete the adc~ object, the dispatch objects
     will no longer be visited when you create the dsp chain
     because of the propagation mechanism
  */

/*    for ( i = 0; i < fts_object_get_inlets_number( o); i++) */
/*      fts_object_delete_from_patcher( this->targets[ i]); */
}

static void adc_tilda_propagate_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  adc_tilda_t *this  = (adc_tilda_t *)o;
  fts_propagate_fun_t propagate_fun = (fts_propagate_fun_t)fts_get_fun(at + 0);
  void *propagate_context = fts_get_ptr(at + 1);
  int n = fts_get_int(at + 2);

  if ( this->targets)
    (*propagate_fun)( propagate_context, this->targets[n], 0);
}

static fts_status_t adc_tilda_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int inlets, i;
  fts_audioport_t *port;

  ac--;
  at++;

  if (ac == 0)
    inlets = 2;
  else
    inlets = ac - 1;

  fts_class_init( cl, sizeof( adc_tilda_t), inlets, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, adc_tilda_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, adc_tilda_delete);

  for ( i = 0; i < inlets; i++)
    fts_dsp_declare_inlet( cl, i);

  fts_class_define_thru( cl, adc_tilda_propagate_input);

  return fts_Success;
}

void adc_tilda_config( void)
{
  fts_metaclass_install( fts_new_symbol( "adc~"), adc_tilda_instantiate, fts_narg_equiv);
}
