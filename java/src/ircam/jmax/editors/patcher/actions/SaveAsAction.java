package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.editors.patcher.*;

public class SaveAsAction extends PatcherAction
{
  ErmesSketchWindow editor;

  public SaveAsAction( ErmesSketchWindow editor)
  {
    super("SaveAs", "Save a Patcher");
    this.editor = editor;
  }

  public  void actionPerformed(ActionEvent e)
  {
    editor.SaveAs();
  }
}
