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

package ircam.jmax.editors.console;
import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.script.*;

import java.io.*;
import java.util.*;
import javax.swing.*;

import ircam.jmax.toolkit.*;
import java.awt.datatransfer.*;

/**
 A generic reusable panel containing a ConsoleThread
  */

import java.awt.*;
import javax.swing.*;
import java.awt.event.*;

public class Console extends JPanel implements Editor, ClipboardOwner, Transferable{
  StringBuffer itsSbuf = new StringBuffer();
  ConsoleTextArea itsTextArea;
  Interpreter itsInterp;
  PrintStream itsPrintStream; 
  ConsoleThread itsConsoleThread;
  ConsoleKeyListener itsKeyListener;
  KeyListener itsContainer;

  JScrollPane itsScrollerView;
  String itsCopiedText;

  public Console(EditorContainer container, Interpreter i) {
    StringBuffer itsSbuf = new StringBuffer();
    itsTextArea = new ConsoleTextArea(40, 40);

    itsScrollerView = new JScrollPane();
    itsScrollerView.setViewportView(itsTextArea); 
    itsScrollerView.getHorizontalScrollBar().setUnitIncrement( 10);
    itsScrollerView.getVerticalScrollBar().setUnitIncrement( 10);
    itsScrollerView.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
    itsScrollerView.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED);

    itsEditorContainer = container;

    itsInterp = i;

    setLayout(new BorderLayout());
    add("Center", itsScrollerView);

    itsKeyListener = new ConsoleKeyListener(this);
    itsTextArea.addKeyListener(itsKeyListener);
    itsTextArea.addMouseListener(new ConsoleMouseListener(this));

    itsConsoleThread = new ConsoleThread(this);
    /*
     * The console thread runs as a daemon so that it gets terminated 
     * automatically when all other user threads are terminated.
     */
    itsPrintStream = new PrintStream(new ConsoleWriter(this));
  }

  public void Start(){
    if (MaxApplication.getProperty("jmaxNoConsoleThread") == null)
      itsConsoleThread.start();
  }

  public void SetContainer(KeyListener theContainer){
    itsContainer = theContainer;
    itsTextArea.addKeyListener(itsContainer);
  }

  public PrintStream getPrintStream() {
    return itsPrintStream;
  }

  public ConsoleTextArea getTextArea() {
    return itsTextArea;
  }
  
  /**
   * Utility function that allows the insertion of text
   * in such a way that the next carriage return will EXECUTE it.
   * Example: copy and paste.
   */
  public void PutInKeyboardBuffer(String s) {
    itsKeyListener.sbuf.append(s);
    Put(s);
  }

  /**
   * Utility function that allows the insertion of text
   * in such a way that the next carriage return will NOT execute it.
   * (Example: system messages)
   */
  public void PutLine(String s) {
    
    itsTextArea.append(s + "\n");
    itsKeyListener.intercept+=s.length();
  }
  
  public void Put(String s) {
    itsTextArea.append(s);
    itsKeyListener.intercept+=s.length();
  }
    
  /*
   * The ConsoleKeyListener object tells the console thread that a 
   * line of input is available and run() can proceed.
   */
  
  public synchronized void LineReadyNotify(String s) {
    itsSbuf.append(s);
    //itsConsoleThread.notify();
    itsConsoleThread.resume();
  }

  /*
   * If itsSbuf contains a complete command, evaluate it and display the
   * result in the text box.  Otherwise, display the secondary prompt.
   */
  
  void ProcessLine() {
    String s = itsSbuf.toString();
    
    if (itsInterp.commandComplete(s)) {
      
      try {
	Object obj = itsInterp.eval(s);

	// Ask FTS to recompute the error objects if needed

	String result = obj.toString();
	if (result.length() > 0) {
	  PutLine(result);
	}
      } catch (ScriptException e) {

	  /*
	if (e.getCompletionCode() == TCL.RETURN) {
	  PutLine(itsInterp.getResult().toString());
	}
	else if (e.getCompletionCode() == TCL.ERROR) {
	  PutLine(itsInterp.getResult().toString());
	} else {
	  PutLine("command returned bad code: " + e.getCompletionCode());
	}
	  */

	  PutLine("Command returned bad error: " + e.getMessage());

      }
      itsSbuf.setLength(0);
      Put("% ");
    } else {
      Put("> ");
    }
  }
  
  //------------------- Editor interface ---------------
  final public Fts getFts()
  {
    return MaxApplication.getFts();
  }

  EditorContainer itsEditorContainer;

  public EditorContainer getEditorContainer(){
    return itsEditorContainer;
  }
  public void Close(boolean doCancel){}
  //-------------------------------------------

  // ----------ClipboardOwner interface methods
  public void lostOwnership(Clipboard clipboard, Transferable contents) {}

  //-----------Tranferable interface methods-------------
  public Object getTransferData(DataFlavor flavor) throws UnsupportedFlavorException, IOException {
    return itsCopiedText;
  }

  public DataFlavor[] getTransferDataFlavors() {
    DataFlavor flavorList[] = new DataFlavor[1];
    flavorList[0] = DataFlavor.plainTextFlavor;
    return (flavorList);
  }

  public boolean isDataFlavorSupported(DataFlavor flavor) {
    return true;
  }
  //end

  public void Copy() {
    if (! itsTextArea.getSelectedText().equals(""))
      {
	MaxApplication.systemClipboard.setContents(this, this);
	itsCopiedText = itsTextArea.getSelectedText();
      }
  }

  public void Paste(){
    String aPastingString = new String();
    Transferable aTransferable = MaxApplication.systemClipboard.getContents(this);
    
    try {
      aPastingString = (String) aTransferable.getTransferData(DataFlavor.plainTextFlavor);
    } catch (Exception e) {}
    
    PutInKeyboardBuffer(aPastingString);
  } 
}






