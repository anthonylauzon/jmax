package ircam.jmax.editors.patcher.interactions;

import java.awt.*;

import ircam.jmax.fts.*;
import ircam.jmax.dialogs.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

/** The interaction handling DoubleClick Edit in objects;
  started, and completed, by a AltClick on an object.
  */


class DoubleClickEditInteraction extends SubInteraction
{
  DoubleClickEditInteraction(InteractionEngine engine, Interaction master)
  {
    super(engine, master);
  }


  void gotSqueack(int squeack, DisplayObject dobject, Point mouse, Point oldMouse)
  {
    if (squeack == (Squeack.DOUBLE_CLICK | Squeack.OBJECT))
	{
	  ErmesObject object = (ErmesObject) dobject;
	  
	  object.editContent();

	  end();
	}
  }
}




