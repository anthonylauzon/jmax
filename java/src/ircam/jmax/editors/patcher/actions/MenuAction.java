package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.editors.patcher.*;

/** An utility class that provide a constructor to make
  actions suitable to be put in an ErmesSketchWindow menu.
  */

abstract public class MenuAction implements ActionListener
{
  public MenuAction()
  {
  }

  public  void actionPerformed(ActionEvent e)
  {
    Component        invoker;
    ErmesSketchWindow editor;

    invoker = ((JPopupMenu) ((JMenuItem)e.getSource()).getParent()).getInvoker();

    editor = (ErmesSketchWindow) SwingUtilities.getAncestorOfClass(ErmesSketchWindow.class,
								   invoker);
    doAction(editor);
  }

  abstract public void doAction(ErmesSketchWindow editor);
}


