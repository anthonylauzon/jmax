
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
    GridBagLayout gridbag = new GridBagLayout();
    GridBagConstraints c = new GridBagConstraints();
    setLayout(gridbag);
    
    c.weightx = 1.0;
    c.weighty = 1.0;
    c.gridwidth = 1;
    c.gridheight = 1;
    c.fill = GridBagConstraints.BOTH;
    c.anchor = GridBagConstraints.NORTHWEST;
    gridbag.setConstraints(theConsole, c);
    add(itsConsole);
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


