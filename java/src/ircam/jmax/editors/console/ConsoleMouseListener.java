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

