package ircam.jmax.editors.patcher.menus;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.utils.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.actions.*;

/** Implement the patcher editor File Menu */

public class AlignMenu extends PatcherMenu
{
  public AlignMenu(ErmesSketchWindow window)
  {
    super("Align", window);
    setHorizontalTextPosition(AbstractButton.LEFT);

    add(new AlignTopAction(getEditor()));
    add(new AlignLeftAction(getEditor()));
    add(new AlignBottomAction(getEditor()));
    add(new AlignRightAction(getEditor()));
  }
}
