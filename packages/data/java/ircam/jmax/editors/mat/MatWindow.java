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
public class MatWindow extends JMaxEditor {
  
  //------------------- fields
  MatPanel itsMatPanel;
  MatDataModel matData;
  
  public MatWindow(MatDataModel data)
  {
    super();
    
    matData = data;
    
    makeTitle();
        
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
        validate();
        pack();
      }
      public void uploading(boolean uploading){}
      public void matNameChanged(String name)
      {
        setWindowName(name);
      }
      public void matSelectRow(int row_id){}
    });
    
    addWindowListener(new WindowListener(){
      public void windowOpened(WindowEvent e)
      {
        itsMatPanel.table.requestFocus();
      }
      public void windowClosed(WindowEvent e){}
      public void windowClosing(WindowEvent e)
      {
        MaxWindowManager.getWindowManager().removeWindow(getFrame());
        itsMatPanel.close(false);
      }
      public void windowDeiconified(WindowEvent e){}
      public void windowIconified(WindowEvent e){}
      public void windowActivated(WindowEvent e)
      {
        itsMatPanel.table.requestFocus();
      }
      public void windowDeactivated(WindowEvent e){}
    });
    
    if(JMaxApplication.getProperty("no_menus") == null)
      makeMenuBar();
    else
      makeSimpleMenuBar();
    
    setLocation(200, 200);
    setSize( matData.getDefaultSize());
    
    getContentPane().validate();
    itsMatPanel.validate();
    validate();
  }
  
  private final void makeTitle()
  {
    setWindowName(matData.getName());
  } 
  
  public void setWindowName(String name)
  {    
    String title;
    if(name != null && !name.equals(""))
      title = matData.getType() +"  "+name;
    else
      title = matData.getType() +" #"+((FtsGraphicObject)matData).getObjectID();
      
    if( !getTitle().equals(title))
    {
      setTitle( MaxWindowManager.getWindowManager().makeUniqueWindowTitle(title));    
      MaxWindowManager.getWindowManager().windowChanged(this);
    }
  }

  private final void makeMenuBar(){
    JMenuBar mb = new JMenuBar();
    
    EditorMenu fileMenu = new DefaultFileMenu();
    fileMenu.setEnabled( false, 3);
    fileMenu.setEnabled( false, 4);
    fileMenu.setEnabled( false, 7);
    mb.add( fileMenu);
    mb.add( new MatEditMenu( this));
    mb.add(new ircam.jmax.toolkit.menus.MaxWindowJMenu("Windows", this)); 
    
    setJMenuBar(mb);
  }

  EditorMenu simpleMenu = null;
  public EditorMenu getSimpleMenu()
  {
    if(simpleMenu == null)
      simpleMenu = new MatSimpleMenu(this, matData.getType());
    
    return simpleMenu; 
  }  
  
  // ------ JMaxEditor  ---------------
  public Editor getEditor(){
    return itsMatPanel;
  }
  public Rectangle getViewRectangle(){
    return null;
  }
}








