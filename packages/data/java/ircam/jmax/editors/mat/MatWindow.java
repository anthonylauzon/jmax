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

package ircam.jmax.editors.mat;

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
public class MatWindow extends JFrame implements EditorContainer{
  
  //------------------- fields
  MatPanel itsMatPanel;
  MatDataModel matData;
  
  public final static int DEFAULT_WIDTH  = 600;
  public final static int DEFAULT_HEIGHT = 350;
  public final static int MAX_HEIGHT     = 800;
  public final static int EMPTY_HEIGHT   = 78;
  
  public MatWindow(MatDataModel data)
  {
    super();
    
    matData = data;
    
    makeTitle();
    
    setSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
    
    itsMatPanel = new MatPanel(this, data);
    getContentPane().add(itsMatPanel);
    
    matData.addMatListener( new MatDataListener(){
      public void matCleared()
    {
        getContentPane().validate();
        itsMatPanel.validate();
        validate();
    }
      public void matDataChanged()
    {
        getContentPane().validate();
        itsMatPanel.validate();
        validate();
    }
      public void matSizeChanged(int n_rows, int n_cols)
    {
        getContentPane().validate();
        itsMatPanel.validate();
        validate();
    }   
      public void uploading(boolean uploading){}
    });
    
    addWindowListener(new WindowListener(){
      public void windowOpened(WindowEvent e){}
      public void windowClosed(WindowEvent e){}
      public void windowClosing(WindowEvent e)
      {
        MaxWindowManager.getWindowManager().removeWindow(getFrame());
      }
      public void windowDeiconified(WindowEvent e){}
      public void windowIconified(WindowEvent e){}
      public void windowActivated(WindowEvent e){}
      public void windowDeactivated(WindowEvent e){}
    });
    
    if(JMaxApplication.getProperty("no_menus") == null)
      makeMenuBar();
    
    setLocation(200, 200);
    setSize(new Dimension(DEFAULT_WIDTH, DEFAULT_HEIGHT));
    
    getContentPane().validate();
    itsMatPanel.validate();
    validate();
    pack();
  }
  
  private final void makeTitle(){
    setTitle(MaxWindowManager.getWindowManager().makeUniqueWindowTitle(matData.getType()));
    MaxWindowManager.getWindowManager().windowChanged(this);
  } 
  
  public void setName(String name)
  {
    setTitle(MaxWindowManager.getWindowManager().makeUniqueWindowTitle(matData.getType() +" : "+name));
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
  
  // ------ editorContainer interface ---------------
  public Editor getEditor(){
    return itsMatPanel;
  }
  public Frame getFrame(){
    return this;
  }
  public Point getContainerLocation(){
    return getLocation();
  }
  public Rectangle getViewRectangle(){
    return null;
  }
}








