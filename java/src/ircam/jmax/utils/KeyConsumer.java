//
// jMax
// Copyright (C) 1999 by IRCAM
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
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.utils;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.text.*;
import javax.swing.event.*;

  // Key Event Listener for Edit Field
  // Horrible hack; it seems that there is bug in our
  // version of Java/AWT and or Swwing, so that the event sequence is not
  // correct, and we need to "filter it"; Pressing Ctrl-E generate:
  //
  // Pressed VK_CONTROL
  // Typed   'e'
  // Pressed Ctrl-E
  // 
  // .. and no released events; this is completely inconsistent with
  // what the doc say !!!

/**
 * An utility class to consume unwanted key codes generated in a JTextArea.
 * Instruction: create a KeyConsumer with the code to consume (no arguments 
 * consumes the VK_CONTROL codes) and attach it to your editable object */
public class KeyConsumer implements KeyListener
{
  
  /**
   * The default constructor eats KeyEvent.VK_CONTROL codes*/
  public KeyConsumer() 
  {
    consumeCode = KeyEvent.VK_CONTROL;
  }

  /**
   * returns a static object that is able to eat VK_CONTROL keys */
  public static KeyConsumer controlConsumer()
  {
    return ctrlConsumer;
  }

  /**
   * Generic constructor: pass the code of the key to consume */
  public KeyConsumer(int consumeKey)
  {
    this.consumeCode = consumeKey;
  }

  public void keyTyped(KeyEvent e)
  {
    if (consumeNext)
      {
	e.consume();
	consumeNext = false;
      }
  }
  
    public void keyPressed(KeyEvent e)
  {
    if (e.getKeyCode() == consumeCode)
      consumeNext = true;
  }
  
  public  void keyReleased(KeyEvent e)
  {
  }

  //---
  boolean consumeNext = false;
  int consumeCode;
  static KeyConsumer ctrlConsumer = new KeyConsumer();
}


