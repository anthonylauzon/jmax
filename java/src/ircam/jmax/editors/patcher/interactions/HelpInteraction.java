package ircam.jmax.editors.patcher.interactions;

import java.awt.*;

import ircam.jmax.fts.*;
import ircam.jmax.dialogs.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

/** The interaction handling help patches; 
  started, and completed, by a AltClick on an object.
  */


class HelpInteraction extends Interaction
{
  boolean locked = false;

  void gotSqueack(ErmesSketchPad editor, int squeack, SensibilityArea area, Point mouse, Point oldMouse)
  {
    GraphicObject object = null;

    /* Please notes that since the ErrorDialog is modal, this interaction
       will continue to receive events, and will not execute the "endInteraction"
       until the OK button is pressed; that is why we have the locked flag; also,
       it is happening in the same thread (the show method of the error dialog)
       so there is nothing to do using synchronized and so on; we just need to ignore
       events different than the first one.
       */

    if ((! locked) && Squeack.isDown(squeack))
      {
	locked = true;

	object = (GraphicObject) area.getTarget();

	if (object != null)
	  if (! FtsHelpPatchTable.openHelpPatch( object.getFtsObject()))
	    new ErrorDialog( editor.getSketchWindow(),
			     "Sorry, no help for object " + object.getFtsObject().getClassName());

	locked = false;
	editor.endInteraction();
      }
  }
}




