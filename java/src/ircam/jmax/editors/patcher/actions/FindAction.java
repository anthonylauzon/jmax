package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

public class FindAction extends MenuAction
{
  ErmesSketchWindow editor;

  public void doAction(ErmesSketchWindow e)
  {
    editor = e;

    if (ErmesSelection.patcherSelection.isSingleton())
      {
	ErmesSelection.patcherSelection.apply(new ObjectAction() {
	  public void processObject(GraphicObject object)
	    { 
	      FindPanel.open(editor.getFts()).findFriends(object.getFtsObject());
	    }});
      }
    else
      FindPanel.open(editor.getFts());
  }
}
