package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.editors.patcher.*;

public class CutAction extends PatcherAction
{
  ErmesSketchWindow editor;

  public CutAction( ErmesSketchWindow editor)
  {
    super("Cut", "Cut the selection", Event.CTRL_MASK, KeyEvent.VK_X);
    this.editor = editor;
  }

  public  void actionPerformed(ActionEvent e)
  {
    editor.Cut();
  }
}
