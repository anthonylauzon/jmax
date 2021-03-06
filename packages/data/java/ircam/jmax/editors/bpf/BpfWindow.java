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
import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.menus.*;

import javax.swing.*;
import javax.swing.table.*;

/**
* This implementation builds a SequencePanel to represent the data.
 */
public class BpfWindow extends JMaxEditor {
  
  //------------------- fields
  BpfPanel itsBpfPanel;
  FtsBpfObject bpfData;
  
  public final static int DEFAULT_WIDTH  = 300;
  public final static int DEFAULT_HEIGHT = 150;
  public final static int MAX_HEIGHT     = 800;
  public final static int EMPTY_HEIGHT   = 78;

  public BpfWindow(FtsBpfObject data)
  {
    super();
    
    bpfData = data;
    
    itsBpfPanel = new BpfPanel(this, data);
    getContentPane().add(itsBpfPanel);
    
    makeBpfWindow();
    setSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
  }
  
  public BpfWindow(BpfWindow copyWin)
  {
    super();
    
    bpfData = copyWin.bpfData;
    itsBpfPanel = copyWin.itsBpfPanel;
    itsBpfPanel.setContainer(this);
    Rectangle bounds = copyWin.getBounds();
    copyWin.getContentPane().remove(itsBpfPanel);
    getContentPane().add(itsBpfPanel);
    
    makeBpfWindow();
    setBounds(bounds);
  }  
  
  private void makeBpfWindow()
  {
    makeTitle();
    makeListeners();
    
    if(JMaxApplication.getProperty("no_menus") == null)
      makeMenuBar();
    else
      makeSimpleMenuBar();
    
    validate();
  }
  
  private void makeListeners()
  {
    bpfData.addBpfListener( new BpfDataListener(){
      public void pointsDeleted(int index, int size){}
      public void pointAdded(int index){}
      public void pointChanged(int oldIndex, int newIndex, float newTime, float newValue){}
      public void pointsChanged(){}
      public void cleared(){}
      public void nameChanged( String name)
      {
        setWindowName(name);
      }
    });
    
    addWindowListener(new WindowListener(){
      public void windowOpened(WindowEvent e){}
      public void windowClosed(WindowEvent e){}
      public void windowClosing(WindowEvent e)
      {
        MaxWindowManager.getWindowManager().removeWindow(getFrame());
        itsBpfPanel.close(true);
      }
      public void windowDeiconified(WindowEvent e){}
      public void windowIconified(WindowEvent e){}
      public void windowActivated(WindowEvent e){}
      public void windowDeactivated(WindowEvent e){}
    });
  }  
  
  private final void makeTitle(){
    setWindowName( bpfData.getName());
  } 
  
  public void setWindowName(String name)
  {
    if(name != null && !name.equals(""))
      setTitle(MaxWindowManager.getWindowManager().makeUniqueWindowTitle("bpf " + name));
    else
      setTitle(MaxWindowManager.getWindowManager().makeUniqueWindowTitle("bpf #"+((FtsGraphicObject)bpfData).getObjectID()));

    MaxWindowManager.getWindowManager().windowChanged(this);
  }
  
  private final void makeMenuBar(){
    JMenuBar mb = new JMenuBar();
    
    EditorMenu fileMenu = new DefaultFileMenu();
    fileMenu.setEnabled( false, 3);
    fileMenu.setEnabled( false, 4);
    fileMenu.setEnabled( false, 7);
    mb.add( fileMenu);
    mb.add(new ircam.jmax.toolkit.menus.MaxWindowJMenu("Windows", this)); 
    
    setJMenuBar(mb);
  }
  // ------ JMaxEditor  ---------------
  public Editor getEditor(){
    return itsBpfPanel;
  }
  public Rectangle getViewRectangle(){
    return itsBpfPanel.getViewRectangle();
  }
}








