package ircam.jmax.editors.console;

import ircam.jmax.*;
import ircam.jmax.dialogs.*;
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
    getContentPane().setLayout(gridbag);
    
    c.weightx = 1.0;
    c.weighty = 1.0;
    c.gridwidth = 1;
    c.gridheight = 1;
    c.fill = GridBagConstraints.BOTH;
    c.anchor = GridBagConstraints.NORTHWEST;
    gridbag.setConstraints(theConsole, c);
    getContentPane().add(itsConsole);
    itsConsole.SetContainer(this);
  }
  

  public MaxDocument GetDocument(){
    return itsConsoleDocument;
  }
  
  public void SetupMenu(){
    GetCloseMenu().setEnabled(false);
    GetSaveMenu().setEnabled(false);
    GetSaveAsMenu().setEnabled(false);
    GetPrintMenu().setEnabled(false);
    GetCutMenu().setEnabled(false);
    GetCopyMenu().setEnabled(false);
    GetPasteMenu().setEnabled(false);
    GetClearMenu().setEnabled(false);
  }

  public boolean CustomMenuActionPerformed(MenuItem theMenuItem, String theString){
    return true;
  }

  public boolean CustomMenuItemStateChanged(CheckboxMenuItem theMenuItem, String theString){
    return true;
  }
}






