//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//

package ircam.jmax.editors.patcher.menus;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.actions.*;


/** Implement the patcher editor File Menu */

public class FileMenu extends PatcherMenu
{
  public FileMenu()
  {
    super("File");

    setHorizontalTextPosition(AbstractButton.LEFT);

    add(Actions.newAction, "New", Event.CTRL_MASK, KeyEvent.VK_N);
    add(Actions.openAction, "Open", Event.CTRL_MASK, KeyEvent.VK_O);

    addSeparator();

    add(Actions.saveAction, "Save", Event.CTRL_MASK, KeyEvent.VK_S);
    add(Actions.saveAsAction, "SaveAs");
    add(Actions.saveToAction, "SaveTo");
    add(Actions.closeAction, "Close", Event.CTRL_MASK, KeyEvent.VK_W);

    addSeparator();

    add(Actions.printAction, "Print", Event.CTRL_MASK, KeyEvent.VK_P);
    add(Actions.statisticsAction, "Statistics");
    add(Actions.quitAction, "Quit", Event.CTRL_MASK, KeyEvent.VK_Q);
  }
}

