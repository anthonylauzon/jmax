//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//

package ircam.jmax.editors.patcher.interactions;

import java.awt.*;

import ircam.jmax.fts.*;
import ircam.jmax.dialogs.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

/** The interaction handling click select of a single connection.
  */


class ConnectionSelectInteraction extends Interaction
{
  void gotSqueack(ErmesSketchPad editor, int squeack, SensibilityArea area, Point mouse, Point oldMouse)
  {
    if (squeack == (Squeack.DOWN | Squeack.CONNECTION))
	{
	  GraphicConnection connection = (GraphicConnection) area.getTarget();

	  ErmesSelection.patcherSelection.redraw();
	  ErmesSelection.patcherSelection.deselectAll();
	  ErmesSelection.patcherSelection.select(connection);
	  connection.redraw();
	  editor.endInteraction();
	}
  }
}




