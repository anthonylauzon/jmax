package ircam.jmax.editors.patcher.interactions;

import java.awt.*;

import ircam.jmax.fts.*;
import ircam.jmax.dialogs.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

/** The interaction handling DoubleClick Edit in objects;
  started, and completed, by a AltClick on an object.
  */


class  DoubleClickEditInteraction extends Interaction
{
  void gotSqueack(ErmesSketchPad editor, int squeack, SensibilityArea area, Point mouse, Point oldMouse)
  {
    ErmesObject object = (ErmesObject) area.getTarget();

    object.editContent();
    editor.endInteraction();
  }
}




