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

#include <fts/fts.h>

static int in_out_check( fts_object_t *o, int ac, const fts_atom_t *at, fts_audioport_t **port, int *channel)
{
  *port = 0;
  *channel = 0;

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
	  fts_object_set_error( o, "default audio port is not defined");
	  return 0;
	}
    }

  if ( !fts_object_is_audioport( (fts_object_t *)*port) )
    {
      fts_object_set_error( o, "argument must be an audio port");
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

  if (in_out_check( o, ac, at, &this->port, &this->channel))
    fts_audioport_add_input_object( this->port, this->channel, (fts_object_t *)this);
}

static void in_tilda_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  in_tilda_t *this = (in_tilda_t *)o;

  if(this->port)
    {
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
}

static void in_tilda_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof( fts_object_t), in_tilda_init, in_tilda_delete);

  fts_dsp_declare_outlet( cl, 0);
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

  in_out_check( o, ac, at, &this->port, &this->channel);
}

static void out_tilda_propagate_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  out_tilda_t *this  = (out_tilda_t *)o;
  fts_propagate_fun_t propagate_fun = (fts_propagate_fun_t)fts_get_pointer(at + 0);
  void *propagate_context = fts_get_pointer(at + 1);
  fts_object_t *outdispatcher = fts_audioport_get_output_dispatcher( this->port);

  if ( outdispatcher)
    (*propagate_fun)( propagate_context, outdispatcher, this->channel);
}

static void out_tilda_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof( out_tilda_t), out_tilda_init, 0);

  fts_class_message_varargs(cl, fts_s_propagate_input, out_tilda_propagate_input);

  fts_dsp_declare_inlet( cl, 0);
}

void in_out_tilda_config( void)
{
  fts_class_install( fts_new_symbol( "in~"), in_tilda_instantiate);
  fts_class_install( fts_new_symbol( "out~"), out_tilda_instantiate);
}
