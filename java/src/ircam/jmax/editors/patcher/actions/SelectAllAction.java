package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.editors.patcher.*;

public class SelectAllAction extends MenuAction
{
  public void doAction(ErmesSketchWindow editor)
  {
    if (! editor.isLocked())
      {
	ErmesSelection.patcherSelection.setOwner(editor.itsSketchPad); 
	ErmesSelection.patcherSelection.deselectAll();
	editor.itsSketchPad.getDisplayList().selectAll();
	editor.itsSketchPad.redraw();
      }
  }
}
