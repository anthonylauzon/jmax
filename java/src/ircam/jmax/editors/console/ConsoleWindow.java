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

import java.io.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.datatransfer.*;
import javax.swing.*;

import ircam.jmax.script.*;

import ircam.jmax.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.mda.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.editors.console.menus.*;

/**
  Window containing a tcl console
  */
 
public class ConsoleWindow extends JFrame implements WindowListener, EditorContainer, KeyListener {
  StringBuffer itsSbuf = new StringBuffer();
  Console itsConsole;

  private FileMenu itsFileMenu;
  private EditMenu itsEditMenu;	
  private JMenu itsWindowsMenu;
  private JMenu itsToolsMenu;

  static private ConsoleWindow theConsoleWindow = null;

  static {
    MaxWindowManager.getWindowManager().addToolFinder( new MaxToolFinder() {
      public String getToolName() { return "Console";}
      public void open() { theConsoleWindow.toFront();}
    });
  }


  public ConsoleWindow() {
    super("jMax Console");

    MaxWindowManager.setTopFrame(this);

    setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);

    itsConsole = new Console(this, MaxApplication.getInterpreter());
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
    
    // Build The Menus and Menu Bar
    makeMenuBar();

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
  
  private final void makeMenuBar(){
    JMenuBar mb = new JMenuBar();
    
    // Build the file menu
    itsFileMenu = new FileMenu();
    mb.add( itsFileMenu); 
    
    // Build the edit menu
    itsEditMenu = new EditMenu(this); 
    mb.add( itsEditMenu); 
    
    // New Tool menu 
    itsToolsMenu = new ircam.jmax.toolkit.menus.MaxToolsJMenu("Tools"); 
    mb.add(itsToolsMenu);

    // New Window Manager based Menu
    itsWindowsMenu = new ircam.jmax.toolkit.menus.MaxWindowJMenu("Windows", this); 
    mb.add(itsWindowsMenu);

    setJMenuBar(mb);
  }

  // ------ editorContainer interface ---------------
  public Editor getEditor(){
    return itsConsole;
  }
  public Frame getFrame(){
    return this;
  }
  public Point getContainerLocation(){
    return getLocation();
  }
  public Rectangle getViewRectangle(){
    return getContentPane().getBounds();
  }
  // ----------------- WindowListener ---------------  
  public void windowClosing(WindowEvent e){
    MaxApplication.Quit();
  }
  public void windowOpened(WindowEvent e){}
  public void windowClosed(WindowEvent e){}
  public void windowIconified(WindowEvent e){}
  public void windowDeiconified(WindowEvent e){}
  public void windowActivated(WindowEvent e){
    requestFocus();
  }
  public void windowDeactivated(WindowEvent e){}
  ////////////////////////////////////////////////////////////WindowListener --fine
 ////////////////////////////////////////////////////////////////keyListener --inizio  
  public void keyTyped(KeyEvent e){}
  public void keyReleased(KeyEvent e){}
  public void keyPressed(KeyEvent e){}
  ///////////////////////////////////////////////////////////////// keyListener --fine
}











