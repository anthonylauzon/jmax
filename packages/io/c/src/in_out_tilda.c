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

#include <fts/fts.h>

static int in_out_class_check( int ac, const fts_atom_t *at, int *inoutlets)
{
  fts_audioport_t *port = 0;

  if (ac == 1)
    {
      if (fts_is_int( at))
	*inoutlets = 1;
      else if ( fts_is_object( at))
	port = (fts_audioport_t *)fts_get_object( at);
      else
	return 0;
    }
  else if ( ac == 2 && fts_is_object( at) && fts_is_int(at+1) )
    {
      *inoutlets = 1;
      port = (fts_audioport_t *)fts_get_object( at);
    }
  else if (ac != 0)
    return 0;

  if (!*inoutlets)
    {
      if (!port)
	port = fts_audioport_get_default( 0);

      if (port && fts_object_is_audioport( (fts_object_t *)port))
	*inoutlets = fts_audioport_get_output_channels( port);
      else
	*inoutlets = 2;
    }

  return 1;
}


static int in_out_check( fts_object_t *o, int ac, const fts_atom_t *at, fts_audioport_t **port, int *channel)
{
  *port = 0;
  *channel = -1;

  if (ac == 1)
    {
      if (fts_is_int( at))
	*channel = fts_get_int( at);
      else if ( fts_is_object( at) )
	*port = (fts_audioport_t *)fts_get_object( at);
    }
  else if ( ac == 2)
    {
      *channel = fts_get_int( at+1);
      *port = (fts_audioport_t *)fts_get_object( at);
    }

  if (!*port)
    {
      *port = fts_audioport_get_default( o);

      if ( !*port)
	{
	  fts_object_set_error( o, "Default audio port is not defined");
	  return 0;
	}
    }

  if ( !fts_object_is_audioport( (fts_object_t *)*port) )
    {
      fts_object_set_error( o, "Argument must be an audio port");
      return 0;
    }

  return 1;
}

/* ********************************************************************** */
/* in~ object                                                             */
/* ********************************************************************** */

typedef struct {
  fts_object_t head;
  fts_audioport_t *port;
  int channel;
} in_tilda_t;

static void in_tilda_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  in_tilda_t *this = (in_tilda_t *)o;

  if ( !in_out_check( o, ac-1, at+1, &this->port, &this->channel))
    return;

  if (this->channel >= 0)
    {
      int i;

      for ( i = 0; i < fts_object_get_outlets_number( o); i++)
	fts_audioport_add_input_object( this->port, i, (fts_object_t *)this);
    }
  else
    {
      fts_audioport_add_input_object( this->port, this->channel, (fts_object_t *)this);
    }
}

static void in_tilda_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  in_tilda_t *this = (in_tilda_t *)o;

  if (this->channel >= 0)
    {
      int i;

      for ( i = 0; i < fts_object_get_outlets_number( o); i++)
	fts_audioport_remove_input_object( this->port, i, (fts_object_t *)this);
    }
  else
    {
      fts_audioport_remove_input_object( this->port, this->channel, (fts_object_t *)this);
    }
}

static fts_status_t in_tilda_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int outlets = 0, i;

  ac--;
  at++;

  if ( !in_out_class_check( ac, at, &outlets))
    return &fts_CannotInstantiate;

  fts_class_init( cl, sizeof( fts_object_t), 0, outlets, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, in_tilda_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, in_tilda_delete);

  for ( i = 0; i < outlets; i++)
    fts_dsp_declare_outlet( cl, i);

  return fts_Success;
}


/* ********************************************************************** */
/* out~ object                                                            */
/* ********************************************************************** */

typedef struct {
  fts_object_t head;
  fts_audioport_t *port;
  int channel;
} out_tilda_t;

static void out_tilda_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  out_tilda_t *this = (out_tilda_t *)o;

  in_out_check( o, ac-1, at+1, &this->port, &this->channel);
}

static void out_tilda_propagate_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  out_tilda_t *this  = (out_tilda_t *)o;
  fts_propagate_fun_t propagate_fun = (fts_propagate_fun_t)fts_get_fun(at + 0);
  void *propagate_context = fts_get_ptr(at + 1);
  int inlet = fts_get_int(at + 2);
  fts_object_t *outdispatcher;

  outdispatcher = fts_audioport_get_output_dispatcher( this->port);

  if ( outdispatcher)
    (*propagate_fun)( propagate_context, outdispatcher, inlet);
}

static fts_status_t out_tilda_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int inlets = 0, i;

  ac--;
  at++;

  if ( !in_out_class_check( ac, at, &inlets))
    return &fts_CannotInstantiate;

  fts_class_init( cl, sizeof( out_tilda_t), inlets, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, out_tilda_init);

  for ( i = 0; i < inlets; i++)
    fts_dsp_declare_inlet( cl, i);

  fts_class_define_thru( cl, out_tilda_propagate_input);

  return fts_Success;
}

void in_out_tilda_config( void)
{
  fts_metaclass_install( fts_new_symbol( "in~"), in_tilda_instantiate, fts_never_equiv);
  fts_metaclass_install( fts_new_symbol( "out~"), out_tilda_instantiate, fts_never_equiv);
}
