package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.editors.patcher.*;

public class LockAction extends MenuAction
{
  public void doAction(ErmesSketchWindow editor)
  {
    editor.setLocked(! editor.isLocked());
  }
}
