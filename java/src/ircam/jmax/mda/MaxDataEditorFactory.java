//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.mda;

/**
 * Factory for editors.
 * A MaxDataEditorFactory is an object able to generate an empty, unbound editor
 * for a given type; each data type have its own default editor 
 * factory, but editors can be also instantiated in other ways.
 */

public interface MaxDataEditorFactory
{
  /** Check if this factory can produce a editor for the given data */

  abstract public boolean       canEdit(MaxData data);

  /** Ask the factory to  produce a new editor for the given data */

  abstract public MaxDataEditor newEditor(MaxData data);
}

  
