package ircam.jmax.editors.patcher.interactions;

import java.awt.*;

import ircam.jmax.fts.*;
import ircam.jmax.dialogs.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

/** The interaction handling help patches; 
  started, and completed, by a AltClick on an object.
  */


class BackgroundPopUpInteraction extends Interaction
{
  void gotSqueack(ErmesSketchPad editor, int squeack, SensibilityArea area, Point mouse, Point oldMouse)
  {
    //Warning: the order is important, because the AddPopUp
    // menu will indirectly invoke an other top level
    // interaction mode.

    editor.endInteraction();
    editor.showAddPopUp(mouse);
  }
}




