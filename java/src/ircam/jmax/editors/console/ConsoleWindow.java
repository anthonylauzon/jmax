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

import java.io.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.datatransfer.*;
import javax.swing.*;

import tcl.lang.*;

import ircam.jmax.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.mda.*;
import ircam.jmax.toolkit.*;

/**
  Window containing a tcl console
  */


 
public class ConsoleWindow extends MaxEditor implements ClipboardOwner, Transferable{
  StringBuffer itsSbuf = new StringBuffer();
  Console itsConsole;
  String itsCopiedText;

  static private ConsoleWindow theConsoleWindow = null;

  static {
    MaxWindowManager.getWindowManager().addToolFinder( new MaxToolFinder() {
      public String getToolName() { return "Console";}
      public void open() { theConsoleWindow.toFront();}
    });
  }


  public ConsoleWindow() {
    super("jMax Console", false);

    MaxWindowManager.setTopFrame(this);

    itsConsole = new Console(MaxApplication.getTclInterp());
    itsConsole.Start();

    if ( (MaxApplication.getProperty("jmaxNoConsole") == null) || 
	(MaxApplication.getProperty("jmaxNoConsole").equals("false")))
      {
	  System.setOut(itsConsole.getPrintStream());
      }
    else
	{
	    itsConsole.getTextArea().setRows( 10);
	}

    getContentPane().setLayout(new BorderLayout());

    getContentPane().add("Center", itsConsole);
    itsConsole.SetContainer(this);

    // Register this console window 
    // as *the* console window

    if (theConsoleWindow == null)
      theConsoleWindow = this;

    Init();

    validate();

    setLocation(0,0);
    pack();
    setVisible(true);

    if ((MaxApplication.getProperty("jmaxNoConsole") != null) &&
	(MaxApplication.getProperty("jmaxNoConsole").equals("true")))
	{
	    itsConsole.getPrintStream().println( "Output redirected to Java standard output");
	}

    itsConsole.getTextArea().setCaretPosition(itsConsole.getTextArea().getText().length());
  }
  

  public void SetupMenu(){
    getCloseMenu().setEnabled(false);
    getSaveMenu().setEnabled(false);
    getSaveAsMenu().setEnabled(false);
    getPrintMenu().setEnabled(true);
    getCutMenu().setEnabled(false);
    getCopyMenu().setEnabled(true);//clipboard test
    getPasteMenu().setEnabled(true);
    getDuplicateMenu().setEnabled(true);
  }

  // start of ClipboardOwner, Tranferable interface methods
  public void lostOwnership(Clipboard clipboard, Transferable contents) {
    //nun me ne po' frega' de meno
  }
  //--
  public Object getTransferData(DataFlavor flavor) throws UnsupportedFlavorException, IOException {
    return itsCopiedText;
  }

  public DataFlavor[] getTransferDataFlavors() {
    DataFlavor flavorList[] = new DataFlavor[1];
    flavorList[0] = DataFlavor.plainTextFlavor;
    //    System.err.println("quali flavor supporto? io dico plainText "+DataFlavor.plainTextFlavor.toString());
    return (flavorList);
  }

  public boolean isDataFlavorSupported(DataFlavor flavor) {
    //System.err.println("is supported "+flavor.toString()+"? io dico si");
    return true;
  }
  //end

  protected void Copy() {
    if (! itsConsole.itsTextArea.getSelectedText().equals(""))
      {
	MaxApplication.systemClipboard.setContents(this, this);
	itsCopiedText = itsConsole.itsTextArea.getSelectedText();//tout simplement
      }
  }

  protected void Paste(){
    String aPastingString = new String();
    Transferable aTransferable = MaxApplication.systemClipboard.getContents(this);

    try {
      aPastingString = (String) aTransferable.getTransferData(DataFlavor.plainTextFlavor);
    } catch (Exception e) {
    }

    itsConsole.PutInKeyboardBuffer(aPastingString);
  }

  //redefini parce-que la console ne doit pas se fermer sans quitter
  public void Close(){}

  public void Print(){
    PrintJob aPrintjob = getToolkit().getPrintJob(this, "Printing Console", MaxApplication.getProperties());
    if(aPrintjob != null){
      Graphics aPrintGraphics = aPrintjob.getGraphics();
      if(aPrintGraphics != null){
	//aPrintGraphics.setClip(0, 0, 400, 400);
	printAll(aPrintGraphics);
	aPrintGraphics.dispose();
      }
      aPrintjob.end();
    }
  }
}






