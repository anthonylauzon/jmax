package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.editors.patcher.*;

public class PrintAction extends PatcherAction
{
  ErmesSketchWindow editor;

  public PrintAction( ErmesSketchWindow editor)
  {
    super("Print", "Print a Patcher", Event.CTRL_MASK, KeyEvent.VK_P);
    this.editor = editor;
  }

  public  void actionPerformed(ActionEvent e)
  {
    editor.Print();
  }
}
