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

package ircam.jmax.dialogs;

import java.awt.*;
import java.awt.event.*;
import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.utils.*;

/**
 * A generic "file not saved" dialog.
 */
public class FileNotSavedDialog extends Dialog implements ActionListener, KeyListener{
  String itsMessage;
  Button itsSaveButton, itsCancelButton, itsNoButton;
  boolean itsToSave = true;
  boolean itsNothingToDo = false;

  public FileNotSavedDialog(Frame theFrame, MaxDocument theDocument) {
    this(theFrame, theDocument, true);
  }

  public FileNotSavedDialog(Frame theFrame, MaxDocument theDocument, boolean doCancel)
  {
    super(theFrame, "File Not Saved Message", true);

    if (theDocument.getDocumentFile() != null)
      itsMessage = "File " + theDocument.getDocumentFile() + " is not saved.\n Do you want to save it now?";
    else {
      if(theDocument.getName()!=null)   
	itsMessage = ("Patch " + theDocument.getName() + " is not saved.\n Do you want to save it now?");
      else
	itsMessage = ("Patch " + theFrame.getTitle() + " is not saved.\n Do you want to save it now?");
    }
    setLayout(new BorderLayout());
    
    //Create middle section.
    Panel p1 = new Panel();
    Label label = new Label(itsMessage);
    p1.add(label);
    add("Center", p1);
    
    //Create bottom row.
    Panel p2 = new Panel();
    // p2.setLayout(new BorderLayout());
    p2.setLayout(new GridLayout(1,3));
    itsSaveButton = new Button("Save");
    itsSaveButton.setBackground(Color.white);
    itsSaveButton.addActionListener(this);
    //p2.add("East", itsSaveButton);
    p2.add(itsSaveButton);
    itsNoButton = new Button("Don't Save");
    itsNoButton.setBackground(Color.white);
    itsNoButton.addActionListener(this);

    //p2.add("Center", itsNoButton);
    p2.add(itsNoButton);
    
    if (doCancel)
      {
	itsCancelButton = new Button("Cancel");
	itsCancelButton.setBackground(Color.white);
	itsCancelButton.addActionListener(this);
	//p2.add("West", itsCancelButton);
	p2.add(itsCancelButton);
      }
    
    add("South", p2);
    
    addKeyListener(this);

    pack();
  }

  public void actionPerformed(ActionEvent e){    
    if(e.getSource() == itsSaveButton){
      itsToSave = true;
      itsNothingToDo = false;
      setVisible(false);
    }
    if(e.getSource() == itsCancelButton){
      itsToSave = false;
      itsNothingToDo = true;
      setVisible(false);
    }
    if(e.getSource() == itsNoButton){
      itsToSave = false;
      itsNothingToDo = false;
      setVisible(false);
    }
  }

  public boolean getToSaveFlag(){
    return itsToSave;
  }

  public boolean getNothingToDoFlag(){
    return itsNothingToDo;
  }
  
  //////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////keyListener--inizio
  public void keyTyped(KeyEvent e)
  {
    if (e.getKeyChar() == '\r')
      {
	itsToSave = true;
	setVisible(false);
      }
  }

  public void keyReleased(KeyEvent e){}
  
  public void keyPressed(KeyEvent e){}

  /////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////keyListener--fine
}








