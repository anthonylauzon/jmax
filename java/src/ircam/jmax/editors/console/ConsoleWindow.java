
package ircam.jmax.editors.console;
import ircam.jmax.*;
import java.io.*;
import java.util.*;
/**
  Window containing a tcl console
  */

import java.awt.*;
import java.awt.event.*;
import tcl.lang.*;

public class ConsoleWindow extends MaxEditor {
  StringBuffer itsSbuf = new StringBuffer();
  Console itsConsole;
  ConsoleDocument itsConsoleDocument = new ConsoleDocument(this);

  public ConsoleWindow(Console theConsole, String theTitle) {
    super(theTitle);
    itsConsole = theConsole;
    //setLayout(new GridBagLayout());
    GridBagLayout gridbag = new GridBagLayout();
    GridBagConstraints c = new GridBagConstraints();
    setLayout(gridbag);
    c.fill = GridBagConstraints.BOTH;
    c.anchor = GridBagConstraints.NORTHEAST;
    gridbag.setConstraints(theConsole, c);
    add(itsConsole);
    //validate();
    //setTitle(theTitle);
    //setVisible(true);
  }
  
  public PrintWriter getPrintWriter() {
    return itsConsole.getPrintWriter();
  }

  public boolean OpenFile(String thename, String thePath){
    return true;
  }
  public MaxDocument GetDocument(){
    return itsConsoleDocument;
  }

}


