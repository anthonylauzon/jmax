package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.editors.patcher.*;

public class LockAction extends PatcherAction
{
  ErmesSketchWindow editor;

  public LockAction( ErmesSketchWindow editor)
  {
    super("Lock", "Lock/Unlock the editor", Event.CTRL_MASK, KeyEvent.VK_E);
    this.editor = editor;
  }

  public void actionPerformed(ActionEvent e)
  {
    editor.setLocked(! editor.isLocked());
  }
}
