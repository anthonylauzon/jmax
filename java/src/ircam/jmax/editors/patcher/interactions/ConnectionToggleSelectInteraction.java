package ircam.jmax.editors.patcher.interactions;

import java.awt.*;

import ircam.jmax.fts.*;
import ircam.jmax.dialogs.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

/** The interaction handling shift click toggle select of a single connection.
  */


class ConnectionToggleSelectInteraction extends Interaction
{
  void gotSqueack(ErmesSketchPad editor, int squeack, SensibilityArea area, Point mouse, Point oldMouse)
  {
    if (squeack == (Squeack.SHIFT | Squeack.DOWN | Squeack.CONNECTION))
	{
	  GraphicConnection connection = (GraphicConnection) area.getTarget();

	  if (connection.isSelected())
	    ErmesSelection.patcherSelection.deselect(connection);
	  else
	    ErmesSelection.patcherSelection.select(connection);

	  connection.redraw();
	  editor.endInteraction();
	}
  }
}




