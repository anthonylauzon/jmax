package ircam.jmax.editors.patcher.menus;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.actions.*;

/** Convenience class that handle Action mnemonics following
  the PatcherAction conventions.
  For some strange reason, swing don't handle mnemonics in Actions,
  so we add a couple of tricks to do it here.
  */

abstract class PatcherMenu extends JMenu
{
  private ErmesSketchWindow editor;

  PatcherMenu(String name, ErmesSketchWindow editor)
  {
    super(name);

    this.editor = editor;
  }

  public final ErmesSketchWindow getEditor()
  {
    return editor;
  }

  public JMenuItem add(PatcherAction action)
  {
    JMenuItem item;

    item = super.add(action);

    if (action.haveMnemonic())
      {
	item.setAccelerator(KeyStroke.getKeyStroke(action.getMnemonic(), action.getModifiers()));
	item.setMnemonic(action.getMnemonic());
      }

    item.setToolTipText((String) action.getValue(Action.SHORT_DESCRIPTION));

    return item;
  }

  public JMenuItem insert(PatcherAction action, int pos) 
  {
    JMenuItem item;

    item = super.insert(action, pos);

    if (action.haveMnemonic())
      {
	item.setAccelerator(KeyStroke.getKeyStroke(action.getMnemonic(), action.getModifiers()));
	item.setMnemonic(action.getMnemonic());
      }

    item.setToolTipText((String) action.getValue(Action.SHORT_DESCRIPTION));

    return item;
  }
}
