package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

public class ResizeToMaxHeightAction extends PatcherAction
{
  ErmesSketchWindow editor;

  public ResizeToMaxHeightAction( ErmesSketchWindow editor)
  {
    super("Max Height", "Resize the selection to its Max Height",
	  Event.CTRL_MASK | Event.META_MASK, KeyEvent.VK_UP);

    this.editor = editor;
  }

  public  void actionPerformed(ActionEvent e)
  {
    if (ErmesSelection.patcherSelection.ownedBy(editor.itsSketchPad))
      ErmesSelection.patcherSelection.resizeToMaxHeight();
  }
}
