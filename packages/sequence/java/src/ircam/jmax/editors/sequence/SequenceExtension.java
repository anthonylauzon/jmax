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

package ircam.jmax.editors.sequence;

import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import ircam.jmax.script.pkg.*;
import ircam.jmax.script.*;
/**
 * The sequence extension; install the sequence data type,
 * register the remote data class, installs
 * the sequence editor factory
 */
public class SequenceExtension extends tcl.lang.Extension implements JavaExtension
{
 public void init(ircam.jmax.script.Interpreter interp)
  {
    Mda.installEditorFactory( new SequenceFactory());

    Fts.registerRemoteDataClass( "sequence_data", ircam.jmax.editors.sequence.SequenceRemoteData.class);
  }

    /* this method should be removed as soon as jacl is completely forgotten about */
  public void init(tcl.lang.Interp interp)
  {
    Mda.installEditorFactory( new SequenceFactory());

    Fts.registerRemoteDataClass( "sequence_data", ircam.jmax.editors.sequence.SequenceRemoteData.class);
  }
}



