package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.editors.patcher.*;

public class SaveToAction extends PatcherAction
{
  ErmesSketchWindow editor;

  public SaveToAction( ErmesSketchWindow editor)
  {
    super("SaveTo", "Save a Copy to a new File");
    this.editor = editor;
  }

  public  void actionPerformed(ActionEvent e)
  {
    editor.SaveTo();
  }
}
