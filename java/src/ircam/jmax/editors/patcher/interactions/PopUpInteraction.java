package ircam.jmax.editors.patcher.interactions;

import java.awt.*;

import ircam.jmax.fts.*;
import ircam.jmax.dialogs.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

/** The interaction handling help patches; 
  started, and completed, by a AltClick on an object.
  */


class PopUpInteraction extends Interaction
{
  boolean locked = false;

  void gotSqueack(ErmesSketchPad editor, int squeack, SensibilityArea area, Point mouse, Point oldMouse)
  {
    ErmesObject object = null;

    if ((! locked) && Squeack.isPopUp(squeack))
      {
	locked = true;
	object = (ErmesObject) area.getTarget();
	object.popUpEdit(mouse);
	locked = false;
	editor.endInteraction();
      }
  }
}




