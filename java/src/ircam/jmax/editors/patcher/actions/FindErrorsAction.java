package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.editors.patcher.*;

public class FindErrorsAction extends PatcherAction
{
  ErmesSketchWindow editor;

  public FindErrorsAction( ErmesSketchWindow editor)
  {
    super("Find Errors", "Find the objects with errors");
    this.editor = editor;
  }

  public  void actionPerformed(ActionEvent e)
  {
    FindPanel.open().findErrors();
  }
}
