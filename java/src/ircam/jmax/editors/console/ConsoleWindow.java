
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
    itsConsole.SetContainer(this);
  }
  
  public boolean OpenFile(File file){
    return true;
  }
  public MaxDocument GetDocument(){
    return itsConsoleDocument;
  }
  
  public void SetupMenu(){
    itsFileMenu.getItem(3).setEnabled(false);
    itsFileMenu.getItem(7).setEnabled(false);
    itsFileMenu.getItem(8).setEnabled(false);
    itsFileMenu.getItem(10).setEnabled(false);
    itsEditMenu.getItem(0).setEnabled(false);
    itsEditMenu.getItem(1).setEnabled(false);
    itsEditMenu.getItem(2).setEnabled(false);
    itsEditMenu.getItem(3).setEnabled(false);
  }
}





