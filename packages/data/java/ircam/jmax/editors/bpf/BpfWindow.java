//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.jmax.editors.bpf;

import java.io.*;
import java.lang.*;
import java.awt.event.*;
import java.awt.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.menus.*;

import javax.swing.*;
import javax.swing.table.*;

/**
 * This implementation builds a SequencePanel to represent the data.
 */
public class BpfWindow extends JFrame implements EditorContainer{

    //------------------- fields
    BpfPanel itsBpfPanel;
    FtsBpfObject bpfData;

    public final static int DEFAULT_WIDTH  = 300;
    public final static int DEFAULT_HEIGHT = 150;
    public final static int MAX_HEIGHT     = 800;
    public final static int EMPTY_HEIGHT   = 78;
  /**
   * Constructor with FtsSequenceObject
   */
    public BpfWindow(FtsBpfObject data)
    {
      super();
      
      MaxWindowManager.getWindowManager().addWindow(this);
      
      bpfData = data;
      
      makeTitle();
      
      // Build The Menus and Menu Bar
      makeMenuBar();
      
      setSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
      
      //... then the SequencePanel
      itsBpfPanel = new BpfPanel(this, data);
      getContentPane().add(itsBpfPanel);
      
      addWindowListener(new WindowListener(){
	  public void windowOpened(WindowEvent e){}
	  public void windowClosed(WindowEvent e){}
	  public void windowClosing(WindowEvent e)
	  {
	    MaxWindowManager.getWindowManager().removeWindow(getFrame());
	  }
	  public void windowDeiconified(WindowEvent e){}
	  public void windowIconified(WindowEvent e){}
	  public void windowActivated(WindowEvent e)
	  {
	  }
	  public void windowDeactivated(WindowEvent e)
	  {
	  }
	});
      
      validate();
      pack();
      setVisible(true);
    }
  
  private final void makeTitle(){
    setTitle(MaxWindowManager.getWindowManager().makeUniqueWindowTitle("Bpf"));
    MaxWindowManager.getWindowManager().windowChanged(this);
  } 
  
  public void setName(String name)
  {
    setTitle(MaxWindowManager.getWindowManager().makeUniqueWindowTitle("Bpf " + name));
    MaxWindowManager.getWindowManager().windowChanged(this);
  }
  
  private final void makeMenuBar(){
    JMenuBar mb = new JMenuBar();
    
    // Build the file menu	
    EditorMenu fileMenu = new DefaultFileMenu();
    fileMenu.setEnabled( false, 3);
    fileMenu.setEnabled( false, 4);
    fileMenu.setEnabled( false, 7);
    mb.add( fileMenu);
    
    // New Window Manager based Menu
    mb.add(new ircam.jmax.toolkit.menus.MaxWindowJMenu("Windows", this)); 
    
    setJMenuBar(mb);
  }
    
  // ------ editorContainer interface ---------------
  public Editor getEditor(){
    return itsBpfPanel;
  }
  public Frame getFrame(){
    return this;
  }
  public Point getContainerLocation(){
    return getLocation();
  }
  public Rectangle getViewRectangle(){
    return itsBpfPanel.getViewRectangle();
  }
}








