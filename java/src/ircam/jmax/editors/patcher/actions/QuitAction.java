package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.editors.patcher.*;

public class QuitAction extends PatcherAction
{
  ErmesSketchWindow editor;

  public QuitAction( ErmesSketchWindow editor)
  {
    super("Quit", "Quit a Patcher", Event.CTRL_MASK, KeyEvent.VK_Q);
    this.editor = editor;
  }

  public  void actionPerformed(ActionEvent e)
  {
    MaxApplication.Quit();
  }
}
