package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

public class ResizeToMaxWidthAction extends PatcherAction
{
  ErmesSketchWindow editor;

  public ResizeToMaxWidthAction( ErmesSketchWindow editor)
  {
    super("Max Width", "Resize the selection to its Max Width",
	  Event.CTRL_MASK | Event.META_MASK, KeyEvent.VK_RIGHT);

    this.editor = editor;
  }

  public  void actionPerformed(ActionEvent e)
  {
    if (ErmesSelection.patcherSelection.ownedBy(editor.itsSketchPad))
      ErmesSelection.patcherSelection.resizeToMaxWidth();
  }
}
