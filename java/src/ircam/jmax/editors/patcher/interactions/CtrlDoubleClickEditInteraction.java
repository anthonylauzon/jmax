package ircam.jmax.editors.patcher.interactions;

import java.awt.*;

import ircam.jmax.fts.*;
import ircam.jmax.dialogs.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

/** The interaction handling DoubleClick Edit in objects;
  started, and completed, by a AltClick on an object.
  */


class CtrlDoubleClickEditInteraction extends Interaction
{
  void gotSqueack(ErmesSketchPad editor, int squeack, DisplayObject dobject, Point mouse, Point oldMouse)
  {
    if (Squeack.onObject(squeack))
	{
	  ErmesObject object = (ErmesObject) dobject;
	  
	  object.editContent();

	  editor.endInteraction();
	}
    else if (Squeack.onText(squeack))
      {
	{
	  TextSensibilityArea area = (TextSensibilityArea) dobject;
	  
	  area.getObject().editContent();

	  editor.endInteraction();
	}
      }
  }
}




