
package ircam.jmax.editors.console;
import ircam.jmax.*;
import java.io.*;
import java.util.*;
import com.sun.java.swing.*;
/**
 A generic reusable panel containing a ConsoleThread
  */

import java.awt.*;
import java.awt.event.*;
import tcl.lang.*;

public class Console extends JPanel{
  StringBuffer itsSbuf = new StringBuffer();
  TextArea itsTextArea;
  Interp itsInterp;
  PrintStream itsPrintStream; 
  ConsoleThread itsConsoleThread;
  ConsoleKeyListener itsKeyListener;
  KeyListener itsContainer;

  public Console(Interp i) {
    StringBuffer itsSbuf = new StringBuffer();
    itsTextArea = new TextArea(40, 40);
    itsInterp = i;

    setLayout(new BorderLayout());
    add("Center", itsTextArea);

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
    itsConsoleThread.start();
  }

  public void SetContainer(KeyListener theContainer){
    itsContainer = theContainer;
    itsTextArea.addKeyListener(itsContainer);
  }

  public PrintStream getPrintStream() {
    return itsPrintStream;
  }

  public TextArea getTextArea() {
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
    //e.m.text.insert(s + "\n", 100000);
    itsTextArea.append(s + "\n");
    itsKeyListener.intercept+=s.length();
  }
  
  public void Put(String s) {
    itsTextArea.append(s);
    itsKeyListener.intercept+=s.length();
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

	if (e.getCompletionCode() == TCL.RETURN) {
	  PutLine(itsInterp.getResult().toString());
	}
	else if (e.getCompletionCode() == TCL.ERROR) {
	  PutLine(itsInterp.getResult().toString());
	} else {
	  PutLine("command returned bad code: " 
+ e.getCompletionCode());
	}
      }
      itsSbuf.setLength(0);
      Put("% ");
    } else {
      Put("> ");
    }
  }
}






