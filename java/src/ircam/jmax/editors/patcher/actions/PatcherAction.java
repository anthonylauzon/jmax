package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

/** An utility class that provide a constructor to add
  a Mnemonic property to the action; this Mnemonic property
  is automatically used in the KeyMap and in the PatcherMenu
  to bind the key and show the mnemonic
  */

abstract public class PatcherAction extends AbstractAction
{
  int    mnemonic;
  int    modifiers;

  boolean doMnemonic;

  public PatcherAction(String name, String description, int modifiers, int mnemonic)
  {
    super(name);

    putValue(Action.SHORT_DESCRIPTION, description);

    doMnemonic = true;
    this.mnemonic = mnemonic;
    this.modifiers = modifiers;
  }

  public PatcherAction(String name, String description)
  {
    super(name);
    putValue(Action.SHORT_DESCRIPTION, description);
    doMnemonic = false;
  }

  public final boolean haveMnemonic()
  {
    return doMnemonic;
  }

  public final int getMnemonic()
  {
    return mnemonic;
  }

  public final int getModifiers()
  {
    return modifiers;
  }
}


