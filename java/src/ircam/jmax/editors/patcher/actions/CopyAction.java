package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.editors.patcher.*;

public class CopyAction extends PatcherAction
{
  ErmesSketchWindow editor;

  public CopyAction( ErmesSketchWindow editor)
  {
    super("Copy", "Copy the selection", Event.CTRL_MASK, KeyEvent.VK_C);
    this.editor = editor;
  }

  public  void actionPerformed(ActionEvent e)
  {
    editor.Copy();
  }
}
