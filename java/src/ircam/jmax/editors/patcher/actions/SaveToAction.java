package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.editors.patcher.*;

public class SaveToAction extends MenuAction
{
  public void doAction(ErmesSketchWindow editor)
  {
    editor.SaveTo();
  }
}
