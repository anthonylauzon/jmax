//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
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

/*
 * This MouseListener widget voids user mouse activity.
 */

class ConsoleMouseListener implements MouseListener {
  Console itsConsole;
  
  public ConsoleMouseListener(Console theConsole) {
  itsConsole = theConsole;
  }

  public void mousePressed(MouseEvent evt) {
  }
  
  public void mouseReleased(MouseEvent evt) {
  }
  /* WARNING:  Not yot working.
   * We want the mouse clicks to always place the cursor at the end of
   * of the text buffer.
   */
  
  public void mouseClicked(MouseEvent evt) {
    /**
     * try to implement the "2nd mouse button behaviour" to copy/paste
     * text into the console.
     */
    //get the text from the console's keyListener string buffer
    //get the text from the console's edit field
    //check if the length of the editField is lower then the "intercept"
    //field (this means that some text have been added without typing)
    // if the edit field contains more text, change the content
    // of the buffer
    StringBuffer lineBuffer = itsConsole.itsKeyListener.sbuf;
    StringBuffer editBuffer = new StringBuffer(itsConsole.getTextArea().getText());
    
    if (itsConsole.getTextArea().getText().length() > itsConsole.itsKeyListener.intercept) {

      String tutto = itsConsole.getTextArea().getText();
      int intercept = itsConsole.itsKeyListener.intercept;
      //String buff = itsConsole.itsKeyListener.sbuf;
      String daAggiungere = tutto.substring(intercept, tutto.length());
      itsConsole.itsKeyListener.sbuf.append(daAggiungere);
	}
  
  }

  public void mouseEntered(MouseEvent evt) {
  }
  
  public void mouseExited(MouseEvent evt) {
  }
}

