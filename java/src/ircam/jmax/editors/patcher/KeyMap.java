//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.editors.patcher;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.utils.*;
import ircam.jmax.editors.patcher.actions.*;

/** Implement the key handling for the sketchpad, using the registerKeyboardAction
    mechanism; note that this can be easily extend for adding scripts commands.

    Actually, should be splitted in two classes, one generic and one specific.
    */


class KeyMap
{
  JComponent  component;
  
  KeyMap(JComponent component)
  {
    // Set local variables

    this.component = component;
    // Install the keyboard actions

    // Move actions

    add(Actions.moveTenLeftAction, Event.SHIFT_MASK, KeyEvent.VK_LEFT);
    add(Actions.moveTenRightAction, Event.SHIFT_MASK, KeyEvent.VK_RIGHT);
    add(Actions.moveTenUpAction, Event.SHIFT_MASK, KeyEvent.VK_UP);
    add(Actions.moveTenDownAction, Event.SHIFT_MASK, KeyEvent.VK_DOWN);

    add(Actions.moveLeftAction, 0, KeyEvent.VK_LEFT);
    add(Actions.moveRightAction, 0, KeyEvent.VK_RIGHT);
    add(Actions.moveUpAction, 0, KeyEvent.VK_UP);
    add(Actions.moveDownAction, 0, KeyEvent.VK_DOWN);

    // Fixed Resize Actions

    add(Actions.resizeTenLeftAction,  Event./*CTRL_MASK*/META_MASK | Event.SHIFT_MASK, KeyEvent.VK_LEFT);
    add(Actions.resizeTenRightAction, Event./*CTRL_MASK*/META_MASK | Event.SHIFT_MASK, KeyEvent.VK_RIGHT);
    add(Actions.resizeTenUpAction,    Event./*CTRL_MASK*/META_MASK | Event.SHIFT_MASK, KeyEvent.VK_UP);
    add(Actions.resizeTenDownAction,  Event./*CTRL_MASK*/META_MASK | Event.SHIFT_MASK, KeyEvent.VK_DOWN);

    add(Actions.resizeLeftAction,  Event./*CTRL_MASK*/META_MASK, KeyEvent.VK_LEFT);
    add(Actions.resizeRightAction, Event./*CTRL_MASK*/META_MASK, KeyEvent.VK_RIGHT);
    add(Actions.resizeUpAction,    Event./*CTRL_MASK*/META_MASK, KeyEvent.VK_UP);
    add(Actions.resizeDownAction,  Event./*CTRL_MASK*/META_MASK, KeyEvent.VK_DOWN);

    // Resize Align operations

    add(Actions.resizeToMaxWidthAction, Event.CTRL_MASK | Event.META_MASK, KeyEvent.VK_RIGHT);
    add(Actions.resizeToMaxHeightAction, Event.CTRL_MASK | Event.META_MASK, KeyEvent.VK_UP);

    // Delete selection

    add(Actions.deleteSelectionAction, 0, KeyEvent.VK_DELETE);
    add(Actions.deleteSelectionAction, 0, KeyEvent.VK_BACK_SPACE);

    // Annotation 

    add(Actions.showErrorAction, Event.CTRL_MASK, KeyEvent.VK_Z);
  }


  /** Add a Action to the keymap */

  public void add(Action action, int modifiers, int mnemonic)
  {
    add(action, modifiers, mnemonic, JComponent.WHEN_FOCUSED);
  }

  public void add(Action action, int modifiers, int mnemonic, int condition)
  {
    component.registerKeyboardAction(action, 
				     KeyStroke.getKeyStroke(mnemonic, modifiers),
				     condition);
  }
}










