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
  boolean consumeNext = false;
  
  /**
   * The default constructor eats KeyEvent.VK_CONTROL codes*/
  public KeyConsumer() 
  {
    consumeCode = KeyEvent.VK_CONTROL;
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
  int consumeCode;
}


