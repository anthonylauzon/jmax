package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

public class AlignLeftAction extends PatcherAction
{
  ErmesSketchWindow editor;

  public AlignLeftAction( ErmesSketchWindow editor)
  {
    super("Left", "Align the selection to its Left", Event.META_MASK, KeyEvent.VK_LEFT);
    this.editor = editor;
  }

  public  void actionPerformed(ActionEvent e)
  {
    if (ErmesSelection.patcherSelection.ownedBy(editor.itsSketchPad))
      ErmesSelection.patcherSelection.alignLeft();
  }
}
