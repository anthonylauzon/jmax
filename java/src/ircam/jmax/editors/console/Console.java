
package ircam.jmax.editors.console;
import ircam.jmax.*;
import java.io.*;
import java.util.*;
/**
 A generic reusable panel containing a ConsoleThread
  */

import java.awt.*;
import java.awt.event.*;
import tcl.lang.*;

public class Console extends Panel{
  StringBuffer itsSbuf = new StringBuffer();
  TextArea itsTextArea;
  Interp itsInterp;
  PrintWriter itsPrintWriter;
  ConsoleThread itsConsoleThread;

  public Console(Interp i) {
    StringBuffer sSbuf = new StringBuffer();
    itsTextArea = new TextArea(40, 40);
    itsInterp = i;

    GridBagLayout gridbag = new GridBagLayout();
    GridBagConstraints c = new GridBagConstraints();
    setLayout(gridbag);
    c.weightx = 1.0;
    c.weighty = 1.0;
    c.gridwidth = 1;
    c.gridheight = 1;
    c.fill = GridBagConstraints.BOTH;
    c.anchor = GridBagConstraints.NORTHWEST;
    gridbag.setConstraints(itsTextArea, c);    
    add(itsTextArea);

    itsTextArea.addKeyListener(new ConsoleKeyListener(this));
    itsConsoleThread = new ConsoleThread(this);
    /*
     * The console thread runs as a daemon so that it gets terminated 
     * automatically when all other user threads are terminated.
     */
    itsPrintWriter = new PrintWriter(new ConsoleWriter(this));
  }

  public void Start(){
    itsConsoleThread.start();
  }

  public PrintWriter getPrintWriter() {
    return itsPrintWriter;
  }

  public TextArea getTextArea() {
    return itsTextArea;
  }
  
  public void PutLine(String s) {
    //e.m.text.insert(s + "\n", 100000);
    itsTextArea.append(s + "\n");
  }
  
  public void Put(String s) {
    itsTextArea.append(s);
    //e.m.text.insert(s, 100000);
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
	itsInterp.eval(s);
	String result = itsInterp.getResult().toString();
	if (result.length() > 0) {
	  PutLine(result);
	}
      } catch (TclException e) {
	
	if (e.compCode == TCL.OK) {
	  PutLine(itsInterp.getResult().toString());
	} else if (e.compCode == TCL.ERROR) {
	  PutLine(itsInterp.getResult().toString());
	} else {
	  PutLine("command returned bad code: " 
+ e.compCode);
	}
      }
      itsSbuf.setLength(0);
      Put("% ");
    } else {
      Put("> ");
    }
  }
}



