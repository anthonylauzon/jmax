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
  void gotSqueack(ErmesSketchPad editor, int squeack, DisplayObject dobject, Point mouse, Point oldMouse)
  {
    ErmesObject object = null;

    if (Squeack.onObject(squeack))
      object = (ErmesObject) dobject;
    else if (Squeack.onText(squeack))
      object = ((TextSensibilityArea) dobject).getObject();

    if (object != null)
      if (! FtsHelpPatchTable.openHelpPatch( object.getFtsObject()))
	new ErrorDialog( editor.getSketchWindow(),
			 "Sorry, no help for object " + object.getFtsObject().getClassName());

    editor.endInteraction();
  }
}




