package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.editors.patcher.*;

public class StatisticsAction extends PatcherAction
{
  ErmesSketchWindow editor;

  public StatisticsAction( ErmesSketchWindow editor)
  {
    super("Statistics", "Statistics a Patcher");
    this.editor = editor;
  }

  public  void actionPerformed(ActionEvent e)
  {
    new StatisticsDialog(editor);
  }
}
