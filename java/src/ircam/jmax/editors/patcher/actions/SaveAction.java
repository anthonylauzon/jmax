package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.editors.patcher.*;

public class SaveAction extends PatcherAction
{
  ErmesSketchWindow editor;

  public SaveAction( ErmesSketchWindow editor)
  {
    super("Save", "Save a Patcher", Event.CTRL_MASK, KeyEvent.VK_S);
    this.editor = editor;
  }

  public  void actionPerformed(ActionEvent e)
  {
    editor.Save();
  }
}
