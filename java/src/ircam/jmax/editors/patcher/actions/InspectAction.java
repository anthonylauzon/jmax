package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

public class InspectAction extends MenuAction
{
  public void doAction(ErmesSketchWindow editor)
  {
    if (ErmesSelection.patcherSelection.ownedBy(editor.itsSketchPad))
      {
	ErmesSelection.patcherSelection.apply(new ObjectAction() {
	  public void processObject(GraphicObject object)
	    { object.inspect();}});
      }
  }
}
