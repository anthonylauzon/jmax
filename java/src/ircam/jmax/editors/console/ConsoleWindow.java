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
import javax.swing.event.*;

import ircam.jmax.script.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.menus.*;
import ircam.jmax.editors.console.menus.*;
import ircam.jmax.widgets.ConsoleArea;

/**
 * Window containing a console
 */
 
public class ConsoleWindow extends JFrame implements EditorContainer, Editor {

  static private ConsoleWindow consoleWindowSingleInstance = null;

  private ConsoleArea consoleArea;
  private boolean noConsole;
  private ConsoleDocument document;
  private ControlPanel controlPanel;
  private DefaultHelpMenu helpMenu;

  static {
    MaxWindowManager.getWindowManager().addToolFinder( new MaxToolFinder() {
	    public String getToolName() { return "Console";}
	    public void open() { consoleWindowSingleInstance.toFront();}
    });
  }

  public static void append( String line)
  {
    if ( consoleWindowSingleInstance.noConsole)
      System.out.println( line);
    else
      consoleWindowSingleInstance.consoleArea.append( line);
  }

  public static void init()
  {
    consoleWindowSingleInstance.getControlPanel().init(MaxApplication.getFts());
    consoleWindowSingleInstance.helpMenu.init();
  }

  public ConsoleWindow()
  {
    super("jMax Console");

    MaxWindowManager.setTopFrame( this);

    setDefaultCloseOperation( JFrame.DO_NOTHING_ON_CLOSE);

    consoleArea = new ConsoleArea( 1000, 80);

    noConsole = true;
    if ( (MaxApplication.getProperty("jmaxNoConsole") == null) || 
	 (MaxApplication.getProperty("jmaxNoConsole").equals("false")))
      {
	System.setOut( new PrintStream( new ConsoleOutputStream( consoleArea)));
	noConsole = false;
      }
    else
      consoleArea.append( "Output redirected to Java standard output");
    
    // Register this console window as *the* console window

    if ( consoleWindowSingleInstance == null)
      consoleWindowSingleInstance = this;
    
    makeMenuBar();

    getContentPane().setLayout(new BorderLayout());

    JScrollPane jsp = new JScrollPane( consoleArea);

    jsp.setVerticalScrollBarPolicy( JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
    //jsp.getViewport().setBackingStoreEnabled( true);

    getContentPane().add( BorderLayout.CENTER, jsp);


    JPanel toolbarPanel = new JPanel();
    toolbarPanel.setLayout(new BorderLayout());
    toolbarPanel.setPreferredSize(new Dimension(500, 23));

    JToolBar toolbar = new JToolBar("Control Panel", JToolBar.HORIZONTAL);
    toolbar.setPreferredSize(new Dimension(250, 23));
    toolbar.add(controlPanel = new ControlPanel(MaxApplication.getFts()));
    toolbar.addAncestorListener(new AncestorListener(){
	    public void ancestorAdded(AncestorEvent event)
	    {
		Frame frame = (Frame)SwingUtilities.getWindowAncestor(event.getAncestor());
		if(!(frame instanceof ConsoleWindow))
		    MaxWindowManager.getWindowManager().addWindow(frame);
	    }
	    public void ancestorRemoved(AncestorEvent event)
	    {
		Frame frame = (Frame)SwingUtilities.getWindowAncestor(event.getAncestor());
		if(!(frame instanceof ConsoleWindow))
		    MaxWindowManager.getWindowManager().removeWindow(frame);
	    }
	    public void ancestorMoved(AncestorEvent event){}
	});

    toolbarPanel.add( BorderLayout.NORTH, toolbar);
    getContentPane().add( BorderLayout.NORTH, toolbarPanel);

    setLocation(0,0);
    setSize( 500, 600);

    document = new ConsoleDocument(MaxApplication.getFts());

    setVisible( true);
  }
  
  private final void makeMenuBar()
  {
    JMenuBar mb = new JMenuBar();

    // File menu    
    mb.add( new FileMenu()); 

    // Edit menu
    //mb.add( new EditMenu( this));
    
    // Tool menu 
    //mb.add( new ircam.jmax.toolkit.menus.MaxToolsJMenu( "Tools"));

    // Windows Menu
    mb.add( new ircam.jmax.toolkit.menus.MaxWindowJMenu( "Windows", this));
    
    // Add some separation between help and the others.
    mb.add(Box.createHorizontalGlue());

    // Build up the help Menu 
    mb.add(helpMenu = new DefaultHelpMenu());
    
    setJMenuBar( mb);
  }

  public ControlPanel getControlPanel()
  {
    return controlPanel;
  }
    
  // Methods from interface Editor
  final public Fts getFts()
  {
    return MaxApplication.getFts();
  }

  public EditorContainer getEditorContainer()
  {
    return this;
  }

  public void Close(boolean doCancel)
  {
  }

  public MaxDocument getDocument(){
    return document;
  }

  // Methods from interface EditorContainer
  public Editor getEditor()
  {
    return this;
  }

  public Frame getFrame()
  {
    return this;
  }

  public Point getContainerLocation()
  {
    return getLocation();
  }

  public Rectangle getViewRectangle()
  {
    return getContentPane().getBounds();
  }
}

