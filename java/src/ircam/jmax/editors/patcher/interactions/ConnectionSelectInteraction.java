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
  void gotSqueack(ErmesSketchPad editor, int squeack, DisplayObject dobject, Point mouse, Point oldMouse)
  {
    if (squeack == (Squeack.DOWN | Squeack.CONNECTION))
	{
	  ErmesConnection connection = (ErmesConnection) dobject;

	  ErmesSelection.patcherSelection.redraw();
	  ErmesSelection.patcherSelection.deselectAll();
	  ErmesSelection.patcherSelection.select(connection);
	  connection.redraw();
	  editor.endInteraction();
	}
  }
}




