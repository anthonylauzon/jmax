package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

public class AlignBottomAction extends PatcherAction
{
  ErmesSketchWindow editor;

  public AlignBottomAction( ErmesSketchWindow editor)
  {
    super("Bottom", "Align the selection to its Bottom", Event.META_MASK, KeyEvent.VK_DOWN);
    this.editor = editor;
  }

  public  void actionPerformed(ActionEvent e)
  {
    if (ErmesSelection.patcherSelection.ownedBy(editor.itsSketchPad))
      ErmesSelection.patcherSelection.alignBottom();
  }
}
