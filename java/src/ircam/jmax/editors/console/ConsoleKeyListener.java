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

class ConsoleKeyListener implements KeyListener {
  public static final int DELETE = 127;
  public static final int BACK_SPACE = 8;
  public StringBuffer sbuf;
  public int intercept = 0;
  Console console;
  boolean itsControlDown = false;

    public ConsoleKeyListener(/*AppletConsole*/Console c) {
	sbuf = new StringBuffer();
	console = c;
    }

    public void keyPressed(KeyEvent evt) {
      if(evt.isControlDown()){
	console.getTextArea().setCaretPosition(console.getTextArea().getText().length());
	itsControlDown = true;
	return;
      }
    }

    public void keyReleased(KeyEvent evt) {
    }

    public void keyTyped(KeyEvent evt) {
      intercept = console.getTextArea().getText().length();
      if(itsControlDown){
	//console.itsContainer.keyPressed(evt);
	String aText = console.getTextArea().getText();
	console.getTextArea().setText(aText.substring(0, aText.length()-1));
	itsControlDown = false;
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

	//qui  posiziona il cursore in fondo alla stringa
	console.getTextArea().setCaretPosition(console.getTextArea().getText().length());

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










