package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.editors.patcher.*;

public class CloseAction extends PatcherAction
{
  ErmesSketchWindow editor;

  public CloseAction( ErmesSketchWindow editor)
  {
    super("Close", "Close a Patcher", Event.CTRL_MASK, KeyEvent.VK_W);
    this.editor = editor;
  }

  public  void actionPerformed(ActionEvent e)
  {
    editor.Close(true);
  }
}
