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

static void in_tilda_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_audioport_t *port;
  int i, outlets;

  ac--;
  at++;

  if ( ! (ac > 0 && fts_is_object( at) && fts_object_is_audioport( fts_get_object( at))) )
    {
      fts_object_set_error( o, "Argument must be an audioport");
      return;
    }

  port = (fts_audioport_t *)fts_get_object( at);

  outlets = fts_object_get_inlets_number( o);

  for ( i = 0; i < outlets; i++)
    fts_audioport_get_in_object( port, o, i);
}

static fts_status_t in_tilda_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int outlets, i;
  fts_audioport_t *port;

  ac--;
  at++;

  /* CHECK ARGS */
  port = (fts_audioport_t *)fts_get_ptr( at);
  outlets = fts_audioport_get_input_channels( port);
  fts_class_init( cl, sizeof( fts_object_t), 0, outlets, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, in_tilda_init);

  for ( i = 0; i < outlets; i++)
    fts_dsp_declare_outlet( cl, i);

  return fts_Success;
}


void in_tilda_config( void)
{
  fts_metaclass_install( fts_new_symbol( "in~"), in_tilda_instantiate, fts_never_equiv);
}
