//
// jMax
// Copyright (C) 1999 by IRCAM
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
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.editors.patcher.interactions;

import java.awt.*;


import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

/** A convenience abstract super class for all the interactions.
  Provide empty definition for all the lexical event methods,
  and a convenience variable to store the InteractionEngine.
  */


abstract class Interaction
{
  // This method configure the input filter
  // for this interaction; is called by the interaction engine.
  // by default, deinstall the move and the location handling

  void configureInputFilter(InteractionEngine filter)
  {
  }

  // This method is also called by the interaction engine
  // to reset the state of the interaction; by default,
  // it do nothing

  void reset()
  {
  }

  // Called for every squeack

  void gotSqueack(ErmesSketchPad editor, int squeack, SensibilityArea area, Point mouse, Point oldMouse)
  {
  }

  Interaction delegateSqueack(ErmesSketchPad editor, int squeack, SensibilityArea area, Point mouse, Point oldMouse)
  {
    return null;
  }
}
