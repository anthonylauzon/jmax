package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.editors.patcher.*;

public class DuplicateAction extends PatcherAction
{
  ErmesSketchWindow editor;

  public DuplicateAction( ErmesSketchWindow editor)
  {
    super("Duplicate", "Duplicate the selection", Event.CTRL_MASK, KeyEvent.VK_D);
    this.editor = editor;
  }

  public  void actionPerformed(ActionEvent e)
  {
    editor.Duplicate();
  }
}
