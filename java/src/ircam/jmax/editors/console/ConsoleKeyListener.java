/*
 * ConsoleKeyListener.java
 *
 * Copyright (c) 1997 Sun Microsystems, Inc.
 *
 *   This class implements the ConsoleKeyListener class.
 *
 * See the file "license.terms" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 * 
 * SCCS: @(#) ConsoleKeyListener.java 1.3 97/10/06 17:46:37
 */

package ircam.jmax.editors.console;

import java.awt.*;
import java.awt.event.*;

/**
 * The KeyListener object intercepts user keyboard inputs.
 * It stores the user's input in a buffer.
 * When the user presses return, the buffer is sent to the
 * AppletConsole object, which evaluates the user's input
 * in the main Tcl interpreter.
 */

public class ConsoleKeyListener implements KeyListener {
    public static final int DELETE = 177;
    public static final int BACK_SPACE = 8;
  /*private*/ public StringBuffer sbuf;
  public int intercept = 0;
  /*AppletConsole*/ Console console;

    public ConsoleKeyListener(/*AppletConsole*/Console c) {
	sbuf = new StringBuffer();
	console = c;
    }

    public void keyPressed(KeyEvent evt) {
    }

    public void keyReleased(KeyEvent evt) {
    }

    public void keyTyped(KeyEvent evt) {
      intercept = console.getTextArea().getText().length();
      if(evt.isControlDown()){
	console.itsContainer.keyPressed(evt);
	return;
      }
      char key = evt.getKeyChar();
      
      if ((key == '\r') || (key == '\n')) {
	/*
	 * Enter and return notify the console that there is a new
	 * line to process and empties the buffer.
	 */
	
	sbuf.append(key);
	console.LineReadyNotify(sbuf.toString());
	sbuf.setLength(0);
      } else if (((int) key == BACK_SPACE) || ((int) key ==  DELETE)) {
	/*
	 * Delete and backspace erase the last character from both
	 * the screen and the buffer.
	 */
	
	int len = sbuf.length();
	if (len > 0) {
	  sbuf.setLength(len-1);
	}
      } else {
	/*
	 * All other keys are appended to the buffer.
	 */
	
	sbuf.append(key);
      }
    }
}










