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
  public void doAction(ErmesSketchWindow editor)
  {
    if (ErmesSelection.patcherSelection.isSingleton())
      {
	ErmesSelection.patcherSelection.apply(new ObjectAction() {
	  public void processObject(ErmesObject object)
	    { 
	      FindPanel.open().findFriends(object.getFtsObject());
	    }});
      }
    else
      FindPanel.open();
  }
}
