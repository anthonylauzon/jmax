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
  public AlignMenu()
  {
    super("Align");
    setHorizontalTextPosition(AbstractButton.LEFT);

    add(Actions.alignTopAction,    "Top",    Event.META_MASK, KeyEvent.VK_UP);
    add(Actions.alignLeftAction,   "Left",   Event.META_MASK, KeyEvent.VK_LEFT);
    add(Actions.alignBottomAction, "Bottom", Event.META_MASK, KeyEvent.VK_DOWN);
    add(Actions.alignRightAction,  "Right",  Event.META_MASK, KeyEvent.VK_RIGHT);
  }
}
