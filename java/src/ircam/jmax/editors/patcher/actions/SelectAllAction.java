package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.editors.patcher.*;

public class SelectAllAction extends PatcherAction
{
  ErmesSketchWindow editor;

  public SelectAllAction( ErmesSketchWindow editor)
  {
    super("SelectAll", "Select All", Event.CTRL_MASK, KeyEvent.VK_A);
    this.editor = editor;
  }

  public  void actionPerformed(ActionEvent e)
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
