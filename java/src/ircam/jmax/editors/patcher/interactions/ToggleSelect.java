package ircam.jmax.editors.patcher.interactions;

import java.awt.*;

import ircam.jmax.fts.*;
import ircam.jmax.dialogs.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

/** The interaction handling help patches; 
  started, and completed, by a AltClick on an object.
  */


class HelpInteraction extends SubInteraction
{
  HelpInteraction(InteractionEngine engine, Interaction master)
  {
    super(engine, master);
  }


  void gotSqueack(int squeack, DisplayObject dobject, Point mouse, Point oldMouse)
  {
    if (squeack == (Squeack.SHIFT | Squeack.DOWN | Squeack.OBJECT))
	{
	  ErmesObject object = (ErmesObject) dobject;

	  if (object.isSelected())
	    engine.getDisplayList().deselect(object);
	  else
	    engine.getDisplayList().select(object);
	  object.redraw();
	  end();
	}
  }
}




