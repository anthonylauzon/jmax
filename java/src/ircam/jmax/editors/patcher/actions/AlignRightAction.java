package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

public class AlignRightAction extends PatcherAction
{
  ErmesSketchWindow editor;

  public AlignRightAction( ErmesSketchWindow editor)
  {
    super("Right", "Align the selection to its Right", Event.META_MASK, KeyEvent.VK_RIGHT);
    this.editor = editor;
  }

  public  void actionPerformed(ActionEvent e)
  {
    if (ErmesSelection.patcherSelection.ownedBy(editor.itsSketchPad))
      ErmesSelection.patcherSelection.alignRight();
  }
}
